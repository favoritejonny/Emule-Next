// eMule Next - GPL-2.0-or-later
#include "stdafx.h"
#include "UPnPImplPcpNatPmp.h"
#include "UPnPImplWrapper.h"
#include "ConnectionSetup.h"
#include "Preferences.h"
#include "emule.h"
#include "EmuleDlg.h"
#include "opcodes.h"
#include "UserMsgs.h"
#include "Resource.h"
#include "Log.h"
#include <iphlpapi.h>
#include <bcrypt.h>
#include <chrono>
#include <algorithm>

namespace {
struct SocketHandle {
	SOCKET value;
	~SocketHandle() { if (value != INVALID_SOCKET) closesocket(value); }
};
struct ComScope {
	HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	~ComScope() { if (SUCCEEDED(result)) CoUninitialize(); }
};
}

uint64_t CUPnPImplPcpNatPmp::Now()
{
	return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch()).count());
}

bool CUPnPImplPcpNatPmp::FindRoute(Route& route)
{
	// A route lookup does not send a packet to this address. Require a direct
	// home gateway on a physical IPv4 adapter; do not guess a VPN/PCP server.
	MIB_IPFORWARDROW best = {};
	if (GetBestRoute(htonl(0x01010101), 0, &best) != NO_ERROR || best.dwForwardMask != 0
		|| !PortMapping::IsPrivateAddress(ntohl(best.dwForwardNextHop))) return false;
	ULONG size = 16 * 1024;
	std::vector<unsigned char> buffer(size);
	ULONG result = GetAdaptersInfo(reinterpret_cast<IP_ADAPTER_INFO*>(buffer.data()), &size);
	if (result == ERROR_BUFFER_OVERFLOW && size <= 1024 * 1024) {
		buffer.resize(size);
		result = GetAdaptersInfo(reinterpret_cast<IP_ADAPTER_INFO*>(buffer.data()), &size);
	}
	if (result != NO_ERROR) return false;
	for (const IP_ADAPTER_INFO* adapter = reinterpret_cast<const IP_ADAPTER_INFO*>(buffer.data()); adapter; adapter = adapter->Next) {
		if (adapter->Index != best.dwForwardIfIndex || (adapter->Type != MIB_IF_TYPE_ETHERNET && adapter->Type != IF_TYPE_IEEE80211)) continue;
		for (const IP_ADDR_STRING* ip = &adapter->IpAddressList; ip; ip = ip->Next) {
			const DWORD address = inet_addr(ip->IpAddress.String);
			const DWORD mask = inet_addr(ip->IpMask.String);
			if (!PortMapping::IsPrivateAddress(ntohl(address)) || mask == 0 || mask == INADDR_NONE
				|| address == best.dwForwardNextHop || (address & mask) != (best.dwForwardNextHop & mask)) continue;
			route.address = address;
			route.gateway = best.dwForwardNextHop;
			route.index = best.dwForwardIfIndex;
			MIB_IPFORWARDROW local = {};
			return GetBestRoute(route.gateway, route.address, &local) == NO_ERROR
				&& local.dwForwardIfIndex == route.index && local.dwForwardNextHop == 0;
		}
	}
	return false;
}

bool CUPnPImplPcpNatPmp::SameRoute(const Route& route)
{
	Route current;
	return FindRoute(current) && current.address == route.address
		&& current.gateway == route.gateway && current.index == route.index;
}

SOCKET CUPnPImplPcpNatPmp::OpenSocket() const
{
	SOCKET socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket == INVALID_SOCKET) return socket;
	sockaddr_in local = {}, gateway = {};
	local.sin_family = gateway.sin_family = AF_INET;
	local.sin_addr.s_addr = m_route.address;
	gateway.sin_addr.s_addr = m_route.gateway;
	gateway.sin_port = htons(5351);
	const int ttl = 1;
	const BOOL exclusive = TRUE;
	u_long nonblocking = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast<const char*>(&exclusive), sizeof(exclusive)) != 0
		|| setsockopt(socket, IPPROTO_IP, IP_TTL, reinterpret_cast<const char*>(&ttl), sizeof(ttl)) != 0
		|| bind(socket, reinterpret_cast<const sockaddr*>(&local), sizeof(local)) != 0
		|| connect(socket, reinterpret_cast<const sockaddr*>(&gateway), sizeof(gateway)) != 0
		|| ioctlsocket(socket, FIONBIO, &nonblocking) != 0) {
		closesocket(socket);
		return INVALID_SOCKET;
	}
	// A connected UDP socket only accepts replies from this gateway:5351.
	return socket;
}

PortMapping::Reply CUPnPImplPcpNatPmp::Exchange(SOCKET socket, const PortMapping::Request& request)
{
	using namespace PortMapping;
	using PortMapping::Status;
	Reply reply;
	const Packet packet = Encode(request);
	if (packet.empty()) { reply.status = Status::Failed; return reply; }
	const unsigned attempts = request.protocol == Protocol::PCP ? 2 : 4;
	for (unsigned attempt = 0; attempt < attempts && !m_cancel.load(); ++attempt) {
		if (GetAutomaticNetworkRestriction() != 0 || !SameRoute(m_route)) {
			reply.status = Status::NetworkChanged;
			return reply;
		}
		const int sent = send(socket, reinterpret_cast<const char*>(packet.data()), static_cast<int>(packet.size()), 0);
		if (sent != static_cast<int>(packet.size())) { reply.status = Status::Failed; return reply; }
		const unsigned baseDelay = request.protocol == Protocol::PCP ? 3000 : 250;
		const unsigned jitterPercent = request.protocol == Protocol::PCP ? 90 + request.nonce[attempt] % 21 : 100;
		const uint64_t until = Now() + (uint64_t(baseDelay) << attempt) * jitterPercent / 100;
		while (!m_cancel.load() && Now() < until) {
			fd_set readSet;
			FD_ZERO(&readSet);
			FD_SET(socket, &readSet);
			timeval wait = {0, 100000};
			const int ready = select(0, &readSet, nullptr, nullptr, &wait);
			if (ready == SOCKET_ERROR) { reply.status = Status::Failed; return reply; }
			if (ready == 0) continue;
			unsigned char response[1100];
			const int length = recv(socket, reinterpret_cast<char*>(response), sizeof(response), 0);
			if (length == SOCKET_ERROR) {
				const int error = WSAGetLastError();
				if (error == WSAECONNRESET || error == WSAECONNREFUSED) { reply.status = Status::Timeout; return reply; }
				if (error == WSAEWOULDBLOCK || error == WSAEMSGSIZE) continue;
				reply.status = Status::Failed;
				return reply;
			}
			reply = Decode(request, response, static_cast<size_t>(length));
			if (reply.status != Status::Ignore) {
				if (GetAutomaticNetworkRestriction() != 0 || !SameRoute(m_route)) reply.status = Status::NetworkChanged;
				return reply;
			}
		}
	}
	reply.status = m_cancel.load() || GetAutomaticNetworkRestriction() != 0 || !SameRoute(m_route)
		? Status::NetworkChanged : Status::Timeout;
	return reply;
}

UINT CUPnPImplPcpNatPmp::Diagnostic(PortMapping::Status status)
{
	using PortMapping::Status;
	switch (status) {
	case Status::Success: return 0;
	case Status::WrongPort: return IDS_PORTMAP_WRONGPORT;
	case Status::NonPublicAddress: return IDS_CONNSETUP_SHARED;
	case Status::Refused: return IDS_PORTMAP_REFUSED;
	case Status::NetworkChanged: return IDS_PORTMAP_NETWORKCHANGED;
	case Status::ShortLease: return IDS_PORTMAP_SHORTLEASE;
	default: return IDS_CONNSETUP_FAILED;
	}
}

LPCTSTR CUPnPImplPcpNatPmp::GetProtocolName() const
{
	return m_session.protocol == PortMapping::Protocol::PCP ? _T("PCP") : _T("NAT-PMP");
}

void CUPnPImplPcpNatPmp::Work() noexcept
{
	ComScope com;
	PortMapping::Status status = PortMapping::Status::Failed;
	const uint64_t started = Now();
	try {
		SocketHandle socket{OpenSocket()};
		if (socket.value != INVALID_SOCKET && SUCCEEDED(com.result))
			status = PortMapping::Run(m_session, [this, &socket](const PortMapping::Request& request) { return Exchange(socket.value, request); });
		if (status == PortMapping::Status::Success) {
			// Count conservatively from before the requests, not from receipt of
			// the last reply: TCP may have been mapped before UDP.
			m_renewAt = started + uint64_t(m_session.renewAfterSeconds) * 1000;
			m_expiresAt = started + uint64_t(m_session.renewAfterSeconds) * 2000;
		} else {
			// Stop renewal after failure. Partial NAT-PMP leases expire naturally;
			// PCP cleanup is nonce-bound and never removes another client's rule.
			RemoveOwnedPcpMappings();
			m_session.established = false;
			m_renewAt = m_expiresAt = 0;
		}
		m_allowFallback = m_session.allowUPnPFallback;
	} catch (...) {
		status = PortMapping::Status::Failed;
		m_allowFallback = false;
		m_session.established = false;
	}
	m_diagnosticMessageID = Diagnostic(status);
	m_bUPnPPortsForwarded = status == PortMapping::Status::Success ? TRIS_TRUE : TRIS_FALSE;
	DebugLog(_T("Port mapping (%s): result %u"), GetProtocolName(), static_cast<unsigned>(status));
	if (!m_cancel.load()) SendResultMessage();
	m_running = false;
}

void CUPnPImplPcpNatPmp::Launch(bool refresh)
{
	if (m_worker.joinable()) m_worker.join();
	m_cancel = false;
	m_running = true;
	m_bCheckAndRefresh = refresh;
	m_bUPnPPortsForwarded = TRIS_UNKNOWN;
	m_diagnosticMessageID = 0;
	try { m_worker = std::thread(&CUPnPImplPcpNatPmp::Work, this); }
	catch (...) { m_running = false; throw UPnPError(); }
}

void CUPnPImplPcpNatPmp::StartDiscovery(uint16 tcp, uint16 udp, uint16 web)
{
	if (!IsReady()) throw UPnPError();
	if (m_worker.joinable()) m_worker.join();
	m_allowFallback = false;
	m_bCheckAndRefresh = false;
	m_bUPnPPortsForwarded = TRIS_FALSE;
	m_diagnosticMessageID = 0;
	const UINT restriction = GetAutomaticSetupRestriction();
	if (!thePrefs.IsUPnPEnabled() || !thePrefs.IsUPnPHomeOnly() || restriction != 0 || tcp == 0 || web != 0) {
		m_diagnosticMessageID = restriction != 0 ? restriction : IDS_CONNSETUP_FAILED;
		SendResultMessage();
		return;
	}
	RemoveOwnedPcpMappings();
	m_session = PortMapping::Session{};
	if (!FindRoute(m_route)) {
		m_diagnosticMessageID = IDS_CONNSETUP_UNKNOWN;
		SendResultMessage();
		return;
	}
	m_session.localAddress = ntohl(m_route.address);
	m_nTCPPort = tcp;
	m_nUDPPort = udp;
	m_nTCPWebPort = 0;
	for (unsigned i = 0; i < m_session.leases.size(); ++i) {
		auto& request = m_session.leases[i].request;
		request.transport = i == 0 ? 6 : 17;
		request.internalPort = request.externalPort = i == 0 ? tcp : udp;
		if (BCryptGenRandom(nullptr, request.nonce.data(), static_cast<ULONG>(request.nonce.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
			m_diagnosticMessageID = IDS_CONNSETUP_FAILED;
			SendResultMessage();
			return;
		}
	}
	if (m_leaseTimer == 0) m_leaseTimer = ::SetTimer(nullptr, 0, 5000, LeaseTimer);
	if (m_leaseTimer == 0) throw UPnPError();
	m_bUPnPPortsForwarded = TRIS_UNKNOWN;
	Launch(false);
}

bool CUPnPImplPcpNatPmp::CheckAndRefresh()
{
	if (!IsReady() || !m_session.established || !thePrefs.IsUPnPEnabled() || !thePrefs.IsUPnPHomeOnly()) return false;
	if (GetAutomaticSetupRestriction() != 0 || !SameRoute(m_route)
		|| thePrefs.GetPort() != m_nTCPPort || thePrefs.GetUDPPort() != m_nUDPPort) {
		m_bUPnPPortsForwarded = TRIS_FALSE;
		m_diagnosticMessageID = IDS_PORTMAP_NETWORKCHANGED;
		return false;
	}
	Launch(true);
	return true;
}

void CUPnPImplPcpNatPmp::MaintainLeases()
{
	if (!thePrefs.IsUPnPEnabled() || !thePrefs.IsUPnPHomeOnly()) {
		StopAsyncFind();
		DeletePorts();
		return;
	}
	if (!IsReady() || !m_session.established) return;
	const uint64_t now = Now();
	if (now >= m_expiresAt) m_bUPnPPortsForwarded = TRIS_FALSE;
	if (now < m_renewAt) return;
	m_renewAt = now + 30000; // bounded retry of local eligibility checks
	SetMessageOnResult(theApp.emuledlg, UM_UPNP_RESULT);
	if (!CheckAndRefresh()) SetMessageOnResult(nullptr, 0);
}

void CALLBACK CUPnPImplPcpNatPmp::LeaseTimer(HWND, UINT, UINT_PTR, DWORD) noexcept
{
	try {
		if (theApp.IsRunning() && theApp.m_pUPnPFinder != nullptr)
			theApp.m_pUPnPFinder->GetImplementation()->MaintainLeases();
	} catch (...) { DebugLogWarning(_T("Port-mapping lease maintenance failed")); }
}

void CUPnPImplPcpNatPmp::RemoveOwnedPcpMappings()
{
	if (m_session.protocol != PortMapping::Protocol::PCP) return;
	bool any = false;
	for (const auto& lease : m_session.leases) any = any || lease.issued;
	if (!any || GetAutomaticNetworkRestriction() != 0 || !SameRoute(m_route)) return;
	SocketHandle socket{OpenSocket()};
	if (socket.value == INVALID_SOCKET) return;
	for (auto& lease : m_session.leases) {
		if (!lease.issued) continue;
		auto request = lease.request;
		request.operation = PortMapping::Operation::Delete;
		const auto packet = PortMapping::Encode(request);
		if (!packet.empty()) send(socket.value, reinterpret_cast<const char*>(packet.data()), static_cast<int>(packet.size()), 0);
		lease.issued = false;
	}
}

void CUPnPImplPcpNatPmp::StopAsyncFind()
{
	m_cancel = true;
	if (m_worker.joinable()) m_worker.join();
	SetMessageOnResult(nullptr, 0);
	if (m_leaseTimer != 0) { ::KillTimer(nullptr, m_leaseTimer); m_leaseTimer = 0; }
}

void CUPnPImplPcpNatPmp::DeletePorts()
{
	StopAsyncFind();
	RemoveOwnedPcpMappings();
	m_session = PortMapping::Session{};
	m_nTCPPort = m_nUDPPort = m_nTCPWebPort = 0;
	m_renewAt = m_expiresAt = 0;
	m_bUPnPPortsForwarded = TRIS_FALSE;
}

CUPnPImplPcpNatPmp::~CUPnPImplPcpNatPmp() { StopAsyncFind(); }

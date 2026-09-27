// eMule Next - GPL-2.0-or-later
#pragma once
#include "UPnPImpl.h"
#include "PortMappingProtocol.h"
#include <thread>

class CUPnPImplPcpNatPmp : public CUPnPImpl
{
public:
	~CUPnPImplPcpNatPmp() override;
	void StartDiscovery(uint16 tcp, uint16 udp, uint16 web) override;
	bool CheckAndRefresh() override;
	void StopAsyncFind() override;
	void DeletePorts() override;
	bool IsReady() override { return !m_running.load(); }
	int GetImplementationID() override { return UPNP_IMPL_PCP_NATPMP; }
	bool CanFallbackToUPnP() const override { return m_allowFallback.load(); }
	void MaintainLeases() override;
	LPCTSTR GetProtocolName() const override;

private:
	struct Route { DWORD address = 0; DWORD gateway = 0; DWORD index = 0; };
	static bool FindRoute(Route& route);
	static bool SameRoute(const Route& route);
	static UINT Diagnostic(PortMapping::Status status);
	static void CALLBACK LeaseTimer(HWND, UINT, UINT_PTR, DWORD) noexcept;
	static uint64_t Now();
	SOCKET OpenSocket() const;
	void Launch(bool refresh);
	void Work() noexcept;
	PortMapping::Reply Exchange(SOCKET socket, const PortMapping::Request& request);
	void RemoveOwnedPcpMappings();

	std::thread m_worker;
	std::atomic<bool> m_running{false};
	std::atomic<bool> m_cancel{false};
	std::atomic<bool> m_allowFallback{false};
	PortMapping::Session m_session;
	Route m_route;
	UINT_PTR m_leaseTimer = 0;
	uint64_t m_renewAt = 0;
	uint64_t m_expiresAt = 0;
};

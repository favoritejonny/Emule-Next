// eMule Next - GPL-2.0-or-later
#pragma once
#include "../srchybrid/PortMappingProtocol.h"

namespace {
using namespace PortMapping;
using PortMapping::Status;
constexpr uint32_t publicIP = 0x5db8d822;

Session NewSession(bool udp = true)
{
	Session session;
	session.localAddress = 0xc0a80102;
	for (unsigned i = 0; i < session.leases.size(); ++i) {
		auto& request = session.leases[i].request;
		request.localAddress = session.localAddress;
		request.transport = i == 0 ? 6 : 17;
		request.internalPort = request.externalPort = i == 0 ? 4662 : udp ? 4672 : 0;
		request.operation = Operation::Map;
		for (unsigned n = 0; n < request.nonce.size(); ++n) request.nonce[n] = static_cast<unsigned char>(1 + n + i * 12);
	}
	return session;
}

// Independent server-side fixtures: never call the request encoder to build
// responses. Test both bytes on the wire and complete negotiation sequences.
Packet Response(const Request& request, unsigned result = 0)
{
	const bool probe = request.operation == Operation::Probe;
	if (request.protocol == Protocol::PCP) {
		Packet packet(probe ? 24 : 60, 0);
		packet[0] = 2;
		packet[1] = probe ? 0x80 : 0x81;
		packet[3] = static_cast<unsigned char>(result);
		Write32(packet.data() + 4, probe || request.operation == Operation::Delete ? 0 : 120);
		Write32(packet.data() + 8, 100);
		if (!probe) {
			std::memcpy(packet.data() + 24, request.nonce.data(), 12);
			packet[36] = request.transport;
			Write16(packet.data() + 40, request.internalPort);
			Write16(packet.data() + 42, request.internalPort);
			packet[54] = packet[55] = 0xff;
			Write32(packet.data() + 56, publicIP);
		}
		return packet;
	}
	Packet packet(probe ? 12 : 16, 0);
	packet[1] = probe ? 128 : request.transport == 6 ? 130 : 129;
	Write16(packet.data() + 2, static_cast<uint16_t>(result));
	Write32(packet.data() + 4, 100);
	if (probe) Write32(packet.data() + 8, publicIP);
	else {
		Write16(packet.data() + 8, request.internalPort);
		Write16(packet.data() + 10, request.internalPort);
		Write32(packet.data() + 12, 120);
	}
	return packet;
}

Reply SuccessfulExchange(const Request& request)
{
	const auto response = Response(request);
	return Decode(request, response.data(), response.size());
}

bool WireTests()
{
	auto session = NewSession();
	Request request = session.leases[0].request;
	request.protocol = Protocol::NATPMP;
	const Packet expected = {0, 2, 0, 0, 0x12, 0x36, 0x12, 0x36, 0, 0, 0, 120};
	if (Encode(request) != expected) return false;
	request.protocol = Protocol::PCP;
	const Packet wire = Encode(request);
	if (wire.size() != 60 || wire[0] != 2 || wire[1] != 1 || Read32(wire.data() + 4) != 120
		|| wire[18] != 0xff || wire[19] != 0xff || Read32(wire.data() + 20) != session.localAddress
		|| std::memcmp(wire.data() + 24, request.nonce.data(), 12) != 0 || wire[36] != 6
		|| Read16(wire.data() + 40) != 4662 || Read16(wire.data() + 42) != 4662) return false;
	const auto good = Response(request);
	const Reply valid = Decode(request, good.data(), good.size());
	if (valid.status != Status::Success || valid.externalPort != 4662 || valid.externalAddress != publicIP || valid.lifetime != 120) return false;
	for (size_t size = 0; size < good.size(); ++size)
		if (Decode(request, good.data(), size).status != Status::Ignore) return false;
	for (unsigned offset : {0u, 1u, 24u, 35u, 36u, 40u, 41u}) {
		auto bad = good;
		bad[offset] ^= 0x40;
		if (Decode(request, bad.data(), bad.size()).status != Status::Ignore) return false;
	}
	auto bad = good;
	bad.resize(64, 0); // unknown mandatory option
	if (Decode(request, bad.data(), bad.size()).status != Status::Ignore) return false;
	bad[60] = 128; // optional, empty option
	if (Decode(request, bad.data(), bad.size()).status != Status::Success) return false;
	bad[63] = 8; // truncated optional payload
	if (Decode(request, bad.data(), bad.size()).status != Status::Ignore) return false;
	request.operation = Operation::Delete;
	const auto deletion = Encode(request);
	if (deletion.size() != 60 || Read32(deletion.data() + 4) != 0
		|| std::memcmp(deletion.data() + 24, request.nonce.data(), 12) != 0 || Read16(deletion.data() + 40) == 0) return false;
	request.protocol = Protocol::NATPMP;
	if (!Encode(request).empty()) return false;
	request.operation = Operation::Map;
	request.internalPort = 0;
	if (!Encode(request).empty()) return false;
	request = session.leases[1].request;
	request.protocol = Protocol::NATPMP;
	const auto udp = Response(request);
	if (Decode(request, udp.data(), udp.size()).status != Status::Success) return false;
	for (unsigned offset : {0u, 1u, 8u, 9u}) {
		auto invalid = udp;
		invalid[offset] ^= 1;
		if (Decode(request, invalid.data(), invalid.size()).status != Status::Ignore) return false;
	}
	Request probe;
	const unsigned char unsupported[] = {0, 0, 0, 1, 0, 0, 0, 100};
	if (Decode(probe, unsupported, sizeof(unsupported)).status != Status::UnsupportedVersion) return false;
	if (Decode(session.leases[0].request, unsupported, sizeof(unsupported)).status != Status::Ignore) return false;
	// Exercise every decoder length with deterministic malformed packets.
	uint32_t random = 17;
	for (unsigned n = 0; n < 4000; ++n) {
		Packet fuzz(n % 128, 0);
		for (auto& byte : fuzz) { random = random * 1664525u + 1013904223u; byte = static_cast<unsigned char>(random >> 24); }
		Decode(request, fuzz.data(), fuzz.size());
		Decode(session.leases[0].request, fuzz.data(), fuzz.size());
	}
	return Decode(request, nullptr, 64).status == Status::Ignore;
}

bool NegotiationTests()
{
	auto session = NewSession();
	unsigned calls = 0;
	auto exchange = [&calls](const Request& request) { ++calls; return SuccessfulExchange(request); };
	if (Run(session, exchange) != Status::Success || calls != 3 || !session.established || session.allowUPnPFallback
		|| session.renewAfterSeconds != 60) return false;
	const auto originalNonce = session.leases[0].request.nonce;
	if (Run(session, exchange) != Status::Success || calls != 5 || session.leases[0].request.nonce != originalNonce) return false;
	session = NewSession(false);
	calls = 0;
	if (Run(session, exchange) != Status::Success || calls != 2 || session.leases[1].issued) return false;

	session = NewSession();
	calls = 0;
	auto natExchange = [&calls](const Request& request) {
		++calls;
		if (request.protocol == Protocol::PCP) { Reply reply; reply.status = Status::UnsupportedVersion; return reply; }
		return SuccessfulExchange(request);
	};
	if (Run(session, natExchange) != Status::Success || calls != 4 || session.protocol != Protocol::NATPMP) return false;
	if (Run(session, natExchange) != Status::Success || calls != 7) return false; // public address rechecked

	for (Status failure : {Status::Refused, Status::Timeout, Status::UnsupportedVersion, Status::Failed, Status::NetworkChanged}) {
		session = NewSession();
		const auto status = Run(session, [failure](const Request&) { Reply reply; reply.status = failure; return reply; });
		if (status != failure || session.established || session.leases[0].issued
			|| session.allowUPnPFallback != (failure == Status::Timeout || failure == Status::UnsupportedVersion)) return false;
	}
	for (Status failure : {Status::Refused, Status::Timeout, Status::UnsupportedVersion, Status::Failed, Status::NetworkChanged}) {
		session = NewSession();
		const auto status = Run(session, [failure](const Request& request) {
			if (request.operation == Operation::Probe) return SuccessfulExchange(request);
			Reply reply; reply.status = failure; return reply;
		});
		if (status != failure || session.allowUPnPFallback || session.established || !session.leases[0].issued || session.leases[1].issued) return false;
	}
	for (unsigned scenario = 0; scenario < 5; ++scenario) {
		session = NewSession();
		const auto status = Run(session, [scenario](const Request& request) {
			auto reply = SuccessfulExchange(request);
			if (request.operation == Operation::Map) {
				if (scenario == 0) ++reply.externalPort;
				if (scenario == 1) reply.externalAddress = 0x64400001; // CGNAT
				if (scenario == 2) reply.lifetime = 0;
				if (scenario == 3 && request.transport == 17) reply.epoch = 1;
				if (scenario == 4 && request.transport == 17) ++reply.externalAddress;
			}
			return reply;
		});
		const Status expected[] = {Status::WrongPort, Status::NonPublicAddress, Status::ShortLease, Status::NetworkChanged, Status::NetworkChanged};
		if (status != expected[scenario] || session.established || session.allowUPnPFallback) return false;
	}
	session = NewSession();
	calls = 0;
	const auto sharedStatus = Run(session, [&calls](const Request& request) {
		++calls;
		Reply reply;
		if (request.protocol == Protocol::PCP) reply.status = Status::UnsupportedVersion;
		else { reply = SuccessfulExchange(request); reply.externalAddress = 0xc0a80101; }
		return reply;
	});
	if (sharedStatus != Status::NonPublicAddress || calls != 2 || session.allowUPnPFallback || session.leases[0].issued) return false;
	return true;
}
}

bool SelfTestPortMapping()
{
	const uint32_t invalid[] = {0, 0x0a000001, 0xac100001, 0xac1fffff, 0xc0a80101, 0x64400001,
		0x647fffff, 0x7f000001, 0xa9fe0001, 0xe0000001, 0xffffffff, 0xc0000201, 0xc6336401, 0xcb007101, 0xc6120001};
	for (uint32_t address : invalid) if (PortMapping::IsUsablePublicAddress(address)) return false;
	return PortMapping::IsUsablePublicAddress(publicIP) && PortMapping::IsUsablePublicAddress(0x08080808)
		&& WireTests() && NegotiationTests();
}

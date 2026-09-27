// eMule Next - GPL-2.0-or-later
// Independent IPv4 wire codec and setup policy for RFC 6886 / RFC 6887.
#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <vector>

namespace PortMapping
{
using Packet = std::vector<unsigned char>;
using Nonce = std::array<unsigned char, 12>;
enum class Protocol { PCP, NATPMP };
enum class Operation { Probe, Map, Delete };
enum class Status { Ignore, Success, Timeout, UnsupportedVersion, UnsupportedOpcode,
	Refused, Failed, WrongPort, NonPublicAddress, NetworkChanged, ShortLease };

struct Request {
	Protocol protocol = Protocol::PCP;
	Operation operation = Operation::Probe;
	uint32_t localAddress = 0; // IPv4, host byte order
	uint8_t transport = 6;
	uint16_t internalPort = 0;
	uint16_t externalPort = 0;
	uint32_t externalAddress = 0;
	uint32_t lifetime = 120;
	Nonce nonce{};
};
struct Reply {
	Status status = Status::Ignore;
	uint32_t lifetime = 0;
	uint32_t epoch = 0;
	uint32_t externalAddress = 0;
	uint16_t externalPort = 0;
};

inline uint16_t Read16(const unsigned char* p) { return uint16_t((uint16_t(p[0]) << 8) | p[1]); }
inline uint32_t Read32(const unsigned char* p) { return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | p[3]; }
inline void Write16(unsigned char* p, uint16_t v) { p[0] = static_cast<unsigned char>(v >> 8); p[1] = static_cast<unsigned char>(v); }
inline void Write32(unsigned char* p, uint32_t v) { p[0] = static_cast<unsigned char>(v >> 24); p[1] = static_cast<unsigned char>(v >> 16); p[2] = static_cast<unsigned char>(v >> 8); p[3] = static_cast<unsigned char>(v); }
inline void WriteAddress(unsigned char* p, uint32_t address) { p[10] = p[11] = 0xff; Write32(p + 12, address); }
inline bool ReadAddress(const unsigned char* p, uint32_t& address)
{
	for (unsigned i = 0; i < 10; ++i)
		if (p[i] != 0) return false;
	if (p[10] != 0xff || p[11] != 0xff) return false;
	address = Read32(p + 12);
	return true;
}

inline bool IsPrivateAddress(uint32_t address)
{
	return (address >> 24) == 10 || (address >> 20) == 0xac1 || (address >> 16) == 0xc0a8;
}
inline bool IsUsablePublicAddress(uint32_t address)
{
	const uint32_t first = address >> 24;
	return first != 0 && first != 127 && first < 224 && !IsPrivateAddress(address)
		&& (address >> 22) != 0x191 // 100.64.0.0/10
		&& (address >> 16) != 0xa9fe && (address >> 17) != 0x6309 // link-local, benchmarking
		&& (address >> 8) != 0xc63364 && (address >> 8) != 0xcb0071
		&& (address >> 8) != 0xc00000 && (address >> 8) != 0xc00002
		&& (address >> 8) != 0xc05863;
}

inline Packet Encode(const Request& request)
{
	const bool probe = request.operation == Operation::Probe;
	const bool remove = request.operation == Operation::Delete;
	if (!probe && (request.internalPort == 0 || (request.transport != 6 && request.transport != 17)
		|| (!remove && (request.externalPort == 0 || request.lifetime == 0))))
		return {};
	// NAT-PMP has no ownership nonce: let its leases expire, never issue a
	// delete-all or delete an indistinguishable pre-existing mapping.
	if (remove && request.protocol == Protocol::NATPMP)
		return {};
	if (request.protocol == Protocol::NATPMP) {
		Packet bytes(probe ? 2 : 12, 0);
		if (!probe) {
			bytes[1] = request.transport == 6 ? 2 : 1;
			Write16(bytes.data() + 4, request.internalPort);
			Write16(bytes.data() + 6, request.externalPort);
			Write32(bytes.data() + 8, request.lifetime);
		}
		return bytes;
	}
	Packet bytes(probe ? 24 : 60, 0);
	bytes[0] = 2;
	bytes[1] = probe ? 0 : 1;
	Write32(bytes.data() + 4, probe || remove ? 0 : request.lifetime);
	WriteAddress(bytes.data() + 8, request.localAddress);
	if (!probe) {
		std::memcpy(bytes.data() + 24, request.nonce.data(), request.nonce.size());
		bytes[36] = request.transport;
		Write16(bytes.data() + 40, request.internalPort);
		Write16(bytes.data() + 42, request.externalPort);
		WriteAddress(bytes.data() + 44, request.externalAddress);
	}
	return bytes;
}

inline Status ResultStatus(unsigned result)
{
	if (result == 0) return Status::Success;
	if (result == 1) return Status::UnsupportedVersion;
	if (result == 2) return Status::Refused;
	return Status::Failed;
}

inline Reply Decode(const Request& request, const unsigned char* bytes, size_t size)
{
	Reply reply;
	if (bytes == nullptr || size < 8 || size > 1100) return reply;
	const bool probe = request.operation == Operation::Probe;
	if (request.protocol == Protocol::PCP) {
		// RFC 6886 section 3.5: old gateways reply with version=0, opcode=0.
		if (probe && size == 8 && bytes[0] == 0 && bytes[1] == 0 && Read16(bytes + 2) == 1) {
			reply.status = Status::UnsupportedVersion;
			return reply;
		}
		if (size < 24 || size % 4 != 0 || bytes[0] != 2 || bytes[1] != (probe ? 0x80 : 0x81)) return reply;
		const unsigned result = bytes[3];
		const size_t payloadSize = probe || (result != 0 && size == 24) ? 24 : 60;
		if (size < payloadSize) return reply;
		// Safely skip optional extensions; reject unknown mandatory options.
		for (size_t offset = payloadSize; offset < size;) {
			if (size - offset < 4 || bytes[offset] < 128) return reply;
			const size_t optionSize = 4 + ((size_t(Read16(bytes + offset + 2)) + 3) & ~size_t(3));
			if (optionSize > size - offset) return reply;
			offset += optionSize;
		}
		if (!probe && payloadSize == 60 && (std::memcmp(bytes + 24, request.nonce.data(), 12) != 0
			|| bytes[36] != request.transport || Read16(bytes + 40) != request.internalPort)) return reply;
		reply.status = result == 4 ? Status::UnsupportedOpcode : ResultStatus(result);
		reply.lifetime = Read32(bytes + 4);
		reply.epoch = Read32(bytes + 8);
		if (reply.status == Status::Success && !probe) {
			reply.externalPort = Read16(bytes + 42);
			if (!ReadAddress(bytes + 44, reply.externalAddress)) reply.status = Status::Failed;
		}
	} else {
		const unsigned opcode = probe ? 128 : request.transport == 6 ? 130 : 129;
		if (size != (probe ? 12u : 16u) || bytes[0] != 0 || bytes[1] != opcode) return reply;
		if (!probe && Read16(bytes + 8) != request.internalPort) return reply;
		const unsigned result = Read16(bytes + 2);
		reply.status = result == 5 ? Status::UnsupportedOpcode : ResultStatus(result);
		reply.epoch = Read32(bytes + 4);
		if (probe) reply.externalAddress = Read32(bytes + 8);
		else { reply.externalPort = Read16(bytes + 10); reply.lifetime = Read32(bytes + 12); }
	}
	return reply;
}

struct Lease {
	Request request;
	bool issued = false;
	uint32_t lifetime = 0;
};
struct Session {
	Protocol protocol = Protocol::PCP;
	uint32_t localAddress = 0;
	uint32_t externalAddress = 0;
	std::array<Lease, 2> leases;
	bool established = false;
	bool allowUPnPFallback = false;
	uint32_t renewAfterSeconds = 0;
};

// Exchange is injectable so CI can run complete negotiations without sockets.
template <typename Exchange>
Status Run(Session& session, Exchange exchange)
{
	session.allowUPnPFallback = false;
	if (session.leases[0].request.internalPort == 0) return Status::Failed;
	const bool renewal = session.established;
	session.established = false;
	if (!renewal) {
		Request probe;
		probe.localAddress = session.localAddress;
		probe.nonce = session.leases[0].request.nonce; // retransmission jitter, not a probe wire field
		Reply reply = exchange(probe);
		if (reply.status == Status::UnsupportedVersion) {
			probe.protocol = Protocol::NATPMP;
			reply = exchange(probe);
			session.protocol = Protocol::NATPMP;
		} else session.protocol = Protocol::PCP;
		// A PCP server lacking ANNOUNCE may still implement MAP.
		if (reply.status != Status::Success && !(session.protocol == Protocol::PCP && reply.status == Status::UnsupportedOpcode)) {
			session.allowUPnPFallback = reply.status == Status::Timeout || reply.status == Status::UnsupportedVersion
				|| reply.status == Status::UnsupportedOpcode;
			return reply.status;
		}
		if (session.protocol == Protocol::NATPMP) {
			if (!IsUsablePublicAddress(reply.externalAddress)) return Status::NonPublicAddress;
			session.externalAddress = reply.externalAddress;
		}
	}
	else if (session.protocol == Protocol::NATPMP) {
		Request probe;
		probe.protocol = Protocol::NATPMP;
		const Reply reply = exchange(probe);
		if (reply.status != Status::Success) return reply.status;
		if (!IsUsablePublicAddress(reply.externalAddress)) return Status::NonPublicAddress;
		if (reply.externalAddress != session.externalAddress) return Status::NetworkChanged;
	}
	session.renewAfterSeconds = 43200;
	uint32_t previousEpoch = 0;
	for (Lease& lease : session.leases) {
		if (lease.request.internalPort == 0) continue; // disabled UDP
		lease.request.protocol = session.protocol;
		lease.request.operation = Operation::Map;
		lease.request.localAddress = session.localAddress;
		lease.issued = true;
		const Reply reply = exchange(lease.request);
		if (reply.status != Status::Success) return reply.status;
		lease.lifetime = reply.lifetime;
		lease.request.externalPort = reply.externalPort;
		if (session.protocol == Protocol::PCP) lease.request.externalAddress = reply.externalAddress;
		if (reply.externalPort != lease.request.internalPort) return Status::WrongPort;
		if (reply.lifetime < 60) return Status::ShortLease;
		if (session.protocol == Protocol::PCP) {
			if (!IsUsablePublicAddress(reply.externalAddress)) return Status::NonPublicAddress;
			if (session.externalAddress != 0 && session.externalAddress != reply.externalAddress) return Status::NetworkChanged;
			session.externalAddress = reply.externalAddress;
		}
		if (previousEpoch > reply.epoch && previousEpoch - reply.epoch > 1) return Status::NetworkChanged;
		previousEpoch = reply.epoch;
		const uint32_t renewalSeconds = reply.lifetime > 86400 ? 43200 : reply.lifetime / 2;
		if (renewalSeconds < session.renewAfterSeconds) session.renewAfterSeconds = renewalSeconds;
	}
	session.established = true;
	return Status::Success;
}
}

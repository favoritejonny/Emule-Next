# Focused technical roadmap

The next development cycle deliberately limits deep architectural changes to
two features. This keeps the mature eD2K/Kad client testable and avoids mixing
unrelated network and storage risks in one release.

## 1. Cache for large shared collections

Goal: reduce repeated disk work and startup time for users sharing thousands of
files without changing eD2K file hashes or sharing semantics.

The design phase will measure cold start, warm start, memory use and hashing
time on representative collections. The cache will record only local technical
metadata needed to recognise unchanged files, including normalised path, size,
last-write data, file identity when available and the already computed hashes.
It will never replace content verification when metadata is ambiguous.

Required properties:

- incremental scanning and bounded background work;
- invalidation after rename, modification, replacement or deletion;
- atomic writes and safe recovery after interruption or power loss;
- a schema/version marker and an automatic rebuild path;
- no telemetry and no transmission of the local file index;
- a preference to clear/rebuild the cache;
- identical visible shares and hashes with the cache enabled or disabled.

Acceptance tests will cover cold and warm starts, at least 1,500 files, changed
and deleted files, timestamp edge cases, interrupted writes, cache corruption
and both Win32 and x64 memory limits.

## 2. Optional VPN protection

Goal: let an informed user bind eMule Next to a selected VPN network interface
and stop new peer traffic if that interface disappears. This is an opt-in
safety feature, not anonymity, protocol obfuscation or a replacement for a
system-wide VPN kill switch.

The implementation will be staged:

1. enumerate interfaces and addresses using supported Windows APIs;
2. let the user explicitly choose an interface, with clear recovery controls;
3. bind listening and outgoing sockets consistently;
4. monitor interface/address changes and fail closed by pausing connections;
5. require explicit user action before falling back to another interface;
6. log locally why traffic was paused, without collecting or uploading data.

Tests must cover VPN connection, reconnect, address change, sleep/resume,
adapter removal, invalid saved selection, application restart and emergency
recovery. The first implementation remains IPv4-only so VPN behaviour can be
validated independently of the later IPv6 work.

## Later versions

IPv6 is a separate compatibility project. It requires an audit of address
storage, filtering, server/Kad assumptions, UI, configuration and mixed
IPv4/IPv6 behaviour before implementation. It will not be slipped into the VPN
or cache release.

QUIC remains experimental research only. It is not part of the production
eD2K/Kad transport roadmap and must not reduce interoperability with existing
clients. Any prototype belongs in an isolated experiment with separate threat,
privacy and compatibility analysis.

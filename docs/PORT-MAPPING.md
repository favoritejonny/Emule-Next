# PCP and NAT-PMP: local experimental integration

31 August 2026. Not a public release and not a guarantee of High ID.

## Reference reviewed and dependencies

The user supplied qBittorrent 5.2.3 sources. Its
`src/base/bittorrent/sessionimpl.cpp` delegates mapping to libtorrent through
`enable_natpmp`, `enable_upnp` and port-mapping alerts. Its COPYING file
describes GPL-2.0-or-later source with an OpenSSL exception, and separately
GPLv3-or-later assets in binary distributions.

Reference file SHA-256:
`f9fe027c0081084228dc97b51df4c1b9de98d187ebe289c1e66552bdcfb71205`.

No qBittorrent, libtorrent, Qt, artwork or third-party source was imported.
The new GPL-2.0-or-later module implements the public protocol formats
independently and uses Windows networking and the already-linked BCrypt RNG.
There are no new third-party dependencies or assets to license.

References: [NAT-PMP, RFC 6886](https://www.rfc-editor.org/rfc/rfc6886.html),
[PCP, RFC 6887](https://www.rfc-editor.org/rfc/rfc6887.html),
[libtorrent settings](https://libtorrent.org/reference-Settings.html).

## Opt-in behavior

Only the new automatic-home-network consent in the first-run wizard enables
the additional protocols. Users who enabled only historical UPnP keep the
previous implementations. Cancel does not save consent. The new path tries:

1. A non-mutating PCP ANNOUNCE request to the directly attached gateway.
2. NAT-PMP if that gateway explicitly reports an unsupported PCP version.
3. Verified MiniUPnP when discovery is unavailable, before any new-protocol
   mapping request has been attempted.

A PCP server lacking ANNOUNCE can still be tried with MAP. Explicit refusal,
uncertain MAP results, changed ports/addresses and non-public WAN addresses
do not trigger fallback that might bypass refusal. A silent PCP gateway
does not cause a blind NAT-PMP downgrade. Each attempt is bounded and runs
outside the UI thread; cooperative cancellation never terminates a thread.

Only the configured TCP and optional UDP ports are requested. Disabled UDP
stays disabled; the Web interface is never mapped by this module. Returned
external ports must equal client ports: a different assignment is reported
as failure, without silently changing preferences. The result names the
protocol used. Router acknowledgment and Internet reachability are distinct.

## Network safeguards

Existing checks remain mandatory: private Windows profile, no detected VPN,
proxy or explicit interface binding. The module requires a private, on-link
IPv4 gateway on the selected physical Ethernet/Wi-Fi route. It binds to that
local address, connects UDP to gateway port 5351, uses TTL 1 and rechecks the
route and network eligibility before requests and before accepting replies.
PCP replies must match a random 96-bit nonce, transport and internal port.
Both decoders check lengths, opcodes and optional-field boundaries.

There is no public IP website, telemetry, router password prompt, firewall
modification, elevation, gateway scan or configurable remote PCP server.
IPv6 mapping, PCP authentication, VPN-provider forwarding and CGNAT traversal
are not included. A non-public WAN address is explained, not repaired.
These checks are not a VPN kill switch and cannot guarantee protection
against every interface change or an unrecognized VPN. The unauthenticated
protocols are for trusted home networks only.

## Lifetime and cleanup

PCP/NAT-PMP request a 120-second lifetime and renew at half the shortest
granted lifetime, independently of eD2K/Kad activity. Leases shorter than
60 seconds fail safely. Time accounting starts before requests; excessively
large replies are capped for local renewal scheduling. The router controls
actual expiry. Renewals preserve PCP nonces and recheck the NAT-PMP external
address. Failure stops renewal and does not imply High ID.

PCP removal uses only nonzero internal ports and nonces of requests issued
by this process. Cleanup is best effort on the same eligible network; lost
cleanup packets leave leases to expire. NAT-PMP cannot distinguish a reused
mapping from an earlier client's mapping to the same endpoint, so this
prototype never sends NAT-PMP deletion requests. It stops renewing instead.
No delete-all request is generated. Partial setups can leave temporary
mappings until router expiry; the consent text states this explicitly.
Disabling the option stops renewal. Unrelated router rules are not explicitly
deleted by this module.

## Tests and publication gate

Offline tests cover wire vectors, malformed/truncated replies, wrong
opcodes/nonces/ports, option lengths, unsupported versions, refusal, partial
failure, different external ports, CGNAT/private addresses, disabled UDP,
renewal, state loss between mappings and 4,000 malformed packets. Complete
negotiations use injected replies and do not contact real gateways.

On 31 August 2026, final Release builds and the complete offline smoke suite
passed on Win32 and x64 (Visual Studio 2026/v145), including saved-profile
preservation and PE hardening checks. The standalone codec/policy suite also
passed under AddressSanitizer on x86 and x64. Sanitizer checks cover the
protocol core, not the full application or the Windows socket worker.

Before publication, test real PCP/NAT-PMP routers, UPnP fallback, multiple
renewal cycles, shutdown/cancellation, sleep/resume, network changes,
conflicting mappings, Windows 10/11 on Win32/x64 and external TCP/UDP
reachability. Multicast reboot announcements are not subscribed to in this
prototype; recovery happens at renewal or an explicit connectivity recheck.
Unusual routers/routes may require manual setup. Offline tests do not
replace these integration checks.

# Automatic home connection setup - experimental

Local development milestone, updated 2026-09-07. Not a published beta release.
The September 3 changes are built locally for Win32/x64 and included in
`dist/guided-first-start-preview-20260903`; earlier packages remain unchanged.

## What this first iteration does

- Six concise pages cover welcome, name/startup, ports plus router setup,
  optional line-speed measurement, eD2K/Kad plus server-list setup, and the
  final summary. Related choices are deliberately kept together.
- The ports page contains a first-run checked option for automatic home-router
  setup. The user can clear it before Finish. The protocol is negotiated
  automatically; this iteration does not add per-protocol overrides.
- Selecting it does not touch the router. Only Finish, after valid ports have
  been bound successfully, enables UPnP and the new private-network safeguard.
  Cancel or a dialog creation failure does not commit this choice.
- Existing profiles retain their UPnP setting unless the user opts in. Opening
  the wizard again loads nickname, startup, priorities, obfuscation and network
  choices from the current preferences instead of replacing them with wizard
  defaults.
- The new mode checks local adapters and Windows network profiles before
  discovery and before refresh. Public, domain, mixed or unknown profiles,
  configured proxies, explicit interface binding and recognizable active VPN
  adapters pause automatic setup. Network profiles are never changed.
- The new mode tries PCP, NAT-PMP on explicit version negotiation, then
  verified MiniUPnP only after non-mutating discovery is unavailable.
  It uses the configured TCP/UDP ports only, with UDP disabled preserved
  and no Web port. See `PORT-MAPPING.md` for negotiation, leases and limits.
- On the UPnP path, an existing mapping must match internal IP, port and
  enabled state. Different/disabled mappings are not overwritten. Deletion
  requires an enabled rule with this client's endpoint and eMule Next's own
  description. Unreadable UPnP router tables fail closed. PCP uses its own
  request nonces for cleanup; NAT-PMP leases are allowed to expire.
- A non-public router WAN address is explained as possible double NAT/CGNAT,
  not diagnosed conclusively as a provider fault. A router acknowledgment is
  explicitly distinguished from Internet reachability and High ID.
- Invalid or out-of-range port values are rejected before conversion. Failed
  binding restores the previous port settings; socket state no longer reports
  a closed, previously bound port as active.
- The optional speed-test page uses Cloudflare's public HTTPS measurement
  endpoints and runs work off the UI thread. It transfers progressively larger
  non-cacheable samples up to about 100 MB, never calls the provider's
  result-recording endpoint, and lets the user edit the measured KB/s values.
  Applying them sets graph capacities, limits upload to 80% of the measured
  capacity and leaves download unlimited. Skipping the test changes nothing.
- The Internet server-list option is checked by default on first run and is
  used only on Finish with eD2K selected, after saved server lists finish
  loading. It requests `server.met` through HTTPS, preserves existing entries
  (including name, priority and failed count), honors IP filters, merges valid
  new entries and saves the resulting ordinary list. If the request fails, the
  eight bundled addresses remain an offline fallback. Previously saved static
  lists remain untouched and automatic server connection is not enabled.
- Safe server connection is deliberately unchecked on a fresh profile because
  enabling it can make connection unnecessarily difficult with a short list;
  existing profiles retain their current choice.

## Boundaries and privacy

The first-run wizard no longer offers a Windows Firewall control-panel button.
Windows' normal permission prompt is unchanged. Removing this shortcut does
**not** grant permission, create firewall rules, elevate the client, disable
protection or enable public-network access. Portable mode receives no new
registry integration. Existing firewall rules and the user's decisions remain
unchanged; a blocked connection can still require manual troubleshooting.

The speed test is strictly manual. Its UI discloses the maximum temporary data
transfer and that Cloudflare can see the public IP. Results are neither sent to
nor stored by the eMule Next project. The optional server-list request likewise
reveals normal connection metadata to eMule-Security, not to the project. These
network requests are documented in `PRIVACY.md`; neither is telemetry. Server
High ID and Kad status remain the normal protocol-level reachability indicators
after connecting.

VPN recognition is a conservative heuristic, **not VPN protection or a kill
switch**. It cannot recognize every VPN or guarantee protection against an
interface/route change while a request is in flight. The consent explicitly
requires a user-controlled home network without a VPN. Do not advertise this
as bypassing CGNAT, guaranteeing High ID, or eliminating firewall setup.

## Verification and remaining work

- Local Release Win32 and x64 build status and the complete offline CI smoke
  suite are recorded in `BUILD_STATUS.md`. The hosted GitHub workflow has not
  been run for this unpushed prototype.
- CI self-tests cover port boundaries, consent/profile policy, matching and
  mismatching mappings, missing enabled state, malformed port responses and
  clean-profile defaults. These tests never perform router discovery or open
  transfer listeners.
- Server-list tests cover all Finish/selection/eD2K combinations before and
  after initialization, repeat additions, existing entries and rejected adds.
  Source checks also require an HTTPS update URL and the offline fallback.
- Speed policy tests cover zero, small and large upload capacities and verify
  the 80% recommendation without making an Internet request.
- Source validation requires all 32 connection/wizard messages in each of the 43 shipped
  language modules, checks UTF-8 encoding and preserves `%u`/`%s` format
  placeholders. A hidden native dialog-resource test loads embedded English
  plus all 43 external language DLLs and measures the new labels and
  status messages across the six first-run pages with the actual
  dialog font. The first translation pass is
  machine-assisted and still benefits from native-speaker review.
- Hidden Win32/x64 probes confirm the firewall button is absent and check that
  affected first-run labels fit with embedded English and all language DLLs.
- Still required before publication: real first-run Finish/Cancel/reopen
  tests, UPnP router success/failure/conflicts, CGNAT and multiple routers,
  Windows firewall behavior, restart/cleanup, Windows 10/11 and high-DPI
  visual checks. Unit/resource tests do not replace these integration tests.
- Native review of the new translations and automated end-to-end reachability
  testing remain subsequent milestones. A duplicate firewall permission step
  is no longer planned for the first-run wizard.

The previously published alpha ZIPs and GitHub release are not replaced by
this local experiment.

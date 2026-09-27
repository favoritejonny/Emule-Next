# eMule Next change record

## 1.0.0-beta.1 (released 2026-09-27)

- Promote the in-development public identity to eMule Next 1.0.0 Beta 1
  (`1.0.0-beta.1`, numeric Windows build `1.0.0.2`). Use that identity in
  window titles, ready notifications, Server information, exported statistics
  and the Web interface while retaining Community 0.72a exclusively as the
  eD2K/Kad compatibility version. Mark Windows resources as prerelease without
  enabling the legacy `_BETA` behaviours that create shared test files.
- Speed up large shared collections without changing `known.met`,
  `known2_64.met`, eD2K hashes or credit files. Known-file reuse now uses a
  lazily rebuilt filename index while retaining the exact timestamp, size and
  name checks, including support for duplicate filenames.
- Increase sequential hashing reads from 8 KiB to 64 KiB and the file stream
  buffer to 256 KiB. MD4 and AICH remain in the same single read pass and a
  new offline regression covers several read-buffer boundaries.
- Reject a hash result when the file size or last-write time changes while the
  file is being read, avoiding stale identities for files still being copied
  or modified. Correct the low 32-bit word used by the HANDLE-based file-size
  helper, which previously read the unrelated file-index field.
- Rebuild the visible shared-files list in linear time by skipping redundant
  duplicate scans when the list has just been cleared, with redraw suspended
  until the rebuild is complete. Normal incremental additions retain their
  duplicate checks.
- Fix first-run auto-connect when the startup timer completes while the modal
  wizard is still open. Finishing a fresh setup now starts the selected eD2K
  and Kad networks when `Connect automatically` is checked, including after
  automatic router setup; clearing the checkbox leaves both networks idle.
  Add an offline regression test for every timing and checkbox combination.
- Remove the obsolete Windows Firewall control-panel shortcut and the
  redundant standalone port-test step from first-run setup. Windows' own
  permission prompt remains independent; firewall rules and saved preferences
  are not changed by removing those wizard controls.
- Restore the main window's resource-template minimum before applying saved
  placement, matching the upstream sizing behavior instead of introducing a
  fixed 1200x800 default. Keep larger saved windows and existing show-state
  choices; leave wizard/secondary dialogs and the shared layout engine alone.
  Zero-initialize the default WINDOWPLACEMENT, retaining upstream coordinates.
- Consolidate first-run setup into six clear pages: ports and automatic router
  setup share one page, while eD2K/Kad and server-list setup share another.
  Fresh profiles select automatic router negotiation by default; no operation
  is committed before Finish, and existing profiles retain their preferences.
- Add a voluntary HTTPS line-speed test with editable KB/s results. Applying
  them sets the upload limit to 80% of measured capacity and leaves download
  unlimited; skipping the test leaves existing speeds unchanged. The client
  does not upload results to the project or call the provider's result logger.
- Make Internet eD2K server-list refresh a checked first-run choice, applied
  only on Finish with eD2K selected. Download and merge `server.met` over HTTPS,
  preserve existing entries and IP filters, and retain the bundled list as an
  offline fallback. Safe server connection is unchecked only for fresh profiles.
- Add a checked first-run choice that downloads `nodes.dat` over HTTPS when Kad
  is selected, so a clean portable profile can obtain initial Kad contacts
  without a separate manual step.
- Correct the wizard's Kad checkbox update when UDP is disabled, so the
  visible checkbox matches the network setting that will be saved.
- Added an experimental IPv4 PCP/NAT-PMP mapper to the explicit home-network
  setup, with bounded background requests, verified replies, lease renewal,
  conservative UPnP fallback and no silent port changes. NAT-PMP mappings
  expire instead of issuing deletion requests without ownership proof.
  Added protocol/negotiation tests; real-router validation remains required.
- Select the Windows display language automatically when no language has
  been saved, including supported regional variants. Keep existing choices
  and use embedded English if a translation is unavailable. Add offline
  language-selection and saved-profile restart tests.
- Added experimental first-run home-network setup, committed only on Finish,
  with private-network/VPN safeguards and router diagnostics. Its 32 current
  messages are present in all 43 shipped language modules; English and Italian
  were reviewed directly, while the first translations remain open to
  native-speaker review.
- Extended source and binary tests to require every connection-setup message,
  preserve format placeholders and measure all localized visible texts against
  the real wizard controls. Shortened one Greek firewall label found by the
  new overflow check.
- Verify UPnP mapping destination and enabled state; preserve conflicting or
  disabled rules and restrict cleanup to identified eMule Next mappings.
- Validate port ranges, preserve existing wizard choices and recover socket
  port state after failed binding. Add offline policy and native layout checks.
- Keep upload and download unlimited when the first-run speed test is skipped
  and when the legacy connection helper is used. Remove the historical hidden
  upload estimate that behaved like a cap while the visible upload limit was
  disabled; slot-opening safeguards remain active.
- Align the completed-file and temporary-file directory fields and their browse
  buttons in Preferences > Directories without changing either saved path.
- Fixed Tools > Useful links > Help and discussions to open the eMule Next
  GitHub Discussions page instead of the repository home page. Other links,
  official localized Help and user preferences are unchanged.

## 1.0.0-alpha.1 (released 2026-08-29)

Base: eMule Community 0.72a.

- Added GitHub Actions Release builds for Win32 and x64, with isolated
  clean-start, hashing, language-loading, upload-policy and shutdown checks.
- Enabled final-binary CFG, ASLR, DEP, security-cookie and
  architecture-specific Windows hardening, with direct PE verification.
- Added per-package JSON manifests, SPDX 2.3 SBOMs and SHA-256 checksums, with
  archive-content validation before an artifact can be uploaded.
- Replaced the Kad entry in the notification-area quick menu and the legacy
  main-window frame icon with a dedicated scalable copper mule head, without
  changing Kad connection-status symbols.
- Replaced the legacy 0.72 base-version branding in notification-area panels
  and popup-menu headings with the eMule Next public product name and version.
- Replaced the legacy Kad-like notification-area artwork with a scalable eMule
  Next mule head, retaining connected, Low ID and disconnected state colours.
- Modernized the two General-options action buttons, clarified the Italian Web
  services label, created a documented `webservices.dat` on first edit, and
  made eD2K plus collection-file registration complete in one confirmed step.
- Kept the eD2K association action available after registration, added a clear
  check-mark state, shortened its Italian label and stacked both General action
  buttons at full width so longer translations are no longer clipped.
- Modernized the local IP-filter Reload and Edit buttons, prevented Reload from
  discarding the active list when `ipfilter.dat` is missing, and made Edit
  create a documented starter file with visible launch and save errors.
- Replaced MiniMule's fixed 16-pixel GIF artwork with scalable interface
  symbols and separate eD2K/Kad status lights; removed its obsolete tiled GIF
  background from the default layout.
- Replaced fixed 14-pixel list sorting bitmaps with DPI-aware, antialiased
  single and secondary-sort chevrons generated by the modern icon renderer.
- Removed dormant Windows 95/98/Me/NT4 branches from menus, preferences,
  per-user file associations and UPnP handling. Windows XP-era compatibility
  code remains isolated for now, pending a separate minimum-OS decision.
- Removed the embedded low-resolution MiniMule, list-sort, notification and
  Windows XP Luna caption-button artwork now superseded by scalable rendering
  or the active Windows visual style. Source assets remain available for
  historical comparison but are no longer compiled into the application.
- Unified Modern Light and Aurora backgrounds and text colours across shared
  secondary dialogs and property pages, while leaving Classic mode unchanged.
- Refined shared context-menu icon sizing and modern menu-title colours without
  replacing Windows native menu rendering or command behaviour.
- Replaced the 2002-era default notification GIF with a theme-aware rounded
  eMule Next notification card, while retaining optional user skins and the
  existing message history and click actions.
- Refreshed MiniMule with a clean Segoe UI card layout, clearer values and
  modern action feedback without changing its commands or transfer data.
- Modernised the notification-area control panel while preserving every
  command and speed setting: theme-aware surfaces, clearer hover feedback,
  Segoe UI typography, scalable 18-pixel command icons and removal of the
  obsolete etched separators.
- Prepared the first public pre-release as separate Win32 and x64 portable ZIP
  packages and added a cautious Windows SmartScreen guide. The guide explains
  source and SHA-256 verification without asking users to disable protection.
- Moved Bug report from the scrollable lower list area to the fixed upper
  Transfers command row and anchored it to the top-right corner.
- Detect restricted Windows access tokens as well as the legacy
  `eMule_Secure` account when applying the visible copy-to-clipboard fallback
  for external links.
- Reworked the five Kad contact-state icons to retain one stable three-node
  shape while changing only its colour from healthy green through blue,
  yellow and orange to stale red.
- Routed the Bug report control through the Transfers window's legacy
  `OnCommand` dispatcher, which previously consumed the button notification
  before its click handler could run.
- Added a safe external-link fallback for the separate `eMule_Secure`
  Windows account: Help and Bug report links are copied to the shared
  clipboard and displayed instead of silently failing to launch another
  user's browser.
- Unified every Help command on the official multilingual eMule help site;
  context buttons no longer fall back to the obsolete bundled Next guide.
- Fixed the Bug report button to forward the public GitHub Issues page through
  Windows Explorer, allowing the normal user's browser to open even when
  eMule is running with reduced privileges; the standard URL association is
  retained as a fallback.
- Added separate Win32 and x64 Release build support and portable-package
  tooling with all supplied language DLLs.
- Added a distinct eMule Next identity, local help, improved DPI-aware visual
  theme support and updated interface icons.
- Improved Windows long-path handling during sharing, hashing and related disk
  operations to prevent avoidable crashes on deep paths.
- Improved transfer status presentation and connection setup guidance.
- Modernised the connection wizard: speeds are consistently shown and stored
  in KB/s, with international generic connection profiles.
- Modernised advanced defaults for new profiles: safer disk-space checks,
  filesystem-aware sparse files, opt-in media metadata parsing, quieter
  diagnostic logging and a modern half-open connection default.
- Fixed the dynamic upload switch so that disabling it is always respected,
  including when upload capacity is set to unlimited.
- Removed the obsolete Windows XP-only automatic firewall-port setting while
  retaining UPnP and the normal port test.
- Replaced the linked legacy ResizableLib dependency with the independent,
  GPL-2.0-or-later NextResizable layout module; Release and UI validation are
  tracked separately before publication.
- Fixed unstable window repainting in NextResizable: anchored controls are now
  repositioned as one batch, child areas are protected while backgrounds are
  erased, and the complete child hierarchy is invalidated after layout changes.
  The main themed window likewise no longer paints its background over visible
  toolbar, status-bar or page controls.
- Restored the five original progressively illuminated Kad contact icons in
  the Kad contact list and lookup graph, preserving their routing-state meaning.
- Redesigned the Transfers "On Queue" icon as a three-person group and applied
  it consistently to both the download and upload window selectors.
- Removed the inactive automatic-update checkbox and five-day slider from
  General preferences; the manual project update command remains available.
- Added a localized "Report a bug" button to Transfers. It opens the eMule
  Next GitHub issue form without collecting or transmitting client data.
- Retained the original protocol compatibility and upstream licence notices.

## Modified source files

The following representative files include eMule Next changes. Each affected
source file must retain its original copyright and licence header; release
notes and commits must identify further changes made after this record.

- `srchybrid/OtherFunctions.cpp`, `srchybrid/OtherFunctions.h`
- `srchybrid/Preferences.cpp`, `srchybrid/Preferences.h`
- `srchybrid/PPgTweaks.cpp`, `srchybrid/PPgTweaks.h`
- `srchybrid/PartFile.cpp`
- `srchybrid/KnownFile.cpp`, `srchybrid/SharedFileList.cpp`
- `srchybrid/UploadDiskIOThread.cpp`
- `srchybrid/Emule.cpp`, `srchybrid/EmuleDlg.cpp`, `srchybrid/Mdump.cpp`,
  `srchybrid/Mdump.h`, `srchybrid/Wizard.cpp`, `srchybrid/emule.rc`
- `srchybrid/ModernIconRenderer.cpp`, `srchybrid/ModernIconRenderer.h`
- `srchybrid/NextTheme.cpp`, `srchybrid/NextTheme.h`
- `srchybrid/NextResizable/NextResizable.cpp`,
  `srchybrid/NextResizable/NextResizable.h`
- `srchybrid/res/*.manifest`

## Release rule

For every public version, add an entry with the version, date, source tag,
binary checksums, user-visible changes, security fixes and dependency changes.

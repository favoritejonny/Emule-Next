# Fallback replacement plan for restricted third-party components

Status: planning only. Do not start these replacements while written
permission requests are pending unless the project maintainer explicitly
chooses the fallback. The tested release candidate must remain recoverable.

The purpose of this plan is to remove the P. J. Naughter source-distribution
blockers if suitable written permission is not received. Replacement code
must be independently authored under GPL-2.0-or-later from the required
behavior and eMule call sites. Do not copy implementation bodies, comments,
images or private helper structure from the restricted files.

## Phase 1: HTTP download dialog

### Current scope

`CHttpDownloadDlg` has four functional call sites:

- server list download in `ServerWnd.cpp`;
- automatic server list update in `ServerList.cpp`;
- Kad `nodes.dat` update in `KademliaWnd.cpp`;
- IP filter update in `PPgSecurity.cpp`.

Those call sites use a small public surface:

- dialog title;
- source URL;
- destination filename;
- modal success/failure result;
- readable error text for the IP filter path.

### Proposed replacement

Create a new project-owned `CNextHttpDownloadDlg` and update the four call
sites explicitly. Use the Windows WinHTTP API and normal MFC controls. Do not
retain the old class name or implementation.

Required behavior:

- accept only HTTP and HTTPS URLs;
- use Windows certificate validation and TLS policy;
- support normal redirects with a finite redirect limit;
- apply connect, send, receive and overall-operation timeouts;
- display progress when a reliable content length is available;
- remain responsive and allow cancellation;
- write to a temporary file and replace the requested destination only after
  a complete successful download;
- delete partial temporary output after cancellation or failure;
- return a localized, user-readable error without exposing credentials;
- avoid logging URL credentials or sensitive query content;
- handle unknown content length and files larger than 4 GiB safely;
- preserve Win32 and x64 behavior.

### Verification gate

1. Build Debug and Release for Win32 and x64.
2. Test valid HTTP and HTTPS downloads for all four call sites.
3. Test redirect, unreachable host, invalid certificate, timeout, disk-full,
   cancellation and truncated-response behavior.
4. Confirm that a failed download never replaces a valid existing file.
5. Run the normal-profile and portable smoke tests.

This phase is isolated enough to complete before the options-tree work.

## Phase 2: advanced options tree

### Current scope

`CTreeOptionsCtrlEx` is used by three preference pages:

- debug options;
- IRC options;
- advanced/tweak options.

The pages rely on groups, check boxes, radio buttons, edit controls, enable or
disable state, expansion, labels, image-list integration and change
notifications. The existing use map contains approximately 170 calls; the
most frequent operations are checkbox insertion, enable-state changes, edit
boxes, tree items and radio buttons.

### Proposed replacement

Create a new project-owned `CNextOptionsTree` derived from `CTreeCtrl`, with
only the behavior actually required by the three pages. Keep the existing
dialog resources stable where practical, but use new GPL-2.0-or-later source,
new item-state storage and new drawing code.

Implement incrementally:

1. groups, normal items, expansion and notification plumbing;
2. check boxes and enabled/disabled state;
3. radio-button groups and keyboard accessibility;
4. edit boxes and value synchronization;
5. theme-aware drawing, DPI scaling and image-list support;
6. migrate Debug, then IRC, then Tweaks, testing each page before continuing;
7. remove the restricted files only after all three pages pass.

Do not use `CMFCPropertyGridCtrl` for the release fallback unless the smaller
compatible control proves impractical. It is a possible later redesign, but
would create a larger interface change immediately before release.

### Verification gate

For every migrated page:

- compare every displayed option and default value with the tested candidate;
- verify mouse, keyboard, focus, scrolling, resizing, DPI and all three themes;
- apply, cancel, reopen and restart the client to confirm persistence;
- test disabled parent/child options and mutually exclusive radio groups;
- build and smoke-test both Win32 and x64;
- confirm normal-profile and portable settings remain separate as intended.

## Decision rule

- If both written permissions clearly cover public modified source and binary
  redistribution under GPL-2.0-or-later, retain the tested implementation and
  archive the replies with the release records.
- If either permission is refused, unclear or unanswered after a reasonable
  period, use the corresponding replacement phase before publication.
- Do not upload the blocked source or binaries merely because a response is
  delayed.

This plan reduces publication risk without changing the already tested client
while permission requests are pending. It is a technical plan, not legal
advice; final clearance may still require qualified legal review.

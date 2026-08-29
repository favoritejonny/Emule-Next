# Publication status - 2026-08-29

## Candidate tested

The current tested candidate is RC1-fix9:

- Win32 portable ZIP SHA-256:
  `FB01274CB7FF59FD3116E93024232370664331A36FC6E921C0A24934173F611D`
- x64 portable ZIP SHA-256:
  `1DAA4E16B20A3BF38F97A74F9FD3F93017726E6B853D58CD802714AA3639C3E0`

Both portable packages passed automated structure checks and manual launch
testing. The normal-profile Win32 and x64 clients were also tested for about
12 hours each without reported anomalies.

## Resolved upstream items

P. J. Naughter granted written permission on 2026-08-29 to release the
modified modules identified in the project's request under GPL-2.0-or-later
in the eMule Next application. This resolves the TreeOptionsCtrl and
HttpDownloadDlg source-distribution blocker. The original email is retained
privately; the public permission record is in
`docs/PERMISSION_REQUEST_PJ_NAUGHTER.md`.

The official eMule Project DevTeam also provided a written clarification on
2026-08-29. It considers source-file artwork licensed under GPL 2.0 and
confirmed that the customary `eMule [own name]` naming convention is fine for
forks that do not pretend to be the official/original eMule. It stated no
intention to enforce trademark rights against such forks, while noting that
implicit rights might exist and not claiming certainty about current formal
registration. `eMule Next` will retain a clear independent/unofficial
disclaimer. The original email is retained privately; the public record is in
`docs/PERMISSION_REQUEST_EMULE_PROJECT.md`.

## Source publication completed

The final privacy, credential and integrity audit completed with a clean
working tree. On 2026-08-29, the reviewed source history was published to the
public repository with a normal, non-forced push. Local and remote `main` were
verified at `1626ae05724530939c4724d83b53367cdac2497d` immediately after the
push.

## Remaining publication steps

1. Regenerate the final portable archives so that they contain the corrected
   legal documents, then verify and briefly smoke-test them.
2. Create the immutable source tag on the exact rebuilt revision and publish
   the two portable ZIPs and checksum file as a GitHub pre-release.

No public binary should be uploaded before these items are complete.

## Repository safety setup

GitHub private vulnerability reporting was enabled by the repository owner on
2026-08-29. The prepared `SECURITY.md` and issue-template contact link will use
that private channel after the source is published.

# Publication status - 2026-08-29

## Final candidate generated

The final local candidate was rebuilt from public revision
`27a14542ef7d02785c83a79e908d7685faa55591`:

- Win32 portable ZIP SHA-256:
  `8893FDB7AD545FFD9AED892F0FD749B3550F10C10AB5842BCF576DC90D7B54D1`
- x64 portable ZIP SHA-256:
  `6A2FB77DB00FAC08FE83A8A42AFCE3EA80C7F3CC155202EC0A905B0F1362B455`

Both final portable packages passed complete automated structure, content,
architecture and checksum checks. The project tester then extracted and tested
both newly generated final ZIPs and reported both working. The earlier
RC1-fix9 portable packages also passed manual testing, and the normal-profile
Win32 and x64 clients were tested for about 12 hours each without reported
anomalies.

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
first verified at `1626ae05724530939c4724d83b53367cdac2497d`. Local and remote
`main` now match at `27a14542ef7d02785c83a79e908d7685faa55591`, the revision
used for the final binary rebuild.

## Publication completed

The first public pre-release was published on 2026-08-29:

`https://github.com/favoritejonny/Emule-Next/releases/tag/v1.0.0-alpha.1`

GitHub identifies it as a pre-release. Tag `v1.0.0-alpha.1` resolves to the
exact build revision `27a14542ef7d02785c83a79e908d7685faa55591`. The public
assets are the verified Win32 and x64 portable ZIPs and the matching SHA-256
file; GitHub supplies the two corresponding source archives from the tag.

## Repository safety setup

GitHub private vulnerability reporting was enabled by the repository owner on
2026-08-29. The prepared `SECURITY.md` and issue-template contact link will use
that private channel after the source is published.

# Legal release status

Status as of 2026-08-29: **the identified upstream licence, name and artwork
issues are documented as resolved for the planned independent non-commercial
GPL pre-release. The reviewed source history is public, and the final Win32
and x64 rebuild, packaging checks and manual launch tests passed. The exact
source tag and GitHub pre-release were published on 2026-08-29.**

The project now has a GPL licence file, attribution, third-party inventory,
privacy statement, source-distribution procedure and release checklist. That
does not by itself resolve all third-party licence questions.

## ResizableLib migration: implementation complete

The legacy `ResizableLib` was previously linked into the executable and is
distributed under the original Artistic License 1.0
(`ResizableLib/Artistic-License.txt`). Its terms are not straightforwardly
compatible with treating the complete linked work as GPL-2.0-or-later.

On 2026-08-20, the eMule Next project was changed to use the independently
authored, GPL-2.0-or-later `srchybrid/NextResizable` module instead. The legacy
static library was removed from all eMule Next link configurations and from the
solution dependencies. On 2026-08-20, both clean Release builds completed with
zero errors (x64: 1,343 historical warnings; Win32: 1,271 historical warnings).
After the project tester reported the changed windows functioning, the unused
legacy source directory was removed from the eMule Next release tree. A local
recovery copy is kept outside the project tree and must not be added to a
source archive or repository.

The replacement removes this specific linked-license concern. This is not yet
a general public-release clearance: the final release verification remains
necessary.

The four files still present under `srchybrid/ResizableLib` are newly written
GPL forwarding headers, not the removed Artistic implementation. They are
needed only to avoid a risky all-at-once include-path rewrite.

Manual testing now covers both installed-profile Release executables for about
12 hours each and both RC1-fix9 portable packages. The tester reported no
anomalies. The local recovery copy is outside this project and the root
`.gitignore` excludes build output and release packages.

## Resolved third-party permission

- **P. J. Naughter components:** `TreeOptionsCtrl.cpp` and `.h` identify
  themselves as v1.74. `HttpDownloadDlg.cpp` and `.h` are an older eMule-
  adapted implementation. The retained terms allow use in a binary but
  restrict source distribution to author-released versions. This is not a
  clear GPL-compatible grant for a public fork by itself, and the HTTP dialog
  is visibly adapted for eMule. On 2026-08-29, in response to the request that
  expressly identified all four files, P. J. Naughter granted written
  permission to release the modified modules under GPL-2.0-or-later in the
  eMule Next application. The original email is retained privately and the
  public record is in `docs/PERMISSION_REQUEST_PJ_NAUGHTER.md`. This blocker is
  resolved. Do not modify the copyright blocks or remove attribution.

## Resolved name and source-artwork clarification

- **eMule Next name and artwork:** on 2026-08-29 the official eMule Project
  administrative/legal contact replied to the project's written request. The
  DevTeam explained that most requested use is already covered by GPL 2.0,
  considers artwork included in the source files licensed under GPL 2.0, and
  confirmed that the customary `eMule [own name]` convention is fine. It did
  not claim certainty that an official registered trademark still exists and
  noted that implicit rights might remain, but stated that it does not intend
  to enforce trademark rights against forks that do not pretend to be the
  official/original eMule. `eMule Next` follows that convention and every
  public surface must retain a clear independent/unofficial disclaimer. The
  original response is retained privately and the public record is in
  `docs/PERMISSION_REQUEST_EMULE_PROJECT.md`.

## Required verification before clearing a release

- **Repository:** the reviewed source history was published to the public
  GitHub repository on 2026-08-29 with a normal, non-forced push. The first
  verified public revision was
  `1626ae05724530939c4724d83b53367cdac2497d`, using the maintainer's GitHub
  no-reply identity. The final binaries were rebuilt from public revision
  `27a14542ef7d02785c83a79e908d7685faa55591`. The public immutable tag
  `v1.0.0-alpha.1` points to that exact revision and was published with the
  corresponding Win32/x64 portable binaries and checksum file.
- Re-audit all linked dependencies after their versions are frozen.
- Retain the restored full miniupnpc BSD-3-Clause text.
- Have a lawyer experienced in copyright and open-source licensing review a
  commercial launch, monetisation, hosted services or data collection.

This status is deliberately conservative. The written upstream responses
resolve the specific issues identified for the planned independent
non-commercial GPL fork, but they are not a universal trademark clearance or
legal advice for future commercial, merchandising, hosted-service or
data-collection activity.

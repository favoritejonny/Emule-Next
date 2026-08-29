# Legal release status

Status as of 2026-08-29: **tested release candidate prepared; public-release
clearance is still blocked.**

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

## Required verification before clearing a release

- **Project name:** the official eMule Project reported in 2004 that the eMule
  trademark was transferred to the eMule developers. The current owner and
  permitted use of `eMule Next` have not yet been verified in the relevant
  registers. Obtain permission or specialist advice, or adopt a name that does
  not use `eMule`, before a public launch. On 2026-08-29 the official site's
  administrative/legal contact route was confirmed and the written permission
  request in `docs/PERMISSION_REQUEST_EMULE_PROJECT.md` was sent. A written
  response is pending and no permission is assumed.
- **Repository:** the intended GitHub repository is now public but empty. This
  working folder was initialized as a local Git repository on 2026-08-29 and
  contains a reviewed clean initial commit using the maintainer's GitHub
  no-reply identity. Do not push the blocked source yet. A public immutable
  source tag must exist before publishing binaries.
- Re-audit all linked dependencies after their versions are frozen.
- Retain the restored full miniupnpc BSD-3-Clause text.
- Have a lawyer experienced in copyright and open-source licensing review a
  commercial launch, monetisation, hosted services or data collection.

This status is deliberately conservative. It prevents us from falsely claiming
that the project is legally ready while a material upstream condition remains
unresolved.

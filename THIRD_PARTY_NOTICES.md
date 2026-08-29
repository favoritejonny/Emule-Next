# Third-party notices

This inventory covers components linked by `srchybrid/emule.vcxproj` and
notable source components embedded in the client. It must be reviewed whenever
a dependency, compiler setting or packaged file changes.

## Linked components

| Component | Licence selected for this distribution | Copyright / notice source |
| --- | --- | --- |
| eMule Community core | GPL-2.0-or-later | Source-file headers in `srchybrid/`; [LICENSE](LICENSE) |
| Crypto++ | Boost Software License 1.0; individual public-domain contributions are listed by upstream | `cryptopp/License.txt` |
| id3lib | GNU Library General Public License v2 | `id3lib/COPYING` |
| Mbed TLS | GPL-2.0-or-later option of its dual licence | `mbedtls/LICENSE` |
| miniupnpc | BSD 3-Clause | `miniupnpc/LICENSE` |
| NextResizable | GPL-2.0-or-later, in-tree independent implementation | `srchybrid/NextResizable/` |
| zlib | zlib licence | `zlib/LICENSE` |

Mbed TLS is deliberately distributed under its GPL-2.0-or-later alternative,
not under Apache-2.0, for this GPL-2.0-or-later code base.

## Embedded source components

The source tree also carries notices for, among others, AsyncSocketEx
(Tim Kosse / FileZilla), tree property-sheet code (Sven Wiegand), and code
originating from Shareaza. Their copyright and licence notices are retained in
the corresponding source files and must never be removed.

`TreeOptionsCtrl.cpp`, `TreeOptionsCtrl.h`, `HttpDownloadDlg.cpp` and
`HttpDownloadDlg.h` originate from P. J. Naughter. The author's published
terms allow binary use and permit source redistribution only for versions
released by the author. Those terms are more restrictive than
GPL-2.0-or-later for modified source distribution. These files therefore
remain public-release blockers until the project has written GPL-compatible
permission from the author or replaces the components. The separate eMule
extension files `TreeOptionsCtrlEx.cpp` and `TreeOptionsCtrlEx.h` retain their
GPL notices.

## Distribution requirements

When distributing binaries, include this file, [NOTICE.md](NOTICE.md),
[LICENSE](LICENSE), and the complete licence texts from the linked component
paths above. The portable packaging script copies these materials into each
new archive.

## Resizable implementation

The former Artistic License 1.0 ResizableLib implementation has been removed.
The `srchybrid/ResizableLib` directory now contains only four small
GPL-2.0-or-later forwarding headers kept for source compatibility; all resize
behaviour is implemented by the in-tree GPL `NextResizable` module.

This inventory is a technical record, not legal advice.

# eMule Next 1.0.0-alpha.1 - published pre-release record

Preparation date: 2026-08-29  
Build source revision: `27a14542ef7d02785c83a79e908d7685faa55591`

This is the record of the first public eMule Next pre-release. It supersedes
RC1-fix9 for publication. On 2026-08-29, the project tester extracted and
tested both final portable archives and reported both working. The verified
archives were then published without modification.

## Release builds

- Win32 Release: completed with 0 errors and 1,360 warnings.
- x64 Release: completed with 0 errors and 1,589 warnings.
- Embedded file and product version: `1.0.0-alpha.1`.
- Distributable translations in each package: 43.
- Both executables are unsigned.
- The remaining warnings are predominantly from historical code, bundled
  dependencies and current Windows/ATL/MFC headers.

The builds were produced sequentially from a clean working tree with Visual
Studio 18, MSVC 14.51 and Windows SDK 10.0.28000.0. The complete logs are in
`srchybrid/build-logs`.

## Final portable archives

Local output directory:

`dist/prerelease/1.0.0-alpha.1-final-20260829-171006`

- `eMuleNext-1.0.0-alpha.1-win32-portable.zip`
  - size: 4,637,454 bytes
  - SHA-256: `8893FDB7AD545FFD9AED892F0FD749B3550F10C10AB5842BCF576DC90D7B54D1`
- `eMuleNext-1.0.0-alpha.1-x64-portable.zip`
  - size: 5,076,787 bytes
  - SHA-256: `6A2FB77DB00FAC08FE83A8A42AFCE3EA80C7F3CC155202EC0A905B0F1362B455`
- `SHA256SUMS-1.0.0-alpha.1.txt`

## Automated verification

Both archives were opened and every entry was read successfully. Each archive
contains 59 entries, including the correct executable, the portable marker,
43 language DLLs, user documentation, GPL notices and third-party licences.

The executable in each ZIP is byte-for-byte identical to its corresponding
Release build. PE architecture checks report `0x014C` for Win32 and `0x8664`
for x64. The packages contain no user configuration, profile, dump, PDB or log
files. `SOURCE-CODE.md` points to the public eMule Next repository, and the
recorded checksum file matches both archives.

## Publication record

- Public release:
  `https://github.com/favoritejonny/Emule-Next/releases/tag/v1.0.0-alpha.1`
- Immutable source tag: `v1.0.0-alpha.1`.
- Tagged commit: `27a14542ef7d02785c83a79e908d7685faa55591`.
- Published assets: the Win32 portable ZIP, x64 portable ZIP and checksum
  file listed above. GitHub also supplies source ZIP and tarball archives for
  the tag.
- GitHub displays the release as a pre-release, not a production release.

# GitHub pre-release draft

Status: **ready for the final GitHub publication step.** The final archives
passed automated verification and manual testing. Publish them only with the
immutable tag that points to the exact build revision.

## GitHub fields

- Tag: `v1.0.0-alpha.1`
- Target: exact approved commit on `main`
- Release title: `eMule Next 1.0.0-alpha.1`
- Mark as pre-release: **yes**
- Mark as latest release: **no**
- Create a discussion: **no**, unless moderation is ready

## Draft release notes

### eMule Next 1.0.0-alpha.1

This is the first public pre-release of eMule Next, an independent community
Windows client derived from eMule Community. It is not an official eMule
Project release and is not affiliated with, endorsed by or sponsored by the
original eMule Project.

The pre-release focuses on Windows build reliability, Win32/x64 packaging,
interface modernization, high-DPI behavior, safer defaults, crash diagnostics
and preservation of eD2K and Kad compatibility.

#### Downloads

Choose exactly one portable ZIP:

- `eMuleNext-1.0.0-alpha.1-win32-portable.zip` for 32-bit Windows or maximum
  compatibility;
- `eMuleNext-1.0.0-alpha.1-x64-portable.zip` for 64-bit Windows.

These packages are portable archives, not installers. Extract the selected
ZIP into a writable folder and run `eMuleNext.exe`. Do not run both packages
against the same configuration folder at the same time.

#### Verify the download

Download `SHA256SUMS-1.0.0-alpha.1.txt` from this release and compare the
SHA-256 value before running the program.

- Win32: `8893FDB7AD545FFD9AED892F0FD749B3550F10C10AB5842BCF576DC90D7B54D1`
- x64: `6A2FB77DB00FAC08FE83A8A42AFCE3EA80C7F3CC155202EC0A905B0F1362B455`

The final source for these exact binaries is the immutable
[`v1.0.0-alpha.1`](https://github.com/favoritejonny/Emule-Next/tree/v1.0.0-alpha.1)
tag.

#### Windows SmartScreen

The alpha packages are not digitally signed, so Windows may show a SmartScreen
warning. This warning does not by itself prove that a file is unsafe. Verify
that the archive came from this repository, compare its SHA-256 checksum and
scan it with your security software. Continue only if those checks succeed.
Do not disable SmartScreen or antivirus protection globally. See the included
`WINDOWS-SMARTSCREEN.md` for details.

#### Important limitations

- This is an alpha pre-release intended for careful testing.
- Only Win32 and x64 portable packages are provided; there is no installer or
  ARM64 build in this release.
- The executables are not digitally signed.
- Back up the existing eMule configuration before testing and do not point two
  running clients at the same configuration folder.
- Crash dumps may contain private memory. Do not attach dumps to public
  issues.

#### Reporting problems

Use the guided **Bug report** form and remove usernames, IP addresses, search
terms, shared-file names and local paths. Report security vulnerabilities only
through GitHub's private vulnerability reporting on the repository Security
page.

#### Lawful use

This is a general-purpose peer-to-peer client. Download and share only
material that you are authorised to receive or distribute. The project does
not provide or endorse copyrighted downloads or unverified server lists.

#### Acknowledgements and licence

eMule Next is derived from eMule and remains available under
GPL-2.0-or-later, subject to the notices for included third-party components.
See `LICENSE`, `NOTICE.md`, `THIRD_PARTY_NOTICES.md`, `PRIVACY.md` and
`SOURCE-CODE.md` in the tagged source and binary packages.

## Final publication checks for this text

- Remove this local status paragraph from the public release notes.
- Confirm the tag link resolves publicly to the exact build revision.
- Paste hashes from the newly generated final checksum file, not RC hashes.
- Confirm uploaded filenames exactly match the names shown above.
- Attach only the two portable ZIPs and checksum file; GitHub supplies source
  archives for the tag automatically.
- Re-read the complete release page while signed out before announcing it.

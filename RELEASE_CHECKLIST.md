# Public release checklist

Complete every item before publishing a Win32, x64 or portable package.

## Legal and identity

- [ ] Resolve every blocker in [LEGAL_STATUS.md](LEGAL_STATUS.md).
- [ ] Confirm the eMule Next name, logo and release-page wording do not create
      a trademark conflict or imply upstream affiliation.
- [ ] Keep [LICENSE](LICENSE), [NOTICE.md](NOTICE.md),
      [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md),
      [CHANGES.md](CHANGES.md), [PRIVACY.md](PRIVACY.md) and
      [SOURCE-CODE.md](SOURCE-CODE.md) in the source tag and binary packages.
- [ ] Include [WINDOWS-SMARTSCREEN.md](WINDOWS-SMARTSCREEN.md) in each unsigned
      binary package and state clearly in the release notes that it is unsigned.
- [ ] Check every new or upgraded dependency and record its licence here.
- [x] Confirm the released source tree no longer contains the unused legacy
      ResizableLib implementation and that the executable links only the
      in-tree GPL NextResizable module.

## Source and binaries

- [ ] Create an immutable Git tag for the exact source revision.
- [ ] Build Win32 and x64 Release sequentially from that tag.
- [x] Test both executables manually without overwriting an existing user
      profile.
- [ ] Resize and restart each main resizable window, including Transfers,
      Search, Servers, Shared Files, Preferences, file details and the
      first-run wizard.
- [x] Create portable archives and SHA-256 checksums.
- [ ] Publish the complete corresponding source from the same tag in the same
      release location as the binaries.

## Security and privacy

- [ ] Record resolved security issues and known limitations in the release
      notes.
- [ ] Verify the release contains no private test data, crash dumps, personal
      paths, tokens or passwords.
- [ ] If data collection was added, update [PRIVACY.md](PRIVACY.md) before the
      release and obtain specialist review where required.

## Publication

- [ ] Link the release notes to the exact source tag and checksum file.
- [ ] Publish only the Win32 and x64 portable ZIPs for the first pre-release;
      do not describe them as installers.
- [ ] Use neutral wording: users must download and share only material they
      are authorised to receive or distribute.
- [ ] Retain the release archive, source archive, build log and checksums for
      reproducibility.

## Test record

- Win32 and x64 normal-profile builds: approximately 12 hours of testing each;
  no anomaly reported.
- RC1-fix9 Win32 and x64 portable packages: both launched and tested by the
  project tester; both reported working.
- Automated RC1-fix9 archive verification: readable ZIPs, 59 entries and 43
  translations each, correct PE architecture, no user configuration, dumps or
  PDB files, and executable hashes matching the tested Release builds.

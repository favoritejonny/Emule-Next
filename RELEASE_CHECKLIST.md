# Public release checklist

Complete every item before publishing a Win32, x64 or portable package.

## Legal and identity

- [x] Resolve every identified upstream blocker in
      [LEGAL_STATUS.md](LEGAL_STATUS.md) for this independent GPL pre-release.
- [x] Record the official eMule DevTeam's written clarification for the
      `eMule [own name]` convention and source artwork, and keep the independent
      non-official disclaimer on the release page and in [NOTICE.md](NOTICE.md).
- [x] Keep [LICENSE](LICENSE), [NOTICE.md](NOTICE.md),
      [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md),
      [CHANGES.md](CHANGES.md), [PRIVACY.md](PRIVACY.md) and
      [SOURCE-CODE.md](SOURCE-CODE.md) in the source tag and binary packages.
- [x] Include [WINDOWS-SMARTSCREEN.md](WINDOWS-SMARTSCREEN.md) in each unsigned
      binary package and state clearly in the release notes that it is unsigned.
- [x] Check every bundled dependency and record its licence in
      [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) and the package licence
      directory.
- [x] Confirm the released source tree no longer contains the unused legacy
      ResizableLib implementation and that the executable links only the
      in-tree GPL NextResizable module.
- [x] Retain P. J. Naughter's written GPL-2.0-or-later permission for the
      modified TreeOptionsCtrl and HttpDownloadDlg modules, preserve their
      copyright notices, and document the public permission record.

## Source and binaries

- [x] Create an immutable Git tag for the exact source revision.
- [x] Build Win32 and x64 Release sequentially from the exact revision now
      identified by that tag.
- [x] Test both executables manually without overwriting an existing user
      profile.
- [ ] Resize and restart each main resizable window, including Transfers,
      Search, Servers, Shared Files, Preferences, file details and the
      first-run wizard.
- [x] Create portable archives and SHA-256 checksums.
- [x] Publish the complete corresponding source from the same tag in the same
      release location as the binaries.

## Security and privacy

- [x] Enable GitHub private vulnerability reporting and document the private
      reporting route in [SECURITY.md](SECURITY.md).
- [x] Record resolved security issues and known limitations in the release
      notes.
- [x] Verify the release contains no private test data, crash dumps, personal
      paths, tokens or passwords.
- [ ] If data collection was added, update [PRIVACY.md](PRIVACY.md) before the
      release and obtain specialist review where required.

## Publication

- [x] Link the release notes to the exact source tag and checksum file.
- [x] Publish only the Win32 and x64 portable ZIPs for the first pre-release;
      do not describe them as installers.
- [x] Use neutral wording: users must download and share only material they
      are authorised to receive or distribute.
- [x] Retain the release archive, source archive, build log and checksums for
      reproducibility.
- [x] Prepare guided bug, feature, pull-request and security-reporting
      templates for the public repository.

## Test record

- Win32 and x64 normal-profile builds: approximately 12 hours of testing each;
  no anomaly reported.
- RC1-fix9 Win32 and x64 portable packages: both launched and tested by the
  project tester; both reported working.
- Automated RC1-fix9 archive verification: readable ZIPs, 59 entries and 43
  translations each, correct PE architecture, no user configuration, dumps or
  PDB files, and executable hashes matching the tested Release builds.
- Final 1.0.0-alpha.1 rebuild on 2026-08-29: Win32 and x64 completed
  sequentially with zero errors. Automated final archive verification passed:
  59 entries and 43 translations each, correct PE architecture, no user data,
  dumps, logs or PDB files, valid checksums, and executable hashes matching the
  fresh Release builds. The project tester then extracted and tested both final
  portable ZIPs and reported both working.
- Public GitHub pre-release published on 2026-08-29 as `v1.0.0-alpha.1`. The
  tag resolves to the exact build commit `27a14542ef7d02785c83a79e908d7685faa55591`;
  GitHub shows the two verified portable archives, checksum file and tagged
  source archives.

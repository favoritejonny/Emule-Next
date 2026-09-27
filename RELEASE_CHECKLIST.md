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
- [x] Record that the SignPath application was not accepted for Beta 1 because
      the project did not yet have enough public reputation. Publish this Beta
      as unsigned and state that fact in the release notes and SmartScreen guide.
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
- [x] Build Win32 and x64 Release sequentially from the exact code content
      identified by the release tag; only release documentation was finalised
      after the successful build and tests.
- [x] Test both executables manually without overwriting an existing user
      profile.
- [x] Resize and restart each main resizable window, including Transfers,
      Search, Servers, Shared Files, Preferences, file details and the
      first-run wizard.
- [x] Create portable archives and SHA-256 checksums.
- [x] Generate the final manifests, SBOMs and SHA-256 checksums after the
      unsigned status and final tested binaries have been verified.
- [ ] Confirm the GitHub Actions Win32 and x64 jobs passed for the exact release
      commit and retain their build/test logs.
- [x] Verify the PE security report passes for both architectures: ASLR, DEP,
      CFG with a non-empty function table, security cookie, x64 high-entropy
      ASLR and Win32 large-address awareness.
- [x] Include and validate the external and embedded file manifest and SPDX 2.3
      SBOM for both portable archives.
- [x] Publish the complete corresponding source from the same tag in the same
      release location as the binaries.

## Security and privacy

- [x] Enable GitHub private vulnerability reporting and document the private
      reporting route in [SECURITY.md](SECURITY.md).
- [x] Record resolved security issues and known limitations in the release
      notes.
- [x] Verify the release contains no private test data, crash dumps, personal
      paths, tokens or passwords.
- [x] Compile the public release with private diagnostics disabled, verify that
      no diagnostic CSV or identifying test profile is packaged, and keep the
      no-project-telemetry statement in [PRIVACY.md](PRIVACY.md).
- [x] Run the isolated automated checks for clean first start, MD4/eD2K hashing,
      43 languages, upload regression rules and clean shutdown on Win32 and x64.
- [x] Separately test real eD2K/Kad connectivity, sustained upload/download,
      firewall prompts and first-run wizard visuals; automated smoke tests do
      not replace these network and interface checks.

## Publication

- [x] Link the release notes to the exact source tag and checksum file.
- [x] Publish only the Win32 and x64 portable ZIPs for the first pre-release;
      do not describe them as installers.
- [x] Do not publish Beta 1 until both explicitly unsigned portable packages
      have passed the full automated suite and manual clean-start test.
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
- Final 1.0.0-beta.1 Win32 and x64 builds on 2026-09-27 completed sequentially
  with zero errors. The automated suite passed clean start, language fallback,
  43 translations, hashing vectors, upload rules, router-mapping policy,
  wizard layout, server bootstrap, clean shutdown and PE hardening checks.
  Both final portable copies were then tested manually and reported working.
  Archive verification found 61 files per architecture, matching embedded and
  external manifests/SBOMs, no private diagnostics or user profile, and 6,682
  individually verified files in the corresponding-source snapshot.

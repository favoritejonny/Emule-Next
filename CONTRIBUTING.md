# Contributing to eMule Next

Thank you for helping improve the project. Contributions must preserve the
stability, network compatibility, privacy and licence obligations of the
client.

## Before starting

1. Search existing issues and pull requests to avoid duplicate work.
2. Open an issue before a large interface, protocol, dependency or packaging
   change.
3. Keep each change focused on one problem.
4. Do not include copyrighted media, server lists of uncertain provenance,
   credentials, personal configuration files, build output or generated
   binaries.

## Building

The main Visual Studio solution is `srchybrid/emule.sln`. Supported release
targets are Win32 and x64. Build instructions and the last known build status
are recorded in `BUILD_STATUS.md`.

On Windows, clone into a short path such as `C:\src\Emule-Next`. Some complete
dependency-source paths become longer than the legacy Windows path limit when
the repository is placed inside a deeply nested directory. Git for Windows
may also require long-path support for a deep checkout.

Do not commit Visual Studio user state, caches, executables, libraries, crash
dumps or release archives. The root `.gitignore` covers the normal generated
paths.

## Source changes

- Preserve existing copyright and licence notices.
- New project code must be compatible with GPL-2.0-or-later.
- Avoid adding a new dependency without documenting its exact version,
  source, licence and notices in `THIRD_PARTY_NOTICES.md`.
- Keep eD2K and Kad protocol compatibility unless a separately reviewed
  migration plan says otherwise.
- Do not add telemetry, analytics, advertising or automatic data submission.
- Never weaken security or privacy controls to bypass network operators,
  access controls or applicable law.
- Keep Win32 and x64 behavior aligned.

## Testing

For a source change, report:

- configuration and platform built;
- compiler and Windows version;
- checks performed;
- any warnings newly introduced;
- manual behavior tested;
- whether normal-profile and portable operation were affected.

Interface changes should include before/after screenshots with personal data,
usernames, IP addresses and shared-file names removed.

## Pull requests

A pull request should explain the problem, the smallest implemented solution,
testing evidence and any user-visible effect. It must not mix unrelated
cleanup with a functional change. Reviewers may request a smaller patch or
additional Win32/x64 testing before acceptance.

By submitting a contribution, you confirm that you have the right to provide
it under the project's applicable licences and that it does not contain
third-party material without compatible permission.

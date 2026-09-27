# SignPath Foundation setup for eMule Next

The SignPath Foundation application was not accepted for Beta 1 because the
project did not yet have enough public reputation. Beta 1 is therefore
published unsigned, with an explicit SmartScreen warning, complete source,
manifests, SPDX SBOMs and SHA-256 checksums. This workflow remains available
for a future application; no release may be described as signed until its
actual Authenticode signatures and timestamps have been verified.

## Application information

- Project: eMule Next
- Repository: https://github.com/favoritejonny/Emule-Next
- Download page: https://github.com/favoritejonny/Emule-Next/releases
- Website: https://favoritejonny.github.io/Emule-Next/
- License: GPL-2.0-or-later
- Code signing policy:
  https://github.com/favoritejonny/Emule-Next/blob/main/CODE_SIGNING_POLICY.md
- Build system: GitHub Actions on windows-2022, Visual Studio 2022, Win32
  and x64 Release configurations
- Artifact: portable ZIP containing eMuleNext.exe, 43 language DLLs,
  documentation, license notices, manifest and SPDX 2.3 SBOM

Suggested project description for the application:

> eMule Next is an independent GPL-2.0-or-later Windows fork of eMule
> Community. It preserves eD2K and Kad interoperability while modernising the
> interface, first-run setup, port mapping, build security and large-library
> handling. The project publishes portable Win32 and x64 packages with 43
> languages, reproducible GitHub Actions builds, SHA-256 checksums, file
> manifests and SPDX SBOMs. It contains no project telemetry or automatic
> crash-report upload.

## Repository requirements

Before submitting the application:

1. publish CODE_SIGNING_POLICY.md and the README link;
2. enable GitHub two-factor authentication for every signing team member;
3. keep the repository, build workflow, release and corresponding source
   publicly accessible;
4. do not store a SignPath token, certificate or password in the repository;
5. retain manual approval for every release signing request.

## Values supplied after approval

The SignPath release-candidate workflow is deliberately disabled until the
repository variable SIGNPATH_ENABLED is set to true. After approval, add these
values under Settings > Secrets and variables > Actions:

- secret SIGNPATH_API_TOKEN;
- variable SIGNPATH_ORGANIZATION_ID;
- variable SIGNPATH_PROJECT_SLUG;
- variable SIGNPATH_SIGNING_POLICY_SLUG;
- variable SIGNPATH_ARTIFACT_CONFIGURATION_SLUG;
- variable SIGNPATH_ENABLED with value true, only after all other values
  have been checked.

The SignPath artifact configuration must preserve the submitted directory
layout and apply Authenticode signing plus a trusted timestamp to exactly:

- Win32/eMuleNext.exe;
- Win32/lang/*.dll (43 files);
- x64/eMuleNext.exe;
- x64/lang/*.dll (43 files).

No configuration, user data, download, temporary file or locally supplied
binary is part of the signing input.

## Release sequence

1. Commit and push the complete beta candidate.
2. Wait for the ordinary Windows build workflow to pass.
3. Run SignPath release candidate manually and enter the release version.
4. Approve the request in SignPath after checking the source commit.
5. Download and test the signed workflow artifact.
6. Verify the Authenticode signer, trusted timestamp, package manifest, SBOM
   and SHA-256 values.
7. Create the immutable source tag and publish those exact signed packages.


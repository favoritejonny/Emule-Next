# Code signing policy

Free code signing provided by [SignPath.io](https://signpath.io/), certificate
by [SignPath Foundation](https://signpath.org/).

This policy applies to Windows release artifacts published by the eMule Next
project. The public 1.0.0-alpha.1 packages predate this policy and are
unsigned. A future release is described as signed only after every executable
and language DLL in both portable packages has passed the verification below.

## Team roles

- Committer and reviewer: [favoritejonny](https://github.com/favoritejonny)
- Signing approver: [favoritejonny](https://github.com/favoritejonny)

Contributions from people without direct commit access must be reviewed before
they are merged. Every public release signing request requires manual approval.
All accounts with commit, review or signing authority must use multi-factor
authentication on GitHub and SignPath.

## Build and signing process

1. A release candidate is built from a specific commit in the public
   [eMule Next repository](https://github.com/favoritejonny/Emule-Next) by a
   GitHub-hosted Windows runner.
2. Win32 and x64 Release builds must pass the project's binary-security and
   isolated functional tests.
3. Only the executable and the 43 language DLLs produced by that workflow are
   submitted to SignPath. Interactive uploads and locally substituted binaries
   are not accepted by the release process.
4. The signing request is checked and manually approved by the signing
   approver. SignPath applies Authenticode signatures and trusted timestamps.
5. The workflow verifies every returned signature before creating the final
   portable packages, embedded manifests, SPDX SBOMs and SHA-256 checksum file.
6. The signed packages are tested again and are published only from the source
   tag that produced them.

The signing workflow remains disabled until SignPath Foundation accepts the
project and the required repository secret and variables have been configured.

## Privacy

eMule Next does not operate telemetry, analytics, advertising identifiers or
automatic crash-report uploads. It transfers network data only when requested
by the user or required for features that the user explicitly enables, such as
eD2K/Kad connectivity, the optional first-run speed test and the optional
server-list download. The complete description, including the independent
services involved, is in [PRIVACY.md](PRIVACY.md).

## Verification

For a signed release, Windows PowerShell must report Valid and identify
SignPath Foundation for every eMuleNext.exe and lang/*.dll file:

~~~powershell
Get-AuthenticodeSignature .\eMuleNext.exe | Format-List Status,StatusMessage,SignerCertificate,TimeStamperCertificate
Get-ChildItem .\lang\*.dll | Get-AuthenticodeSignature | Format-Table Path,Status
~~~

The SHA-256 checksum distributed with the release verifies the complete ZIP.
Checksums are generated only after the signed files have been returned and
validated; pre-signing hashes are never published as final release hashes.

## Compromise or signing failure

If a signing credential, workflow or release artifact is suspected of being
compromised, publication stops immediately. The maintainer will contact
SignPath, request revocation where appropriate, preserve the affected commit
and workflow evidence, and publish a security notice through the repository's
private vulnerability-reporting process described in [SECURITY.md](SECURITY.md).

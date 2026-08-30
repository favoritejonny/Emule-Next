# Windows build, security and automated checks

Every push to `main`, every pull request and every manual workflow run builds
the Release configuration for both Win32 and x64 on GitHub Actions. Each build
must pass the checks below before its portable package is uploaded as a
short-lived workflow artifact.

## Binary hardening

The shared native-build defaults enable:

- compiler SDL checks and Control Flow Guard instrumentation;
- linker Control Flow Guard, ASLR and DEP/NX compatibility;
- Terminal Services awareness;
- high-entropy ASLR for x64;
- large-address awareness for Win32;
- the compiler security cookie supplied by the Microsoft toolchain.

The CI job inspects the finished PE executable with the Microsoft build tools.
It fails if the expected architecture or any required flag, security cookie,
CFG instrumentation or non-empty CFG function table is absent. This verifies
the final binary rather than relying only on project settings.

Spectre-mitigated runtime libraries and CET compatibility are not yet release
requirements. They must first be built and tested for every in-tree dependency
and both architectures; they must not be enabled as an unverified checkbox.

## Automated smoke tests

The executable has a non-interactive `-ci-self-test` mode intended only for the
test pipeline. It runs in a newly created portable directory with no existing
configuration and verifies:

- first-start detection from an empty profile;
- known MD4 vectors used by the eD2K hashing foundation;
- discovery and resource loading of all 43 distributed language DLLs;
- the upload target-rate, slot-limit and unlimited-mode regression rules;
- clean process shutdown within the time limit.

These are deterministic regression tests. They do not connect to eD2K/Kad and
do not claim to measure real Internet upload speed. Network interoperability,
firewall behaviour, sustained transfer load and the visual first-run wizard
remain explicit manual/release-candidate tests.

## Package evidence

For each architecture the package job creates:

- the portable ZIP;
- a JSON file manifest with path, size and SHA-256 for every payload file;
- an SPDX 2.3 JSON software bill of materials (SBOM);
- a SHA-256 checksum list covering all three outputs.

The manifest and SBOM are also embedded in the portable ZIP. Packaging fails if
the archive cannot be opened, a required file is absent, the language count is
wrong, the manifest is empty, or manifest and SBOM file counts disagree.

GitHub workflow artifacts are test evidence, not automatically published
releases. A public release still requires the manual checks in
`RELEASE_CHECKLIST.md` and a tag identifying the exact source revision.

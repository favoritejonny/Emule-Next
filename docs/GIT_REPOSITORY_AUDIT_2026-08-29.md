# Local Git repository audit - 2026-08-29

Audit target: local `main` at commit `ea11274`, before adding this audit note.

## Result

- Remote configured as `https://github.com/favoritejonny/Emule-Next.git`.
- Working tree clean at the audit point.
- 6,634 tracked files.
- One commit identity only:
  `Jonny Favorite <318965171+favoritejonny@users.noreply.github.com>`.
- No reachable missing or corrupt Git objects.
- Largest reachable blob: 5,352,613 bytes, well below GitHub's 100 MB hard
  per-file limit.
- No blob above 50 MiB.
- Longest tracked relative path: 165 characters.
- No executable, library, build-cache, dump, release archive, personal path,
  credential or private key was found by the preceding clean-source checks.

`git fsck --full` reported dangling blobs created by the interrupted first
indexing attempt. They are unreferenced temporary Git objects, not repository
history or missing data, and will not be included in a normal push. They can
be left for Git's normal maintenance; no destructive cleanup is required.

## Windows path note

The current Codex working directory is deeply nested, so the absolute path of
some complete mbedTLS dependency files exceeds the legacy Windows path limit.
Repository-local Git long-path support was enabled to create the index.
Contributors should clone to a short location such as `C:\src\Emule-Next` and
enable the supported Git for Windows long-path option if their environment
still rejects a checkout.

## Publication interpretation

This audit confirms that the local Git structure was technically suitable for
GitHub at the audited revision. The later written upstream responses are
recorded in `LEGAL_STATUS.md`. Because documentation commits were added after
this audit, repeat the clean-tree, privacy, credential and integrity checks
immediately before any push. This historical audit does not by itself
authorise a public tag or binary release.

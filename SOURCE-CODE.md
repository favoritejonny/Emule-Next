# Source code for binary releases

Every eMule Next binary archive must have complete corresponding source code
available under GPL-2.0-or-later. The source must match the binary exactly,
including modified sources, project files, build scripts, dependency sources
and this legal documentation.

The intended project repository is:

https://github.com/favoritejonny/Emule-Next

As checked on 2026-08-29, that repository is public but empty. This working
folder is now a local Git repository with a reviewed initial source commit and
subsequent documentation commits using the maintainer's GitHub no-reply
identity. Do not push the source or publish binaries until the recorded legal
blockers are resolved. The identified upstream issues were resolved by the
written responses recorded on 2026-08-29. A fresh clean-tree, privacy,
credential and integrity audit is still required immediately before the first
normal source push. Before binaries are published, the public repository must
contain the reviewed complete source and an immutable tag for the exact
release revision.

For a public release, create an immutable version tag and publish both of the
following in the same release location:

1. the Win32/x64 binary archive with SHA-256 checksum; and
2. a source archive created from that exact tag, or an equally accessible link
   to that exact tag.

Do not publish a binary if its source tag, source archive and checksum have
not been checked. A future distributor must be able to obtain the same source
without needing to ask for permission or create an account.

This document is a release procedure, not a substitute for the source itself.

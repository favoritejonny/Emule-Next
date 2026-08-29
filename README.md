# eMule Next

eMule Next is an independent community build for Windows, based on the eMule
Community 0.72a code base. It keeps compatibility with the eD2K and Kad
networks while modernising the Windows build, reliability, interface and
packaging.

The project is maintained by Jonny Favorite. It is not an official eMule
Project release and is not affiliated with, endorsed by, or sponsored by the
original eMule Project.

## Current status

The Windows Win32 and x64 release candidates and their portable packages have
completed the current manual stability tests. The first public pre-release is
not published yet: the remaining items in [LEGAL_STATUS.md](LEGAL_STATUS.md)
and [RELEASE_CHECKLIST.md](RELEASE_CHECKLIST.md) must be resolved first.

Please use the repository only for project source, documentation and lawful
technical reports. Do not upload copyrighted media, download links, server
lists of uncertain provenance, personal configuration files or credentials.

## License

eMule Next is a derivative work of eMule. The original source headers license
the program under the GNU General Public License, version 2 or (at the
recipient's option) any later version. See [LICENSE](LICENSE).

This means that every recipient may run, study, modify and redistribute the
program under those terms. A binary release must always be accompanied by the
complete corresponding source code and the notices required by the included
third-party components.

Read the following before making a public release:

- [NOTICE.md](NOTICE.md) - upstream attribution and trademark clarification;
- [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) - libraries and notices;
- [CHANGES.md](CHANGES.md) - eMule Next changes;
- [SOURCE-CODE.md](SOURCE-CODE.md) - source availability rules;
- [WINDOWS-SMARTSCREEN.md](WINDOWS-SMARTSCREEN.md) - safe first-run checks for
  the currently unsigned Windows builds;
- [RELEASE_CHECKLIST.md](RELEASE_CHECKLIST.md) - mandatory release checks;
- [LEGAL_STATUS.md](LEGAL_STATUS.md) - current release-clearance status.

## Safe and lawful use

The software is a general-purpose peer-to-peer client. Users are responsible
for downloading and sharing only material they are authorised to receive or
distribute. Do not use official project channels to publish copyrighted
content, infringing server lists, or material that encourages copyright
infringement.

## Privacy

The current distribution does not collect crash reports, analytics or other
telemetry for the project maintainer. See [PRIVACY.md](PRIVACY.md). Peer-to-peer
network traffic is inherently exchanged with other participating peers and
servers as needed for the protocol.

## Builds and packages

The Visual Studio solution is [srchybrid/emule.sln](srchybrid/emule.sln).
Release executables are built separately for Win32 and x64. The portable
package script is [packaging/Build-PortablePackages.ps1](packaging/Build-PortablePackages.ps1).
The first public pre-release is planned as two portable ZIP archives, one for
Win32 and one for x64, without an installer. Each archive includes the
SmartScreen notice and must be published with its SHA-256 checksum.

Do not publish a package until every item in [RELEASE_CHECKLIST.md](RELEASE_CHECKLIST.md)
has been completed and the blockers in [LEGAL_STATUS.md](LEGAL_STATUS.md) are
resolved.

## Participate

- Read [SUPPORT.md](SUPPORT.md) before asking for help.
- Use the guided GitHub issue forms for reproducible bugs and feature ideas.
- Read [CONTRIBUTING.md](CONTRIBUTING.md) before proposing a source change.
- Report security vulnerabilities privately as described in
  [SECURITY.md](SECURITY.md), never in a public issue.
- Follow [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) in all project spaces.

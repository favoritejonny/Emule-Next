# Privacy statement for the current eMule Next distribution

Last reviewed: 2026-09-07

The current eMule Next distribution does not operate project telemetry,
analytics, advertising identifiers, automatic crash-report uploads or a
maintainer-operated account service. It does not send crash dumps to Jonny
Favorite or to an eMule Next-operated server.

eMule Next is a peer-to-peer client. When a user connects to the eD2K or Kad
networks, network information necessary for those protocols, including the
user's network address and requested or shared file metadata, can be exchanged
with peers or servers. This is a technical consequence of using a peer-to-peer
network; users choose whether to run the client, connect and share files.

Project pages and download hosts such as GitHub or SourceForge are independent
services and apply their own privacy policies.

During first-run setup, the user may optionally run an Internet speed test.
The test transfers temporary data over HTTPS through Cloudflare's public speed
test endpoints. Cloudflare can therefore receive the user's IP address and
ordinary connection metadata. At most about 100 MB is transferred. eMule Next
does not call Cloudflare's result-recording endpoint, does not send the measured
values to the eMule Next project and stores them only in the local configuration
if the user chooses to apply them.

The first-run server-list option, when left selected and completed with eD2K
enabled, requests a `server.met` file over HTTPS from eMule-Security. That
service can receive the user's IP address and ordinary connection metadata.
The downloaded entries are filtered and merged locally; existing personal
servers are preserved. If the request fails, the bundled offline list is used.
This request is not telemetry and no result is sent to the eMule Next project.

Before enabling any crash reporting, update checks with identifiers, usage
analytics, support form, mailing list or other project-operated collection,
the maintainer must update this document and publish a privacy notice covering
the data, purpose, legal basis, recipients, retention period, security measures
and contact method.

# Publication status - 2026-08-29

## Candidate tested

The current tested candidate is RC1-fix9:

- Win32 portable ZIP SHA-256:
  `FB01274CB7FF59FD3116E93024232370664331A36FC6E921C0A24934173F611D`
- x64 portable ZIP SHA-256:
  `1DAA4E16B20A3BF38F97A74F9FD3F93017726E6B853D58CD802714AA3639C3E0`

Both portable packages passed automated structure checks and manual launch
testing. The normal-profile Win32 and x64 clients were also tested for about
12 hours each without reported anomalies.

## Publication blockers

1. Resolve TreeOptionsCtrl and HttpDownloadDlg's non-GPL source-distribution
   conditions by written permission or replacement. The permission request was
   sent to the author's published contact address on 2026-08-29; a written
   response is pending and permission is not yet assumed.
2. Resolve use of the `eMule Next` name and mule artwork by permission,
   specialist review or renaming. The official administrative/legal contact
   route has been identified and a request is prepared in
   `docs/PERMISSION_REQUEST_EMULE_PROJECT.md`, but it has not yet been sent.
3. The public GitHub repository now exists and is empty. A reviewed local Git
   repository and clean initial commit were created on 2026-08-29 using the
   maintainer's GitHub no-reply identity. Push only after the legal blockers
   are resolved, then create an immutable source tag.
4. Regenerate the final portable archives so that they contain the corrected
   legal documents, then verify and briefly smoke-test them.

No public binary should be uploaded before these items are complete.

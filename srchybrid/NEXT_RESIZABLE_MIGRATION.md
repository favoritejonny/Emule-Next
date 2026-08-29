# NextResizable migration

## Objective

Replace the binary dependency on the legacy Artistic License 1.0 ResizableLib
with a small, independently authored GPL-2.0-or-later module.  The replacement
keeps the client-facing MFC layout API required by eMule Next.

## Completed in this step

- Added `srchybrid/NextResizable`, an independent anchor-layout module.
- Implemented the four client base classes: dialog, property page, property
  sheet, and form view.
- Implemented all in-use layout operations: `AddAnchor`, `RemoveAnchor`,
  `AddAllOtherAnchors`, and `EnableSaveRestore`.
- Removed `resizablelib.lib` from every eMule Next build configuration.
- Removed the ResizableLib project and project dependency from `emule.sln`.
- Added local compatibility headers so the existing window classes retain their
  established source interface without carrying any source from the legacy
  library.

## Build verification completed on 2026-08-20

- The x64 Release configuration completed with zero errors.
- The Win32 Release configuration completed with zero errors.
- The project, solution, package script and both build logs contain no legacy
  `resizablelib.lib` or `ResizableLib.vcxproj` link reference.
- Following the project tester's functional check, the unused legacy
  ResizableLib source tree was removed from the eMule Next release tree.

## Final release checks

1. Record the final manual check of the main window, Transfers, Search,
   Servers, Shared Files, Preferences, file details and the first-run wizard.
2. Confirm that window size and position are restored after restarting.
3. Exclude the local recovery copy of the legacy library from every source
   archive and repository.

The old implementation is no longer part of the release source tree. The
remaining checks are part of the wider release process described in
`LEGAL_STATUS.md`.

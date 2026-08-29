# eMule Next Portable

The portable package contains the normal release files plus the
`eMuleNext.portable` marker beside `eMuleNext.exe`.

It also contains all bundled language DLLs in `lang\\`. The user can select
Italian or any other supplied language from Preferences without downloading
additional files.

When the marker exists, eMule Next keeps configuration, logs, temporary files,
incoming files, skins, and installed language files in its own folder. It does
not read or write the eMule directory-mode setting, register the `ed2k:` link
handler, or add itself to Windows startup.

Package each platform separately:

- `eMuleNext-1.0.0-alpha.1-win32-portable.zip`
- `eMuleNext-1.0.0-alpha.1-x64-portable.zip`

ARM64 is intentionally deferred: its preparation remains in the source tree,
but it is not part of the first portable release.

The first public pre-release is distributed only as separate Win32 and x64
portable ZIP archives. It does not include an installer.

## First run and Windows SmartScreen

The current pre-release executables are unsigned, so Windows SmartScreen may
display a warning. Read `WINDOWS-SMARTSCREEN.md` before continuing. Verify the
official download and its SHA-256 checksum first; never disable SmartScreen,
Microsoft Defender or another Windows security feature to run the program.

## Creazione degli archivi

Dopo una build Release, eseguire da PowerShell nella cartella del progetto:

```powershell
.\packaging\Build-PortablePackages.ps1
```

Lo script genera gli archivi Win32 e x64 in `dist\portable\`, include un file
di checksum SHA-256 e non sovrascrive pacchetti esistenti. Per creare un nuovo
set di archivi, indicare una nuova versione con `-Version`.

Do not extract a portable package below `Program Files`, because Windows may
not let the program save its own data there.

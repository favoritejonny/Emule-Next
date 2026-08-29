# Windows SmartScreen notice

eMule Next pre-release executables are currently unsigned and may show the
Windows SmartScreen message "Windows protected your PC". A warning can be
caused by a new application's limited reputation, but it must never be ignored
without first checking the download.

Before running eMule Next:

1. Download it only from the official eMule Next release page:
   <https://github.com/favoritejonny/Emule-Next/releases>
2. Compare the ZIP file's SHA-256 value with the value published in the
   release's `SHA256SUMS` file.
3. Scan the archive and executable with up-to-date security software.
4. Do not run the program if the source or checksum does not match.

To calculate the SHA-256 value in Windows PowerShell:

```powershell
Get-FileHash .\eMuleNext-*.zip -Algorithm SHA256
```

If all checks pass and you choose to continue, select **More info** in the
SmartScreen window, confirm that the displayed application is eMule Next, then
select **Run anyway**. Until code signing is introduced, Windows may display
the publisher as **Unknown publisher**.

Never disable SmartScreen, Microsoft Defender or other system protection just
to run eMule Next. A computer managed by an organisation may intentionally
prevent this override; follow the organisation's security policy.

## Avviso SmartScreen in italiano

Gli eseguibili preliminari di eMule Next non sono ancora firmati e Windows può
mostrare il messaggio SmartScreen "PC protetto da Windows". Non ignorare
l'avviso senza aver prima controllato il file.

Scarica il pacchetto soltanto dalla pagina ufficiale indicata sopra, confronta
il suo checksum SHA-256 con `SHA256SUMS` ed esegui una scansione antivirus. Se
i controlli corrispondono e decidi di continuare, scegli **Ulteriori
informazioni**, verifica che l'applicazione indicata sia eMule Next, quindi
seleziona **Esegui comunque**. Non disattivare SmartScreen, Defender o altre
protezioni di Windows.

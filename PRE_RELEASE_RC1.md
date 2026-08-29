# eMule Next 1.0.0-alpha.1 RC1

Data di preparazione: 24 agosto 2026  
Ultimo controllo: 29 agosto 2026

La candidata collaudata e RC1-fix9. Tutti i pacchetti RC1 precedenti sono
superati. I due archivi approvati per il collaudo si trovano nella cartella
`dist/prerelease/1.0.0-alpha.1-rc1-fix9-20260828-103711`.

Questa cartella di consegna contiene la prima candidata portatile destinata
al collaudo finale. Non e ancora l'autorizzazione alla pubblicazione pubblica:
restano da completare gli elementi indicati in `RELEASE_CHECKLIST.md` e
`LEGAL_STATUS.md`.

## Risultato della compilazione

- Release x64: completata con zero errori.
- Release Win32: completata con zero errori.
- Versione incorporata negli eseguibili: `1.0.0-alpha.1`.
- Traduzioni incluse in ogni pacchetto: 43.
- Gli avvisi rimasti provengono prevalentemente dal codice storico, dagli
  header Windows/ATL/MFC e dalle librerie di terze parti; i log completi sono
  conservati in `srchybrid/build-logs`.

## Pacchetti portatili

Gli archivi collaudati sono:

- `eMuleNext-1.0.0-alpha.1-rc1-fix9-win32-portable.zip`
  (4.708.500 byte; SHA-256
  `FB01274CB7FF59FD3116E93024232370664331A36FC6E921C0A24934173F611D`)
- `eMuleNext-1.0.0-alpha.1-rc1-fix9-x64-portable.zip`
  (5.177.080 byte; SHA-256
  `1DAA4E16B20A3BF38F97A74F9FD3F93017726E6B853D58CD802714AA3639C3E0`)
- `SHA256SUMS-1.0.0-alpha.1-rc1-fix9.txt`

Entrambi gli ZIP sono stati riaperti dopo la creazione. Hanno 59 elementi
ciascuno e contengono l'eseguibile, il marcatore portatile, 43 DLL di lingua,
la documentazione, la licenza GPL e le licenze delle dipendenze. Non contengono
preferenze personali, file di configurazione utente, dump o simboli di debug.
Gli eseguibili incorporati corrispondono ai Release collaudati e hanno
l'architettura PE corretta. Il tester ha avviato e provato entrambi i pacchetti
portable con esito positivo.

## Verifiche ancora necessarie

1. Le risposte scritte per TreeOptionsCtrl, HttpDownloadDlg, nome `eMule Next`
   e grafica sono state ricevute e registrate con le relative condizioni.
2. Eseguire l'audit finale e caricare nella repository pubblica il sorgente
   locale revisionato senza usare push forzati.
3. Creare un tag Git immutabile della revisione esatta approvata e pubblicare
   il sorgente corrispondente insieme ai binari, come richiesto dalla GPL.
4. Rigenerare i due ZIP finali per includere i documenti legali corretti e
   ripetere la verifica automatica e un breve test di avvio.
5. Decidere se firmare digitalmente gli eseguibili. La RC1 attuale non e
   firmata e Windows puo quindi mostrare un avviso SmartScreen.

## Regola per la pubblicazione

Non caricare ancora questa RC1 come release pubblica. Prima si risolvono i
blocchi legali, si carica nella repository pubblica vuota il sorgente locale
gia revisionato e lo si congela con un tag. Solo dopo si generano gli archivi
finali con nuovi checksum.

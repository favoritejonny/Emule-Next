# Stato di sviluppo - eMule Next 1.0.0-beta.1

## 8 settembre 2026 - cache grandi condivisioni e hashing

- Eliminata la scansione completa di tutti i record `known.met` per ogni file
  trovato nelle cartelle condivise. Un indice in memoria restringe la ricerca
  ai soli nomi candidati, mantenendo i controlli esatti su data, dimensione e
  nome e gestendo correttamente nomi duplicati.
- Portati i blocchi di lettura dell'hashing da 8 KiB a 64 KiB e il buffer del
  flusso a 256 KiB. MD4 e AICH continuano a essere calcolati insieme nello
  stesso passaggio; non sono stati introdotti thread paralleli né cambiati i
  formati `known.met`, `known2_64.met` o dei crediti.
- L'hashing viene ora scartato se dimensione o data di modifica cambiano durante
  la lettura. Corretto inoltre `statUTC(HANDLE)`, che componeva la dimensione
  usando il campo basso dell'indice del file anziché quello della dimensione.
- La ricostruzione della lista grafica dei file condivisi evita la precedente
  ricerca duplicati per ogni riga quando la lista è appena stata svuotata e
  sospende il ridisegno fino al termine. Le aggiunte normali conservano il
  controllo duplicati.
- Release x64 e Win32 compilate con exit code 0. Suite offline completa superata
  su entrambe: vettori MD4/eD2K e nuovo test oltre i confini del buffer, primo
  avvio portable, 43 lingue, rete simulata, upload, chiusura e protezioni PE
  CFG/ASLR/DEP/security cookie. Restano solo warning storici di codice, SDK e
  librerie, senza nuove soppressioni.
- Creati e verificati i pacchetti puliti
  `1.0.0-dev.large-share-cache.2` in
  `dist/large-share-cache-preview-final-20260908`; 61 file e 43 lingue per
  architettura, manifest e SBOM coerenti, nessuna cartella `config` inclusa.
  ZIP x64: 5.411.932 byte, SHA-256
  `7FCD36FC3F98FBD39C6F846548EA6F5EB8DDDFE5F7870920CC1E4A4AD66684CA`;
  ZIP Win32: 4.947.365 byte, SHA-256
  `DED20865BF7BD4BADAF024DFE9AB02F194C9BA4CB8EA71B52FEAB3709A07E1A0`.
- Eseguibile x64: 9.783.296 byte, SHA-256
  `26D1A75379D09BB36BCCBAC2B47399A92ADC0CDAE66B13D06634574B77CE813C`;
  eseguibile Win32: 8.717.312 byte, SHA-256
  `32420EDC9A9F0FFF538EF18A185EBEFEEF589AB52F32A3480ED10DA0035CBFBA`.

## 8 settembre 2026 - connessione automatica al termine del primo avvio

- Corretto il caso in cui il timer di inizializzazione leggeva la vecchia
  preferenza mentre la procedura guidata era ancora aperta. Se l'avvio è già
  terminato, `Fine` inoltra ora il normale comando di connessione; se non è
  terminato, resta valido il percorso automatico già esistente.
- La connessione viene richiesta soltanto durante il primo avvio, con la
  relativa casella selezionata e l'applicazione pronta. Deselezionando la
  casella, eD2K e Kad restano disconnessi. Le riaperture manuali della procedura
  non provocano connessioni inattese.
- Aggiunto un test di regressione per le quattro combinazioni rilevanti.
  Release x64 e Win32 compilate con exit code 0; suite offline completa
  superata su entrambe, incluse 43 lingue e protezioni PE.
- Collaudo reale della procedura eseguito controllando una copia portable x64
  pulita. Con la casella selezionata eD2K si è connesso e Kad ha avviato la
  connessione dopo il tentativo router; senza casella, dopo lista server,
  tentativo router e 15 secondi di osservazione, entrambi sono rimasti
  `Disconnesso` e il comando mostrato era `Connetti`.
- Creati e verificati i pacchetti locali
  `1.0.0-dev.first-start-autoconnect.1` in
  `dist/first-start-autoconnect-preview-20260908`, con 61 file e 43 lingue.
  ZIP x64: 5.407.779 byte, SHA-256
  `2C55945D9E182E5D2B258FC01899CA381EAAF1DCB45149C04EEA9BA794A0D01D`;
  ZIP Win32: 4.946.334 byte, SHA-256
  `ED196D018C24E89500D9605C27EEBEBCCAB8617AA6A04012856C784706CAA0EC`.

## 7 settembre 2026 - procedura compatta, speed test e lista server online

- Compilate con esito positivo le Release x64 e Win32 usando Visual Studio
  2026/v145; entrambe hanno concluso con exit code 0 e tutte le 43 DLL lingua.
  I warning rimasti provengono dal codice storico, dalle vecchie librerie e
  dagli header Microsoft; non sono state disattivate protezioni per compilare.
- La nuova procedura ha sei pagine: porte e configurazione router sono riunite,
  così come reti e lista server. Su un profilo nuovo la configurazione automatica
  del router e l'aggiornamento della lista eD2K sono selezionati, mentre la
  connessione sicura ai server non lo è. I profili esistenti restano preservati.
- Aggiunto un test velocità esclusivamente volontario, eseguito su thread di
  lavoro tramite endpoint HTTPS Cloudflare. I valori restano modificabili;
  applicandoli, upload è limitato all'80% della capacità e download resta
  illimitato. Il test non è stato eseguito durante la build o la CI.
- La lista eD2K selezionata scarica via HTTPS il `server.met` di eMule-Security,
  filtra e unisce le voci senza eliminare server personali; la lista inclusa
  resta il ripiego offline. Nessun download è stato eseguito dalla CI.
- Suite offline completa superata su entrambe le architetture: protezioni PE,
  primo avvio isolato, hashing, 43 lingue, rilevamento lingua Windows,
  PCP/NAT-PMP, regole upload, porte e profili, sei pagine senza testi tagliati,
  lista server, chiusura e riavvio senza perdita delle preferenze di prova.
- Output compilati: `srchybrid/x64/Release/eMuleNext.exe` (9.776.640 byte,
  SHA-256 `A7F2D2D0BF90318DE2B0BB31ABF56AC4C7B2BCF6F95E21D3D116730CAB001F4B`)
  e `srchybrid/Win32/Release/eMuleNext.exe` (8.712.192 byte, SHA-256
  `97D5CA2E3EE5C7E9DE05F6A1843DF661BBE2B5EE581BF0899FD49C7EAC35CC21`).
- Rimosso da `emule.slnx` il riferimento residuo alla vecchia ResizableLib.
  Aggiornati documentazione di collaudo e privacy. Non sono stati avviati
  client interattivi né modificati profili personali.

## 3 settembre 2026 - Release Win32/x64 e portable aggiornati

- Compilate entrambe le Release con Visual Studio 2026, toolset v145
  14.51.36231 e Windows SDK 10.0.28000.0. Exit code 0 per entrambe; nessun
  errore. Restano 982 avvisi Win32 e 1.036 x64 nel codice storico e negli SDK.
  Non sono state disattivate protezioni o soppressi warning per ottenere la build.
- Incluse la dimensione minima standard della finestra principale e la rimozione
  del collegamento Firewall Windows dall'avvio guidato, con le 43 traduzioni.
  Le impostazioni esistenti e i protocolli di connessione restano invariati.
- Suite offline completa superata su entrambe: CFG/ASLR/DEP/security cookie,
  primo avvio portable, hashing, lingue, geometria dei testi della procedura,
  regole upload, porte/consenso, PCP/NAT-PMP simulati, server facoltativi,
  chiusura e riavvio con lingua e preferenze di prova conservate.
- Creati e verificati i pacchetti locali `1.0.0-dev.guided-setup.2` in
  `dist/guided-first-start-preview-20260903`: 65 file ciascuno, 43 DLL lingua,
  manifest, SBOM e checksum SHA-256. Autotest superato anche dopo l'estrazione
  dagli ZIP. Copie Win32/x64 pronte, senza cartella config, con lo stesso hash
  degli eseguibili Release. I pacchetti precedenti non sono stati sostituiti.
- ZIP Win32: 4.943.500 byte, SHA-256
  `c3cf50a85b7e239baa1d2caa3a7f52032e395aeb3d9815c5ba4bdb3b7e3677e1`.
- ZIP x64: 5.402.747 byte, SHA-256
  `042ba6802f6000f445bab4e2d26d7f13301c8a3b6c38fa39e5cbdb257b111a13`.
- Log di build, CI e pacchetti in `dist/build-check-20260903-134916`.
  Normalizzato solo l'ambiente dei processi di build per evitare Path/PATH
  duplicati. Nessuna impostazione persistente di Windows modificata.
- Eseguibili ancora non firmati; nessun caricamento su GitHub, SourceForge o
  servizi antivirus. Non sono stati avviati client interattivi né toccati profili
  personali. Restano da collaudare visivamente avvio, ridimensionamento,
  ripristino dalla tray e percorso completo della procedura guidata.

## 3 settembre 2026 - collegamento firewall rimosso dall'avvio guidato (sorgenti)

- Rimossi il pulsante che apriva il pannello Firewall Windows e il relativo
  gestore dalla procedura iniziale. La pagina ora riguarda soltanto il test
  facoltativo delle porte, con informativa sulla condivisione IP/porte e nota
  di attendere Fine se è stata scelta la configurazione automatica del router.
- Conservati i valori numerici delle risorse; aggiornati i nomi della pagina
  e della spiegazione. I testi in inglese e nelle 43 lingue sono stati accorciati
  eliminando le frasi relative al collegamento rimosso. Nessuna nuova traduzione
  automatica; il contenuto restante è quello già presente.
- Non modificati il consenso di Windows, le regole del firewall, le porte,
  i protocolli del router o le preferenze. Le diagnostiche di connessione
  continuano a distinguere mappatura del router e raggiungibilità effettiva.
- Aggiunto `-SourceOnly` a `tests/Invoke-CiTests.ps1` per verificare le
  traduzioni senza avviare client; aggiunti controlli contro la reintroduzione
  del pulsante, anche nell'autotest nativo della prossima build.
- Verifiche superate: completezza/UTF-8/segnaposto delle 43 lingue,
  compilazione delle 44 risorse inglese+traduzioni e prove native Win32/x64
  con finestre nascoste. Controllate 176 etichette per architettura, incluse
  le istruzioni delle porte, senza testi fuori spazio o pulsante firewall.
- Materiale di prova: `dist/wizard-port-test-resource-check-20260903-134024`;
  sorgente della prova isolata `../../maintenance/WizardPortTestProbe.cpp`.
  Nessuna esecuzione di codice del client, accesso ai profili o richiesta di rete.
- La successiva build completa e i pacchetti aggiornati sono descritti sopra.
  Resta il collaudo visivo del percorso Avanti/Indietro/Fine/Annulla e della
  finestra principale.

## 3 settembre 2026 - dimensione standard della finestra principale (sorgenti)

- Confrontati i file originali Preferences.cpp, EmuleDlg.cpp ed emule.rc del
  tag `eMule_v0.72a-community`, revisione
  `ac3d52eafd88e9f6d9e0c2204c6dbead18db750b`:
  [sorgenti upstream](https://github.com/irwir/eMule/tree/eMule_v0.72a-community/srchybrid).
  Il modello principale resta 515x339 unità dialogo, con coordinate iniziali
  (10,10)-(700,500). La vecchia gestione ResizableLib impostava anche il
  minimo dal modello; NextResizable non lo faceva automaticamente.
- Ripristinato quel limite soltanto nella finestra principale, prima del
  recupero della posizione salvata. Nessuna dimensione arbitraria 1200x800,
  nessuna massimizzazione forzata. Le dimensioni salvate più grandi e le
  modalità di avvio già scelte restano rispettate; quelle inferiori vengono
  ricondotte al minimo del modello. Non modificati procedura guidata, temi,
  finestre secondarie, profili personali o implementazione NextResizable.
- Inizializzata a zero l'intera struttura della posizione predefinita,
  mantenendo le coordinate upstream e il formato del file di preferenze.
- Prova nativa isolata Win32/x64 con risorsa del client caricata come soli
  dati: senza minimo 690x490, con minimo 789x590 sul sistema di prova;
  dimensioni 789x590 e 889x690 preservate. Tutti i 12 casi superati,
  finestre sempre nascoste e mai massimizzate. Il valore in pixel dipende
  dalle metriche di Windows: il codice non fissa 789x590.
- Sorgente della prova: `../../maintenance/MainWindowSizeProbe.cpp`.
  Eseguibili della sola prova in
  `dist/main-window-standard-check-20260903-125028`.
  Non si tratta di un collaudo dell'intera interfaccia MFC.
- Modifica inclusa nella successiva build completa descritta sopra. Resta il
  controllo visivo di avvio normale, ripristino dalla tray e ridimensionamento
  su entrambi i client.

## 2 settembre 2026 - avvio guidato a passaggi e server facoltativi

- Separati porte TCP/UDP, apertura automatica del router, firewall/test,
  reti eD2K/Kad e lista server. Le altre pagine e i valori già salvati restano
  invariati. Non compare più una richiesta di consenso entrando nella pagina.
- Aggiunte sei nuove spiegazioni/etichette in inglese e in tutti i 43 moduli
  lingua; verifica di completezza, UTF-8 e ingombro dei testi sui cinque
  dialoghi nativi superata. Resta utile la revisione dei madrelingua.
- Lista server inizialmente non selezionata, applicata solo su Fine con
  eD2K abilitato. Non viene più generata automaticamente staticservers.dat.
  I server già presenti sono conservati; i nuovi sono ordinari, soggetti al
  filtro IP. Gli indirizzi inclusi sono quelli già presenti, datati 11 agosto.
- Test offline superati per consenso/annullamento, inizializzazione differita,
  duplicati, risorse, primo avvio portable, lingua Windows, profilo salvato,
  hashing, upload, PCP/NAT-PMP, porte, chiusura e protezioni PE.
- Build Release Win32 e x64 completate con exit code 0, Visual Studio 2026
  v145. Restano i warning noti del codice storico, SDK e librerie. La prima
  compilazione ha rilevato un include standard mancante, corretto e ricompilato.
  L'ambiente del solo processo di build è stato normalizzato per evitare due
  variabili Path/PATH duplicate; nessuna variabile Windows persistente cambiata.
- Percorso delle nuove prove: `dist/guided-first-start-preview-20260902`.
  Guida al collaudo: `docs/PROVA-AVVIO-GUIDATO.md`. Non sono state cancellate
  configurazioni personali né modificate le release pubblicate.
- ZIP `1.0.0-dev.guided-setup.1` creati e verificati: 65 file ciascuno,
  43 DLL lingua, manifest/SBOM coerenti e autotest riuscito dal pacchetto.
  Copie Win32/x64 estratte senza cartella config. Dimensioni e SHA-256:
  - Win32: 4.946.495 byte,
    `cf12e304506641f1fface43a632638e6881b2bbe0a849dbd717dc050912e8997`.
  - x64: 5.405.939 byte,
    `a7000c4bc4771a9278abce5b89b29abcaa77360e014dcb58abcc4c60a19cf631`.
- Da verificare manualmente: percorso completo Avanti/Indietro/Fine/Annulla,
  diversi livelli DPI e comportamento con router reali. I test offline non
  sostituiscono questi collaudi di interazione e rete.

## 1 settembre 2026 - configurazione iniziale in tutte le lingue

- Inserite le 24 nuove scritte della configurazione connessione in tutti i 43
  moduli lingua distribuiti. Inglese e italiano sono stati rivisti
  direttamente; le altre traduzioni sono una prima versione assistita da
  sottoporre anche ai beta tester madrelingua.
- Il controllo sorgenti verifica per ogni modulo: presenza di tutte le
  risorse, codifica UTF-8, assenza di duplicati e segnaposto `%u`/`%s`
  invariati rispetto all'inglese.
- L'autotest binario carica l'inglese incorporato e tutte le 43 DLL lingua,
  misura i testi visibili con i controlli reali della procedura e segnala il
  modulo preciso in caso di testo tagliato. Il controllo ha individuato e
  permesso di accorciare il solo pulsante Firewall in greco.
- Release Win32 e x64 compilate senza errori con Visual Studio 2026/v145.
  Entrambe hanno superato la suite offline completa: protezioni PE, primo
  avvio portable isolato, hashing, lingua automatica Windows, PCP/NAT-PMP,
  upload, consenso e porte, 43 lingue, riavvio e chiusura pulita.
- Creati e verificati i due ZIP portatili locali
  `1.0.0-dev.localized-setup.1` in
  `dist/localized-first-start-preview-20260901`, con copie Win32/x64 gia
  estratte e pulite per il collaudo. Ogni ZIP contiene 65 file e 43 DLL lingua;
  manifest, SBOM, checksum, assenza di configurazioni e autotest dal pacchetto
  sono stati verificati dal nuovo script riutilizzabile
  `packaging/Verify-PortablePackages.ps1`.
- Non e stata modificata la release Alpha pubblicata. La revisione linguistica
  comunitaria resta opportuna prima della Beta.

## 31 agosto 2026 - prototipo locale PCP / NAT-PMP

- Esaminati i sorgenti qBittorrent forniti dall'utente: la gestione delle
  porte e delegata a libtorrent. Nessun codice o asset qBittorrent/libtorrent
  importato e nessuna nuova dipendenza; modulo indipendente documentato in
  `docs/PORT-MAPPING.md`.
- Release Win32 e x64 compilate senza errori con Visual Studio 2026/v145;
  superata nuovamente l'intera suite offline dopo l'ultimo ritocco ai callback
  dei rinnovi. Restano warning del codice storico/SDK e di inclusione relativa
  dell'header di test: non si tratta di build prive di warning.
- Verificate protezioni PE, avvio portable isolato, hashing, 43 lingue,
  preferenze conservate, regole upload, risorse EN/IT e chiusura.
- Aggiunti test di pacchetti e negoziazioni PCP/NAT-PMP simulate, rinnovi,
  risposte errate, rifiuti e 4.000 pacchetti malformati. Gli stessi test del
  nucleo protocollo sono passati con AddressSanitizer x86 e x64; questa
  verifica non copre l'intera GUI o il trasporto su router reali.
- Attivazione solo con consenso domestico esplicito, porte TCP/UDP del client
  senza Web server, rinnovi temporanei e fallback UPnP conservativo. Nessuna
  apertura reale di porte o modifica al firewall durante i test automatici.
- Pacchetti locali creati in `dist/port-mapping-preview-20260831`, versione
  `1.0.0-dev.port-mapping.1`; istruzioni in `docs/PROVA-CONNESSIONE.md`.
  La versione visualizzata dal client resta 1.0.0-alpha.1.
- ZIP estratti e verificati: 63 file di payload ciascuno, 43 DLL di lingua,
  manifest/SBOM, documenti aggiornati e nessuna cartella config. Tutti gli
  hash del manifest corrispondono; eseguibili identici alle build collaudate.
  Presenti anche copie gia estratte nelle sottocartelle Win32 e x64.
  SHA-256 ZIP Win32:
  `17d2c52f973ef83d4ebd16ee4fe2626c8f03d892edae19fc98d7f6eb9375987d`.
  SHA-256 ZIP x64:
  `5635ea1a432af66f9fc150471c6aa8ab40ad3c743f7960e010a43323812b42e0`.
- Prima di pubblicare servono test su router PCP/NAT-PMP e UPnP reali,
  raggiungibilita TCP/UDP, rinnovi, annullamento/chiusura, sospensione/ripresa
  e cambi rete. L'ID alto non e garantito e il CGNAT non viene aggirato.
- La release GitHub e i profili personali non sono stati modificati.

## 31 agosto 2026 - lingua automatica al primo avvio

- Release Win32 e x64 compilate senza errori dopo la modifica al rilevamento
  della lingua. Viene usata la lingua dell'interfaccia Windows, non il formato
  regionale; preferenze gia salvate prioritarie e inglese di riserva.
- Superata su entrambe l'intera suite offline, inclusi i nuovi casi sulle
  varianti regionali, la scelta manuale e il riavvio da un profilo di prova:
  lingua francese, alias, porte e consenso UPnP conservati; file di preferenze
  del test identico prima e dopo il riavvio.
- Non sono state cambiate le impostazioni di Windows o dei client personali.
  Le finestre reali della procedura restano da collaudare manualmente.
- Copie locali aggiornate in `dist/language-startup-preview-20260831`, versione
  dei pacchetti `1.0.0-dev.first-start.1`. Documentazione:
  `docs/LANGUAGE-STARTUP.md` e `docs/PROVA-CONNESSIONE.md`.
- Nessuna pubblicazione o modifica alla release GitHub.

## 31 agosto 2026 - prototipo locale della configurazione connessione

- Release Win32 e x64 compilate con Visual Studio 2026/v145 senza errori.
- Superati su entrambe: verifica PE delle protezioni, avvio portatile isolato,
  hashing, caricamento delle 43 lingue, regole upload e chiusura.
- Superati i nuovi test offline di validazione porte, consenso/profili di rete,
  controllo delle mappature e dimensionamento dei testi inglesi/italiani.
- Nessuna apertura di porte reali durante questi test; restano da collaudare
  manualmente la procedura completa e il comportamento con router reali.
- Le copie di prova non sostituiscono l'Alpha pubblicata. Dettagli e limiti in
  `docs/CONNECTION-SETUP.md`; istruzioni in `docs/PROVA-CONNESSIONE.md`.

## Base importata

- sorgenti Community 0.72a, tag `eMule_v0.72a-community`;
- progetto `srchybrid/emule.slnx` con target Win32, x64 e ARM64;
- output configurato come `eMuleNext.exe`.

## Verifiche eseguite

- `emule.rc` compilato con successo in una risorsa Windows;
- controllo sintattico x86 e x64 completato per branding, layout, Kad,
  WebSocket e TLS;
- manifest Win32, x64 e ARM64 aggiunti per il nuovo nome prodotto.

## Aggiornamento della migrazione

- cache delle icone resa sicura per puntatori a 64 bit;
- le API di ancoraggio usate da Community 0.72a sono ora fornite dal modulo
  interno GPL `NextResizable`;
- Crypto++ 5.6 gestito senza conversioni verso `CryptoPP::byte`;
- threading e WebSocket adattati alle interfacce di mbedTLS 4.2;
- sorgenti delle dipendenze inclusi nella nuova copia di lavoro.

I controlli mirati x86 e x64 relativi a queste modifiche sono conclusi senza
errori. Una compilazione ampia del solo codice eMule ha raggiunto gli adattamenti
delle API aggiornate; il processo e' stato fermato solo dal limite di tempo.

## Stato attuale

- Gli ultimi ritocchi grafici e alla pagina Generale hanno superato
  compilazione e collegamento Release Win32/x64 senza errori il 24 agosto
  2026: ripristinate le cinque illuminazioni dei contatti Kad, ridisegnata
  l'icona "In coda" come gruppo di persone in entrambe le finestre
  Trasferimenti e rimossi i controlli automatici di aggiornamento inattivi.
- La revisione delle opzioni avanzate ha superato compilazione e collegamento
  Release Win32/x64 senza errori il 21 agosto 2026. Le nuove installazioni
  usano 20 connessioni parziali sui Windows moderni, controllo dello spazio
  disco attivo con riserva automatica fra 512 MiB e 2 GiB, anteprima automatica
  degli archivi disattivata, estrazione ID3 volontaria e registri diagnostici
  secondari disattivati. I file sparsi vengono usati solo sui volumi che li
  dichiarano supportati. Le preferenze gia salvate dagli utenti restano
  invariate. Corretto inoltre il limite dinamico di upload, che ora resta
  realmente disattivato quando la relativa opzione e spenta; rimosso il vecchio
  automatismo di apertura porte limitato a Windows XP, mantenendo UPnP e il
  controllo manuale delle porte.
- Per il collaudo della procedura iniziale sono state preparate due copie
  portatili, pulite e indipendenti, una x64 e una Win32. Entrambe includono le
  43 traduzioni, non contengono `config/preferences.ini` e quindi mostrano la
  procedura guidata al primo avvio senza leggere il profilo eMule gia presente
  in Windows.
- Il collegamento binario a ResizableLib e stato rimosso. Il nuovo modulo
  interno `NextResizable`, scritto per eMule Next e distribuito sotto GPL,
  mantiene le ancore e il salvataggio delle finestre usati dal client. La
  compilazione Release Win32/x64 e stata completata senza errori il 20 agosto
  2026. Dopo il test funzionale comunicato dal progetto, la vecchia
  implementazione e stata rimossa dalla cartella pubblicabile; resta il
  controllo finale da annotare prima della pubblicazione.
- Dopo il primo test pulito e stato corretto il ridisegno di `NextResizable`:
  i controlli vengono riposizionati in un'unica operazione, le loro aree non
  vengono piu coperte durante la cancellazione dello sfondo e la gerarchia
  visibile viene ridisegnata dopo ogni variazione del layout. La stessa
  protezione e stata applicata allo sfondo tematico della finestra principale.
  La compilazione Release Win32/x64 e stata completata senza errori il 23
  agosto 2026; resta il controllo visivo del beta tester.
- Corretto nel sorgente il calcolo iniziale delle ancore delle viste MFC create
  prima alla dimensione provvisoria di 50x50 pixel. La correzione mantiene i
  controlli inferiori, compresa la voce degli utenti in coda in Trasferimenti,
  all'interno dell'area visibile. La compilazione Release Win32/x64 e stata
  completata senza errori il 21 agosto 2026; resta la verifica visiva.
- La barra laterale storica dei comandi download non viene piu mostrata nella vista Trasferimenti: e limitata ai file download selezionati e non era una barra di navigazione. I comandi restano nel menu contestuale dei download e la barra puo essere riattivata volontariamente da quel menu. I normali pulsanti per scegliere Download, Upload, Coda e Client non vengono modificati.
- Il reporting dei crash e ora locale e volontario: dopo un arresto anomalo eMule Next propone la creazione di un minidump senza inviarlo. Al riavvio successivo, se il dump esiste, l'utente puo aprire la pagina GitHub del progetto per segnalarlo e allegarlo, rimandare il promemoria oppure non visualizzarlo piu. Nessun dump viene trasmesso automaticamente.
- Il riferimento ufficiale del progetto e `https://github.com/favoritejonny/Emule-Next`: i comandi di aggiornamento manuale e segnalazione crash aprono questa pagina. Il pulsante Help principale apre la guida ufficiale eMule della finestra Server; nella schermata Trasferimenti apre invece l'indice ufficiale nella lingua selezionata nel client. Sono usate soltanto le 23 lingue del client effettivamente disponibili sul sito; per le altre traduzioni viene aperta la guida inglese. La modifica ha superato compilazione e collegamento Release Win32/x64 il 21 agosto 2026. Il controllo automatico non interroga piu i servizi del progetto eMule storico; verra riattivato solo con un feed eMule Next firmato. Nelle Preferenze e nella procedura iniziale, il nome utente predefinito e l'indirizzo della repository.
- Modalita portatile implementata: il marcatore `eMuleNext.portable` forza
  configurazione, dati e log accanto all'eseguibile e disabilita integrazioni
  nel registro di Windows. Il modello di pacchetto e disponibile in
  `packaging/portable/`; i file coinvolti hanno superato il controllo
  sintattico Win32 e x64.
- zlib 1.3.2 ha ora un progetto Visual Studio moderno e librerie statiche
  Release generate per Win32 e x64. La precedente libreria ResizableLib e
  stata ritirata dal prodotto e sostituita dal modulo interno `NextResizable`.
- Crypto++ 5.6 ha ora un progetto statico v145 per Win32 e x64. Tutti i suoi
  moduli, incluse le ottimizzazioni assembler, sono stati compilati con il
  toolset attuale. `zdeflate.cpp` non dipende piu dall'API `stdext` rimossa
  dalle versioni moderne di Visual Studio.
- id3lib 3.8.3 ha ora un progetto statico v145 per Win32 e x64. La sua
  configurazione Windows e riproducibile e non usa piu il vecchio workaround
  che ridefiniva la parola chiave C++ `for`.
- miniupnpc 2.3.3 ha ora un progetto statico v145 per Win32 e x64. Il timeout
  delle connessioni UPnP usa correttamente i millisecondi richiesti da Winsock,
  anziche la struttura di timeout usata dai sistemi Unix.
- mbedTLS 4.2.0 ha ora un progetto statico v145 per Win32 e x64. I moduli TLS,
  X.509 e PSA Crypto vengono raccolti in un unico archivio statico, mantenendo
  compatibile il collegamento gia previsto dal progetto eMule Next. Entrambe le
  versioni Release hanno compilato tutti i 109 moduli richiesti.
- eMule Next Release e stato collegato con successo per entrambe le architetture:
  `srchybrid\\x64\\Release\\eMuleNext.exe` (x64) e
  `srchybrid\\Win32\\Release\\eMuleNext.exe` (Win32). I file riportano la
  versione prodotto `1.0.0-alpha.1` e dipendono solo da componenti di Windows.
- La guida locale `docs/eMuleNext-Help.html`, inclusa automaticamente sia
  nell'output Win32 sia in quello x64, resta disponibile per le schermate che
  non hanno un collegamento ufficiale dedicato. Riceve la lingua selezionata
  nel client e contiene le 44 localizzazioni, compresi i layout da destra a
  sinistra per arabo, persiano, ebraico e uiguro.
- Tutte le 43 traduzioni disponibili vengono compilate automaticamente e
  incluse nella cartella `lang` degli output Win32 e x64. La raccolta comprende
  `it_IT.dll`; l'utente puo scegliere la lingua dalle Preferenze senza scaricare
  file aggiuntivi.
- La barra principale usa ora icone essenziali e scalabili nei colori eMule
  Next, con stati di passaggio e selezione piu leggibili. I temi personalizzati
  e le toolbar esterne restano supportati.
- I comandi di Ricerca e Messaggi usano ora il componente `CNextButton`: azione
  principale in turchese, azioni secondarie chiare, stati hover/pressione e
  focus da tastiera. Il componente e pronto per essere applicato gradualmente
  alle altre finestre.
- La modernizzazione del rendering e iniziata in modo incrementale: i pulsanti
  `CNextButton` usano Direct2D e DirectWrite per bordi arrotondati e testo piu
  nitido, con ritorno automatico al rendering MFC classico se necessario.
- Le finestre principali hanno ora una superficie chiara azzurra e testo ad
  alto contrasto; l'aspetto di Windows viene mantenuto quando e attivo il
  contrasto elevato di sistema.
- Le icone standard della barra principale sono disegnate come forme vettoriali
  antialias e generate quattro volte piu grandi prima della riduzione in una
  lista immagini a 32 bit con trasparenza. Questo elimina i bordi a quadratini
  e gli aloni delle vecchie icone a colore chiave.
- Corretto un crash di avvio introdotto nel rendering delle icone: gli oggetti
  GDI+ vengono ora distrutti prima della chiusura del runtime grafico. La
  versione x64 e stata ricompilata e ha superato un avvio reale su Windows.
- La cache delle icone dei menu non usa piu conversioni improprie tra puntatori
  e indici: questo elimina un rischio di troncamento nelle compilazioni a 64 bit.
- Il calcolo delle dimensioni di file compressi o sparsi ricompone ora i due
  valori a 32 bit con operazioni a 64 bit esplicite, senza alias di memoria.
- I timer della coda download/upload, delle richieste fonti, delle ricerche
  Kad, delle liste clienti e dei timeout per i blocchi ricevuti gestiscono
  correttamente il riavvio del contatore di Windows dopo circa 49 giorni,
  evitando ritardi o controlli eseguiti al momento sbagliato durante sessioni
  molto lunghe.
- La stessa protezione copre i timeout delle connessioni TCP, dei server e
  delle richieste DNS UDP, cosi una sessione molto lunga non lascia socket o
  richieste in attesa oltre l'intervallo previsto.
- Il gestore dei proxy SOCKS legge ora indirizzi e porte dai pacchetti in modo
  sicuro anche quando i dati non sono allineati in memoria: migliora la
  robustezza a 64 bit e conserva la compatibilita per una futura build ARM.
- Le risposte SOCKS5 che usano un nome host lungo calcolano ora la dimensione
  del buffer come valore senza segno, evitando allocazioni troppo piccole per
  nomi validi oltre 127 caratteri.
- NextResizable inizializza `LayoutInfo` con valori definiti e verifica che il
  numero di controlli possa essere passato all'API di Windows senza
  troncamenti. Questo conserva il comportamento delle finestre e rimuove i
  warning ricorrenti del modulo di layout.
- Il salvataggio di `clients.met` copia le strutture di credito come dati
  binari espliciti, senza reinterpretare un buffer di byte come oggetto C++;
  il formato del file rimane invariato. Anche il suo salvataggio periodico
  resta corretto dopo il riavvio del timer di Windows.
- Il calcolo della priorita media della coda upload tratta esplicitamente la
  coda vuota: non puo piu eseguire una divisione per zero in presenza di una
  configurazione o sequenza atipica. Le conversioni usate dai grafici di rete
  sono inoltre esplicite, senza modificare le misure visualizzate.
- Con il limite di upload disattivato, la coda apre ora alcuni slot iniziali
  anche quando la velocita appena misurata e di pochi byte al secondo. Questo
  evita che una divisione intera a zero lasci due connessioni quasi inattive a
  occupare tutti gli slot mentre un utente e in attesa; non imposta alcun
  limite di banda.
- In modalita upload senza limite, il regolatore non interpreta piu una stima
  automatica ancora assente come banda pari a zero: dopo il primo minuto non
  puo quindi degradare gli upload a soli pacchetti di mantenimento.
- Le righe di upload in stato "trickling" restano ora pienamente leggibili nei
  temi chiari: lo stato continua a essere mostrato nella sua colonna, senza
  rendere quasi trasparente tutto il resto della riga.
- Le icone dell'interfaccia principale, dei comandi, delle liste, della rete e
  dei pannelli sono ora disegnate dal client come pittogrammi moderni con
  trasparenza e ridimensionamento ad alta definizione. Le skin che forniscono
  una propria icona conservano la precedenza; le faccine della chat restano
  invariate per mantenere le loro espressioni riconoscibili.
- La dimensione della coda upload letta dalla configurazione viene riportata
  nell'intervallo supportato dall'interfaccia (da 2.000 a 10.000 utenti): un
  file INI vecchio o modificato manualmente non puo piu impostare valori
  negativi o irragionevoli.
- Anche la dimensione del buffer dei file viene validata nel suo intervallo
  supportato (da 16 KiB a 1,5 MiB). La conversione delle vecchie preferenze
  usa ora un calcolo a 64 bit, evitando overflow prima della normalizzazione.
- Il tempo di svuotamento del buffer usa una conversione esplicita e limitata
  in millisecondi: valori negativi tornano al valore predefinito e valori
  estremi non possono provocare un overflow aritmetico.
- Le scadenze per il salvataggio delle liste amici, file noti e server, oltre
  alla verifica dei ban temporanei, gestiscono correttamente il riavvio del
  contatore di Windows dopo circa 49 giorni di esecuzione continua.
- La ricerca Kad degli amici e la protezione IRC contro richieste troppo
  ravvicinate mantengono gli stessi intervalli anche attraverso il riavvio del
  contatore di Windows.
- I timer dell'interfaccia principale gestiscono lo stesso caso: aggiornamenti
  inattivi, finestra iniziale e calcolo dei comandi disponibili per i download
  non subiscono ritardi anomali nelle sessioni molto lunghe.
- I valori di velocita, dimensione e colore visualizzati dall'interfaccia usano
  ora conversioni numeriche esplicite, senza modificare il formato mostrato
  all'utente e senza warning del compilatore moderno per queste operazioni.
- I sotto-menu della lista download sono ora individuati tramite la loro
  posizione nel menu padre, invece di convertire il relativo handle in un
  valore a 32 bit. Questo elimina un punto fragile nelle build a 64 bit e
  mantiene invariati i comandi del menu contestuale.
- La stessa gestione sicura dei sotto-menu e ora riusabile in tutta
  l'interfaccia e viene applicata anche ai menu dei file condivisi e delle
  directory condivise.
- La cache delle icone di sistema usa ora indici tipizzati, invece di
  memorizzare valori numerici in puntatori generici: rimuove conversioni
  superflue e rende il comportamento identico fra Win32 e x64.
- Il controllo degli accessi Web, gli indicatori grafici Kad e il test del
  firewall UDP mantengono ora le loro scadenze corrette anche oltre il
  riavvio periodico del contatore di Windows.
- Anche la lista delle fonti bloccate, le richieste Kad UDP, i dati di fiducia
  Kad e la ripubblicazione dei file condivisi gestiscono correttamente il
  riavvio periodico del contatore di Windows.
- La coda dei pacchetti UDP, il filtro di ricerca dell'interfaccia e il registro
  delle prestazioni mantengono ora i loro intervalli anche nelle sessioni molto
  lunghe.
- Il calcolo delle medie di velocita conserva correttamente solo i campioni
  recenti anche oltre il riavvio periodico del contatore di Windows.
- La calibrazione automatica del limite di upload usa ora un confronto sicuro
  rispetto al riavvio del contatore di Windows: dopo circa 49 giorni non puo
  piu iniziare in anticipo o rimandare la propria stima di banda.
- Anche i tempi di scambio fonti, svuotamento dei file e pulizia delle fonti
  download attraversano correttamente il riavvio del contatore di Windows,
  evitando richieste o operazioni disco troppo ravvicinate nelle sessioni
  molto lunghe.
- Le richieste ai server, l'aggiornamento della lista download, i ping Kad del
  buddy e il timeout della ricerca UPnP mantengono ora la stessa cadenza anche
  oltre il riavvio periodico del contatore di Windows.
- Anche le due barre di avanzamento nella lista Trasferimenti usano una
  scadenza sicura rispetto al riavvio del contatore: dopo una sessione molto
  lunga continuano ad aggiornarsi senza attese anomale. I calcoli grafici dei
  file molto grandi dichiarano inoltre le conversioni numeriche intenzionali.
- Le barre con sfumatura 3D restano ora stabili anche con una preferenza
  grafica non valida o con un controllo molto basso: il valore viene limitato
  per il rendering e non puo piu generare divisioni per zero.
- Il registro del recupero archivi effettua in modo esplicito il calcolo
  percentuale per file grandi, senza conversioni implicite segnalate dal
  compilatore moderno.
- Il recupero ZIP legge ora i valori dai buffer senza assumere un allineamento
  della memoria e verifica che ogni record della directory centrale sia
  interamente disponibile prima di allocarlo. Un archivio incompleto o
  corrotto viene quindi scartato senza letture oltre il file o strutture
  temporanee residue.
- Il controllo degli archivi RAR e ACE verifica ora le dimensioni di intestazioni,
  nomi e commenti prima di usarli. Le anteprime ACE usano buffer locali al
  thread e nomi sempre terminati, evitando letture di memoria oltre il dato
  dichiarato quando l'archivio e corrotto.
- I record ISO vengono letti solo se l'intestazione e il nome sono completi e
  coerenti con la relativa lunghezza dichiarata; nomi Joliet troncati vengono
  ignorati senza spostamenti anomali nel file.
- Le routine condivise per leggere e scrivere numeri nei buffer di file e
  pacchetti usano ora copie sicure invece di cast diretti. Il formato binario
  rimane identico, mentre gli accessi non allineati non possono piu causare
  comportamenti instabili nei percorsi rete, Kad e trasferimenti.
- Il lettore ZIP dell'applicazione convalida ora la directory centrale e i
  limiti dei file estratti. Archivi troncati o con dimensioni falsificate non
  possono piu generare salti oltre il file, cicli di decompressione senza dati
  in ingresso o scritture oltre la dimensione dichiarata.
- Le formule delle velocita medie dichiarano ora in modo esplicito le
  conversioni necessarie per il risultato decimale, eliminando gli avvisi del
  compilatore senza modificare i valori mostrati.
- Le funzioni di testo e diagnostica ora verificano le dimensioni dei buffer:
  il log di debug non puo piu scrivere oltre il proprio spazio e la conversione
  degli indirizzi IP rifiuta dimensioni non valide. Anche la gestione UPnP usa
  formattazione con limite esplicito.
- Le routine interne per ordinare le stringhe usano ora l'ordinamento C++
  tipizzato invece della versione C a dimensioni manuali: il comportamento resta
  invariato, ma il codice e piu sicuro e leggibile su Win32 e x64.
- La cache delle icone dei menu usa ora handle grafici tipizzati invece di
  puntatori generici, eliminando conversioni non necessarie nella parte grafica.
- Lo spostamento delle righe nelle liste conserva ora testi e callback con una
  struttura tipizzata; libera sempre le copie temporanee e non prova a
  ripristinare sotto-elementi se Windows non riesce a creare la nuova riga.
- Le pagine di stato del WebServer usano ora formattazione testuale con limite
  esplicito per percentuali, velocita e contatori, evitando qualunque scrittura
  oltre i buffer locali anche con valori anomali.
- I totali del WebServer per file e trasferimenti sono ora sommati con interi a
  64 bit, evitando perdite di precisione prima della visualizzazione dei dati.
- Le liste server, download, upload e file condivisi del WebServer usano ora
  l'ordinamento C++ tipizzato con gli stessi criteri precedenti, eliminando le
  chiamate C a dimensioni manuali.
- La composizione della pagina trasferimenti del WebServer riceve ora le liste
  con riferimenti tipizzati invece di puntatori generici, rendendo impossibili
  conversioni errate o valori nulli in questa chiamata interna.
- Anche il contesto delle richieste Web conserva ora direttamente un puntatore
  al WebServer, invece di un puntatore generico convertito ripetutamente.
- La ricezione HTTP del WebServer limita dimensione di intestazioni e contenuti,
  gestisce correttamente gli overflow durante la crescita del buffer e valida
  `Content-Length` senza leggere oltre la riga ricevuta.
- I file statici del pannello Web rifiutano ora percorsi anomali, non tentano
  allocazioni oltre il limite tecnico dell'API Windows e restituiscono un errore
  controllato se la memoria disponibile non basta.
- Le preferenze delle colonne del pannello Web verificano ora sempre i limiti
  dell'indice ricevuto: una richiesta alterata non può più scrivere fuori dai
  rispettivi array interni.
- Le preferenze Web ignorano ora valori negativi per velocità, capacità e
  limiti di connessione, evitando conversioni involontarie in valori enormi.
- Le sessioni del pannello Web usano ora token casuali a 128 bit, invece dei
  numeri pseudo-casuali prevedibili della versione originale.
- I filtri di ricerca del pannello Web trattano valori negativi o troppo grandi
  in modo sicuro, senza overflow nelle dimensioni dei file o nella disponibilità.
- Le sessioni e i tentativi di accesso del pannello Web sono ora sincronizzati
  tra le richieste concorrenti; anche i log usano l'indirizzo della richiesta
  corretta.
- La compressione delle pagine Web usa un buffer di dimensione verificata e
  scritture binarie sicure per intestazione e trailer gzip.
- La copia del testo negli Appunti usa ora buffer con dimensione esplicita,
  sostituendo le ultime copie testuali non delimitate presenti nel sorgente.
- La selezione ricorsiva delle sottocartelle condivise evita ora collegamenti e
  junction di Windows, ed esegue la visita in modo iterativo per non esaurire
  lo stack su strutture molto profonde.
- La lettura dei tag MP3/Xing ora gestisce correttamente gli header VBR completi
  (frame, byte, TOC e scala), evitando un arresto durante la scansione dei file
  condivisi.
- Il primo aggiornamento della libreria ID3 rimuove le copie e le formattazioni
  testuali senza limite, corregge il buffer temporaneo dei file dei tag e
  riattiva i controlli di sicurezza standard del compilatore per la libreria.
- L'importazione di dati binari nei tag verifica ora apertura, ricerca,
  dimensione e lettura completa del file; la memoria temporanea viene gestita
  automaticamente anche nei casi di errore.
- Le API ID3 per percorso, buffer e liste di frame rifiutano ora dimensioni
  fuori limite o riferimenti nulli prima di copiare i dati, evitando overflow
  dei percorsi e conversioni non sicure sulle build a 64 bit.
- In `Preferenze -> Display` e disponibile il selettore `Tema / Theme` con
  `Modern light`, `Aurora light` e `Classic Windows`. Aurora sostituisce il
  precedente tema scuro con una base chiara sfumata fra azzurro intenso e
  lilla, superfici leggermente colorate e accenti viola/turchese. La scelta viene applicata
  subito con il pulsante Applica, rispetta il contrasto elevato di Windows e
  viene salvata nel piccolo file `eMuleNextTheme.ini` accanto alla configurazione,
  quindi funziona anche nella versione portatile senza alterare i parametri di
  rete. Pulsanti e toolbar ricevono colori coerenti con il tema scelto. Aurora
  ha superato compilazione e collegamento Release sia Win32 sia x64.
- La procedura guidata del primo avvio usa ora un'illustrazione originale
  `Modern light`, un emblema di rete piu nitido nell'intestazione e titoli
  Segoe UI semibold; le pagine, le opzioni e tutte le traduzioni restano
  invariate.
- Il campo Alias della procedura guidata parte sempre dal link della repository
  `https://github.com/favoritejonny/Emule-Next`; l'utente puo naturalmente
  sostituirlo prima di completare la configurazione.
- L'avviso finale della procedura guidata ha spazio per piu righe, cosi le
  traduzioni lunghe (compreso l'italiano) non vengono piu tagliate.
- Il titolo finale della procedura guidata ha ora una larghezza maggiore per
  visualizzare per intero `Completamento procedura guidata`.
- Le 15 categorie di `Opzioni` usano ora icone vettoriali nitide e coerenti
  con la palette eMule Next; le vecchie icone restano come fallback automatico
  se il rendering grafico di Windows non fosse disponibile.
- Anche l'icona nell'intestazione della pagina selezionata in `Opzioni` usa la
  stessa versione moderna mostrata nell'elenco a sinistra; i due lati restano
  cosi coerenti durante ogni cambio di categoria.
- Il menu `Strumenti` usa ora icone vettoriali antialias per cartella,
  conversione, procedura guidata, filtro IP, collegamenti e pianificazione.
  Le icone vengono create al volo con trasparenza, senza cambiare i comandi;
  la Release e stata ricompilata e collegata con successo per Win32 e x64.
- La composizione dei percorsi con limite `MAX_PATH` usa ora l'API sicura di
  Windows prima di scrivere nel buffer. Un percorso non valido o troppo lungo
  viene rifiutato senza eseguire copie oltre il limite, anche durante
  l'importazione di parti di download.
- La schermata iniziale identifica `Jonny Favorite` come responsabile del
  progetto e `Eddy` come assistente IA per il supporto tecnico; il riferimento
  a Merkur resta come attribuzione dell'opera eMule originale.
- Una nuova configurazione riceve otto server eD2K statici, verificati l'11
  agosto 2026 sulla lista eMule-Security. Il file `staticservers.dat` viene
  creato una sola volta, non sovrascrive mai le scelte dell'utente e non scarica
  automaticamente liste di server da fonti esterne.
- La predisposizione ARM64 e le librerie statiche gia compilate sono conservate
  per un aggiornamento futuro, ma non rientrano nella prima release, nei test
  obbligatori o nei pacchetti da distribuire.
- La revisione multimediale ha eliminato accessi non allineati e calcoli che
  potevano andare oltre i limiti nei parser RIFF/WAV/AVI, RealMedia e Windows
  Media. I metadati ID3 e MediaInfo verificano ora i campi facoltativi e la
  durata MPEG non puo andare in overflow con dati corrotti.
- La generazione delle anteprime video valida le strutture restituite da
  DirectShow, libera sempre i buffer COM e limita immagini o buffer anomali.
  `PreviewApps.dat` conserva correttamente gli argomenti del lettore esterno;
  l'avvio del lettore e inoltre sicuro anche nelle build a 64 bit.
- I file INI storici supportano valori UTF-8 lunghi e interi DWORD completi,
  rifiutano blob binari non validi e mantengono la compatibilita con i valori
  negativi usati come sentinelle nelle vecchie configurazioni.
- Prima di aprire un file `.iso` completato, eMule Next mostra ora un avviso
  specifico con scelta predefinita negativa: Windows potrebbe montare
  l'immagine, quindi l'utente deve prima verificarne la provenienza e
  analizzarla con l'antivirus. L'avviso e disponibile anche in italiano.
- Le verifiche Release mirate dei moduli modificati e il collegamento degli
  eseguibili Win32 e x64 sono completati il 12 agosto 2026. Restano soltanto
  warning provenienti da header di Windows/ATL e da id3lib 3.8.3; gli avvisi
  diretti dei moduli revisionati sono stati rimossi senza silenziarli in modo
  globale.

Gli eseguibili Release Win32 e x64 sono stati ricompilati in sequenza il 29
agosto 2026 dalla revisione pubblica
`27a14542ef7d02785c83a79e908d7685faa55591`. Entrambe le build hanno concluso
con zero errori; gli output riportano la versione `1.0.0-alpha.1`, le corrette
architetture PE e tutte le 43 traduzioni distribuibili.

## Prossimo passo

I due ZIP finali portatili sono stati creati e verificati integralmente. Hanno
59 elementi e 43 traduzioni ciascuno, non contengono profili, dump, log o
simboli di debug e i loro eseguibili corrispondono byte per byte alle build
appena prodotte. Dimensioni e checksum sono registrati in
`PRE_RELEASE_1.0.0_ALPHA1.md`.

Il progetto tester ha estratto e provato entrambi gli ZIP finali il 29 agosto
2026 e li ha dichiarati funzionanti. Nella stessa data, il tag immutabile
`v1.0.0-alpha.1` e la pre-release GitHub sono stati pubblicati sulla revisione
esatta usata per la build. La prima pubblicazione comprende solo i pacchetti
portatili Win32 e x64: installer e ARM64 restano obiettivi di un aggiornamento
futuro e non bloccano questa pre-release.

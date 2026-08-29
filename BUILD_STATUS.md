# Stato di build — eMule Next 1.0.0-alpha.1
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

Il prossimo passo e un breve collaudo manuale di avvio dei due ZIP finali.
Dopo il suo esito positivo verranno creati il tag immutabile
`v1.0.0-alpha.1` e la pre-release GitHub. La prima pubblicazione comprende
solo i pacchetti portatili Win32 e x64: installer e ARM64 restano obiettivi di
un aggiornamento futuro e non bloccano questa pre-release.

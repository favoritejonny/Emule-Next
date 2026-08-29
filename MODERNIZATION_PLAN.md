# Piano incrementale eMule Next

1. **Base 0.72a e identità del prodotto** — completato.
2. **Catena di compilazione** — aggiornare e verificare le librerie esterne,
   con build ripetibili Win32 e x64. zlib, Crypto++, id3lib, miniupnpc e
   mbedTLS sono completate; il nuovo modulo interno NextResizable e in fase
   di validazione Win32/x64 per sostituire la precedente dipendenza. La
   predisposizione ARM64 viene conservata per un aggiornamento futuro.
3. **Affidabilità a 64 bit** — eliminare conversioni dipendenti dalla dimensione
   dei puntatori e attivare gli avvisi utili. La cache delle icone è il primo
   intervento completato.
4. **Audit per moduli** — rete, file system, hashing, UPnP, web server e GUI;
   le scadenze di rete, il salvataggio dei file, le cache grafiche, il recupero
   ZIP/RAR/ACE/ISO e il lettore binario condiviso sono gia stati verificati e
   corretti. Anche i parser RIFF/WAV/AVI, RealMedia, WM, ID3 e le anteprime
   DirectShow, oltre alle configurazioni INI e `PreviewApps.dat`, sono stati
   revisionati. Completata anche la revisione delle opzioni avanzate: valori
   iniziali piu prudenti, spazio libero automatico, file sparsi vincolati alle
   capacita del volume e rimozione dell'automatismo firewall riservato a
   Windows XP. Ogni correzione continua ad avere una verifica mirata per Win32
   e x64.
5. **Esperienza Windows moderna** — DPI, accessibilità, tema e prestazioni
   dell'interfaccia senza togliere le personalizzazioni classiche.
6. **Distribuzione** — installer e pacchetti portatili separati per Win32 e
   x64, con test di aggiornamento e disinstallazione su Windows pulito. ARM64
   verra valutato in un aggiornamento successivo, se utile.

Nessuna fase modifica il protocollo eD2K/Kad senza analisi di compatibilità e
test di interoperabilità.

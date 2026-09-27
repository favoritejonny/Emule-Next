# Collaudo del nuovo avvio guidato - aggiornato il 7 settembre 2026

Queste modifiche sono presenti nelle build locali Release Win32 e x64. I
pacchetti pubblicati in precedenza non vengono sostituiti automaticamente.

Usare le copie portable di prova, una alla volta. Non cancellare le proprie
configurazioni: una nuova cartella estratta da ciascuno ZIP parte senza profilo.
Questa versione locale non sostituisce l'Alpha pubblicata.

## Percorso

Benvenuto → nome e avvio → porte e configurazione automatica del router →
test facoltativo della velocità → reti eD2K/Kad e lista server → Fine.

- Le porte proposte possono restare invariate. Se sono già inoltrate nel
  router, inserire quelle corrette. Disabilitare UDP disabilita anche Kad.
- PCP, NAT-PMP e UPnP sono spiegati nella stessa pagina delle porte. La casella
  di configurazione automatica è selezionata al primo avvio e negozia il
  protocollo compatibile; l'utente può deselezionarla. La richiesta parte solo
  dopo Fine e richiede una rete domestica idonea senza VPN.
- Non è più presente il collegamento al pannello Firewall Windows. Il normale
  avviso di autorizzazione di Windows resta indipendente dalla procedura.
  Il firewall non viene disattivato né modificato dalla procedura.
- Il nuovo test della velocità è volontario. Trasferisce dati temporanei tramite
  Cloudflare (massimo circa 100 MB), che può vedere l'indirizzo IP; eMule Next
  non invia né conserva il risultato fuori dal PC. Download e upload misurati
  sono modificabili prima dell'applicazione. Se applicati, il limite upload è
  impostato all'80% e il download resta senza limite.
- `Aggiorna e carica lista di server eD2K da Internet` è selezionata al primo
  avvio. Con eD2K attivo, dopo Fine il client scarica via HTTPS un `server.met`,
  filtra e unisce le voci senza cancellare i server personali. La lista inclusa
  resta disponibile come ripiego se Internet o la fonte non rispondono. La
  connessione sicura ai server non è selezionata per impostazione predefinita.

## Cose da provare su Win32 e x64

1. Avanzare e tornare indietro: testi leggibili, valori e scelte conservati.
2. Primo avvio senza spunta server: nessuna lista predefinita aggiunta.
3. Primo avvio con spunta ed eD2K: download HTTPS, fusione e salvataggio dei
   server ammessi dal filtro IP; simulare anche rete assente per il ripiego.
4. Scegliere solo Kad: la spunta server non è utilizzabile e non aggiunge nulla.
5. Selezionare le opzioni, poi annullare: nessuna lista o nuovo consenso salvato.
6. Riaprire la procedura da Strumenti con un profilo già usato: non perdere
   server personali, nomi, priorità, preferenze o regole già impostate.
7. Ripetere l'aggiunta: nessun duplicato; gli eventuali server già presenti
   non devono cambiare nome, priorità o conteggio dei tentativi falliti.
8. Eseguire il test velocità, modificarne i due risultati e applicarli: capacità
   in KB/s corrette, upload limitato all'80% e download senza limite. Ripetere
   saltando il test e verificare che i valori precedenti restino invariati.
9. Provare italiano e inglese e, se possibile, altre lingue e zoom Windows
   100%, 150%, 200%. Le traduzioni sono una prima stesura assistita: segnalare
   eventuali formulazioni poco naturali ai beta tester madrelingua.
10. La pagina porte non deve mostrare il pulsante Firewall Windows e deve
    contenere anche la scelta automatica PCP/NAT-PMP/UPnP.
11. In una copia pulita, lasciare selezionato `Connetti automaticamente eMule
    Next al suo avvio`: dopo `Fine` devono partire sia eD2K sia Kad. Se la
    configurazione router è in corso, la connessione deve iniziare subito dopo
    il suo esito o timeout.
12. Ripetere da un'altra copia pulita deselezionando la stessa casella: dopo
    `Fine`, eD2K e Kad devono restare `Disconnesso` e il pulsante principale
    deve continuare a mostrare `Connetti`.

I test automatici offline verificano risorse e regole decisionali. Non
sostituiscono il collaudo visivo della procedura o la prova con un router reale.

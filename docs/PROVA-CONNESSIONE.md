# Prova locale della nuova configurazione connessione

Aggiornato il 27 settembre 2026. Procedura verificata per la Beta 1.
Le nuove copie in `dist/guided-first-start-preview-20260903` includono la
rimozione del collegamento Firewall Windows e la dimensione standard della finestra.
Il nome di versione mostrato nel programma e 1.0.0-beta.1.

Le due copie di prova Win32 e x64 sono portatili e partono senza configurazioni.
Non occorre cancellare le impostazioni delle versioni che usi normalmente.

Al primo avvio il client usa automaticamente la lingua dell'interfaccia di
Windows, non il formato regionale di date e numeri. Se la traduzione non e
disponibile usa l'inglese. Non compare un selettore iniziale: puoi cambiare
lingua in Opzioni > Generale. Una scelta gia salvata non viene sostituita
agli avvii successivi, nemmeno spostando la copia portable su un altro PC.

1. Chiudi gli altri client e apri una sola copia di prova alla volta.
2. Avvia `eMuleNext.exe` nella cartella Win32 oppure x64.
3. Nella procedura iniziale arriva alla pagina della connessione. Puoi lasciare
   le porte proposte. Se la rete è idonea, viene offerta la scelta automatica.
4. Conferma solo sulla rete domestica che gestisci e senza VPN. Con VPN, proxy
   o rete pubblica il programma sospende l'automatismo e spiega il motivo.
   Non serve disattivare la VPN per provare il client: puoi proseguire con la
   configurazione manuale. Il rilevamento non riconosce tutte le VPN e non è
   una protezione contro le perdite di traffico.
5. La scelta viene applicata solo con **Fine**. Prima puoi annullarla con il
   pulsante della pagina, oppure annullare l'intera procedura.
6. Attendi l'esito del router, che ora indica il protocollo PCP, NAT-PMP o
   UPnP usato. Un messaggio positivo conferma la mappatura,
   non garantisce l'ID alto. Autorizza il client nel normale avviso del firewall
   Windows solo per la rete che intendi usare. Nella procedura non compare
   più il collegamento alle impostazioni del firewall; il test porte resta
   facoltativo e non concede autorizzazioni al suo posto.
7. Connettiti e osserva separatamente ID del server e stato Kad. Se necessario
   usa il test manuale delle porte, che apre il servizio ufficiale eMule.
8. Chiudi e riapri la copia: se hai confermato l'automatismo, riproverà sulle
   reti private idonee senza riproporre la procedura iniziale.

Se viene usato PCP o NAT-PMP, lascia acceso il client per almeno 10 minuti
per collaudare i rinnovi delle aperture temporanee. In caso di rifiuto o porta
esterna diversa viene mostrato un errore, senza cambiare le porte del client.
Le regole NAT-PMP non vengono cancellate esplicitamente: alla chiusura o
disattivazione si interrompono i rinnovi e scadono quando stabilito dal router.
Anche tentativi incompleti possono lasciare regole temporanee fino alla scadenza.
Non serve cambiare le impostazioni di sicurezza del router per forzare il test.

Per riaprire la procedura usa Strumenti > Procedura guidata. Le opzioni già
salvate vengono riportate nelle pagine: non dovrebbero cambiare se lasciate
invariate. Controlla in particolare alias, avvio automatico e priorità.

Annota: architettura provata, messaggio mostrato, rete privata/pubblica,
eventuale VPN, ID eD2k e stato Kad. Non inviare indirizzi IP, password del
router o dati personali. Sono ancora necessari test su router differenti;
il prototipo non apre automaticamente il firewall e non risolve il CGNAT.

Le 24 nuove scritte della configurazione connessione sono incluse in tutti i
43 moduli lingua distribuiti. Italiano e inglese sono stati rivisti
direttamente; le altre traduzioni costituiscono una prima versione assistita e
andranno perfezionate con il contributo di beta tester madrelingua. L'inglese
resta la lingua di sicurezza se una risorsa futura dovesse mancare.

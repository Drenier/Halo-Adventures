🚀 Halo Adventure

    Progetto accademico per l'esame di Programmazione Procedurale e Laboratorio

    Università degli Studi di Perugia (UniPG)

    Docente: Prof. Simone Santini

    Autore: Erald Bektashi

📖 Presentazione del Progetto

Halo Adventure è un gioco di ruolo/avventura testuale a turni sviluppato in linguaggio C.

Nasce come progetto d'esame per il corso di Programmazione Procedurale e Laboratorio presso l'Università degli Studi di Perugia (UniPG). L'obiettivo del progetto è applicare in modo concreto e completo i concetti fondamentali del C procedurale: strutture dati dinamiche, gestione avanzata della memoria tramite allocazione e deallocazione manuale (pointer & dynamic memory allocation), e modularizzazione del codice.
🎮 Meccaniche e Regole di Gioco

Il gioco permette da 1 a 4 giocatori di esplorare e combattere attraverso una mappa composta da due mondi paralleli e speculari:

    Zeta Halo (Mondo Reale): Il mondo di superficie, dove si trovano oggetti, equipaggiamenti e nemici di varia natura.

    Alta Opera (Soprasotto): La dimensione oscura/spaziale, più ostile, dove risiede l'Arbiter (il boss finale).

🏆 Obiettivo

I giocatori devono sopravvivere, raccogliere equipaggiamenti, potenziarsi e viaggiare tra i due mondi con lo scopo di trovare e sconfiggere l'Arbiter. Eliminato l'Arbiter, la galassia è salva e la partita termina con la vittoria dei giocatori rimasti in vita.
🕹️ Struttura del Turno

A ogni turno, l'ordine dei giocatori viene casualmente rimescolato per dinamicità. Nel proprio turno, un giocatore può effettuare diverse azioni:

    Movimento: Avanzare o indietreggiare lungo la mappa (se non ci sono nemici attivi nello stesso blocco).

    Cambio Mondo: Aprire un portale per passare da Zeta Halo ad Alta Opera (o tentare di ritornare a Zeta Halo superando un test sulla Fortuna).

    Combattimento: Ingaggiare il nemico presente nella zona corrente. Il danno inflitto e subito si basa sui punti di Attacco, Difesa e su un lancio casuale di dado (1-20), modulato dalla Fortuna.

    Gestione Oggetti: Raccogliere ed equipaggiare/usare oggetti dallo zaino (fino a 3 slot) per curarsi o aumentare le proprie statistiche (Stimolanti, Medikit, Fucili MA5B, Spada Energetica, Scudi Energetici, Corazza Mjolnir, Teschio Heretic, Manufatti Precursore).

⚙️ Architettura Tecnica e Strutture Dati

Il codice è stato suddiviso secondo le buone norme di modularizzazione in tre file distinti:

    main.c: Gestisce il menu principale, il ciclo di esecuzione del programma e l'inizializzazione del generatore casuale.

    gamelib.h: Contiene le definizioni di tipi enumerativi (enum), strutture dati (struct) e prototipi di funzione.

    gamelib.c: Implementa la logica completa del gioco, la gestione del setup, la mappa e i combattimenti.

Punti chiave dell'implementazione:

    Liste Doppiamente Collegate e Speculari:

        Ciascuna zona della mappa è un nodo di una lista doppiamente collegata (avanti e indietro).

        Ogni nodo di Zeta Halo possiede un puntatore bidirezionale (link_AltaOpera / link_ZetaHalo) diretto al corrispettivo nodo in Alta Opera.

    Gestione Dinamica della Memoria:

        La mappa e i giocatori vengono allocati nello heap tramite malloc().

        Viene garantita la deallocazione completa della memoria tramite free() (funzioni dealloca_mappa() e dealloca_giocatori()) per evitare memory leak quando si reinizializza la partita o si esce dal programma.

    Controllo ed Editing della Mappa:

        Durante il setup, è possibile generare una mappa casuale oppure inserire/rimuovere nodi manualmente.

        Prima di iniziare la partita, la funzione chiudi_mappa_func() convalida i vincoli del sistema (minimo 15 zone e presenza di un solo Arbiter).

🛠️ Compilazione ed Esecuzione

Per compilare ed eseguire il progetto su un terminale Linux / MacOS o Windows (tramite GCC / MinGW):
1. Compilazione
Bash

gcc -std=c11 main.c gamelib.c -o halo_adventure

2. Esecuzione
Bash

./halo_adventure

📋 Requisiti del Sistema

    Compilatore C compatibile con lo standard C99 / C11 (es. gcc, clang).

    Terminale con supporto ai comandi clear (Linux/MacOS) o cls (Windows).

👨‍💻 Crediti

    Autore: Erald Bektashi

    Corso di Laurea: Ingegneria - Università degli Studi di Perugia (UniPG)

    Materia: Programmazione Procedurale e Laboratorio

    Docente: Prof. Simone Santini

# 🚀 Halo Adventure

<p align="center">
  <img src="https://img.shields.io/badge/Language-C11-blue.svg" alt="C11">
  <img src="https://img.shields.io/badge/University-UniPG-red.svg" alt="UniPG">
  <img src="https://img.shields.io/badge/Course-Programmazione%20Procedurale-green.svg" alt="Course">
</p>

> **Progetto Accademico per l'esame di *Programmazione Procedurale e Laboratorio***  
> **Università degli Studi di Perugia (UniPG)**  
> **Docente:** Prof. Simone Santini  
> **Autore:** Erald Bektashi  

---

## 📖 Presentazione del Progetto

**Halo Adventure** è un gioco di ruolo e avventura testuale a turni sviluppato in linguaggio **C**. 

Il progetto applica in modo concreto e approfondito i concetti fondamentali della programmazione procedurale:
- **Strutture dati dinamiche** (liste doppiamente collegate e grafi/connessioni bidirezionali).
- **Gestione avanzata della memoria** tramite allocazione/deallocazione dinamica (`malloc`, `free`, prevenzione dei memory leak).
- **Modularizzazione del codice** e gestione pulita di header e librerie custom.

---

## 🎮 Meccaniche e Regole di Gioco

Il gioco supporta da **1 a 4 giocatori** impegnati ad esplorare e combattere attraverso una mappa composta da **due mondi paralleli e speculari**:

| Mondo | Tipo | Descrizione |
| :--- | :--- | :--- |
| **Zeta Halo** | *Mondo Reale* | La superficie dell'anello: contiene oggetti, armi, medikit e nemici Covenant standard. |
| **Alta Opera** | *Soprasotto* | Dimensione oscura e ostile: ospita nemici più temibili e il boss finale (**Arbiter**). |

### 🏆 Obiettivo
I giocatori devono sopravvivere, raccogliere equipaggiamenti, potenziarsi e viaggiare tra i due mondi per **trovare e sconfiggere l'Arbiter**. Eliminato l'Arbiter, la galassia è salva e la partita termina con la vittoria dei giocatori superstiti.

### 🕹️ Struttura del Turno
In ogni turno l'ordine delle mosse viene rimescolato casualmente. Nel proprio turno, il giocatore può:
- 🏃 **Spostarsi**: Avanzare o indietreggiare (se la zona attuale non è bloccata da nemici).
- 🌀 **Cambiare Mondo**: Attraversare un portale tra Zeta Halo ed Alta Opera (il ritorno richiede un test sulla *Fortuna*).
- ⚔️ **Combattere**: Iniziare uno scontro contro il nemico di zona. Il danno si basa su *Attacco*, *Difesa*, il lancio di un dado (1-20) e la *Fortuna*.
- 🎒 **Gestire l'Inventario**: Raccogliere ed usare fino a 3 oggetti (*Stimolanti, Medikit, Fucili MA5B, Spada Energetica, Scudi Energetici, Corazza Mjolnir, Teschio Heretic, Manufatti Precursore*).

---

## ⚙️ Architettura del Codice

Il progetto segue una struttura modulare suddivisa in tre file principali:

```text
├── main.c        # Entry point: gestione menu, ciclo principale e RNG
├── gamelib.h     # Definizione di enum, struct e prototipi di funzione
└── gamelib.c     # Logica di gioco, gestione memoria, mappa e combattimenti

Punti Chiave dell'Implementazione

    Liste Doppiamente Collegate: Ciascuna zona possiede puntatori avanti e indietro.

    Collegamento Tra Mondi: Ogni nodo in Zeta Halo ha un puntatore diretto (link_AltaOpera) al rispettivo nodo in Alta Opera e viceversa.

    Validazione Mappa: La funzione chiudi_mappa_func() verifica che ci siano almeno 15 zone ed un solo Arbiter prima di consentire l'avvio della partita.

🛠️ Compilazione ed Esecuzione
Requisiti

    Compilatore C compatibile con lo standard C99 / C11 (es. gcc, clang).

1. Compilazione
Bash

gcc -std=c11 main.c gamelib.c -o halo_adventure

2. Esecuzione
Bash

./halo_adventure

👨‍💻 Crediti

    Autore: Erald Bektashi

    Ateneo: Università degli Studi di Perugia (UniPG)

    Materia: Programmazione Procedurale e Laboratorio

    Docente: Prof. Simone Santini

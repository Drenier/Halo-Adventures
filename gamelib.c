#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

// --- VARIABILI GLOBALI ---
static struct Zona_ZetaHalo* zona_iniziale_ZH = NULL; // Spawn point del giocatore in Zeta Halo
static struct Zona_AltaOpera* zona_iniziale_AO = NULL; // Spawn point del giocatore in Alta Opera
static struct Giocatore* giocatori[4] = {NULL, NULL, NULL, NULL}; // Array per gestire fino a 4 giocatori
static int n_giocatori = 0;
static int mappa_chiusa = 0; // Diventa 1 quando la mappa è stata creata correttamente
static int gioco_impostato = 0; // Diventa 1 quando il gioco è stato impostato correttamente
static char ultimi_vincitori[3][50] = {"Nessuno", "Nessuno", "Nessuno"}; // nomi degli ultimi 3 vincitori
static int index_vincitore = 0; // per scorrere i vincitori
static int ultimi_turni = 0;    // numero di turni dell'ultima partita

// --- FUNZIONI DI SUPPORTO CHE RICHIAMO SE MI SERVONO ---

// La richiamo per pulire il terminale dopo uno scanf, se no basta un INVIO e congratulazioni hai sbloccato segmentation fault
static void pulisci_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Il dado e il random in generale che mi serve per far funzionare certe meccaniche
static int numero_random(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void pulisci_schermo() {
    #ifdef _WIN32
        system("cls"); // Comando per Windows non si sa mai
    #else
        system("clear"); // Comando per Unix/Linux/MacOS
    #endif
    }

static void stampa_tipo_zona(Tipo_zona t) {
    switch(t) {
        case avamposto_unsc: printf("Avamposto UNSC"); break;
        case nave_covenant: printf("Nave Covenant"); break;
        case rovine_precursore: printf("Rovine Precursore"); break;
        case palude_flood: printf("Palude Flood"); break;
        case base_orbitale: printf("Base Orbitale"); break;
        case hangar_spaziale: printf("Hangar Spaziale"); break;
        case relitto_spaziale: printf("Relitto Spaziale"); break;
        case laboratorio_precursore: printf("Laboratorio Precursore"); break;
        case avamposto_esiliati: printf("Avamposto Esiliati"); break;
        case archivio_ancestrale: printf("Archivio Ancestrale"); break;
    }
}

static void stampa_nemico(Tipo_nemico t) {
    switch(t) {
        case Nessun_covenant: printf("Nessuno"); break;
        case Flood: printf("Flood"); break;
        case Jackal: printf("Jackal"); break;
        case Grunt: printf("Grunt"); break;
        case Brute: printf("Brute"); break;
        case Elite: printf("Elite"); break;
        case Arbiter: printf("Arbiter"); break;
    }
}

static void stampa_oggetto(Tipo_oggetto t) {
    switch(t) {
        case Nessun_oggetto: printf("Vuoto"); break;
        case Stimolante_unsc: printf("Stimolante UNSC"); break;
        case Medikit: printf("Medikit"); break;
        case Fucile_assalto_MA5BICWS: printf("Fucile d'Assalto MA5B ICWS"); break;
        case Spada_energetica: printf("Spada Energetica"); break;
        case Scudo_energetico: printf("Scudo Energetico"); break;
        case Corazza_mjolnir: printf("Corazza Mjolnir"); break;
        case Teshio_heretic: printf("Teshio Heretic"); break;
        case Manufatti_precursore: printf("Manufatti Precursore"); break;
    }
}

// --- PULIZIA DELLA MEMORIA (Per non far crashare il PC) ---

// Ripulisco la Ram, se decido di chiudere il gioco o rigenero la mappa
static void dealloca_mappa() { // naturalmente sia zh e sia ao
    struct Zona_ZetaHalo* temp_zh = zona_iniziale_ZH;
    while (temp_zh != NULL) {
        struct Zona_ZetaHalo* next = temp_zh->avanti;
        free(temp_zh);
        temp_zh = next;
    }
    zona_iniziale_ZH = NULL;

    struct Zona_AltaOpera* temp_ao = zona_iniziale_AO;
    while (temp_ao != NULL) {
        struct Zona_AltaOpera* next = temp_ao->avanti;
        free(temp_ao);
        temp_ao = next;
    }
    zona_iniziale_AO = NULL;
    mappa_chiusa = 0;
}
// Ripulisco la Ram dei giocatori, se decido di ricominciare il gioco
static void dealloca_giocatori() {
    for (int i = 0; i < 4; i++) {
        if (giocatori[i] != NULL) {
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }
    n_giocatori = 0;
    gioco_impostato = 0;
}

// --- FUNZIONI DI SUPPORTO PER LA GESTIONE DELLA MAPPA ---
static int conta_zone() {
    int count = 0;
    struct Zona_ZetaHalo* p = zona_iniziale_ZH;
    while (p != NULL) {
        count++;
        p = p->avanti;
    }
    return count;
}

static int conta_arbiter() {
    int count = 0;
    struct Zona_AltaOpera* p = zona_iniziale_AO;
    while (p != NULL) {
        if (p->nemico == Arbiter) count++;
        p = p->avanti;
    }
    return count;
}
// --- GENERAZIONE E MODIFICA MAPPA ---
// Creo una nuova zona collegata (mondoreale + soprasotto) con parametri casuali o vuoti
static void crea_nuova_zona_collegata(struct Zona_ZetaHalo** nuova_zh, struct Zona_AltaOpera** nuova_ao, bool casuale) {
    *nuova_zh = (struct Zona_ZetaHalo*)malloc(sizeof(struct Zona_ZetaHalo));
    *nuova_ao = (struct Zona_AltaOpera*)malloc(sizeof(struct Zona_AltaOpera));

    Tipo_zona tipo = (Tipo_zona)numero_random(0, 9);
    (*nuova_zh)->tipo = tipo;
    (*nuova_ao)->tipo = tipo;

    if (casuale) {
        int r_nem = numero_random(0, 100);
        if (r_nem < 50) (*nuova_zh)->nemico = Nessun_covenant;
        else if (r_nem < 65) (*nuova_zh)->nemico = Jackal; // 15%
        else if (r_nem < 80) (*nuova_zh)->nemico = Grunt;   // 15%
        else if (r_nem < 95) (*nuova_zh)->nemico = Elite;    // 15%
        else (*nuova_zh)->nemico = Flood; // 5%

        r_nem = numero_random(0, 100);
        if (r_nem < 50) (*nuova_ao)->nemico = Nessun_covenant;
        else if (r_nem < 65) (*nuova_ao)->nemico = Flood; // 15%
        else if (r_nem < 80) (*nuova_ao)->nemico = Elite;  // 15%
        else if (r_nem< 95) (*nuova_ao)->nemico = Brute;    // 15%
        else (*nuova_ao)->nemico = Arbiter; // 5%

    int r_ogg = numero_random(0, 100);
    if     (r_ogg < 30)(*nuova_zh)->oggetto = Nessun_oggetto;            // 30%
    else if (r_ogg < 43)(*nuova_zh)->oggetto = Stimolante_unsc;           // 13%
     else if (r_ogg < 56)(*nuova_zh)->oggetto = Fucile_assalto_MA5BICWS;   // 13%
      else if (r_ogg < 69)(*nuova_zh)->oggetto = Scudo_energetico;          // 13%
       else if (r_ogg < 82)(*nuova_zh)->oggetto = Teshio_heretic;            // 13%
        else if (r_ogg < 87)(*nuova_zh)->oggetto = Manufatti_precursore;      // 5%
         else if (r_ogg < 92)(*nuova_zh)->oggetto = Medikit;                   // 5%
          else if (r_ogg < 97)(*nuova_zh)->oggetto = Spada_energetica;          // 5%
           else    (*nuova_zh)->oggetto = Corazza_mjolnir;                       // 2%
    } else {
        (*nuova_zh)->nemico = Nessun_covenant;
        (*nuova_zh)->oggetto = Nessun_oggetto;
        (*nuova_ao)->nemico = Nessun_covenant;
    }

    (*nuova_zh)->link_AltaOpera = *nuova_ao;
    (*nuova_ao)->link_ZetaHalo = *nuova_zh;
    (*nuova_zh)->avanti = (*nuova_zh)->indietro = NULL;
    (*nuova_ao)->avanti = (*nuova_ao)->indietro = NULL;
}

static void genera_mappa() {
    dealloca_mappa();
    
    struct Zona_ZetaHalo* coda_zh = NULL;
    struct Zona_AltaOpera* coda_ao = NULL;

    for (int i = 0; i < 15; i++) {
        struct Zona_ZetaHalo* nuovo_zh;
        struct Zona_AltaOpera* nuovo_ao;
        crea_nuova_zona_collegata(&nuovo_zh, &nuovo_ao, true);

        if (zona_iniziale_ZH == NULL) {
            zona_iniziale_ZH = nuovo_zh;
            zona_iniziale_AO = nuovo_ao;
        } else {
            coda_zh->avanti = nuovo_zh;
            nuovo_zh->indietro = coda_zh;
            coda_ao->avanti = nuovo_ao;
            nuovo_ao->indietro = coda_ao;
        }
        coda_zh = nuovo_zh;
        coda_ao = nuovo_ao;
    }

    if (conta_arbiter() == 0) {
        if (coda_ao != NULL) coda_ao->nemico = Arbiter; 
    } else {
        int found = 0;
        struct Zona_AltaOpera* curr = zona_iniziale_AO;
        while(curr != NULL) {
            if(curr->nemico == Arbiter) {
                if(found == 0) found = 1;
                else curr->nemico = Elite;
            }
            curr = curr->avanti;
        }
        if (found == 0 && coda_ao != NULL) coda_ao->nemico = Arbiter;
    }
    printf("Mappa generata con 15 zone.\n");
}

static void inserisci_zona() {
    int pos;
    int input_ok;

    do {
        input_ok = 0;
        printf("Inserire posizione (0 per testa, -1 per coda, o 1 indice): ");
        if (scanf("%d", &pos) != 1) {
            printf("Errore: devi inserire un numero!\n");
            pulisci_buffer();
        } else {
            pulisci_buffer();
            input_ok = 1;
        }
    } while (!input_ok);

    struct Zona_ZetaHalo* nuovo_zh;
    struct Zona_AltaOpera* nuovo_ao;
    crea_nuova_zona_collegata(&nuovo_zh, &nuovo_ao, false); 

    int temp;

    do {
        printf("Scegli nemico ZH (0:Nessuno, 1:Jackal, 2:Grunt, 3:Flood, 4:Elite): ");
        if (scanf("%d", &temp) != 1) {
            printf("Inserisci un numero valido!\n");
            pulisci_buffer();
            temp = -1; 
        } else {
            pulisci_buffer();
            if (temp < 0 || temp > 4) printf("Numero fuori range (0-4)!\n");
        }
    } while (temp < 0 || temp > 4);

    if(temp == 1) nuovo_zh->nemico = Jackal;
    else if(temp == 2) nuovo_zh->nemico = Grunt;
    else if(temp == 3) nuovo_zh->nemico = Flood;
    else if(temp == 4) nuovo_zh->nemico = Elite;
    else nuovo_zh->nemico = Nessun_covenant;

    do {
        printf("Scegli nemico SS (0:Nessuno, 1:Flood, 2:Elite, 3:Brute, 4:Arbiter): ");
        if (scanf("%d", &temp) != 1) {
            printf("Inserisci un numero valido!\n");
            pulisci_buffer();
            temp = -1; 
        } else {
            pulisci_buffer();
            if (temp < 0 || temp > 4) printf("Numero fuori range (0-4)!\n");
        }
    } while (temp < 0 || temp > 4);

    if(temp == 1) nuovo_ao->nemico = Flood;
    else if(temp == 2) nuovo_ao->nemico = Elite;
    else if(temp == 3) nuovo_ao->nemico = Brute;
    else if(temp == 4) nuovo_ao->nemico = Arbiter;
    else nuovo_ao->nemico = Nessun_covenant;

    do {
        printf("Scegli oggetto ZH (0-8):\n");
        printf("0:Nessuno, 1:Stimolante, 2:Medikit, 3:MA5B, 4:Spada, 5:Scudo, 6:Mjolnir, 7:Teschio, 8:Manufatti: ");
        if (scanf("%d", &temp) != 1) {
            printf("Inserisci un numero valido!\n");
            pulisci_buffer();
            temp = -1; 
        } else {
            pulisci_buffer();
            if (temp < 0 || temp > 8) printf("Numero fuori range (0-8)!\n");
        }
    } while (temp < 0 || temp > 8);

    if(temp >= 1 && temp <= 8) nuovo_zh->oggetto = (Tipo_oggetto)temp;
    else nuovo_zh->oggetto = Nessun_oggetto;

    if (pos == 0 || zona_iniziale_ZH == NULL) {
        nuovo_zh->avanti = zona_iniziale_ZH;
        nuovo_ao->avanti = zona_iniziale_AO;
        if (zona_iniziale_ZH) zona_iniziale_ZH->indietro = nuovo_zh;
        if (zona_iniziale_AO) zona_iniziale_AO->indietro = nuovo_ao;
        zona_iniziale_ZH = nuovo_zh;
        zona_iniziale_AO = nuovo_ao;
    } else {
        struct Zona_ZetaHalo* curr_zh = zona_iniziale_ZH;
        struct Zona_AltaOpera* curr_ao = zona_iniziale_AO;
        int i = 0;
        while (curr_zh->avanti != NULL && (pos == -1 || i < pos - 1)) {
            curr_zh = curr_zh->avanti;
            curr_ao = curr_ao->avanti;
            i++;
        }
       
        nuovo_zh->avanti = curr_zh->avanti;
        nuovo_zh->indietro = curr_zh;
        if (curr_zh->avanti) curr_zh->avanti->indietro = nuovo_zh;
        curr_zh->avanti = nuovo_zh;

        nuovo_ao->avanti = curr_ao->avanti;
        nuovo_ao->indietro = curr_ao;
        if (curr_ao->avanti) curr_ao->avanti->indietro = nuovo_ao;
        curr_ao->avanti = nuovo_ao;
    }
    printf("Zona inserita con successo.\n");
}

static void cancella_zona() {
    if (!zona_iniziale_ZH) { printf("Mappa vuota.\n"); return; }
    
    int pos;
    printf("Posizione da cancellare: ");
    if (scanf("%d", &pos) != 1 || pos < 0) { 
        printf("Input non valido (inserisci un numero).\n");
        pulisci_buffer();
        return;
    };
    pulisci_buffer();

    struct Zona_ZetaHalo* del_zh = zona_iniziale_ZH;
    struct Zona_AltaOpera* del_ao = zona_iniziale_AO;

    int i = 0;
    while(del_zh != NULL && i < pos) {
        del_zh = del_zh->avanti;
        del_ao = del_ao->avanti;
        i++;
    }

    if(del_zh == NULL) { printf("Posizione non valida.\n"); return; }

    if(del_zh->indietro) del_zh->indietro->avanti = del_zh->avanti;
        else zona_iniziale_ZH = del_zh->avanti;
    if(del_zh->avanti) del_zh->avanti->indietro = del_zh->indietro;

    if(del_ao->indietro) del_ao->indietro->avanti = del_ao->avanti;
        else zona_iniziale_AO = del_ao->avanti;
    if(del_ao->avanti) del_ao->avanti->indietro = del_ao->indietro;

        free(del_zh);
        free(del_ao);
    printf("Zona rimossa.\n");
}

static void stampa_mappa_dettagliata() {
    int scelta = -1;
    printf("1. Zeta Halo\n2. Alta Opera\nScelta: ");
    
    if (scanf("%d", &scelta) != 1) {
        pulisci_buffer();
        printf("Input non valido (inserisci 1 o 2).\n");
        return;
    }
    pulisci_buffer();

    if (scelta == 1) {
        struct Zona_ZetaHalo* curr = zona_iniziale_ZH;
        int i = 0;
        printf("\n--- ZETA HALO ---\n");
        while(curr) {
            printf("[%d] ", i++);
            stampa_tipo_zona(curr->tipo);
            printf(" | Nem: "); stampa_nemico(curr->nemico);
            printf(" | Ogg: "); stampa_oggetto(curr->oggetto);
            printf("\n");
            curr = curr->avanti;
        }
    } else if (scelta == 2) {
        struct Zona_AltaOpera* curr = zona_iniziale_AO;
        int i = 0;
        printf("\n--- ALTA OPERA ---\n");
        while(curr) {
            printf("[%d] ", i++);
            stampa_tipo_zona(curr->tipo);
            printf(" | Nem: "); stampa_nemico(curr->nemico);
            printf("\n");
            curr = curr->avanti;
        }
    } else {
        printf("Scelta non valida.\n");
    }
}

static void stampa_zona() {
    int pos;
    printf("Indice zona: ");
    if (scanf("%d", &pos) != 1 || pos < 0) {
        pulisci_buffer();
        printf("Input non valido (inserisci un numero).\n");
        return;
    }
    pulisci_buffer();

    struct Zona_ZetaHalo* zh = zona_iniziale_ZH;
    struct Zona_AltaOpera* ao = zona_iniziale_AO;
    int i = 0;

    while (zh && i < pos) {
        zh = zh->avanti;
        ao = ao->avanti;
        i++;
    }

    if (!zh) {
        printf("Zona inesistente.\n");
        return;
    }

    printf("\n[Zeta Halo]\n");
    stampa_tipo_zona(zh->tipo);
    printf(" | Nemico: "); stampa_nemico(zh->nemico);
    printf(" | Oggetto: "); stampa_oggetto(zh->oggetto);

    printf("\n[Alta Opera]\n");
    stampa_tipo_zona(ao->tipo);
    printf(" | Nemico: "); stampa_nemico(ao->nemico);
    printf("\n");
}

static void chiudi_mappa_func() {
    int zone = conta_zone();
    int demo = conta_arbiter();
    
    if (zone < 15) {
        printf("Errore: Servono almeno 15 zone (attuali: %d).\n", zone);
        return;
    }
    if (demo != 1) {
        printf("Errore: Deve esserci per forza UN SOLO ARBITER (attuali: %d).\n", demo);
        return;
    }
    mappa_chiusa = 1;
    pulisci_schermo();
    printf("\nMappa chiusa e validata.\n\n");
}

// --- IMPOSTAZIONE GIOCO ---

void imposta_gioco() {
    dealloca_giocatori(); 
    dealloca_mappa(); 

    ultimi_turni = 0;

    printf("Numero giocatori (1-4): ");


n_giocatori = -1;

while (n_giocatori < 0 || n_giocatori > 4) {
    if (scanf("%d", &n_giocatori) != 1) {
        printf("Che stai facendo? Cambia input metti un NUMERO tra 1 e 4!\n");
        pulisci_buffer();
        continue;
            pulisci_schermo(); // Pulisce lo schermo dopo l'input per una migliore esperienza utente
            printf("Che stai facendo? Cambia input metti un NUMERO tra 1 e 4!\n");   
        }
    }    

    pulisci_buffer();

    if(n_giocatori < 1 || n_giocatori > 4) n_giocatori = 1;

    int spartan_scelto = 0;

    for(int i=0; i<n_giocatori; i++) {
        giocatori[i] = (struct Giocatore*)malloc(sizeof(struct Giocatore));
        
        // FIX: Lettura nome corretta
        printf("NOME DEL GIOCATORE %d: ", i+1);
        fgets(giocatori[i]->nome, 50, stdin);
        giocatori[i]->nome[strcspn(giocatori[i]->nome, "\n")] = 0; 

        giocatori[i]->attacco = numero_random(1, 20);
        giocatori[i]->difesa = numero_random(1, 20);
        giocatori[i]->fortuna = numero_random(1, 20);
        giocatori[i]->salute = 100;
        giocatori[i]->mondo = 0;
        giocatori[i]->oggetti_posseduti = 0;
        for(int k=0; k<3; k++) giocatori[i]->zaino[k] = Nessun_oggetto;

        printf("Stats %s: Attacco %d, Difesa %d, Fortuna %d\n", giocatori[i]->nome, 
               giocatori[i]->attacco, giocatori[i]->difesa, giocatori[i]->fortuna);

        printf("    Modifica stats:\n       1. +3 Attacco / -3 Difesa\n        2. +3 Difesa / -3 Attacco\n");
if (!spartan_scelto) {
    printf("         3. Diventa Spartan (+4 Attacco/+4 Difesa/-7 Fortuna)\n");
    if (giocatori[i]->fortuna < 8) {
        printf("            [BLOCCATO: Hai troppa poca fortuna per questa trasformazione!]\n");
    }
}
printf("          0. Nessuna modifica\nScelta: ");

int mod = -1;

while (mod < 0 || mod > 3 || (mod == 3 && (spartan_scelto || giocatori[i]->fortuna < 8))) {
    
    if (scanf("%d", &mod) != 1) {
        printf("My man che stai facendo? Cambia input metti un NUMERO!\n");
        pulisci_buffer();
        mod = -1;
        continue;
    }
    
    pulisci_buffer();

    if (mod == 3 && giocatori[i]->fortuna < 8) {
        printf("Non hai abbastanza fortuna (minimo 8) per diventare Spartan! Scegli altro.\n");
        mod = -1; 
    } else if (mod == 2 && giocatori[i]->attacco < 4) {
        printf("Non hai abbastanza attacco (minimo 4) per questa modifica! Scegli altro.\n");
        mod = -1;
    } else if (mod == 1 && giocatori[i]->difesa < 4) {
        printf("Non hai abbastanza difesa (minimo 4) per questa modifica! Scegli altro.\n");
        mod = -1;
    } else if (mod < 0 || mod > 3 || (mod == 3 && spartan_scelto)) {
        printf("Questa scelta non esiste o il personaggio è già stato preso. Riprova!\n");
        mod = -1;
    }
}
        if (mod == 1) {
            giocatori[i]->attacco += 3;
            if(giocatori[i]->difesa > 3) giocatori[i]->difesa -= 3; else giocatori[i]->difesa = 0;
        } else if (mod == 2) {
            if(giocatori[i]->attacco > 3) giocatori[i]->attacco -= 3; else giocatori[i]->attacco = 0;
            giocatori[i]->difesa += 3;
        } else if (mod == 3 && !spartan_scelto) {
            char buffer[100];
            sprintf(buffer, "Spartan (%s)", giocatori[i]->nome);
            strcpy(giocatori[i]->nome, buffer);
            giocatori[i]->attacco += 4;
            giocatori[i]->difesa += 4;
            if(giocatori[i]->fortuna > 7) giocatori[i]->fortuna -= 7; else giocatori[i]->fortuna = 0;
            spartan_scelto = 1;
        }
    

        if (mod != 0) { 
            printf("\nEcco le tue stats aggiornate %s \n", giocatori[i]->nome);
            printf("New Stats: Attacco: %d | Difesa: %d | Fortuna: %d\n\n", 
                   giocatori[i]->attacco, 
                   giocatori[i]->difesa, 
                   giocatori[i]->fortuna);
        }
    }

    gioco_impostato = 1;

int scelta_m = 0;
    do {
        printf("\n--- CONFIGURAZIONE MAPPA ---\n");
        printf("1. Genera Mappa Casuale\n");
        printf("2. Inserisci Zona\n");
        printf("3. Cancella Zona\n");
        printf("4. Stampa Mappa\n");
        printf("5. Stampa Zona\n");
        printf("6. Chiudi Mappa (Termina setup)\n");
        printf("Scelta: ");
        
        scelta_m = -1; 
        if (scanf("%d", &scelta_m) != 1) {
            printf("\nDevi inserire un numero!\n");
            pulisci_buffer(); 
            continue;
        }
        pulisci_buffer();

        switch(scelta_m) {
            case 1: genera_mappa(); break;
            case 2: inserisci_zona(); break;
            case 3: cancella_zona(); break;
            case 4: stampa_mappa_dettagliata(); break;
            case 5: stampa_zona(); break;
            case 6: chiudi_mappa_func(); break;
            default: printf("Scelta non valida, riprova.\n"); break;
        }
    } while (!mappa_chiusa);
}

// --- LOGICA DI GIOCO ---

static void stampa_giocatore(struct Giocatore* g) {
    printf("\n--- %s ---\n", g->nome);
    printf("Mondo: %s\n", g->mondo == 0 ? "Zeta Halo" : "Alta Opera");
    printf("HP: %d | Att: %d | Dif: %d | Fort: %d\n", g->salute, g->attacco, g->difesa, g->fortuna);
    printf("Zaino: ");
    for(int i=0; i<3; i++) { stampa_oggetto(g->zaino[i]); printf(" "); }
    printf("\n");
}

static void stampa_zona_corrente(struct Giocatore* g) {
    printf("\nPosizione Attuale:\n");
    if(g->mondo == 0) {
        printf("Zeta Halo: "); stampa_tipo_zona(g->pos_ZetaHalo->tipo);
        printf("\nNemico: "); stampa_nemico(g->pos_ZetaHalo->nemico);
        printf("\nOggetto: "); stampa_oggetto(g->pos_ZetaHalo->oggetto);
    } else {
        printf("Alta Opera: "); stampa_tipo_zona(g->pos_AltaOpera->tipo);
        printf("\nNemico: "); stampa_nemico(g->pos_AltaOpera->nemico);
    }
    printf("\n");
}

static int nemico_presente(struct Giocatore* g) {
    if (g->mondo == 0) return g->pos_ZetaHalo->nemico != Nessun_covenant;
    else return g->pos_AltaOpera->nemico != Nessun_covenant;
}

static void combattimento(struct Giocatore* g, int* nemico_sconfitto_flag) {
    Tipo_nemico nemico = (g->mondo == 0) ? g->pos_ZetaHalo->nemico : g->pos_AltaOpera->nemico;
    
    int forza_nemico = 0;
    if (nemico == Jackal)  forza_nemico = 30;
    if (nemico == Grunt)   forza_nemico = 30;
    if (nemico == Elite)   forza_nemico = 36;
    if (nemico == Flood)   forza_nemico = 40;
    if (nemico == Brute)   forza_nemico = 50;
    if (nemico == Arbiter) forza_nemico = 90;

    printf("\n*** INIZIO COMBATTIMENTO CONTRO "); stampa_nemico(nemico); printf(" ***\n");

    while (g->salute > 0 && forza_nemico > 0) {
        int tiro = numero_random(1, 20);
        int danno_inflitto = (g->attacco + tiro) / 2;
        if (danno_inflitto < 1) danno_inflitto = 1;

        forza_nemico -= danno_inflitto;
        printf("Attacchi con forza %d! (Nemico HP rimasti stima: %d)\n", danno_inflitto, forza_nemico);

        if (forza_nemico <= 0) break;

        tiro = numero_random(1, 20);
        int danno_subito = (forza_nemico / 2 + tiro) - g->difesa;
        if (danno_subito < 1) danno_subito = 1;

        if (numero_random(1, 20) < g->fortuna) {
            printf("Colpo di FORTUNA! Schivi parzialmente.\n");
            danno_subito /= 2;
        }

        g->salute -= danno_subito;
        printf("Il nemico colpisce per %d danni! (Tuoi HP: %d)\n", danno_subito, g->salute);
    }

    if (g->salute <= 0) {
        printf("\nSei stato sconfitto...\n");
        *nemico_sconfitto_flag = 0;
    } else {
        printf("\nVITTORIA!\n");
        *nemico_sconfitto_flag = 1;
        
        if (nemico == Arbiter) {
            printf("\n#   L'ARBITER E' CADUTO HAI VINTO!     #\n");
            printf("\n#   IL GIOCATORE %s HA SALVATO LA GALASSIA!   #\n", g->nome);
            if (g->mondo == 0) g->pos_ZetaHalo->nemico = Nessun_covenant;
            else g->pos_AltaOpera->nemico = Nessun_covenant;
        } else {
            if (numero_random(1, 100) <= 50) {
                if (g->mondo == 0) g->pos_ZetaHalo->nemico = Nessun_covenant;
                else g->pos_AltaOpera->nemico = Nessun_covenant;
                printf("\nIl nemico si dissolve in polvere...\n\n");
            } else {
                printf("\nIl corpo del nemico rimane a terra.\n\n");
            }
        }
    }
}

static void avanza(struct Giocatore* g, int* ha_mosso, int* nemico_battuto) {
    if (nemico_presente(g) && !(*nemico_battuto)) {
        printf("Non puoi avanzare! C'è un nemico!\n");
        return;
    }

    if (g->mondo == 0) { 
        if (g->pos_ZetaHalo->avanti != NULL) {
            g->pos_ZetaHalo = g->pos_ZetaHalo->avanti;
            g->pos_AltaOpera = g->pos_ZetaHalo->link_AltaOpera;
            printf("\nSei avanzato nello Zeta Halo.\n");
            *ha_mosso = 1;
            *nemico_battuto = 0;
        } else printf("\nSei alla fine della mappa.\n");
    } else { 
        if (g->pos_AltaOpera->avanti != NULL) {
            g->pos_AltaOpera = g->pos_AltaOpera->avanti;
            g->pos_ZetaHalo = g->pos_AltaOpera->link_ZetaHalo;
            printf("Sei avanzato in Alta Opera.\n");
            *ha_mosso = 1;
            *nemico_battuto = 0;
        } else printf("Sei alla fine di Alta Opera.\n");
    }
}

static void indietreggia(struct Giocatore* g, int* ha_mosso, int* nemico_battuto) {
    if (nemico_presente(g) && !(*nemico_battuto)) {
        printf("Non puoi indietreggiare! C'è un nemico!\n");
        return;
    }

    if (g->mondo == 0) {
        if (g->pos_ZetaHalo->indietro != NULL) {
            g->pos_ZetaHalo = g->pos_ZetaHalo->indietro;
            g->pos_AltaOpera = g->pos_ZetaHalo->link_AltaOpera;
            printf("Sei tornato indietro nello Zeta Halo.\n");
            *ha_mosso = 1;
            *nemico_battuto = 0;
        } else printf("Sei all'inizio della mappa.\n");
    } else {
        if (g->pos_AltaOpera->indietro != NULL) {
            g->pos_AltaOpera = g->pos_AltaOpera->indietro;
            g->pos_ZetaHalo = g->pos_AltaOpera->link_ZetaHalo;
            printf("Sei tornato indietro in Alta Opera.\n");
            *ha_mosso = 1;
            *nemico_battuto = 0;
        } else printf("Sei all'inizio della mappa.\n");
    }
}

static void cambia_mondo(struct Giocatore* g, int* ha_mosso, int* nemico_battuto) {
    if (*ha_mosso) {
        printf("Ti sei già mosso in questo turno.\n");
        return;
    }

    if (g->mondo == 0) {
        if (nemico_presente(g) && !(*nemico_battuto)) {
            printf("Non puoi cambiare mondo con un nemico attivo!\n");
            return;
        }
        g->mondo = 1;
        printf("Un portale si apre... entri in Alta Opera!\n");
        *ha_mosso = 1; 
        *nemico_battuto = 0; 
    } else {
        int tiro = numero_random(1, 20);
        printf("Cerchi un'uscita... (Dado: %d vs Fortuna: %d)\n", tiro, g->fortuna);
        if (tiro < g->fortuna) {
            g->mondo = 0;
            printf("Successo! Sei tornato nello Zeta Halo.\n");
            *ha_mosso = 1;
            *nemico_battuto = 0;
        } else {
            printf("Fallimento! Rimani bloccato qui.\n");
            *ha_mosso = 1; 
        }
    }
}

static void raccogli_oggetto(struct Giocatore* g, int nemico_battuto) {
    if (g->mondo == 1) { printf("Non ci sono oggetti utili nel Soprasotto.\n"); return; }
    if (g->pos_ZetaHalo->oggetto == Nessun_oggetto) { printf("Nessun oggetto qui.\n"); return; }
    if (nemico_presente(g) && !nemico_battuto) { printf("Sconfiggi prima il nemico!\n"); return; }
    
    if (g->oggetti_posseduti >= 3) { printf("Zaino pieno!\n"); return; }

    for(int i=0; i<3; i++) {
        if (g->zaino[i] == Nessun_oggetto) {
            g->zaino[i] = g->pos_ZetaHalo->oggetto;
            printf("Hai raccolto: "); stampa_oggetto(g->pos_ZetaHalo->oggetto); printf("\n");
            g->pos_ZetaHalo->oggetto = Nessun_oggetto; 
            g->oggetti_posseduti++;
            return;
        }
    }
}

static void utilizza_oggetto(struct Giocatore* g) {
    if (g->oggetti_posseduti == 0) { printf("Zaino vuoto.\n"); return; }
    
    printf("Scegli oggetto (0-2):\n");
    for(int i=0; i<3; i++) {
        printf("%d: ", i); stampa_oggetto(g->zaino[i]); printf("\n");
    }
    int idx;
    scanf("%d", &idx);
    pulisci_buffer();

    if(idx < 0 || idx > 2 || g->zaino[idx] == Nessun_oggetto) return;

    Tipo_oggetto obj = g->zaino[idx];
    
    switch(obj) {
        case Stimolante_unsc: 
            printf("\nUsi lo stimolante. Recuperi 15 HP!\n\n");
            g->salute += 15;
            break;
        case Medikit:
            printf("\nUsi il medikit. Recuperi 25 HP!\n\n");
            g->salute += 25;
            break;
        case Fucile_assalto_MA5BICWS:
            printf("\nEquipaggi il fucile d'assalto MA5BICWS. (+5 ATK)\n\n");
            g->attacco += 5;
            break;
        case Spada_energetica:
            printf("\nEquipaggi la spada energetica. (+7 ATK)\n\n");
            g->attacco += 7;
            break;
        case Scudo_energetico: 
            printf("\nEquipaggi lo scudo energetico. (+5 DEF)\n\n");
            g->difesa += 5;
            break;
        case Corazza_mjolnir:
            printf("\nEquipaggi la corazza Mjolnir. (+7 DEF)\n\n");
            g->difesa += 7;
            break;
        case Teshio_heretic:
             printf("\nUsi il teschio heretic. (+5 FORT & +10 HP)\n\n");
            g->fortuna += 5;
            g->salute += 10;
            break;
        case Manufatti_precursore:
             printf("\nUsi i manufatti precursore. (+7 FORT & +15 HP)\n\n");
            g->fortuna += 7;
            g->salute += 15;
            break;
        default: break;
    }

    g->zaino[idx] = Nessun_oggetto;
    g->oggetti_posseduti--;
}

void gioca() {
    if (!zona_iniziale_ZH || !zona_iniziale_AO) {
        printf("Errore critico: mappa non inizializzata.\n");
        return;
    }

    if (!gioco_impostato || !mappa_chiusa) {
        pulisci_schermo();
        printf("Errore: Devi prima impostare il gioco!\n\n");
        return;
    }

    for (int i=0; i<n_giocatori; i++) {
        giocatori[i]->pos_ZetaHalo = zona_iniziale_ZH;
        giocatori[i]->pos_AltaOpera = zona_iniziale_AO;
        giocatori[i]->mondo = 0;
        giocatori[i]->salute = 100; 
    }

    int vittoria = 0;
    int game_over = 0;
    int turn_cnt = 0;

    while (!vittoria && !game_over) {
        int ordine[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--) {
        int r = rand() % (i + 1);
        int tmp = ordine[i];
        ordine[i] = ordine[r];
        ordine[r] = tmp;
    }


        int giocatori_vivi = 0;
        for(int k=0; k<n_giocatori; k++) if(giocatori[k] != NULL) giocatori_vivi++;
        if(giocatori_vivi == 0) { game_over = 1; break; }

        printf("\n=== INIZIO TURNO GLOBALE %d ===\n", ++turn_cnt);

        for (int k=0; k<4; k++) {
            int idx = ordine[k];
            if (idx >= n_giocatori || giocatori[idx] == NULL) continue;

            if (conta_arbiter() == 0) { vittoria = 1; break; }

            struct Giocatore* g = giocatori[idx];
            printf("\n--- TOCCA A: %s ---\n", g->nome);
            stampa_zona_corrente(g);

            int ha_mosso = 0;
            int turno_finito = 0;
            int nemico_battuto_in_questo_turno = 0;

            while (!turno_finito && g->salute > 0 && !vittoria) {
                printf("AZIONI: 1.Avanza 2.Indietreggia 3.CambiaMondo 4.Combatti\n");
                printf("        5.InfoGioc 6.InfoZona 7.Raccogli 8.UsaOggetto 9.PASSA\n");
                printf("                                                       Scelta: ");
                int mossa;
                scanf("%d", &mossa);
                pulisci_buffer();

                switch(mossa) {
                    case 1: 
                        if(!ha_mosso) {
                        avanza(g, &ha_mosso, &nemico_battuto_in_questo_turno);
                        stampa_zona_corrente(g);
                        } else { printf("Movimento già usato! (Premi 9 per passare)\n");
                        }
                        break;
                    case 2: 
                        if(!ha_mosso) { 
                        indietreggia(g, &ha_mosso, &nemico_battuto_in_questo_turno);
                        stampa_zona_corrente(g);
                        } else { printf("Movimento già usato! (Premi 9 per passare)\n"); 
                        }
                        break;
                    case 3: 
                        cambia_mondo(g, &ha_mosso, &nemico_battuto_in_questo_turno);
                        stampa_zona_corrente(g);
                        break;
                    case 4:
                        if (nemico_presente(g) && !nemico_battuto_in_questo_turno) {
                            combattimento(g, &nemico_battuto_in_questo_turno);
                        } else {
                            printf("\nNessun nemico combattibile o gia' battuto in questo turno.\n\n");
                        }
                        break;
                    case 5: stampa_giocatore(g); break;
                    case 6: stampa_zona_corrente(g); break;
                    case 7: raccogli_oggetto(g, nemico_battuto_in_questo_turno);
                            stampa_zona_corrente(g);
                            break;
                    case 8: utilizza_oggetto(g); break;
                    case 9: turno_finito = 1; break;
                    default: printf("Comando errato.\n");
                }

                if (g->salute <= 0) {
                    printf("\n%s E' MORTO.\n", g->nome);
                    free(g);
                    giocatori[idx] = NULL;
                    turno_finito = 1;
                }
                
                if (conta_arbiter() == 0) {
                vittoria = 1;
                turno_finito = 1;

            strcpy(ultimi_vincitori[index_vincitore], g->nome); // Mi deve tener conto del nome del vincitore
            index_vincitore = (index_vincitore + 1) % 3;
            ultimi_turni = turn_cnt;
            }

            }
            if (vittoria) break;
        }
        
        giocatori_vivi = 0;
        for(int k=0; k<n_giocatori; k++) if(giocatori[k] != NULL) giocatori_vivi++;
        
        if(giocatori_vivi == 0 && !vittoria) {
            printf("\nTUTTI I GIOCATORI SONO STATI SCONFITTI. GAME OVER.\n");
            game_over = 1;
        }

        if (vittoria) {
            printf("\nCONGRATULAZIONI! IL GIOCO E' FINITO CON UNA VITTORIA!\n");
        }
    }
    termina_gioco();
}

void termina_gioco() {
    dealloca_giocatori();
    dealloca_mappa();
    printf("\nGrazie per aver giocato a Halo Adventure!\n\n");
}

void visualizza_crediti() {
    printf("\n--- CREDITI & STATISTICHE ---\n");
    printf("Creatore del gioco: Erald Bektashi\n");

    printf("\nUltimi vincitori:\n");
    for (int i = 0; i < 3; i++) {
        printf("%d: %s\n", i+1, ultimi_vincitori[i]);
    }

    printf("\nStatistiche dell'ultima partita:\n");
    if (ultimi_turni > 0) {
        printf("Numero di turni giocati: %d\n", ultimi_turni);
        int giocatori_vivi = 0;
        for(int i=0; i<n_giocatori; i++) {
            if(giocatori[i] != NULL) giocatori_vivi++;
        }
        printf("Giocatori rimasti alla fine: %d\n\n", giocatori_vivi);
    } else {
        printf("Nessuna partita giocata ancora.\n\n");
    }
}

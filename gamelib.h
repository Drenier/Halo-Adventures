#ifndef GAMELIB_H
#define GAMELIB_H

// --- ENUMERAZIONI ---

typedef enum {
    avamposto_unsc,
    nave_covenant,
    rovine_precursore,
    palude_flood,
    base_orbitale,
    hangar_spaziale,
    relitto_spaziale,
    laboratorio_precursore,
    avamposto_esiliati,
    archivio_ancestrale
} Tipo_zona;

typedef enum {
    Nessun_covenant,    
    Jackal, // sostituito billi & solo ZetaHalo
    Grunt, // solo ZetaHalo
    Flood, // raro in ZetaHalo & comune in AltaOpera
    Elite, // sostituito demoncane & sia ZetaHalo sia AltaOpera
    Brute, // solo AltaOpera
    Arbiter // sostituito demotorzone & solo AltaOpera
} Tipo_nemico;

typedef enum {
    Nessun_oggetto,
    Stimolante_unsc,    // ripristina punti salute +10 HP
        Medikit,       // ripristina punti salute +15 HP
    Fucile_assalto_MA5BICWS,         // aumenta l'attacco massimo +4 ATK
        Spada_energetica,  // aumenta l'attacco massimo +7 ATK
    Scudo_energetico,        // aumenta la difesa massima +5 DEF
        Corazza_mjolnir,    // aumenta la difesa massima +7 DEF
    Teshio_heretic,                // aumenta la fortuna massima +5 FRT + 10 HP
        Manufatti_precursore  // aumenta la fortuna massima +7 FRT + 15 HP
} Tipo_oggetto;

// --- STRUTTURE DATI ---  
    struct Zona_ZetaHalo;  //Sostituto del MondoReale
    struct Zona_AltaOpera; //Sostituto del SopraSotto

// Adesso definiamo le strutture dati principali del gioco in pratica i 2 mondi

struct Zona_ZetaHalo {
    Tipo_zona tipo;
    Tipo_nemico nemico;
    Tipo_oggetto oggetto;
    struct Zona_ZetaHalo* avanti;
    struct Zona_ZetaHalo* indietro;
    struct Zona_AltaOpera* link_AltaOpera;
};

struct Zona_AltaOpera {
    Tipo_zona tipo;
    Tipo_nemico nemico;
    struct Zona_AltaOpera* avanti;
    struct Zona_AltaOpera* indietro;
    struct Zona_ZetaHalo* link_ZetaHalo;
};

struct Giocatore {
    char nome [50];
    int mondo; // 0 = Zeta Halo, 1 = Alta Opera

    // Puntatori alla posizione corrente
    struct Zona_ZetaHalo* pos_ZetaHalo;
    struct Zona_AltaOpera* pos_AltaOpera;

    // Statistiche del giocatore
    int salute;    // punti salute
    unsigned int attacco;   // punti attacco
    unsigned int difesa;    // punti difesa
    unsigned int fortuna;   // punti fortuna

    // Zaino (max 3 oggetti)
    Tipo_oggetto zaino[3];
    int oggetti_posseduti; // numero di oggetti posseduti
};

// --- RICHIAMO DELLE FUNZIONI ---

void imposta_gioco();
void gioca();
void termina_gioco();
void visualizza_crediti();

#endif
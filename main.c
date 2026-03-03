#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
void pulizia_schermo() {
    #ifdef _WIN32
        system("cls"); // Comando per Windows non si sa mai
    #else
        system("clear"); // Comando per Unix/Linux/MacOS
    #endif
    }

int main(){
    int scelta;
    time_t t;   // Serve per tenere conto delle varie esecuzioni del programma
    srand((unsigned)time(&t)); // Generatore di numeri casuali
 
    //
    pulizia_schermo();

    do{
        printf("Progetto COSE STRANE\n");
        printf("Benvenuto in Halo Adventure!\n");
        printf("1) Imposta gioco\n");
        printf("2) Gioca\n");
        printf("3) Termina il gioco\n");
        printf("4) Visualizza crediti\n");
        printf("Inserisci la tua scelta: ");
        
        if(scanf("%d", &scelta)!=1){
            while(getchar()!='\n'); // Pulisce il buffer di input
            scelta = 0; // Imposta una scelta non valida
        }

        switch(scelta){
            case 1:
                imposta_gioco();
                break;
            case 2:
                gioca();
                break;
            case 3:
                termina_gioco();
                printf("Uscita dal gioco. See ya!\n\n");
                break;
            case 4:
                visualizza_crediti();
                break;
            default:
                printf("\nL'UNSC non ricosce questo imput. Riprova.\n");
                printf("Premi INVIO per riprovare...\n");
                getchar(); // Attende che l'utente prema INVIO
                pulizia_schermo();
                break;
        }
        
    } while (scelta != 3);
    return 0;
}
#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <ncurses.h>
#include <pthread.h>



//#define printStringDebugLog(isDebugging,string,something) _Generic((isDebugging,string,something), int*: printStringIntDebugLog, char*: printStringCharDebugLog)(isDebugging,string,something);

#define D_IS_AUTONOMUS true // permette al gioco di giocarsi da solo
#define SEGNAPOSTO_ALIENO "<(^_^)>" // apparenza alieno primo livello
#define SEGNAPOSTO_ALIENO_CATTIVO ">.<" // apparenza alieno secondo livello
#define SEGNAPOSTO_NAVE "7:^)" // apparenza nave spazziale
#define SEGNAPOSTO_PROIETTILE "*" // apparenza proiettile
#define SEGNAPOSTO_DROPBOMB "o" // apparenza bombe alieni
#define LUNGHEZZA_SEGNAPOSTO 7
#define D_NUMERO_ALIENI 8
#define D_NUMERO_ALIENI_CATTIVI 5
#define D_NUMERO_GIOCATORI 1
#define D_NUMERO_MAX_PROIETTILI 16
#define D_VITE_NAVE 6
#define D_VITE_ALIENI 2
#define D_VITE_ALIENI_CATTIVI 2

#define D_PASSI_IN_VERTICALE 3 // assi in verticale degli alieni
#define D_VELOCITA_PROIETTILI 130 // moltiplicatore sugli spostamenti //
#define D_VELOCITA_PERSONAGGI 150 // moltiplicatore sui tempi //
#define SU 65 // Freccia su //
#define GIU 66 // Freccia giu //
#define DESTRA 67 // Freccia destra //
#define SINISTRA 68 // Freccia sinistra //
#define QUIT 'q' // flag per uscire dal gioco
#define BLANK_SPACE ' ' // flag per fare fuoco
#define EMPTY '0'
#define EMPTY_STRING "                   "
#define LOST 'l' // flag per terminare il gioco



#define MIN_RANDOM_STEP SU //definisce l'inizio dei comandi di spostamento
#define MAX_RANDOM_STEP SINISTRA //definisce la fine dei comandi di spostamento
#define MAX_STRING_SIZE 20 


#define DEBUGGING false //abilita la funzione di debugging/logging su file
#define DEBUGGING2 false  //abilita la funzione di debugging/logging su file

/*
    struttura dati utilizzata per definire un entità o personaggio
*/
struct proprietaOggetto{ 
    char segnaposto[MAX_STRING_SIZE];
    int lunghezzaSegnaposto;
    int istanza;
    int x;
    int y;
    int oldX;
    int oldY;
    pid_t pid;
    char flag;    
    int vite;            
} ;


FILE *fptr; // puntatore al file di logging
int debugIndex; // indice di debug/logging
static int maxX; //dimensione asse x schermo
static int maxY; //dimensione asse y schermo
clock_t clockStart; // orologio per valutazione fps
int numeroNemici; // contatore numero nemici rimanenti

/*
    settaggi vari come variabili globali per essere modificati durante il gioco
*/
int NUMERO_ALIENI;
int NUMERO_ALIENI_CATTIVI;
int NUMERO_GIOCATORI;
int NUMERO_MAX_PROIETTILI;
int VITE_NAVE;
int VITE_ALIENI;
int VITE_ALIENI_CATTIVI;
int PASSI_IN_VERTICALE ;
int VELOCITA_PROIETTILI;
int VELOCITA_PERSONAGGI;
int IS_AUTONOMUS;

int aliveProcesses; // contatore processi atttivi




//main
void controllo (int fileDescriptor[]); //controlla stampe, interazioni e legge da pipe
void controlloNave(int *viteTotali); // stampa il numero di vite di una nave con la grafica
void controlloAlieno(int fileDescriptor[], struct proprietaOggetto *alieno, struct proprietaOggetto alienoCattivo[]); // gestisce il comportamento di un alieno di primo livello in caso di morte


// controllo processi
void pipeCeck(int *p); //controlla che la pipe sia stata eseguita correttamente
pid_t myForkSwitch(struct proprietaOggetto *personaggio, int *fileDescriptor, void (*figlio) (int, struct proprietaOggetto *p));
void forkSwitch(pid_t *pid, int *fileDescriptor, void (*padre) (int), void (*figlio) (int)); // esegue una fork
void freeTheBuffer(int pipein,struct proprietaOggetto valore_letto); // ripulisce tutto il buffer
void killThemAll(struct proprietaOggetto personaggio[], int numeroPersonaggi); // elimina un intero array di personaggi
void killIt(struct proprietaOggetto *personaggio); // elimina un personaggio
void creaGruppoPersonaggi(struct proprietaOggetto personaggio[], int *fileDescriptor, void (*figlio) (int, struct proprietaOggetto *p), int numeroPersonaggi); // crea un insieme di personaggi
int checkContacts(struct proprietaOggetto *personaggioA, struct proprietaOggetto arrayPersonaggiB[], int numeroPersonaggiB); // verifica i contatti tra un array e un entità e aggiorna gli interessati in caso di contatto


// disegno
void setActualFieldSize(); // imposta le dimensioni iniziali dello schermo
int getXfieldSize(); // restituisce la dimensione delle x dello schermo
int getYfieldSize(); // restituisce la dimensione delle y dello schermo
void buildFieldBorders(int x, int y); // disegna il campo da gioco partendo da 0,0
void buildFieldBordersFromTo(int startingX, int startingY, int endingX, int endingY); // disegna il campo da gioco partendo
void gameOver(int x, int y); //animazione del game over
void youWon(int x, int y); // animazione vittoria
void resetField(int startingX, int stratingY, int endingX, int endingY); // iposta l'area come vuota
void printPropietaOggetto(struct proprietaOggetto *oggetto); // stampa un entità sullo schermo
void deletePropietaOggetto(struct proprietaOggetto *oggetto); // cancella un entità dallo schermo
void printFPS(int startingX, int startingY, int *FPScounter); // stampa gli fps dello schermo
void printEnemiesLeft(int startingX, int startingY, int numeroNemici); // stampa il numero di nemici rimasti
int customMenu(char nomeMenu[], char voceMenu[][25], int *interazioni[], int numeroVoci);// crea e stampa il menu interattivo
void printNAliveProcesses(int startingX, int startingY, int *nProcesses); // stampa il numero di processi in vita
void printLifesLeft(int startingX, int startingY, int lifesLeft); //stampa il numero di vite rimaste


// entita gioco
void setPersonaggio (struct proprietaOggetto *proprieta_personaggio, char characterPlaceHolder[], int startingX, int startingY, pid_t pidToAssign, int viteIniziali, int istanza); // imposta i valori iniziali del personaggio
void copyPersonaggio (struct proprietaOggetto *copiante,struct proprietaOggetto *copiato); // copia un entità all interno dell altra
void inizializzaPersonaggi(struct proprietaOggetto *daCopiare, struct proprietaOggetto Personaggio[], int numeroPersonaggi); // copia un entità all interno di un array di entità
void personaggioF (int pipeout, struct proprietaOggetto *pos_personaggio, int isAutonomus, char (*spostamento)(struct proprietaOggetto*,bool)); // gestisce il personaggio
void createRandomLocation(struct proprietaOggetto *el); //assegna ad una struttura proprietaOggetto una posizione casuale
bool isSameLocation(struct proprietaOggetto *elA, struct proprietaOggetto *elB); // valuta se 2 strutture proprietaOggetto hanno la stessa posizione
bool isSameLocationArray(struct proprietaOggetto *elA, struct proprietaOggetto elB[], int arrayLenght); // valuta se una struttura ed un array hanno la stessa posizione
int indexOfWhoIsSameLocationArray(struct proprietaOggetto *elA, struct proprietaOggetto elB[], int arrayLenght); // valuta se una struttura ed un array hanno la stessa posizione e restituisce la posizione dell array del primo match
bool isOutOfBound(struct proprietaOggetto *elA); // verificha se un entità si trova all esterno dell area di gioco
char passo(struct proprietaOggetto *personaggio, char c); // aggiorna l' entità cambiandone la sua posizione
void updateProprietaOggetto(struct proprietaOggetto *daSovrascrivere, struct proprietaOggetto *daCopiare ); // aggiorna l' entità
void proiettileF (int pipeout, struct proprietaOggetto *proprieta_proiettile, char (*spostamento)(struct proprietaOggetto*)); // controlla un entità tipo proiettile o bomba
void waitTOJumpIn(struct proprietaOggetto *proprieta_personaggio); // attende un tot di tempo prima di far entrare in gioco un alieno 
void alienoF(int pipeout, struct proprietaOggetto *personaggio); // crea un alieno
void naveSpazialeF(int pipeout, struct proprietaOggetto *personaggio); // crea una nave spaziale
void proiettileSX(int pipeout, struct proprietaOggetto *personaggio); // crea un proiettile SX
void proiettileDX(int pipeout, struct proprietaOggetto *personaggio); // crea un proiettile DX
void dropBombF(int pipeout,struct proprietaOggetto *personaggio); // crea una drop bomb
void spara(struct proprietaOggetto proiettile[], struct proprietaOggetto *valore_letto, int fileDescriptor[],int istanzaProiettile); // crea 2 proiettili (SX e DX) 
    //comportamenti
    char spostamentoAPassi (struct proprietaOggetto *personaggio, bool isRandom); // gestisce lo spostamento di un personaggio 
    char spostamentoLineare (struct proprietaOggetto *personaggio, bool isRandom); // gestisce lo spostamento di un alieno
    char spostamentoAPassiLaterali (struct proprietaOggetto *personaggio, bool isRandom); // gestisce lo spostamento di un personaggio su una sola riga
    char spostamentoProiettileSX (struct proprietaOggetto *proiettile); 
    char spostamentoProiettileDX (struct proprietaOggetto *proiettile);
    char spostamentoDropBomb (struct proprietaOggetto *proiettile);

// custom e debug
void printStringIntDebugLog(bool isDebugging, char* string, int *something); // stampa una stringa ed un intero su log
void printStringCharDebugLog(bool isDebugging, char* string,char *something); // stampa una stringa ed un carattere su log
void printStringStringDebugLog(bool isDebugging, char* string,char something[]); // stampa 2 stringhe su log
int customRandom(int min, int max); // restituisce un valore random compresi gli estremi
void createDebugLog(bool isDebugging); // crea il file di log
void printDebugLog(bool isDebugging); // stampa l' indice di debug sul log
void printProprietaOggettoDebugLog(bool isDebugging,struct proprietaOggetto *personaggio); // stampa un entità sul log

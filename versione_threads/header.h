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
#define LUNGHEZZA_CODA 40 // lunghezza coda per comunicazione con controllo
#define SEGNAPOSTO_ALIENO "<(^_^)>" // apparenza alieno primo livello
#define SEGNAPOSTO_ALIENO_CATTIVO ">.<" // apparenza alieno secondo livello
#define SEGNAPOSTO_NAVE "7:^)" // apparenza nave spazziale
#define SEGNAPOSTO_PROIETTILE "*" // apparenza proiettile
#define SEGNAPOSTO_DROPBOMB "o" // apparenza bombe alieni
#define LUNGHEZZA_SEGNAPOSTO 7
#define D_NUMERO_ALIENI 8
#define D_NUMERO_ALIENI_CATTIVI 5
#define D_NUMERO_GIOCATORI 1
#define D_NUMERO_MAX_PROIETTILI 20
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

#define DEBUGGING_NEEDED false //abilita la funzione di debugging/logging su file
#define DEBUGGING_MUTEX false //abilita la funzione di debugging/logging su file
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
//    pid_t pid;
    pthread_mutex_t mutex;
    pthread_t tid;         
    char flag;
    bool isAlive;    
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
pthread_mutex_t lock; //mutex di comunicazione
pthread_mutex_t printMutex; //mutex di stampa

struct proprietaOggetto codaProprieta[LUNGHEZZA_CODA]; // coda circolare di comunicazione con controllo
int posizioneCoda; // indice su dove si trova l'elemento corrente
int numeroElementiAttualiCoda; // autoesplicativo
    
/*
   array di entità come variabili globali per essere modificati e letti da tutti i threads
*/
struct proprietaOggetto* alieno; 
struct proprietaOggetto* alienoCattivo;
struct proprietaOggetto* naveSpaziale;
struct proprietaOggetto* dropBomb;
struct proprietaOggetto* proiettile;

int istanzaProiettile; //posizione dell array di proiettili da cui partire con la creazione degli stessi
int istanzaDropBomb; //posizione dell array di bombe da cui partire con la creazione degli stessi

//other


//main
void controllo ();
void *alienoF(void* voidComm);
void *naveSpazialeF(void* voidComm);
void spara(struct proprietaOggetto proiettile[], struct proprietaOggetto *valore_letto);
void *proiettileSX(void* voidComm);
void *proiettileDX(void* voidComm);
void *dropBombF(void* voidComm);
int checkContacts(struct proprietaOggetto *personaggioA, struct proprietaOggetto arrayPersonaggiB[], int numeroPersonaggiB);
void controlloNave(int *viteTotali);
void controlloAlieno(struct proprietaOggetto *alieno, struct proprietaOggetto alienoCattivo[]);
int push(struct proprietaOggetto coda[],struct proprietaOggetto *oggetto);
struct proprietaOggetto pop(struct proprietaOggetto coda[]);


// controllo processi
void pipeCeck(int *p); //controlla che la pipe sia stata eseguita correttamente
//void mutexCeck(pthread_mutex_t *m);
void mutexLock(pthread_mutex_t *m, char nomeMutex[]);
void mutexUnlock(pthread_mutex_t *m, char nomeMutex[]);
void scrivi (struct proprietaOggetto *personaggio);
void leggi ( struct proprietaOggetto *valore_letto);
pid_t myForkSwitch(struct proprietaOggetto *personaggio, int *fileDescriptor, void* (*figlio) (void*));
pthread_t myThreadCreate(struct proprietaOggetto *personaggio, void* (*figlio) (void*));
void freeTheBuffer(int pipein,struct proprietaOggetto valore_letto); // ripulisce tutto il buffer
void killThemAll(struct proprietaOggetto personaggio[], int numeroPersonaggi);
void killIt(struct proprietaOggetto *personaggio);
void creaGruppoPersonaggi(struct proprietaOggetto personaggio[], void* (*figlio) (void*), int numeroPersonaggi);

// disegno
void setActualFieldSize();
int getXfieldSize();
int getYfieldSize();
void buildFieldBorders(int x, int y); // disegna il campo da gioco partendo da 0,0
void buildFieldBordersFromTo(int startingX, int startingY, int endingX, int endingY); // disegna il campo da gioco partendo
void gameOver(int x, int y); //animazione del game over
void youWon(int x, int y); // animazione vittoria
void resetField(int startingX, int stratingY, int endingX, int endingY); // iposta l'area come vuota
//void printPropietaOggetto(struct proprietaOggetto *oggetto);
void printPropietaOggetto(struct proprietaOggetto *oggetto, int vite, void (*apparenze)(int));
void deletePropietaOggetto(struct proprietaOggetto *oggetto);
void printFPS(int startingX, int startingY, int *FPScounter);
void printEnemiesLeft(int startingX, int startingY, int numeroNemici);
int customMenu(char nomeMenu[], char voceMenu[][25], int *interazioni[], int numeroVoci);
void printNAliveProcesses(int startingX, int startingY, int *nProcesses);
void apparenzaAlieno(int vite);
void apparenzaAlienoCattivo(int vite);
void apparenzaNaveSpaziale(int vite);
void apparenzaProiettile(int vite);
void apparenzaDropBomb(int vite);

// entita gioco
void setPersonaggio (struct proprietaOggetto *proprieta_personaggio, char characterPlaceHolder[], int startingX, int startingY, pid_t pidToAssign, int viteIniziali, int istanza); // imposta i valori iniziali del personaggio
void copyPersonaggio (struct proprietaOggetto *copiante,struct proprietaOggetto *copiato);
void inizializzaPersonaggi(struct proprietaOggetto *daCopiare, struct proprietaOggetto Personaggio[], int numeroPersonaggi);
//void personaggioF (struct proprietaOggetto *pos_personaggio, int isAutonomus, char (*spostamento)(struct proprietaOggetto*,bool)); // gestisce il personaggio
void createRandomLocation(struct proprietaOggetto *el); //assegna ad una struttura proprietaOggetto una posizione casuale
bool isSameLocation(struct proprietaOggetto *elA, struct proprietaOggetto *elB); // valuta se 2 strutture proprietaOggetto hanno la stessa posizione
bool isSameLocationArray(struct proprietaOggetto *elA, struct proprietaOggetto elB[], int arrayLenght);
int indexOfWhoIsSameLocationArray(struct proprietaOggetto *elA, struct proprietaOggetto elB[], int arrayLenght);
bool isOutOfBound(struct proprietaOggetto *elA);
char passo(struct proprietaOggetto *personaggio, char c);
void printLifesLeft(int startingX, int startingY, int lifesLeft);
void updateProprietaOggetto(struct proprietaOggetto *daSovrascrivere, struct proprietaOggetto *daCopiare );
void proiettileF (struct proprietaOggetto *proprieta_proiettile, char (*spostamento)(struct proprietaOggetto*));
void waitTOJumpIn(struct proprietaOggetto *proprieta_personaggio);
    //comportamenti
    char spostamentoAPassi (struct proprietaOggetto *personaggio, bool isRandom); // gestisce lo spostamento di un personaggio 
    char spostamentoLineare (struct proprietaOggetto *personaggio, bool isRandom);
    char spostamentoAPassiLaterali (struct proprietaOggetto *personaggio, bool isRandom);
    char spostamentoProiettileSX (struct proprietaOggetto *proiettile);
    char spostamentoProiettileDX (struct proprietaOggetto *proiettile);
    char spostamentoDropBomb (struct proprietaOggetto *proiettile);

// custom e debug
//void printStringDebugLog(bool isDebugging, char* string, void *something);
void printStringIntDebugLog(bool isDebugging, char* string, int *something);
void printStringCharDebugLog(bool isDebugging, char* string,char *something);
void printStringStringDebugLog(bool isDebugging, char* string,char something[]);
int customRandom(int min, int max);
void createDebugLog(bool isDebugging);
void printDebugLog(bool isDebugging);
void printProprietaOggettoDebugLog(bool isDebugging,struct proprietaOggetto *personaggio);
void printMutexDebugLog(bool isDebugging, char nomeMutex[], char statoMutex[]);
#include "header.h"

/*
verifica l'effettivo funzionamento della pipe
*/
void pipeCeck(int *p){
    if(pipe(p)==-1) {
        perror("pipe call");
        _exit(1);
    }
}

/*
versione modificata del forkSwitch
    anzi che dividere il processo in due da cui partono due funzioni diverse,
    crea un processo figlio che si stacca dal primo lasciando l'esecuzione del padre immutata
*/
pid_t myForkSwitch(struct proprietaOggetto *personaggio, int *fileDescriptor, void (*figlio) (int, struct proprietaOggetto *p)){
    personaggio->pid = fork();
    switch(personaggio->pid){
        case 1:
            printStringIntDebugLog(DEBUGGING,"my fork switch error index: %d; ", &debugIndex);
            perror("fork call");
            _exit(2);
        case 0: // processo figlio //
            close(fileDescriptor[0]); // chiusura del descrittore di lettura //
            //printStringDebugLog(DEBUGGING," myForkSwitch passa istanza = %d \n", &(personaggio->istanza));
            figlio(fileDescriptor[1],personaggio);
            printStringIntDebugLog(DEBUGGING,"oddio che è appena successo!!!!!!!!!!!!%d\n", &debugIndex);
            _exit(3);
        break;
        // processo padre //
    }
    return personaggio->pid;
}



/*
    Fork switch di esempio
*/
void forkSwitch(pid_t *pid, int *fileDescriptor, void (*padre) (int), void (*figlio) (int)){
    *pid = fork();
    //printStringDebugLog(DEBUGGING, "file descriptor %d for guardia; ",fileDescriptor);
    switch(*pid){
        case 1:
            perror("fork call");
            _exit(2);
        case 0: // processo figlio //
            close(fileDescriptor[0]); // chiusura del descrittore di lettura //
            figlio(fileDescriptor[1]);
        break;
        default: // processo padre //
            close(fileDescriptor[1]); // chiusura del descrittore di scrittura //
            padre(fileDescriptor[0]);
    }
}

/*
    crea una serie di processi figli 
    con personaggio e comportamento passati come parametro
*/
void creaGruppoPersonaggi(struct proprietaOggetto personaggio[], int *fileDescriptor, void (*figlio) (int, struct proprietaOggetto *p), int numeroPersonaggi){
    int i;
    for (i = 0; i < numeroPersonaggi; i++)
    {
        myForkSwitch(&(personaggio[i]),fileDescriptor,figlio);               
    }    
}


/*
    libera il buffer in caso di errori di gestione
*/
void freeTheBuffer(int pipein,struct proprietaOggetto valore_letto){    
    printStringIntDebugLog(DEBUGGING2,"dentro free the buffer! %d\n ", &debugIndex);

    while(sizeof(valore_letto)<=fcntl(pipein, F_SETFL, O_NONBLOCK)) // pipe non bloccante
    {
        printStringIntDebugLog(DEBUGGING,"free the buffer! %d\n", &debugIndex);
    }
}

/*
    chiude una serie di processi
*/
void killThemAll(struct proprietaOggetto personaggio[], int numeroPersonaggi){
    int i;
    printStringCharDebugLog(DEBUGGING," kill Them All! %c \n", &personaggio[0].segnaposto[0]);
    for (i=0; i<numeroPersonaggi; i++)
    {
        killIt(&personaggio[i]);       
    }
}


/*
    chiude un processo 
    assicurandosi che fosse aperto in precedenza.
    cancella 2 volte perchè la delete ha 2 comportamenti
*/
void killIt(struct proprietaOggetto *personaggio){
    if (personaggio->pid!=0){
        deletePropietaOggetto(personaggio); 
        kill(personaggio->pid,1);
        personaggio->pid = 0;  
        deletePropietaOggetto(personaggio); 
        aliveProcesses--;     
    }
}


/*
    controlla se proiettili e navi siano entrati in contatto e ne aggiorna le proprietà
*/
int checkContacts(struct proprietaOggetto *personaggioA, struct proprietaOggetto arrayPersonaggiB[], int numeroPersonaggiB){
    int index=0;
    index = indexOfWhoIsSameLocationArray(personaggioA,arrayPersonaggiB, numeroPersonaggiB);    
    if(index >=0){  //verifica la presenza di un contatto (index == -1 quando non c'è contatto)
        personaggioA->vite--;
        if (personaggioA->vite<=0){
            killIt(personaggioA);
        }
        arrayPersonaggiB[index].vite--;
        if (arrayPersonaggiB[index].vite<=0){
            killIt(&arrayPersonaggiB[index]);
        }
        return index;
    }    
    return index;
}
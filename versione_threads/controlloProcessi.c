#include "header.h"
long mutexIndex;

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
    wrapper della pthread_mutex_lock con scopo di debug/logging
*/
void mutexLock(pthread_mutex_t *m, char nomeMutex[]){
    
    /*if(pthread_mutex_trylock(m)==0){
        printMutexDebugLog(DEBUGGING_MUTEX, nomeMutex, "locked");
        return;
    }else{
        printMutexDebugLog(DEBUGGING_MUTEX, nomeMutex, "waiting");        
        pthread_mutex_lock(m);
        printMutexDebugLog(DEBUGGING_MUTEX, nomeMutex, "locked");
        //printMutexDebugLog(DEBUGGING_MUTEX, nomeMutex, "skipped");    
    }*/
    pthread_mutex_lock(m);
}

/*
    wrapper della pthread_mutex_unlock con scopo di debug/logging
*/
void mutexUnlock(pthread_mutex_t *m, char nomeMutex[]){
    //printMutexDebugLog(DEBUGGING_MUTEX, nomeMutex, "UNLOCKED");
    pthread_mutex_unlock(m);
}

/*
    wrapper della push con scopo di debug/logging
*/
void scrivi (struct proprietaOggetto *personaggio){
    //printStringCharDebugLog(DEBUGGING,"%c: ",&personaggio->segnaposto[0]);
    //printStringIntDebugLog(DEBUGGING, "%d, sta provando a scrivere\n",&personaggio->istanza);
    push(codaProprieta,personaggio);   
    //printStringIntDebugLog(DEBUGGING, "%d, ha scritto\n",&personaggio->istanza);
}

/*
    wrapper della pop con scopo di debug/logging
*/
void leggi ( struct proprietaOggetto *valore_letto){    
    printStringIntDebugLog(DEBUGGING2, "sta provando a leggere %d\n",&debugIndex);
    *valore_letto = pop(codaProprieta);    
    printStringCharDebugLog(DEBUGGING2,"%c: ",&valore_letto->segnaposto[0]);
    printStringIntDebugLog(DEBUGGING2, "%d, è stato letto\n",&valore_letto->istanza);
}



/*
    logicamente simile alla myForkSwitch
    crea unnuovo thread e aggiorna il numero di processi attivi
*/
pthread_t myThreadCreate(struct proprietaOggetto* personaggio, void* (*figlio) (void*)){
    // printStringCharDebugLog(DEBUGGING,"%c: ",&personaggio->segnaposto[0]);
    // printStringIntDebugLog(DEBUGGING, "%d, sto creando il tread\n",&personaggio->istanza);
    // printProprietaOggettoDebugLog(DEBUGGING,personaggio);
    // fflush(NULL);

    pthread_create(&personaggio->tid,NULL,figlio,personaggio);
    aliveProcesses++;
    // printNAliveProcesses(getXfieldSize()-15,0,&aliveProcesses);
    // printStringIntDebugLog(DEBUGGING, "%d, tread creato\n",&personaggio->istanza);
    // fflush(NULL);
    return personaggio->tid;
}



/*
    crea una serie di processi figli 
    con personaggio e comportamento passati come parametro
*/
void creaGruppoPersonaggi(struct proprietaOggetto personaggio[],  void* (*figlio) (void*), int numeroPersonaggi){
    int i;
    for (i = 0; i < numeroPersonaggi; i++)
    {
        myThreadCreate(&(personaggio[i]),figlio);
               
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
    e ne attende la chiusura
*/
void killThemAll(struct proprietaOggetto personaggio[], int numeroPersonaggi){
    int i;
    printStringCharDebugLog(DEBUGGING," kill Them All! %c \n", &personaggio[0].segnaposto[0]);
    for (i=0; i<numeroPersonaggi; i++)
    {
        killIt(&personaggio[i]);
        pthread_join(personaggio[i].tid,NULL);      
    }
}


/*
    invia il segnale di chiusura ad un thread 
    assicurandosi che fosse aperto in precedenza.
    decrementa il numero di processi attivi
*/
void killIt(struct proprietaOggetto *personaggio){
    if (personaggio->tid!=0){
        personaggio->isAlive=false;
        mutexLock(&printMutex,"cancello perche morto");
        deletePropietaOggetto(personaggio);        
        //kill(personaggio->pid,1);
        printStringIntDebugLog(DEBUGGING2,"killit in esecuzione %d\n",&debugIndex);
        //fflush(NULL);
        
        printStringIntDebugLog(DEBUGGING_NEEDED,"killit eseguita %d\n",&debugIndex);
        //pthread_join(personaggio->tid,NULL);
        
        personaggio->tid = 0;  
        deletePropietaOggetto(personaggio);
        mutexUnlock(&printMutex,"cancello perche morto");
        scrivi(personaggio); 
        aliveProcesses--;
    } 
    printNAliveProcesses(getXfieldSize()-15,0,&aliveProcesses); 
}


/*
    controlla se proiettili e navi siano entrati in contatto e ne aggiorna le proprietà
*/
int checkContacts(struct proprietaOggetto *personaggioA, struct proprietaOggetto arrayPersonaggiB[], int numeroPersonaggiB){
    int index=0;
    index = indexOfWhoIsSameLocationArray(personaggioA,arrayPersonaggiB, numeroPersonaggiB);    
    if(index >=0){          
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
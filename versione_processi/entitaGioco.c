#include "header.h"


/*
    imposta un entità
*/
void setPersonaggio (struct proprietaOggetto *proprieta_personaggio, char characterPlaceHolder[], int startingX, int startingY, pid_t pidToAssign, int viteIniziali, int istanza){
    printStringIntDebugLog(DEBUGGING2,"setPersonaggio() %d\n; ", &debugIndex);

    strncpy(proprieta_personaggio->segnaposto,characterPlaceHolder,MAX_STRING_SIZE);
    proprieta_personaggio->lunghezzaSegnaposto=strlen(proprieta_personaggio->segnaposto);

    proprieta_personaggio->x=startingX;
    proprieta_personaggio->y=startingY;
    proprieta_personaggio->oldX=-1;
    proprieta_personaggio->oldY=-1;
    proprieta_personaggio->istanza=istanza;
    proprieta_personaggio->pid = pidToAssign;          
    proprieta_personaggio->flag = EMPTY;    
    proprieta_personaggio->vite = viteIniziali; 
}

/*
    copia un entità dentro un altra
*/
void copyPersonaggio (struct proprietaOggetto *copiante,struct proprietaOggetto *copiato){
    strncpy(copiante->segnaposto,copiato->segnaposto,MAX_STRING_SIZE);
    copiante->lunghezzaSegnaposto = copiato->lunghezzaSegnaposto;
    copiante->istanza=copiato->istanza;
    copiante->x=copiato->x;
    copiante->y=copiato->y;
    copiante->oldX=copiato->oldX;
    copiante->oldY=copiato->oldY;
    copiante->pid=copiato->pid;
    copiante->flag=copiato->flag;
    copiante->vite=copiato->vite;
}

/*
    copia un entità dentro un altra all' interno di un array tenendo conto dell' istanza
*/
void inizializzaPersonaggi(struct proprietaOggetto *daCopiare, struct proprietaOggetto Personaggio[], int numeroPersonaggi){
    int i;
    for ( i = 0; i < numeroPersonaggi; i++)
    {
        copyPersonaggio(&Personaggio[i],daCopiare);
        Personaggio[i].istanza=i;
        printProprietaOggettoDebugLog(DEBUGGING2,&Personaggio[i]);
    }
}


/*
    controlla l' andamento generale di un personaggio 
*/
void personaggioF (int pipeout, struct proprietaOggetto *proprieta_personaggio, int isAutonomus, char (*spostamento)(struct proprietaOggetto*,bool)){    
    int bombDrop = customRandom(10,30);
    int counter =0;

    waitTOJumpIn(proprieta_personaggio);
    
    write(pipeout,proprieta_personaggio,sizeof(*proprieta_personaggio));
    while(1) {
        counter++;
        if(counter==100){
            counter=0;
            printStringCharDebugLog(DEBUGGING,"personaggio %c ",&proprieta_personaggio->segnaposto[0]);
            printStringIntDebugLog(DEBUGGING,"con istanza %d: e ancora vivo\n",&proprieta_personaggio->istanza);
        }        
        if(isAutonomus){
            napms(VELOCITA_PERSONAGGI);
            spostamento(proprieta_personaggio, false);                      
            if (bombDrop==0)
            {
                proprieta_personaggio->flag=BLANK_SPACE;
                bombDrop = customRandom(30,90);
            }
            bombDrop--;
        }
        else{
            spostamento(proprieta_personaggio, IS_AUTONOMUS);
        }
        printStringCharDebugLog(DEBUGGING2, "personaggio flag:%c; \n", &(proprieta_personaggio->flag));
        printStringIntDebugLog(DEBUGGING2, "personaggio istanza:%d; \n", &(proprieta_personaggio->istanza));
        if(proprieta_personaggio->y>=getYfieldSize()-2||proprieta_personaggio->y<=0){
           printStringIntDebugLog(DEBUGGING," !!!!!!!!\nERRORE il personaggio con istanza  = %d  si e eliminato da solo\n!!!!!!!!!\n", &proprieta_personaggio->istanza);
           printPropietaOggetto(proprieta_personaggio);
           fflush(NULL);
           proprieta_personaggio->flag=LOST;
        }

        write(pipeout,proprieta_personaggio,sizeof(*proprieta_personaggio));
    }
}


/*
    fa attendere un personaggio finchè non si troverà nella corretta posizione per partire
*/
void waitTOJumpIn(struct proprietaOggetto *proprieta_personaggio){
    int whaitingCount =0;
    int i, j;
    if (strcmp(proprieta_personaggio->segnaposto,SEGNAPOSTO_ALIENO)==0){
        whaitingCount = (strlen(SEGNAPOSTO_ALIENO_CATTIVO)+1)*(NUMERO_ALIENI_CATTIVI+1);
        whaitingCount += strlen(SEGNAPOSTO_ALIENO)+3;
        
        for ( i = 0; i < proprieta_personaggio->istanza; i++)
        {
            for ( j = 0; j <whaitingCount+2; j++)
            {
                napms(VELOCITA_PERSONAGGI);
            }        
        }
    }
}

/*
    permette ad un personaggio di spostarsi sulle assi x e y
*/
char spostamentoAPassi (struct proprietaOggetto *personaggio, bool isRandom){
    char c;
    if (isRandom){
        c = customRandom(MIN_RANDOM_STEP, MAX_RANDOM_STEP);
    }
    else{
        c=getch();
    }
    return passo(personaggio,c);
}

/*
    permette ad un personaggio di spostarsi sull asse x
*/
char spostamentoAPassiLaterali (struct proprietaOggetto *personaggio, bool isRandom){
    char c = 'n';
    if (isRandom){
       napms(VELOCITA_PERSONAGGI);
        c = customRandom(DESTRA, SINISTRA); 
        if((c==DESTRA && personaggio->x < getXfieldSize()-personaggio->lunghezzaSegnaposto-1) || (c==SINISTRA && personaggio->x > 1) ){
            passo(personaggio,c);
        }       
        
        if (customRandom(0,2)==0){
            personaggio->flag=BLANK_SPACE;
        }
    }
    else{
        c=getch();

        if(personaggio->istanza==0){
            if(c==DESTRA && personaggio->x < getXfieldSize()-personaggio->lunghezzaSegnaposto-1 ){
                passo(personaggio,c);
            }else if (c==SINISTRA && personaggio->x > 1)        {
                passo(personaggio,c);
            }else if(c == BLANK_SPACE || c==QUIT){
                passo(personaggio,c);
            }else{
                passo(personaggio, EMPTY);
            }
        }
        else{
            if(c=='d' && personaggio->x < getXfieldSize()-personaggio->lunghezzaSegnaposto-1 ){
                passo(personaggio,DESTRA);
            }else if (c=='a' && personaggio->x > 1)        {
                passo(personaggio,SINISTRA);
            }else if(c == 'w' ){
                passo(personaggio,BLANK_SPACE);
            }else if( c==QUIT){
                passo(personaggio,QUIT);
            }else{
                passo(personaggio, EMPTY);
            }

        }
        
    }
    return c;
}

/*
    comportamento specifico degli alieni:
    parte da sinistra verso destra e come incontra un bordo del campo,
    scende e cambia direszione
*/
char spostamentoLineare (struct proprietaOggetto *personaggio, bool isRandom){
    char c='x';
    int i=0;
    if (isRandom){
        c = customRandom(MIN_RANDOM_STEP, MAX_RANDOM_STEP);        
    }
    else{
        if (personaggio->y >= getYfieldSize()-3)
        {
            return passo(personaggio,LOST);
        }
        
        if(personaggio->oldX!=-1 && personaggio->oldY != -1 && personaggio->x != personaggio->oldX && (personaggio->x <=1 ||personaggio->x + strlen(SEGNAPOSTO_ALIENO) >= getXfieldSize()-1)){
            c=GIU;           
        }
        else{
            if(personaggio->x <=1 || personaggio->x > personaggio->oldX){
                c=DESTRA;                
            }
            else{
                c=SINISTRA;
            } 
        }
    }
    c=passo(personaggio,c);
    if(c==GIU&&PASSI_IN_VERTICALE>2){
        personaggio->y=personaggio->y+PASSI_IN_VERTICALE-1;        
    }
    return c;
    
}

/*
    imposta il personaggio per fargli eseguire un passo
*/
char passo(struct proprietaOggetto *personaggio, char c){
    personaggio->flag = EMPTY;      
    switch(c) {
        case SU:
            if(personaggio->y>1)
            personaggio->oldY=personaggio->y;
            personaggio->oldX=personaggio->x;
            personaggio->y-=1;
            break;
        case GIU:
            if(personaggio->y<getYfieldSize()-3)
            personaggio->oldY=personaggio->y;
            personaggio->oldX=personaggio->x;
            personaggio->y+=1;
            break;
        case SINISTRA:
            if(personaggio->x>1)
            personaggio->oldY=personaggio->y;
            personaggio->oldX=personaggio->x;
            personaggio->x-=1;
            break;
        case DESTRA:
            if(personaggio->x<getXfieldSize()-personaggio->lunghezzaSegnaposto)
            personaggio->oldY=personaggio->y;
            personaggio->oldX=personaggio->x;
            personaggio->x+=1;
            break;
        case QUIT:
            personaggio->flag = QUIT;
            break;
        case BLANK_SPACE:
            personaggio->flag = BLANK_SPACE;
            break;
        case LOST:
            personaggio->flag = LOST;
            break;
    }
    return c;
}

/*
    imposta un entità in una posizione random
*/
void createRandomLocation(struct proprietaOggetto *el){
    el->x=customRandom(2,getXfieldSize()-2);
    el->y=customRandom(2,getYfieldSize()-2);
}

/*
    verifica la presenza di due entità nello stesso punto
*/
bool isSameLocation(struct proprietaOggetto *elA, struct proprietaOggetto *elB){
    if (elA->pid != 0 && elB->pid != 0 &&(( elA->x <= elB->x && elA->x+elA->lunghezzaSegnaposto-1 >= elB->x)||( elB->x <= elA->x && elB->x+elB->lunghezzaSegnaposto-1 >= elA->x)) && elA->y == elB->y)
    {
        return true;
    }    
    return false;
}

/*
    verifica la presenza di un entità ed un array di entità nello stesso punto
*/
bool isSameLocationArray(struct proprietaOggetto *elA, struct proprietaOggetto elB[], int arrayLenght){
    int i;
    for ( i = 0; i < arrayLenght; i++)
    {
        if(isSameLocation(elA,&elB[i])){return true;}
    }
    
    return false;
}

/*
    verifica la presenza di un entità ed un array di entità nello stesso punto 
    e restituisce la posizione nell array di chi si trova nella stessa posizione,
    -1 altrimenti;
*/
int indexOfWhoIsSameLocationArray(struct proprietaOggetto *elA, struct proprietaOggetto elB[], int arrayLenght){
    int i;
    if (elA->x !=1 || elA->y!=1){
        for ( i = 0; i < arrayLenght; i++)
        {
            if (elB[i].x !=1 || elB[i].y!=1){
                if(isSameLocation(elA,&elB[i])){return i;}
            }
        }
    }    
    return -1;
}

/*
    verifica se un entità si trova fuori dall area di gioco
*/
bool isOutOfBound(struct proprietaOggetto *elA){
    if (elA->pid != 0 && elA->x >=2 && elA->y >=2 && elA->x < getXfieldSize()-2 && elA->y <=getYfieldSize()-4)
    {
        return false;
    }    
    return true;
}

/*
    aggiorna un entità con una nuova
*/
void updateProprietaOggetto(struct proprietaOggetto *daSovrascrivere, struct proprietaOggetto *daCopiare ){
    daSovrascrivere->oldX = daCopiare->oldX;
    daSovrascrivere->oldY = daCopiare->oldY;

    daSovrascrivere->x = daCopiare->x;
    daSovrascrivere->y = daCopiare->y;
    
    if (daCopiare->pid != 0){daSovrascrivere->pid = daCopiare->pid;}
              
    daSovrascrivere->flag = EMPTY; 

    if (daSovrascrivere->vite < 0 ){daSovrascrivere->vite = daCopiare->vite;}
}

/*
    gestisce il comportamento generale di un proiettile o bomba
*/
void proiettileF (int pipeout, struct proprietaOggetto *proprieta_proiettile, char (*spostamento)(struct proprietaOggetto*)){    
    int counter =0;
    printStringIntDebugLog(DEBUGGING2," pipeout = %d \n", &pipeout);
    write(pipeout,proprieta_proiettile,sizeof(*proprieta_proiettile));
    while(1) {
        counter++;
        if(counter==100){
            counter=0;
            printStringCharDebugLog(DEBUGGING,"personaggio %c ",&proprieta_proiettile->segnaposto[0]);
            printStringIntDebugLog(DEBUGGING,"con istanza %d: e ancora vivo\n",&proprieta_proiettile->istanza);
        }

        spostamento(proprieta_proiettile);
        napms(VELOCITA_PROIETTILI);
        
       printStringIntDebugLog(DEBUGGING2, "proiettile istanza:%d; \n", &(proprieta_proiettile->istanza));
       if(proprieta_proiettile->y>=getYfieldSize()||proprieta_proiettile->y<0){
            printStringCharDebugLog(DEBUGGING," !!!!!!!!\nERRORE il proiettile %c", &proprieta_proiettile->segnaposto[0]);
            printStringIntDebugLog(DEBUGGING," con istanza  = %d  si e eliminato da solo\n!!!!!!!!!\n", &proprieta_proiettile->istanza);
           fflush(NULL);
           exit(1);
        }
        write(pipeout,proprieta_proiettile,sizeof(*proprieta_proiettile));
    }
}


/*
    spostamento diagonale sinistro verso l'alto
*/
char spostamentoProiettileSX (struct proprietaOggetto *proiettile){
    char c = passo(proiettile,SINISTRA);
    proiettile->y=proiettile->y-1;
    return c;
}

/*
    spostamento diagonale destro verso l'alto
*/
char spostamentoProiettileDX (struct proprietaOggetto *proiettile){
    char c = passo(proiettile,DESTRA);
    proiettile->y=proiettile->y-1;
    return c;
}

/*
    spostamento verso il basso
*/
char spostamentoDropBomb (struct proprietaOggetto *proiettile){
    char c = GIU;
    return passo(proiettile,c);
}

/*
    crea un proiettile sinistro
*/
void proiettileSX(int pipeout, struct proprietaOggetto *personaggio){
    proiettileF(pipeout,personaggio,spostamentoProiettileSX);
}

/*
    crea un proiettile destro
*/
void proiettileDX(int pipeout, struct proprietaOggetto *personaggio){
    proiettileF(pipeout,personaggio,spostamentoProiettileDX);
}

/*
    crea una bomba
*/
void dropBombF(int pipeout,struct proprietaOggetto *personaggio){
    proiettileF(pipeout,personaggio,spostamentoDropBomb);
}

/*
    gestisce il caso in cui un anavicella spara:
    controlla che i proiettili siano inattivi altrimenti li elimina e li ricrea
*/
void spara(struct proprietaOggetto proiettile[], struct proprietaOggetto *valore_letto, int fileDescriptor[],int istanzaProiettile){
    aliveProcesses+=2;
    killIt(&proiettile[istanzaProiettile]);
    killIt(&proiettile[(istanzaProiettile+1)%NUMERO_MAX_PROIETTILI]);

    setPersonaggio(&proiettile[istanzaProiettile],SEGNAPOSTO_PROIETTILE,valore_letto->x+valore_letto->lunghezzaSegnaposto/2-1,valore_letto->y-1,0,proiettile[istanzaProiettile].vite,istanzaProiettile);                    
    setPersonaggio(&proiettile[(istanzaProiettile+1)%NUMERO_MAX_PROIETTILI],SEGNAPOSTO_PROIETTILE,valore_letto->x+valore_letto->lunghezzaSegnaposto/2-1,valore_letto->y-1,0,proiettile[(istanzaProiettile+1)%NUMERO_MAX_PROIETTILI].vite,(istanzaProiettile+1)%NUMERO_MAX_PROIETTILI);                    

    myForkSwitch(&(proiettile[istanzaProiettile]),fileDescriptor,proiettileSX);
    myForkSwitch(&(proiettile[(istanzaProiettile+1)%NUMERO_MAX_PROIETTILI]),fileDescriptor,proiettileDX);
}

/*
    crea un alieno
*/
void alienoF(int pipeout, struct proprietaOggetto *personaggio){
    printStringIntDebugLog(DEBUGGING2,"creato singolo alieno() %d; \n", &debugIndex);
    printStringIntDebugLog(DEBUGGING2," alieno creato con istanza = %d \n", &personaggio->istanza);
    
    /*
        siccome gli stack sono separati ma uguali, 
        per evitare che gli alieni rilasciassero le bombe sempre nello stesso punto,
        ho preferito inizializzare il seed del random dentro ogni processo; 
    */
    srand((int)(personaggio->istanza*(int)time(0)^(1/5)));
    personaggioF(pipeout,personaggio,true,spostamentoLineare);
}


/*
    crea una nave spaziale
*/
void naveSpazialeF(int pipeout, struct proprietaOggetto *personaggio){
    printStringIntDebugLog(DEBUGGING,"entrato dentro naveSpaziale() %d; \n", &debugIndex);
    printStringIntDebugLog(DEBUGGING2,"setPersonaggio() eseguito %d; \n", &debugIndex);
    personaggioF(pipeout,personaggio,false,spostamentoAPassiLaterali);
}
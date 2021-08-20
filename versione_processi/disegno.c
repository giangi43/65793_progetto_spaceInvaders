#include "header.h"

/*
    legge e setta le dimensioni del terminale.
    queste verranno usate per gestire qualunque disegno o posizionamento 
*/
void setActualFieldSize(){
    maxY = 0;
    maxX = 0;
    getmaxyx(stdscr, maxY, maxX);
}

/*
    restituisce la lunghezza dell'asse X del terminale.
    usata come funzione per evitare qualunque errore di sovrascrittura
*/
int getXfieldSize(){
    return maxX;
}

/*
    restituisce la lunghezza dell'asse Y del terminale.
    usata come funzione per evitare qualunque errore di sovrascrittura
*/
int getYfieldSize(){
    return maxY;
}


/*
    costruisce i bordi del campo partendo dalla coordinata (0,0)
*/
void buildFieldBorders(int endingX, int endingY){
    buildFieldBordersFromTo(0,0,endingX,endingY);
}

/*
    costruisce un rettangolo di '/'
    partendo e terminando a cordinate a scelta 
*/
void buildFieldBordersFromTo(int startingX, int startingY, int endingX, int endingY){
    int i,j;
    for ( i = startingX; i < endingX; i++){
        for ( j = startingY; j < endingY; j++)
        {
            if (i==0 || j==0 || i==endingX-1 || j==endingY-1){
                mvaddch(j,i,'/');
                //usleep(1);
            }
        }        
    } 
}

/*
    pulisce completamente lo schermo
*/
void resetField(int startingX, int stratingY, int endingX, int endingY){
    int i,j;
    for ( i = startingX; i < endingX; i++){
        for ( j = stratingY; j < endingY; j++)
        {
            mvaddch(j,i,' ');
        }        
    } 
}

/*
    stampa il numero di vite rimaste
    in cordinate a scelta
*/
void printLifesLeft(int startingX, int startingY, int lifesLeft){
    mvprintw(startingY, startingX, "vite:%d/", lifesLeft);
}

/*
    cstampa il numero di nemici rimasti
    in cordinate a scelta
*/
void printEnemiesLeft(int startingX, int startingY, int numeroNemici){
    mvprintw(startingY, startingX, "nemici:%d//", numeroNemici); 
}

/*
    stampa quanto spesso si aggiorna lo schermo in un secondo
*/
void printFPS(int startingX, int startingY, int *FPScounter){
    
    clock_t difference = clock() - clockStart;
    int msec = difference*100000/ CLOCKS_PER_SEC;
    
    int trigger = 1;

    if (msec > trigger){
        mvprintw(startingY, startingX, "fps:%d//", *FPScounter);
        *FPScounter=0;
        clockStart = clock();
    } 
    *FPScounter = *FPScounter + 1;
    
}


/*
    stampa quanti processi o thread sono attivi in un dato momento
*/
void printNAliveProcesses(int startingX, int startingY, int *nProcesses){    
    mvprintw(startingY, startingX, "processes:%d//", *nProcesses);
}

/*
    stampa un personaggio nella sua posizione
*/
void printPropietaOggetto(struct proprietaOggetto *oggetto){    
    //printProprietaOggettoDebugLog( oggetto->segnaposto[0] == '*',oggetto);    
    
    if (oggetto->pid!=0)
    {
        if (oggetto->oldX != -1 &&oggetto->oldY!=-1){
            deletePropietaOggetto(oggetto);
        }
        mvaddnstr(oggetto->y, oggetto->x, oggetto->segnaposto,oggetto->lunghezzaSegnaposto);
    }   
    
    attrset(A_NORMAL);
}

/*
    stampa un array di personaggi nella loro posizione
*/
void printArrayPropietaOggetto(struct proprietaOggetto oggetto[], int numeroOggetti){    
    
    for (int i = 0; i < numeroOggetti; i++)
    {
        if(oggetto[i].pid!=0){
            mvprintw(oggetto[i].y, oggetto[i].x, oggetto[i].segnaposto);
        }
    }    
    attrset(A_NORMAL);
}


/*
    cancella un personaggio dall' ultima sua posizione:
    -posizione attuale se morto;
    -posizione precedente se vivo;
*/
void deletePropietaOggetto(struct proprietaOggetto *oggetto){
    if(oggetto->pid==0){
        mvaddnstr(oggetto->y, oggetto->x, EMPTY_STRING, oggetto->lunghezzaSegnaposto);
        //printProprietaOggettoDebugLog( DEBUGGING,oggetto);
    }
    else{
        if (oggetto->oldX<oggetto->x){
            mvaddch(oggetto->oldY, oggetto->oldX,' ');
        }else if(oggetto->oldX>oggetto->x){
            mvaddch(oggetto->oldY, oggetto->oldX+oggetto->lunghezzaSegnaposto-1,' ');
        }else{
            mvaddnstr(oggetto->oldY, oggetto->oldX, EMPTY_STRING,oggetto->lunghezzaSegnaposto);
        }
    }
}

/*
    animazione di game over
*/
void gameOver(int x, int y){
    char string[]="Game Over";
    int h=0;
    int i,j;
    for ( i = 0; i < x; i++){ //colonne
        for ( j = 0; j < y; j++) //righe
        {
            if ((i>= (int) (x-9)/2 && h<9) && j==(int)((y/2.0)+0.5)){ //Game Over
                attron(A_BOLD);
                attron(COLOR_PAIR(1));
                mvaddch(j,i,string[h]);
                h++;
                refresh();
                usleep(100000);
                attrset(A_NORMAL);
            }
            else{
                 mvaddch(j,i,'-');                 
            }
        }   
        refresh();
        usleep(100000);     
    } 
    usleep(5000000);
}


/*
    animazione di vittoria
*/
void youWon(int x, int y){
    char string[]="You Won!!";
    int h=0;
    int i,j;
    for ( i = 0; i < x; i++){ //colonne
        for ( j = 0; j < y; j++) //righe
        {
            if ((i>= (int) (x-9)/2 && h<9) && j==(int)((y/2.0)+0.5)){ //Game Over
                attron(A_BOLD);
                attron(COLOR_PAIR(3));
                mvaddch(j,i,string[h]);
                h++;
                refresh();
                usleep(100000);
                attrset(A_NORMAL);

            }
            else{
                 mvaddch(j,i,'.');                 
            }
        }   
        refresh();
        usleep(100000);     
    } 
    usleep(5000000);
}


/*
    menu interattivo
    permette di selezionare una voce ed eventualmente modificarne le proprietà
    per modificare (se possibile):
        premere la freccia destra;
        apparira una freccetta "->" ed il colore diventerà blu;
        digitare il valore voluto e premere invio; 
*/
int customMenu(char nomeMenu[], char voceMenu[][25], int* interazioni[], int numeroVoci){
    int selected=0;
    bool interacting=false;
    int input;
    char c = EMPTY;    
    attrset(COLOR_PAIR(7));
    resetField(0,0,getXfieldSize(),getYfieldSize());
    //attron(A_BOLD);
    do{
        interacting=false;
        if (c==SU){
            selected = (selected -1);
            if (selected<0){
                selected = numeroVoci-1;
            }
        }else if(c==GIU){
            selected = (selected +1)%numeroVoci;
        }
        else if(c==DESTRA){
            if (interazioni!=NULL){
                interacting = true;
            }
        }        
        
        attrset(A_BOLD);
        attron(COLOR_PAIR(7));
        if (interacting){
            resetField(0,0,getXfieldSize(),getYfieldSize());
        }
        mvprintw(getYfieldSize()-3,getXfieldSize()/2-8,"made by Giangi ");
        mvprintw(3,getXfieldSize()/2-strlen(nomeMenu)/2,nomeMenu);
        attrset(A_NORMAL);
        attron(COLOR_PAIR(6));
        
        for (int i = 0; i < numeroVoci; i++)
        {
            if (selected==i){
                if (interacting){
                    attrset(COLOR_PAIR(8));
                }else{
                    attrset(COLOR_PAIR(7));
                }
                attron(A_BOLD);
            }

            mvprintw((getYfieldSize()/2)-(numeroVoci/2)+i,3,voceMenu[i]);
            if (interazioni!=NULL){
                mvprintw((getYfieldSize()/2)-(numeroVoci/2)+i,3+strlen(voceMenu[i]),": %d      ",*interazioni[i]);
            }            

            if (selected==i){
                if (interacting){
                    mvprintw((getYfieldSize()/2)-(numeroVoci/2)+i,3+strlen(voceMenu[i])+5,"->");
                    refresh();
                    if (scanf("%d",&input)!=NULL){
                       *interazioni[i] = input;
                    }
                }
                attrset(A_NORMAL);
                attron(COLOR_PAIR(6));
            }
        }
        refresh();
        if(!interacting){
            c = getch();
        }else{
            c=EMPTY;
        }
    }while(c!='\n'&&c!='q');
    return selected;
}




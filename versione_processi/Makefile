spaceInvaders: main.o customAndDebug.o entitaGioco.o disegno.o controlloProcessi.o
	gcc main.o customAndDebug.o entitaGioco.o disegno.o controlloProcessi.o -o spaceInvaders.out -lncurses

customAndDebug.o: customAndDebug.c header.h
	gcc -std=c99 -c customAndDebug.c

main.o: main.c header.h
	gcc -std=c99 -c main.c

entitaGioco.o: entitaGioco.c header.h
	gcc -std=c99 -c entitaGioco.c

disegno.o: disegno.c header.h
	gcc -std=c99 -c disegno.c

controlloProcessi.o: controlloProcessi.c header.h
	gcc -std=c99 -c controlloProcessi.c

clean:
	rm *.out
	rm *.o
PROGRAM=test
CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -g
LDFLAGS=-lm


all:$(PROGRAM)

$(PROGRAM): main.o sintetizador.o tramo.o nota.o midi.o muestreo.o wave.o _main.o
	$(CC) $(CFLAGS) -o $(PROGRAM) main.o sintetizador.o tramo.o nota.o midi.o muestreo.o wave.o _main.o $(LDFLAGS) 

main.o: main.c sintetizador.h nota.h tramo.h wave.h _main.h 
	$(CC) $(CFLAGS) -c main.c

_main.o: _main.c _main.h sintetizador.h nota.h tramo.h wave.h _main.h 
	$(CC) $(CFLAGS) -c _main.c

sintetizador.o: sintetizador.c sintetizador.h nota.h tramo.h
	$(CC) $(CFLAGS) -c sintetizador.c

tramo.o: tramo.c tramo.h muestreo.h
	$(CC) $(CFLAGS) -c tramo.c

nota.o: nota.c nota.h midi.h
	$(CC) $(CFLAGS) -c nota.c

muestreo.o: muestreo.c muestreo.h
	$(CC) $(CFLAGS) -c muestreo.c

midi.o: midi.c midi.h
	$(CC) $(CFLAGS) -c midi.c

wave.o: wave.c wave.h
	$(CC) $(CFLAGS) -c wave.c


clean:
	rm -vf  *.o $(PROGRAM)

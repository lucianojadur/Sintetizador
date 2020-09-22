PROGRAM=sintetizar
CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -g
LDFLAGS=-lm


all:$(PROGRAM)

$(PROGRAM): main.o sintetizador.o tramo.o contenedor_notas.o nota.o midi.o muestreo.o wave.o 
	$(CC) $(CFLAGS) -o $(PROGRAM) main.o sintetizador.o tramo.o contenedor_notas.o nota.o midi.o muestreo.o wave.o $(LDFLAGS) 

main.o: main.c sintetizador.h contenedor_notas.h nota.h tramo.h wave.h main.h 
	$(CC) $(CFLAGS) -c main.c

sintetizador.o: sintetizador.c sintetizador.h contenedor_notas.h nota.h tramo.h
	$(CC) $(CFLAGS) -c sintetizador.c

midi.o: midi.c midi.h contenedor_notas.h nota.h
	$(CC) $(CFLAGS) -c midi.c

contenedor_notas.o: contenedor_notas.c contenedor_notas.h nota.h
	$(CC) $(CFLAGS) -c contenedor_notas.c

nota.o: nota.c nota.h
	$(CC) $(CFLAGS) -c nota.c

wave.o: wave.c wave.h tramo.h
	$(CC) $(CFLAGS) -c wave.c

tramo.o: tramo.c tramo.h muestreo.h
	$(CC) $(CFLAGS) -c tramo.c

muestreo.o: muestreo.c muestreo.h
	$(CC) $(CFLAGS) -c muestreo.c


clean:
	rm -vf  *.o $(PROGRAM)

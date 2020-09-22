#ifndef MAIN_H
#define MAIN_H

#include "sintetizador.h"
#include "contenedor_notas.h"
#include "nota.h"
#include "midi.h"
#include "tramo.h"
#include "wave.h"

#define MAX_CHARS_FILE	80

typedef struct{
	char sint[MAX_CHARS_FILE];
	char midi[MAX_CHARS_FILE];
	char wave[MAX_CHARS_FILE];
	int canal;
	int f_muestreo;
	int pulsos;
}argumentos_t;


argumentos_t *crear_args();

bool verificar_argumentos(int argc, const char *argv[], argumentos_t *args);

void uso(const char *arg);

void liberar(synth_t *s, contenedor_t *c, tramo_t *t, FILE *fm, argumentos_t *args);

#endif

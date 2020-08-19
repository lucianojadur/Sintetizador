#ifndef MAIN
#define MAIN

#include "sintetizador.h"
#include "nota.h"
#include "midi.h"
#include "tramo.h"
#include "wave.h"

#define MAX_CHARS_FILE 40
#define METAEVENTO_FIN_DE_PISTA 0x2F
#define EVENTO_MAX_LONG 10

enum {EVNOTA_NOTA, EVNOTA_VELOCIDAD};
enum {METAEVENTO_TIPO, METAEVENTO_LONGITUD};

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

void liberar_todo(synth_t *s, nota_t *n, tramo_t *t, FILE *fm, argumentos_t *args);

void borrar_argumentos(argumentos_t *args);

#endif
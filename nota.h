#ifndef NOTA_H
#define NOTA_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "tramo.h"

#define MAX_PARAMS		2
#define REL_FREC_INDEX	0
#define REL_AMP_INDEX	1
#define MAX_FREC_DIGITS	1
#define MAX_AMP_DIGITS	8

#define LINE_DELIMITER " "

/*typedef enum {
	[0] = A,
	[1] = A#,
	[2] = B,
	[3] = C,
	[4] = C#,
	[5] = D,
	[6] = D#,
	[7] = E,
	[8] = F,
	[9] = F#,
	[10]= G,
	[11]= G#,
}nombre_t;*/

typedef struct {
	tramo_t * onda;
	float armonicos[8][2];
	int cantidad_armonicos;
}nota_t;



nota_t* crear_nota();

bool obtener_cantidad_armonicos(FILE *f, nota_t *nota);
	/*Lee y guarda la primera línea de un archivo sintetizador, que indica cuántos armónicos modulan al timbre*/

bool obtener_armonicos(FILE*  f, nota_t* nota);
	/*Obtiene de un archivo de sintetizador la cantidad de armónicos de un determinado timbre de una nota dada 
	y las frecuencias e intensidades relativas a las fundamentales de dicha nota.*/

void imprimir_cantidad_armonicos(nota_t * nota);

void imprimir_armonicos(nota_t * nota);



#endif
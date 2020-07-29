#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tramo.h"

#define MAX_PARAMS		2
#define REL_FREC_INDEX	0
#define REL_AMP_INDEX	1
#define MAX_FREC_DIGITS	1
#define MAX_AMP_DIGITS	8
#define HARM_STR_LENGTH	11
#define PARAMS_LENGTH	25

#define LINE_DELIMITER	" "
#define NUL_STR			""


typedef struct {
	tramo_t * onda;
	float armonicos[8][2];
	int cantidad_armonicos;
	char * parametros[3];	//arreglo de cadenas que indican [0]Ataque, [1]Sustain y [2] Dec con sus respectivos parámetros 
}nota_t;



nota_t* crear_nota();

bool obtener_cantidad_armonicos(FILE *f, nota_t *nota);
	/*Lee y guarda la primera línea de un archivo sintetizador, que indica cuántos armónicos modulan al timbre*/

bool obtener_armonicos(FILE*  f, nota_t* nota);
	/*Obtiene de un archivo de sintetizador la cantidad de armónicos de un determinado timbre de una nota dada 
	y las frecuencias e intensidades relativas a las fundamentales de dicha nota.*/

bool obtener_parametros_modular(FILE *f, nota_t *nota);

void imprimir_cantidad_armonicos(nota_t * nota);

void imprimir_armonicos(nota_t * nota);

void imprimir_parametros(nota_t *nota);

void destruir_parametros(nota_t *nota);


#endif

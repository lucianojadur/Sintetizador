/*
TDA SINTETIZADOR

	Este TDA se encarga de sintetizar frecuencias. Es decir, dada una función de onda correspondiente a una nota
	determinada, esto se encarga de modular la nota de forma tal que la amplitud quede modulada 
	por el ATAQUE, el SUSTAIN y la CAÍDA según la duración de cada intervalo.

	Tanto la información de los armónicos que dan forma al timbre de un determinado instrumento como
	los parámetros para modular cada intervalo (así como también la forma de la función que los modula), 
	vienen indicados en un archivo de texto plano con una estructura en particular. 
	
	La validez del archivo queda determinada al momento de crear el tda sintetizador, que guarda toda la información
	del archivo para poder sintetizar cada nota. En caso de que el archivo esté corrupto o no sea válido, el tda
	se creará vacío.
*/

#ifndef SYNTH_H
#define SYNTH_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include "contenedor_notas.h"
#include "nota.h"
#include "tramo.h"

#define MAX_HARM			30
#define MAX_KEYS_LENGTH		9
#define PARAM_LINES_NUMBER	3
#define HARM_STR_LENGTH		25
#define FREC_INDEX			0
#define AMP_INDEX			1
#define MAX_FREC_DIGITS		9
#define MAX_AMP_DIGITS		9
#define PARAMS_LENGTH		25
#define FUNCTIONS			12
#define MAX_MOD_FUNC		14
#define LINE_DELIMITER		" "


typedef struct sintetizador synth_t;


synth_t *sintetizador_crear(FILE *f);
/*Crea un tda sintetizador dinámico inicializado con todos los valores indicados por el archivo apuntado por f*/

void sintetizador_imprimir_data(synth_t *s);

void sintetizador_destruir(synth_t *t);
/*Libera toda la memoria pedida para crear el tda*/

tramo_t *sintetizar_nota(nota_t *n, synth_t *s, int f_m);
/*Crea un tramo con los primeros 4 parámetros muestreado a una f_m y lo sintetiza según los atributos del sintetizador s*/

bool sintetizador_sintetizar_pistas(tramo_t *tramo, synth_t *s, contenedor_t *c, int f_m);

#endif

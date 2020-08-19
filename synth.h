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
#include "nota.h"
#include "tramo.h"

#define MAX_HARM			10
#define MAX_KEYS_LENGTH		9
#define PARAM_LINES_NUMBER	3
#define HARM_STR_LENGTH		11
#define FREC_INDEX			0
#define AMP_INDEX			1
#define MAX_FREC_DIGITS		1
#define MAX_AMP_DIGITS		8
#define PARAMS_LENGTH		25
#define FUNCTIONS			12
#define MAX_MOD_FUNC		14
#define LINE_DELIMITER		" "

typedef float (*accion_t)(double, float [3]);

typedef enum {ATAQUE, SUSTAIN, CAIDA} intervalo_t;

typedef enum {
	CONSTANT,
	LINEAR,
	INVLINEAR,
	SIN,
	EXP,
	INVEXP,
	QUARTCOS,
	QUARTSIN,
	HALFCOS,
	HALFSIN,
	LOG,
	INVLOG,
	TRI,
	PULSES
}acciones_t;

typedef struct {
	char* clave;
	accion_t accion;
}funciones_t;

typedef struct{
	char clave[MAX_KEYS_LENGTH];
	float instantes[3];
}params_t;

typedef struct{
	size_t n;
	params_t *data[3];
	float armonicos[][2];
}synth_t;



synth_t *sintetizador_crear(FILE *f);
/*Crea un tda sintetizador dinámico inicializado con todos los valores indicados por el archivo apuntado por f*/

params_t *obtener_informacion(char * linea);
/*Almacena en una estructura el nombre de la función que modula y los parámetros temporales escritos en linea*/

void imprimir_data(synth_t *s);

void destruir_sintetizador(synth_t *t);
/*Libera toda la memoria pedida para crear el tda*/

tramo_t *sintetizar_nota(nota_t *nota, synth_t *s, int f_m);
/*Crea un tramo con los primeros 4 parámetros muestreado a una frecuencia de muestreo f_m 
y lo sintetiza según los atributos del sintetizador s*/


#endif

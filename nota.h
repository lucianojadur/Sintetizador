/*TDA nota
Contiene información sobre una nota musical dentro de una pista de audio.
Sus atributos guardan:
	el nombre de la nota en cifrado americano
	la octava que le corresponde
	su frecuencia fundamental
	su intensidad
	el momento en el cual inicia dentro de la pista
	la duración en tiempo de la nota hasta que inicia su decaimiento
*/

#ifndef NOTA_H
#define NOTA_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_PARAMS		2
#define MAX_NOTAS		10000
#define FREC_INDEX		0
#define AMP_INDEX		1
#define PARAMS_LENGTH	25
#define A4_FREC			440
#define A4_OCT			4

#define LINE_DELIMITER	" "
#define NUL_STR			""

typedef enum { C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B }notas_t;

typedef struct nota nota_t;


nota_t * nota_crear(notas_t simb, signed char oct, uint8_t i, double t0);

bool nota_aumentar_duracion(nota_t *n, float t);

notas_t nota_obtener_nombre(nota_t *n);

signed char nota_obtener_octava(nota_t *n);

double nota_obtener_inicio(nota_t *n);

double nota_obtener_final(nota_t *n);

double nota_obtener_duracion(nota_t *n);

double nota_obtener_amplitud(nota_t *n);

double nota_obtener_frecuencia(nota_t *n);

bool nota_finalizo(nota_t *nota);

void nota_terminar(nota_t *n);

void nota_imprimir(nota_t *nota);

void nota_borrar(nota_t *n);


#endif

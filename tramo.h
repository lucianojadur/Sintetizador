/*TDA tramo
Sirve para almacenar dinámicamente un tramo de audio muestreado discretamente a una determinada frecuencia de muestreo.
Las muestras corresponden a valores numéricos de tipo flotantes que conforman una o la suma de varias señales 
de tipo senoidal.
Entre sus atributos se incluyen:
	un vector dinámico donde se almacena la totalidad de las muestras del tramo
	la cantidad de elementos incluidos en el vector
	el instante inicial del tramo respecto del comienzo de la pista
	la frecuencia de muestreo a la que se obtienen las muestras 
*/

#ifndef TRAMO_H
#define TRAMO_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "muestreo.h"

typedef struct {
    float *v;		
    size_t n;		
    double t0;		
    int f_m;		
}tramo_t;			



tramo_t *_tramo_crear(double t_inicial, double t_final, int f_muestreo);

tramo_t *tramo_crear_muestreo(double t_inicial, double t_final, int f_muestreo, float f, float a, float fa[][2], size_t n_fa);
/*crea un tramo nuevo muestreando entre el intervalo <t_inicial> y el <t_final> a una frecuencia de muestreo <f_muestreo>
	la nota dada por la frecuencia fundamental <f>, amplitud <a> y los armónicos <fa> de longitud <n_fa>.*/

void tramo_destruir(tramo_t *t);

tramo_t *tramo_clonar(const tramo_t *t);

bool tramo_redimensionar(tramo_t *t, double tf);
/*Modifica la cantidad de muestras del tramo t. Si el nuevo tamaño es mayor al anterior, las muestras nuevas se completan con 0.*/

bool tramo_extender(tramo_t *destino, const tramo_t *extension);
/*Suma los datos de extension a los de destino.*/

#endif

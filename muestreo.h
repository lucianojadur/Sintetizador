
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#ifndef MPI
#define MPI 3.1415926535897932
#endif
#define MSJ_ERROR "El vector declarado no es válido"
#define FREC 440
#define AMP 1



void inicializar_muestras(float *v, size_t n);
/*Crea un arreglo inicializado a 0 con n cantidad de elementos*/

void muestrear_senoidal(float *v, size_t n, double t0, int f_m, float f, float a);
/*suma a cada elemento de v el valor correspondiente a la onda en dicho t según los parámetros recibidos*/

void muestrear_armonicos(float *v, size_t n, double t0, int f_m, float f, float a, float fa[][2], size_t n_fa);
/*recibo el arreglo de muestras v, sin ningún valor asignado a sus elementos y sobre cada elemento sumo el valor
	de cada armónico en un instante t = t0 + i / f_m*/

void imprimir_muestras(const float v[], size_t n, double t0, int f_m);
/*Imprime por renglón el valor de cada muestra con su instante correspondiente*/
	
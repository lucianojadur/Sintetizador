/*Este módulo contiene las funciones necesarias para escribir un archivo WAVE a partir de un arreglo de valores
numéricos que representan las muestras discretas de una pista de audio.*/

#ifndef WAVE_H
#define WAVE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>



FILE* crear_fichero(const char *nombre);
/*Crea un archivo de salida en modo escritura con el nombre recibido como parámetro.*/

bool escribir_encabezado_wave(FILE *f, size_t n, int f_m);
/*Escribe sobre un archivo el encabezado para un archivo de tipo WAVE con determinadas especificaciones.*/

bool volcar_muestras(FILE *f, float v[], size_t n);
/*Recbie un arreglo de flotantes y los escala a enteros de 16 bits signados para luego volcarlos como información
en el archivo apuntado por f.*/


#endif
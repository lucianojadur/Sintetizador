//TDA CONTENENDOR
/*
Guarda una lista con todas la notas presentes en un archivo de audio.
*/
#ifndef CONTENEDOR_H
#define CONTENEDOR_H
#include <stdbool.h>
#include <stdlib.h>
#include "nota.h"



typedef struct cont contenedor_t;


contenedor_t *contenedor_notas_crear(size_t max);

void contenedor_notas_limpiar(contenedor_t *cont);

size_t contenedor_hallar_posicion_nota(notas_t nombre, signed char oct, contenedor_t *cont);

bool contenedor_nota_ya_existe(notas_t nombre, signed char oct, contenedor_t *cont, size_t *pos);

void contenedor_agregar_nota(contenedor_t *cont, nota_t *nota, size_t posicion);

nota_t *contenedor_hallar_nota(contenedor_t *cont, size_t i);

size_t contenedor_obtener_max(contenedor_t *cont);


#endif
#include "contenedor_notas.h"
#include "nota.h"

struct cont{
	nota_t **notas;
	size_t cantidad;
};


contenedor_t *contenedor_notas_crear(size_t max){
	contenedor_t * c = malloc(sizeof(struct cont));
	if (c == NULL) return NULL;
	nota_t ** v = malloc(sizeof(nota_t*) * max);
	if (v == NULL){
		free(c);
		return NULL;
	}
	for (size_t i = 0; i < max; i++)
		v[i] = NULL;

	c->notas = v;
	c->cantidad = max;
	return c;
}

void contenedor_notas_limpiar(contenedor_t *cont){
	for (size_t i = 0; i < cont->cantidad; i++){
		if (cont->notas[i] != NULL)
			nota_borrar(cont->notas[i]);
	}
	free(cont->notas);
	free(cont);
}

size_t contenedor_hallar_posicion_nota(notas_t nombre, signed char oct, contenedor_t *cont){
	if (cont->cantidad == 0) return 0;
	size_t i = 0;
	for (i = 0; i < cont->cantidad; i++){
		if (cont->notas[i] != NULL && nota_obtener_nombre(cont->notas[i]) == nombre && nota_obtener_octava(cont->notas[i]) == oct)
			if(!nota_finalizo(cont->notas[i]))
				return i;
		if (cont->notas[i] ==  NULL) return i;
	}
	return -1;
}

bool contenedor_nota_ya_existe(notas_t nombre, signed char oct, contenedor_t *cont, size_t *pos){
	if (cont->cantidad == 0) return false;
	for (size_t i = 0; i < cont->cantidad; i++){
		if (cont->notas[i] != NULL && nota_obtener_nombre(cont->notas[i]) == nombre && nota_obtener_octava(cont->notas[i]) == oct){
			if(!nota_finalizo(cont->notas[i])){
				*pos = i;
				return true;
			}
		}
	}
	return false;
}

void contenedor_agregar_nota(contenedor_t *cont, nota_t *nota, size_t posicion){
	cont->notas[posicion] = nota;
}

nota_t *contenedor_hallar_nota(contenedor_t *cont, size_t i){
	return cont->notas[i];
}

size_t contenedor_obtener_max(contenedor_t *cont){
	return cont->cantidad;
}
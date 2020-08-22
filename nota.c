#include "nota.h"


static bool _obtener_frecuencia(nota_t *n);

nota_t *crear_nota(notas_t simb, signed char oct, uint8_t i, double t0){
	nota_t *n = malloc(sizeof(nota_t));
	if (n == NULL)
		return NULL;
	n->simbolo = simb;
	n->octava = oct;
	n->a = (float)i;
	n->inicio = t0;
	n->final = t0;
	if(!_obtener_frecuencia(n)){
		free(n);
		return NULL;
	}
	return n;
}

bool aumentar_duracion(nota_t *n, float t){
	if (n == NULL) return false;
	n->final += t;
	return true;
}

void imprimir_nota(nota_t *nota){
	printf("8va: %d, ", nota->octava);
	printf("f: %.2f, ", nota->f);
	printf("i: %.2f, ", nota->a);
	printf("t0: %.2f, ", nota->inicio);
	printf("tf: %.2f\n", nota->final);
}

bool nota_ya_existe(notas_t nombre, signed char oct, nota_t *notas[], size_t n, size_t *pos){
	if (n == 0) return false;
	for (size_t i = 0; i < n; i++){
		if(notas[i] == NULL)
		if (notas[i] != NULL && notas[i]->simbolo == nombre && notas[i]->octava == oct){
			*pos = i;
			return true;
		}
	}
	return false;
}

void nota_borrar(nota_t *n){
	free(n);
}

size_t hallar_posicion(notas_t nombre, signed char oct, nota_t *notas[], size_t n){
	if (n == 0) return 0;
	size_t i = 0;
	for (i = 0; i < n; i++){
		if (notas[i] != NULL && nombre == notas[i]->simbolo && oct == notas[i]->octava)
			return i;
	}
	return -1;
}

size_t posicion_nota_nueva(nota_t *notas[], size_t n){
	for(size_t i = 0; i < n; i++){
		if (notas[i] == NULL)
			return i;
	}
	return 0;
}

void actualizar_lista(nota_t *notas[]){
	for (size_t i = 0; i < MAX_NOTAS; i++){
		if (notas[i] == NULL && notas[i+1] != NULL){
			nota_t *aux = notas[i+1];
			notas[i] = aux;
			notas[i+1] = NULL;
		}
	}
}

static bool _obtener_frecuencia(nota_t *n){	
	if((n->octava == 0 && n->simbolo < A) || (n->octava == 8 && n->simbolo != C) || n->octava > 8)
		return false;
	n->f = A4_FREC * pow(2, (n->octava - A4_OCT) + ((float)n->simbolo - A) / 12);
	return true;
}


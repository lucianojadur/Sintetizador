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
	n->duracion = t0;
	if(!_obtener_frecuencia(n)){
		free(n);
		return NULL;
	}
	return n;
}


void imprimir_nota(nota_t *nota){

	printf("8va: %d, ", nota->octava);
	printf("f: %.2f, ", nota->f);
	printf("i: %.2f ,", nota->a);
	printf("t0: %.2f ,", nota->inicio);
	printf("tf: %.2f\n", nota->duracion);
}


static bool _obtener_frecuencia(nota_t *n){	
	if((n->octava == 0 && n->simbolo < A) || (n->octava == 8 && n->simbolo != C) || n->octava > 8)
		return false;
	n->f = A4_FREC * pow(2, (n->octava - A4_OCT) + ((float)n->simbolo - A) / 12);
	return true;
}

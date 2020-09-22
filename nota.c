#include "nota.h"


struct nota{
	notas_t simbolo;
	signed char octava;
	float f;
	float a;
	double inicio;
	double final;
	bool finalizada;
};


static bool _obtener_frecuencia(nota_t *n);

nota_t * nota_crear(notas_t simb, signed char oct, uint8_t i, double t0){
	nota_t *n = malloc(sizeof(nota_t));
	if (n == NULL)
		return NULL;
	n->simbolo = simb;
	n->octava = oct;
	n->a = (float)i;
	n->inicio = t0;
	n->final = t0;
	n->finalizada =  false;
	if(!_obtener_frecuencia(n)){
		free(n);
		return NULL;
	}
	return n;
}

bool nota_aumentar_duracion(nota_t *n, float t){
	if (n == NULL) return false;
	n->final += t;
	return true;
}

notas_t nota_obtener_nombre(nota_t *n){
	return n->simbolo;
}	

signed char nota_obtener_octava(nota_t *n){
	return n->octava;
}

double nota_obtener_inicio(nota_t *n){
	return n->inicio;
}

double nota_obtener_final(nota_t *n){
	return n->final;
}

double nota_obtener_duracion(nota_t *n){
	return n->final - n->inicio;
}

double nota_obtener_frecuencia(nota_t *n){
	return n->f;
}

double nota_obtener_amplitud(nota_t *n){
	return n->a;
}

void nota_terminar(nota_t *n){
	n->finalizada = true;
}

void nota_borrar(nota_t *n){
	free(n);
}

bool nota_finalizo(nota_t *nota){
	return nota->finalizada;
}

void nota_imprimir(nota_t *nota){
	printf("8va: %d, ", nota->octava);
	printf("f: %.2f, ", nota->f);
	printf("i: %.2f, ", nota->a);
	printf("t0: %.2f, ", nota->inicio);
	printf("tf: %.2f\n", nota->final);
}


static bool _obtener_frecuencia(nota_t *n){	
	if((n->octava == 0 && n->simbolo < A) || (n->octava == 8 && n->simbolo != C) || n->octava > 8)
		return false;
	n->f = A4_FREC * pow(2, (n->octava - A4_OCT) + ((float)n->simbolo - A) / 12);
	return true;
}

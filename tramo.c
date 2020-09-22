#include "tramo.h"
#include "muestreo.h"


struct tramo{
    float *v;		
    size_t n;		
    double t0;		
    int f_m;		
};			


tramo_t *_tramo_crear(double t_inicial, double t_final, int f_muestreo){
	tramo_t * tramo = malloc(sizeof(tramo_t));
	if(tramo == NULL) 
		return NULL;

	size_t cantidad_muestras = f_muestreo * (t_final - t_inicial);	

	float * muestras = malloc(sizeof(float) * cantidad_muestras);
	if(muestras == NULL){
		free(tramo);
		return NULL;
	}

	tramo->v = muestras;
	tramo->n = cantidad_muestras;	
	tramo->t0 = t_inicial;
	tramo->f_m = f_muestreo;
	
	return tramo;
}


tramo_t *tramo_crear_muestreo(double t_inicial, double t_final, int f_muestreo, float f, float a, float fa[][2], size_t n_fa){
	size_t cantidad_muestras = (t_final - t_inicial) * f_muestreo;								
	tramo_t * tramo_muestras = _tramo_crear(t_inicial, t_final, f_muestreo); 
	if(tramo_muestras == NULL)
		return NULL;
	muestrear_armonicos(tramo_muestras->v, cantidad_muestras, t_inicial, f_muestreo, f, a, fa, n_fa);
	return tramo_muestras;
}


void tramo_destruir(tramo_t *t){
	free(t->v);
	free(t);
}


tramo_t *tramo_clonar(const tramo_t *t){	
	double t_final = (double)t->n / t->f_m + t->t0;
	tramo_t * tramo_copia = _tramo_crear(t->t0, t_final, t->f_m);
	if(tramo_copia == NULL) return NULL;

	for(size_t i = 0; i < t->n; i++)
		tramo_copia->v[i] = t->v[i];

	return tramo_copia;
}


bool tramo_redimensionar(tramo_t *t, double tf){
	size_t n_aux = (tf - t->t0) * t->f_m;
	if(n_aux == t->n)
		return true;

	float *muestras_aux = realloc(t->v, (n_aux  + 1)* sizeof(float) );
	if(muestras_aux == NULL){
		tramo_destruir(t);
		return false;
	}
	t->v = muestras_aux;
	for(size_t i = t->n; i < n_aux; i++)
		t->v[i] = 0;
	t->n = n_aux;
	return true;
}


bool tramo_extender(tramo_t *destino, const tramo_t *extension){
	if(destino == NULL || destino->v == NULL){
		free(destino);
		destino = tramo_clonar(extension);
		return true;
	}
	const size_t inicio_extension = (extension->t0 - destino->t0) * destino->f_m;
	double t_f_destino = (double)destino->n / destino->f_m + destino->t0;
	double t_f_extension = (double)extension->n /extension->f_m +extension->t0;
	
	
	if(t_f_extension > t_f_destino)
		if(!tramo_redimensionar(destino, t_f_extension)) return false;

	for(size_t i = 0; i < extension->n; i++)
		destino->v[i+inicio_extension] += extension->v[i];

	return true;
}

void tramo_multiplicar_muestra(tramo_t *tramo, size_t i, float valor){
	tramo->v[i] *= valor;
}

size_t tramo_obtener_tamanio(tramo_t *tramo){
	return tramo->n;
}

int tramo_obtener_f_muestreo(tramo_t *tramo){
	return tramo->f_m;
}

float tramo_copiar_muestra(tramo_t *tramo, size_t i){
	return tramo->v[i];
}

float tramo_hallar_max(tramo_t *tramo){
	float max = tramo->v[0];
	for (size_t i = 1; i < tramo->n; i++){
		if (tramo->v[i] > max)
			max = tramo->v[i];
	}
	return max;
}

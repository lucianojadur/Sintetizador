#include "muestreo.h"


void inicializar_muestras(float *v, size_t n){
	for(int i = 0; i < n; i++)
		v[i] = 0;
}

void muestrear_senoidal(float *v, size_t n, double t0, int f_m, float f, float a){
	for (int i = 0; i < n; i++){
		double t = t0 + i / (double)f_m;
		v [i] += a * sin(2 * MPI * f * t);
	}
}

void muestrear_armonicos(float *v, size_t n, double t0, int f_m, float f, float a, float fa[][2], size_t n_fa){
	inicializar_muestras(v, n);				
	for(int fil = 0; fil < n_fa; fil++){
 		int frec_rel = fa[fil][0] * f;
 		double amp_rel = fa[fil][1] * a;
 		muestrear_senoidal(v, n, t0, f_m, frec_rel, amp_rel);
	}
}


void imprimir_muestras(const float v[], size_t n, double t0, int f_m){
	for(int i = 0; i < n; i++){
		double t = t0 + ((double)i)/f_m;
		printf("%f; %f\n", t, v[i]);
	}
}

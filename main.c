#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sintetizador.h"
#include "nota.h"
#include "midi.h"
#include "tramo.h"
#include "wave.h"
#include "_main.h"


int main(int argc, char const *argv[]){

//VALIDACION DE ARGUMENTOS
	argumentos_t *args = crear_args();
	if (!verificar_argumentos(argc, argv, args)){
		uso(argv[0]);
		free(args);
		return -1;
	}
	int cnl = args->canal, f_m = args->f_muestreo, pps = args->pulsos;
	size_t max_cantidad_notas = MAX_NOTAS;
	
	tramo_t *tramo = _tramo_crear(0,0,f_m);

	FILE *fmidi = fopen(args->midi, "rb");
	if(fmidi == NULL) {
	    fprintf(stderr, "No se pudo abrir \"%s\"\n", args->midi);
		free(args);
	    return 1;
	}

	nota_t **notas = malloc(sizeof(nota_t*) * max_cantidad_notas);
	if (notas == NULL){
		fprintf(stderr, "No se pudo abrir \"%s\"\n", args->midi);
		liberar(NULL, NULL, tramo, fmidi, args);
		return 1;
	}for (size_t i = 0; i < max_cantidad_notas; i++)
		notas[i] = NULL;	//Si no hago esto me tira conditional jump(...) en valgrind

	FILE *fs = fopen(args->sint, "rt");
	if (fs == NULL){
		fprintf(stderr, "No se pudo abrir el archivo de sintetizador\n");
		liberar(NULL, notas, tramo, fmidi, args);
		return -1;
	}
	synth_t *s = sintetizador_crear(fs);
	fclose(fs);
	if (s == NULL){
		fprintf(stderr, "No se pudo crear el tda sintetizador\n");
		liberar(s, notas, tramo, fmidi, args);
		return -1;
	}

//PROCESAMIENTO DE MIDI Y CARGA DE NOTAS
	if(!procesar_midi(fmidi, notas, max_cantidad_notas, cnl, pps)){
		fprintf(stderr, "No se pudo procesar el midi\n");
		liberar(s, notas, tramo, fmidi, args);
		return -1;
	}
	fclose(fmidi);

//SÍNTESIS DE NOTAS
	imprimir_data(s);

	if (!sintetizar_pistas(tramo, s, notas, max_cantidad_notas, f_m)){
		fprintf(stderr, "No se pudo sintetizar la pista\n");
		liberar(s, notas, tramo, fmidi, args);
		return -1;
	}
	printf("Se pudo sintetizar la pista\n");
	vaciar_contenedor_notas(notas, max_cantidad_notas);
	destruir_sintetizador(s);

//ESCRITURA DEL WAVE
	FILE *fw = crear_fichero(args->wave);

	if (escribir_encabezado_wave(fw, tramo)){
		if (!volcar_muestras(fw, tramo)){
			liberar(NULL, NULL, tramo, fw, args);
			return -1;
		}
	}
	printf("\n\n\n### Se pudo escribir el wave :D ###\n");	
	liberar(NULL, NULL, tramo, fw, args);

	return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sintetizador.h"
#include "contenedor_notas.h"
#include "nota.h"
#include "midi.h"
#include "tramo.h"
#include "wave.h"
#include "main.h"


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

	contenedor_t *notas = contenedor_notas_crear(max_cantidad_notas);
	if (notas == NULL){
		fprintf(stderr, "No se pudo abrir \"%s\"\n", args->midi);
		liberar(NULL, NULL, tramo, fmidi, args);
		return 1;
	}

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

//Inicio del proceso
//LECTURA DE MIDI Y CARGA DE NOTAS
	if(!procesar_midi(fmidi, notas, cnl, pps)){
		fprintf(stderr, "No se pudo procesar el midi\n");
		liberar(s, notas, tramo, fmidi, args);
		return -1;
	}
	fclose(fmidi);

//SÍNTESIS DE NOTAS
	sintetizador_imprimir_data(s);

	if (!sintetizador_sintetizar_pistas(tramo, s, notas, f_m)){
		fprintf(stderr, "No se pudo sintetizar la pista\n");
		liberar(s, notas, tramo, fmidi, args);
		return -1;
	}
	printf("Se pudo sintetizar la pista\n");
	contenedor_notas_limpiar(notas);
	sintetizador_destruir(s);

//ESCRITURA DEL WAVE
	FILE *fw = crear_fichero(args->wave);

	if (escribir_encabezado_wave(fw, tramo)){
		if (!volcar_muestras(fw, tramo)){
			liberar(NULL, NULL, tramo, fw, args);
			return -1;
		}
	}
	printf("\n\n### Se pudo escribir el wave :D ###\n");	
	liberar(NULL, NULL, tramo, fw, args);

	return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////

void uso(const char *arg){
	fprintf(stderr, "Error. Ingrese al menos 3 pares de argumentos.\n");
	fprintf(stderr, "Uso básico: %s -s <sintetizador.txt> -i <entrada.mid> -o <salida.wav>\n", arg);
	fprintf(stderr, "Argumentos opcionales:\n\t-f <frecuencia de muestreo>\n\t-c <canal>\n\t-r <pulsos/segundo>\n\n");
}

argumentos_t *crear_args(){
	argumentos_t *args = malloc(sizeof(argumentos_t));
	args->canal = 0;
	args->f_muestreo = 44100;
	args->pulsos = 138; 
	return args;
}

bool verificar_argumentos(int argc, const char *argv[], argumentos_t *args){
	if(argc != 7 && argc != 9 && argc != 11 && argc != 13)
		return false;
	
	for(size_t i = 1; i < argc; i++){
		if(strcmp(argv[i], "-s") == 0){
			strcpy(args->sint, argv[i+1]);
		}
		else if(strcmp(argv[i], "-i") == 0){
			strcpy(args->midi, argv[i+1]);
		}
		else if(strcmp(argv[i], "-o") == 0){
			strcpy(args->wave, argv[i+1]);
		}
		else if(strcmp(argv[i], "-c") == 0)
			args->canal = atoi(argv[i+1]);
		else if(strcmp(argv[i], "-f") == 0)
			args->f_muestreo = atoi(argv[i+1]);
		else if(strcmp(argv[i], "-r") == 0)
			args->pulsos = atoi(argv[i+1]);
	}
	return true;
}

void liberar(synth_t *s, contenedor_t *c, tramo_t *t, FILE *fm, argumentos_t *args){
	if (s != NULL) sintetizador_destruir(s);
    if (t !=  NULL) tramo_destruir(t);
   	if (fm != NULL) fclose(fm);
   	if (c != NULL) contenedor_notas_limpiar(c);
   	free(args);
}

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

	argumentos_t *args = crear_args();

	if (!verificar_argumentos(argc, argv, args)){
		uso(argv[0]);
		free(args);
		return -1;
	}

	int cnl = args->canal; 
	int f_m = args->f_muestreo;
	int pps = args->pulsos;

	FILE *fs = fopen(args->sint, "rt");
	if (fs == NULL){
		fprintf(stderr, "No se pudo abrir el archivo de sintetizador\n");
		return -1;
	}
	synth_t *s = sintetizador_crear(fs);
	fclose(fs);
	if (s == NULL){
		fprintf(stderr, "No se pudo crear el tda sintetizador\n");
		return -1;
	} imprimir_data(s);






	tramo_t *tramo = _tramo_crear(0,0,f_m);
	nota_t *nota [MAX_NOTAS]; //= calloc(MAX_NOTAS, sizeof(nota_t*));
	for (int i = 0; i < MAX_NOTAS; i++){
		nota[i] = NULL;
	}
	size_t cantidad_notas = MAX_NOTAS;






	// APERTURA DE ARCHIVO MIDI:
	FILE *fmidi = fopen(args->midi, "rb");
	if(fmidi == NULL) {
	    fprintf(stderr, "No se pudo abrir \"%s\"\n", args->midi);
	    destruir_sintetizador(s);
	    tramo_destruir(tramo);
	    return 1;
	}
	formato_t formato;
	uint16_t numero_pistas;
	uint16_t pulsos_negra;
	if(! leer_encabezado(fmidi, &formato, &numero_pistas, &pulsos_negra)) {
	    fprintf(stderr, "Fallo lectura encabezado\n");
	    liberar(s, nota, tramo, fmidi, args);
	    return 1;
	}printf("Encabezado:\n\tFormato: %s\n\tNumero de pistas: %d\n\tPulsos por negra: %d\n", codificar_formato(formato), numero_pistas, pulsos_negra);

	// ITERAMOS LAS PISTAS:
	for(uint16_t pista = 0; pista < numero_pistas; pista++) {
		uint32_t tamagno_pista;
		if(! leer_pista(fmidi, &tamagno_pista)) {
			fprintf(stderr, "Fallo lectura pista\n");
			liberar(s, nota, tramo, fmidi, args);
			return 1;
		}printf("Pista %d:\n\tTamaño: %d\n", pista, tamagno_pista);

		evento_t evento;
		char canal;
		uint32_t tiempo = 0;
		double inicio_nota = 0;
		int longitud;

		
		while(1) {
			uint32_t delta_tiempo;
			leer_tiempo(fmidi, &delta_tiempo);
			tiempo += delta_tiempo;
		//	printf("[%d] ", tiempo);

			uint8_t buffer[EVENTO_MAX_LONG];
			if(! leer_evento(fmidi, &evento, &canal, &longitud, buffer)) {
			    fprintf(stderr, "Error leyendo evento\n");
			    liberar(s, nota, tramo, fmidi, args);
			    return 1;
			}
		//	printf("Evento: %s, Canal: %d", codificar_evento(evento), canal);

	// PROCESAMIENTO DEL EVENTO:
			if(evento == METAEVENTO && canal == 0xF) {
			    // METAEVENTO:
			    if(buffer[METAEVENTO_TIPO] == METAEVENTO_FIN_DE_PISTA) {
			        printf("\nFinal de la pista %d.\n", pista);
			        break;
			    }
				descartar_metaevento(fmidi, buffer[METAEVENTO_LONGITUD]);
			}

	// PROCESAMIENTO DE LA NOTA:
			else if (evento == NOTA_ENCENDIDA || evento == NOTA_APAGADA) {   
				notas_t n_simbolo;
				signed char oct;
				inicio_nota = (double)tiempo / pps;

				if(! decodificar_nota(buffer[EVNOTA_NOTA], &n_simbolo, &oct)) {
			        fprintf(stderr, "Error leyendo nota\n");
			        liberar(s, nota, tramo, fmidi, args);
			        return -1;
			    }
			//	printf(", Nota: %s, Octava: %d, Velocidad: %d\n", codificar_nota(n_simbolo), oct, buffer[EVNOTA_VELOCIDAD]);



		//FILTRO EL CANAL
				if (canal == cnl){

					size_t i = 0;
					while(nota[i] != NULL && i < MAX_NOTAS){
						if (nota[i]->octava > 8) break;
						aumentar_duracion(nota[i], (double)delta_tiempo / pps);
						i++;
					} //Salgo cuando i == NULL (o sea, no hay ninguna nota en ese lugar del arreglo)

					if (evento == NOTA_ENCENDIDA && buffer[EVNOTA_VELOCIDAD] != 0){
						if(!nota_ya_existe(n_simbolo, oct, nota, cantidad_notas, &i)){
		/*CREO LA NOTA*/	nota[i] = crear_nota(n_simbolo, oct, buffer[EVNOTA_VELOCIDAD], inicio_nota);
						    if (nota[i] == NULL){
						    	fprintf(stderr, "\nNo se pudo guardar la nota en el tiempo [%d]\n\n", tiempo);
						    	liberar(s, nota, tramo, fmidi, args);
						    	return -1;
					    	}
					    }
					}
					else if(evento == NOTA_APAGADA || (evento == NOTA_ENCENDIDA && buffer[EVNOTA_VELOCIDAD] == 0)){	
				    	i = hallar_posicion(n_simbolo, oct, nota, cantidad_notas);
						if (nota[i] == NULL)
							continue;

					//	printf("Posición: %lu\n", i);
			    	//	imprimir_nota(nota[i]);
						
				//SÍNTESIS DE LA NOTA:
						tramo_t *tramo_nota = sintetizar_nota(nota[i], s, f_m);
						if(tramo_nota == NULL){
							//fprintf(stderr, "\n\n######\n #ERROR: No se pudo sintetizar la nota.\n######\n\n");
							nota_borrar(nota[i]);
							nota[i] = NULL;
							actualizar_lista(nota);
							continue;
						}
						if(!tramo_extender(tramo, tramo_nota)){
							fprintf(stderr, "Error extendiendo el tramo\n");
					        liberar(s, nota, tramo, fmidi, args);
					        return -1;
			        	}
			        	tramo_destruir(tramo_nota); 
					//	printf("Cantidad de muestras parcial: %lu\n\n", tramo->n);
						nota_borrar(nota[i]);
						nota[i] = NULL;
						if (nota != NULL)
							actualizar_lista(nota);
					}
				}
			}
		}

        
	}//FIN de procesado del MIDI

	destruir_sintetizador(s);
	fclose(fmidi);

//procesado del  WAVE
	FILE *fw = crear_fichero(args->wave);

	if (escribir_encabezado_wave(fw, tramo)){
		if (!volcar_muestras(fw, tramo)){
			fprintf(stderr, "\nNo se pudo escribir el archivo WAVE\n");
			liberar(NULL, nota, tramo, NULL, args);
			return -1;
		}
	}
	printf("\n\n\n### Se pudo escribir el wave :D ###\n");
	free(args);
	tramo_destruir(tramo);
	fclose(fw);

	return 0;
}

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

	argumentos_t *args = crear_args();

	if (!verificar_argumentos(argc, argv, args)){
		uso(argv[0]);
		free(args);
		return -1;
	}

	int cnl = args->canal; 
	int f_m = args->f_muestreo;
	int pps = args->pulsos;

	FILE *fs = fopen(args->sint, "rt");
	if (fs == NULL){
		fprintf(stderr, "No se pudo abrir el archivo de sintetizador\n");
		return -1;
	}
	synth_t *s = sintetizador_crear(fs);
	fclose(fs);
	if (s == NULL){
		fprintf(stderr, "No se pudo crear el tda sintetizador\n");
		return -1;
	} imprimir_data(s);






	tramo_t *tramo = _tramo_crear(0,0,f_m);
	nota_t *nota [MAX_NOTAS]; //= calloc(MAX_NOTAS, sizeof(nota_t*));
	for (int i = 0; i < MAX_NOTAS; i++){
		nota[i] = NULL;
	}
	size_t cantidad_notas = MAX_NOTAS;






	// APERTURA DE ARCHIVO MIDI:
	FILE *fmidi = fopen(args->midi, "rb");
	if(fmidi == NULL) {
	    fprintf(stderr, "No se pudo abrir \"%s\"\n", args->midi);
	    destruir_sintetizador(s);
	    tramo_destruir(tramo);
	    return 1;
	}
	formato_t formato;
	uint16_t numero_pistas;
	uint16_t pulsos_negra;
	if(! leer_encabezado(fmidi, &formato, &numero_pistas, &pulsos_negra)) {
	    fprintf(stderr, "Fallo lectura encabezado\n");
	    liberar(s, nota, tramo, fmidi, args);
	    return 1;
	}printf("Encabezado:\n\tFormato: %s\n\tNumero de pistas: %d\n\tPulsos por negra: %d\n", codificar_formato(formato), numero_pistas, pulsos_negra);

	// ITERAMOS LAS PISTAS:
	for(uint16_t pista = 0; pista < numero_pistas; pista++) {
		uint32_t tamagno_pista;
		if(! leer_pista(fmidi, &tamagno_pista)) {
			fprintf(stderr, "Fallo lectura pista\n");
			liberar(s, nota, tramo, fmidi, args);
			return 1;
		}printf("Pista %d:\n\tTamaño: %d\n", pista, tamagno_pista);

		evento_t evento;
		char canal;
		uint32_t tiempo = 0;
		double inicio_nota = 0;
		int longitud;

		
		while(1) {
			uint32_t delta_tiempo;
			leer_tiempo(fmidi, &delta_tiempo);
			tiempo += delta_tiempo;
		//	printf("[%d] ", tiempo);

			uint8_t buffer[EVENTO_MAX_LONG];
			if(! leer_evento(fmidi, &evento, &canal, &longitud, buffer)) {
			    fprintf(stderr, "Error leyendo evento\n");
			    liberar(s, nota, tramo, fmidi, args);
			    return 1;
			}
		//	printf("Evento: %s, Canal: %d", codificar_evento(evento), canal);

	// PROCESAMIENTO DEL EVENTO:
			if(evento == METAEVENTO && canal == 0xF) {
			    // METAEVENTO:
			    if(buffer[METAEVENTO_TIPO] == METAEVENTO_FIN_DE_PISTA) {
			        printf("\nFinal de la pista %d.\n", pista);
			        break;
			    }
				descartar_metaevento(fmidi, buffer[METAEVENTO_LONGITUD]);
			}

	// PROCESAMIENTO DE LA NOTA:
			else if (evento == NOTA_ENCENDIDA || evento == NOTA_APAGADA) {   
				notas_t n_simbolo;
				signed char oct;
				inicio_nota = (double)tiempo / pps;

				if(! decodificar_nota(buffer[EVNOTA_NOTA], &n_simbolo, &oct)) {
			        fprintf(stderr, "Error leyendo nota\n");
			        liberar(s, nota, tramo, fmidi, args);
			        return -1;
			    }
			//	printf(", Nota: %s, Octava: %d, Velocidad: %d\n", codificar_nota(n_simbolo), oct, buffer[EVNOTA_VELOCIDAD]);



		//FILTRO EL CANAL
				if (canal == cnl){

					size_t i = 0;
					while(nota[i] != NULL && i < MAX_NOTAS){
						if (nota[i]->octava > 8) break;
						aumentar_duracion(nota[i], (double)delta_tiempo / pps);
						i++;
					} //Salgo cuando i == NULL (o sea, no hay ninguna nota en ese lugar del arreglo)

					if (evento == NOTA_ENCENDIDA && buffer[EVNOTA_VELOCIDAD] != 0){
						if(!nota_ya_existe(n_simbolo, oct, nota, cantidad_notas, &i)){
		/*CREO LA NOTA*/	nota[i] = crear_nota(n_simbolo, oct, buffer[EVNOTA_VELOCIDAD], inicio_nota);
						    if (nota[i] == NULL){
						    	fprintf(stderr, "\nNo se pudo guardar la nota en el tiempo [%d]\n\n", tiempo);
						    	liberar(s, nota, tramo, fmidi, args);
						    	return -1;
					    	}
					    }
					}
					else if(evento == NOTA_APAGADA || (evento == NOTA_ENCENDIDA && buffer[EVNOTA_VELOCIDAD] == 0)){	
				    	i = hallar_posicion(n_simbolo, oct, nota, cantidad_notas);
						if (nota[i] == NULL)
							continue;

					printf("Posición: %lu\n", i);
			    		imprimir_nota(nota[i]);
						
				//SÍNTESIS DE LA NOTA:
						tramo_t *tramo_nota = sintetizar_nota(nota[i], s, f_m);
						if(tramo_nota == NULL){
							//fprintf(stderr, "\n\n######\n #ERROR: No se pudo sintetizar la nota.\n######\n\n");
							nota_borrar(nota[i]);
							nota[i] = NULL;
							actualizar_lista(nota);
							continue;
						}
						if(!tramo_extender(tramo, tramo_nota)){
							fprintf(stderr, "Error extendiendo el tramo\n");
					        liberar(s, nota, tramo, fmidi, args);
					        return -1;
			        	}
			        	tramo_destruir(tramo_nota); 
					printf("Cantidad de muestras parcial: %lu\n\n", tramo->n);
					nota_borrar(nota[i]);
					nota[i] = NULL;
					if (nota != NULL)
						actualizar_lista(nota);
					}
				}
			}
		}

        
	}//FIN de procesado del MIDI

	destruir_sintetizador(s);
	fclose(fmidi);

//procesado del  WAVE
	FILE *fw = crear_fichero(args->wave);

	if (escribir_encabezado_wave(fw, tramo)){
		if (!volcar_muestras(fw, tramo)){
			fprintf(stderr, "\nNo se pudo escribir el archivo WAVE\n");
			liberar(NULL, nota, tramo, NULL, args);
			return -1;
		}
	}
	printf("\n\n\n### Se pudo escribir el wave :D ###\n");
	free(args);
	tramo_destruir(tramo);
	fclose(fw);

	return 0;
}




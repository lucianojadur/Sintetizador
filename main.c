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
		borrar_argumentos(args);
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
	nota_t *nota = NULL;

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

		// ITERAMOS LAS PISTAS:
		while(1) {
			uint32_t delta_tiempo;
			leer_tiempo(fmidi, &delta_tiempo);
			tiempo += delta_tiempo;
			printf("[%d] ", tiempo);
			

			uint8_t buffer[EVENTO_MAX_LONG];
			if(! leer_evento(fmidi, &evento, &canal, &longitud, buffer)) {
			    fprintf(stderr, "Error leyendo evento\n");
			    liberar(s, nota, tramo, fmidi, args);
			    return 1;
			}
			printf("Evento: %s, Canal: %d", codificar_evento(evento), canal);
// PROCESAMIENTO DEL EVENTO:
			if(evento == METAEVENTO && canal == 0xF) {
			    // METAEVENTO:
			    if(buffer[METAEVENTO_TIPO] == METAEVENTO_FIN_DE_PISTA) {
			        putchar('\n');
			        printf("Final de la pista %d.\n", pista);
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
			    } printf(", Nota: %s, Octava: %d, Velocidad: %d\n", codificar_nota(n_simbolo), oct, buffer[EVNOTA_VELOCIDAD]);
				

				if (canal == cnl){
				
					if (evento == NOTA_ENCENDIDA && buffer[EVNOTA_VELOCIDAD] != 0){
						if(nota == NULL){
		/*CREO LA NOTA*/	nota = crear_nota(n_simbolo, oct, buffer[EVNOTA_VELOCIDAD], inicio_nota);
						    if (nota == NULL){
						    	fprintf(stderr, "\nNo se pudo crear la nota en el tiempo [%d]\n\n", tiempo);
						    	liberar(s, nota, tramo, fmidi, args);
						    	return -1;
					    	}
					    }else
					    	nota->duracion += (double)delta_tiempo / pps;

					}else if(evento == NOTA_APAGADA || (evento == NOTA_ENCENDIDA && buffer[EVNOTA_VELOCIDAD] == 0)){
						if (nota == NULL)
							continue;
						aumentar_duracion(nota, (double)delta_tiempo / pps);
						
			    		imprimir_nota(nota);
						
				//SÍNTESIS DE LA NOTA:
						tramo_t *tramo_nota = sintetizar_nota(nota, s, f_m);
						if(tramo_nota == NULL){
							fprintf(stderr, "\n\n######\n #ERROR: No se pudo sintetizar la nota.\n######\n\n");
							continue;
						}
						if(!tramo_extender(tramo, tramo_nota)){
							fprintf(stderr, "Error extendiendo el tramo\n");
					        liberar(s, nota, tramo, fmidi, args);
					        return -1;
			        	}
			        	tramo_destruir(tramo_nota); 
						printf("Cantidad de muestras parcial: %lu\n\n", tramo->n);
						nota_borrar(nota);
						nota = NULL;
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
			liberar(NULL, NULL, tramo, NULL, args);
			return -1;
		}
	}
	printf("\n\n\n### Se pudo escribir el wave :D ###\n");
	borrar_argumentos(args);
	tramo_destruir(tramo);
	fclose(fw);

	return 0;
}

/*Modulo MIDI
Este módulo se encarga de procesar la información contenida en un archivo de estandar MIDI. Contiene funciones de 
lectura de archivo binario para traducir la información del fichero a BIG ENDIAN y funciones para extraer información
de cada pista del archivo segmentada en: información de pista, intervalos de tiempo y eventos.
*/

#ifndef MIDI_H
#define MIDI_H
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "nota.h"

#define MSB_MASK_ON		0x80
#define HEADER_VALUE	0x4D546864
#define TRACK_ID_VALUE	0x4D54726B
#define MSB_MAX_TIME_ON	0x80000000

#define SHIFT_1_BIT			1
#define SHIFT_1_BYTE 		8
#define SHIFT_2_BYTES 		16
#define SHIFT_3_BYTES 		24
#define SHIFT_4_BYTES		32
#define SHIFT_HEADER		8
#define SHIFT_TRACK_ID		4		
#define SHIFT_TRACK_SIZE	8
#define SHIFT_VALUE			4
#define METAEVENTO_FIN_DE_PISTA 0x2F
#define EVENTO_MAX_LONG 10

enum {EVNOTA_NOTA, EVNOTA_VELOCIDAD};
enum {METAEVENTO_TIPO, METAEVENTO_LONGITUD};

typedef enum {
	PISTA_UNICA,
	MULTIPISTA_SINCRONICA,
	MULTIPISTA_ASINCRONICA
}formato_t;

typedef enum{
	NOTA_APAGADA,
	NOTA_ENCENDIDA,
	NOTA_DURANTE,
	CAMBIO_CONTROL,
	CAMBIO_PROGRAMA,
	VARIAR_CANAL,
	CAMBIO_TONO,
	METAEVENTO
}evento_t;


typedef struct{
	evento_t evento;
	char * tipo_evento;
	int longitud;
}prop_evento_t;



bool procesar_midi(FILE *f, nota_t *notas[], size_t cantidad_notas, int cnl, int pps);

bool decodificar_formato(uint16_t valor, formato_t *formato);

bool decodificar_evento(uint8_t valor, evento_t *evento, char *canal, int *longitud);

bool decodificar_nota(uint8_t valor, notas_t *nota, signed char *octava);


const char *codificar_formato(formato_t formato);

const char *codificar_evento(evento_t evento);

const char *codificar_nota(notas_t nota);


uint8_t leer_uint8_t(FILE *f);

uint16_t leer_uint16_big_endian(FILE *f);

uint32_t leer_uint32_big_endian(FILE *f);


bool leer_encabezado(FILE *f, formato_t *formato, uint16_t *numero_pistas, uint16_t *pulsos_negra);

bool leer_pista(FILE *f, uint32_t *tamagno);

bool leer_tiempo(FILE *f, uint32_t *tiempo);

bool leer_evento(FILE *f, evento_t *evento, char *canal, int *longitud, uint8_t mensaje[]);

void descartar_metaevento(FILE *f, uint8_t tamagno);


#endif

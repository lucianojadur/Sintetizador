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
#include "contenedor_notas.h"
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


bool procesar_midi(FILE *f, contenedor_t *notas, int cnl, int pps);
/*Lee un archivo midi y guarda todas las notas de un canal determinado por cnl con sus correspondientes atributos
en notas. Los tiempos de cada nota son convertidos de pulsos por negra a segundos según el valor de pps*/


#endif

#include "midi.h"


const char * nombres_formatos[] = {
	[PISTA_UNICA] = "Pista única",
	[MULTIPISTA_SINCRONICA] = "Multipista sincrónica",
	[MULTIPISTA_ASINCRONICA] = "Multipista asincrónica"
};

prop_evento_t propiedades[] = {
	{NOTA_APAGADA, "Nota apagada", 2},
	{NOTA_ENCENDIDA, "Nota encendida", 2},
	{NOTA_DURANTE, "Nota durante", 2},
	{CAMBIO_CONTROL, "Cambio de control", 2},
	{CAMBIO_PROGRAMA,"Cambio de programa", 1},
	{VARIAR_CANAL,"Variar canal", 1},
	{CAMBIO_TONO,"Cambio de tono", 2},
	{METAEVENTO,"Metaevento", 2}
};

const char * lista_notas_sostenido[] = {
	[C] = "C",
	[C_SHARP] = "C#",
	[D] = "D",
	[D_SHARP] = "D#",
	[E] = "E",
	[F] = "F",
	[F_SHARP] = "F#",
	[G] = "G",
	[G_SHARP] = "G#",
	[A] = "A",
	[A_SHARP] = "A#",
	[B] = "B",
};



//DECODIFICACIÓN

bool decodificar_formato(uint16_t valor, formato_t *formato){
	/*Dado el valor recibido, guarda en el puntero formato el formato representado por el valor.
	Si el valor es correcto devuelve true, de otra forma devuelve false*/
	if(valor >= 0 && valor <= 2){
		*formato = valor;
		return true;
	}
	return false;
}


bool decodificar_evento(uint8_t valor, evento_t *evento, char *canal, int *longitud){
	/*Dado el valor recibido guarda en el puntero a evento el evento que dicho valor representa; en el puntero a canal,
	el canal que dicho valor representa; y en el puntero a longitud la longitud del evento.
	El valor ingresado debe contener su MSB en estado alto, es decir, debe ser mayor a 127. De otro modo, la
	palabra correspondiente al evento no será válida.
	Si el valor no se corresponde con un evento válido devuelve false sin modificar ningún parámetro; si todo sale bien
	devuevle true*/ 
	
	if((valor & MSB_MASK_ON) == MSB_MASK_ON){
		*evento = (valor & (~MSB_MASK_ON)) >> SHIFT_VALUE; 
		*canal = (uint8_t)(valor << SHIFT_VALUE) >> SHIFT_VALUE;
		*longitud = propiedades[*evento].longitud;
		
		return true;
	}
	return false;
}


bool decodificar_nota(uint8_t valor, notas_t *nota, signed char *octava){
	/*Dado el valor recibido guarda en nota la nota representada por dicho valor y en el puntero octava
	guarda la octava correspondiente. Si la nota es correcta devueve true, en caso contrario devuelve false*/

	if(valor <= 127){
		*nota = valor % 12;
		*octava = (char)(valor / 12 - 1); //resto 1 porque los primeros 12 semitonos corresponden a la octava 0
		return true;
	}
	return false;
}



//CODIFICACIÓN

const char *codificar_formato(formato_t formato){
	return nombres_formatos[formato];
}


const char *codificar_evento(evento_t evento){
	return propiedades[evento].tipo_evento;
}


const char *codificar_nota(notas_t nota){
	return lista_notas_sostenido[nota];
}



//ENDIANNES

uint8_t leer_uint8_t(FILE *f){
	uint8_t palabra;
	fread(&palabra, sizeof(uint8_t), 1, f);
	
	return palabra;
}


uint16_t leer_uint16_big_endian(FILE *f){
	uint8_t p[2];
	fread(p, 1, 2, f);
	uint16_t palabra = p[1] | p[0];
	return palabra;
}


uint32_t leer_uint32_big_endian(FILE *f){
	uint8_t p[4];
	fread(p, 1, 4, f);				
	uint32_t palabra = (p[0] << SHIFT_3_BYTES) | (p[1] << SHIFT_2_BYTES) | (p[2] << SHIFT_1_BYTE) | p[3];
	return palabra;
}


//PROCESAMIENTO DE ARCHIVO

bool leer_encabezado(FILE *f, formato_t *formato, uint16_t *numero_pistas, uint16_t *pulsos_negra){
	uint32_t header = leer_uint32_big_endian(f);
	uint32_t hsize = leer_uint32_big_endian(f);
	if(header != HEADER_VALUE || hsize != 6) return false;

	uint16_t fmt = leer_uint16_big_endian(f);
	*numero_pistas = leer_uint16_big_endian(f);
	*pulsos_negra = leer_uint16_big_endian(f);
	return decodificar_formato(fmt, formato);
}


//PISTAS

bool leer_pista(FILE *f, uint32_t *tamagno){
	uint32_t track_id = leer_uint32_big_endian(f);
	if (track_id != TRACK_ID_VALUE) return false;

	*tamagno = leer_uint32_big_endian(f);
	return true;
}


bool leer_tiempo(FILE *f, uint32_t *tiempo){
	size_t i = 0;
	uint32_t t_total = 0;

	while(i < 4){
		uint8_t byte = leer_uint8_t(f);
		uint8_t t = byte & (~MSB_MASK_ON);
		t_total = (t_total << (i * SHIFT_1_BYTE - i)) | t;
		if ((byte & MSB_MASK_ON) == 0)
			break;
		i++;
	}
	if (i == 3 && (t_total & MSB_MAX_TIME_ON) == MSB_MAX_TIME_ON)
		return false; //el tamaño del tiempo es 4 y su MSB está en 1
	
	*tiempo = t_total;
	return true;
}


bool leer_evento(FILE *f, evento_t *evento, char *canal, int *longitud, uint8_t mensaje[]){
	uint8_t dato;
	size_t i = 0;
	fread(&dato, sizeof(uint8_t), 1, f);
	if(!decodificar_evento(dato, evento, canal, longitud)){
		mensaje[0] = dato;
		i++;
	}
	while(i < *longitud ){
		mensaje[i] = leer_uint8_t(f);
		i++;
	}
	return true;
}


void descartar_metaevento(FILE *f, uint8_t tamagno){
	fseek(f, tamagno, SEEK_CUR);
}
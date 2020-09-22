#include "wave.h"

#define MAX_INT16_T 32767


FILE* crear_fichero(const char *nombre){
	FILE *f = fopen(nombre, "wb");
	if(f == NULL)
		return NULL;
	return f;
}

bool escribir_encabezado_wave(FILE *f, tramo_t *tramo){
	size_t n =  tramo_obtener_tamanio(tramo);

	if (tramo == NULL || n == 0)
		return false;
	const char chunk_id[5] = "RIFF";
	const char format[5] = "WAVE";
	const char subchunk1_id[5] = "fmt ";
	const uint32_t chunk_size = 36 + 2 * n;
	const uint32_t subchunk1_size = 16;
	const uint16_t audio_format = 1;
	const uint16_t num_channels = 1;
	const uint32_t sample_rate = tramo_obtener_f_muestreo(tramo);
	uint32_t byte_rate = 2 * sample_rate;
	uint16_t block_align = 2;
	uint16_t bp_sample = 16;
	const char subchunk2_id[5] = "data";
	const uint32_t subchunk2_size = 2 * n;

	fwrite(&chunk_id, sizeof(char), 4, f);
	fwrite(&chunk_size, sizeof(uint32_t), 1, f);
	fwrite(&format, sizeof(char), 4, f);
	fwrite(&subchunk1_id, sizeof(char), 4, f);
	fwrite(&subchunk1_size, sizeof(uint32_t), 1, f);
	fwrite(&audio_format, sizeof(uint16_t), 1, f);
	fwrite(&num_channels, sizeof(uint16_t), 1, f);
	fwrite(&sample_rate, sizeof(uint32_t), 1, f);
	fwrite(&byte_rate, sizeof(uint32_t), 1, f);
	fwrite(&block_align, sizeof(uint16_t),1, f);
	fwrite(&bp_sample, sizeof(uint16_t), 1, f);
	fwrite(&subchunk2_id, sizeof(char), 4, f);
	fwrite(&subchunk2_size, sizeof(uint32_t), 1, f);

	return true;
}


bool volcar_muestras(FILE *f, tramo_t* tramo){
	size_t n =  tramo_obtener_tamanio(tramo);

	int16_t *vint =  malloc(sizeof(int16_t)*(n + 1));
	if(vint == NULL)
		return false;

	float max = tramo_hallar_max(tramo);
	if (max == 0) return false;
	for(size_t i = 0; i < n; i++){
		tramo_multiplicar_muestra(tramo, i, (MAX_INT16_T / max));
		vint[i] = (int16_t)tramo_copiar_muestra(tramo, i);
		fwrite(vint + i, sizeof(int16_t), 1, f);
	}
	free(vint);
	return true;
}


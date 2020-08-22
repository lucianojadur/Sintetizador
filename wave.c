#include "wave.h"

#define MAX_INT16_T 32767


static float _max(float v[], size_t i);
/*Recibe un vector -no necesariamente ordenado- y devuelve el mayor elemento*/


FILE* crear_fichero(const char *nombre){
	FILE *f = fopen(nombre, "wb");
	if(f == NULL)
		return NULL;
	return f;
}

bool escribir_encabezado_wave(FILE *f, tramo_t *tramo){
	if (tramo == NULL || tramo->n == 0)
		return false;
	const char chunk_id[5] = "RIFF";
	const char format[5] = "WAVE";
	const char subchunk1_id[5] = "fmt ";
	const uint32_t chunk_size = 36 + 2 * tramo->n;
	const uint32_t subchunk1_size = 16;
	const uint16_t audio_format = 1;
	const uint16_t num_channels = 1;
	const uint32_t sample_rate = tramo->f_m;
	uint32_t byte_rate = 2 * sample_rate;
	uint16_t block_align = 2;
	uint16_t bp_sample = 16;
	const char subchunk2_id[5] = "data";
	const uint32_t subchunk2_size = 2 * tramo->n;

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
	int16_t *vint =  malloc(sizeof(int16_t)*(tramo->n + 1));
	if(vint == NULL)
		return false;

	float max = _max(tramo->v, tramo->n);
	if (max == 0) return false;
	for(size_t i = 0; i < tramo->n; i++){
		vint[i] = (int16_t)(tramo->v[i] * (MAX_INT16_T / max));
		fwrite(vint + i, sizeof(int16_t), 1, f);
	}
	free(vint);
	return true;
}


static float _max(float v[], size_t n){
	float max = v[0];
	for (size_t i = 1; i < n; i++){
		if (v[i] > max)
			max = v[i];
	}
	return max;
}


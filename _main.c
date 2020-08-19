#include "_main.h"


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

void borrar_argumentos(argumentos_t *args){
	free(args);
}

void liberar_todo(synth_t *s, nota_t *n, tramo_t *t, FILE *fm, argumentos_t *args){
    destruir_sintetizador(s);
    nota_borrar(n);
    tramo_destruir(t);
   	fclose(fm);
}
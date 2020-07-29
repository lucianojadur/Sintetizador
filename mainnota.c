//prueba del tda nota
#include <stdio.h>
#include "nota.h"


int main(int argc, char const *argv[]){
	
	if (argc != 2){
		fprintf(stderr, "Ingresá un archivo de sintetizador como argumento\n");
		return -1;
	}

	FILE *fichero = fopen(argv[1], "r");
	
	if (fichero == NULL){
		fprintf(stderr, "No se pudo abrir el archivo correctamente\n");
		return -1;
	}
	nota_t * nota = crear_nota();
	if (nota == NULL){
		fprintf(stderr, "Error al crear una nota\n");
		fclose(fichero);
		return -1;
	}
	if(!obtener_cantidad_armonicos(fichero, nota)){
		fprintf(stderr, "No se pudo leer la cantidad de armonicos\n");
		free(nota);
		fclose(fichero);
		return -1;
	}
	imprimir_cantidad_armonicos(nota);
	if (!obtener_armonicos(fichero, nota)){
		fprintf(stderr, "No se pudieron obtener los armónicos\n");
		free(nota);
		fclose(fichero);
		return -1;
	}
	imprimir_armonicos(nota);
	free(nota);
	fclose(fichero);

	return 0;
}


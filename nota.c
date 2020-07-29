//TDA NOTA
//#include "muestreo.h"
#include "nota.h"



static char * _leer_linea(FILE *f, size_t size);

static bool _obtener_datos_armonico(char * armonico_linea, const char * delim, float * f, float * a);

//static bool _obtener_instantes(char * linea, float instantes[3]){
//}


nota_t* crear_nota() {
	nota_t *nota = malloc(sizeof(nota_t));
	if (nota == NULL)
		return NULL;
	return nota;
}


bool obtener_cantidad_armonicos(FILE *f, nota_t* nota){
	char cantidad[2];
	if(fgets(cantidad, 2, f) == NULL) return false;
	nota->cantidad_armonicos = atoi(cantidad);
	return true;
}


bool obtener_armonicos(FILE *f, nota_t *nota){
	char * linea;
	const char delim[2] = LINE_DELIMITER;
	fgetc(f);
	for (size_t i =  0; i < nota->cantidad_armonicos; i++){	//[cantidad de armonicos] líneas siguientes
		if ((linea = _leer_linea(f, HARM_STR_LENGTH + 1)) == NULL) //malloc
			return false;
		if (!_obtener_datos_armonico(linea, delim, &nota->armonicos[i][REL_FREC_INDEX], &nota->armonicos[i][REL_AMP_INDEX]))
			return false;
		free(linea);
	}
	return true;
}


bool obtener_parametros_modular(FILE *f, nota_t *nota){
	char *data = _leer_linea(f, PARAMS_LENGTH);	//malloc
	if (data == NULL)
		return false;
	while(isdigit(data[0]) > 0) {
		if((data = _leer_linea(f, PARAMS_LENGTH)) == NULL)
			return false;
	}									//cuando data[0] es alfabético, salgo del while y data = primer parámetro
	for (size_t i = 0; i < 3; i++){
		if (isalpha(data[0]) == 0 || data == NULL)
			return false;
		nota->parametros[i] = data;
		if(i < 2)
			data = _leer_linea(f, PARAMS_LENGTH);
	}
	return true;
}



static char * _leer_linea(FILE *f, size_t size){
	char *linea = malloc(sizeof(char) * size);
	fgets(linea, 20, f);
	return linea;
}

static bool _obtener_datos_armonico(char * armonico_linea, const char * delim, float * f, float * a){
	if (strlen(armonico_linea) != HARM_STR_LENGTH)
		return false;
	char * amp_ptr = armonico_linea + 2;	//creo puntero al comienzo de la data de amplitud en la cadena
	char temp[MAX_AMP_DIGITS + 1];	// = malloc(sizeof(char) * 9);

	*f = atof(strtok(armonico_linea, delim)); //guardo en f la primera cadena antes del espacio ('\b')
	strcpy(temp, amp_ptr);	//copio a temp la cadena que queda después del espacio	
	*a = atof(temp);

	return true;  
}

/*static char * _leer_linea(FILE *f){
	char *linea = malloc(sizeof(char) * 12);
	if (linea == NULL)
		return NULL;
	if (fgets(linea, 20, f) == NULL){
		free(linea);
		return NULL;
	}
	return linea;
}*/



 /////////////////
//impresión//////
void imprimir_cantidad_armonicos(nota_t *nota){
	printf("cantidad de armónicos: %i\n", nota->cantidad_armonicos);
}

void imprimir_armonicos(nota_t *nota) {
	for(size_t i = 0; i < nota->cantidad_armonicos; i++)
		printf("Frec: %.1f; Amp: %.5f\n", nota->armonicos[i][REL_FREC_INDEX], nota->armonicos[i][REL_AMP_INDEX]);
}

void imprimir_parametros(nota_t *nota){
	puts("parámetros para modular:");
	for (size_t i = 0; i < 3; i++){
		printf("%s\n", nota->parametros[i]);
	}
}

void destruir_parametros(nota_t *nota){
	for (size_t i = 0; i < 3; i++)
		free(nota->parametros[i]);
}

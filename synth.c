//TDA SINTETIZADOR
#include "sintetizador.h"

#define PI 3.141592

static params_t *_obtener_informacion(char* linea);
/*Copia en una estructura los atributos correspondientes a una linea de texto que contiene información sobre la 
modulación de la onda senoidal de una nota musical*/

static bool _obtener_cantidad_armonicos(FILE *f, size_t *n);
/*Copia el primer caracter de un archivo de texto con la información de un sintetizador en caso de iniciar correctamente*/

static bool _obtener_armonicos(FILE *f, float v[][2], size_t n);
/*Guarda en una matriz de 2 columnas de n filas los valores numéricos correspondientes a los armónicos de un timbre determinado
guardados el un archivo de sintetizador*/

static bool _obtener_lineas_parametros(FILE *f, char* lineas[], const size_t cantidad_lineas);
//guarda en lineas[] las ultimas 3 lineas del archivo de sintetizador apuntado por f

static void _obtener_clave(char *linea, char *clave);
/*copia en clave la cadena alfabética contenida en linea. Se espera que la cadena se encuentre al comienzo de linea*/

static void _obtener_valores_parametros(char* linea, float v[3]);

static char *_leer_linea(FILE *f, size_t size);
//copia en un char * una línea de un archivo de texto

static bool _obtener_datos_armonico(char * linea, float * f, float * a);
//recibe una cadena de caracteres y guarda en f y a los valores correspondientes separados por delim

static bool _hallar_operaciones(accion_t f[], size_t n, synth_t *s, const funciones_t funciones[]);
//Guarda en funciones[] las 3 funciones que modulan cada intervalo de la onda de una nota.

static tramo_t *_modular_intervalo(nota_t* nota, size_t inicio, double duracion, int f_m, synth_t *s, intervalo_t in, accion_t accion);
/*crea un tramo de muestras de cierto intervalo de duración de una nota musical a partir de su índice de inicio hasta su duración, 
a una frecuencia de muestreo f_m, sintetizado por la función acción según los parámetros guardados en el sintetizador s*/

static void _imprimir_parametros(params_t *p);


static float _constant(double t, float params[3]) { return 1; }
static float _linear(double t, float params[3])	{ return t/params[0]; }
static float _invlinear(double t, float params[3]) { return 1 - t/(float)params[0];}
static float _sin(double t, float params[3]) { return 1 + params[1] * sin(params[2] * t); }
static float _exp(double t, float params[3]) { return exp((5 * (t - params[0])) / params[0]); }
static float _invexp(double t, float params[3]) { return exp((-5 * t) / params[0]); }
static float _quartcos(double t, float params[3]) { return cos((PI * t) / (2 * params[0])); }
static float _quartsin(double t, float params[3]) { return sin((PI * t) / (2 * params[0])); }
static float _halfcos(double t, float params[3]) { return 0.5 + cos((PI * t) / params[0]) / 2; }
static float _halfsin(double t, float params[3]) { return 0.5 + sin(PI * (t / params[0] - 0.5)) / 2; }
static float _log(double t, float params[3]) { return log10(1 + (9 * t)/params[0]); }
static float _invlog(double t, float params[3]) { return t < params[0] ? log10(10 + (-9 * t)/params[0]) : 0; }
static float _tri(double t, float params[3]) {
	if(t < params[1])
		return t * params[2] / params[1];
	else if(t > params[1])
		return ((float)(t - params[0]) / (params[1] - params[0])) * (params[2] - 1) + params[2];
	return 1;
}
static float _pulses(double t, float params[3]){
	return 1;
}

static const funciones_t funciones[] = {	
	[CONSTANT]	= {"CONSTANT", _constant},
	[LINEAR] 	= {"LINEAR", _linear},
	[INVLINEAR] = {"INVLINEAR", _invlinear},
	[SIN] 		= {"SIN", _sin},
	[EXP] 		= {"EXP", _exp},
	[INVEXP] 	= {"INVEXP", _invexp},
	[QUARTCOS]	= {"QUARTCOS", _quartcos},
	[QUARTSIN]	= {"QUARTSIN", _quartsin},
	[HALFCOS] 	= {"HALFCOS", _halfcos},
	[HALFSIN]	= {"HALFSIN", _halfsin},
	[LOG]		= {"LOG", _log},
	[INVLOG]	= {"INVLOG", _invlog},
	[TRI]		= {"TRI", _tri},
	[PULSES]	= {"PULSES", _pulses}
};


synth_t *sintetizador_crear(FILE *f){
	synth_t *s = malloc(sizeof(synth_t)*3);
	if (s == NULL) return NULL;

	if(!_obtener_cantidad_armonicos(f, &s->n) || !_obtener_armonicos(f, s->armonicos, s->n)){
		free(s);
		return NULL;
	}
	char* lineas[3];
	if(!_obtener_lineas_parametros(f, lineas, 3)){
		free(s);
		return NULL;
	}
	for(size_t i = 0; i < 3; i++){
		s->data[i]  = _obtener_informacion(lineas[i]);	//malloc
		if(s->data[i] == NULL){
			destruir_sintetizador(s);
			return NULL;
		}
		free(lineas[i]);
	}
	return s;
}

void destruir_sintetizador(synth_t *s){
	if(s->data != NULL)
		for(size_t i = 0; i < 3; i++)
			free(s->data[i]);
	free(s);
}


tramo_t *sintetizar_nota(nota_t *nota, synth_t *s, int f_m){
	accion_t operaciones[3];
	if(! _hallar_operaciones(operaciones, 3, s, funciones))
		return NULL;

	double t_a = s->data[ATAQUE]->instantes[0];
	double t_s = nota->duracion - t_a - nota->inicio;
	double t_d = s->data[CAIDA]->instantes[0];
//ATAQUE
	tramo_t *nota_tramo = _modular_intervalo(nota, 0, t_a, f_m, s, ATAQUE, operaciones[ATAQUE]);
	if(nota_tramo == NULL)
		return NULL;
//SUSTAIN
	tramo_t *sus_tramo = _modular_intervalo(nota, nota_tramo->n, t_s, f_m, s, SUSTAIN, operaciones[SUSTAIN]);
	if(sus_tramo == NULL){
		tramo_destruir(nota_tramo);
		return NULL;
	}
	if(!tramo_extender(nota_tramo, sus_tramo)){
		tramo_destruir(sus_tramo);
		return NULL;
	}tramo_destruir(sus_tramo);
//CAIDA
	tramo_t *dec_tramo = _modular_intervalo(nota, nota_tramo->n, t_d, f_m, s, CAIDA, operaciones[CAIDA]);
	if(dec_tramo == NULL){
		tramo_destruir(nota_tramo);
		return NULL;
	}if(!tramo_extender(nota_tramo, dec_tramo)){
		tramo_destruir(dec_tramo);
		return NULL;
	}tramo_destruir(dec_tramo);

	return nota_tramo;
}


void imprimir_data(synth_t *s){
	printf("Cantidad de armónicos: %lu\n", s->n);
	for (size_t i = 0; i < s->n; i++)
		printf("Frec: %.1f, Amp: %.3f\n", s->armonicos[i][0], s->armonicos[i][1]);

	for(size_t i = 0; i < 3; i++)
		_imprimir_parametros(s->data[i]);
}




static params_t *_obtener_informacion(char* linea){
	params_t *data = malloc(sizeof(params_t)+1);
	if(data == NULL)return NULL;

	char nombre_funcion[MAX_KEYS_LENGTH+1];	//data->clave
	_obtener_clave(linea, nombre_funcion);

	float v[3] = {0,0,0};								//data->instantes
	_obtener_valores_parametros(linea, v);
	strcpy(data->clave, nombre_funcion);
	for (size_t i = 0; i < 3; i++){
		data->instantes[i] = v[i];
	}
	
	return data;
}

static bool _obtener_cantidad_armonicos(FILE *f, size_t *n){
	char cantidad[2];
	if(fgets(cantidad, 2, f) == NULL) return false;
	*n = atoi(cantidad);
	return true;
}

static bool _obtener_armonicos(FILE *f, float v[][2], size_t n){
	char * linea;
	fgetc(f);
	for (size_t i =  0; i < n; i++){	
		if ((linea = _leer_linea(f, HARM_STR_LENGTH + 1)) == NULL) //malloc
			return false;
		if (!_obtener_datos_armonico(linea, &v[i][FREC_INDEX], &v[i][AMP_INDEX])){
			free(linea);
			return false;
		}
		free(linea);
	}
	return true;
}

static bool _obtener_lineas_parametros(FILE *f, char *lineas[], const size_t cantidad_lineas){
	char *data = _leer_linea(f, PARAMS_LENGTH);	//malloc
	if (data == NULL)
		return false;
	while(isdigit(data[0]) > 0) {
		if((data = _leer_linea(f, PARAMS_LENGTH)) == NULL)
			return false;
		free(data);
	}	//cuando data[0] es alfabético, salgo del while y ya data = primer parámetro
	for (size_t i = 0; i < cantidad_lineas; i++){
		if (isalpha(data[0]) == 0 || data == NULL){
			free(data);
			return false;
		}
		lineas[i] = data;
		data = _leer_linea(f, PARAMS_LENGTH+5);
	}
	free(data);
	return true;
}


static void _obtener_clave(char *linea, char *clave){
	size_t c = 0;
	while(isalpha(linea[c]) != 0 || linea[c] == '\0' ){
		c++;
	}
	memcpy(clave, linea, c);
	clave[c] = '\0';
}


static void _obtener_valores_parametros(char* linea, float v[3]){
	char temp[10];
	size_t aux;
	size_t c = 0, i = 0;
	while(linea[c] != '\0'){

		if (isdigit(linea[c]) > 0 || linea[c] == '.'){
			aux = c;	//copio la direccion apuntada por c
			aux++;		//avanzo desde la copia	
			while(linea[aux] != '\0'){
				if(linea[aux] == ' ' || linea[aux] == '\n') break;//dejo de avanzar cuando termina el numero
				aux++;	//avanzo desde la copia
			}
			memcpy(temp, &linea[c], aux - c);
			v[i] = strtof(temp, NULL);
			memset(temp, 0, 10);
			i++;
			c = aux;
		}
		else
			c++;
	}
}



static char * _leer_linea(FILE *f, size_t size){
	char *linea = malloc(sizeof(char) * (size + 1));
	if(linea == NULL) return NULL;
	fgets(linea, size, f);
	return linea;
}

static bool _obtener_datos_armonico(char * linea, float * f, float * a){
	if (strlen(linea) > HARM_STR_LENGTH)
		return false;
	char *p;
	*f = strtof(linea, &p);
	*a = strtof(p, NULL);

	return true;  
}

static bool _hallar_operaciones(accion_t f[], size_t n, synth_t *s, const funciones_t funciones[]){
	if( n != 3 || s == NULL)
		return false;
	for(size_t i = 0; i < n; i++){	
		for(size_t j = 0; j < MAX_MOD_FUNC; j++){
			if(strcmp(s->data[i]->clave, funciones[j].clave) == 0){
				f[i] = funciones[j].accion;
				break;
			}
		}
	}
	return true;
}

static tramo_t *_modular_intervalo(nota_t* nota, size_t i_inicio, double duracion, int f_m, synth_t *s, intervalo_t in, accion_t accion){
	double t0_intervalo = nota->inicio + (double)i_inicio / f_m;
	tramo_t *intervalo = tramo_crear_muestreo(t0_intervalo, t0_intervalo + duracion, f_m, nota->f, nota->a, s->armonicos, s->n);
	if(intervalo == NULL)
		return NULL;

	for(size_t i = 0; i < intervalo->n; i++)
		intervalo->v[i] *= accion((double)i/f_m, s->data[in]->instantes);
	return intervalo;	
}

static void _imprimir_parametros(params_t *p){
	printf("%s ", p->clave);
	printf("%.2f %.2f %.2f \n", p->instantes[0], p->instantes[1], p->instantes[2]);
}

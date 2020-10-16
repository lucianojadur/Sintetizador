//Sintetizador
//
TP final de Algoritmos y Programación I (95.11) cátedra Essaya - Facultad de Ingeniería (UBA).
Consiste en un programa que crea un archivo de sonido en formato WAVE (con especificaciones a detallar) a partir de un archivo MIDI ingresado por 
el stream de entrada. La síntesis de audio se genera para un único canal, a partir de todas las pistas grabadas sobre tal es decir, sintetiza sólo el sonido
de un instrumento, en caso de haber más de uno grabado en el MIDI. Se aconseja sintetizar a una frecuencia de muestreo de 44 kHz (valor por defecto).

El objetivo del trabajo es lograr un correcto diseño e implementación de los tipos de datos abstractos a utilizar y una modularización del programa de forma 
que facilite su mantenibilidad y que logre dividirlo en distintos niveles de abstracción según lo requiera.

Implementado en C (std = ISO C99), módulos compilados con Makefile.
//

EJECUCIÓN:

./sintetizador -s <sintetizador> -i <entrada> -o <salida> [-f <frecuencia_muestreo> -c <canal> -r <pps>]

<sintetizador> : archivo de texto con los parámetros correspondientes al timbre a emular

<entrada> : archivo MIDI (extensión .mid o .midi)

<salida> : archivo WAVE generado por el programa


[Argumentos opcionales]:

    <frecuencia_muestreo> : número natural. El valor por omision es 44 kHz 

    <canal> : canal del archivo MIDI a sintetizar (en gral es un canal por arreglo de instrumento). El valor por omisión es 0

    <pps> : pulsos por segundo a convertir. El valor por omisión es 130.

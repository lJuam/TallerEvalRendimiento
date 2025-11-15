/*#######################################################################################
#* Pontificia Universidad Javeriana
#* Fecha: Noviembre 2025
#* Autores: Juan David Garzon Ballen
#*          Juan Pablo Sanchez
#* Materia: Sistemas Operativos
#* Tema: Taller de Evaluación de Rendimiento
#* Fichero: mmCommon.h - Interfaz de funciones comunes
#* Descripción: Header que contiene las declaraciones de todas las funciones
#*              compartidas entre los diferentes algoritmos de multiplicación
#*              de matrices. Define la interfaz pública de la biblioteca.
######################################################################################*/

#ifndef MM_COMMON_H
#define MM_COMMON_H

#include <sys/time.h>

/**
 * InicioMuestra - Registra el tiempo de inicio de ejecución
 * 
 * Captura el tiempo actual del sistema usando gettimeofday() y lo almacena
 * en una variable global para posterior cálculo del tiempo transcurrido.
 */
void InicioMuestra();

/**
 * FinMuestra - Calcula y muestra el tiempo transcurrido en microsegundos
 * 
 * Calcula la diferencia entre el tiempo actual y el tiempo de inicio
 * registrado por InicioMuestra(). Imprime el resultado en microsegundos
 * en formato de 9 dígitos para facilitar el análisis estadístico.
 */
void FinMuestra();

/**
 * iniMatrix - Inicializa dos matrices con valores aleatorios
 * @m1: Puntero a la primera matriz (valores entre 1.0 y 5.0)
 * @m2: Puntero a la segunda matriz (valores entre 5.0 y 9.0)
 * @D: Dimensión de las matrices cuadradas (D x D)
 * 
 * Llena ambas matrices con números aleatorios en rangos distintos
 * para facilitar la verificación y depuración de los algoritmos.
 * Requiere que srand() haya sido llamado previamente.
 */
void iniMatrix(double *m1, double *m2, int D);

/**
 * impMatrix - Imprime una matriz en formato legible
 * @matrix: Puntero a la matriz a imprimir
 * @D: Dimensión de la matriz cuadrada (D x D)
 * 
 * Imprime la matriz solo si su dimensión es menor a 9x9 para evitar
 * saturar la salida estándar. Útil para debugging con matrices pequeñas.
 */
void impMatrix(double *matrix, int D);

/**
 * verificarMultiplicacion - Verifica la correctitud de la multiplicación
 * @mA: Puntero a la matriz A (multiplicando)
 * @mB: Puntero a la matriz B (multiplicador)
 * @mC: Puntero a la matriz resultado C (producto)
 * @D: Dimensión de las matrices cuadradas (D x D)
 * 
 * Realiza una verificación elemento por elemento del resultado de la
 * multiplicación C = A × B, comparando con el cálculo directo.
 * Usa un epsilon de 1e-6 para manejar errores de precisión de punto flotante.
 * 
 * @return: 1 si la multiplicación es correcta, 0 si hay errores
 */
int verificarMultiplicacion(double *mA, double *mB, double *mC, int D);

/**
 * multiMatrix - Realiza la multiplicación de matrices clásica para un rango de filas
 * @mA: Puntero a la matriz A (multiplicando)
 * @mB: Puntero a la matriz B (multiplicador)
 * @mC: Puntero a la matriz resultado C (producto)
 * @D: Dimensión de las matrices cuadradas (D x D)
 * @filaI: Fila inicial del rango a procesar (inclusiva)
 * @filaF: Fila final del rango a procesar (exclusiva)
 * 
 * Implementa el algoritmo clásico de multiplicación de matrices
 * C[i,j] = suma(A[i,k] × B[k,j]) para k=0..D-1
 * Optimizado con aritmética de punteros para mejor rendimiento.
 * Procesa solo el rango [filaI, filaF) para permitir paralelización.
 */
void multiMatrix(double *mA, double *mB, double *mC, int D, int filaI, int filaF);

/**
 * transposeMatrix - Crea la transpuesta de una matriz
 * @src: Puntero a la matriz fuente (NO se modifica)
 * @dst: Puntero a la matriz destino donde se guarda la transpuesta
 * @D: Dimensión de la matriz cuadrada (D x D)
 * 
 * Calcula dst[j,i] = src[i,j] para todos los elementos.
 * La matriz fuente permanece intacta, la transpuesta se almacena
 * en una matriz separada para mejorar la localidad de caché.
 */
void transposeMatrix(double *src, double *dst, int D);

/**
 * multiMatrixTrans - Multiplicación usando matriz transpuesta
 * @mA: Puntero a la matriz A (multiplicando)
 * @mBT: Puntero a la matriz B transpuesta (B^T)
 * @mC: Puntero a la matriz resultado C (producto)
 * @D: Dimensión de las matrices cuadradas (D x D)
 * 
 * Calcula C = A × B usando B^T para mejorar la localidad de caché.
 * En lugar de acceder a B por columnas (saltos de D posiciones),
 * accede a B^T por filas (acceso secuencial), lo que reduce
 * los fallos de caché y mejora significativamente el rendimiento.
 * 
 * Fórmula: C[i,j] = suma(A[i,k] × B^T[j,k]) para k=0..D-1
 */
void multiMatrixTrans(double *mA, double *mBT, double *mC, int D);

#endif /* MM_COMMON_H */

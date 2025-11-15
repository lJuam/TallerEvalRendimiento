/*#######################################################################################
#* Pontificia Universidad Javeriana
#* Fecha: Noviembre 2025
#* Autores: Juan David Garzon Ballen
#*          Juan Pablo Sanchez
#* Materia: Sistemas Operativos
#* Tema: Taller de Evaluación de Rendimiento
#* Fichero: mmCommon.c - Implementación de funciones comunes
#* Descripción: Biblioteca que implementa todas las funciones compartidas
#*              entre los diferentes algoritmos de multiplicación de matrices.
#*              Incluye inicialización, impresión, verificación, medición de
#*              tiempo y algoritmos de multiplicación básicos.
######################################################################################*/

#include "mmCommon.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

/* Variables globales para medición de tiempo */
static struct timeval inicio, fin;

/**
 * InicioMuestra - Registra el tiempo de inicio de ejecución
 */
void InicioMuestra() {
    gettimeofday(&inicio, (void *)0);
}

/**
 * FinMuestra - Calcula y muestra el tiempo transcurrido en microsegundos
 * 
 * Calcula la diferencia entre el tiempo actual y el registrado en InicioMuestra.
 * El resultado se imprime en microsegundos con formato de 9 dígitos.
 */
void FinMuestra() {
    gettimeofday(&fin, (void *)0);
    fin.tv_usec -= inicio.tv_usec;
    fin.tv_sec -= inicio.tv_sec;
    double tiempo = (double)(fin.tv_sec * 1000000 + fin.tv_usec);
    printf("%9.0f \n", tiempo);
}

/**
 * iniMatrix - Inicializa dos matrices con valores aleatorios
 * @m1: Primera matriz (valores entre 1.0 y 5.0)
 * @m2: Segunda matriz (valores entre 5.0 y 9.0)
 * @D: Dimensión de las matrices
 * 
 * Los rangos distintos facilitan la identificación de errores en pruebas.
 */
void iniMatrix(double *m1, double *m2, int D) {
    for (int i = 0; i < D * D; i++, m1++, m2++) {
        *m1 = (double)rand() / RAND_MAX * (5.0 - 1.0);
        *m2 = (double)rand() / RAND_MAX * (9.0 - 5.0);
    }
}

/**
 * impMatrix - Imprime una matriz en formato legible
 * @matrix: Matriz a imprimir
 * @D: Dimensión de la matriz
 * 
 * Solo imprime matrices menores a 9x9 para evitar saturar la salida.
 */
void impMatrix(double *matrix, int D) {
    if (D < 9) {
        printf("\nImpresión ...\n");
        for (int i = 0; i < D * D; i++, matrix++) {
            if (i % D == 0) printf("\n");
            printf(" %.2f ", *matrix);
        }
        printf("\n>-------------------->\n");
    }
}

/**
 * verificarMultiplicacion - Verifica la correctitud de la multiplicación
 * @mA: Matriz A
 * @mB: Matriz B
 * @mC: Matriz resultado C
 * @D: Dimensión de las matrices
 * 
 * Realiza verificación elemento por elemento comparando con cálculo directo.
 * Usa epsilon = 1e-6 para tolerar errores de redondeo de punto flotante.
 * 
 * @return: 1 si correcta, 0 si hay errores
 */
int verificarMultiplicacion(double *mA, double *mB, double *mC, int D) {
    double epsilon = 1e-6;
    int errores = 0;
    
    for (int i = 0; i < D; i++) {
        for (int j = 0; j < D; j++) {
            double suma = 0.0;
            for (int k = 0; k < D; k++) {
                suma += mA[i * D + k] * mB[k * D + j];
            }
            if (fabs(suma - mC[i * D + j]) > epsilon) {
                if (D < 9 && errores < 3) {
                    printf("Error en [%d][%d]: esperado=%.2f, obtenido=%.2f\n",
                           i, j, suma, mC[i * D + j]);
                }
                errores++;
            }
        }
    }
    return (errores == 0);
}

/**
 * multiMatrix - Multiplicación clásica de matrices para un rango de filas
 * @mA: Matriz A
 * @mB: Matriz B
 * @mC: Matriz resultado C
 * @D: Dimensión de las matrices
 * @filaI: Fila inicial (inclusiva)
 * @filaF: Fila final (exclusiva)
 * 
 * Implementa C[i,j] = suma(A[i,k] × B[k,j]) para k=0..D-1
 * Usa aritmética de punteros para optimizar el acceso a memoria.
 * Procesa solo filas en el rango [filaI, filaF) para paralelización.
 */
void multiMatrix(double *mA, double *mB, double *mC, int D, int filaI, int filaF) {
    double Suma, *pA, *pB;
    
    for (int i = filaI; i < filaF; i++) {
        for (int j = 0; j < D; j++) {
            Suma = 0.0;
            pA = mA + i * D;      // Apunta al inicio de la fila i de A
            pB = mB + j;          // Apunta a la columna j de B
            for (int k = 0; k < D; k++, pA++, pB += D) {
                Suma += *pA * *pB;
            }
            mC[i * D + j] = Suma;
        }
    }
}

/**
 * transposeMatrix - Crea la transpuesta de una matriz
 * @src: Matriz fuente (NO se modifica)
 * @dst: Matriz destino para la transpuesta
 * @D: Dimensión de la matriz
 * 
 * Calcula dst[j,i] = src[i,j] sin modificar la matriz original.
 * Mejora la localidad de caché en algoritmos de multiplicación.
 */
void transposeMatrix(double *src, double *dst, int D) {
    for (int i = 0; i < D; i++) {
        for (int j = 0; j < D; j++) {
            dst[j * D + i] = src[i * D + j];
        }
    }
}

/**
 * multiMatrixTrans - Multiplicación usando matriz transpuesta
 * @mA: Matriz A
 * @mBT: Matriz B transpuesta (B^T)
 * @mC: Matriz resultado C
 * @D: Dimensión de las matrices
 * 
 * Calcula C = A × B usando B^T para mejorar la localidad de caché.
 * En lugar de acceder a B por columnas (saltos de D posiciones),
 * accede a B^T por filas (acceso secuencial).
 * 
 * Fórmula: C[i,j] = suma(A[i,k] × B^T[j,k]) para k=0..D-1
 * 
 * Nota: Esta función NO usa OpenMP, debe paralelizarse externamente.
 */
void multiMatrixTrans(double *mA, double *mBT, double *mC, int D) {
    double Suma, *pA, *pB;
    
    for (int i = 0; i < D; i++) {
        for (int j = 0; j < D; j++) {
            pA = mA + (i * D);    // Apunta a la fila i de A
            pB = mBT + (j * D);   // Apunta a la fila j de B^T (= columna j de B)
            Suma = 0.0;
            for (int k = 0; k < D; k++, pA++, pB++) {
                Suma += (*pA) * (*pB);
            }
            mC[i * D + j] = Suma;
        }
    }
}

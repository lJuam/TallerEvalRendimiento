/*#######################################################################################
#* Pontificia Universidad Javeriana
#* Fecha: Noviembre 2025
#* Autores: Juan David Garzon Ballen
#*          Juan Pablo Sanchez
#* Materia: Sistemas Operativos
#* Tema: Taller de Evaluación de Rendimiento
#* Programa: Multiplicación de Matrices - Algoritmo Clásico con OpenMP
#* Descripción: Programa principal que implementa multiplicación de matrices
#*              usando OpenMP para paralelización automática de bucles.
#*              OpenMP usa directivas #pragma para distribuir iteraciones
#*              del bucle entre múltiples hilos de forma transparente.
#*              Utiliza la biblioteca mmCommon para funciones básicas.
#* Versión: Paralelismo con OpenMP - Algoritmo Clásico
######################################################################################*/

#include "mmCommon.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

/**
 * multiMatrixOpenMP - Multiplicación de matrices con paralelización OpenMP
 * @mA: Puntero a la matriz A (multiplicando)
 * @mB: Puntero a la matriz B (multiplicador)
 * @mC: Puntero a la matriz resultado C (producto)
 * @D: Dimensión de las matrices cuadradas (D x D)
 * 
 * Implementa el algoritmo clásico C[i,j] = suma(A[i,k] × B[k,j])
 * con paralelización automática del bucle externo mediante OpenMP.
 * 
 * La directiva #pragma omp parallel for distribuye las iteraciones
 * del bucle i entre los hilos disponibles. Variables privadas (Suma, pA, pB)
 * se crean independientemente para cada hilo para evitar condiciones de carrera.
 */
void multiMatrixOpenMP(double *mA, double *mB, double *mC, int D) {
    double Suma, *pA, *pB;

    /* Paralelización del bucle externo (filas)
     * - parallel: Crea equipo de hilos
     * - for: Distribuye iteraciones entre hilos
     * - private: Cada hilo tiene su propia copia de estas variables */
    #pragma omp parallel for private(Suma, pA, pB)
    for (int i = 0; i < D; i++) {
        for (int j = 0; j < D; j++) {
            pA = mA + i * D;      // Apunta al inicio de la fila i de A
            pB = mB + j;          // Apunta a la columna j de B
            Suma = 0.0;
            for (int k = 0; k < D; k++, pA++, pB += D) {
                Suma += *pA * *pB;
            }
            mC[i * D + j] = Suma;
        }
    }
}

/**
 * main - Programa principal para multiplicación con OpenMP clásico
 * @argc: Cantidad de argumentos de línea de comandos
 * @argv: Vector de argumentos [programa, tamaño_matriz, num_hilos]
 * 
 * Algoritmo:
 * 1. Valida argumentos de entrada
 * 2. Configura número de hilos OpenMP
 * 3. Asigna memoria para matrices A, B, C
 * 4. Inicializa matrices con valores aleatorios
 * 5. Ejecuta multiplicación con paralelización automática OpenMP
 * 6. Mide tiempo de ejecución y verifica correctitud
 * 7. Libera memoria
 * 
 * @return: 0 si éxito, 1 si error
 */
int main(int argc, char *argv[]) {
    /* Validación de argumentos */
    if (argc < 3) {
        printf("\n \t\tUse: $./mmClasicaOpenMP SIZE Hilos\n");
        printf("\t\tSIZE: Dimensión de la matriz cuadrada (NxN)\n");
        printf("\t\tHilos: Número de hilos OpenMP paralelos\n\n");
        exit(0);
    }

    int N = atoi(argv[1]);     // Dimensión de la matriz
    int TH = atoi(argv[2]);    // Número de hilos OpenMP

    /* Asignación de memoria dinámica para matrices (inicializada a cero) */
    double *matrixA = (double *)calloc(N * N, sizeof(double));
    double *matrixB = (double *)calloc(N * N, sizeof(double));
    double *matrixC = (double *)calloc(N * N, sizeof(double));

    if (!matrixA || !matrixB || !matrixC) {
        fprintf(stderr, "Error: No se pudo asignar memoria para matrices\n");
        exit(1);
    }

    /* Configuración de OpenMP */
    srand(time(NULL));
    omp_set_num_threads(TH);  // Establece el número de hilos para OpenMP

    /* Inicialización de matrices con valores aleatorios */
    iniMatrix(matrixA, matrixB, N);
    impMatrix(matrixA, N);
    impMatrix(matrixB, N);

    /* Medición de tiempo de ejecución de la multiplicación */
    InicioMuestra();
    multiMatrixOpenMP(matrixA, matrixB, matrixC, N);
    FinMuestra();

    /* Impresión del resultado (solo matrices pequeñas) */
    impMatrix(matrixC, N);

    /* Verificación de correctitud para matrices pequeñas */
    if (N < 9) {
        if (verificarMultiplicacion(matrixA, matrixB, matrixC, N)) {
            printf("\n[OK] Verificación: Multiplicación correcta\n");
        } else {
            printf("\n[ERROR] Verificación: Multiplicación incorrecta\n");
        }
    }

    /* Liberación de memoria */
    free(matrixA);
    free(matrixB);
    free(matrixC);

    return 0;
}

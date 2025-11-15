/*#######################################################################################
#* Pontificia Universidad Javeriana
#* Fecha: Noviembre 2025
#* Autores: Juan David Garzon Ballen
#*          Juan Pablo Sanchez
#* Materia: Sistemas Operativos
#* Tema: Taller de Evaluación de Rendimiento
#* Programa: Multiplicación de Matrices - Algoritmo Transpuesta con OpenMP
#* Descripción: Programa principal que implementa multiplicación de matrices
#*              usando la técnica de transpuesta para mejorar la localidad de caché.
#*              Crea B^T (transpuesta de B) y multiplica A × B^T, lo que permite
#*              acceso secuencial a memoria en lugar de saltos, reduciendo fallos
#*              de caché. Paraleliza con OpenMP. Usa biblioteca mmCommon.
#* Versión: Paralelismo con OpenMP - Algoritmo Transpuesta Optimizado
######################################################################################*/

#include "mmCommon.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

/**
 * impMatrixCustom - Imprime una matriz con opciones de formato personalizadas
 * @matrix: Puntero a la matriz a imprimir
 * @D: Dimensión de la matriz cuadrada
 * @t: Tipo de impresión (0=normal por filas, 1=por columnas)
 * 
 * Función específica para este programa que permite imprimir matrices
 * en diferentes formatos para verificar la transpuesta visualmente.
 * Solo imprime matrices menores a 6x6 para evitar saturar la salida.
 */
void impMatrixCustom(double *matrix, int D, int t) {
    int aux = 0;
    if (D < 6) {
        switch (t) {
            case 0:  // Impresión normal por filas
                for (int i = 0; i < D * D; i++) {
                    if (i % D == 0) printf("\n");
                    printf("%.2f ", matrix[i]);
                }
                printf("\n>-------------------->\n");
                break;
            case 1:  // Impresión por columnas (útil para verificar transpuesta)
                while (aux < D) {
                    for (int i = aux; i < D * D; i += D) {
                        printf("%.2f ", matrix[i]);
                    }
                    aux++;
                    printf("\n");
                }
                printf("\n>-------------------->\n");
                break;
            default:
                printf("Tipo de impresión no reconocido\n");
        }
    }
}

/**
 * multiMatrixTransOpenMP - Multiplicación usando transpuesta con OpenMP
 * @mA: Puntero a la matriz A (multiplicando)
 * @mBT: Puntero a la matriz B transpuesta (B^T)
 * @mC: Puntero a la matriz resultado C (producto)
 * @D: Dimensión de las matrices cuadradas (D x D)
 * 
 * Calcula C = A × B usando B^T para mejorar la localidad de caché.
 * Fórmula: C[i,j] = suma(A[i,k] × B^T[j,k]) para k=0..D-1
 * 
 * Ventaja: En lugar de acceder a B por columnas (saltos de D posiciones),
 * accede a B^T por filas (acceso secuencial), reduciendo fallos de caché.
 * 
 * Paraleliza el bucle externo con OpenMP para distribuir carga entre hilos.
 */
void multiMatrixTransOpenMP(double *mA, double *mBT, double *mC, int D) {
    double Suma, *pA, *pB;

    /* Paralelización con OpenMP
     * - parallel: Crea equipo de hilos
     * - for: Distribuye iteraciones entre hilos
     * - private: Variables privadas por hilo para evitar condiciones de carrera */
    #pragma omp parallel
    {
        #pragma omp for private(Suma, pA, pB)
        for (int i = 0; i < D; i++) {
            for (int j = 0; j < D; j++) {
                pA = mA + (i * D);    // Apunta a la fila i de A
                pB = mBT + (j * D);   // Apunta a la fila j de B^T (= columna j de B)
                Suma = 0.0;
                /* Producto punto: Ambos accesos son secuenciales */
                for (int k = 0; k < D; k++, pA++, pB++) {
                    Suma += (*pA) * (*pB);
                }
                mC[i * D + j] = Suma;
            }
        }
    }
}

/**
 * main - Programa principal para multiplicación con transpuesta y OpenMP
 * @argc: Cantidad de argumentos de línea de comandos
 * @argv: Vector de argumentos [programa, tamaño_matriz, num_hilos]
 * 
 * Algoritmo:
 * 1. Valida argumentos de entrada
 * 2. Configura número de hilos OpenMP
 * 3. Asigna memoria para matrices A, B, B^T, C
 * 4. Inicializa matrices A y B con valores aleatorios
 * 5. Calcula transpuesta de B (B^T) usando función de biblioteca
 * 6. Ejecuta multiplicación A × B^T con paralelización OpenMP
 * 7. Mide tiempo de ejecución y verifica correctitud usando B ORIGINAL
 * 8. Libera todas las matrices
 * 
 * @return: 0 si éxito, 1 si error
 */
int main(int argc, char *argv[]) {
    /* Validación de argumentos */
    if (argc < 3) {
        printf("\n \t\tUse: $./mmFilasOpenMP SIZE Hilos\n");
        printf("\t\tSIZE: Dimensión de la matriz cuadrada (NxN)\n");
        printf("\t\tHilos: Número de hilos OpenMP paralelos\n\n");
        exit(0);
    }

    int N = atoi(argv[1]);     // Dimensión de la matriz
    int TH = atoi(argv[2]);    // Número de hilos OpenMP

    /* Asignación de memoria para matrices (inicializada a cero)
     * Nota: matrixB_T es matriz adicional para almacenar la transpuesta */
    double *matrixA = (double *)calloc(N * N, sizeof(double));
    double *matrixB = (double *)calloc(N * N, sizeof(double));
    double *matrixB_T = (double *)calloc(N * N, sizeof(double));
    double *matrixC = (double *)calloc(N * N, sizeof(double));

    if (!matrixA || !matrixB || !matrixB_T || !matrixC) {
        fprintf(stderr, "Error: No se pudo asignar memoria para matrices\n");
        exit(1);
    }

    /* Configuración de OpenMP y generador aleatorio */
    srand(time(NULL));
    omp_set_num_threads(TH);

    /* Inicialización de matrices A y B con valores aleatorios */
    iniMatrix(matrixA, matrixB, N);
    impMatrixCustom(matrixA, N, 0);
    impMatrixCustom(matrixB, N, 0);

    /* Crear transpuesta de B sin modificar B original
     * Esta es la clave de la optimización: B^T permite acceso secuencial */
    transposeMatrix(matrixB, matrixB_T, N);

    /* Impresión de B^T para verificación visual (solo matrices pequeñas) */
    if (N < 6) {
        printf("\nMatriz B Transpuesta (B^T):\n");
        impMatrixCustom(matrixB_T, N, 0);
    }

    /* Medición de tiempo de ejecución de la multiplicación optimizada */
    InicioMuestra();
    multiMatrixTransOpenMP(matrixA, matrixB_T, matrixC, N);
    FinMuestra();

    /* Impresión del resultado */
    impMatrixCustom(matrixC, N, 0);

    /* Verificación de correctitud para matrices pequeñas
     * IMPORTANTE: Verifica usando B ORIGINAL, no B^T */
    if (N < 6) {
        if (verificarMultiplicacion(matrixA, matrixB, matrixC, N)) {
            printf("\n[OK] Verificación: Multiplicación correcta\n");
        } else {
            printf("\n[ERROR] Verificación: Multiplicación incorrecta\n");
        }
    }

    /* Liberación de memoria (incluye B^T adicional) */
    free(matrixA);
    free(matrixB);
    free(matrixB_T);
    free(matrixC);

    return 0;
}

/*#######################################################################################
#* Pontificia Universidad Javeriana
#* Fecha: Noviembre 2025
#* Autores: Juan David Garzon Ballen
#*          Juan Pablo Sanchez
#* Materia: Sistemas Operativos
#* Tema: Taller de Evaluación de Rendimiento
#* Programa: Multiplicación de Matrices - Algoritmo Clásico con Pthreads
#* Descripción: Programa principal que implementa multiplicación de matrices
#*              usando hilos POSIX (pthreads). Cada hilo calcula un subconjunto
#*              de filas de la matriz resultado. Las matrices son compartidas
#*              automáticamente entre hilos (mismo espacio de direcciones).
#*              Utiliza la biblioteca mmCommon para funciones básicas.
#* Versión: Paralelismo con Hilos Pthreads "POSIX"
######################################################################################*/

#include "mmCommon.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/* Variables globales compartidas entre todos los hilos */
pthread_mutex_t MM_mutex;
double *matrixA, *matrixB, *matrixC;

/**
 * struct parametros - Estructura para pasar parámetros a cada hilo
 * @nH: Número total de hilos en ejecución
 * @idH: Identificador único del hilo actual (0 a nH-1)
 * @N: Dimensión de la matriz cuadrada (NxN)
 * 
 * Esta estructura se pasa como argumento a pthread_create() para que
 * cada hilo sepa qué porción de la matriz debe procesar.
 */
struct parametros {
    int nH;   // Número total de hilos
    int idH;  // ID del hilo actual
    int N;    // Dimensión de la matriz
};

/**
 * multiMatrixThread - Función ejecutada por cada hilo para multiplicar matrices
 * @variables: Puntero a estructura de parámetros del hilo
 * 
 * Cada hilo calcula un subconjunto contiguo de filas de la matriz resultado C.
 * La distribución de trabajo se hace dividiendo N filas entre nH hilos.
 * 
 * Algoritmo:
 * 1. Extrae parámetros (ID hilo, total hilos, dimensión)
 * 2. Calcula rango de filas asignadas: [filaI, filaF)
 * 3. Ejecuta multiplicación clásica para ese rango
 * 4. Sincroniza con mutex (sin operación crítica, solo para demostración)
 * 
 * @return: NULL (requerido por pthread)
 */
void *multiMatrixThread(void *variables) {
    struct parametros *data = (struct parametros *)variables;

    int idH = data->idH;    // ID de este hilo
    int nH = data->nH;      // Total de hilos
    int D = data->N;        // Dimensión de matriz

    /* Cálculo del rango de filas que procesa este hilo */
    int filaI = (D / nH) * idH;           // Fila inicial (inclusiva)
    int filaF = (D / nH) * (idH + 1);     // Fila final (exclusiva)

    /* Llamada a la función de multiplicación de la biblioteca */
    multiMatrix(matrixA, matrixB, matrixC, D, filaI, filaF);

    /* Sincronización con mutex (placeholder para futura coordinación) */
    pthread_mutex_lock(&MM_mutex);
    /* Aquí podrían ir operaciones críticas como actualizar contadores */
    pthread_mutex_unlock(&MM_mutex);

    pthread_exit(NULL);
}

/**
 * main - Programa principal para multiplicación con Pthreads
 * @argc: Cantidad de argumentos de línea de comandos
 * @argv: Vector de argumentos [programa, tamaño_matriz, num_hilos]
 * 
 * Algoritmo:
 * 1. Valida argumentos de entrada
 * 2. Asigna memoria para matrices A, B, C
 * 3. Inicializa matrices con valores aleatorios
 * 4. Crea N hilos POSIX con pthread_create()
 * 5. Cada hilo calcula su rango de filas asignado
 * 6. Espera finalización de todos los hilos con pthread_join()
 * 7. Mide tiempo de ejecución y verifica correctitud
 * 8. Libera recursos (memoria, mutex, atributos)
 * 
 * @return: 0 si éxito, 1 si error
 */
int main(int argc, char *argv[]) {
    /* Validación de argumentos */
    if (argc < 3) {
        printf("\n \t\tUse: $./mmClasicaPosix tamMatriz numHilos\n");
        printf("\t\ttamMatriz: Dimensión de la matriz cuadrada (NxN)\n");
        printf("\t\tnumHilos: Número de hilos POSIX paralelos\n\n");
        exit(0);
    }

    int N = atoi(argv[1]);            // Dimensión de la matriz
    int n_threads = atoi(argv[2]);    // Número de hilos

    /* Arreglo de identificadores de hilos y atributos */
    pthread_t p[n_threads];
    pthread_attr_t atrMM;

    /* Asignación de memoria dinámica para matrices */
    matrixA = (double *)calloc(N * N, sizeof(double));
    matrixB = (double *)calloc(N * N, sizeof(double));
    matrixC = (double *)calloc(N * N, sizeof(double));

    if (!matrixA || !matrixB || !matrixC) {
        fprintf(stderr, "Error: No se pudo asignar memoria para matrices\n");
        exit(1);
    }

    /* Inicialización de matrices con valores aleatorios */
    srand(time(NULL));
    iniMatrix(matrixA, matrixB, N);
    impMatrix(matrixA, N);
    impMatrix(matrixB, N);

    /* Inicio de medición de tiempo */
    InicioMuestra();

    /* Inicialización de mutex y atributos de hilos */
    pthread_mutex_init(&MM_mutex, NULL);
    pthread_attr_init(&atrMM);
    pthread_attr_setdetachstate(&atrMM, PTHREAD_CREATE_JOINABLE);

    /* Creación de hilos */
    for (int j = 0; j < n_threads; j++) {
        /* Asignar memoria para parámetros de cada hilo */
        struct parametros *datos = (struct parametros *)malloc(sizeof(struct parametros));
        if (!datos) {
            fprintf(stderr, "Error: No se pudo asignar memoria para parámetros\n");
            exit(1);
        }

        datos->idH = j;             // ID del hilo
        datos->nH = n_threads;      // Total de hilos
        datos->N = N;               // Dimensión de matriz

        /* Crear hilo que ejecutará multiMatrixThread */
        if (pthread_create(&p[j], &atrMM, multiMatrixThread, (void *)datos) != 0) {
            fprintf(stderr, "Error: No se pudo crear hilo %d\n", j);
            exit(1);
        }
    }

    /* Espera de finalización de todos los hilos */
    for (int j = 0; j < n_threads; j++) {
        pthread_join(p[j], NULL);  // Bloquea hasta que el hilo j termine
    }

    /* Fin de medición de tiempo */
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

    /* Liberación de recursos */
    pthread_attr_destroy(&atrMM);
    pthread_mutex_destroy(&MM_mutex);
    free(matrixA);
    free(matrixB);
    free(matrixC);

    return 0;
}

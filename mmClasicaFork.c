/*#######################################################################################
#* Pontificia Universidad Javeriana
#* Fecha: Noviembre 2025
#* Autores: Juan David Garzon Ballen
#*          Juan Pablo Sanchez
#* Materia: Sistemas Operativos
#* Tema: Taller de Evaluación de Rendimiento
#* Programa: Multiplicación de Matrices - Algoritmo Clásico con Fork
#* Descripción: Programa principal que implementa multiplicación de matrices
#*              usando procesos fork() con memoria compartida (mmap).
#*              Cada proceso hijo calcula un subconjunto de filas de la matriz
#*              resultado. Utiliza la biblioteca mmCommon para funciones básicas.
#* Versión: Paralelismo con Procesos Fork y Memoria Compartida
######################################################################################*/

#include "mmCommon.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>

/**
 * main - Programa principal para multiplicación con Fork
 * @argc: Cantidad de argumentos de línea de comandos
 * @argv: Vector de argumentos [programa, tamaño_matriz, num_procesos]
 * 
 * Algoritmo:
 * 1. Valida argumentos de entrada
 * 2. Crea memoria compartida para las tres matrices (A, B, C)
 * 3. Inicializa matrices A y B con valores aleatorios
 * 4. Crea N procesos hijo mediante fork()
 * 5. Cada hijo calcula un rango de filas de la matriz resultado
 * 6. El padre espera a que todos los hijos terminen
 * 7. Mide tiempo de ejecución y verifica correctitud (matrices pequeñas)
 * 8. Libera memoria compartida
 * 
 * @return: 0 si éxito, 1 si error
 */
int main(int argc, char *argv[]) {
    /* Validación de argumentos */
    if (argc < 3) {
        printf("\n \t\tUse: $./mmClasicaFork Size Procesos \n");
        printf("\t\tSize: Dimensión de la matriz cuadrada (NxN)\n");
        printf("\t\tProcesos: Número de procesos paralelos\n\n");
        exit(0);
    }

    int N = (int)atoi(argv[1]);        // Dimensión de la matriz
    int num_P = (int)atoi(argv[2]);    // Número de procesos

    /* Creación de memoria compartida para las matrices
     * MAP_SHARED: Compartida entre procesos padre e hijos
     * MAP_ANONYMOUS: No respaldada por archivo, solo en RAM */
    double *matA = mmap(NULL, N * N * sizeof(double), PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    double *matB = mmap(NULL, N * N * sizeof(double), PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    double *matC = mmap(NULL, N * N * sizeof(double), PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (matA == MAP_FAILED || matB == MAP_FAILED || matC == MAP_FAILED) {
        perror("Error: mmap failed - No se pudo asignar memoria compartida");
        exit(1);
    }

    /* Inicialización de matrices con valores aleatorios */
    srand(time(0));
    iniMatrix(matA, matB, N);
    impMatrix(matA, N);
    impMatrix(matB, N);

    /* Cálculo de filas por proceso para distribución de carga */
    int rows_per_process = N / num_P;

    /* Inicio de medición de tiempo */
    InicioMuestra();

    /* Creación de procesos hijos para paralelizar el cálculo */
    for (int i = 0; i < num_P; i++) {
        pid_t pid = fork();

        if (pid == 0) {  
            /* CÓDIGO DEL PROCESO HIJO */
            int start_row = i * rows_per_process;
            int end_row = (i == num_P - 1) ? N : start_row + rows_per_process;

            /* Cada hijo calcula su rango de filas asignado */
            multiMatrix(matA, matB, matC, N, start_row, end_row);

            /* Debug: Información del proceso hijo (solo matrices pequeñas) */
            if (N < 9) {
                printf("\nProceso hijo PID %d calculó filas %d a %d\n",
                       getpid(), start_row, end_row - 1);
            }

            /* El hijo termina aquí para no crear más procesos */
            exit(0);
            
        } else if (pid < 0) {
            /* Error en fork() */
            perror("Error: fork failed - No se pudo crear proceso hijo");
            exit(1);
        }
        /* Si pid > 0, es el proceso padre, continúa el bucle */
    }

    /* CÓDIGO DEL PROCESO PADRE */
    /* Esperar a que todos los procesos hijos terminen */
    for (int i = 0; i < num_P; i++) {
        wait(NULL);  // Espera a cualquier hijo
    }

    /* Fin de medición de tiempo */
    FinMuestra();

    /* Impresión del resultado (solo matrices pequeñas) */
    impMatrix(matC, N);

    /* Verificación de correctitud para matrices pequeñas */
    if (N < 9) {
        if (verificarMultiplicacion(matA, matB, matC, N)) {
            printf("\n[OK] Verificación: Multiplicación correcta\n");
        } else {
            printf("\n[ERROR] Verificación: Multiplicación incorrecta\n");
        }
    }

    /* Liberación de memoria compartida */
    munmap(matA, N * N * sizeof(double));
    munmap(matB, N * N * sizeof(double));
    munmap(matC, N * N * sizeof(double));

    return 0;
}

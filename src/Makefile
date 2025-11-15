#######################################################################################
# Pontificia Universidad Javeriana
# Fecha: Noviembre 2025
# Autores: Juan David Garzon Ballen, Juan Pablo Sanchez
# Materia: Sistemas Operativos
# Tema: Taller de Evaluación de Rendimiento
# Fichero: Makefile para compilación modular
# Descripción: Makefile que compila los programas de multiplicación de matrices
#              utilizando estructura modular (biblioteca + interfaz + principal).
#              Compila primero la biblioteca de funciones comunes (mmCommon.o)
#              y luego enlaza con cada programa principal específico.
#######################################################################################

# Configuración del compilador y flags
GCC = gcc
CFLAGS = -lm -Wall -O2
FOPENMP = -fopenmp -O3
POSIX = -lpthread

# Archivos objeto y ejecutables
COMMON_OBJ = mmCommon.o
PROGRAMAS = mmClasicaFork mmClasicaPosix mmClasicaOpenMP mmFilasOpenMP

# Target principal: compila todos los programas
all: $(PROGRAMAS)

# Compilación de la biblioteca de funciones comunes
# Este archivo objeto se enlaza con todos los programas
mmCommon.o: mmCommon.c mmCommon.h
	@echo "==> Compilando biblioteca de funciones comunes..."
	$(GCC) -c mmCommon.c -o mmCommon.o $(CFLAGS)
	@echo "    [OK] mmCommon.o generado"

# Compilación del programa con Fork (requiere memoria compartida)
# Enlaza: mmClasicaFork.c + mmCommon.o
mmClasicaFork: mmClasicaFork.c $(COMMON_OBJ) mmCommon.h
	@echo "==> Compilando mmClasicaFork (paralelismo con procesos)..."
	$(GCC) mmClasicaFork.c $(COMMON_OBJ) -o mmClasicaFork $(CFLAGS)
	@echo "    [OK] Ejecutable mmClasicaFork generado"

# Compilación del programa con POSIX threads
# Enlaza: mmClasicaPosix.c + mmCommon.o + biblioteca pthread
mmClasicaPosix: mmClasicaPosix.c $(COMMON_OBJ) mmCommon.h
	@echo "==> Compilando mmClasicaPosix (paralelismo con pthreads)..."
	$(GCC) mmClasicaPosix.c $(COMMON_OBJ) -o mmClasicaPosix $(CFLAGS) $(POSIX)
	@echo "    [OK] Ejecutable mmClasicaPosix generado"

# Compilación del programa con OpenMP (algoritmo clásico)
# Enlaza: mmClasicaOpenMP.c + mmCommon.o + soporte OpenMP
mmClasicaOpenMP: mmClasicaOpenMP.c $(COMMON_OBJ) mmCommon.h
	@echo "==> Compilando mmClasicaOpenMP (paralelismo con OpenMP)..."
	$(GCC) mmClasicaOpenMP.c $(COMMON_OBJ) -o mmClasicaOpenMP $(CFLAGS) $(FOPENMP)
	@echo "    [OK] Ejecutable mmClasicaOpenMP generado"

# Compilación del programa con OpenMP (algoritmo transpuesta)
# Enlaza: mmFilasOpenMP.c + mmCommon.o + soporte OpenMP
mmFilasOpenMP: mmFilasOpenMP.c $(COMMON_OBJ) mmCommon.h
	@echo "==> Compilando mmFilasOpenMP (OpenMP con transpuesta)..."
	$(GCC) mmFilasOpenMP.c $(COMMON_OBJ) -o mmFilasOpenMP $(CFLAGS) $(FOPENMP)
	@echo "    [OK] Ejecutable mmFilasOpenMP generado"

# Target para crear el directorio de resultados
crear_directorio:
	@echo "==> Creando directorio de resultados..."
	@mkdir -p Resultados
	@echo "    [OK] Directorio Resultados/ creado"

# Limpieza de archivos generados
clean:
	@echo "==> Limpiando archivos generados..."
	$(RM) $(PROGRAMAS) $(COMMON_OBJ) *.dat
	@echo "    [OK] Ejecutables y archivos temporales eliminados"

# Limpieza completa (incluye resultados)
clean_all: clean
	@echo "==> Limpieza completa (incluye resultados)..."
	$(RM) -r Resultados/
	@echo "    [OK] Todo limpio"

# Target para pruebas básicas de funcionamiento
# Ejecuta cada programa con matriz 4x4 y 2 hilos/procesos
test: all
	@echo ""
	@echo "========================================"
	@echo "  PRUEBAS BÁSICAS DE FUNCIONAMIENTO"
	@echo "========================================"
	@echo ""
	@echo ">>> Prueba 1: mmClasicaFork (4x4, 2 procesos)"
	./mmClasicaFork 4 2
	@echo ""
	@echo ">>> Prueba 2: mmClasicaPosix (4x4, 2 hilos)"
	./mmClasicaPosix 4 2
	@echo ""
	@echo ">>> Prueba 3: mmClasicaOpenMP (4x4, 2 hilos)"
	./mmClasicaOpenMP 4 2
	@echo ""
	@echo ">>> Prueba 4: mmFilasOpenMP (4x4, 2 hilos)"
	./mmFilasOpenMP 4 2
	@echo ""
	@echo "========================================"
	@echo "  PRUEBAS COMPLETADAS"
	@echo "========================================"

# Target para preparar el entorno de experimentación
setup: all crear_directorio
	@echo ""
	@echo "========================================"
	@echo "  ENTORNO DE EXPERIMENTACIÓN LISTO"
	@echo "========================================"
	@echo "Ejecutables compilados:"
	@ls -lh $(PROGRAMAS) 2>/dev/null || echo "  [ERROR] Faltan ejecutables"
	@echo ""
	@echo "Directorio de resultados:"
	@ls -ld Resultados/ 2>/dev/null || echo "  [ERROR] No existe directorio"
	@echo ""
	@echo "Listo para ejecutar: ./lanzador.pl"
	@echo "========================================"

# Target para mostrar información del sistema
info:
	@echo ""
	@echo "========================================"
	@echo "  INFORMACIÓN DEL SISTEMA"
	@echo "========================================"
	@echo "Compilador: $(GCC)"
	@gcc --version | head -n 1
	@echo ""
	@echo "Procesador:"
	@lscpu | grep "Model name" || echo "  [No disponible]"
	@lscpu | grep "CPU(s):" | head -n 1 || echo "  [No disponible]"
	@echo ""
	@echo "Memoria:"
	@free -h | grep "Mem:" || echo "  [No disponible]"
	@echo "========================================"

# Dependencias explícitas para recompilación automática
# Si mmCommon.h cambia, recompilar todo
$(PROGRAMAS): mmCommon.h

# Ayuda sobre los targets disponibles
help:
	@echo ""
	@echo "========================================"
	@echo "  MAKEFILE - TALLER DE RENDIMIENTO"
	@echo "========================================"
	@echo "Targets disponibles:"
	@echo ""
	@echo "  make              - Compila todos los programas"
	@echo "  make all          - Igual que 'make'"
	@echo "  make test         - Compila y ejecuta pruebas básicas"
	@echo "  make setup        - Prepara entorno (compila + crea directorio)"
	@echo "  make clean        - Elimina ejecutables y .o"
	@echo "  make clean_all    - Limpieza completa (incluye resultados)"
	@echo "  make info         - Muestra información del sistema"
	@echo "  make help         - Muestra esta ayuda"
	@echo ""
	@echo "Programas compilados:"
	@echo "  mmClasicaFork     - Paralelismo con procesos fork()"
	@echo "  mmClasicaPosix    - Paralelismo con hilos POSIX"
	@echo "  mmClasicaOpenMP   - Paralelismo con OpenMP clásico"
	@echo "  mmFilasOpenMP     - Paralelismo con OpenMP + transpuesta"
	@echo ""
	@echo "Uso de programas:"
	@echo "  ./[programa] <tamaño_matriz> <num_hilos/procesos>"
	@echo "  Ejemplo: ./mmClasicaOpenMP 100 4"
	@echo "========================================"

#!/bin/bash
#######################################################################
# Script para ejecutar todas las pruebas del taller
# Autores: Juan David Garzon Ballen, Juan Pablo Sanchez
#######################################################################

echo "=========================================="
echo "Taller de Evaluación de Rendimiento"
echo "Ejecución Automatizada de Pruebas"
echo "=========================================="
echo ""

# Configuración
TAMANOS=(100 200 400 600 800 1000 1200 1400 1600)
HILOS=(1 2 4 6 8 10 12)
REPETICIONES=10
PROGRAMAS=("mmClasicaFork" "mmClasicaPosix" "mmClasicaOpenMP" "mmFilasOpenMP")

# Crear directorio de resultados si no existe
mkdir -p Resultados

# Contador de progreso
total_pruebas=$((${#PROGRAMAS[@]} * ${#TAMANOS[@]} * ${#HILOS[@]} * REPETICIONES))
contador=0

# Ejecutar pruebas para cada programa
for programa in "${PROGRAMAS[@]}"; do
    echo ""
    echo "=========================================="
    echo "Probando: $programa"
    echo "=========================================="
    
    for tamano in "${TAMANOS[@]}"; do
        for hilo in "${HILOS[@]}"; do
            archivo_salida="Resultados/${programa}-${tamano}-Hilos-${hilo}.dat"
            
            # Limpiar archivo si existe
            > "$archivo_salida"
            
            echo -n "  N=$tamano, Hilos=$hilo ... "
            
            for ((i=0; i<REPETICIONES; i++)); do
                ./$programa $tamano $hilo >> "$archivo_salida" 2>&1
                contador=$((contador + 1))
            done
            
            echo "OK (${contador}/${total_pruebas})"
        done
    done
done

echo ""
echo "=========================================="
echo "Pruebas Completadas"
echo "Total de ejecuciones: $contador"
echo "Resultados en: ./Resultados/"
echo "=========================================="

# Resumen de archivos generados
echo ""
echo "Archivos generados:"
ls -lh Resultados/ | wc -l
echo "archivos .dat creados"

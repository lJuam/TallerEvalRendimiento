[![C](https://img.shields.io/badge/C-00599C?logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Python](https://img.shields.io/badge/Python-3776AB?logo=python&logoColor=white)](https://www.python.org/)
[![OpenMP](https://img.shields.io/badge/OpenMP-Parallel-blue)](https://www.openmp.org/)

# Taller de Evaluación de Rendimiento

**Autores:** Juan David Garzon Ballen, Juan Pablo Sanchez
**Universidad:** Pontificia Universidad Javeriana
**Materia:** Sistemas Operativos
**Fecha:** Noviembre 2025

---

## Descripción

Proyecto de evaluación de rendimiento que compara diferentes implementaciones de multiplicación de matrices utilizando:
- **Fork**: Procesos con memoria compartida
- **POSIX Threads**: Hilos con pthread
- **OpenMP Clásico**: Paralelización automática de bucles
- **OpenMP Transpuesta**: Optimización con transpuesta de matriz

---

## Estructura del Proyecto

```
Entrega
│
├── mmCommon.h                         # Interfaz
├── mmCommon.c                         # Biblioteca
├── mmClasicaFork.c                    # Principal Fork
├── mmClasicaPosix.c                   # Principal Pthreads
├── mmClasicaOpenMP.c                  # Principal OpenMP
├── mmFilasOpenMP.c                    # Principal Transpuesta
├── Makefile                           # Compilación
├── lanzador.pl                        # Script Perl (documentado)
├── ejecutar_todas_pruebas.sh          # Script Bash
├── analizar_resultados.py             # Script Python
├── README.md                          # Instrucciones
└── Informe_Taller_Rendimiento.pdf     # Documento final
```

---

## Compilación

### Requisitos
- GCC con soporte OpenMP
- Python 3.x (pandas, matplotlib, numpy)
- Perl (opcional, si usa lanzador.pl)

### Compilar todos los programas
```bash
make clean
make all
```

### Verificar compilación
```bash
make test
```

---

## Ejecución

### Ejecución manual (pruebas rápidas)
```bash
./mmClasicaFork 100 4
./mmClasicaPosix 100 4
./mmClasicaOpenMP 100 4
./mmFilasOpenMP 100 4
```

Argumentos:
- `arg1`: Tamaño de matriz (NxN)
- `arg2`: Número de hilos/procesos

### Batería automatizada (Bash)
```bash
chmod +x ejecutar_todas_pruebas.sh
./ejecutar_todas_pruebas.sh
```

Genera archivos `.dat` en `Resultados/`

### Batería automatizada (Perl)
```bash
chmod +x lanzador.pl
./lanzador.pl
```

---

## Análisis de Resultados

### Procesar datos y generar gráficas
```bash
python3 analizar_resultados.py
```

### Salidas generadas
- `resultados_procesados.csv`: Datos con estadísticas
- `grafica_*.png`: Gráficas de tiempo, speedup, eficiencia
- `tablas_resultados.tex`: Tablas para LaTeX
- `resumen_resultados.md`: Resumen ejecutivo

---

## Métricas Calculadas

- **Tiempo promedio** de ejecución (μs)
- **Speedup**: T(1) / T(N)
- **Eficiencia**: Speedup(N) / N × 100%
- **Desviación estándar** de tiempos

---

## Configuración de Experimentos

### Tamaños de matriz probados
100, 200, 400, 600, 800, 1000, 1200, 1400, 1600

### Configuraciones de hilos/procesos
1, 2, 4, 6, 8, 10, 12

### Repeticiones por configuración
30 (recomendado para análisis estadístico confiable)

---

## Documentación del Código

### Estructura modular
- **mmCommon.h**: Interfaz pública de funciones
- **mmCommon.c**: Implementación de:
  - `iniMatrix()`: Inicialización de matrices
  - `multiMatrix()`: Multiplicación clásica
  - `transposeMatrix()`: Cálculo de transpuesta
  - `verificarMultiplicacion()`: Validación de resultados
  - `InicioMuestra()` / `FinMuestra()`: Medición de tiempo

### Programas principales
Cada programa contiene:
- Validación de argumentos
- Asignación de memoria
- Lógica de paralelización específica
- Medición de rendimiento
- Verificación de correctitud

---

## Targets del Makefile

```bash
make              # Compila todos los programas
make test         # Pruebas básicas (4x4, 2 hilos)
make setup        # Prepara entorno (compila + crea directorios)
make clean        # Elimina ejecutables y .o
make clean_all    # Limpieza completa (incluye resultados)
make info         # Información del sistema
make help         # Ayuda de targets disponibles
```

---

## Verificación de Correctitud

Los programas incluyen verificación automática para matrices pequeñas:
```bash
./mmClasicaOpenMP 4 2
# Salida incluirá: [OK] Verificación: Multiplicación correcta
```

---

## Troubleshooting

### Error: "No se encuentra el ejecutable"
```bash
make clean
make all
```

### Error: "Permission denied"
```bash
chmod +x ejecutar_todas_pruebas.sh
chmod +x lanzador.pl
```

### Error: "ModuleNotFoundError" (Python)
```bash
pip install pandas matplotlib numpy
```

### Error: OpenMP no disponible
Verificar instalación de GCC con soporte OpenMP:
```bash
gcc --version
# Debe incluir soporte para -fopenmp
```

---

## Contacto

**Autores:**
- Juan David Garzon Ballen: jd.garzonb@javeriana.edu.co
- Juan Pablo Sanchez: sanchez.jp@javeriana.edu.co

**Institución:** Pontificia Universidad Javeriana
**Materia:** Sistemas Operativos
**Fecha:** Noviembre 2025

---


#!/usr/bin/perl
#**************************************************************
#         	Pontificia Universidad Javeriana
#     Autores: Juan David Garzon Ballen, Juan Pablo Sanchez
#     Fecha: Noviembre 2025
#     Materia: Sistemas Operativos
#     Tema: Taller de Evaluación de Rendimiento
#     Fichero: lanzador.pl - Script de automatización mejorado
#     Descripción: Script automatizado para ejecutar baterías de
#                  experimentos de multiplicación de matrices con
#                  diferentes configuraciones de tamaño y paralelismo.
#                  
#     Funcionalidad:
#     - Ejecuta múltiples repeticiones de cada configuración
#     - Genera archivos .dat con tiempos de ejecución
#     - Organiza resultados en directorio específico
#     - Permite fácil análisis estadístico posterior
#     - Muestra progreso y resumen de ejecución
#     
#     Uso: ./lanzador.pl
#     
#     Formato de salida:
#     - Archivos: Resultados/[ejecutable]-[size]-Hilos-[num].dat
#     - Cada línea del archivo contiene el tiempo en microsegundos
#     - Los archivos pueden importarse a Excel, LibreOffice, etc.
#****************************************************************/

use strict;
use warnings;

#==========================================
# CONFIGURACIÓN DE PARÁMETROS DE PRUEBA
#==========================================

# Obtiene el directorio actual de trabajo
my $Path = `pwd`;
chomp($Path);

# Nombre del ejecutable a probar (cambiar según el programa)
# Opciones: mmClasicaFork, mmClasicaPosix, mmClasicaOpenMP, mmFilasOpenMP
my $Nombre_Ejecutable = "mmClasicaFork"; # <-- CAMBIAR POR EL NOMBRE DEL EJECUTABLE A PROBAR

# Tamaños de matrices a probar (dimensión NxN)
# Recomendación: Valores potencia de 2 o múltiplos de 100
# Ajustar según capacidad del sistema y tiempo disponible
my @Size_Matriz = (100, 200, 400, 600, 800, 1000, 1200, 1400, 1600);

# Número de hilos/procesos a probar
# Recomendación: Incluir 1 (serie) y valores cercanos al número de cores
# Ejemplo para 8 cores: (1, 2, 4, 6, 8, 10, 12)
my @Num_Hilos = (1, 2, 4, 6, 8, 10, 12);

# Número de repeticiones por configuración
# Recomendación: Mínimo 30 para análisis estadístico confiable
# Más repeticiones = mejor estimación del promedio pero más tiempo
my $Repeticiones = 10;

#==========================================
# VERIFICACIONES PREVIAS
#==========================================

# Verificar que el ejecutable existe
unless (-e "$Path/$Nombre_Ejecutable") {
    print "[ERROR] No se encuentra el ejecutable: $Nombre_Ejecutable\n";
    print "        Verifique que el programa esté compilado.\n";
    print "        Ejecute: make all\n";
    exit(1);
}

# Verificar que el ejecutable tiene permisos de ejecución
unless (-x "$Path/$Nombre_Ejecutable") {
    print "[ERROR] El ejecutable no tiene permisos de ejecución\n";
    print "        Ejecute: chmod +x $Nombre_Ejecutable\n";
    exit(1);
}

# Crear directorio de resultados si no existe
my $dir_resultados = "$Path/Resultados";
unless (-d $dir_resultados) {
    mkdir($dir_resultados) or die "[ERROR] No se pudo crear directorio Resultados: $!\n";
    print "[OK] Directorio Resultados/ creado\n\n";
}

#==========================================
# ENCABEZADO DEL SCRIPT
#==========================================

print "="x60 . "\n";
print "  SCRIPT DE EVALUACIÓN DE RENDIMIENTO - BATERÍA AUTOMATIZADA\n";
print "="x60 . "\n";
print "Autores: Juan David Garzon Ballen\n";
print "         Juan Pablo Sanchez\n";
print "Universidad: Pontificia Universidad Javeriana\n";
print "="x60 . "\n\n";

print "Configuración de la batería de experimentos:\n";
print "-" x 60 . "\n";
print "Ejecutable       : $Nombre_Ejecutable\n";
print "Tamaños matriz   : " . join(", ", @Size_Matriz) . "\n";
print "Número de hilos  : " . join(", ", @Num_Hilos) . "\n";
print "Repeticiones     : $Repeticiones por configuración\n";
print "Directorio salida: $dir_resultados/\n";
print "-" x 60 . "\n\n";

# Cálculo de estimación de tiempo
my $total_configuraciones = scalar(@Size_Matriz) * scalar(@Num_Hilos);
my $total_ejecuciones = $total_configuraciones * $Repeticiones;

print "Estadísticas de la experimentación:\n";
print "  - Configuraciones a probar  : $total_configuraciones\n";
print "  - Ejecuciones totales       : $total_ejecuciones\n";
print "\n";

#==========================================
# EJECUCIÓN DE LA BATERÍA DE EXPERIMENTOS
#==========================================

print "Iniciando batería de experimentos...\n";
print "="x60 . "\n\n";

my $contador_config = 0;
my $tiempo_inicio_global = time();

# Bucle principal: Recorre todos los tamaños de matriz
foreach my $size (@Size_Matriz) {
    
    # Bucle secundario: Recorre todos los números de hilos
    foreach my $hilo (@Num_Hilos) {
        
        $contador_config++;
        
        # Construcción del nombre del archivo de salida
        # Formato: ejecutable-SIZE-Hilos-NUM.dat
        my $archivo_salida = "$dir_resultados/$Nombre_Ejecutable-$size-Hilos-$hilo.dat";
        
        # Mostrar progreso de la configuración actual
        my $progreso = sprintf("%.1f", ($contador_config / $total_configuraciones) * 100);
        print "[$contador_config/$total_configuraciones] ($progreso%) ";
        print "Ejecutando: Matriz=${size}x${size}, Hilos=$hilo\n";
        
        # Eliminar archivo previo si existe (para evitar acumulación de datos)
        unlink($archivo_salida) if (-e $archivo_salida);
        
        # Bucle de repeticiones: Ejecuta el programa N veces
        for (my $i = 0; $i < $Repeticiones; $i++) {
            
            # Comando de ejecución del programa
            # El operador >> redirige la salida (tiempo) al archivo .dat
            my $comando = "$Path/$Nombre_Ejecutable $size $hilo >> $archivo_salida 2>&1";
            
            # Ejecutar el comando
            my $resultado = system($comando);
            
            # Verificar si la ejecución fue exitosa
            if ($resultado != 0) {
                print "  [ADVERTENCIA] Error en repetición " . ($i+1) . "\n";
                print "                Código de salida: $resultado\n";
            }
            
            # Mostrar punto de progreso cada 5 repeticiones (feedback visual)
            print "." if (($i + 1) % 5 == 0);
        }
        
        print " OK\n";
        
        # Verificar que el archivo de salida se creó correctamente
        if (-e $archivo_salida) {
            my $num_lineas = `wc -l < $archivo_salida`;
            chomp($num_lineas);
            print "  → Resultados guardados: $archivo_salida ($num_lineas mediciones)\n";
        } else {
            print "  [ERROR] No se generó el archivo de resultados\n";
        }
        
        print "\n";
    }
}

#==========================================
# RESUMEN FINAL DE EJECUCIÓN
#==========================================

my $tiempo_fin_global = time();
my $tiempo_total_segundos = $tiempo_fin_global - $tiempo_inicio_global;
my $tiempo_minutos = int($tiempo_total_segundos / 60);
my $tiempo_segundos = $tiempo_total_segundos % 60;

print "="x60 . "\n";
print "  BATERÍA DE EXPERIMENTOS COMPLETADA\n";
print "="x60 . "\n";
print "Total de configuraciones probadas : $total_configuraciones\n";
print "Total de ejecuciones realizadas   : $total_ejecuciones\n";
print "Tiempo total de ejecución         : ${tiempo_minutos}m ${tiempo_segundos}s\n";
print "Archivos de resultados generados  : $dir_resultados/\n";
print "="x60 . "\n\n";

print "Próximos pasos:\n";
print "1. Importar archivos .dat a hoja de cálculo (Excel, LibreOffice)\n";
print "2. Calcular estadísticas: promedio, desviación estándar, etc.\n";
print "3. Generar gráficas comparativas de rendimiento\n";
print "4. Analizar speedup y eficiencia de paralelización\n";
print "\n";

print "¡Evaluación de rendimiento completada con éxito!\n";

#==========================================
# NOTAS PARA ANÁLISIS POSTERIOR
#==========================================

# FORMATO DE ARCHIVOS .dat:
# - Cada línea contiene un tiempo de ejecución en microsegundos
# - Importar como CSV con delimitador de nueva línea
# - Calcular promedio: AVG() en Excel
# - Calcular desviación: STDEV() en Excel
# - Eliminar outliers si es necesario (valores atípicos)

# MÉTRICAS RECOMENDADAS PARA EL INFORME:
# - Tiempo promedio de ejecución vs tamaño de matriz
# - Speedup: T(1)/T(N) donde N es número de hilos
# - Eficiencia: Speedup(N) / N
# - Comparación entre algoritmos (clásico vs transpuesta)
# - Comparación entre modelos (Fork vs Pthreads vs OpenMP)

exit(0);

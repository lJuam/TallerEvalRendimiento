#!/usr/bin/env python3
"""
Script de Análisis de Resultados - Taller de Evaluación de Rendimiento
Autores: Juan David Garzon Ballen, Juan Pablo Sanchez
Pontificia Universidad Javeriana - Sistemas Operativos
"""

import os
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

# Configuración de estilo de gráficas
plt.style.use('seaborn-v0_8-darkgrid')
plt.rcParams['figure.figsize'] = (12, 7)
plt.rcParams['font.size'] = 11

def procesar_resultados(directorio_resultados='Resultados'):
    """
    Procesa los archivos .dat y genera estadísticas
    """
    resultados = []
    patron = r'(\w+)-(\d+)-Hilos-(\d+)\.dat'
    
    archivos_procesados = 0
    archivos_con_error = 0
    
    for archivo in os.listdir(directorio_resultados):
        if archivo.endswith('.dat'):
            match = re.match(patron, archivo)
            if match:
                programa = match.group(1)
                tamano = int(match.group(2))
                hilos = int(match.group(3))
                
                ruta_completa = os.path.join(directorio_resultados, archivo)
                with open(ruta_completa, 'r') as f:
                    tiempos = []
                    for linea in f:
                        try:
                            tiempo = float(linea.strip())
                            # Filtrar outliers extremos (> 1 hora en microsegundos)
                            if tiempo < 3600000000:
                                tiempos.append(tiempo)
                        except:
                            continue
                    
                    if tiempos:
                        resultados.append({
                            'Programa': programa,
                            'Tamano': tamano,
                            'Hilos': hilos,
                            'Tiempo_Promedio': np.mean(tiempos),
                            'Tiempo_Min': np.min(tiempos),
                            'Tiempo_Max': np.max(tiempos),
                            'Desv_Std': np.std(tiempos),
                            'Repeticiones': len(tiempos)
                        })
                        archivos_procesados += 1
                    else:
                        archivos_con_error += 1
    
    print(f"Archivos procesados: {archivos_procesados}")
    print(f"Archivos con error: {archivos_con_error}")
    
    return pd.DataFrame(resultados)

def calcular_speedup(df, programa, tamano):
    """
    Calcula el speedup y eficiencia para un programa y tamaño específicos
    """
    datos = df[(df['Programa'] == programa) & (df['Tamano'] == tamano)]
    if len(datos) == 0:
        return None
    
    datos = datos.sort_values('Hilos').copy()
    
    # Buscar tiempo secuencial (1 hilo)
    tiempo_seq = datos[datos['Hilos'] == 1]['Tiempo_Promedio']
    if len(tiempo_seq) == 0:
        return None
    
    tiempo_secuencial = tiempo_seq.values[0]
    
    datos['Speedup'] = tiempo_secuencial / datos['Tiempo_Promedio']
    datos['Eficiencia'] = (datos['Speedup'] / datos['Hilos']) * 100
    
    return datos

def generar_graficas_tiempo(df):
    """
    Genera gráficas de tiempo de ejecución
    """
    programas = sorted(df['Programa'].unique())
    tamanos = sorted(df['Tamano'].unique())
    
    # Seleccionar tamaños representativos
    tamanos_graf = [t for t in [400, 800, 1200, 1600] if t in tamanos]
    
    for tamano in tamanos_graf:
        fig, ax = plt.subplots(figsize=(12, 7))
        
        for programa in programas:
            datos = df[(df['Programa'] == programa) & (df['Tamano'] == tamano)]
            if len(datos) > 0:
                datos = datos.sort_values('Hilos')
                ax.plot(datos['Hilos'], datos['Tiempo_Promedio']/1000000, 
                       marker='o', label=programa, linewidth=2.5, markersize=8)
        
        ax.set_xlabel('Número de Hilos/Procesos', fontsize=13, fontweight='bold')
        ax.set_ylabel('Tiempo de Ejecución (segundos)', fontsize=13, fontweight='bold')
        ax.set_title(f'Tiempo de Ejecución vs Hilos (Matriz {tamano}×{tamano})', 
                    fontsize=15, fontweight='bold', pad=20)
        ax.legend(loc='best', fontsize=11, framealpha=0.9)
        ax.grid(True, alpha=0.3, linestyle='--')
        ax.set_xticks(sorted(df['Hilos'].unique()))
        
        plt.tight_layout()
        plt.savefig(f'grafica_tiempo_{tamano}.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"✓ Generada: grafica_tiempo_{tamano}.png")

def generar_graficas_speedup(df):
    """
    Genera gráficas de speedup
    """
    programas = sorted(df['Programa'].unique())
    tamanos = sorted(df['Tamano'].unique())
    
    tamanos_graf = [t for t in [400, 800, 1200, 1600] if t in tamanos]
    
    for tamano in tamanos_graf:
        fig, ax = plt.subplots(figsize=(12, 7))
        
        for programa in programas:
            datos_speedup = calcular_speedup(df, programa, tamano)
            if datos_speedup is not None and len(datos_speedup) > 0:
                ax.plot(datos_speedup['Hilos'], datos_speedup['Speedup'], 
                       marker='s', label=programa, linewidth=2.5, markersize=8)
        
        # Línea ideal
        max_hilos = df['Hilos'].max()
        ax.plot([1, max_hilos], [1, max_hilos], '--', 
               color='red', label='Speedup Ideal', linewidth=2, alpha=0.7)
        
        ax.set_xlabel('Número de Hilos/Procesos', fontsize=13, fontweight='bold')
        ax.set_ylabel('Speedup', fontsize=13, fontweight='bold')
        ax.set_title(f'Speedup vs Hilos (Matriz {tamano}×{tamano})', 
                    fontsize=15, fontweight='bold', pad=20)
        ax.legend(loc='best', fontsize=11, framealpha=0.9)
        ax.grid(True, alpha=0.3, linestyle='--')
        ax.set_xticks(sorted(df['Hilos'].unique()))
        
        plt.tight_layout()
        plt.savefig(f'grafica_speedup_{tamano}.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"✓ Generada: grafica_speedup_{tamano}.png")

def generar_grafica_eficiencia(df):
    """
    Genera gráfica comparativa de eficiencia
    """
    programas = sorted(df['Programa'].unique())
    tamanos = sorted(df['Tamano'].unique())
    
    fig, ax = plt.subplots(figsize=(14, 8))
    
    tamanos_graf = [t for t in [400, 800, 1200] if t in tamanos]
    
    markers = ['o', 's', '^', 'D']
    
    for idx, programa in enumerate(programas):
        for tamano in tamanos_graf:
            datos_speedup = calcular_speedup(df, programa, tamano)
            if datos_speedup is not None and len(datos_speedup) > 0:
                label = f'{programa} (N={tamano})'
                ax.plot(datos_speedup['Hilos'], datos_speedup['Eficiencia'], 
                       marker=markers[idx % len(markers)], 
                       label=label, linewidth=2, markersize=7)
    
    ax.set_xlabel('Número de Hilos/Procesos', fontsize=13, fontweight='bold')
    ax.set_ylabel('Eficiencia (%)', fontsize=13, fontweight='bold')
    ax.set_title('Eficiencia de Paralelización', fontsize=15, fontweight='bold', pad=20)
    ax.legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=10)
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.set_xticks(sorted(df['Hilos'].unique()))
    ax.axhline(y=100, color='red', linestyle='--', alpha=0.5, label='100% Eficiencia')
    
    plt.tight_layout()
    plt.savefig('grafica_eficiencia.png', dpi=300, bbox_inches='tight')
    plt.close()
    print(f"✓ Generada: grafica_eficiencia.png")

def generar_grafica_comparativa_general(df):
    """
    Genera gráfica comparativa para N=1200
    """
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))
    
    tamano = 1200
    programas = sorted(df['Programa'].unique())
    
    # Subplot 1: Tiempo
    for programa in programas:
        datos = df[(df['Programa'] == programa) & (df['Tamano'] == tamano)]
        if len(datos) > 0:
            datos = datos.sort_values('Hilos')
            ax1.plot(datos['Hilos'], datos['Tiempo_Promedio']/1000000, 
                    marker='o', label=programa, linewidth=2.5, markersize=8)
    
    ax1.set_xlabel('Número de Hilos/Procesos', fontsize=12, fontweight='bold')
    ax1.set_ylabel('Tiempo (segundos)', fontsize=12, fontweight='bold')
    ax1.set_title(f'Tiempo de Ejecución (N={tamano})', fontsize=13, fontweight='bold')
    ax1.legend(loc='best')
    ax1.grid(True, alpha=0.3)
    ax1.set_xticks(sorted(df['Hilos'].unique()))
    
    # Subplot 2: Speedup
    for programa in programas:
        datos_speedup = calcular_speedup(df, programa, tamano)
        if datos_speedup is not None and len(datos_speedup) > 0:
            ax2.plot(datos_speedup['Hilos'], datos_speedup['Speedup'], 
                    marker='s', label=programa, linewidth=2.5, markersize=8)
    
    max_hilos = df['Hilos'].max()
    ax2.plot([1, max_hilos], [1, max_hilos], '--', 
            color='red', label='Ideal', linewidth=2, alpha=0.7)
    
    ax2.set_xlabel('Número de Hilos/Procesos', fontsize=12, fontweight='bold')
    ax2.set_ylabel('Speedup', fontsize=12, fontweight='bold')
    ax2.set_title(f'Speedup (N={tamano})', fontsize=13, fontweight='bold')
    ax2.legend(loc='best')
    ax2.grid(True, alpha=0.3)
    ax2.set_xticks(sorted(df['Hilos'].unique()))
    
    plt.tight_layout()
    plt.savefig('grafica_comparativa_general.png', dpi=300, bbox_inches='tight')
    plt.close()
    print(f"✓ Generada: grafica_comparativa_general.png")

def generar_tablas_latex(df):
    """
    Genera tablas en formato LaTeX
    """
    with open('tablas_resultados.tex', 'w', encoding='utf-8') as f:
        programas = sorted(df['Programa'].unique())
        
        for programa in programas:
            datos_programa = df[df['Programa'] == programa]
            
            f.write(f"\n% Tabla para {programa}\n")
            f.write("\\begin{table}[H]\n")
            f.write("\\centering\n")
            f.write("\\caption{Resultados de " + programa.replace('_', '\\_') + "}\n")
            f.write("\\label{tab:" + programa.lower() + "}\n")
            f.write("\\begin{tabular}{|c|c|r|r|r|}\n")
            f.write("\\hline\n")
            f.write("\\textbf{Tamaño} & \\textbf{Hilos} & \\textbf{Tiempo (s)} & \\textbf{Speedup} & \\textbf{Eficiencia (\\%)} \\\\\n")
            f.write("\\hline\n")
            
            tamanos_importantes = [400, 800, 1200, 1600]
            
            for tamano in tamanos_importantes:
                if tamano not in datos_programa['Tamano'].values:
                    continue
                    
                datos_speedup = calcular_speedup(df, programa, tamano)
                
                if datos_speedup is not None:
                    f.write("\\hline\n")
                    for _, row in datos_speedup.iterrows():
                        tiempo_seg = row['Tiempo_Promedio'] / 1000000
                        f.write(f"{int(row['Tamano'])} & {int(row['Hilos'])} & ")
                        f.write(f"{tiempo_seg:.3f} & ")
                        f.write(f"{row['Speedup']:.2f} & ")
                        f.write(f"{row['Eficiencia']:.1f} \\\\\n")
            
            f.write("\\hline\n")
            f.write("\\end{tabular}\n")
            f.write("\\end{table}\n\n")
    
    print(f"✓ Generadas tablas LaTeX: tablas_resultados.tex")

def generar_resumen_ejecutivo(df):
    """
    Genera resumen ejecutivo en Markdown
    """
    with open('resumen_resultados.md', 'w', encoding='utf-8') as f:
        f.write("# Resumen de Resultados\n\n")
        f.write("## Taller de Evaluación de Rendimiento\n")
        f.write("**Autores:** Juan David Garzon Ballen, Juan Pablo Sanchez\n\n")
        f.write("---\n\n")
        
        programas = sorted(df['Programa'].unique())
        
        f.write("## Mejores Configuraciones\n\n")
        
        for tamano in [400, 800, 1200, 1600]:
            if tamano not in df['Tamano'].values:
                continue
            
            f.write(f"### Matriz {tamano}×{tamano}\n\n")
            
            for programa in programas:
                datos = df[(df['Programa'] == programa) & (df['Tamano'] == tamano)]
                if len(datos) > 0:
                    mejor = datos.loc[datos['Tiempo_Promedio'].idxmin()]
                    f.write(f"**{programa}:**\n")
                    f.write(f"- Mejor configuración: {int(mejor['Hilos'])} hilos\n")
                    f.write(f"- Tiempo: {mejor['Tiempo_Promedio']/1000000:.3f} segundos\n\n")
        
        f.write("\n## Análisis Comparativo\n\n")
        
        # Programa más rápido por tamaño
        f.write("### Programa Más Rápido por Tamaño\n\n")
        for tamano in [400, 800, 1200, 1600]:
            if tamano not in df['Tamano'].values:
                continue
            datos_tam = df[df['Tamano'] == tamano]
            mejor = datos_tam.loc[datos_tam['Tiempo_Promedio'].idxmin()]
            f.write(f"- **N={tamano}:** {mejor['Programa']} con {int(mejor['Hilos'])} hilos ")
            f.write(f"({mejor['Tiempo_Promedio']/1000000:.3f}s)\n")
    
    print(f"✓ Generado resumen: resumen_resultados.md")

def main():
    print("="*70)
    print("Análisis de Resultados - Taller de Rendimiento")
    print("Autores: Juan David Garzon Ballen, Juan Pablo Sanchez")
    print("="*70)
    print()
    
    # Procesar resultados
    print("📊 Procesando resultados...")
    df = procesar_resultados('Resultados')
    
    if df.empty:
        print("❌ No se encontraron datos para procesar")
        return
    
    print(f"\n✓ Total de configuraciones analizadas: {len(df)}")
    print(f"✓ Programas: {', '.join(sorted(df['Programa'].unique()))}")
    print(f"✓ Tamaños: {sorted(df['Tamano'].unique())}")
    print(f"✓ Hilos: {sorted(df['Hilos'].unique())}")
    
    # Guardar CSV
    print("\n📝 Guardando datos procesados...")
    df.to_csv('resultados_procesados.csv', index=False)
    print("✓ Archivo 'resultados_procesados.csv' generado")
    
    # Generar gráficas
    print("\n📈 Generando gráficas...")
    generar_graficas_tiempo(df)
    generar_graficas_speedup(df)
    generar_grafica_eficiencia(df)
    generar_grafica_comparativa_general(df)
    
    # Generar tablas LaTeX
    print("\n📋 Generando tablas LaTeX...")
    generar_tablas_latex(df)
    
    # Generar resumen
    print("\n📄 Generando resumen ejecutivo...")
    generar_resumen_ejecutivo(df)
    
    # Estadísticas finales
    print("\n" + "="*70)
    print("ESTADÍSTICAS FINALES")
    print("="*70)
    
    for programa in sorted(df['Programa'].unique()):
        print(f"\n{programa}:")
        datos_prog = df[df['Programa'] == programa]
        print(f"  Tiempo mínimo: {datos_prog['Tiempo_Min'].min()/1000000:.3f}s")
        print(f"  Tiempo máximo: {datos_prog['Tiempo_Max'].max()/1000000:.3f}s")
        mejor_config = datos_prog.loc[datos_prog['Tiempo_Promedio'].idxmin()]
        print(f"  Mejor: N={int(mejor_config['Tamano'])}, Hilos={int(mejor_config['Hilos'])}")
    
    print("\n" + "="*70)
    print("✅ Análisis Completado")
    print("="*70)

if __name__ == "__main__":
    main()

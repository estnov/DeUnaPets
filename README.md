## Instalación
1. Clona el repositorio:
   git clone https://github.com/estnov/DeUnaPets.git

2. Instala las dependencias:
   cd .\DeUnaPets\Simulacion\De-Una-Pets\
   npm install
   

---
# Modelos

## Descripción
Este directorio contiene un programa en C++ diseñado para analizar transacciones financieras a partir de un archivo CSV y una simulación del entorno DeUna para el proyecto con la implementación DeUna pets. El objetivo principal es identificar y clasificar los gastos hormiga, predecir los gastos futuros mediante un modelo de series de tiempo y expresar estas conclusiones en una mascota virtual la cual correlacionará su estado emocional acorde al nivel de organización financiera del usuario. El programa en c++ utiliza técnicas de análisis estadístico y modelado para proporcionar insights útiles sobre los hábitos de gasto; mientras que la simulación se ha desarrollado enteramente en Angular.
 
---

## Descripción del Algoritmo de análisis de datos
El programa sigue estos pasos:

1. **Carga de Datos:**
- Lee las transacciones desde un archivo CSV especificado en la configuración.
- Limpia y valida los datos antes de procesarlos.

2. **Clasificación de Gastos Hormiga:**
- Calcula un umbral percentil (al 25% e inferiores) para identificar los gastos pequeños y frecuentes.
- Imprime y guarda los gastos hormiga detectados.

3. **Identificación de Gastos Recurrentes:**
- Detecta patrones en los gastos hormiga basándose en la descripción de las transacciones.
- Guarda estos datos en `recurrencias.dat`.

4. **Predicción de Gastos Futuros:**
- Utiliza un modelo de series de tiempo con suavizamiento exponencial simple.
- Genera predicciones para los próximos 30 días, considerando un factor de variabilidad para imprevistos.
- Guarda los datos de la predicción en `serie_tiempo.dat`.

5. **Generación de Archivos:**
- `recurrencias.dat` para gastos recurrentes.
- `serie_tiempo.dat` para visualizar la tendencia de los gastos.

---

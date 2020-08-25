A continuación se detallan los targets del Makefile:
- all: compilar todos los binarios
- clean: eliminar todos los binarios
- player_gridsearch: binario del jugador elegido del gridsearch, para el juez de Python proporcionado por la cátedra.
- player_genetico: binario del jugador elegido del genetico, para el juez de Python proporcionado por la cátedra.
- all-gen: para compilar todas las variantes del programa con el algoritmo genético.
- gen-tct-estr-crand: compilar genético con método de evaluación "torneo TCT", selección estricta y crossover random
- gen-tct-pond-crand: compilar genético con método de evaluación "torneo TCT", selección ponderada y crossover random
- gen-tct-pond-cfijo: compilar genético con método de evaluación "torneo TCT", selección ponderada y crossover fijo
- gen-ant-estr-crand: compilar genético con método de evaluación "jugar contra mejores anteriores", selección estricta y crossover random
- gen-ant-pond-crand: compilar genético con método de evaluación "jugar contra mejores anteriores", selección ponderada y crossover random
- gen-ant-pond-cfijo: compilar genético con método de evaluación "jugar contra mejores anteriores", selección ponderada y crossover fijo
- gen-tct-pond-cfijo-redmut: compilar genético con método de evaluación "torneo TCT", selección ponderada, crossover fijo y mutaciones reducidas (prob 0.01).
- gridsearch: compilar programa de grid-search exhaustivo (fuerza bruta).
- torneo: compilar programa que realiza un torneo todos contra todos entre una serie de jugadores.

------------------------------------------------------------
Instrucciones de uso de los programas del algoritmo genético
------------------------------------------------------------

El modo de uso de los programas que empiezan con "gen-" es el siguiente:

	> ./gen-x-y-z <nombre_experimento>

El programa crea dos archivos:
- <nombre_experimento>.out: aquí se van agregando los mejores jugadores por cada generación.
- <nombre_experimento>_ultimapob.txt: por cada iteración este archivo se sobreescribe con los datos de la población actual, que son
  los parámetros de todos los jugadores junto con sus valores de fitness.

El segundo modo de uso es:

	> ./gen-x-y-z <nombre_experimento> <path_ultima_poblacion>

Tiene el mismo comportamiento pero continúa desde la población guardada en el archivo "path_ultima_poblacion", con el mismo formato con
el cual se guardan las poblaciones en los archivos terminados con "_ultimapob".

------------------------------------------------------------
Carpetas y scripts
------------------------------------------------------------

En la carpeta "resultados_genetico" se encuentran todos los resultados obtenidos de las experimentaciones con el algoritmo genético. El
script "plot.py" grafica estos resultados, aunque en la carpeta "graficos" ya se encuentran creados. A su vez, el archivo 
"genetico_torneo_final.txt" contiene los jugadores obtenidos de todas las variantes del algoritmo genético, listos para ser leídos por 
el programa "torneo".

En la carpeta "resultados_gridsearch" se encuentran los resultados obtenidos de las experimentaciones relacionadas con grid search. 
El notebook "experimentacion-gridsearch.ipynb" es donde se genera el mapa de calor. El archivo "rivales.txt" muestra el conjunto de 
jugadores rivales que se usaron para dar puntaje a los jugadores. El archivo "resultados-tct-rivales.txt" tiene los resultados de un 
torneo todos contra todos entre los rivales.

------------------------------------------------------------
Instrucciones de uso del programa "torneo"
------------------------------------------------------------

El programa "torneo" recibe una lista de jugadores y realiza un torneo todos contra todos entre ellos, devolviendo los resultados.
El modo de uso es el siguiente:

	> ./torneo <path_jugadores> <path_resultados>

<path_jugadores> es el path del archivo con los jugadores. La primera línea debe indicar la cantidad N de jugadores en el archivo,
la mínima cantidad de filas de los tableros, la máxima cantidad de filas de los tableros, y la cantidad de tableros a usar por cada
par de jugadores que se enfrentan (se juegan dos partidos por tablero). Los cuatro valores deben estar separados por espacios.

Luego deben seguir N líneas de la forma:

	nombre parametro1 parametro2 ... parametro6

donde "nombre" es un string que identifica al jugador.

El programa devuelve por el archivo con path <path_resultados> los resultados. Este archivo contiene N líneas, cada una de la forma:

	nombre partidos_ganados partidos_empatados partidos_perdidos puntaje


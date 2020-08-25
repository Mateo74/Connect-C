GENETICO_BINARIOS = \
	gen-tct-estr-crand \
	gen-tct-pond-crand \
	gen-ant-estr-crand \
	gen-ant-pond-crand \
	gen-tct-pond-cfijo \
	gen-tct-pond-cfijo-redmut \
	gen-ant-pond-cfijo 

.PHONY: all clean

all: player_gridsearch player_genetico $(GENETICO_BINARIOS) gridsearch torneo

player_gridsearch:
	g++ -std=c++11 -Wall -Wextra -O3 player_gridsearch.cpp -o $@

player_genetico:
	g++ -std=c++11 -Wall -Wextra -O3 player_genetico.cpp -o $@


all-gen: $(GENETICO_BINARIOS)


gen-tct-estr-crand:
	g++ -std=c++11 -Wall -Wextra -O3 -DTORNEO -DCROSSOVER_RANDOM genetico.cpp -o $@

gen-tct-pond-crand:
	g++ -std=c++11 -Wall -Wextra -O3 -DTORNEO -DSELECCION_PONDERADA -DCROSSOVER_RANDOM genetico.cpp -o $@

gen-tct-pond-cfijo:
	g++ -std=c++11 -Wall -Wextra -O3 -DTORNEO -DSELECCION_PONDERADA genetico.cpp -o $@


gen-ant-estr-crand:
	g++ -std=c++11 -Wall -Wextra -O3 -DCROSSOVER_RANDOM genetico.cpp -o $@

gen-ant-pond-crand:
	g++ -std=c++11 -Wall -Wextra -O3 -DSELECCION_PONDERADA -DCROSSOVER_RANDOM genetico.cpp -o $@

gen-ant-pond-cfijo:
	g++ -std=c++11 -Wall -Wextra -O3 -DSELECCION_PONDERADA genetico.cpp -o $@


gen-tct-pond-cfijo-redmut:
	g++ -std=c++11 -Wall -Wextra -O3 -DTORNEO -DSELECCION_PONDERADA -DPOCAS_MUTACIONES genetico.cpp -o $@


gridsearch:
	g++ -std=c++11 -Wall -Wextra -O3 gridsearch.cpp -o $@


torneo:
	g++ -std=c++11 -Wall -Wextra -O3 torneo.cpp -o $@


clean:
	rm -f player_gridsearch player_genetico $(GENETICO_BINARIOS) gridsearch torneo

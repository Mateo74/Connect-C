#include "juez_cpp.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

#define MUTACION_LINEAS_C_DESVIO_STD 100.0
#define MUTACION_MAXLONG_DESVIO_STD 20.0
#define MUTACION_CANT_LINEAS_DESVIO_STD 20.0

#define MIN_LINEAS_C  (-1000.0)
#define MAX_LINEAS_C  1000.0
#define MIN_MAXLONG (-100.0)
#define MAX_MAXLONG  100.0
#define MIN_CANT_LINEAS (-100.0)
#define MAX_CANT_LINEAS  100.0

#ifdef TORNEO
	#pragma message("Evaluacion: torneo")
#else
	#pragma message("Evaluacion: jugar contra anteriores")
#endif

#ifdef SELECCION_PONDERADA
	#pragma message("Seleccion: ponderada")
#else
	#pragma message("Seleccion: estricta")
#endif

#ifdef CROSSOVER_RANDOM
	#pragma message("Crossover: random")
#else
	#pragma message("Crossover: fijo")
#endif

#ifdef POCAS_MUTACIONES
	#pragma message("Mutaciones: reducidas")
#else
	#pragma message("Mutaciones: activadas")
#endif


std::default_random_engine generator(std::time(0));

//Genera un jugador random valido (distribucion uniforme)
Parametros jugador_random()
{
	std::uniform_real_distribution<double> udist_lineas_c(MIN_LINEAS_C, MAX_LINEAS_C);
	std::uniform_real_distribution<double> udist_maxlong(MIN_MAXLONG, MAX_MAXLONG);
	std::uniform_real_distribution<double> udist_cant_lineas(MIN_CANT_LINEAS, MAX_CANT_LINEAS);

	Parametros res;
	
	res.lineas_c_player = udist_lineas_c(generator);
	res.lineas_c_oponente = udist_lineas_c(generator);
	res.maxima_longitud_player = udist_maxlong(generator);
	res.maxima_longitud_oponente = udist_maxlong(generator);
	res.cant_lineas_player = udist_cant_lineas(generator);
	res.cant_lineas_oponente = udist_cant_lineas(generator);
	
	return res;
}

//Con probabilidad 'prob' suma un valor a 'gen' con distribucion normal centrada 
//en 0.0 con desvio estandar dado. En caso de hacerlo, hay una probabilidad de 0.2
//de que cambie de signo
void mutar(double& gen, double prob, double desvio_std)
{
	std::uniform_real_distribution<double> udist(0.0, 1.0);
	if (udist(generator) <= prob)
	{
		std::normal_distribution<double> ndist(0.0, desvio_std);
		gen += ndist(generator);
		
		//En caso de mutacion, probabilidad de 0.2 de cambiar de signo
		if (udist(generator) <= 0.2)
		{
			gen = - gen;
		}
	}
}

//Muta cada gen de un jugador con cierta probabilidad.
void mutar(Parametros& params)
{
	#ifdef POCAS_MUTACIONES
	double prob = 0.01;
	#else
	double prob = 0.1; //Probabilidad de mutar cada gen
	#endif

	mutar(params.lineas_c_player, prob, MUTACION_LINEAS_C_DESVIO_STD);
	mutar(params.lineas_c_oponente, prob, MUTACION_LINEAS_C_DESVIO_STD);
	mutar(params.maxima_longitud_player, prob, MUTACION_MAXLONG_DESVIO_STD);
	mutar(params.maxima_longitud_oponente, prob, MUTACION_MAXLONG_DESVIO_STD);
	mutar(params.cant_lineas_player, prob, MUTACION_CANT_LINEAS_DESVIO_STD);
	mutar(params.cant_lineas_oponente, prob, MUTACION_CANT_LINEAS_DESVIO_STD);
}

// Cruza a y b y devuelve dos jugadores resultantes. El punto de 
// corte es random.
void cruzar_corte_random(const Parametros& a, const Parametros& b, Parametros& res1, Parametros& res2)
{
	//Punto de corte random. Casteamos 'a' y 'b' a arreglos de doubles para que sea mas comodo.
	const double* a_array = reinterpret_cast<const double*>(&a);
	const double* b_array = reinterpret_cast<const double*>(&b);
	std::uniform_int_distribution<int> udist(0,6);
	int corte = udist(generator);
	
	double* res1_array = reinterpret_cast<double*>(&res1);
	double* res2_array = reinterpret_cast<double*>(&res2);
	for (int i = 0; i < corte; i++) {
		res1_array[i] = a_array[i];
		res2_array[i] = b_array[i];
	}
	for (int i = corte; i < 6; i++) {
		res1_array[i] = b_array[i];
		res2_array[i] = a_array[i];
	}
}

// Cruza a y b de manera no-random y devuelve dos jugadores resultantes.
void cruzar_corte_fijo(const Parametros& a, const Parametros& b, Parametros& res1, Parametros& res2)
{
	res1.lineas_c_player = a.lineas_c_player;
	res1.lineas_c_oponente = b.lineas_c_oponente;
	res1.maxima_longitud_player = a.maxima_longitud_player;
	res1.maxima_longitud_oponente = b.maxima_longitud_oponente;
	res1.cant_lineas_player = a.cant_lineas_player;
	res1.cant_lineas_oponente = b.cant_lineas_oponente;

	res2.lineas_c_player = b.lineas_c_player;
	res2.lineas_c_oponente = a.lineas_c_oponente;
	res2.maxima_longitud_player = b.maxima_longitud_player;
	res2.maxima_longitud_oponente = a.maxima_longitud_oponente;
	res2.cant_lineas_player = b.cant_lineas_player;
	res2.cant_lineas_oponente = a.cant_lineas_oponente;
}

// Cruza a y b
void cruzar(const Parametros& a, const Parametros& b, Parametros& res1, Parametros& res2)
{
	#ifdef CROSSOVER_RANDOM
	cruzar_corte_random(a, b, res1, res2);
	#else
	cruzar_corte_fijo(a, b, res1, res2);
	#endif
}

// Imprime todos los parametros separados por espacios en 'os'.
// 'os' puede ser std::cout, std::cerr, o un std::ofstream (es decir un archivo).
void output_jugador(std::ostream& os, const Parametros& params)
{
	os << params.lineas_c_player << " ";
	os << params.lineas_c_oponente << " ";
	os << params.maxima_longitud_player << " ";
	os << params.maxima_longitud_oponente << " ";
	os << params.cant_lineas_player << " ";
	os << params.cant_lineas_oponente << " " << std::endl;
}

// Representa a una poblacion de jugadores
class Poblacion
{
public:

	//Poblacion vacia
	Poblacion()
	{

	}
	
	//Cantidad de jugadores de la poblacion
	unsigned tam() const {return jugadores.size();}
	
	//Inicia poblacion con jugadores random
	void iniciar(int cantidad)
	{
		for (int i = 0; i < cantidad; i++)
		{
			Parametros nuevo = jugador_random();
			insertar(nuevo);
		}
	}
	
	//Inicia poblacion leyendo jugadores con fitness de un istream
	void iniciar(std::istream& input)
	{
		for (int i = 0; i < 20; i++)
		{
			int fitness;
			input >> fitness;
			Parametros params;
			input >> params.lineas_c_player;
			input >> params.lineas_c_oponente;
			input >> params.maxima_longitud_player;
			input >> params.maxima_longitud_oponente;
			input >> params.cant_lineas_player;
			input >> params.cant_lineas_oponente;
			insertar(params, fitness);
		}
		
		#ifdef SELECCION_PONDERADA
		preparar_seleccion_ponderada();
		#else
		preparar_seleccion_estricta();
		#endif
	}
	
	//Inserta un jugador
	void insertar(const Parametros& j, int fitness = 0)
	{
		jugadores.push_back(std::make_pair(j, fitness));
	}
	
	void evaluar_jugadores(Poblacion& anterior)
	{
		#ifdef TORNEO
		(void)anterior; //Callar al compilador
		evaluar_jugadores_torneo();
		#else
		evaluar_jugadores_contra_anterior(anterior);
		#endif
		
		// Una vez calculados los fitness dejamos preparadas algunas cositas para 
		// que despues se puedan ir seleccionando los jugadores de acuerdo al 
		// metodo de seleccion elegido
		
		#ifdef SELECCION_PONDERADA
		preparar_seleccion_ponderada();
		#else
		preparar_seleccion_estricta();
		#endif
	}

	//Selecciona a un jugador en base al criterio de seleccion elegido
	//(ponderada vs seleccionar a los mejores)
	const Parametros& seleccionar()
	{
		#ifdef SELECCION_PONDERADA
		return seleccionar_ponderada();
		#else
		return seleccionar_estricta();
		#endif
	}
	
	//Devuelve el mejor jugador (el de mayor fitness)
	const Parametros& mejor_jugador()
	{
		int best_fitness = jugadores[0].second;
		const Parametros* best = &(jugadores[0].first);
		for (unsigned i = 1; i < jugadores.size(); i++)
		{
			if (jugadores[i].second > best_fitness)
			{
				best_fitness = jugadores[i].second;
				best = &(jugadores[i].first);
			}
		}
		return *best;
	}
	
	//Ordena jugadores por fitness
	void ordenar_por_fitness()
	{
		auto comp = [](const std::pair<Parametros,int>& a, const std::pair<Parametros,int>& b) {
			return a.second < b.second;
		};
		
		std::sort(jugadores.begin(), jugadores.end(), comp);
	}
	
	//Obtiene el (n-1-i)-ésimo jugador
	const Parametros& obtener_jugador(int i) const
	{
		return jugadores[jugadores.size() - 1 - i].first;
	}
	
	void print(std::ostream& os) const
	{
		for (unsigned i = 0; i < jugadores.size(); i++)
		{
			os << jugadores[i].second << std::endl;
			output_jugador(os, jugadores[i].first);
		}
	}
	
private:
	
	//Computa funcion de fitness para cada jugador haciendo torneo todos contra todos.
	//El fitness de cada uno es la cantidad de puntos que sumo en todo el torneo.
	void evaluar_jugadores_torneo()
	{
		for (unsigned i = 0; i < jugadores.size(); i++)
		{
			for (unsigned j = 0; j < i; j++)
			{
				jugar(jugadores[i].first, jugadores[j].first, jugadores[i].second, jugadores[j].second);
			}
		}
	}
	
	void evaluar_jugadores_contra_anterior(Poblacion& anterior)
	{
		anterior.ordenar_por_fitness();
		for (unsigned i = 0; i < jugadores.size(); i++)
		{
			for (unsigned j = 0; j < anterior.tam()/2; j++)
			{
				int unused;
				jugar(jugadores[i].first, anterior.obtener_jugador(j), jugadores[i].second, unused);
			}
		}
	}
	
	void preparar_seleccion_ponderada()
	{
		sumas_fitness.resize(jugadores.size());
		total_fitness = jugadores[0].second;
		sumas_fitness[0] = total_fitness;
		for (unsigned i = 1; i < jugadores.size(); i++)
		{
			total_fitness += jugadores[i].second;
			sumas_fitness[i] = sumas_fitness[i-1] + jugadores[i].second;
		}
	}
	
	void preparar_seleccion_estricta()
	{
		ordenar_por_fitness();
		next = jugadores.size() - 1; //Proximo jugador a seleccionar
	}
	
	const Parametros& seleccionar_ponderada()
	{
		std::uniform_int_distribution<int> udist(0, total_fitness-1);
		int rand = udist(generator);
		for (unsigned i = 0; i < sumas_fitness.size(); i++)
		{
			if (rand < sumas_fitness[i]) return jugadores[i].first;
		}
		return jugadores.back().first;//Nunca va a llegar aca, lo pongo para evitar warnings
	}
	
	const Parametros& seleccionar_estricta()
	{
		return jugadores[next--].first;
	}
	
	
	//Lista de jugadores, cada uno con su fitness
	std::vector<std::pair<Parametros, int> > jugadores;
	
	//Para seleccion ponderada
	std::vector<int> sumas_fitness; //Cuando se selecciona al azar se recorre este arreglo para ver a quien le "pegamos".
	int total_fitness; //La seleccion ponderada consiste en generar un numero entre 0 y total_fitness - 1
	
	//Para seleccion estricta
	int next;

friend void swap(Poblacion& a, Poblacion& b);

};

void swap(Poblacion& a, Poblacion& b)
{
	a.jugadores.swap(b.jugadores);
	a.sumas_fitness.swap(b.sumas_fitness);
	std::swap(a.total_fitness, b.total_fitness);
	std::swap(a.next, b.next);
}

//Esto tiene que definirse bien
bool criterio_de_terminacion(const Poblacion& /*p*/, int /*iteraciones*/, double tiempo)
{
	return tiempo > 60.0*60.0*1000.0;
}

//El algoritmo genetico
void genetico(const std::string& nombre_experimento, Poblacion actual)
{
	std::string path_jugadores = nombre_experimento + ".out";
	std::string path_ultima_poblacion = nombre_experimento + "_ultimapob.txt";
	
	auto start = std::chrono::steady_clock::now();
	double tiempo = 0.0; //Contador de tiempo, puede servir para criterio de terminacion
	int iteraciones = 0;
	
	while (!criterio_de_terminacion(actual, iteraciones, tiempo))
	{
		std::cerr << "Iteracion: " << iteraciones << std::endl;
		
		Poblacion nueva;
		for (unsigned i = 0; i < actual.tam()/2; i++)
		{
			const Parametros& padre1 = actual.seleccionar();
			const Parametros& padre2 = actual.seleccionar();
			Parametros hijo1, hijo2;
			cruzar(padre1, padre2, hijo1, hijo2);
			mutar(hijo1);
			mutar(hijo2);
			nueva.insertar(hijo1);
			nueva.insertar(hijo2);
		}

		nueva.evaluar_jugadores(actual);
		swap(actual, nueva);
		
		auto diff = std::chrono::steady_clock::now() - start;
		tiempo = std::chrono::duration<double, std::milli>(diff).count(); //Actualizamos contador de tiempo
		iteraciones++;
		
		std::ofstream output(path_jugadores, std::ios::app);
		output_jugador(output, actual.mejor_jugador());
		
		std::ofstream output_pob(path_ultima_poblacion);
		actual.print(output_pob);
	}
}

void genetico_iniciar(const std::string& nombre_experimento)
{
	Poblacion actual;
	actual.iniciar(20);
	
	#ifdef TORNEO
	actual.evaluar_jugadores(actual);
	#else
	{
		Poblacion otra;
		otra.iniciar(20);
		actual.evaluar_jugadores(otra);
	}
	#endif

	genetico(nombre_experimento, actual);
}

void genetico_continuar(const std::string& nombre_experimento, const std::string& path_poblacion)
{
	std::cerr << "Continuando con experimentacion..." << std::endl;
	std::ifstream input(path_poblacion);
	if (!input.is_open())
	{
		std::cerr << "No se pudo abrir el archivo con la poblacion guardada." << std::endl;
		return;
	}
	
	Poblacion actual;
	actual.iniciar(input);
	input.close();
	genetico(nombre_experimento, actual);
}

int main(int argc, char* argv[])
{
	std::string nombre_experimento = "genetico_output";
	std::string path_poblacion = "none";
	bool continuar = false;
	if (argc == 2)
	{
		nombre_experimento = std::string(argv[1]);
	}
	else if (argc == 3)
	{
		nombre_experimento = std::string(argv[1]);
		path_poblacion = std::string(argv[2]);
		continuar = true;
	}
	
	if (continuar)
	{
		genetico_continuar(nombre_experimento, path_poblacion);
	}
	else
	{
		genetico_iniciar(nombre_experimento);
	}
}
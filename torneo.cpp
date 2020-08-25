#include "juez_cpp.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>

using namespace std;

std::default_random_engine generator(time(0));

struct Resultados
{
	Resultados()
	{
		puntaje_a = 0;
		ganados_a = 0;
		empatados_a = 0;
		perdidos_a = 0;
		
		puntaje_b = 0;
		ganados_b = 0;
		empatados_b = 0;
		perdidos_b = 0;
	}
	
	void gano_a()
	{
		ganados_a++;
		perdidos_b++;
	}
	
	void gano_b()
	{
		ganados_b++;
		perdidos_a++;
	}
	
	void empataron()
	{
		empatados_a++;
		empatados_b++;
	}
	
	void calcular_puntajes()
	{
		puntaje_a = 2*ganados_a + empatados_a;
		puntaje_b = 2*ganados_b + empatados_b;
	}
	
	int puntaje_a;
	int ganados_a;
	int empatados_a;
	int perdidos_a;
	
	int puntaje_b;
	int ganados_b;
	int empatados_b;
	int perdidos_b;
};

Resultados enfrentar(const Parametros& a, const Parametros& b, int minN, int maxN, int tableros)
{
	Resultados res;
	
	uniform_int_distribution<int> generar_N(minN, maxN);
	
	// 3 partidos. En realidad cada partido son dos: se alterna quien empieza primero.
	// Ganador suma 2 puntos y perdedor 0. Si hay empate suman 1 punto cada uno
	for (int k = 0; k < tableros; k++)
	{
		// Se genera N aleatoriamente, M aleatoriamente en funcion 
		// de N y c aleatoriamente en funcion de N y M.
		// La idea es que los tableros sean lo mas cuadrados posibles, 
		// y que c no sea arbitrariamente chico.
		int N = generar_N(generator);
		uniform_int_distribution<int> generar_M(3*N/4 + 1, 3*N/2);
		int M = generar_M(generator);
		uniform_int_distribution<int> generar_c(min(N,M)/3 + 1, 2*min(N,M)/3 + 1);
		int c = generar_c(generator);
		
		//'a' empieza jugando
		int result = jugar(N, M, c, a, b);
		if (result == 1)
		{
			res.gano_a();
		}
		else if (result == 0)
		{
			res.empataron();
		}
		else
		{
			res.gano_b();
		}
		
		//'b' empieza jugando
		result = jugar(N, M, c, b, a);
		if (result == 1)
		{
			res.gano_b();
		}
		else if (result == 0)
		{
			res.empataron();
		}
		else
		{
			res.gano_a();
		}
	}
	
	res.calcular_puntajes();
	return res;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cerr << "Uso: ./torneo <path_jugadores> <path_resultados>" << endl;
		return 0;
	}
	
	string path_input = argv[1];
	string path_output = argv[2];
	
	std::ifstream input(path_input);
	if (!input.is_open())
	{
		cerr << "No se pudo abrir " << path_input << endl;
		return 1;
	}
	
	unsigned num_jugadores;
	int minN, maxN, tableros;
	input >> num_jugadores;
	input >> minN;
	input >> maxN;
	input >> tableros;
	
	vector<string> nombres(num_jugadores);
	vector<Parametros> jugadores(num_jugadores);
	
	for (unsigned i = 0; i < num_jugadores; i++)
	{
		input >> nombres[i];
		input >> jugadores[i].lineas_c_player;
		input >> jugadores[i].lineas_c_oponente;
		input >> jugadores[i].maxima_longitud_player;
		input >> jugadores[i].maxima_longitud_oponente;
		input >> jugadores[i].cant_lineas_player;
		input >> jugadores[i].cant_lineas_oponente;
	}
	
	input.close();
	
	vector<int> puntajes(num_jugadores, 0);
	vector<int> ganados(num_jugadores, 0);
	vector<int> perdidos(num_jugadores, 0);
	vector<int> empatados(num_jugadores, 0);
	
	for (unsigned i = 0; i < jugadores.size(); i++)
	{
		for (unsigned j = 0; j < i; j++)
		{
			Resultados res = enfrentar(jugadores[i], jugadores[j], minN, maxN, tableros);
			
			puntajes[i] += res.puntaje_a;
			ganados[i] += res.ganados_a;
			perdidos[i] += res.perdidos_a;
			empatados[i] += res.empatados_a;
	
			puntajes[j] += res.puntaje_b;
			ganados[j] += res.ganados_b;
			perdidos[j] += res.perdidos_b;
			empatados[j] += res.empatados_b;
		}
	}
	
	std::ofstream output(path_output);
	for (unsigned i = 0; i < jugadores.size(); i++)
	{
		output << nombres[i] << " ";
		output << ganados[i] << " ";
		output << empatados[i] << " ";
		output << perdidos[i] << " ";
		output << puntajes[i] << " ";
		output << endl;
	}
	
	output.close();
	return 0;
}
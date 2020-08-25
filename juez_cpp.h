#ifndef JUEZ_CPP_H
#define JUEZ_CPP_H

#include "board.h"
#include <limits>
#include <random>

extern std::default_random_engine generator;

//Heuristica para determinar la proxima jugada (proxima columna a elegir). 
//Evalua todas las jugadas y se queda con la que genere el mejor tablero.
int next_column(Board& board, Player player, const Parametros& params)
{
	double best = std::numeric_limits<double>::lowest();
	int best_column = -1;
	for (int j = 0; j < board.columns(); j++)
	{
		if (board.colsize(j) < board.rows())
		{
			board.insert(j, player);
			double eval = board.evaluar(player, params);
			board.undo_insert();
			if (eval > best)
			{
				best = eval;
				best_column = j;
			}
		}
	}
	
	return best_column;
}

//Hace jugar una vez al jugador azul contra el rojo. Empieza el azul.
//Devuelve 1 si gana azul, -1 si gana rojo, 0 si empataron. En 'rendimiento'
//devuelve un valor entre 0 y 1 que indica que tan bien le fue al azul.
// int jugar(int N, int M, int c, const Parametros& azul, const Parametros& rojo, double* rendimiento = NULL)
int jugar(int N, int M, int c, const Parametros& azul, const Parametros& rojo)
{
	Board board(N,M,c);
	int resultado;
	while (true)
	{
		int next = next_column(board, PLAYER_1, azul);
		if (next == -1)
		{
			resultado = 0; break;
		}
		
		board.insert(next, PLAYER_1);
		if (board.ganador(PLAYER_1))
		{
			resultado = 1; break;
		}
		
		next = next_column(board, PLAYER_2, rojo);
		if (next == -1)
		{
			resultado = 0; break;
		}
		
		board.insert(next, PLAYER_2);
		if (board.ganador(PLAYER_2))
		{
			resultado = -1; break;
		}
	}

	return resultado;
}

// Ejecuta tres juegos con configuraciones random entre a y b.
// Suma los puntos correspondientes a cada jugador y devuelve 
// el rendimiento promedio de a.
void jugar(const Parametros& a, const Parametros& b, int& contador_puntos_a, int& contador_puntos_b)
{
	std::uniform_int_distribution<int> generar_N(10, 14);
	
	// 3 partidos. En realidad cada partido son dos: se alterna quien empieza primero.
	// Ganador suma 2 puntos y perdedor 0. Si hay empate suman 1 punto cada uno
	for (int k = 0; k < 3; k++)
	{
		// Se genera N aleatoriamente, M aleatoriamente en funcion 
		// de N y c aleatoriamente en funcion de N y M.
		// La idea es que los tableros sean lo mas cuadrados posibles, 
		// y que c no sea arbitrariamente chico.
		int N = generar_N(generator);
		std::uniform_int_distribution<int> generar_M(3*N/4 + 1, 3*N/2);
		int M = generar_M(generator);
		std::uniform_int_distribution<int> generar_c(std::min(N,M)/3 + 1, 2*std::min(N,M)/3 + 1);
		int c = generar_c(generator);
		
		//'a' empieza jugando
		int result = jugar(N, M, c, a, b);
		if (result == 1)
		{
			contador_puntos_a += 2;
		}
		else if (result == 0)
		{
			contador_puntos_a += 1;
			contador_puntos_b += 1;
		}
		else
		{
			contador_puntos_b += 2;
		}
		
		//'b' empieza jugando
		result = jugar(N, M, c, b, a);
		if (result == 1)
		{
			contador_puntos_b += 2;
		}
		else if (result == 0)
		{
			contador_puntos_a += 1;
			contador_puntos_b += 1;
		}
		else
		{
			contador_puntos_a += 2;
		}
	}
}

#endif
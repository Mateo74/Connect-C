#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <sys/stat.h>
#include <limits>
#include "board.h"

int ganados = 0;
int perdidos = 0;
int empatados = 0;

//Heuristica para determinar la proxima jugada (proxima columna a elegir).
//Evalua todas las jugadas y se queda con la que genere el mejor tablero.
int next_column(Board& board, const Parametros& params)
{
	double best = std::numeric_limits<double>::lowest();
	int best_column = -1;
	for (int j = 0; j < board.columns(); j++)
	{
		if (board.colsize(j) < board.rows())
		{
			board.insert(j, PLAYER_1);
			double eval = board.evaluar(PLAYER_1, params);
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

/*--------------------------------------------------------------------------------*/

void send(const std::string& msg) {
	std::cout << msg << std::endl;
}

void send(int msg) {
	std::cout << msg << std::endl;
}

int read_int() {
	std::string msg;
	std::cin >> msg;
	if (msg == "salir") {
		std::cerr << "Partidos ganados por el genetico/grid-search: " << ganados << std::endl;
		send("listo");
		std::exit(0);
	}
	return std::stoi(msg);
}

std::string read_str() {
	std::string msg;
	std::cin >> msg;
	if (msg == "salir") {
		std::cerr << "Partidos ganados por el genetico/grid-search: " << ganados << std::endl;
		send("listo");
		std::exit(0);
	}
	return msg;
}

int main()
{
	Parametros params = {3685.79, -2865.87, 356.439, -103.028, 60.7866, 43.7294};

	std::string msg, color, oponent_color, go_first;
	int columns, rows, c, p, move;
	(void)p;

	while (true) {
		color = read_str();
		oponent_color = read_str();

		columns = read_int();
		rows = read_int();
		c = read_int();
		p = read_int();

		Board board(rows, columns, c); //Crea el tablero vacio

		go_first = read_str();
		if (go_first == "vos") {
			move = next_column(board, params);
			board.insert(move, PLAYER_1); //Insertar ficha nuestra
			send(move);
		}

		while (true) {
			msg = read_str();
			if (msg == "ganaste" || msg == "perdiste" || msg == "empataron") {
				if (msg == "ganaste") {
					ganados++;
				}
				else if (msg == "perdiste") {
					perdidos++;
				}
				else {
					empatados++;
				}
				break;
			}

			board.insert(std::stoi(msg), PLAYER_2); //Insertar ficha del oponente

			move = next_column(board, params); //Proxima jugada
			board.insert(move, PLAYER_1); //Insertamos ficha nuestra
			send(move);
		}
	}

	std::cerr << "Partidos ganados por el genetico/grid-search: " << ganados << std::endl;
	std::cerr << "Partidos empatados por el genetico/grid-search: " << empatados << std::endl;
	std::cerr << "Partidos perdidos por el genetico/grid-search: " << perdidos << std::endl;

	return 0;
}

#include <iostream>
#include "board.h"
#include "juez_cpp.h"
#include <fstream>

#define CANT_PARAMETROS 6
#define CANT_VALORES 9
#define CANT_RIVALES 9

void imprimirJugador(Parametros jugador) {
	std::cout <<
	"lineas_c_player = "           << jugador.lineas_c_player          << std::endl <<
	"lineas_c_oponente = "         << jugador.lineas_c_oponente        << std::endl <<
	"maxima_longitud_player = "    << jugador.maxima_longitud_player   << std::endl <<
	"maxima_longitud_oponente = "  << jugador.maxima_longitud_oponente << std::endl <<
	"cant_lineas_player = "        << jugador.cant_lineas_player       << std::endl <<
	"cant_lineas_oponente = "      << jugador.cant_lineas_oponente     << std::endl;
}

double valores_parametros[CANT_PARAMETROS][CANT_VALORES] = {
	{-1000, -500, -100, -10, 0, 10, 100, 500, 1000},
	{-1000, -500, -100, -10, 0, 10, 100, 500, 1000},
	{-150, -80, -10, -2, 0, 2, 10, 80, 150},
	{-150, -80, -10, -2, 0, 2, 10, 80, 150},
	{-150, -80, -10, -2, 0, 2, 10, 80, 150},
	{-150, -80, -10, -2, 0, 2, 10, 80, 150},
};

int dims[3][3] = {
{7, 6, 4},
{9, 8, 5},
{14,14,7}
};

Parametros rivales[CANT_RIVALES];
int mejoresResultados[CANT_PARAMETROS][CANT_VALORES];


void inicializarRivales()
{
	//rivales[0]: jugador super ofensivo, no le importa lo que hace el rival.
	rivales[0].lineas_c_player = 1000;
	rivales[0].lineas_c_oponente = 0;
	rivales[0].maxima_longitud_player = 100;
	rivales[0].maxima_longitud_oponente = 0;
	rivales[0].cant_lineas_player = 10;
	rivales[0].cant_lineas_oponente = 0;

	//rivales[1]: Da pesos con con signo sensato, pero no necesariamente peso sensato
	rivales[1].lineas_c_player = 1;
	rivales[1].lineas_c_oponente = - 1;
	rivales[1].maxima_longitud_player = 10;
	rivales[1].maxima_longitud_oponente = - 10;
	rivales[1].cant_lineas_player = 1;
	rivales[1].cant_lineas_oponente = -1;

	//rivales[2]: Opuesto al 1, debería ser malísimo
	rivales[2].lineas_c_player = -1;
	rivales[2].lineas_c_oponente = 1;
	rivales[2].maxima_longitud_player = -10;
	rivales[2].maxima_longitud_oponente = 10;
	rivales[2].cant_lineas_player = -1;
	rivales[2].cant_lineas_oponente = 1;

	//rivales[3]: jugador super defensivo, se preocupa por lo que hace el rival y no le interesa lo propio
	rivales[3].lineas_c_player = 0;
	rivales[3].lineas_c_oponente = -1000;
	rivales[3].maxima_longitud_player = 0;
	rivales[3].maxima_longitud_oponente = -100;
	rivales[3].cant_lineas_player = 0;
	rivales[3].cant_lineas_oponente = -10;

	//rivales[4]: en principio parece bueno, le interesa más la longitud que la cantidad de líneas
	rivales[4].lineas_c_player = 1000;
	rivales[4].lineas_c_oponente = -500;
	rivales[4].maxima_longitud_player = 40;
	rivales[4].maxima_longitud_oponente = -20;
	rivales[4].cant_lineas_player = 2;
	rivales[4].cant_lineas_oponente = -1;

	//rivales[5]: como el 4 pero le interesa más la cantidad que la longitud de líneas
	rivales[5].lineas_c_player = 1000;
	rivales[5].lineas_c_oponente = -500;
	rivales[5].maxima_longitud_player = 2;
	rivales[5].maxima_longitud_oponente = -1;
	rivales[5].cant_lineas_player = 40;
	rivales[5].cant_lineas_oponente = -20;

	//rivales[6]: un buen jugador sacado del genético. No le gusta tener muchas lineas.
	rivales[6].lineas_c_player = 1141.92;
	rivales[6].lineas_c_oponente = -980.758;
	rivales[6].maxima_longitud_player = 166.094;
	rivales[6].maxima_longitud_oponente = -271.139;
	rivales[6].cant_lineas_player = -31.9399;
	rivales[6].cant_lineas_oponente = 87.3286;

	//rivales[7]: un jugador agresivo como el 0, pero prefiere tener pocas lineas
	rivales[7].lineas_c_player = 1000;
	rivales[7].lineas_c_oponente = 0;
	rivales[7].maxima_longitud_player = 100;
	rivales[7].maxima_longitud_oponente = 0;
	rivales[7].cant_lineas_player = -10;
	rivales[7].cant_lineas_oponente = 0;

	//rivales[8]: un jugador defensivo como el 3, pero no le importa la cantidad de lineas del oponente
	rivales[8].lineas_c_player = 0;
	rivales[8].lineas_c_oponente = -1000;
	rivales[8].maxima_longitud_player = 0;
	rivales[8].maxima_longitud_oponente = -100;
	rivales[8].cant_lineas_player = 0;
	rivales[8].cant_lineas_oponente = 0;
}

void actualizarMatriz(int puntaje) {

}

int main()
{
	for (int i=0; i < CANT_PARAMETROS; i++) {
		for (int j=0; j < CANT_VALORES; j++) {
			mejoresResultados[i][j] = -100;
		}
	}
	inicializarRivales();
	Parametros jugador;
	Parametros jugadorMaximo = jugador;
	int puntajeMaximo = -100;
	int cant_mejores = 0;
	int iteraciones = 0;
	for (int a=0; a<CANT_VALORES; a++) {
		jugador.lineas_c_player = valores_parametros[0][a];
		for (int b=0; b<CANT_VALORES; b++) {
			jugador.lineas_c_oponente = valores_parametros[1][b];
			for (int c=0; c<CANT_VALORES; c++) {
				jugador.maxima_longitud_player = valores_parametros[2][c];
				for (int d=0; d<CANT_VALORES; d++) {
					jugador.maxima_longitud_oponente = valores_parametros[3][d];
					for (int e=0; e<CANT_VALORES; e++) {
						jugador.cant_lineas_player = valores_parametros[4][e];
						for (int f=0; f<CANT_VALORES; f++) {
							jugador.cant_lineas_oponente = valores_parametros[5][f];
							int puntaje = 0;
							for (int j = 0; j < CANT_RIVALES; j++)
							{
								for (int r=0; r<3; r++)
								{
									puntaje += jugar(dims[r][0], dims[r][1], dims[r][2], jugador, rivales[j]);
									puntaje -= jugar(dims[r][0], dims[r][1], dims[r][2], rivales[j], jugador);
									//El puntaje suma 1 si gana, -1 si pierde, 0 si empata. Se podría definir de otra forma.
								}
							}
							iteraciones++;
							if (iteraciones%3000 == 0)
							{
								std::cout << "(" << iteraciones << ") Puntaje jugador máximo: " << puntajeMaximo << std::endl;
								std::cout << "Cantidad de jugadores con ese puntaje: " << cant_mejores << std::endl;
							}
							if (puntaje >= puntajeMaximo)
							{
								if (puntaje == puntajeMaximo)
									cant_mejores++;
								else
								{
									cant_mejores = 1;
									puntajeMaximo = puntaje;
									jugadorMaximo = jugador;
									//std::cout << "Ese es el puntaje máximo actual" << std::endl;
								}
							}


							if (puntaje > mejoresResultados[0][a]) {
								mejoresResultados[0][a] = puntaje;
							}
							if (puntaje > mejoresResultados[1][b]) {
								mejoresResultados[1][b] = puntaje;
							}
							if (puntaje > mejoresResultados[2][c]) {
								mejoresResultados[2][c] = puntaje;
							}
							if (puntaje > mejoresResultados[3][d]) {
								mejoresResultados[3][d] = puntaje;
							}
							if (puntaje > mejoresResultados[4][e]) {
								mejoresResultados[4][e] = puntaje;
							}
							if (puntaje > mejoresResultados[5][f]) {
								mejoresResultados[5][f] = puntaje;
							}
						}
					}
				}
			}
		}
	}

	std::cout << "El mejor jugador es:" << std::endl;
	imprimirJugador(jugadorMaximo);
	std::cout << "Su puntaje fue " << puntajeMaximo << "/54." << std::endl;
	std::cout << "La cantidad de jugadores máximos fue " << cant_mejores << std::endl;

	std::ofstream output;
	output.open("output-gridexp.csv");
	output << "1,2,3,4,5,6,7,8,9\n";
	for (int i=0; i<CANT_PARAMETROS; i++) {
		for (int j=0; j<CANT_RIVALES; j++) {
			if (j<CANT_RIVALES-1)
				output << mejoresResultados[i][j] << ",";
			else
				output << mejoresResultados[i][j];
		}
		if (i < CANT_PARAMETROS - 1)
			output << "\n";
	}
	output.close();
	return 0;
}

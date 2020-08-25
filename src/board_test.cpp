#include "board.h"
#include <iostream>

using namespace std;

#define EXPECT(expr) do { if (!(expr)) {std::cerr << "Linea " << __LINE__ << ": Expect fallido: " #expr << std::endl;pasa_test=false;}} while(0)

#define THIS_PLAYER PLAYER_1
#define OPPONENT PLAYER_2

int main()
{
	Board board(4,6,4);
	/*
	Construimos este tablero de 4x6 (x == THIS_PLAYER, o == OPPONENT):
	------
	-o---x
	xxo--o
	xxxooo
	
	Vamos insertando las fichas en un orden tal que a veces extendamos lineas 
	a izquierda, a derecha, o unamos dos lineas. Cada tanto insertamos una 
	cualquiera y deshacemos la insercion.
	*/
	board.insert(1, OPPONENT);
	board.undo_insert();
	
	board.insert(0, THIS_PLAYER);
	board.insert(2, THIS_PLAYER);
	board.insert(1, THIS_PLAYER);
	board.insert(1, THIS_PLAYER);
	
		board.insert(0, THIS_PLAYER);
		board.undo_insert();
	
	board.insert(5, OPPONENT);
	board.insert(4, OPPONENT);
	
		board.insert(3, OPPONENT);
		board.undo_insert();
	
	board.insert(3, OPPONENT);
	board.insert(0, THIS_PLAYER);
	board.insert(1, OPPONENT);
	
		board.insert(2, OPPONENT);
		board.undo_insert();
	
	board.insert(2, OPPONENT);
	
		board.insert(5, OPPONENT);
		board.undo_insert();
	
	board.insert(5, OPPONENT);
	board.insert(5, THIS_PLAYER);
	
		board.insert(3, THIS_PLAYER);
		board.undo_insert();

	//Ahora armamos las lineas tal como deben ser
	std::vector<std::list<Board::Linea> > horizontales = 
	{
		{{Board::Posicion(0,0), 3, THIS_PLAYER, true, true}, {Board::Posicion(0,3), 3, OPPONENT, true, true}},
		{{Board::Posicion(1,0), 2, THIS_PLAYER, true, true}, {Board::Posicion(1,2), 1, OPPONENT, true, false}, {Board::Posicion(1,5), 1, OPPONENT, false, true}},
		{{Board::Posicion(2,1), 1, OPPONENT, false, false}, {Board::Posicion(2,5), 1, THIS_PLAYER, false, true}},
		{}
	};

	std::vector<std::list<Board::Linea> > verticales = 
	{
		{{Board::Posicion(0,0), 2, THIS_PLAYER, true, false}},
		{{Board::Posicion(0,1), 2, THIS_PLAYER, true, true}, {Board::Posicion(2,1), 1, OPPONENT, true, false}},
		{{Board::Posicion(0,2), 1, THIS_PLAYER, true, true}, {Board::Posicion(1,2), 1, OPPONENT, true, false}},
		{{Board::Posicion(0,3), 1, OPPONENT, true, false}},
		{{Board::Posicion(0,4), 1, OPPONENT, true, false}},
		{{Board::Posicion(0,5), 2, OPPONENT, true, true}, {Board::Posicion(2,5), 1, THIS_PLAYER, true, false}}
	};
	
	std::vector<std::list<Board::Linea> > diagonales1 = 
	{
		{{Board::Posicion(0,0), 1, THIS_PLAYER, true, true}},
		{{Board::Posicion(1,0), 2, THIS_PLAYER, true, true}},
		{{Board::Posicion(1,1), 2, THIS_PLAYER, false, true}},
		{{Board::Posicion(2,1), 3, OPPONENT, false, true}},
		{{Board::Posicion(0,4), 1, OPPONENT, false, true}},
		{{Board::Posicion(0,5), 1, OPPONENT, false, true}},
		{{Board::Posicion(1,5), 1, OPPONENT, false, true}},
		{{Board::Posicion(2,5), 1, THIS_PLAYER, false, true}},
		{}
	};
	
	std::vector<std::list<Board::Linea> > diagonales2 = 
	{
		{{Board::Posicion(0,5), 1, OPPONENT, true, true}},
		{{Board::Posicion(0,4), 2, OPPONENT, true, true}},
		{{Board::Posicion(0,3), 1, OPPONENT, true, false}, {Board::Posicion(2,5), 1, THIS_PLAYER, false, true}},
		{{Board::Posicion(0,2), 1, THIS_PLAYER, true, false}},
		{{Board::Posicion(0,1), 1, THIS_PLAYER, true, true}, {Board::Posicion(1,2), 1, OPPONENT, true, false}},
		{{Board::Posicion(0,0), 2, THIS_PLAYER, true, false}},
		{{Board::Posicion(1,0), 1, THIS_PLAYER, true, true}, {Board::Posicion(2,1), 1, OPPONENT, true, false}},
		{},
		{}
	};
	
	bool pasa_test = true;
	
	//Chequear que las lineas son correctas
	EXPECT(horizontales[0] == board.lineas[HORIZONTAL][0]);
	EXPECT(horizontales[1] == board.lineas[HORIZONTAL][1]);
	EXPECT(horizontales[2] == board.lineas[HORIZONTAL][2]);
	EXPECT(horizontales[3] == board.lineas[HORIZONTAL][3]);
	
	EXPECT(verticales[0] == board.lineas[VERTICAL][0]);
	EXPECT(verticales[1] == board.lineas[VERTICAL][1]);
	EXPECT(verticales[2] == board.lineas[VERTICAL][2]);
	EXPECT(verticales[3] == board.lineas[VERTICAL][3]);
	EXPECT(verticales[4] == board.lineas[VERTICAL][4]);
	EXPECT(verticales[5] == board.lineas[VERTICAL][5]);
	
	EXPECT(diagonales1[0] == board.lineas[DIAGONAL_1][0]);
	EXPECT(diagonales1[1] == board.lineas[DIAGONAL_1][1]);
	EXPECT(diagonales1[2] == board.lineas[DIAGONAL_1][2]);
	EXPECT(diagonales1[3] == board.lineas[DIAGONAL_1][3]);
	EXPECT(diagonales1[4] == board.lineas[DIAGONAL_1][4]);
	EXPECT(diagonales1[5] == board.lineas[DIAGONAL_1][5]);
	EXPECT(diagonales1[6] == board.lineas[DIAGONAL_1][6]);
	EXPECT(diagonales1[7] == board.lineas[DIAGONAL_1][7]);
	EXPECT(diagonales1[8] == board.lineas[DIAGONAL_1][8]);
	
	EXPECT(diagonales2[0] == board.lineas[DIAGONAL_2][0]);
	EXPECT(diagonales2[1] == board.lineas[DIAGONAL_2][1]);
	EXPECT(diagonales2[2] == board.lineas[DIAGONAL_2][2]);
	EXPECT(diagonales2[3] == board.lineas[DIAGONAL_2][3]);
	EXPECT(diagonales2[4] == board.lineas[DIAGONAL_2][4]);
	EXPECT(diagonales2[5] == board.lineas[DIAGONAL_2][5]);
	EXPECT(diagonales2[6] == board.lineas[DIAGONAL_2][6]);
	EXPECT(diagonales2[7] == board.lineas[DIAGONAL_2][7]);
	EXPECT(diagonales2[8] == board.lineas[DIAGONAL_2][8]);
	
	// ------
	// -o---x
	// xxo--o
	// xxxooo
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if (i == board.free_boxes[j])
			{
				EXPECT(!board.puede_formar_linea_c<HORIZONTAL>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<VERTICAL>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_1>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_2>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<HORIZONTAL>(PLAYER_2, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<VERTICAL>(PLAYER_2, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_1>(PLAYER_2, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_2>(PLAYER_2, Board::Posicion(i,j)));
			}
		}
	}
	
	board.insert(0, PLAYER_2);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if (i == board.free_boxes[j] and j != 0)
			{
				EXPECT(!board.puede_formar_linea_c<HORIZONTAL>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<VERTICAL>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_1>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_2>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<HORIZONTAL>(PLAYER_2, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<VERTICAL>(PLAYER_2, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_1>(PLAYER_2, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_2>(PLAYER_2, Board::Posicion(i,j)));
			}
			if (i == board.free_boxes[j] and j == 0)
			{
				EXPECT(!board.puede_formar_linea_c<HORIZONTAL>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<VERTICAL>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_1>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_2>(PLAYER_1, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<HORIZONTAL>(PLAYER_2, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<VERTICAL>(PLAYER_2, Board::Posicion(i,j)));
				EXPECT(board.puede_formar_linea_c<DIAGONAL_1>(PLAYER_2, Board::Posicion(i,j)));
				EXPECT(!board.puede_formar_linea_c<DIAGONAL_2>(PLAYER_2, Board::Posicion(i,j)));
			}
		}
	}
	
	board.undo_insert();

	Parametros ps = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0};
	EXPECT(board.evaluar(PLAYER_1, ps) == (double)(2-3+8-14));


	if (pasa_test) cerr << "Pasa el test." << endl;

	return 0;
}
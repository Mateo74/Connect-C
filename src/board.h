#ifndef BOARD_H
#define BOARD_H

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <list>
#include <vector>

//Para identificar que hay en una casilla del tablero
enum Player
{
	PLAYER_1,
	PLAYER_2,
	NONE
};

//Los cuatro tipos de linea que existen.
//Se usan como template parameters para casi todas las funciones
//del tablero, lo cual nos ahorra de tener que hacer cosas como
//if (tipo == HORIZONTAL) ... else if (tipo == VERTICAL) ... else if ...
enum TipoLinea
{
	HORIZONTAL,
	VERTICAL,
	DIAGONAL_1, //Significa que va de arriba-izquierda hacia abajo-derecha
	DIAGONAL_2  //Significa que va de abajo-izquierda hacia arriba-derecha
};

//Para representar diferentes direcciones. Tambien se usan como 
//template parameters
enum Direccion
{
	IZQUIERDA,
	DERECHA,
	ARRIBA,
	ABAJO,
	ARRIBA_IZQUIERDA,
	ARRIBA_DERECHA,
	ABAJO_IZQUIERDA,
	ABAJO_DERECHA
};

struct Parametros
{
	double lineas_c_player;
	double lineas_c_oponente;
	double maxima_longitud_player;
	double maxima_longitud_oponente;
	double cant_lineas_player;
	double cant_lineas_oponente;
};

//Clase para representar tablero.
class Board
{
public:
	//Crea tablero vacio con filas y columnas dadas y valor de C dado.
	Board(int rows, int columns, int c);
	
	//Cantidad de filas
	int rows() const {return board.size();}
	//Cantidad de columnas
	int columns() const {return board[0].size();}
	//Cantidad de lugares ocupados de una columna
	int colsize(int column) {return free_boxes[column];}

	//Agrega ficha del jugador 'player' (player != NONE) a la columna 'column' 
	void insert(int column, Player player);
	//Deshace la ultima insercion realizada
	void undo_insert();
	//Evalua el tablero y devuelve que tan bueno es, para 'player'
	double evaluar(Player player, const Parametros& params);
	//Devuelve true si en el tablero actual 'player' es ganador
	bool ganador(Player player);
	
public:

	//Representa una posicion cualquiera en el tablero
	struct Posicion
	{
		Posicion() {}
		Posicion(int i, int j): i(i), j(j) {}
		
		//Mueve la posicion 'k' veces en la direccion indicada por 'dir'.
		template<Direccion dir> Posicion mover(int k = 1) const;
		
		bool operator==(const Posicion& otra) const {
			return i == otra.i and j == otra.j;
		}
		bool operator!=(const Posicion& otra) const {
			return i != otra.i or j != otra.j;
		}
		int i, j; //Fila, columna
	};

	//Guarda informacion sobre una linea cualquiera.
	struct Linea
	{
		//En que posicion empieza. Para lineas horizontales y diagonales esta posicion
		//es el extremo izquierdo de la linea. Para verticales es el extremo inferior.
		Posicion posicion;
		
		int longitud; //Su longitud
		Player player; //A que jugador pertenece
		bool bloqueada_izquierda, bloqueada_derecha;
		
		//Devuelve una clave de ordenamiento determinada a partir de su posicion.
		//El tablero mantiene las lineas ordenadas en base a esto.
		template<TipoLinea tipo> int clave() const;
		bool bloqueada() const {return bloqueada_izquierda and bloqueada_derecha;}
		
		bool operator==(const Linea& otra) const {
			return
			posicion == otra.posicion and 
			longitud == otra.longitud and
			player == otra.player and 
			bloqueada_izquierda == otra.bloqueada_izquierda and
			bloqueada_derecha == otra.bloqueada_derecha;
		}
	};

	//En el codigo las lineas se van a manipular principalmente con iteradores de listas.
	//Es como si en el struct Linea hubieramos agregado los campos "Linea* prev" y "Linea* next",
	//transformandolo en el nodo de una lista enlazada
	typedef std::list<Linea>::iterator ItLinea;

	//Los datos presentes en cada casilla del tablero
	struct BoardEntry
	{
		BoardEntry(Player player): player(player) {}
		
		//En caso de pertenecer a una linea de tipo T, padre[T] apunta
		//a la casilla "padre", que es de la misma línea y puede ser la posicion 
		//de la línea (en cuyo caso el padre es NULL) o tener a su vez otra casilla 
		//padre. Es como en disjoint sets con árboles.
		BoardEntry* padre[4];
		
		//En caso de ser la posición de una línea de tipo T, linea[T] es un 
		//iterador a esa línea.
		ItLinea linea[4];
		
		//Qué jugador ocupa la casilla.
		Player player;
	};

	//Actualiza líneas de tipo 'tipo' sabiendo que en 'pos' se agregó una ficha nueva.
	template<TipoLinea tipo> void actualizar_lineas(const Posicion& pos);
	//Deshace los cambios hechos a las líneas de tipo 'tipo' causados por la última inserción.
	template<TipoLinea tipo> void undo_insert();
	//Dada una línea de tipo 'tipo' asegura que todas las casillas que le pertenecen 
	//sean realmente de ella.
	template<TipoLinea tipo> void reconstruir_linea(ItLinea linea);
	template<TipoLinea tipo> bool chequear_bloqueos_izquierda(const Posicion& pos);
	template<TipoLinea tipo> bool chequear_bloqueos_derecha(const Posicion& pos);

	template<TipoLinea tipo> bool puede_formar_linea_c(Player player, Posicion pos);
	
	template<TipoLinea tipo> std::list<Linea>& lista_de_lineas(const Posicion& p);
	template<TipoLinea tipo> ItLinea obtener_linea(const Posicion& p);
	template<TipoLinea tipo> BoardEntry* representante_linea(const Posicion& p);
	template<TipoLinea tipo> BoardEntry* representante_linea(BoardEntry* entry);
	
	template<Direccion dir> bool tiene_vecino(const Posicion& p, Player target) const;
	
	bool posicion_valida(const Posicion& p) const;
	BoardEntry& casilla(const Posicion& p);

	
	std::vector<std::vector<BoardEntry> > board; //El tablero propiamente dicho
	std::vector<int> free_boxes; //Indices de las casillas libres por cada columna
	std::vector<std::list<Linea> > lineas[4]; //Lineas actuales del tablero
	int c;
	
	Posicion ultima_pos; //Posición donde se insertó la última ficha
	std::vector<std::pair<ItLinea, Linea> > lineas_viejas[4];
	std::pair<Linea, ItLinea> linea_eliminada[4];
	bool se_agrego_linea[4];
	bool se_elimino_linea[4];

	template<TipoLinea tipo>
	void backup_linea(ItLinea linea) {
		lineas_viejas[tipo].push_back(std::make_pair(linea, *linea));
	}

	template<TipoLinea tipo>
	void backup_linea_a_eliminar(ItLinea linea) {
		linea_eliminada[tipo] = std::make_pair(*linea, std::next(linea));
	}
};

#include "board.hpp"

#endif
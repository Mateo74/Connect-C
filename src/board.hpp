template<TipoLinea tipo> constexpr Direccion direccion1();
template<TipoLinea tipo> constexpr Direccion direccion2();

/*---------- lista_de_lineas ----------*/
//Esto lo pongo acá porque si no no compila
template<>
inline std::list<Board::Linea>& Board::lista_de_lineas<HORIZONTAL>(const Posicion& p) {
	return lineas[HORIZONTAL][p.i];
}
template<>
inline std::list<Board::Linea>& Board::lista_de_lineas<VERTICAL>(const Posicion& p) {
	return lineas[VERTICAL][p.j];
}
template<>
inline std::list<Board::Linea>& Board::lista_de_lineas<DIAGONAL_1>(const Posicion& p) {
	return lineas[DIAGONAL_1][p.i + p.j];
}
template<>
inline std::list<Board::Linea>& Board::lista_de_lineas<DIAGONAL_2>(const Posicion& p) {
	return lineas[DIAGONAL_2][columns() - 1 + p.i - p.j];
}


inline Board::Board(int rows, int columns, int c):
	board(rows, std::vector<BoardEntry>(columns, BoardEntry(NONE))),
	free_boxes(columns, 0),
	c(c)
{
	lineas[HORIZONTAL] = std::vector<std::list<Linea> >(rows);
	lineas[VERTICAL] = std::vector<std::list<Linea> >(columns);
	lineas[DIAGONAL_1] = std::vector<std::list<Linea> >(rows + columns - 1);
	lineas[DIAGONAL_2] = std::vector<std::list<Linea> >(rows + columns - 1);
}

inline void Board::insert(int column, Player player)
{
	for (int i = 0; i < 4; i++)
	{
		lineas_viejas[i].clear();
		se_agrego_linea[i] = false;
		se_elimino_linea[i] = false;
	}

	board[free_boxes[column]][column].player = player;
	free_boxes[column]++;
	
	int i = free_boxes[column] - 1;
	int j = column;
	Posicion pos(i,j);
	
	actualizar_lineas<HORIZONTAL>(pos);
	actualizar_lineas<VERTICAL>(pos);
	actualizar_lineas<DIAGONAL_1>(pos);
	actualizar_lineas<DIAGONAL_2>(pos);
	
	ultima_pos = pos;
}

inline void Board::undo_insert()
{
	undo_insert<HORIZONTAL>();
	undo_insert<VERTICAL>();
	undo_insert<DIAGONAL_1>();
	undo_insert<DIAGONAL_2>();

	casilla(ultima_pos).player = NONE;
	free_boxes[ultima_pos.j]--;
}

inline bool Board::ganador(Player player)
{
	for (int i = 0; i < 4; i++)
	{
		for (auto& lista : lineas[i])
		{
			for (auto& linea : lista)
			{
				if (linea.player == player and linea.longitud >= c)
				{
					return true;
				}
			}
		}
	}
	return false;
}


template<TipoLinea tipo>
inline void Board::actualizar_lineas(const Posicion& pos)
{
	constexpr Direccion dir1 = direccion1<tipo>();
	constexpr Direccion dir2 = direccion2<tipo>();
	
	bool vecino_dir1 = tiene_vecino<dir1>(pos, casilla(pos).player);
	bool vecino_dir2 = tiene_vecino<dir2>(pos, casilla(pos).player);
	
	if (vecino_dir1 and vecino_dir2)
	{		
		BoardEntry* rep1 = representante_linea<tipo>(pos.mover<dir1>());
		BoardEntry* rep2 = &casilla(pos.mover<dir2>()); 
		ItLinea linea1 = rep1->linea[tipo];
		ItLinea linea2 = rep2->linea[tipo];
		
		backup_linea<tipo>(linea1);
		backup_linea_a_eliminar<tipo>(linea2);
		
		linea1->longitud += 1 + linea2->longitud;
		linea1->bloqueada_derecha = linea2->bloqueada_derecha;
		
		lista_de_lineas<tipo>(pos).erase(linea2);
		
		rep2->padre[tipo] = rep1;
		casilla(pos).padre[tipo] = rep1;
		
		se_elimino_linea[tipo] = true;
	}
	else if (vecino_dir1)
	{
		BoardEntry* rep1 = representante_linea<tipo>(pos.mover<dir1>());
		ItLinea linea1 = rep1->linea[tipo];
		
		backup_linea<tipo>(linea1);
		
		linea1->longitud++;
		linea1->bloqueada_derecha = chequear_bloqueos_derecha<tipo>(pos);
		
		casilla(pos).padre[tipo] = rep1;
	}
	else if (vecino_dir2)
	{
		BoardEntry* rep2 = &casilla(pos.mover<dir2>());
		ItLinea linea2 = rep2->linea[tipo];
		
		backup_linea<tipo>(linea2);
		
		linea2->posicion = linea2->posicion.mover<dir1>();
		linea2->longitud++;
		linea2->bloqueada_izquierda = chequear_bloqueos_izquierda<tipo>(pos);
		
		rep2->padre[tipo] = &casilla(pos);
		casilla(pos).linea[tipo] = linea2;
		casilla(pos).padre[tipo] = NULL;
	}
	else
	{
		Linea nueva_linea;
		nueva_linea.posicion = pos;
		nueva_linea.longitud = 1;
		nueva_linea.player = casilla(pos).player;
		nueva_linea.bloqueada_izquierda = chequear_bloqueos_izquierda<tipo>(pos);
		nueva_linea.bloqueada_derecha = chequear_bloqueos_derecha<tipo>(pos);
		
		std::list<Linea>& lista = lista_de_lineas<tipo>(pos);
		int clave = nueva_linea.clave<tipo>();
		auto it = lista.begin();
		while (it != lista.end() and it->clave<tipo>() <= clave) ++it;
		casilla(pos).linea[tipo] = lista.insert(it, nueva_linea);
		casilla(pos).padre[tipo] = NULL;
		
		se_agrego_linea[tipo] = true;
	}
}

template<TipoLinea tipo>
inline void Board::undo_insert()
{
	if (se_elimino_linea[tipo])
	{
		auto& lista = lista_de_lineas<tipo>(ultima_pos);
		auto it = lista.insert(linea_eliminada[tipo].second, linea_eliminada[tipo].first);
		reconstruir_linea<tipo>(it);
	}
	else if (se_agrego_linea[tipo])
	{
		auto& lista = lista_de_lineas<tipo>(ultima_pos);
		lista.erase(obtener_linea<tipo>(ultima_pos));
	}

	for (unsigned i = 0; i < lineas_viejas[tipo].size(); i++)
	{
		*lineas_viejas[tipo][i].first = lineas_viejas[tipo][i].second;
		reconstruir_linea<tipo>(lineas_viejas[tipo][i].first);
	}
}

template<TipoLinea tipo>
inline void Board::reconstruir_linea(ItLinea linea)
{
	constexpr Direccion dir2 = direccion2<tipo>();
	BoardEntry* rep = &casilla(linea->posicion);
	rep->player = linea->player;
	rep->linea[tipo] = linea;
	rep->padre[tipo] = NULL;
	Posicion p = linea->posicion.mover<dir2>();
	for (int k = 1; k < linea->longitud; k++)
	{
		casilla(p).player = linea->player;
		casilla(p).padre[tipo] = rep;
		p = p.mover<dir2>();
	}
}

template<TipoLinea tipo>
inline bool Board::chequear_bloqueos_izquierda(const Posicion& pos)
{
	constexpr Direccion dir1 = direccion1<tipo>();
	Posicion p1 = pos.mover<dir1>();
	if (posicion_valida(p1) and casilla(p1).player == NONE)
	{
		return false;
	}
	else if (posicion_valida(p1))
	{
		ItLinea linea = obtener_linea<tipo>(p1);
		backup_linea<tipo>(linea);
		linea->bloqueada_derecha = true;
		return true;
	}
	else
	{
		return true;
	}
}

template<TipoLinea tipo>
inline bool Board::chequear_bloqueos_derecha(const Posicion& pos)
{
	constexpr Direccion dir2 = direccion2<tipo>();
	Posicion p2 = pos.mover<dir2>();
	if (posicion_valida(p2) and casilla(p2).player == NONE)
	{
		return false;
	}
	else if (posicion_valida(p2))
	{
		ItLinea linea = obtener_linea<tipo>(p2);
		backup_linea<tipo>(linea);
		linea->bloqueada_izquierda = true;
		return true;
	}
	else
	{
		return true;
	}
}

/*--------------------------------------------------------------------------------*/
/* Metodos relacionados con evaluar el tablero */

inline double Board::evaluar(Player player, const Parametros& params)
{
	Player oponente = (player == PLAYER_1) ? PLAYER_2 : PLAYER_1;
	int lineas_c_nuestras = 0;
	int lineas_c_oponente = 0;
	int maxima_longitud_nuestra = 0;
	int maxima_longitud_oponente = 0;
	int lineas_nuestras_no_bloqueadas = 0;
	int lineas_oponente_no_bloqueadas = 0;
	
	for (int i = 0; i < 4; i++) {
		for (auto& lista : lineas[i]) {
			for (ItLinea linea = lista.begin(); linea != lista.end(); ++linea) {
				if (linea->player == player) {
					if (linea->longitud >= c) lineas_c_nuestras++;
					if (!linea->bloqueada()) {
						lineas_nuestras_no_bloqueadas++;
						if (linea->longitud > maxima_longitud_nuestra) maxima_longitud_nuestra = linea->longitud;
					}
				}
			
				else {
					if (!linea->bloqueada()) {
						lineas_oponente_no_bloqueadas++;
						if (linea->longitud > maxima_longitud_oponente) maxima_longitud_oponente = linea->longitud;
					}
				}
			}
		}
	}
	
	//Contar cantidad de lugares libres donde oponente puede formar linea de longitud c
	for (unsigned j = 0; j < board.size(); j++)
	{
		Posicion pos(free_boxes[j], j);
		if (posicion_valida(pos) and casilla(pos).player == NONE)
		{
			if (puede_formar_linea_c<HORIZONTAL>(oponente, pos)) lineas_c_oponente++;
			if (puede_formar_linea_c<VERTICAL>(oponente, pos)) lineas_c_oponente++;
			if (puede_formar_linea_c<DIAGONAL_1>(oponente, pos)) lineas_c_oponente++;
			if (puede_formar_linea_c<DIAGONAL_2>(oponente, pos)) lineas_c_oponente++;
		}
	}
	
	//Calculo puntaje total del tablero
	double puntaje = 0.0;
	puntaje += params.lineas_c_player * lineas_c_nuestras;
	puntaje += params.lineas_c_oponente * lineas_c_oponente;
	puntaje += params.maxima_longitud_player * maxima_longitud_nuestra;
	puntaje += params.maxima_longitud_oponente * maxima_longitud_oponente;
	puntaje += params.cant_lineas_player * lineas_nuestras_no_bloqueadas;
	puntaje += params.cant_lineas_oponente * lineas_oponente_no_bloqueadas;
	return puntaje;
}

template<TipoLinea tipo>
inline bool Board::puede_formar_linea_c(Player player, Posicion pos)
{
	constexpr Direccion dir1 = direccion1<tipo>();
	constexpr Direccion dir2 = direccion2<tipo>();
	
	int potencial_longitud = 1;
	Posicion p1 = pos.mover<dir1>();
	Posicion p2 = pos.mover<dir2>();
	if (posicion_valida(p1) and casilla(p1).player == player)
	{
		potencial_longitud += obtener_linea<tipo>(p1)->longitud;
	}
	if (posicion_valida(p2) and casilla(p2).player == player)
	{
		potencial_longitud += obtener_linea<tipo>(p2)->longitud;
	}
	
	return potencial_longitud >= c;
}

template<TipoLinea tipo>
inline Board::ItLinea Board::obtener_linea(const Posicion& p) {
	return representante_linea<tipo>(p)->linea[tipo];
}

template<TipoLinea tipo>
inline Board::BoardEntry* Board::representante_linea(const Posicion& p) {
	return representante_linea<tipo>(&casilla(p));
}

template<TipoLinea tipo>
inline Board::BoardEntry* Board::representante_linea(BoardEntry* entry) {
	if (entry->padre[tipo] == NULL) {
		return entry;
	} else {
		return entry->padre[tipo] = representante_linea<tipo>(entry->padre[tipo]);
	}
}

inline bool Board::posicion_valida(const Posicion& p) const {
	return 0 <= p.i and p.i < rows() and 0 <= p.j and p.j < columns();
}

inline Board::BoardEntry& Board::casilla(const Posicion& p) {
	return board[p.i][p.j];
}

// ================================================================================
// Especializaciones de varias funciones auxiliares

/*---------- direccion1 y direccion2 ----------*/
template<> constexpr Direccion direccion1<HORIZONTAL>() {return IZQUIERDA;}
template<> constexpr Direccion direccion2<HORIZONTAL>() {return DERECHA;}

template<> constexpr Direccion direccion1<VERTICAL>() {return ABAJO;}
template<> constexpr Direccion direccion2<VERTICAL>() {return ARRIBA;}

template<> constexpr Direccion direccion1<DIAGONAL_1>() {return ARRIBA_IZQUIERDA;}
template<> constexpr Direccion direccion2<DIAGONAL_1>() {return ABAJO_DERECHA;}

template<> constexpr Direccion direccion1<DIAGONAL_2>() {return ABAJO_IZQUIERDA;}
template<> constexpr Direccion direccion2<DIAGONAL_2>() {return ARRIBA_DERECHA;}

/*---------- tiene_vecino ----------*/
template<>
inline bool Board::tiene_vecino<IZQUIERDA>(const Posicion& p, Player target) const {
	int i = p.i, j = p.j;
	return j >= 1 and board[i][j-1].player == target;
}
template<>
inline bool Board::tiene_vecino<DERECHA>(const Posicion& p, Player target) const {
	int i = p.i, j = p.j;
	return j+1 < columns() and board[i][j+1].player == target;
}
template<>
inline bool Board::tiene_vecino<ARRIBA>(const Posicion& p, Player target) const {
	int i = p.i, j = p.j;
	return i+1 < rows() and board[i+1][j].player == target;
}
template<>
inline bool Board::tiene_vecino<ABAJO>(const Posicion& p, Player target) const {
	int i = p.i, j = p.j;
	return i >= 1 and board[i-1][j].player == target;
}
template<>
inline bool Board::tiene_vecino<ARRIBA_IZQUIERDA>(const Posicion& p, Player target) const {
	int i = p.i, j = p.j;
	return i+1 < rows() and j >= 1 and board[i+1][j-1].player == target;
}
template<>
inline bool Board::tiene_vecino<ARRIBA_DERECHA>(const Posicion& p, Player target) const {
	int i = p.i, j = p.j;
	return i+1 < rows() and j+1 < columns() and board[i+1][j+1].player == target;
}
template<>
inline bool Board::tiene_vecino<ABAJO_IZQUIERDA>(const Posicion& p, Player target) const {
	int i = p.i, j = p.j;
	return i >= 1 and j >= 1 and board[i-1][j-1].player == target;
}
template<>
inline bool Board::tiene_vecino<ABAJO_DERECHA>(const Posicion& p, Player target) const {
	int i = p.i, j = p.j;
	return i >= 1 and j+1 < columns() and board[i-1][j+1].player == target;
}

/*---------- Posicion::mover ----------*/
template<>
inline Board::Posicion Board::Posicion::mover<IZQUIERDA>(int k) const {
	return Posicion(i,j-k);
}
template<>
inline Board::Posicion Board::Posicion::mover<DERECHA>(int k) const {
	return Posicion(i,j+k);
}
template<>
inline Board::Posicion Board::Posicion::mover<ARRIBA>(int k) const {
	return Posicion(i+k,j);
}
template<>
inline Board::Posicion Board::Posicion::mover<ABAJO>(int k) const {
	return Posicion(i-k,j);
}
template<>
inline Board::Posicion Board::Posicion::mover<ARRIBA_IZQUIERDA>(int k) const {
	return Posicion(i+k,j-k);
}
template<>
inline Board::Posicion Board::Posicion::mover<ARRIBA_DERECHA>(int k) const {
	return Posicion(i+k,j+k);
}
template<>
inline Board::Posicion Board::Posicion::mover<ABAJO_IZQUIERDA>(int k) const {
	return Posicion(i-k,j-k);
}
template<>
inline Board::Posicion Board::Posicion::mover<ABAJO_DERECHA>(int k) const {
	return Posicion(i-k,j+k);
}

/*---------- Linea::clave ----------*/
template<>
inline int Board::Linea::clave<HORIZONTAL>() const {
	return posicion.j;
}
template<>
inline int Board::Linea::clave<VERTICAL>() const {
	return posicion.i;
}
template<>
inline int Board::Linea::clave<DIAGONAL_1>() const {
	return posicion.j;
}
template<>
inline int Board::Linea::clave<DIAGONAL_2>() const {
	return posicion.j;
}

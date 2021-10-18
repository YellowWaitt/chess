#include "chessgame960.h"

#include <list>

#include "util/random.h"
#include "util/sgn.h"


inline void getCastleSquares(Square& kingS, Square& rookS, int y, int side) {
	if (side < 0) {
		kingS.x = 2; kingS.y = y;
		rookS.x = 3; rookS.y = y;
	} else {
		kingS.x = 6; kingS.y = y;
		rookS.x = 5; rookS.y = y;
	}
}

/******************************************************************************/

ChessGame960::ChessGame960(std::unique_ptr<Chessboard> board)
: ChessGame()
{
	this->board = std::move(board);
	start();
}

std::shared_ptr<ChessGame> ChessGame960::clone() const {
	return std::make_shared<ChessGame960>(*this);
}

/******************************************************************************/

#define fillLists(start, stop) \
	for (int i = start; i < stop; ++i) { \
		if (i % 2 == 0) { \
			blackCells.push_back(i); \
		} else { \
			whiteCells.push_back(i); \
		} \
	}
void ChessGame960::start() {
	PieceName layout[8];
	Random rand;
	
	layout[colStart.leftRook = rand.uniform(0, 5)] = PieceName::Rook;
	layout[colStart.king = rand.uniform(colStart.leftRook + 1, 6)] = PieceName::King;
	layout[colStart.rightRook = rand.uniform(colStart.king + 1, 7)] = PieceName::Rook;
	
	std::list<int> whiteCells;
	std::list<int> blackCells;
	
	fillLists(0, colStart.leftRook);
	fillLists(colStart.leftRook + 1, colStart.king);
	fillLists(colStart.king + 1, colStart.rightRook);
	fillLists(colStart.rightRook + 1, 8);
	
	layout[rand.extract(whiteCells)] = PieceName::Bishop;
	layout[rand.extract(blackCells)] = PieceName::Bishop;
	
	whiteCells.splice(whiteCells.end(), blackCells);
	
	layout[rand.extract(whiteCells)] = PieceName::Queen;
	layout[rand.extract(whiteCells)] = PieceName::Knight;
	layout[rand.extract(whiteCells)] = PieceName::Knight;
	
	std::string pos = "";
	for (int i = 0; i < 8; ++i) {
		pos += charFromPiece({layout[i], Color::Black});
	}
	pos += "/pppppppp/8/8/8/8/PPPPPPPP/";
	for (int i = 0; i < 8; ++i) {
		pos += charFromPiece({layout[i], Color::White});
	}
	
	startFrom(pos, Color::White);
}
#undef fillLists

/******************************************************************************/

void ChessGame960::castle(Move const& move) {
	Square kingS, rookS;
	getCastleSquares(kingS, rookS, move.from.y, move.to.x - move.from.x);
	board->clear(move.from);
	board->clear(move.to);
	board->setPiece(kingS, {PieceName::King, turn});
	board->setPiece(rookS, {PieceName::Rook, turn});
}

void ChessGame960::uncastle(Move const& move) {
	Square kingS, rookS;
	getCastleSquares(kingS, rookS, move.from.y, move.to.x - move.from.x);
	board->clear(kingS);
	board->clear(rookS);
	board->setPiece(move.from, {PieceName::King, turn});
	board->setPiece(move.to, {PieceName::Rook, turn});
}

/******************************************************************************/

void ChessGame960::getValidKingMoves(std::vector<Move>& moves, Square const& square) const {
	Move move;
	move.from = square;
	for (move.to.x = std::max(square.x - 1, 0); move.to.x <= std::min(square.x + 1, 7); ++move.to.x) {
		for (move.to.y = std::max(square.y - 1, 0); move.to.y <= std::min(square.y + 1, 7); ++move.to.y) {
			if (isValidKing(move) and kingSafeAfter(move)) {
				moves.push_back(move);
			}
		}
	}
	move.to.y = move.from.y;
	move.to.x = colStart.leftRook;
	if (isValidCastle(move)) {
		moves.push_back(move);
	}
	move.to.x = colStart.rightRook;
	if (isValidCastle(move)) {
		moves.push_back(move);
	}
}

/******************************************************************************/

bool ChessGame960::isCastle(Move const& move) const {
	return board->getPiece(move.from) == Piece{PieceName::King, turn}
		and board->getPiece(move.to) == Piece{PieceName::Rook, turn};
}

/******************************************************************************/

bool ChessGame960::krHasMoved(Move const& move, int side) const {
	int y = turn == Color::White ? 0 : 7;
	Square kingS = {colStart.king, y};
	Square rookS = {side > 0 ? colStart.rightRook : colStart.leftRook, y};
	Piece king = board->getPiece(kingS);
	Piece rook = board->getPiece(rookS);
	if (move.from != kingS or king.name != PieceName::King or king.color != turn or
			move.to != rookS or rook.name != PieceName::Rook or rook.color != turn) {
		return true;
	}
	for (Play const& play : historic) {
		if (play.move.from == kingS or play.move.from == rookS or play.move.to == rookS) {
			return true;
		}
	}
	return false;
}

/*----------------------------------------------------------------------------*/

bool ChessGame960::isValidCastle(Move const& move) const {
	Color opp = opposite(turn);
	int side = move.to.x - move.from.x;
	if (side == 0 or move.to.y != move.from.y or krHasMoved(move, side)
			or isControlled(move.from, opp)) {
		return false;
	}
	Square kingS, rookS;
	getCastleSquares(kingS, rookS, move.from.y, side);
	Square square = move.from;
	int dx = sgn(kingS.x - move.from.x);
	for (square.x += dx; square.x != kingS.x + dx; square.x += dx) {
		if (isControlled(square, opp) or (square != move.to and not board->isEmpty(square))) {
			return false;
		}
	}
	dx = sgn(rookS.x - move.to.x);
	for (square.x = move.to.x + dx; square.x != rookS.x + dx; square.x += dx) {
		if (square != move.from and not board->isEmpty(square)) {
			return false;
		}
	}
	return true;
}

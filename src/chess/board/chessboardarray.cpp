#include "chessboardarray.h"


/******************************************************************************/

ChessboardArray::ChessboardArray() {
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			board[i][j].name = PieceName::Empty;
		}
	}
}

std::shared_ptr<Chessboard> ChessboardArray::clone() const {
	return std::make_shared<ChessboardArray>(*this);
}

/******************************************************************************/

void ChessboardArray::clear(Square const& square) {
	board[square.x][square.y].name = PieceName::Empty;
}

/******************************************************************************/

bool ChessboardArray::isEmpty(Square const& square) const {
	return board[square.x][square.y].name == PieceName::Empty;
}

/******************************************************************************/

Piece ChessboardArray::getPiece(Square const& square) const {
	return board[square.x][square.y];
}

/******************************************************************************/

void ChessboardArray::setPiece(Square const& square, Piece const& piece) {
	board[square.x][square.y] = piece;
}

/******************************************************************************/

void ChessboardArray::move(Square const& from, Square const& to) {
	board[to.x][to.y] = board[from.x][from.y];
	board[from.x][from.y].name = PieceName::Empty;
}

/******************************************************************************/

bool ChessboardArray::operator==(Chessboard const& board) const {
	ChessboardArray const* cb = dynamic_cast<ChessboardArray const*>(&board);
	if (cb == NULL) {
		return false;
	}
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (this->board[i][j] != cb->board[i][j]) {
				return false;
			}
		}
	}
	return true;
}

/******************************************************************************/

size_t ChessboardArray::hash() const {
	size_t hash = 0;
	Square square;
	for (square.x = 0; square.x < 8; ++square.x) {
		for (square.y = 0; square.y < 8; ++square.y) {
			Piece const piece = board[square.x][square.y];
			hash <<= 1;
			if (piece.name != PieceName::Empty) {
				hash ^= (int(piece.name) << 1) + int(piece.color);
			}
		}
	}
	return hash;
}

#include "chessboardbits.h"

#define MASK       0b1111
#define MASK_NAME  0b0111
#define MASK_COLOR 0b1000

#define get(b, s)     ((b[s.y] & (MASK << (s.x << 2))) >> (s.x << 2))
#define set(b, m, s)  ((b[s.y] & ~(MASK << (s.x << 2))) | (m << (s.x << 2)))
#define clearAt(b, s)  b[s.y] &= ~(MASK << (s.x << 2))

/******************************************************************************/

ChessboardBits::ChessboardBits() {
	for (int i = 0; i < 8; ++i) {
		board[i] = 0;
	}
}

std::shared_ptr<Chessboard> ChessboardBits::clone() const {
	return std::make_shared<ChessboardBits>(*this);
}

/******************************************************************************/

void ChessboardBits::clear(Square const& square) {
	clearAt(board, square);
}

/******************************************************************************/

bool ChessboardBits::isEmpty(Square const& square) const {
	return (board[square.y] & (MASK << (square.x << 2))) == 0;
}

/******************************************************************************/

Piece ChessboardBits::getPiece(Square const& square) const {
	uint32_t bits = get(board, square);
	return {static_cast<PieceName>(bits & MASK_NAME),
			static_cast<Color>((bits & MASK_COLOR) >> 3)};
}

/******************************************************************************/

void ChessboardBits::setPiece(Square const& square, Piece const& piece) {
	uint32_t bits = (uint32_t(piece.color) << 3) | uint32_t(piece.name);
	board[square.y] = set(board, bits, square);
}

/******************************************************************************/

void ChessboardBits::move(Square const& from, Square const& to) {
	board[to.y] = set(board, get(board, from), to);
	clearAt(board, from);
}

/******************************************************************************/

bool ChessboardBits::operator==(Chessboard const& board) const {
	ChessboardBits const* cb = dynamic_cast<ChessboardBits const*>(&board);
	if (cb == NULL) {
		return false;
	}
	for (int y = 0; y < 8; ++y) {
		uint32_t diff = this->board[y] ^ cb->board[y];
		if (diff) {
			return false;
		}
	}
	return true;
}

/******************************************************************************/

size_t ChessboardBits::hash() const {
	size_t hash = 0;
	for (int i = 0; i < 8; ++i) {
		hash <<= 4;
		hash ^= board[i];
	}
	return hash;
}

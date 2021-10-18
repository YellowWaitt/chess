#include "chessboard.h"

#include <cctype>

#define EMPTYCHAR ' '

/******************************************************************************/

char charFromPiece(Piece const& piece) {
	switch (piece.name) {
		case PieceName::Pawn:
			return piece.color == Color::White ? 'P' : 'p';
		case PieceName::Rook:
			return piece.color == Color::White ? 'R' : 'r';
		case PieceName::Knight:
			return piece.color == Color::White ? 'N' : 'n';
		case PieceName::Bishop:
			return piece.color == Color::White ? 'B' : 'b';
		case PieceName::Queen:
			return piece.color == Color::White ? 'Q' : 'q';
		case PieceName::King:
			return piece.color == Color::White ? 'K' : 'k';
		default:
			return EMPTYCHAR;
	}
}

Piece pieceFromChar(char c) {
	Color color = isupper(c) ? Color::White : Color::Black;
	PieceName name;
	switch (c) {
		case 'P':
		case 'p':
			name = PieceName::Pawn;
			break;
		case 'R':
		case 'r':
			name = PieceName::Rook;
			break;
		case 'N':
		case 'n':
			name = PieceName::Knight;
			break;
		case 'B':
		case 'b':
			name = PieceName::Bishop;
			break;
		case 'Q':
		case 'q':
			name = PieceName::Queen;
			break;
		case 'K':
		case 'k':
			name = PieceName::King;
			break;
		default:
			name = PieceName::Empty;
	}
	return {name, color};
}

/******************************************************************************/

void Chessboard::start() {
	startFrom(STARTING_POSITION);
}

/******************************************************************************/

inline void incrSquare(Square& square) {
	int incr = square.x + 1;
	square.y -= incr / 8;
	square.x = incr % 8;
}

void Chessboard::startFrom(std::string const& pos) {
	Square square = {0, 7};
	for (char const& c : pos) {
		if (c == '/') {
			continue;
		}
		if (isdigit(c)) {
			for (int i = c - '0'; i > 0; --i) {
				clear(square);
				incrSquare(square);
			}
		} else {
			Piece piece = pieceFromChar(c);
			setPiece(square, piece);
			incrSquare(square);
		}
	}
}

/******************************************************************************/

std::string Chessboard::toString() const {
	std::string pos = "";
	Square square;
	for (square.y = 7; square.y >= 0; --square.y) {
		for (square.x = 0; square.x < 8; ++square.x) {
			Piece piece = getPiece(square);
			char c = charFromPiece(piece);
			if (c != EMPTYCHAR) {
				pos.push_back(c);
			} else {
				char nbEmpty = 0;
				do {
					++nbEmpty;
					++square.x;
					piece = getPiece(square);
				} while (square.x < 8 && charFromPiece(piece) == EMPTYCHAR);
				pos.push_back(nbEmpty + '0');
				if (square.x != 8) --square.x;
			}
		}
		if (square.y != 0) pos.push_back('/');
	}
	return pos;
}

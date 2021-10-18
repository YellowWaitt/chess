#pragma once

/******************************************************************************/

enum class PieceName {
	Empty,
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

/******************************************************************************/

enum class Color {
	White, Black
};

inline Color opposite(Color color) {
	return static_cast<Color>(1 - int(color));
}

/******************************************************************************/

struct Piece {
	PieceName name; Color color;
	
	inline bool operator==(Piece const& p) const {
		return p.name == name and (p.color == color or name == PieceName::Empty);
	}
	inline bool operator!=(Piece const& p) const {
		return p.name != name or (p.color != color and name != PieceName::Empty);
	}
};

char charFromPiece(Piece const& piece);
Piece pieceFromChar(char c);

/******************************************************************************/

struct Square {
	int x; int y;

	inline bool operator==(Square const& s) const {
		return s.x == x and s.y == y;
	}
	inline bool operator!=(Square const& s) const {
		return s.x != x or s.y != y;
	}
};

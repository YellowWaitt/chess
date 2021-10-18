#pragma once

#include "chessboard.h"


class ChessboardBits : public Chessboard {
	public:
		ChessboardBits();
		ChessboardBits(ChessboardBits const& board) = default;
		
		void clear(Square const& square);
		bool isEmpty(Square const& square) const;
		Piece getPiece(Square const& square) const;
		void setPiece(Square const& square, Piece const& piece);
		void move(Square const& from, Square const& to);
		
		std::shared_ptr<Chessboard> clone() const;
		size_t hash() const;
		bool operator==(Chessboard const& board) const;

	private:
		uint32_t board[8];
};

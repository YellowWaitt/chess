#pragma once

#include <memory>
#include <string>

#include "chess/types.h"

#define STARTING_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"


class Chessboard {
	public:
		void start();
		void startFrom(std::string const& pos);
		std::string toString() const;

		virtual void clear(Square const& square) = 0;
		virtual bool isEmpty(Square const& square) const = 0;
		virtual Piece getPiece(Square const& square) const = 0;
		virtual void setPiece(Square const& square, Piece const& piece) = 0;
		virtual void move(Square const& from, Square const& to) = 0;
				
		virtual std::shared_ptr<Chessboard> clone() const = 0;
		virtual size_t hash() const = 0;
		virtual bool operator==(Chessboard const& board) const = 0;
};

#pragma once

#include "chessgame.h"


class ChessGame960 : public ChessGame {
	public:
		ChessGame960(std::unique_ptr<Chessboard> board);
		ChessGame960(ChessGame960 const& game) = default;
		
		std::shared_ptr<ChessGame> clone() const override;
		bool isCastle(Move const& move) const override;
		void start() override;
	
	protected:
		struct {
			int king, leftRook, rightRook;
		} colStart;
	
		void castle(Move const& move) override;
		void getValidKingMoves(std::vector<Move>& moves, Square const& square) const override;
		bool isValidCastle(Move const& move) const override;
		bool krHasMoved(Move const& move, int side) const override;
		void uncastle(Move const& move) override;
};

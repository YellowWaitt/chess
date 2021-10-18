#pragma once

#include "chessgame.h"


class Horde : public ChessGame {
	public:
		Horde(std::unique_ptr<Chessboard> board);
		Horde(Horde const& game) = default;
		
		std::shared_ptr<ChessGame> clone() const override;
		void start() override;
		void startFrom(std::string const& pos, Color turn) override;
	
	protected:
		bool isControlled(Square const& square, Color color) const override;
		bool isValidPawn(Move const& move) const override;
		void updateResultat() override;
};

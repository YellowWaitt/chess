#pragma once

#include "chessgame.h"


class KingOfTheHill : public ChessGame {
	public:
		KingOfTheHill(std::unique_ptr<Chessboard> board);
		KingOfTheHill(KingOfTheHill const& game) = default;
		
		std::shared_ptr<ChessGame> clone() const override;
	
	protected:
		void updateResultat() override;
};

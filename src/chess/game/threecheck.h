#pragma once

#include "chessgame.h"


class ThreeCheck : public ChessGame {
	public:
		ThreeCheck(std::unique_ptr<Chessboard> board);
		ThreeCheck(ThreeCheck const& game) = default;
		
		int checkRemaining(Color color) const;
		
		std::shared_ptr<ChessGame> clone() const override;
		void startFrom(std::string const& pos, Color turn) override;
		void undo() override;
	
	protected:
		int checks[2];
		
		void updateResultat() override;
};

#include "threecheck.h"


ThreeCheck::ThreeCheck(std::unique_ptr<Chessboard> board)
: ChessGame()
{
	this->board = std::move(board);
	start();
}

std::shared_ptr<ChessGame> ThreeCheck::clone() const {
	return std::make_shared<ThreeCheck>(*this);
}

/*----------------------------------------------------------------------------*/

void ThreeCheck::startFrom(std::string const& pos, Color turn) {
	checks[0] = checks[1] = 0;
	ChessGame::startFrom(pos, turn);
}

/******************************************************************************/

int ThreeCheck::checkRemaining(Color color) const {
	return 3 - checks[size_t(color)];
}

/******************************************************************************/

void ThreeCheck::undo() {
	if (historic.empty()) {
		return;
	}
	
	if (isControlled(kings[size_t(turn)], opposite(turn))) {
		--checks[size_t(turn)];
	}
	ChessGame::undo();
}

/******************************************************************************/

void ThreeCheck::updateResultat() {
	if (isControlled(kings[size_t(turn)], opposite(turn))) {
		++checks[size_t(turn)];
		if (checks[size_t(turn)] == 3) {
			surrender(turn);
			return;
		}
	}
	ChessGame::updateResultat();
}

#include "kingofthehill.h"


KingOfTheHill::KingOfTheHill(std::unique_ptr<Chessboard> board)
: ChessGame(std::move(board))
{}

std::shared_ptr<ChessGame> KingOfTheHill::clone() const {
	return std::make_shared<KingOfTheHill>(*this);
}

/******************************************************************************/

void KingOfTheHill::updateResultat() {
	int color = int(opposite(turn));
	if (3 <= kings[color].x and kings[color].x <= 4 and
			3 <= kings[color].y and kings[color].y <= 4) {
		surrender(turn);
	} else {
		ChessGame::updateResultat();
	}
}

#include "horde.h"


Horde::Horde(std::unique_ptr<Chessboard> board)
: ChessGame()
{
	this->board = std::move(board);
	start();
}

std::shared_ptr<ChessGame> Horde::clone() const {
	return std::make_shared<Horde>(*this);
}

/******************************************************************************/

void Horde::start() {
	startFrom("rnbqkbnr/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP", Color::White);
}

void Horde::startFrom(std::string const& pos, Color turn) {
	ChessGame::startFrom(pos, turn);
	kings[int(Color::White)] = {100, 100};
}

/******************************************************************************/

bool Horde::isControlled(Square const& square, Color color) const {
	if (square == kings[int(Color::White)]) {
		return false;
	} else {
		return ChessGame::isControlled(square, color);
	}
}

/******************************************************************************/

bool Horde::isValidPawn(Move const& move) const {
	int dx = abs(move.to.x - move.from.x);
	int dy = move.to.y - move.from.y;
	if (dx == 0 and dy == 2 and move.from.y == 0) {
		return board->isEmpty(move.to) and board->isEmpty({move.to.x, 1});
	}
	return ChessGame::isValidPawn(move);
}

/******************************************************************************/

void Horde::updateResultat() {
	if (turn == Color::White) {
		Square square;
		for (square.x = 0; square.x < 8; ++square.x) {
			for (square.y = 0; square.y < 8; ++square.y) {
				if (not board->isEmpty(square)
						and board->getPiece(square).color == Color::White) {
					return ChessGame::updateResultat();
				}
			}
		}
		surrender(Color::White);
		return;
	}
	ChessGame::updateResultat();
}

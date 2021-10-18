#if 0

class MoveComparator {
	public:
		MoveComparator(std::shared_ptr<ChessGame> game) : game(game) {}
		
		bool operator()(Move const& move1, Move const& move2) {
			eval_t score1 = evalMove(move1);
			eval_t score2 = evalMove(move2);
			return game->getTurn() == Color::White ? score1 > score2 : score1 < score2;
		}
	
	private:
		std::shared_ptr<ChessGame> game;
		
		eval_t evalMove(Move const& move) {
			eval_t score;
			game->play(move);
			if (game->isOver()) {
				score = scoreWhenOver(game->getResultat());
			} else {
				score = materialScore(*game);
			}
			game->undo();
			return score;
		}
};

/******************************************************************************/

eval_t minmax(std::shared_ptr<ChessGame> game, int depth, std::vector<Move>& moves,
int& nbMoves) {
	if (game->isOver()) {
		return scoreWhenOver(game->getResultat());
	} else if (depth == 0) {
		return materialScore(*game);
	}
	
	Color gameTurn = game->getTurn();
	eval_t bestEval = gameTurn == Color::White ?
			std::numeric_limits<eval_t>::min() :
			std::numeric_limits<eval_t>::max();
	auto movesToTest = game->getValidMoves();
	std::vector<Move> movesTemp(depth);
	for (auto& move : *movesToTest) {
		++nbMoves;
		game->play(move);
		eval_t eval = minmax(game, depth - 1, movesTemp, nbMoves);
		game->undo();
		if (gameTurn == Color::White) {
			if (eval > bestEval) {
				bestEval = eval;
				insertBack(moves, movesTemp);
				moves[moves.size() - depth] = move;
			}
		} else {
			if (eval < bestEval) {
				bestEval = eval;
				insertBack(moves, movesTemp);
				moves[moves.size() - depth] = move;
			}
		}
	}
	
	return bestEval;
}

/******************************************************************************/

eval_t negamax(std::shared_ptr<ChessGame> game, int depth, std::vector<Move>& moves,
int& nbMoves) {
	if (game->isOver()) {
		return game->getTurn() * scoreWhenOver(game->getResultat());
	} else if (depth == 0) {
		return game->getTurn() * materialScore(*game);
	}
	
	eval_t bestEval = std::numeric_limits<eval_t>::min();
	auto movesToTest = game->getValidMoves();
	std::vector<Move> movesTemp(depth);
	for (auto& move : *movesToTest) {
		++nbMoves;
		game->play(move);
		eval_t eval = -negamax(game, depth - 1, movesTemp, nbMoves);
		game->undo();
		if (eval > bestEval) {
			bestEval = eval;
			insertBack(moves, movesTemp);
			moves[moves.size() - depth] = move;
		}
	}
	
	return bestEval;
}

#endif
#include "ai.h"

#include <vector>

#include "evaluation.h"
#include "util/chrono.h"
#include "util/printdatas.h"
#include "util/random.h"

inline void insertBack(std::vector<Move>& target,
        std::vector<Move> const& source) {
    auto trit = target.rbegin();
    for (auto srit = source.rbegin(); srit != source.rend(); ++srit) {
        *trit = *srit;
        ++trit;
    }
}

/******************************************************************************/

Move randomMove(ChessGame const& game) {
    return Random().random(game.getValidMoves());
}

/******************************************************************************/

struct AI::Private {
        Private();

        int nbMoves;
        int nbLeafs;
        bool stop;
        int maxDepth;

        void reset();
        eval_t alphabeta(std::shared_ptr<ChessGame> game, int depth,
                eval_t alpha, std::vector<Move>& moves);
};

AI::Private::Private() :
        maxDepth(4) {
}

void AI::Private::reset() {
    nbMoves = 0;
    nbLeafs = 0;
    stop = false;
}

eval_t AI::Private::alphabeta(std::shared_ptr<ChessGame> game, int depth,
        eval_t alpha, std::vector<Move>& moves) {
    if (game->resultat != Resultat::NotOver) {
        return game->turn * scoreWhenOver(game->resultat);
    } else if (depth == 0) {
        ++nbLeafs;
        return game->turn * evaluate(*game->board);
    }
    
    eval_t bestEval = eval_min();
    std::vector<Move> movesTemp(depth);
    for (auto &move : game->getValidMoves()) {
        if (stop) {
            return bestEval;
        }
        ++nbMoves;
        game->play(move);
        eval_t eval = -alphabeta(game, depth - 1, bestEval, movesTemp);
        game->undo();
        if (eval > bestEval) {
            bestEval = eval;
            insertBack(moves, movesTemp);
            moves[moves.size() - depth] = move;
            if (-bestEval <= alpha) {
                return bestEval;
            }
        }
    }
    
    return bestEval;
}

/******************************************************************************/

AI::AI(std::shared_ptr<ChessGame> game) :
        d(std::make_shared<Private>()),
        game(game) {
}

int AI::getDepth() {
    return d->maxDepth;
}

void AI::setDepth(int depth) {
    d->maxDepth = depth;
}

void AI::setGame(std::shared_ptr<ChessGame> game) {
    this->game = game;
}

void AI::stopSearch() {
    d->stop = true;
}

bool AI::wasStopped() const {
    return d->stop;
}

Move AI::search() {
    const int depth = d->maxDepth;
    d->reset();
    std::vector<Move> moves(depth);
    
    Chrono chrono(true);
    eval_t eval = d->alphabeta(game->clone(), depth, eval_min(), moves);
    chrono.pause();
    printDatas(depth, game->getTurn() * eval, d->nbMoves, d->nbLeafs, chrono,
        moves);
    
    return moves[0];
}

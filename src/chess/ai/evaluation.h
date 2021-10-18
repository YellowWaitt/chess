#pragma once

#include "chess/game/chessgame.h"

typedef float eval_t;

inline constexpr eval_t eval_min() {
    return -1000000;
}

inline constexpr eval_t eval_max() {
    return -eval_min();
}

inline eval_t operator*(Color const color, eval_t const eval) {
    return color == Color::White ? eval : -eval;
}

eval_t scoreWhenOver(Resultat res);

eval_t materialScore(Chessboard const& board);

eval_t controlledSquare(Chessboard const& board);

eval_t evaluate(Chessboard const& board);

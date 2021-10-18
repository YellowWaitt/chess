#include "evaluation.h"

eval_t evaluate(Chessboard const& board) {
    return materialScore(board) + 0.05 * controlledSquare(board);
}

/******************************************************************************/

eval_t scoreWhenOver(Resultat res) {
    switch (res) {
        case Resultat::White:
            return eval_max() - 1;
        case Resultat::Black:
            return eval_min() + 1;
        default:
            return 0;
    }
}

/******************************************************************************/

eval_t materialScore(Chessboard const& board) {
    static eval_t scores[7] = { 0, 1, 3, 3, 5, 10, 0 };
    Square square;
    eval_t score = 0;
    for (square.x = 0; square.x < 8; ++square.x) {
        for (square.y = 0; square.y < 8; ++square.y) {
            Piece piece = board.getPiece(square);
            if (piece.color == Color::White) {
                score += scores[size_t(piece.name)];
            } else {
                score -= scores[size_t(piece.name)];
            }
        }
    }
    return score;
}

/******************************************************************************/

#define CHECK(s, p) if (board.getPiece(s) == p) ++controlled
#define CHECK_KNIGHT(s, p, dx, dy) \
	s.x = square.x + dx; \
	s.y = square.y + dy; \
	if (0 <= s.x and s.x < 8 and 0 <= s.y and s.y < 8) CHECK(s, p)
#define CHECK_NOT_EMPTY(s, p, q) \
	Piece piece = board.getPiece(s); \
	if (piece.name != PieceName::Empty) { \
		if (piece == opp or piece == q) ++controlled; \
		break; \
	}
int nbControlled(Chessboard const& board, Square const& square, Color color) {
    int controlled = 0;
    Square s;
    
    Piece opp = { PieceName::King, color };
    for (s.x = std::max(square.x - 1, 0); s.x <= std::min(square.x + 1, 7);
            ++s.x) {
        for (s.y = std::max(square.y - 1, 0); s.y <= std::min(square.y + 1, 7);
                ++s.y) {
            CHECK(s, opp);
        }
    }
    
    opp.name = PieceName::Pawn;
    s.y = color == Color::Black ? square.y + 1 : square.y - 1;
    if (0 <= s.y and s.y < 8) {
        s.x = square.x - 1;
        if (s.x >= 0) {
            CHECK(s, opp);
        }
        s.x = square.x + 1;
        if (s.x < 8) {
            CHECK(s, opp);
        }
    }
    
    opp.name = PieceName::Knight;
    CHECK_KNIGHT(s, opp, 2, -1);
    CHECK_KNIGHT(s, opp, 2, 1);
    CHECK_KNIGHT(s, opp, 1, -2);
    CHECK_KNIGHT(s, opp, 1, 2);
    CHECK_KNIGHT(s, opp, -1, -2);
    CHECK_KNIGHT(s, opp, -1, 2);
    CHECK_KNIGHT(s, opp, -2, -1);
    CHECK_KNIGHT(s, opp, -2, 1);
    
    Piece queen = { PieceName::Queen, color };
    
    opp.name = PieceName::Bishop;
    s.x = square.x + 1;
    s.y = square.y + 1;
    while (s.x < 8 and s.y < 8) {
        CHECK_NOT_EMPTY(s, opp, queen);
        ++s.x;
        ++s.y;
    }
    s.x = square.x + 1;
    s.y = square.y - 1;
    while (s.x < 8 and s.y >= 0) {
        CHECK_NOT_EMPTY(s, opp, queen);
        ++s.x;
        --s.y;
    }
    s.x = square.x - 1;
    s.y = square.y + 1;
    while (s.x >= 0 and s.y < 8) {
        CHECK_NOT_EMPTY(s, opp, queen);
        --s.x;
        ++s.y;
    }
    s.x = square.x - 1;
    s.y = square.y - 1;
    while (s.x >= 0 and s.y >= 0) {
        CHECK_NOT_EMPTY(s, opp, queen);
        --s.x;
        --s.y;
    }
    
    opp.name = PieceName::Rook;
    s.x = square.x;
    for (s.y = square.y + 1; s.y < 8; ++s.y) {
        CHECK_NOT_EMPTY(s, opp, queen);
    }
    for (s.y = square.y - 1; s.y >= 0; --s.y) {
        CHECK_NOT_EMPTY(s, opp, queen);
    }
    s.y = square.y;
    for (s.x = square.x + 1; s.x < 8; ++s.x) {
        CHECK_NOT_EMPTY(s, opp, queen);
    }
    for (s.x = square.x - 1; s.x >= 0; --s.x) {
        CHECK_NOT_EMPTY(s, opp, queen);
    }
    
    return controlled;
}
#undef CHECK
#undef CHECK_KNIGHT
#undef CHECK_NOT_EMPTY

/*----------------------------------------------------------------------------*/

eval_t controlledSquare(Chessboard const& board) {
    Square square;
    eval_t score = 0;
    for (square.x = 0; square.x < 8; ++square.x) {
        for (square.y = 0; square.y < 8; ++square.y) {
            score += nbControlled(board, square, Color::White);
            score -= nbControlled(board, square, Color::Black);
        }
    }
    return score;
}

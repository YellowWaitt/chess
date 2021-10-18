#include "chessgame.h"

#include "util/sgn.h"

inline bool operator==(std::shared_ptr<Chessboard> const& board1,
        std::shared_ptr<Chessboard> const& board2) {
    return *board1 == *board2;
}

/******************************************************************************/

ChessGame::ChessGame(std::unique_ptr<Chessboard> board) :
        board(std::move(board)) {
    start();
}

ChessGame::ChessGame() {
}

ChessGame::ChessGame(ChessGame const& game) :
        board(game.board->clone()),
        historic(game.historic),
        previousPos(game.previousPos),
        kings(game.kings),
        resultat(game.resultat),
        turn(game.turn),
        fiftyMoveCounter(game.fiftyMoveCounter) {
}

std::shared_ptr<ChessGame> ChessGame::clone() const {
    return std::make_shared<ChessGame>(*this);
}

/*----------------------------------------------------------------------------*/

void ChessGame::start() {
    startFrom(STARTING_POSITION, Color::White);
}

void ChessGame::startFrom(std::string const& pos, Color turn) {
    board->startFrom(pos);
    this->turn = turn;
    fiftyMoveCounter = 0;
    historic.clear();
    previousPos.clear();
    previousPos.insert(std::make_pair(board->clone(), 1));
    kings[int(Color::White)] = search({PieceName::King, Color::White});
    kings[int(Color::Black)] = search({PieceName::King, Color::Black});
    resultat = Resultat::NotOver;
    updateResultat();
}

/******************************************************************************/

bool ChessGame::isOver() const {
    return resultat != Resultat::NotOver;
}

std::vector<Play> const& ChessGame::getHistoric() const {
    return historic;
}

Piece ChessGame::getPiece(Square const& square) const {
    return board->getPiece(square);
}

Square ChessGame::getSquareKing(Color color) const {
    return kings[size_t(color)];
}

Resultat ChessGame::getResultat() const {
    return resultat;
}

Color ChessGame::getTurn() const {
    return turn;
}

/*----------------------------------------------------------------------------*/

void ChessGame::draw() {
    resultat = Resultat::Draw;
}

void ChessGame::surrender(Color color) {
    if (color == Color::White) {
        resultat = Resultat::Black;
    } else {
        resultat = Resultat::White;
    }
}

/*----------------------------------------------------------------------------*/

std::string ChessGame::toString() const {
    return board->toString();
}

/******************************************************************************/

void ChessGame::getValidPawnMoves(std::vector<Move>& moves,
        Square const& square) const {
    int dy = turn == Color::White ? +1 : -1;
    Move move;
    move.from = square;
    move.to.y = square.y + dy;
    for (move.to.x = std::max(0, square.x - 1);
            move.to.x <= std::min(7, square.x + 1); ++move.to.x) {
        if (isValidPawn(move) and kingSafeAfter(move)) {
            moves.push_back(move);
        }
    }
    move.to.x = square.x;
    move.to.y = square.y + 2 * dy;
    if (isValidPawn(move) and kingSafeAfter(move)) {
        moves.push_back(move);
    }
}

/*----------------------------------------------------------------------------*/

#define CHECK_KNIGHT(l, f, m, dx, dy) \
	m.to.x = f.x + dx; \
	m.to.y = f.y + dy; \
	if (0 <= m.to.x and m.to.x < 8 and 0 <= m.to.y and m.to.y < 8) { \
		if (isValidKnight(m) and kingSafeAfter(move)) { \
			l.push_back(m); \
		} \
	}
void ChessGame::getValidKnightMoves(std::vector<Move>& moves,
        Square const& square) const {
    Move move;
    move.from = square;
    CHECK_KNIGHT(moves, square, move, 2, -1);
    CHECK_KNIGHT(moves, square, move, 2, 1);
    CHECK_KNIGHT(moves, square, move, 1, -2);
    CHECK_KNIGHT(moves, square, move, 1, 2);
    CHECK_KNIGHT(moves, square, move, -1, -2);
    CHECK_KNIGHT(moves, square, move, -1, 2);
    CHECK_KNIGHT(moves, square, move, -2, -1);
    CHECK_KNIGHT(moves, square, move, -2, 1);
}
#undef CHECK_KNIGHT

/*----------------------------------------------------------------------------*/

#define CHECK(name, l, m) \
	if (isValid##name(m)) { \
		if (kingSafeAfter(m)) { \
			l.push_back(m); \
		} \
	} else { \
		break; \
	}

void ChessGame::getValidBishopMoves(std::vector<Move>& moves,
        Square const& square) const {
    Move move;
    move.from = square;
    move.to.x = square.x + 1;
    move.to.y = square.y + 1;
    while (move.to.x < 8 and move.to.y < 8) {
        CHECK(Bishop, moves, move);
        ++move.to.x;
        ++move.to.y;
    }
    move.to.x = square.x + 1;
    move.to.y = square.y - 1;
    while (move.to.x < 8 and move.to.y >= 0) {
        CHECK(Bishop, moves, move);
        ++move.to.x;
        --move.to.y;
    }
    move.to.x = square.x - 1;
    move.to.y = square.y + 1;
    while (move.to.x >= 0 and move.to.y < 8) {
        CHECK(Bishop, moves, move);
        --move.to.x;
        ++move.to.y;
    }
    move.to.x = square.x - 1;
    move.to.y = square.y - 1;
    while (move.to.x >= 0 and move.to.y >= 0) {
        CHECK(Bishop, moves, move);
        --move.to.x;
        --move.to.y;
    }
}

void ChessGame::getValidRookMoves(std::vector<Move>& moves,
        Square const& square) const {
    Move move;
    move.from = square;
    move.to.x = square.x;
    for (move.to.y = square.y + 1; move.to.y < 8; ++move.to.y) {
        CHECK(Rook, moves, move);
    }
    for (move.to.y = square.y - 1; move.to.y >= 0; --move.to.y) {
        CHECK(Rook, moves, move);
    }
    move.to.y = square.y;
    for (move.to.x = square.x + 1; move.to.x < 8; ++move.to.x) {
        CHECK(Rook, moves, move);
    }
    for (move.to.x = square.x - 1; move.to.x >= 0; --move.to.x) {
        CHECK(Rook, moves, move);
    }
}

#undef CHECK

/*----------------------------------------------------------------------------*/

void ChessGame::getValidQueenMoves(std::vector<Move>& moves,
        Square const& square) const {
    getValidBishopMoves(moves, square);
    getValidRookMoves(moves, square);
}

/*----------------------------------------------------------------------------*/

void ChessGame::getValidKingMoves(std::vector<Move>& moves,
        Square const& square) const {
    Move move;
    move.from = square;
    for (move.to.x = std::max(square.x - 1, 0);
            move.to.x <= std::min(square.x + 1, 7); ++move.to.x) {
        for (move.to.y = std::max(square.y - 1, 0);
                move.to.y <= std::min(square.y + 1, 7); ++move.to.y) {
            if (isValidKing(move) and kingSafeAfter(move)) {
                moves.push_back(move);
            }
        }
    }
    move.to.y = square.y;
    move.to.x = square.x - 2;
    if (move.to.x == 2 and isValidCastle(move)) {
        moves.push_back(move);
    }
    move.to.x = square.x + 2;
    if (move.to.x == 6 and isValidCastle(move)) {
        moves.push_back(move);
    }
}

/*----------------------------------------------------------------------------*/

std::vector<Move> ChessGame::getValidMoves(Square const& square) const {
    std::vector<Move> moves;
    Piece piece = board->getPiece(square);
    if (piece.color != turn or isOver()) {
        return moves;
    }
    moves.reserve(27);
    switch (piece.name) {
        case PieceName::Empty:
            return moves;
        case PieceName::Pawn:
            getValidPawnMoves(moves, square);
            break;
        case PieceName::Knight:
            getValidKnightMoves(moves, square);
            break;
        case PieceName::Bishop:
            getValidBishopMoves(moves, square);
            break;
        case PieceName::Rook:
            getValidRookMoves(moves, square);
            break;
        case PieceName::Queen:
            getValidQueenMoves(moves, square);
            break;
        case PieceName::King:
            getValidKingMoves(moves, square);
            break;
    }
    return moves;
}

/*----------------------------------------------------------------------------*/

std::vector<Move> ChessGame::getValidMoves() const {
    std::vector<Move> moves;
    moves.reserve(137);
    Square square;
    for (square.x = 0; square.x < 8; ++square.x) {
        for (square.y = 0; square.y < 8; ++square.y) {
            auto toPush = getValidMoves(square);
            moves.insert(moves.end(), toPush.begin(), toPush.end());
        }
    }
    return moves;
}

/******************************************************************************/

Square ChessGame::search(Piece const& piece) const {
    Square square;
    for (square.x = 0; square.x < 8; ++square.x) {
        for (square.y = 0; square.y < 8; ++square.y) {
            if (board->getPiece(square) == piece) {
                return square;
            }
        }
    }
    return square;
}

/******************************************************************************/

#define CHECK(s, p) if (board->getPiece(s) == p) return true
#define CHECK_KNIGHT(s, p, dx, dy) \
	s.x = square.x + dx; \
	s.y = square.y + dy; \
	if (0 <= s.x and s.x < 8 and 0 <= s.y and s.y < 8) CHECK(s, p)
#define CHECK_NOT_EMPTY(s, p, q) \
	Piece piece = board->getPiece(s); \
	if (piece.name != PieceName::Empty) { \
		if (piece == opp or piece == q) return true; \
		break; \
	}
bool ChessGame::isControlled(Square const& square, Color color) const {
    Square s;
    
    Piece opp = {PieceName::King, color};
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

    Piece queen = {PieceName::Queen, color};
    
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
    
    return false;
}
#undef CHECK
#undef CHECK_KNIGHT
#undef CHECK_NOT_EMPTY

/******************************************************************************/

bool ChessGame::isCastle(Move const& move) const {
    Piece king = board->getPiece(move.from);
    return king.name == PieceName::King and abs(move.from.x - move.to.x) == 2;
}

/*----------------------------------------------------------------------------*/

bool ChessGame::isPassCapture(Move const& move, Piece const& taken) const {
    Piece pawn = board->getPiece(move.from);
    return pawn.name == PieceName::Pawn and move.from.x - move.to.x != 0
        and taken.name == PieceName::Empty;
}

/*----------------------------------------------------------------------------*/

bool ChessGame::isPromotion(Move const& move) const {
    Piece pawn = board->getPiece(move.from);
    if (pawn.name == PieceName::Pawn) {
        if (pawn.color == Color::White) {
            return move.to.y == 7;
        } else {
            return move.to.y == 0;
        }
    }
    return false;
}

/******************************************************************************/

bool ChessGame::checkEmpty(Square const& from, Square const& to, int dx,
        int dy) const {
    Square square = {from.x + dx, from.y + dy};
    while (square != to) {
        if (not board->isEmpty(square)) {
            return false;
        }
        square.x += dx;
        square.y += dy;
    }
    return true;
}

/*----------------------------------------------------------------------------*/

bool ChessGame::isValidPawn(Move const& move) const {
    int dx = abs(move.to.x - move.from.x);
    int dy = move.to.y - move.from.y;
    if ((turn == Color::White and dy < 0)
        or (turn == Color::Black and dy > 0)) {
        return false;
    }
    int ady = abs(dy);
    int sdy = sgn(dy);
    if (dx == 0) {
        if (ady == 1) {
            return board->isEmpty(move.to);
        } else if (ady == 2) {
            return move.from.y == (turn == Color::White ? 1 : 6)
                and board->isEmpty(move.to)
                and board->isEmpty({move.to.x, move.to.y - sdy});
        }
    } else if (dx == 1 and ady == 1) {
        Piece taken = board->getPiece(move.to);
        if (taken.name == PieceName::Empty) {
            if (historic.empty()) {
                return false;
            }
            Play last = historic.back();
            Square pawnS = {move.to.x, move.to.y - sdy};
            Piece pawn = board->getPiece(pawnS);
            return pawn.name == PieceName::Pawn and pawn.color == opposite(turn)
                and last.move.to == pawnS
                and last.move.from == (Square) {move.to.x, move.to.y + sdy};
        } else {
            return taken.color == opposite(turn);
        }
    }
    return false;
}

/*----------------------------------------------------------------------------*/

bool ChessGame::isValidKnight(Move const& move) const {
    int dx = abs(move.to.x - move.from.x);
    int dy = abs(move.to.y - move.from.y);
    if (dx + dy == 3 and dx != 3 and dy != 3) {
        Piece taken = board->getPiece(move.to);
        return taken.name == PieceName::Empty or taken.color == opposite(turn);
    }
    return false;
}

/*----------------------------------------------------------------------------*/

bool ChessGame::isValidBishop(Move const& move) const {
    int dx = move.to.x - move.from.x;
    int dy = move.to.y - move.from.y;
    if (abs(dx) == abs(dy) and dx != 0) {
        Piece taken = board->getPiece(move.to);
        return checkEmpty(move.from, move.to, sgn(dx), sgn(dy))
            and (taken.name == PieceName::Empty or taken.color == opposite(turn));
    }
    return false;
}

/*----------------------------------------------------------------------------*/

bool ChessGame::isValidRook(Move const& move) const {
    int dx = move.to.x - move.from.x;
    int dy = move.to.y - move.from.y;
    if (dx != 0 or dy != 0) {
        Piece taken = board->getPiece(move.to);
        return checkEmpty(move.from, move.to, sgn(dx), sgn(dy))
            and (taken.name == PieceName::Empty or taken.color == opposite(turn));
    }
    return false;
}

/*----------------------------------------------------------------------------*/

bool ChessGame::isValidQueen(Move const& move) const {
    return isValidBishop(move) or isValidRook(move);
}

/*----------------------------------------------------------------------------*/

bool ChessGame::isValidKing(Move const& move) const {
    int dx = abs(move.to.x - move.from.x);
    int dy = abs(move.to.y - move.from.y);
    if (dx + dy != 0 and dx < 2 and dy < 2) {
        Piece taken = board->getPiece(move.to);
        return not isControlled(move.to, opposite(turn))
            and (taken.name == PieceName::Empty or taken.color == opposite(turn));
    }
    return false;
}

/*----------------------------------------------------------------------------*/

bool ChessGame::kingSafeAfter(Move const& move) const {
    bool kingSafe;
    Piece taken = board->getPiece(move.to);
    bool isPass = isPassCapture(move, taken);
    auto pThis = const_cast<ChessGame*>(this);
    
    Square kingS =
        board->getPiece(move.from).name == PieceName::King ? move.to :
            kings[int(turn)];
    
    pThis->board->move(move.from, move.to);
    if (isPass) {
        pThis->passCapture(move);
        kingSafe = not isControlled(kingS, opposite(turn));
        pThis->unpassCapture(move);
    } else {
        kingSafe = not isControlled(kingS, opposite(turn));
    }
    pThis->board->move(move.to, move.from);
    pThis->board->setPiece(move.to, taken);
    
    return kingSafe;
}

/*----------------------------------------------------------------------------*/

bool ChessGame::isValid(Move const& move) const {
    Piece piece = board->getPiece(move.from);
    if (piece.color != turn or isOver()) {
        return false;
    }
    switch (piece.name) {
        case PieceName::Pawn:
            return isValidPawn(move) and kingSafeAfter(move);
        case PieceName::Knight:
            return isValidKnight(move) and kingSafeAfter(move);
        case PieceName::Bishop:
            return isValidBishop(move) and kingSafeAfter(move);
        case PieceName::Rook:
            return isValidRook(move) and kingSafeAfter(move);
        case PieceName::Queen:
            return isValidQueen(move) and kingSafeAfter(move);
        case PieceName::King:
            return (isValidKing(move) and kingSafeAfter(move))
                or isValidCastle(move);
        default:
            return false;
    }
}

/******************************************************************************/

bool ChessGame::krHasMoved(Move const& move, int side) const {
    int y = turn == Color::White ? 0 : 7;
    Square kingS = {4, y};
    Square rookS = {side > 0 ? 7 : 0, y};
    Piece king = board->getPiece(kingS);
    Piece rook = board->getPiece(rookS);
    if (move.from != kingS or king.name != PieceName::King or king.color != turn
        or rook.name != PieceName::Rook or rook.color != turn) {
        return true;
    }
    for (Play const &play : historic) {
        if (play.move.from == kingS or play.move.from == rookS
            or play.move.to == rookS) {
            return true;
        }
    }
    return false;
}

/*----------------------------------------------------------------------------*/

bool ChessGame::isValidCastle(Move const& move) const {
    int side = move.to.x - move.from.x;
    if (abs(side) != 2 or move.from.y != move.to.y or krHasMoved(move, side)) {
        return false;
    }
    Color opp = opposite(turn);
#define CTRL(x) isControlled({x, move.from.y}, opp)
#define EMPT(x) board->isEmpty({x, move.from.y})
    if (side > 0) {
        if (not EMPT(5) or not EMPT(6) or CTRL(4) or CTRL(5) or CTRL(6)) {
            return false;
        }
    } else {
        if (not EMPT(3) or not EMPT(2) or not EMPT(1) or CTRL(4) or CTRL(3)
            or CTRL(2)) {
            return false;
        }
    }
#undef CTRL
#undef EMPT
    return true;
}

/******************************************************************************/

void ChessGame::castle(Move const& move) {
    Square from = {7, move.from.y}, to = {5, move.from.y};
    if (move.to.x - move.from.x < 0) {
        from.x = 0;
        to.x = 3;
    }
    board->move(move.from, move.to);
    board->move(from, to);
}

/*----------------------------------------------------------------------------*/

void ChessGame::passCapture(Move const& move) {
    if (turn == Color::White) {
        board->clear({move.to.x, move.to.y - 1});
    } else {
        board->clear({move.to.x, move.to.y + 1});
    }
}

/*----------------------------------------------------------------------------*/

bool ChessGame::play(Move const& move, Piece const* promotion) {
    if (not isValid(move)) {
        return false;
    }
    
    Play play = {move, board->getPiece(move.to), MoveType::Normal,
        fiftyMoveCounter};
    
    if (isCastle(move)) {
        castle(move);
        ++fiftyMoveCounter;
        play.type = MoveType::Castle;
        kings[int(turn)] = move.to;
    } else {
        if (isPromotion(move)) {
            board->clear(move.from);
            if (promotion == nullptr) {
                Piece queen = {PieceName::Queen, turn};
                board->setPiece(move.to, queen);
            } else {
                board->setPiece(move.to, *promotion);
            }
            fiftyMoveCounter = 0;
            play.type = MoveType::Promotion;
        } else {
            if (isPassCapture(move, play.taken)) {
                passCapture(move);
                fiftyMoveCounter = 0;
                play.type = MoveType::PassCapture;
            } else {
                Piece from = board->getPiece(move.from);
                if (from.name == PieceName::King) {
                    kings[int(turn)] = move.to;
                }
                if (from.name == PieceName::Pawn
                    or play.taken.name != PieceName::Empty) {
                    fiftyMoveCounter = 0;
                } else {
                    ++fiftyMoveCounter;
                }
            }
            board->move(move.from, move.to);
        }
    }
    
    historic.push_back(play);
    turn = opposite(turn);
    auto [iter, success] = previousPos.insert(std::make_pair(board->clone(), 1));
    if (not success and ++iter->second == 3) {
        draw();
    } else {
        updateResultat();
    }
    
    return true;
}

/******************************************************************************/

void ChessGame::uncastle(Move const& move) {
    Square from = {7, move.from.y}, to = {5, move.from.y};
    if (move.to.x - move.from.x < 0) {
        from.x = 0;
        to.x = 3;
    }
    board->move(move.to, move.from);
    board->move(to, from);
}

/*----------------------------------------------------------------------------*/

void ChessGame::unpassCapture(Move const& move) {
    if (turn == Color::White) {
        board->setPiece({move.to.x, move.to.y - 1}, {PieceName::Pawn,
            Color::Black});
    } else {
        board->setPiece({move.to.x, move.to.y + 1}, {PieceName::Pawn,
            Color::White});
    }
}

/*----------------------------------------------------------------------------*/

void ChessGame::undo() {
    if (historic.empty()) {
        return;
    }
    
    auto find = previousPos.find(board);
    if (--find->second == 0) previousPos.erase(find);
    Play last = historic.back();
    historic.pop_back();
    turn = opposite(turn);
    fiftyMoveCounter = last.fiftyMoveCounter;
    resultat = Resultat::NotOver;
    
    if (last.type == MoveType::Castle) {
        uncastle(last.move);
        kings[int(turn)] = last.move.from;
    } else {
        if (last.type == MoveType::Promotion) {
            board->setPiece(last.move.from, {PieceName::Pawn, turn});
        } else {
            Piece to = board->getPiece(last.move.to);
            if (last.type == MoveType::PassCapture) {
                unpassCapture(last.move);
            } else if (to.name == PieceName::King) {
                kings[int(turn)] = last.move.from;
            }
            board->move(last.move.to, last.move.from);
        }
        board->setPiece(last.move.to, last.taken);
    }
}

/******************************************************************************/

void ChessGame::updateResultat() {
    Square square;
    for (square.x = 0; square.x < 8; ++square.x) {
        for (square.y = 0; square.y < 8; ++square.y) {
            auto moves = getValidMoves(square);
            if (not moves.empty()) {
                if (fiftyMoveCounter == 50) {
                    draw();
                }
                return;
            }
        }
    }
    if (isControlled(kings[int(turn)], opposite(turn))) {
        surrender(turn);
    } else {
        draw();
    }
}

/******************************************************************************/

#if 0
bool ChessGame::hasThreeRepetitions() const {
	auto pThis = const_cast<ChessGame*>(this);
	bool ret = false;
	auto save = board->clone();
	std::unordered_map<std::shared_ptr<Chessboard>, int, hashChessboard> positions;
	positions.insert(std::make_pair(board->clone(), 1));
	Color turn = this->turn;
	for (auto it = historic.rbegin(); it != historic.rend(); ++it) {
		turn = opposite(turn);
		if (it->type == MoveType::Castle) {
			pThis->uncastle(it->move);
		} else {
			if (it->type == MoveType::Promotion) {
				board->setPiece(it->move.from, {PieceName::Pawn, turn});
			} else {
				if (it->type == MoveType::PassCapture) {
					pThis->unpassCapture(it->move);
				}
				board->move(it->move.to, it->move.from);
			}
			board->setPiece(it->move.to, it->taken);
		}
		
		auto ins = positions.insert(std::make_pair(board->clone(), 1));
		if (not ins.second) {
			if (++ins.first->second == 3) {
				ret = true;
				break;
			}
		}
	}
	save.swap(pThis->board);
	return ret;
}
#endif

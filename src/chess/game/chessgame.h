#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "chess/board/chessboard.h"


enum class Resultat {
	NotOver, White, Draw, Black
};

enum class MoveType {
	Normal, Castle, PassCapture, Promotion
};

struct Move {
	Square from; Square to;
};

struct Play {
	Move move; Piece taken; MoveType type; int fiftyMoveCounter;
};


class ChessGame {
	friend class AI;
	
	public:
		ChessGame(std::unique_ptr<Chessboard> board);
		ChessGame(ChessGame const& game);
		
		virtual std::shared_ptr<ChessGame> clone() const;
		void draw();
		std::vector<Play> const& getHistoric() const;
		Piece getPiece(Square const& square) const;
		Square getSquareKing(Color color) const;
		Resultat getResultat() const;
		Color getTurn() const;
		std::vector<Move> getValidMoves() const;
		std::vector<Move> getValidMoves(Square const& square) const;
		virtual bool isCastle(Move const& move) const;
		bool isOver() const;
		bool isPassCapture(Move const& move, Piece const& taken) const;
		bool isPromotion(Move const& move) const;
		bool isValid(Move const& move) const;
		bool play(Move const& move, Piece const* promotion = nullptr);
		virtual void start();
		virtual void startFrom(std::string const& pos, Color turn);
		void surrender(Color color);
		std::string toString() const;
		virtual void undo();
	
	protected:
		struct hashChessboard {
			inline size_t operator()(std::shared_ptr<Chessboard> const& board) const {
				return board->hash();
			}
		};
		
		std::shared_ptr<Chessboard> board;
		std::vector<Play> historic;
		std::unordered_map<std::shared_ptr<Chessboard>, int, hashChessboard> previousPos;
		Square kings[2];
		Resultat resultat;
		Color turn;
		int fiftyMoveCounter;
		
		ChessGame();
		
		virtual void castle(Move const& move);
		bool checkEmpty(Square const& from, Square const& to, int dx, int dy) const;
		void getValidBishopMoves(std::vector<Move>& moves, Square const& square) const;
		virtual void getValidKingMoves(std::vector<Move>& moves, Square const& square) const;
		void getValidKnightMoves(std::vector<Move>& moves, Square const& square) const;
		void getValidPawnMoves(std::vector<Move>& moves, Square const& square) const;
		void getValidQueenMoves(std::vector<Move>& moves, Square const& square) const;
		void getValidRookMoves(std::vector<Move>& moves, Square const& square) const;
		virtual bool isControlled(Square const& square, Color color) const;
		bool isValidBishop(Move const& move) const;
		virtual bool isValidCastle(Move const& move) const;
		bool isValidKing(Move const& move) const;
		bool isValidKnight(Move const& move) const;
		virtual bool isValidPawn(Move const& move) const;
		bool isValidQueen(Move const& move) const;
		bool isValidRook(Move const& move) const;
		bool kingSafeAfter(Move const& move) const;
		virtual bool krHasMoved(Move const& move, int side) const;
		void passCapture(Move const& move);
		Square search(Piece const& piece) const;
		virtual void uncastle(Move const& move);
		void unpassCapture(Move const& move);
		virtual void updateResultat();
};

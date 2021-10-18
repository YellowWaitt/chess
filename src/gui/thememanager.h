#pragma once

#include <string>

#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>


class ThemeManager {
	public:
		ThemeManager();
		
		void loadPieceTheme(std::string const& name);
		
		Gdk::RGBA arrow[4];
		Gdk::RGBA board[2];
		Gdk::RGBA lastMove;
		Gdk::RGBA mark[4];
		Gdk::RGBA selected;
		Gdk::RGBA validMoves;
		
		Glib::RefPtr<Gdk::Pixbuf> background;
		Glib::RefPtr<Gdk::Pixbuf> chessboard;
		Glib::RefPtr<Gdk::Pixbuf> icon;
		
		Glib::RefPtr<Gdk::Pixbuf> blackBishop;
		Glib::RefPtr<Gdk::Pixbuf> blackKing;
		Glib::RefPtr<Gdk::Pixbuf> blackKnight;
		Glib::RefPtr<Gdk::Pixbuf> blackPawn;
		Glib::RefPtr<Gdk::Pixbuf> blackQueen;
		Glib::RefPtr<Gdk::Pixbuf> blackRook;
		Glib::RefPtr<Gdk::Pixbuf> whiteBishop;
		Glib::RefPtr<Gdk::Pixbuf> whiteKing;
		Glib::RefPtr<Gdk::Pixbuf> whiteKnight;
		Glib::RefPtr<Gdk::Pixbuf> whitePawn;
		Glib::RefPtr<Gdk::Pixbuf> whiteQueen;
		Glib::RefPtr<Gdk::Pixbuf> whiteRook;
};

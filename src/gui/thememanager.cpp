#include "thememanager.h"

#include <iostream>

#include <giomm/resource.h>
#include <glibmm.h>


Glib::RefPtr<Gdk::Pixbuf> loadImage(std::string const& fileName) {
	Glib::RefPtr<Gdk::Pixbuf> image;
	try {
		image = Gdk::Pixbuf::create_from_resource(fileName);
	} catch(Glib::FileError const& ex) {
		std::cerr << "FileError: " << ex.what() << std::endl;
	} catch(Gio::ResourceError const& ex) {
		std::cerr << "ResourceError: " << ex.what() << std::endl;
	} catch(Gdk::PixbufError const& ex) {
		std::cerr << "PixbufError: " << ex.what() << std::endl;
	}
	return image;
}

Gdk::RGBA meanColor(Glib::RefPtr<Gdk::Pixbuf> const image,
		int x0, int y0, int width, int height) {
	double r = 0.0, g = 0.0, b = 0.0;
	auto pixels = image->get_pixels();
	for (int x = x0; x <= x0 + width; ++x) {
		for (int y = y0; y <= y0 + height; ++y) {
			int offset = y * image->get_rowstride() + x * image->get_n_channels();
			auto pixel = &pixels[offset];
			r += pixel[0];
			g += pixel[1];
			b += pixel[2];
		}
	}
	size_t nbPixel = height * width * 255;
	Gdk::RGBA color;
	color.set_rgba(r / nbPixel, g / nbPixel, b / nbPixel, 1);
	return color;
}

/******************************************************************************/

ThemeManager::ThemeManager() {
	background = loadImage("/images/background.jpg");
	chessboard = loadImage("/images/wood_board.jpg");
	icon = loadImage("/images/icon.png");
	
	loadPieceTheme("/images/default/");
	
	lastMove.set_rgba(0.06, 0.62, 0.91, 0.25);
	selected.set_rgba(0.06, 0.62, 0.91, 0.25);
	validMoves.set_rgba(0.06, 0.62, 0.91, 0.8);
	
	arrow[0].set_rgba(1.0, 0.51, 0.0, 1.0);
	arrow[1].set_rgba(0.83, 0.06, 0.06, 1.0);
	arrow[2].set_rgba(0.2, 0.79, 0.14, 1.0);
	arrow[3].set_rgba(0.15, 0.77, 0.93, 1.0);
	
	mark[0].set_rgba(0.83, 0.06, 0.06, 0.8);
	mark[1].set_rgba(1.0, 0.51, 0.0, 0.8);
	mark[2].set_rgba(0.2, 0.79, 0.14, 0.8);
	mark[3].set_rgba(0.15, 0.77, 0.93, 0.8);
	
	int width = chessboard->get_width();
	int height = chessboard->get_height();
	board[0] = meanColor(chessboard, 0, 0, width / 8, height / 8);
	board[1] = meanColor(chessboard, width / 8, 0, width / 8, height / 8);
}

void ThemeManager::loadPieceTheme(std::string const& name) {
	whitePawn = loadImage(name + "white_pawn.png");
	whiteKnight = loadImage(name + "white_knight.png");
	whiteBishop = loadImage(name + "white_bishop.png");
	whiteRook = loadImage(name + "white_rook.png");
	whiteQueen = loadImage(name + "white_queen.png");
	whiteKing = loadImage(name + "white_king.png");
	blackPawn = loadImage(name + "black_pawn.png");
	blackKnight = loadImage(name + "black_knight.png");
	blackBishop = loadImage(name + "black_bishop.png");
	blackRook = loadImage(name + "black_rook.png");
	blackQueen = loadImage(name + "black_queen.png");
	blackKing = loadImage(name + "black_king.png");
}

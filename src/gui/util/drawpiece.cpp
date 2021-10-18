#include "drawpiece.h"

#include <gdkmm/general.h>

#define casePiece(name) \
	case PieceName::name: \
		if (piece.color == Color::White) { \
			toDraw = theme->white##name->scale_simple(size, size, Gdk::INTERP_HYPER); \
		} else { \
			toDraw = theme->black##name->scale_simple(size, size, Gdk::INTERP_HYPER); \
		} \
		break
void drawPiece(std::shared_ptr<ThemeManager const> const& theme,
	Piece const& piece,
	Cairo::RefPtr<Cairo::Context> const& cr,
	double x0,
	double y0,
	double x,
	double y,
	double size
) {
	Glib::RefPtr<Gdk::Pixbuf> toDraw;
	switch (piece.name) {
		casePiece(Pawn);
		casePiece(Knight);
		casePiece(Bishop);
		casePiece(Rook);
		casePiece(Queen);
		casePiece(King);
		default:
			return;
	}
	Gdk::Cairo::set_source_pixbuf(cr, toDraw, x0 + x * size, y0 + y * size);
	cr->paint();
}

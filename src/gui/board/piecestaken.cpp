#include "piecestaken.h"

#include "gui/util/drawpiece.h"


struct PiecesTaken::Private {
	Private(std::shared_ptr<ThemeManager const> theme,
			std::shared_ptr<double const> pieceSize,
			Color color);
	
	std::shared_ptr<ThemeManager const> theme;
	std::shared_ptr<double const> pieceSize;
	
	int taken[6];
	int nbPieces;
	Color color;
};

/******************************************************************************/

PiecesTaken::Private::Private(
	std::shared_ptr<ThemeManager const> theme,
	std::shared_ptr<double const> pieceSize,
	Color color)
: theme(theme),
  pieceSize(pieceSize),
  color(color)
{}

/*----------------------------------------------------------------------------*/

PiecesTaken::PiecesTaken(BaseObjectType* cobject,
	Glib::RefPtr<Gtk::Builder> const& builder)
: PiecesTaken(cobject, builder, nullptr, nullptr, Color::White)
{}

PiecesTaken::PiecesTaken(BaseObjectType* cobject,
	Glib::RefPtr<Gtk::Builder> const& /*builder*/,
	std::shared_ptr<ThemeManager const> theme,
	std::shared_ptr<double const> pieceSize,
	Color color)
: Gtk::DrawingArea(cobject),
  d(std::make_shared<Private>(theme, pieceSize, color))
{
	clear();
}

/******************************************************************************/

void PiecesTaken::add(Piece const& piece) {
	add(piece.name);
}

void PiecesTaken::add(PieceName const name) {
	if (name != PieceName::Empty) {
		++d->taken[size_t(name) - 1];
		++d->nbPieces;
		queue_draw();
	}
}

void PiecesTaken::clear() {
	for (int i = 0; i < 6; ++i) {
		d->taken[i] = 0;
	}
	d->nbPieces = 0;
	queue_draw();
}

void PiecesTaken::remove(Piece const& piece) {
	remove(piece.name);
}

void PiecesTaken::remove(PieceName const name) {
	if (name != PieceName::Empty) {
		--d->taken[size_t(name) - 1];
		--d->nbPieces;
		queue_draw();
	}
}

/******************************************************************************/

bool PiecesTaken::on_draw(Cairo::RefPtr<Cairo::Context> const& cr) {
	Gtk::Allocation allocation = get_allocation();
	int height = allocation.get_height();
	int width = allocation.get_width();
	
	int nbPerColumn = height / *d->pieceSize;
	int nbColumn = d->nbPieces / nbPerColumn;
	
	double pady1 = double(height - *d->pieceSize * nbPerColumn) / 2;
	double pady2 = double(height - *d->pieceSize * (d->nbPieces % nbPerColumn)) / 2;
	
	bool toRight = d->color == Color::White;
	double x0 = toRight ? width : 0;
	
	int cumul = 0;
	for (int i = 0; i < 6; ++i) {
		Piece piece = {static_cast<PieceName>(i + 1), d->color};
		for (int j = 0; j < d->taken[i]; ++j) {
			int nCol = cumul / nbPerColumn;
			drawPiece(d->theme, piece, cr,
				x0, nCol != nbColumn ? pady1 : pady2,
				toRight ? -(nCol + 1) : nCol, cumul % nbPerColumn,
				*d->pieceSize);
			++cumul;
		}
	}
	
	return true;
}

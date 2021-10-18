#include "promotiondialog.h"

#include <gdkmm/general.h>
#include <gtkmm/dialog.h>
#include <gtkmm/drawingarea.h>

#include "gui/util/drawpiece.h"
#include "gui/util/loadglade.h"


struct PromotionDialog::Private {
	Private(Gtk::Widget& widget,
			std::shared_ptr<ThemeManager const> theme,
			std::shared_ptr<double const> pieceSize,
			Color color);
	~Private();
	
	std::shared_ptr<ThemeManager const> theme;
	std::shared_ptr<double const> pieceSize;
	Color color;
	Gtk::Dialog* pDialog;
	
	PieceName selected;
	
	bool drawPieces(Cairo::RefPtr<Cairo::Context> const& cr);
	bool on_button_press(GdkEventButton* event);
};

/******************************************************************************/

PromotionDialog::Private::Private(
	Gtk::Widget& widget,
	std::shared_ptr<ThemeManager const> theme,
	std::shared_ptr<double const> pieceSize,
	Color color)
: theme(theme),
  pieceSize(pieceSize),
  color(color)
{
	Gdk::RGBA black;
	black.set_grey(0.0);
	
	auto builder = Gtk::Builder::create();
	loadGlade(builder, "/glade/promotion.glade");
	
	builder->get_widget("topLevel", pDialog);
	pDialog->set_size_request(*pieceSize, *pieceSize * 4 + 5);
	pDialog->set_transient_for(*dynamic_cast<Gtk::Window*>(widget.get_toplevel()));
	pDialog->set_position(Gtk::WindowPosition::WIN_POS_MOUSE);
	pDialog->override_background_color(black);
	
	Gtk::DrawingArea* pArea;
	builder->get_widget("area", pArea);
	pArea->signal_draw().connect(sigc::mem_fun(*this, &Private::drawPieces));
	pArea->add_events(Gdk::BUTTON_PRESS_MASK);
	pArea->signal_button_press_event().connect(
		sigc::mem_fun(*this, &Private::on_button_press));
}

PromotionDialog::Private::~Private() {
	delete pDialog;
}

/*----------------------------------------------------------------------------*/

PromotionDialog::PromotionDialog(Gtk::Widget& widget,
	std::shared_ptr<ThemeManager const> theme,
	std::shared_ptr<double const> pieceSize,
	Color color)
: d(std::make_shared<Private>(widget, theme, pieceSize, color))
{}

/******************************************************************************/

PieceName PromotionDialog::run() {
	d->pDialog->run();
	return d->selected;
}

bool PromotionDialog::Private::drawPieces(Cairo::RefPtr<Cairo::Context> const& cr) {
	Gdk::Cairo::set_source_pixbuf(cr, theme->background, 0, 0);
	cr->paint();
	
	drawPiece(theme, {PieceName::Queen, color}, cr,
		0, 0, 0, 0, *pieceSize);
	drawPiece(theme, {PieceName::Rook, color}, cr,
		0, 0, 0, 1, *pieceSize);
	drawPiece(theme, {PieceName::Bishop, color}, cr,
		0, 0, 0, 2, *pieceSize);
	drawPiece(theme, {PieceName::Knight, color}, cr,
		0, 0, 0, 3, *pieceSize);
	
	return true;
}

bool PromotionDialog::Private::on_button_press(GdkEventButton* event) {
	int y = event->y / *pieceSize;
	switch (y) {
		case 0:
			selected = PieceName::Queen;
			break;
		case 1:
			selected = PieceName::Rook;
			break;
		case 2:
			selected = PieceName::Bishop;
			break;
		case 3:
			selected = PieceName::Knight;
			break;
	}
	pDialog->hide();
	return true;
}

#include "widgetdrawer.h"

#include <gdkmm/general.h>

#include "drawpiece.h"


bool WidgetDrawer::background(Cairo::RefPtr<Cairo::Context> const& cr,
Glib::RefPtr<Gdk::Pixbuf>* image) {
	Gtk::Allocation allocation = get_allocation();
	int width = allocation.get_width();
	int height = allocation.get_height();
	
	auto toDraw = (*image)->scale_simple(width, height, Gdk::INTERP_HYPER);
	Gdk::Cairo::set_source_pixbuf(cr, toDraw, 0, 0);
	cr->paint();
	
	return on_draw(cr);
}

/******************************************************************************/

bool WidgetDrawer::pieces(Cairo::RefPtr<Cairo::Context> const& cr,
std::shared_ptr<ThemeManager> theme) {
	double size, x0, y0;
	{
		Gtk::Allocation allocation = get_allocation();
		double width = double(allocation.get_width()) / 6;
		double height = double(allocation.get_height()) / 2;
		if (width < height) {
			size = width;
			x0 = 0;
			y0 = (allocation.get_height() - 2 * size) / 2;
		} else {
			size = height;
			x0 = (allocation.get_width() - 6 * size) / 2;
			y0 = 0;
		}
	}
	
	for (int i = 0; i < 6; ++i) {
		drawPiece(theme, {static_cast<PieceName>(i + 1), Color::White}, cr,
			x0, y0, i, 0, size);
		drawPiece(theme, {static_cast<PieceName>(i + 1), Color::Black}, cr,
			x0, y0, i, 1, size);
	}
	
	return on_draw(cr);
}

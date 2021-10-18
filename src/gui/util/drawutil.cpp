#include "drawutil.h"

#include <pangomm/layout.h>


void drawText(Cairo::RefPtr<Cairo::Context> const& cr, std::string const& text,
		double size, double x, double y) {
	Pango::FontDescription font;
	font.set_family("Monospace");
	font.set_weight(Pango::WEIGHT_BOLD);
	font.set_absolute_size(size * PANGO_SCALE);
	
	auto layout = Pango::Layout::create(cr);
	layout->set_text(text);
	layout->set_font_description(font);

	int textWidth;
	int textHeight;

	layout->get_pixel_size(textWidth, textHeight);
	cr->move_to(x - textWidth / 2, y - textHeight / 2);
	
	layout->show_in_cairo_context(cr);
}
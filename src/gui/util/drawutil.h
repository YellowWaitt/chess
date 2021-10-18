#pragma once

#include <complex>
#include <string>

#include <cairomm/context.h>

#include "chess/types.h"


inline void drawSquare(Cairo::RefPtr<Cairo::Context> const& cr,
		double x0, double y0, double width, Square const& square) {
	cr->rectangle(
		x0 + width * square.x,
		y0 + width * square.y,
		width,
		width
	);
}

/******************************************************************************/

inline void drawLine(Cairo::RefPtr<Cairo::Context> const& cr,
		double x0, double y0, double dx, double dy) {
	cr->move_to(x0, y0);
	cr->line_to(x0 + dx, y0 + dy);
}

/******************************************************************************/

void drawText(Cairo::RefPtr<Cairo::Context> const& cr, std::string const& text,
		double size, double x, double y);

/******************************************************************************/

inline void line_to(Cairo::RefPtr<Cairo::Context> const& cr, std::complex<double> const& vec) {
	cr->line_to(vec.real(), vec.imag());
}

/******************************************************************************/

inline void move_to(Cairo::RefPtr<Cairo::Context> const& cr, std::complex<double> const& vec) {
	cr->move_to(vec.real(), vec.imag());
}

/******************************************************************************/

inline void drawTriangle(Cairo::RefPtr<Cairo::Context> const& cr,
std::complex<double> const& a,
std::complex<double> const& b,
std::complex<double> const& c) {
	move_to(cr, a);
	line_to(cr, b);
	line_to(cr, c);
	line_to(cr, a);
	cr->fill();
}
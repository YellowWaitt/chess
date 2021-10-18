#pragma once

#include <memory>

#include <cairomm/context.h>

#include "chess/types.h"
#include "gui/thememanager.h"


void drawPiece(std::shared_ptr<ThemeManager const> const& theme,
	Piece const& piece,
	Cairo::RefPtr<Cairo::Context> const& cr,
	double x0,
	double y0,
	double x,
	double y,
	double size
);

#pragma once

#include <memory>

#include <gtkmm/widget.h>

#include "chess/types.h"
#include "gui/thememanager.h"


class PromotionDialog {
	public:
		PromotionDialog(Gtk::Widget& widget,
			std::shared_ptr<ThemeManager const> theme,
			std::shared_ptr<double const> pieceSize,
			Color color);

		PieceName run();
	
	private:
		struct Private;
		std::shared_ptr<Private> d;
};

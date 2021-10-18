#pragma once

#include <memory>

#include <gtkmm/widget.h>

#include "gui/thememanager.h"


class ThemeDialog {
	public:
		ThemeDialog(Gtk::Widget& widget, std::shared_ptr<ThemeManager> theme);

		void run();
	
	private:
		struct Private;
		std::shared_ptr<Private> d;
};

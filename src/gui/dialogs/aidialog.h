#pragma once

#include <memory>

#include <gtkmm/widget.h>

#include "gui/thememanager.h"
#include "chess/ai/ai.h"


class AiDialog {
	public:
		AiDialog(Gtk::Widget& widget,
			std::shared_ptr<ThemeManager> theme,
			std::shared_ptr<AI> ai);

		void run();
	
	private:
		struct Private;
		std::shared_ptr<Private> d;
};

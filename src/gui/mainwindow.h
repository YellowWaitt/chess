#pragma once

#include <memory>

#include <gtkmm/builder.h>
#include <gtkmm/window.h>

#include "thememanager.h"
#include "gamemanager.h"


class MainWindow : public Gtk::Window {
	public:
		MainWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& builder);
	
	private:
		struct Private;
		std::shared_ptr<Private> d;
};

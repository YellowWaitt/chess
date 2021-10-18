#pragma once

#include <cairomm/context.h>
#include <gtkmm/widget.h>

#include "gui/thememanager.h"


class WidgetDrawer : public Gtk::Widget {
	public:
		bool background(Cairo::RefPtr<Cairo::Context> const& cr,
			Glib::RefPtr<Gdk::Pixbuf>* image);
		
		bool pieces(Cairo::RefPtr<Cairo::Context> const& cr,
			std::shared_ptr<ThemeManager> theme);
		
/******************************************************************************/
		
		static inline
		void connect(Gtk::Widget* widget, Glib::RefPtr<Gdk::Pixbuf>* image) {
			widget->signal_draw().connect(sigc::bind(
				sigc::mem_fun(*reinterpret_cast<WidgetDrawer*>(widget), &WidgetDrawer::background),
				image)
			);
		}
		
		static inline
		void connect(Gtk::Widget* widget, std::shared_ptr<ThemeManager> theme) {
			widget->signal_draw().connect(sigc::bind(
				sigc::mem_fun(*reinterpret_cast<WidgetDrawer*>(widget), &WidgetDrawer::pieces),
				theme)
			);
		}
};

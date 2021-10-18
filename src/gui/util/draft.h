#if 0

#pragma once

#include <memory>

#include <cairomm/context.h>
#include <gtkmm/builder.h>
#include <gtkmm/widget.h>

#include <gui/thememanager.h>

using ThemeImage = Glib::RefPtr<Gdk::Pixbuf> ThemeManager::*;


template <ThemeImage Image>
class BackgroundImage : public virtual Gtk::Widget {
	public:
		BackgroundImage(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& builder);
		BackgroundImage(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& builder,
			std::shared_ptr<ThemeManager const> theme);
	
	protected:
		std::shared_ptr<ThemeManager const> theme;
		
		bool on_draw(Cairo::RefPtr<Cairo::Context> const& cr) override;
};

/******************************************************************************/

template <ThemeImage Image>
BackgroundImage<Image>::BackgroundImage(BaseObjectType* cobject,
	Glib::RefPtr<Gtk::Builder> const& builder)
: BackgroundImage(cobject, builder, nullptr)
{}

template <ThemeImage Image>
BackgroundImage<Image>::BackgroundImage(BaseObjectType* cobject,
	Glib::RefPtr<Gtk::Builder> const& /*builder*/,
	std::shared_ptr<ThemeManager const> theme)
: Widget(cobject),
  theme(theme)
{}

/******************************************************************************/

template <ThemeImage Image>
bool BackgroundImage<Image>::on_draw(Cairo::RefPtr<Cairo::Context> const& cr) {
	Gtk::Allocation allocation = get_allocation();
	int width = allocation.get_width();
	int height = allocation.get_height();
	
	auto toDraw = theme->(*Image)->scale_simple(width, height, Gdk::INTERP_HYPER);
	Gdk::Cairo::set_source_pixbuf(cr, toDraw, 0, 0);
	cr->paint();
	
	return Widget::on_draw(cr);
}

#endif
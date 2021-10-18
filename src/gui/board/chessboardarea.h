#pragma once

#include <memory>

#include <cairomm/context.h>
#include <glibmm/property.h>
#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>

#include "gui/gamemanager.h"
#include "gui/thememanager.h"


class ChessboardArea : public Gtk::DrawingArea {
	public:
		ChessboardArea();
		ChessboardArea(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& builder);
		ChessboardArea(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& builder,
			std::shared_ptr<ThemeManager const> theme,
			std::shared_ptr<GameManager> game);
	
	protected:
		Glib::Property<bool> do_drawGrid;
		Glib::Property<bool> do_drawLastMove;
		Glib::Property<bool> do_drawValidMoves;
		Glib::Property<bool> do_drawCoordinates;
		Glib::Property<bool> do_flipBoard;
	
		bool on_draw(Cairo::RefPtr<Cairo::Context> const& cr) override;
		bool on_motion_notify_event(GdkEventMotion* event) override;
	
	private:
		struct Private;
		std::shared_ptr<Private> d;
};

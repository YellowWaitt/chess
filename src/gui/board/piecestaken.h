#pragma once

#include <memory>

#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>

#include "chess/types.h"
#include "gui/thememanager.h"


class PiecesTaken : public Gtk::DrawingArea {
	public:
		PiecesTaken(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& builder);
		PiecesTaken(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& builder,
			std::shared_ptr<ThemeManager const> theme,
			std::shared_ptr<double const> pieceSize,
			Color color);
		
		void add(Piece const& piece);
		void add(PieceName const name);
		void clear();
		void remove(Piece const& piece);
		void remove(PieceName const name);
	
	protected:
		bool on_draw(Cairo::RefPtr<Cairo::Context> const& cr) override;
	
	private:
		struct Private;
		std::shared_ptr<Private> d;
};

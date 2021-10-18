#pragma once

#include <gtkmm/messagedialog.h>

#include "chess/game/chessgame.h"

class EndGameDialog : public Gtk::MessageDialog {
	public:
		EndGameDialog(Gtk::Widget& widget, Resultat resultat);
};

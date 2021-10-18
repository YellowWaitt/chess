#include "endgamedialog.h"


EndGameDialog::EndGameDialog(Gtk::Widget& widget, Resultat resultat)
: MessageDialog(*dynamic_cast<Gtk::Window*>(widget.get_toplevel()),
		"", false, Gtk::MessageType::MESSAGE_OTHER)
{
	set_title("Fin de la partie");
	switch (resultat) {
		case Resultat::Draw:
			set_message("Partie nulle");
			break;
		case Resultat::White:
			set_message("Victoire des blancs");
			break;
		case Resultat::Black:
			set_message("Victoire des noirs");
			break;
		default:
			set_message("La partie est pas finie ducon");
			break;
	}
}

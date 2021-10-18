#include "aidialog.h"

#include <gtkmm/dialog.h>
#include <gtkmm/scale.h>

#include "gui/util/connectbutton.h"
#include "gui/util/loadglade.h"


struct AiDialog::Private {
	Private(Gtk::Widget& widget, std::shared_ptr<ThemeManager> theme,
		std::shared_ptr<AI> ai);
	~Private();
	
	Gtk::Dialog* pDialog;
	Gtk::Scale* pScale;
	
	std::shared_ptr<AI> ai;
	
	void handle_ok();
};

/******************************************************************************/

AiDialog::Private::Private(Gtk::Widget& widget,
	std::shared_ptr<ThemeManager> theme,
	std::shared_ptr<AI> ai)
: ai(ai)
{
	auto builder = Gtk::Builder::create();
	loadGlade(builder, "/glade/ai.glade");
	
	builder->get_widget("topLevel", pDialog);
	pDialog->set_icon(theme->icon);
	pDialog->set_transient_for(*dynamic_cast<Gtk::Window*>(widget.get_toplevel()));
	
	builder->get_widget("scale", pScale);
	pScale->set_value(ai->getDepth());
	
	connectButton(builder, Private, ok);
}

AiDialog::Private::~Private() {
	delete pDialog;
}

/*----------------------------------------------------------------------------*/

AiDialog::AiDialog(Gtk::Widget& widget,
	std::shared_ptr<ThemeManager> theme,
	std::shared_ptr<AI> ai)
: d(std::make_shared<Private>(widget, theme, ai))
{}

/******************************************************************************/

void AiDialog::run() {
	d->pDialog->run();
}

/******************************************************************************/

void AiDialog::Private::handle_ok() {
	ai->setDepth(pScale->get_value());
	pDialog->hide();
}

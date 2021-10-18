#include "themedialog.h"

#include <gtkmm/dialog.h>
#include <gtkmm/drawingarea.h>

#include "gui/util/connectbutton.h"
#include "gui/util/loadglade.h"
#include "gui/util/widgetdrawer.h"


struct ThemeDialog::Private {
	Private(Gtk::Widget& widget, std::shared_ptr<ThemeManager> theme);
	~Private();
	
	std::shared_ptr<ThemeManager> theme;
	ThemeManager current;
	Gtk::Dialog* pDialog;
	
	void handle_ok();
	void handle_default_();
	void handle_cancel();
};

/******************************************************************************/

ThemeDialog::Private::Private(Gtk::Widget& widget,
	std::shared_ptr<ThemeManager> theme)
: theme(theme),
  current(*theme)
{
	auto builder = Gtk::Builder::create();
	loadGlade(builder, "/glade/theme.glade");
	
	builder->get_widget("topLevel", pDialog);
	pDialog->set_icon(theme->icon);
	pDialog->set_transient_for(*dynamic_cast<Gtk::Window*>(widget.get_toplevel()));
	
	{
	Gtk::Widget* ptr;
	builder->get_widget("background", ptr);
	WidgetDrawer::connect(ptr, &theme->background);
	builder->get_widget("chessboard", ptr);
	WidgetDrawer::connect(ptr, &theme->chessboard);
	builder->get_widget("pieces", ptr);
	WidgetDrawer::connect(ptr, theme);
	}
	
	connectButton(builder, Private, ok);
	connectButton(builder, Private, default_);
	connectButton(builder, Private, cancel);
}

ThemeDialog::Private::~Private() {
	delete pDialog;
}

/*----------------------------------------------------------------------------*/

ThemeDialog::ThemeDialog(Gtk::Widget& widget,
	std::shared_ptr<ThemeManager> theme)
: d(std::make_shared<Private>(widget, theme))
{}

/******************************************************************************/

void ThemeDialog::run() {
	d->pDialog->run();
}

/******************************************************************************/

void ThemeDialog::Private::handle_ok() {
	*theme = current;
	pDialog->hide();
}

void ThemeDialog::Private::handle_default_() {
	current = ThemeManager();
}

void ThemeDialog::Private::handle_cancel() {
	pDialog->hide();
}

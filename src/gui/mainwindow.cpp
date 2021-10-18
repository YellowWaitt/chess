#include "mainwindow.h"

#include "board/chessboardarea.h"
#include "dialogs/aidialog.h"
#include "dialogs/themedialog.h"
#include "util/connectmenuitem.h"
#include "util/widgetdrawer.h"


struct MainWindow::Private {
	Private(MainWindow* mw, Glib::RefPtr<Gtk::Builder> const& builder);
	
	MainWindow* q;
	
	std::shared_ptr<ThemeManager> theme;
	std::shared_ptr<GameManager> game;
	
	void handle_editTheme();
	void handle_aiSettings();
};

/******************************************************************************/

MainWindow::Private::Private(MainWindow* mw, Glib::RefPtr<Gtk::Builder> const& builder)
: q(mw),
  theme(std::make_shared<ThemeManager>()),
  game(std::make_shared<GameManager>(builder))
{
	ChessboardArea* pChessboard;
	builder->get_widget_derived("chessBoard", pChessboard, theme, game);
	
	Gtk::Widget* pBackground;
	builder->get_widget("background", pBackground);
	WidgetDrawer::connect(pBackground, &theme->background);
	
	connectMenuItem(builder, Private, editTheme);
	connectMenuItem(builder, Private, aiSettings);
}

/*----------------------------------------------------------------------------*/

MainWindow::MainWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& builder)
: Gtk::Window(cobject),
  d(std::make_shared<Private>(this, builder))
{
	connectMenuItem(builder, MainWindow, quit, hide);
	set_icon(d->theme->icon);
}

/******************************************************************************/

void MainWindow::Private::handle_editTheme() {
	ThemeDialog dialog(*q, theme);
	dialog.run();
}

void MainWindow::Private::handle_aiSettings() {
	AiDialog dialog(*q, theme, game->getAi());
	dialog.run();
}

#include <gtkmm/application.h>
#include <gtkmm/builder.h>

#include "gui/board/chessboardarea.h"
#include "gui/mainwindow.h"
#include "gui/util/loadglade.h"


template <typename T>
inline void createDummy() {
	T* dummy = new T();
	delete dummy;
}

int main(int argc, char **argv) {
	auto app = Gtk::Application::create(argc, argv);
	
	createDummy<ChessboardArea>();
	
	auto builder = Gtk::Builder::create();
	loadGlade(builder, "/glade/gui.glade");
	
	MainWindow* pWindow = nullptr;
	builder->get_widget_derived("mainWindow", pWindow);
	
	if (pWindow) {
		app->run(*pWindow);
	}
	
	delete pWindow;
	
	return 0;
}

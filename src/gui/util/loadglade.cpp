#include "loadglade.h"

#include <iostream>

#include <glibmm.h>



int loadGlade(Glib::RefPtr<Gtk::Builder>& builder, std::string const& fileName) {
	try {
#ifdef USE_RESOURCES
		builder->add_from_resource(fileName);
#else
		builder->add_from_file("resources" + fileName);
#endif
	} catch(Gio::ResourceError const& ex) {
		std::cerr << "ResourceError: " << ex.what() << std::endl;
		return 1;
	} catch(Glib::FileError const& ex) {
		std::cerr << "FileError: " << ex.what() << std::endl;
		return 1;
	} catch(Glib::MarkupError const& ex) {
		std::cerr << "MarkupError: " << ex.what() << std::endl;
		return 1;
	} catch(Gtk::BuilderError const& ex) {
		std::cerr << "BuilderError: " << ex.what() << std::endl;
		return 1;
	}
	return 0;
}

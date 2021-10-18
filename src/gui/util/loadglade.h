#pragma once

#include <string>

#include <gtkmm/builder.h>


int loadGlade(Glib::RefPtr<Gtk::Builder>& builder, std::string const& fileName);

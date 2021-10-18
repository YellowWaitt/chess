#pragma once

#include <gtkmm/menuitem.h>

#define __connectMenuItem4(builder, class, name, fun) \
	{ \
	Gtk::MenuItem* __p_##name = nullptr; \
	builder->get_widget(#name, __p_##name); \
	__p_##name->signal_activate().connect(sigc::mem_fun(*this, &class::fun)); \
	}

#define __connectMenuItem3(builder, class, name) \
	__connectMenuItem4(builder, class, name, handle_##name)

#define __connectMenuItem(_1, _2, _3, _4, macro, ...) macro

#define connectMenuItem(...) __connectMenuItem(__VA_ARGS__, \
	__connectMenuItem4, \
	__connectMenuItem3) \
	(__VA_ARGS__)

#pragma once

#include <gtkmm/button.h>

#define __connectButton4(builder, class, name, fun) \
	{ \
	Gtk::Button* __p_##name = nullptr; \
	builder->get_widget(#name, __p_##name); \
	__p_##name->signal_clicked().connect(sigc::mem_fun(*this, &class::fun)); \
	}

#define __connectButton3(builder, class, name) \
	__connectButton4(builder, class, name, handle_##name)

#define __connectButton(_1, _2, _3, _4, macro, ...) macro

#define connectButton(...) __connectButton(__VA_ARGS__, \
	__connectButton4, \
	__connectButton3) \
	(__VA_ARGS__)

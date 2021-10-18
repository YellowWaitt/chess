#pragma once

#include <gtkmm/radiomenuitem.h>

#define __connectRadioMenuItem5(builder, grp, class, name, fun) \
	{ \
	Gtk::RadioMenuItem* __p_##name = nullptr; \
	builder->get_widget(#name, __p_##name); \
	__p_##name->join_group(grp); \
	__p_##name->signal_activate().connect(sigc::mem_fun(*this, &class::fun)); \
	}

#define __connectRadioMenuItem4(builder, grp, class, name) \
	__connectRadioMenuItem5(builder, grp, class, name, handle_##name)

#define __connectRadioMenuItem(_1, _2, _3, _4, _5, macro, ...) macro

#define connectRadioMenuItem(...) __connectRadioMenuItem(__VA_ARGS__, \
	__connectRadioMenuItem5, \
	__connectRadioMenuItem4) \
	(__VA_ARGS__)

#pragma once

#include <gtkmm/checkmenuitem.h>

#define __connectCheckMenuItem5(builder, class, name, fun, bind) \
	{ \
	Gtk::CheckMenuItem* __p_##name = nullptr; \
	builder->get_widget(#name, __p_##name); \
	auto __proxy = do_##name.get_proxy(); \
	auto __proxy_active = __p_##name->property_active(); \
	do_##name.set_value(__proxy_active.get_value()); \
	bind = Glib::Binding::bind_property_value(__proxy_active, __proxy, \
		Glib::BindingFlags::BINDING_BIDIRECTIONAL); \
	__proxy.signal_changed().connect(sigc::mem_fun(*this, &class::fun)); \
	}

#define __connectCheckMenuItem4(builder, class, name, fun) \
	__connectCheckMenuItem5(builder, class, name, fun, bind_##name)

#define __connectCheckMenuItem3(builder, class, name) \
	__connectCheckMenuItem4(builder, class, name, switch_##name)

#define __connectCheckMenuItem(_1, _2, _3, _4, _5, macro, ...) macro

#define connectCheckMenuItem(...) __connectCheckMenuItem(__VA_ARGS__, \
	__connectCheckMenuItem5, \
	__connectCheckMenuItem4, \
	__connectCheckMenuItem3) \
	(__VA_ARGS__)

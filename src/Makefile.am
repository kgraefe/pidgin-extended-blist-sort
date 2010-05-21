plugindir = $(libdir)/pidgin
DEFS = -DLOCALEDIR=\"$(localedir)\"

plugin_LTLIBRARIES = extended_blist_sort.la
extended_blist_sort_la_SOURCES = \
	extended_blist_sort.c \
	extended_menu.c \
	prefs.c \
	sort_methods.c \
	tooltip.c \
	pidgin_internals.c

extended_blist_sort_la_LDFLAGS = -module -avoid-version


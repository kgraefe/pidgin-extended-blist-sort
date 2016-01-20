/*
 * Extended BList Sort
 * Copyright (C) 2008-2016 Konrad Gräfe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1301, USA.
 */

#include "extended_blist_sort.h"

#include "sort_methods.h"
#include "prefs.h"
#include "extended_menu.h"
#include "tooltip.h"
#include "plugin_actions.h"

#include <version.h>
#include <gtkplugin.h>
#include <util.h>

PurplePlugin *plugin;

static gboolean plugin_load(PurplePlugin *_plugin) {
	plugin = _plugin;
	
	init_sort_methods();
	init_prefs();
	init_extended_menu();
	init_tooltip();
	
	
	return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin) {
	uninit_sort_methods();
	
	return TRUE;
}

static PidginPluginUiInfo ui_info = {
	get_pref_frame,
	0,   /* page_num (Reserved) */
	/* Padding */
	NULL,
	NULL,
	NULL,
	NULL
};

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,		/**< type           */
	PIDGIN_PLUGIN_TYPE,		/**< ui_requirement */
	0,				/**< flags          */
	NULL,				/**< dependencies   */
	PURPLE_PRIORITY_DEFAULT,	/**< priority       */

	PLUGIN_ID,			/**< id             */
	NULL,				/**< name           */
	PLUGIN_VERSION,			/**< version        */
	NULL,				/**  summary        */
				
	NULL,				/**  description    */
	PLUGIN_AUTHOR,			/**< author         */
	PLUGIN_WEBSITE,			/**< homepage       */

	plugin_load,			/**< load           */
	plugin_unload,			/**< unload         */
	NULL,				/**< destroy        */

	&ui_info,			/**< ui_info        */
	NULL,				/**< extra_info     */
	NULL,				/**< prefs_info     */
	plugin_actions,			/**< actions        */
	
	/* padding */
	NULL,
	NULL,
	NULL,
	NULL
};

static void init_plugin(PurplePlugin *plugin) {
	const char *str = "Extended BList Sort";
	gchar *plugins_locale_dir;

#ifdef ENABLE_NLS
	plugins_locale_dir = g_build_filename(purple_user_dir(), "locale", NULL);

	bindtextdomain(GETTEXT_PACKAGE, plugins_locale_dir);
	if(str == _(str)) {
		bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
	}
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");

	g_free(plugins_locale_dir);
#endif /* ENABLE_NLS */

	info.name        = _("Extended BList Sort");
	info.summary     = _("This Plugin allows you to sort your buddy list more flexible than pidgin does.");
	info.description = _("This Plugin allows you to sort your buddy list more flexible than pidgin does.");

	purple_prefs_add_none(PLUGIN_PREFS_PREFIX);
	purple_prefs_add_int(PLUGIN_PREFS_PREFIX "/sort1", 0);
	purple_prefs_add_bool(PLUGIN_PREFS_PREFIX "/sort1_reverse", FALSE);
	purple_prefs_add_int(PLUGIN_PREFS_PREFIX "/sort2", 0);
	purple_prefs_add_bool(PLUGIN_PREFS_PREFIX "/sort2_reverse", FALSE);
	purple_prefs_add_int(PLUGIN_PREFS_PREFIX "/sort3", 0);
	purple_prefs_add_bool(PLUGIN_PREFS_PREFIX "/sort3_reverse", FALSE);
	purple_prefs_add_string(PLUGIN_PREFS_PREFIX "/old_sort_method", "");
}

PURPLE_INIT_PLUGIN(PLUGIN_STATIC_NAME, init_plugin, info)


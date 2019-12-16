/* Copyright (C) 2008-2019 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This software may be modified and distributed under the terms
 * of the GPL2.0 license. See the COPYING file for details.
 */

#include "plugin_actions.h"

#include "extended_blist_sort.h"

#include <gtkblist.h>

static void refresh_blist_cb(PurplePluginAction *action) {
	pidgin_blist_refresh(purple_get_blist());
}

GList *plugin_actions(PurplePlugin *plugin, gpointer context) {
	GList *l;
	PurplePluginAction *action;

	l = NULL;

	action = purple_plugin_action_new(_("Refresh Buddy List"), refresh_blist_cb);
	l = g_list_append(l, action);

	return l;
}


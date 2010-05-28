/*
 * Extended BList Sort
 * Copyright (C) 2008 Konrad Gräfe
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


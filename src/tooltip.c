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

#include "tooltip.h"

#include "extended_blist_sort.h"
#include "sort_methods.h"

#include <gtkblist.h>

extern PurplePlugin *plugin;

static void drawing_tooltip_cb(PurpleBlistNode *node, GString *str, gboolean full, void *data) {
	int priority;
	gchar *strPriority=NULL;
	
	if(!PURPLE_BLIST_NODE_IS_CONTACT(node) &&
	   !PURPLE_BLIST_NODE_IS_BUDDY(node) &&
	   !PURPLE_BLIST_NODE_IS_CHAT(node)) return;
	
	if(purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort1") != SORT_METHOD_PRIORITY &&
	   purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort2") != SORT_METHOD_PRIORITY &&
	   purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort3") != SORT_METHOD_PRIORITY) return;
	
	priority = purple_blist_node_get_int(node, "extended_sort_method_priority");

	if(priority == PRIORITY_UNDEFINED) return;
	if(priority == PRIORITY_VERY_LOW) strPriority=_("Very Low");
	if(priority == PRIORITY_LOW) strPriority=_("Low");
	if(priority == PRIORITY_NORMAL) return;
	if(priority == PRIORITY_HIGH) strPriority=_("High");
	if(priority == PRIORITY_VERY_HIGH) strPriority=_("Very High");
	
	g_string_append_printf(str, _("\n<b>Priority</b>: %s"), strPriority);
}

void init_tooltip(void) {
	purple_signal_connect(pidgin_blist_get_handle(), "drawing-tooltip", plugin, PURPLE_CALLBACK(drawing_tooltip_cb), NULL);
}


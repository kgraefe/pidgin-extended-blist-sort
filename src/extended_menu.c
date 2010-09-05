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

#include "extended_menu.h"

#include "extended_blist_sort.h"
#include "sort_methods.h"

#include <signals.h>
#include <blist.h>
#include <gtkblist.h>

extern PurplePlugin *plugin;

static void buddy_set_priority_cb(PurpleBlistNode *node, gpointer priority) {
	purple_blist_node_set_int(node, "extended_sort_method_priority", GPOINTER_TO_INT(priority));
	pidgin_blist_refresh(purple_get_blist());
}

static void extended_buddy_menu_cb(PurpleBlistNode *node, GList **menu) {
	PurpleMenuAction *action = NULL;
	GList *submenu = NULL;
	int priority;
	void (*callback)();
	
	if(!PURPLE_BLIST_NODE_IS_CONTACT(node) &&
	   !PURPLE_BLIST_NODE_IS_BUDDY(node) &&
	   !PURPLE_BLIST_NODE_IS_CHAT(node)) return;
	
	if(purple_blist_node_get_flags(node) & PURPLE_BLIST_NODE_FLAG_NO_SAVE) return;
	
	if(purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort1") != SORT_METHOD_PRIORITY &&
	   purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort2") != SORT_METHOD_PRIORITY &&
	   purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort3") != SORT_METHOD_PRIORITY) return;
	
	priority = purple_blist_node_get_int(node, "extended_sort_method_priority");
	
	
	if(priority == PRIORITY_VERY_HIGH) {
		// Disabled Menu Entry
		callback=NULL;
	} else {
		callback=buddy_set_priority_cb;
	}
	submenu = g_list_append(submenu, purple_menu_action_new(_("Very High"), PURPLE_CALLBACK(callback), GINT_TO_POINTER(PRIORITY_VERY_HIGH), NULL));
	
	if(priority == PRIORITY_HIGH) {
		callback=NULL;
	} else {
		callback=buddy_set_priority_cb;
	}
	submenu = g_list_append(submenu, purple_menu_action_new(_("High"), PURPLE_CALLBACK(callback), GINT_TO_POINTER(PRIORITY_HIGH), NULL));
	
	if(priority == PRIORITY_NORMAL || priority == PRIORITY_UNDEFINED) {
		callback=NULL;
	} else {
		callback=buddy_set_priority_cb;
	}
	submenu = g_list_append(submenu, purple_menu_action_new(_("Normal"), PURPLE_CALLBACK(callback), GINT_TO_POINTER(PRIORITY_NORMAL), NULL));
	
	if(priority == PRIORITY_LOW) {
		callback=NULL;
	} else {
		callback=buddy_set_priority_cb;
	}
	submenu = g_list_append(submenu, purple_menu_action_new(_("Low"), PURPLE_CALLBACK(callback), GINT_TO_POINTER(PRIORITY_LOW), NULL));
	
	if(priority == PRIORITY_VERY_LOW) {
		callback=NULL;
	} else {
		callback=buddy_set_priority_cb;
	}
	submenu = g_list_append(submenu, purple_menu_action_new(_("Very Low"), PURPLE_CALLBACK(callback), GINT_TO_POINTER(PRIORITY_VERY_LOW), NULL));
	
	
	action = purple_menu_action_new(_("Set Priority"), NULL, NULL, submenu);
	*menu = g_list_append(*menu, action);
}

void init_extended_menu(void) {
	purple_signal_connect(purple_blist_get_handle(), "blist-node-extended-menu", plugin, PURPLE_CALLBACK(extended_buddy_menu_cb), NULL);
	
}

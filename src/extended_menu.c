/* Copyright (C) 2008-2017 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This software may be modified and distributed under the terms
 * of the GPL2.0 license. See the COPYING file for details.
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

static GList *menu_append(GList *menu, PurpleBlistNode *node, int priority, const char *label) {
	int nodePriority;
	void (*callback)();

	nodePriority = purple_blist_node_get_int(node, "extended_sort_method_priority");

	if(nodePriority == priority) {
		/* Disabled menu entry */
		callback = NULL;
	} else if(nodePriority == PRIORITY_UNDEFINED && priority == PRIORITY_NORMAL) {
		callback = NULL;
	} else {
		callback = buddy_set_priority_cb;
	}

	return g_list_append(menu, purple_menu_action_new(
		label, PURPLE_CALLBACK(callback),
		GINT_TO_POINTER(priority), NULL
	));
}

static void extended_buddy_menu_cb(PurpleBlistNode *node, GList **menu) {
	PurpleMenuAction *action = NULL;
	GList *submenu = NULL;

	if(
		!PURPLE_BLIST_NODE_IS_CONTACT(node) &&
		!PURPLE_BLIST_NODE_IS_BUDDY(node) &&
		!PURPLE_BLIST_NODE_IS_CHAT(node)
	) {
		return;
	}

	if(purple_blist_node_get_flags(node) & PURPLE_BLIST_NODE_FLAG_NO_SAVE) {
		return;
	}

	if(
		purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort1") != SORT_METHOD_PRIORITY &&
		purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort2") != SORT_METHOD_PRIORITY &&
		purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort3") != SORT_METHOD_PRIORITY
	) {
		return;
	}

	submenu = menu_append(submenu, node, PRIORITY_VERY_HIGH, _("Very High"));
	submenu = menu_append(submenu, node, PRIORITY_HIGH, _("High"));
	submenu = menu_append(submenu, node, PRIORITY_NORMAL, _("Normal"));
	submenu = menu_append(submenu, node, PRIORITY_LOW, _("Low"));
	submenu = menu_append(submenu, node, PRIORITY_VERY_LOW, _("Very Low"));

	action = purple_menu_action_new(_("Set Priority"), NULL, NULL, submenu);
	*menu = g_list_append(*menu, action);
}

void init_extended_menu(void) {
	purple_signal_connect(
		purple_blist_get_handle(), "blist-node-extended-menu",
		plugin, PURPLE_CALLBACK(extended_buddy_menu_cb), NULL
	);
}

/* Copyright (C) 2008-2019 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This software may be modified and distributed under the terms
 * of the GPL2.0 license. See the COPYING file for details.
 */

#include "tooltip.h"

#include "extended_blist_sort.h"
#include "sort_methods.h"

#include <gtkblist.h>

extern PurplePlugin *plugin;

static void drawing_tooltip_cb(PurpleBlistNode *node, GString *str, gboolean full, void *data) {
	gchar *strPriority=NULL;
	
	if(
		!PURPLE_BLIST_NODE_IS_CONTACT(node) &&
		!PURPLE_BLIST_NODE_IS_BUDDY(node) &&
		!PURPLE_BLIST_NODE_IS_CHAT(node)
	) {
		return;
	}
	
	if(
		purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort1") != SORT_METHOD_PRIORITY &&
		purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort2") != SORT_METHOD_PRIORITY &&
		purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort3") != SORT_METHOD_PRIORITY
	) {
		return;
	}
	
	switch(purple_blist_node_get_int(node, "extended_sort_method_priority")) {
	case PRIORITY_VERY_LOW:
		strPriority = _("Very Low");
		break;
	case PRIORITY_LOW:
		strPriority = _("Low");
		break;
	case PRIORITY_HIGH:
		strPriority = _("High");
		break;
	case PRIORITY_VERY_HIGH:
		strPriority = _("Very High");
		break;
	case PRIORITY_NORMAL:
	case PRIORITY_UNDEFINED:
	default:
		return;
	}
	
	g_string_append_printf(str, _("\n<b>Priority</b>: %s"), strPriority);
}

void init_tooltip(void) {
	purple_signal_connect(
		pidgin_blist_get_handle(),
		"drawing-tooltip", plugin,
		PURPLE_CALLBACK(drawing_tooltip_cb), NULL
	);
}


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

// *includes* {
#include "config.h"

#define PLUGIN_PREFS_PREFIX "/plugins/gtk/extended_blist_sort"

#ifndef PURPLE_PLUGINS
#define PURPLE_PLUGINS
#endif

#include "internal.h"

#include <plugin.h>
#include <version.h>

#include <gtkblist.h>
#include <gtkdebug.h>
#include <gtkutils.h>
#include <gtkplugin.h>

#include <core.h>
// }


//<Original Pidgin-Code> {
static PidginBuddyList *gtkblist = NULL;
typedef enum {
	PIDGIN_BLIST_NODE_HAS_PENDING_MESSAGE    =  1 << 0,  /* Whether there's pending message in a conversation */
} PidginBlistNodeFlags;

typedef struct _pidgin_blist_node { 
	GtkTreeRowReference *row;
	gboolean contact_expanded;
	gboolean recent_signonoff;
	gint recent_signonoff_timer;
	struct {
		PurpleConversation *conv;
		time_t last_message;          /* timestamp for last displayed message */
		PidginBlistNodeFlags flags;
	} conv;
} PidginBlistNode;

static void sort_method_none(PurpleBlistNode *node, PurpleBuddyList *blist, GtkTreeIter parent_iter, GtkTreeIter *cur, GtkTreeIter *iter);
static gboolean get_iter_from_node(PurpleBlistNode *node, GtkTreeIter *iter);
static void sort_method_none(PurpleBlistNode *node, PurpleBuddyList *blist, GtkTreeIter parent_iter, GtkTreeIter *cur, GtkTreeIter *iter) {
	PurpleBlistNode *sibling = node->prev;
	GtkTreeIter sibling_iter;

	if (cur != NULL) {
		*iter = *cur;
		return;
	}

	while (sibling && !get_iter_from_node(sibling, &sibling_iter)) {
		sibling = sibling->prev;
	}

	gtk_tree_store_insert_after(gtkblist->treemodel, iter,
			node->parent ? &parent_iter : NULL,
			sibling ? &sibling_iter : NULL);
}

static gboolean get_iter_from_node(PurpleBlistNode *node, GtkTreeIter *iter) {
	struct _pidgin_blist_node *gtknode = (struct _pidgin_blist_node *)node->ui_data;
	GtkTreePath *path;

	if (!gtknode) {
		return FALSE;
	}

	if (!gtkblist) {
		purple_debug_error(PLUGIN_STATIC_NAME, "get_iter_from_node was called, but we don't seem to have a blist\n");
		return FALSE;
	}

	if (!gtknode->row)
		return FALSE;


	if ((path = gtk_tree_row_reference_get_path(gtknode->row)) == NULL)
		return FALSE;

	if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(gtkblist->treemodel), iter, path)) {
		gtk_tree_path_free(path);
		return FALSE;
	}
	gtk_tree_path_free(path);
	return TRUE;
}
// </Original Pidgin-Code> }


static PurplePlugin *sortMethodPlugin = NULL;
static PurpleBuddyList *blist;

enum sort_methods {
	SORT_METHOD_NOTHING,
	SORT_METHOD_NAME,
	SORT_METHOD_STATUS,
	SORT_METHOD_ONOFFLINE,
	SORT_METHOD_PROTOCOL,
	SORT_METHOD_PRIORITY,
	SORT_METHOD_ONOFFLINETIME,
	SORT_METHOD_LOGSIZE,
	SORT_METHOD_ACCOUNT
};

enum priorities{
	PRIORITY_UNDEFINED,
	PRIORITY_VERY_LOW,
	PRIORITY_LOW,
	PRIORITY_NORMAL,
	PRIORITY_HIGH,
	PRIORITY_VERY_HIGH
};


static void sort_method_ext_blist_sort(PurpleBlistNode *node, PurpleBuddyList *blist, GtkTreeIter groupiter, GtkTreeIter *cur, GtkTreeIter *iter);

static void reset_sort_method() {
	if(purple_utf8_strcasecmp(purple_prefs_get_string(PIDGIN_PREFS_ROOT "/blist/sort_type"), "ext_blist_sort")==0) {
		if(purple_utf8_strcasecmp(purple_prefs_get_string(PLUGIN_PREFS_PREFIX "/old_sort_method"), "")!=0) {
			purple_prefs_set_string(PIDGIN_PREFS_ROOT "/blist/sort_type", purple_prefs_get_string(PLUGIN_PREFS_PREFIX "/old_sort_method"));
		} else {
			purple_prefs_set_string(PIDGIN_PREFS_ROOT "/blist/sort_type", "none");
		}
	}
	
}

static void option_changed_cb() {
	pidgin_blist_refresh(blist);
}

static void buddy_set_priority_cb(PurpleBlistNode *node, gpointer priority) {
	purple_blist_node_set_int(node, "extended_sort_method_priority", GPOINTER_TO_INT(priority));
	pidgin_blist_refresh(blist);
}

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

static gboolean plugin_load(PurplePlugin *plugin) {
	sortMethodPlugin=plugin;
	
	blist = purple_get_blist();
	gtkblist=PIDGIN_BLIST(blist);
	
	if(purple_utf8_strcasecmp(purple_prefs_get_string(PIDGIN_PREFS_ROOT "/blist/sort_type"), "ext_blist_sort")!=0) {
		purple_prefs_set_string(PLUGIN_PREFS_PREFIX "/old_sort_method", purple_prefs_get_string(PIDGIN_PREFS_ROOT "/blist/sort_type"));
	}
	
	pidgin_blist_sort_method_reg("ext_blist_sort", _("Extended BList Sort"), sort_method_ext_blist_sort);
	
	purple_prefs_set_string(PIDGIN_PREFS_ROOT "/blist/sort_type", "ext_blist_sort");
	pidgin_blist_update_sort_methods();
	
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort1", option_changed_cb, NULL);
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort1_reverse", option_changed_cb, NULL);
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort2", option_changed_cb, NULL);
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort2_reverse", option_changed_cb, NULL);
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort3", option_changed_cb, NULL);
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort3_reverse", option_changed_cb, NULL);
	
	purple_signal_connect(purple_get_core(), "quitting", plugin, reset_sort_method, NULL);
	purple_signal_connect(purple_blist_get_handle(), "blist-node-extended-menu", plugin, PURPLE_CALLBACK(extended_buddy_menu_cb), NULL);
	purple_signal_connect(pidgin_blist_get_handle(), "drawing-tooltip", plugin, PURPLE_CALLBACK(drawing_tooltip_cb), NULL);
	
	return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin) {
	reset_sort_method();

	pidgin_blist_sort_method_unreg("ext_blist_sort");
	
	return TRUE;
}

static gint compare_nothing(PurpleBlistNode *node1, PurpleBlistNode *node2) {
	return 0;
}

static gint compare_name(PurpleBlistNode *node1, PurpleBlistNode *node2) {
	const char *name1=NULL, *name2=NULL;

	if(PURPLE_BLIST_NODE_IS_CONTACT(node1)) {
		name1 = purple_contact_get_alias((PurpleContact *)node1);
	}
	if(!name1) name1 = PURPLE_BLIST_NODE_NAME(node1);

	if(PURPLE_BLIST_NODE_IS_CONTACT(node2)) {
		name2 = purple_contact_get_alias((PurpleContact *)node2);
	}
	if(!name2) name2 = PURPLE_BLIST_NODE_NAME(node2);

	return purple_utf8_strcasecmp(name1, name2);
}

static gint compare_status(PurpleBlistNode *node1, PurpleBlistNode *node2) {
	PurplePresence *p1=NULL, *p2=NULL;

	if(!node1) return 0;

	if(PURPLE_BLIST_NODE_IS_CHAT(node1) && PURPLE_BLIST_NODE_IS_CHAT(node2)) {
		return 0;
	} else if(PURPLE_BLIST_NODE_IS_CHAT(node1)) {
		return 1;
	} else if(PURPLE_BLIST_NODE_IS_CHAT(node2)) {
		return -1;
	}

	if(PURPLE_BLIST_NODE_IS_CONTACT(node1)) {
		p1 = purple_buddy_get_presence(purple_contact_get_priority_buddy((PurpleContact *)node1));
	}

	if(node2 && PURPLE_BLIST_NODE_IS_CONTACT(node2)) {
		p2 = purple_buddy_get_presence(purple_contact_get_priority_buddy((PurpleContact *)node2));
	}

	return purple_presence_compare(p1, p2);
}

static gint compare_onoffline(PurpleBlistNode *node1, PurpleBlistNode *node2) {
	PurpleBuddy *buddy1=NULL, *buddy2=NULL;
	
	if(!node1) return 0;
	if(!node2) return 0;

	if(PURPLE_BLIST_NODE_IS_CHAT(node1) && PURPLE_BLIST_NODE_IS_CHAT(node2)) {
		return 0;
	} else if(PURPLE_BLIST_NODE_IS_CHAT(node1)) {
		return 1;
	} else if(PURPLE_BLIST_NODE_IS_CHAT(node2)) {
		return -1;
	}

	if(PURPLE_BLIST_NODE_IS_CONTACT(node1)) {
		buddy1 = purple_contact_get_priority_buddy((PurpleContact *)node1);
	} else if(PURPLE_BLIST_NODE_IS_BUDDY(node1)) {
		buddy1 = (PurpleBuddy *)node1;
	}

	if(PURPLE_BLIST_NODE_IS_CONTACT(node2)) {
		buddy2 = purple_contact_get_priority_buddy((PurpleContact *)node2);
	} else if(PURPLE_BLIST_NODE_IS_BUDDY(node2)) {
		buddy2 = (PurpleBuddy *)node2;
	}

	if(PURPLE_BUDDY_IS_ONLINE(buddy1)) {
		if(PURPLE_BUDDY_IS_ONLINE(buddy2)) {
			return 0;
		} else {
			return -1;
		}
	} else {
		if(PURPLE_BUDDY_IS_ONLINE(buddy2)) {
			return 1;
		} else {
			return 0;
		}
	}

	return 0;
}

static gint compare_protocol(PurpleBlistNode *node1, PurpleBlistNode *node2) {
	PurpleAccount *acc1=NULL, *acc2=NULL;

	if(!node1) return 0;

	if(PURPLE_BLIST_NODE_IS_CHAT(node1)) {
		acc1 = ((PurpleChat *)node1)->account;
	} else if(PURPLE_BLIST_NODE_IS_BUDDY(node1)) {
		acc1 = ((PurpleBuddy *)node1)->account;
	} else if(PURPLE_BLIST_NODE_IS_CONTACT(node1)) {
		acc1 = (purple_contact_get_priority_buddy((PurpleContact *)node1))->account;
	}

	if(node2) {
		if(PURPLE_BLIST_NODE_IS_CHAT(node2)) {
			acc2 = ((PurpleChat *)node2)->account;
		} else if(PURPLE_BLIST_NODE_IS_BUDDY(node2)) {
			acc2 = ((PurpleBuddy *)node2)->account;
		} else if(PURPLE_BLIST_NODE_IS_CONTACT(node2)) {
			acc2 = (purple_contact_get_priority_buddy((PurpleContact *)node2))->account;
		}
	}

	return purple_utf8_strcasecmp(
		purple_account_get_protocol_id(acc1),
		acc2 ? purple_account_get_protocol_id(acc2) : NULL
	);
}

static gint compare_priority(PurpleBlistNode *node1, PurpleBlistNode *node2) {
	int p1, p2;
	
	if(!node1) return 0;
	
	if(PURPLE_BLIST_NODE_IS_CONTACT(node1)) {
		p1 = purple_blist_node_get_int((PurpleBlistNode *)purple_contact_get_priority_buddy((PurpleContact *)node1), "extended_sort_method_priority");
	} else {
		p1 = purple_blist_node_get_int(node1, "extended_sort_method_priority");
	}

	if(PURPLE_BLIST_NODE_IS_CONTACT(node2)) {
		p2 = purple_blist_node_get_int((PurpleBlistNode *)purple_contact_get_priority_buddy((PurpleContact *)node2), "extended_sort_method_priority");
	} else {
		p2 = purple_blist_node_get_int(node2, "extended_sort_method_priority");
	}
	
	if(p1 == PRIORITY_UNDEFINED) p1=PRIORITY_NORMAL;
	if(p2 == PRIORITY_UNDEFINED) p2=PRIORITY_NORMAL;
	
	if(p1 > p2) return -1;
	if(p2 > p1) return 1;
	return 0;
}

static gint compare_onofflinetime(PurpleBlistNode *node1, PurpleBlistNode *node2) {
	PurpleBuddy *buddy1=NULL, *buddy2=NULL;
	int t1=0, t2=0;
	
	if(!node1) return 0;

	if(PURPLE_BLIST_NODE_IS_CHAT(node1) && PURPLE_BLIST_NODE_IS_CHAT(node2)) {
		return 0;
	} else if(PURPLE_BLIST_NODE_IS_CHAT(node1)) {
		return 1;
	} else if(PURPLE_BLIST_NODE_IS_CHAT(node2)) {
		return -1;
	}

	if(PURPLE_BLIST_NODE_IS_CONTACT(node1)) {
		buddy1 = purple_contact_get_priority_buddy((PurpleContact *)node1);
	} else if(PURPLE_BLIST_NODE_IS_BUDDY(node1)) {
		buddy1 = (PurpleBuddy *)node1;
	}
	if(PURPLE_BUDDY_IS_ONLINE(buddy1)) {
		t1 = purple_presence_get_login_time(purple_buddy_get_presence(buddy1));
	} else {
		t1 = purple_blist_node_get_int((PurpleBlistNode *)buddy1, "last_seen");
	}

	if(PURPLE_BLIST_NODE_IS_CONTACT(node2)) {
		buddy2 = purple_contact_get_priority_buddy((PurpleContact *)node2);
	} else if(PURPLE_BLIST_NODE_IS_BUDDY(node2)) {
		buddy2 = (PurpleBuddy *)node2;
	}
	if(PURPLE_BUDDY_IS_ONLINE(buddy2)) {
		t2 = purple_presence_get_login_time(purple_buddy_get_presence(buddy2));
	} else {
		t2 = purple_blist_node_get_int((PurpleBlistNode *)buddy2, "last_seen");
	}
	
	if(t1 > t2) return -1;
	if(t2 > t1) return 1;
	return 0;
}

static gint compare_logsize(PurpleBlistNode *node1, PurpleBlistNode *node2) {
	PurpleBlistNode *child;
	int logsize1=0, logsize2=0;
	
	if(PURPLE_BLIST_NODE_IS_CONTACT(node1)) {
		for (child = node1->child; child; child = child->next) {
			if(PURPLE_BLIST_NODE_IS_BUDDY(child)) {
				logsize1 += purple_log_get_total_size(PURPLE_LOG_IM, ((PurpleBuddy*)(child))->name, ((PurpleBuddy*)(child))->account);
			}
		}
	}
	if(PURPLE_BLIST_NODE_IS_BUDDY(node1)) {
		logsize1 = purple_log_get_total_size(PURPLE_LOG_IM, PURPLE_BLIST_NODE_NAME(node1), ((PurpleBuddy *)node1)->account);
	}
	if(PURPLE_BLIST_NODE_IS_CHAT(node1)) {
		logsize1 = purple_log_get_total_size(PURPLE_LOG_CHAT, PURPLE_BLIST_NODE_NAME(node1), ((PurpleChat *)node1)->account);
	}

	if(PURPLE_BLIST_NODE_IS_CONTACT(node2)) {
		for (child = node2->child; child; child = child->next) {
			if(PURPLE_BLIST_NODE_IS_BUDDY(child)) {
				logsize2 += purple_log_get_total_size(PURPLE_LOG_IM, ((PurpleBuddy*)(child))->name, ((PurpleBuddy*)(child))->account);
			}
		}
	}
	if(PURPLE_BLIST_NODE_IS_BUDDY(node2)) {
		logsize2 = purple_log_get_total_size(PURPLE_LOG_IM, PURPLE_BLIST_NODE_NAME(node2), ((PurpleBuddy *)node2)->account);
	}
	if(PURPLE_BLIST_NODE_IS_CHAT(node2)) {
		logsize2 = purple_log_get_total_size(PURPLE_LOG_CHAT, PURPLE_BLIST_NODE_NAME(node2), ((PurpleChat *)node2)->account);
	}
	
	if(logsize1 > logsize2) return -1;
	if(logsize2 > logsize1) return 1;
	return 0;
}

static gint compare_account(PurpleBlistNode *node1, PurpleBlistNode *node2) {
	GList *cur;
	PurpleAccount *acc1, *acc2, *account;

	if(!node1 || !node2) return 0;

	if(PURPLE_BLIST_NODE_IS_CHAT(node1)) {
		acc1 = ((PurpleChat *)node1)->account;
	} else if(PURPLE_BLIST_NODE_IS_BUDDY(node1)) {
		acc1 = ((PurpleBuddy *)node1)->account;
	} else if(PURPLE_BLIST_NODE_IS_CONTACT(node1)) {
		acc1 = (purple_contact_get_priority_buddy((PurpleContact *)node1))->account;
	}

	if(PURPLE_BLIST_NODE_IS_CHAT(node2)) {
		acc2 = ((PurpleChat *)node2)->account;
	} else if(PURPLE_BLIST_NODE_IS_BUDDY(node2)) {
		acc2 = ((PurpleBuddy *)node2)->account;
	} else if(PURPLE_BLIST_NODE_IS_CONTACT(node2)) {
		acc2 = (purple_contact_get_priority_buddy((PurpleContact *)node2))->account;
	}

	for(cur = purple_accounts_get_all(); cur; cur = cur->next) {
		account = cur->data;
		if(account == acc1 && account == acc2) {
			return 0;
		} else if(account == acc1) {
			return -1;
		} else if(account == acc2) {
			return 1;
		}
	}

	return 0;
}

static gint compare(gint sort_method, gboolean reverse, PurpleBlistNode *node1, PurpleBlistNode *node2) {
	gint ret = 0;
	
	if (sort_method == SORT_METHOD_NOTHING) ret = compare_nothing(node1, node2);
	if (sort_method == SORT_METHOD_NAME) ret = compare_name(node1, node2);
	if (sort_method == SORT_METHOD_STATUS) ret = compare_status(node1, node2);
	if (sort_method == SORT_METHOD_ONOFFLINE) ret = compare_onoffline(node1, node2);
	if (sort_method == SORT_METHOD_PROTOCOL) ret = compare_protocol(node1, node2);
	if (sort_method == SORT_METHOD_PRIORITY) ret = compare_priority(node1, node2);
	if (sort_method == SORT_METHOD_ONOFFLINETIME) ret = compare_onofflinetime(node1, node2);
	if (sort_method == SORT_METHOD_LOGSIZE) ret = compare_logsize(node1, node2);
	if (sort_method == SORT_METHOD_ACCOUNT) ret = compare_account(node1, node2);
	
	if(reverse) ret *= (-1);
	
	return ret;
}

static void sort_method_ext_blist_sort(PurpleBlistNode *node, PurpleBuddyList *blist, GtkTreeIter group, GtkTreeIter *cur, GtkTreeIter *ret) {
	GtkTreeIter tmp_iter;
	

	if(!PURPLE_BLIST_NODE_IS_CONTACT(node) && !PURPLE_BLIST_NODE_IS_CHAT(node)) {
		sort_method_none(node, blist, group, cur, ret);
                return;	
	}

	// Erstes Kind der Gruppe holen, falls keine Kinder vorhanden sofort einfügen
	if (!gtk_tree_model_iter_children(GTK_TREE_MODEL(gtkblist->treemodel), &tmp_iter, &group)) {
        	gtk_tree_store_insert(gtkblist->treemodel, ret, &group, 0);
        	return;
        }

	// Alle Kinder durchgehen
	do {
		GValue val;
		PurpleBlistNode *n;
		gint cmp1, cmp2, cmp3;

		// BlistNode aus "TreeModelNode" holen
		val.g_type=0;
		gtk_tree_model_get_value(GTK_TREE_MODEL(gtkblist->treemodel), &tmp_iter, NODE_COLUMN, &val);
		n = g_value_get_pointer(&val);

		// vergleichen...
		cmp1=compare(purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort1"), purple_prefs_get_bool(PLUGIN_PREFS_PREFIX "/sort1_reverse"), node, n);
		cmp2=compare(purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort2"), purple_prefs_get_bool(PLUGIN_PREFS_PREFIX "/sort2_reverse"), node, n);
		cmp3=compare(purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort3"), purple_prefs_get_bool(PLUGIN_PREFS_PREFIX "/sort3_reverse"), node, n);

		// Kommt der Knoten vor den aktuellen?
		if(cmp1 < 0 ||
			(cmp1 == 0 &&
				(cmp2 < 0 ||
					(cmp2 == 0 &&
						(cmp3 < 0 ||
							(cmp3 == 0 && node < n)))))) {
			if(cur == NULL) {
				gtk_tree_store_insert_before(gtkblist->treemodel, ret, &group, &tmp_iter);
			} else {
				gtk_tree_store_move_before(gtkblist->treemodel, cur, &tmp_iter);
				*ret = *cur;
			}
			return;
		}

		g_value_unset(&val);

	// Gehe zum nächsten Knoten
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(gtkblist->treemodel), &tmp_iter));

	// Ans Ende einfügen
	if(cur == NULL) {
		gtk_tree_store_append(gtkblist->treemodel, ret, &group);
	} else {
		gtk_tree_store_move_before(gtkblist->treemodel, cur, NULL);
		*ret = *cur;
        }
        return;
	
}

/*** Einstellungen ***/
enum pref_columns {
	PREF_LIST_COL_LABEL,
	PREF_LIST_COL_VALUE
};

static void toggle_cb(GtkWidget *widget, gpointer data) {
	gboolean value;
	gchar *pref;

	value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	pref = (gchar *) data;

	purple_prefs_set_bool(pref, value);
}

static void combo_changed_cb(GtkWidget *widget, gpointer data) {
	GtkTreeModel *model = NULL;
	GtkTreeIter iter;
	
	gint value;
	gchar *pref;
	
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widget), &iter);
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(widget));
	
	gtk_tree_model_get(model, &iter, PREF_LIST_COL_VALUE, &value, -1);
	pref = (gchar *) data;
	
	purple_prefs_set_int(pref, value);
}

static gboolean get_iter_by_sort_method_id(GtkTreeModel *model, gint id, GtkTreeIter *iter) {
	gint current_id;
	
	if(!gtk_tree_model_get_iter_first(model, iter)) return FALSE;
	while(iter) {
		gtk_tree_model_get(model, iter, PREF_LIST_COL_VALUE, &current_id, -1);
		if(id == current_id) {
			return TRUE;
		}
		gtk_tree_model_iter_next(model, iter);
	}
	return FALSE;
}

static GtkWidget *get_pref_frame(PurplePlugin *plugin) {
	GtkWidget *ret = NULL;
	GtkWidget *label = NULL;
	GtkWidget *table = NULL;
	GtkWidget *combo = NULL;
	GtkWidget *toggle = NULL;
	GtkCellRenderer *renderer = NULL;
	
	GtkListStore *model = NULL;
	GtkTreeIter iter;
	
	gchar *markup;
	gint current_row = 0;
	
	model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
				PREF_LIST_COL_LABEL, _("(Nothing)"),
				PREF_LIST_COL_VALUE, SORT_METHOD_NOTHING,
				-1);
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
				PREF_LIST_COL_LABEL, _("Log Size"),
				PREF_LIST_COL_VALUE, SORT_METHOD_LOGSIZE,
				-1);
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
				PREF_LIST_COL_LABEL, _("Name"),
				PREF_LIST_COL_VALUE, SORT_METHOD_NAME,
				-1);
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
				PREF_LIST_COL_LABEL, _("Online/Offline"),
				PREF_LIST_COL_VALUE, SORT_METHOD_ONOFFLINE,
				-1);
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
				PREF_LIST_COL_LABEL, _("On-/Offline Time"),
				PREF_LIST_COL_VALUE, SORT_METHOD_ONOFFLINETIME,
				-1);
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
				PREF_LIST_COL_LABEL, _("Priority"),
				PREF_LIST_COL_VALUE, SORT_METHOD_PRIORITY,
				-1);
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
				PREF_LIST_COL_LABEL, _("Protocol"),
				PREF_LIST_COL_VALUE, SORT_METHOD_PROTOCOL,
				-1);
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
				PREF_LIST_COL_LABEL, _("Account"),
				PREF_LIST_COL_VALUE, SORT_METHOD_ACCOUNT,
				-1);
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
				PREF_LIST_COL_LABEL, _("Status"),
				PREF_LIST_COL_VALUE, SORT_METHOD_STATUS,
				-1);
	
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width(GTK_CONTAINER(ret), 12);
	
	label = gtk_label_new(NULL);
	markup = g_markup_printf_escaped("<span weight=\"bold\">%s</span>", _("Choose your way to sort the Buddy List:"));
	gtk_label_set_markup(GTK_LABEL(label), markup);
	gtk_container_add(GTK_CONTAINER(ret), label);
	
	table = gtk_table_new(3, 2, FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table), 5);
	gtk_table_set_col_spacings(GTK_TABLE(table), 5);
	gtk_container_add(GTK_CONTAINER(ret), table);
	
	current_row = 0;
	
	/* erste Sortierung */
	label = gtk_label_new(_("First:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.0f, 0.5f);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, current_row, current_row + 1);
	
	combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(combo), renderer, "text", PREF_LIST_COL_LABEL, NULL);
	
	if(get_iter_by_sort_method_id(GTK_TREE_MODEL(model), purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort1"), &iter)) {
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combo), &iter);
	}
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(combo_changed_cb), PLUGIN_PREFS_PREFIX "/sort1");
	
	gtk_table_attach_defaults(GTK_TABLE(table), combo, 1, 2, current_row, current_row + 1);
	
	toggle = gtk_check_button_new_with_mnemonic(_("Reverse"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), purple_prefs_get_bool(PLUGIN_PREFS_PREFIX "/sort1_reverse"));
	g_signal_connect(G_OBJECT(toggle), "toggled", G_CALLBACK(toggle_cb), PLUGIN_PREFS_PREFIX "/sort1_reverse");
	gtk_table_attach_defaults(GTK_TABLE(table), toggle, 1, 2, current_row + 1, current_row + 2);
	
	current_row+=2;
	
	/* zweite Sortierung */
	label = gtk_label_new(_("Second:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.0f, 0.5f);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, current_row, current_row + 1);
	
	combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(combo), renderer, "text", PREF_LIST_COL_LABEL, NULL);
	
	if(get_iter_by_sort_method_id(GTK_TREE_MODEL(model), purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort2"), &iter)) {
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combo), &iter);
	}
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(combo_changed_cb), PLUGIN_PREFS_PREFIX "/sort2");
	
	gtk_table_attach_defaults(GTK_TABLE(table), combo, 1, 2, current_row, current_row + 1);
	
	toggle = gtk_check_button_new_with_mnemonic(_("Reverse"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), purple_prefs_get_bool(PLUGIN_PREFS_PREFIX "/sort2_reverse"));
	g_signal_connect(G_OBJECT(toggle), "toggled", G_CALLBACK(toggle_cb), PLUGIN_PREFS_PREFIX "/sort2_reverse");
	gtk_table_attach_defaults(GTK_TABLE(table), toggle, 1, 2, current_row + 1, current_row + 2);
	
	current_row+=2;
	
	/* dritte Sortierung */
	label = gtk_label_new(_("Last:"));
	gtk_misc_set_alignment(GTK_MISC(label), 0.0f, 0.5f);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, current_row, current_row + 1);
	
	combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(combo), renderer, "text", PREF_LIST_COL_LABEL, NULL);
	
	if(get_iter_by_sort_method_id(GTK_TREE_MODEL(model), purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort3"), &iter)) {
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combo), &iter);
	}
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(combo_changed_cb), PLUGIN_PREFS_PREFIX "/sort3");
	
	gtk_table_attach_defaults(GTK_TABLE(table), combo, 1, 2, current_row, current_row + 1);
	
	toggle = gtk_check_button_new_with_mnemonic(_("Reverse"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), purple_prefs_get_bool(PLUGIN_PREFS_PREFIX "/sort3_reverse"));
	g_signal_connect(G_OBJECT(toggle), "toggled", G_CALLBACK(toggle_cb), PLUGIN_PREFS_PREFIX "/sort3_reverse");
	gtk_table_attach_defaults(GTK_TABLE(table), toggle, 1, 2, current_row + 1, current_row + 2);
	
	current_row+=2;
	
	return ret;
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
	PURPLE_PLUGIN_STANDARD,								/**< type           */
	PIDGIN_PLUGIN_TYPE,									/**< ui_requirement */
	0,													/**< flags          */
	NULL,												/**< dependencies   */
	PURPLE_PRIORITY_DEFAULT,							/**< priority       */

	PLUGIN_ID,											/**< id             */
	NULL,												/**< name           */
	PLUGIN_VERSION,										/**< version        */
	NULL,												/**  summary        */
				
	NULL,												/**  description    */
	PLUGIN_AUTHOR,										/**< author         */
	PLUGIN_WEBSITE,
														/**< homepage       */

	plugin_load,										/**< load           */
	plugin_unload,										/**< unload         */
	NULL,												/**< destroy        */

	&ui_info,											/**< ui_info        */
	NULL,												/**< extra_info     */
	NULL,												/**< prefs_info     */
	NULL,												/**< actions        */
	
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

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

#include "sort_methods.h"

#include "extended_blist_sort.h"

#include "pidgin_internals.h"

#include <gtkblist.h>

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
	PurpleAccount *acc1=NULL, *acc2=NULL, *account;

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
	PidginBuddyList *gtkblist;

	gtkblist = PIDGIN_BLIST(purple_get_blist());
	

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

static void reset_sort_method() {
	if(purple_utf8_strcasecmp(purple_prefs_get_string(PIDGIN_PREFS_ROOT "/blist/sort_type"), "ext_blist_sort")==0) {
		if(purple_utf8_strcasecmp(purple_prefs_get_string(PLUGIN_PREFS_PREFIX "/old_sort_method"), "")!=0) {
			purple_prefs_set_string(PIDGIN_PREFS_ROOT "/blist/sort_type", purple_prefs_get_string(PLUGIN_PREFS_PREFIX "/old_sort_method"));
		} else {
			purple_prefs_set_string(PIDGIN_PREFS_ROOT "/blist/sort_type", "none");
		}
	}
	
}

void init_sort_methods(void) {
	pidgin_blist_sort_method_reg("ext_blist_sort", _("Extended BList Sort"), sort_method_ext_blist_sort);
}

void uninit_sort_methods(void) {
	reset_sort_method();

	pidgin_blist_sort_method_unreg("ext_blist_sort");
}


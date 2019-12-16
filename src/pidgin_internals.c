/* Copyright (C) 2008-2019 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This software may be modified and distributed under the terms
 * of the GPL2.0 license. See the COPYING file for details.
 */

#include "extended_blist_sort.h"

#include "pidgin_internals.h"

#include <gtkblist.h>
#include <debug.h>


void sort_method_none(PurpleBlistNode *node, PurpleBuddyList *blist, GtkTreeIter parent_iter, GtkTreeIter *cur, GtkTreeIter *iter) {
	PurpleBlistNode *sibling = node->prev;
	GtkTreeIter sibling_iter;
	PidginBuddyList *gtkblist;

	gtkblist = PIDGIN_BLIST(purple_get_blist());

	if (cur != NULL) {
		*iter = *cur;
		return;
	}

	while (sibling && !get_iter_from_node(sibling, &sibling_iter)) {
		sibling = sibling->prev;
	}

	gtk_tree_store_insert_after(gtkblist->treemodel, iter,
		node->parent ? &parent_iter : NULL,
		sibling ? &sibling_iter : NULL
	);
}

gboolean get_iter_from_node(PurpleBlistNode *node, GtkTreeIter *iter) {
	struct _pidgin_blist_node *gtknode = (struct _pidgin_blist_node *)node->ui_data;
	GtkTreePath *path;
	PidginBuddyList *gtkblist;

	gtkblist = PIDGIN_BLIST(purple_get_blist());

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



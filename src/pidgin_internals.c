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
                        sibling ? &sibling_iter : NULL);
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



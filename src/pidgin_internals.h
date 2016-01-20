/*
 * Extended BList Sort
 * Copyright (C) 2008-2016 Konrad Gräfe
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

#ifndef _PIDGIN_INTERNALS_H
#define _PIDGIN_INTERNALS_H

#include <gtk/gtk.h>
#include <conversation.h>

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

void sort_method_none(PurpleBlistNode *node, PurpleBuddyList *blist, GtkTreeIter parent_iter, GtkTreeIter *cur, GtkTreeIter *iter);

gboolean get_iter_from_node(PurpleBlistNode *node, GtkTreeIter *iter);

void sort_method_none(PurpleBlistNode *node, PurpleBuddyList *blist, GtkTreeIter parent_iter, GtkTreeIter *cur, GtkTreeIter *iter);

gboolean get_iter_from_node(PurpleBlistNode *node, GtkTreeIter *iter);

#endif /* _PIDGIN_INTERNALS_H */


/* Copyright (C) 2008-2019 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This software may be modified and distributed under the terms
 * of the GPL2.0 license. See the COPYING file for details.
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


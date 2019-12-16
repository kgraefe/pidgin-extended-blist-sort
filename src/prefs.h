/* Copyright (C) 2008-2019 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This software may be modified and distributed under the terms
 * of the GPL2.0 license. See the COPYING file for details.
 */

#ifndef _PREFS_H
#define _PREFS_H

#include "extended_blist_sort.h"

#include <plugin.h>
#include <gtk/gtk.h>

void init_prefs(void);
GtkWidget *get_pref_frame(PurplePlugin *plugin);

#endif /* _PREFS_H */


/* Copyright (C) 2008-2019 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This software may be modified and distributed under the terms
 * of the GPL2.0 license. See the COPYING file for details.
 */

#include "gtkutils.h"

GtkWidget *make_info_widget(gchar *markup, gchar *stock_id, gboolean indent) {
	GtkWidget *infobox, *label, *img, *align;

	if(!markup) return NULL;

	infobox = gtk_hbox_new(FALSE, 5);

	if(indent) {
		label = gtk_label_new("");
		gtk_box_pack_start(GTK_BOX(infobox), label, FALSE, FALSE, 10);
	}

	if(stock_id) {
		align = gtk_alignment_new(0.5, 0, 0, 0); /* align img to the top of the space */
		gtk_box_pack_start(GTK_BOX(infobox), align, FALSE, FALSE, 0);

		img = gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_MENU);
		gtk_container_add(GTK_CONTAINER(align), img);
	}

	label = gtk_label_new(NULL);
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_label_set_markup(GTK_LABEL(label), markup);
	gtk_box_pack_start(GTK_BOX(infobox), label, FALSE, FALSE, 0);

	return infobox;
}


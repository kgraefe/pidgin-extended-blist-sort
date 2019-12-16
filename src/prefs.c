/* Copyright (C) 2008-2019 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This software may be modified and distributed under the terms
 * of the GPL2.0 license. See the COPYING file for details.
 */

#include "prefs.h"

#include "extended_blist_sort.h"
#include "sort_methods.h"
#include "gtkutils.h"

#include <util.h>
#include <pidgin.h>
#include <gtkblist.h>
#include <pidgin/gtkutils.h>
#include <notify.h>

extern PurplePlugin *plugin;

enum pref_columns {
	PREF_LIST_COL_LABEL,
	PREF_LIST_COL_VALUE
};

typedef struct _info_buttons {
	GtkWidget *infobutton1;
	GtkWidget *infobutton2;
	GtkWidget *infobutton3;
} InfoButtons;

static InfoButtons infobuttons;

static void infobutton_clicked_cb(GtkWidget *widget, gpointer data) {
	GtkWidget *dialog;
	
	dialog = gtk_message_dialog_new_with_markup(NULL,
		GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK, NULL
	);

	gtk_window_set_title(GTK_WINDOW(dialog), _("Extended BList Sort"));

	gtk_message_dialog_set_markup(
		GTK_MESSAGE_DIALOG(dialog),
		pidgin_make_pretty_arrows(
			/* Translators: Please maintain the use of -> and <- to refer to menu hierarchy */
			_("You can reorder the accounts using drag and drop in the <b>Accounts</b> window at <b>Accounts->Manage Accounts</b>.")
		)
	);

	gtk_message_dialog_format_secondary_markup(
		GTK_MESSAGE_DIALOG(dialog), "%s",
		pidgin_make_pretty_arrows(
			/* Translators: Please maintain the use of -> and <- to refer to menu hierarchy */
			_("Please note that the plugin wont recognize this change so that you have to use <b>Tools->Extended BList Sort->Refresh</b> to refresh the buddy list.")
		)
	);
	
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

	gtk_widget_show_all(dialog);
	
}

static void update_infobuttons(void) {
	if(purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort1") == SORT_METHOD_ACCOUNT) {
		gtk_widget_show(infobuttons.infobutton1);
	} else {
		gtk_widget_hide(infobuttons.infobutton1);
	}

	if(purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort2") == SORT_METHOD_ACCOUNT) {
		gtk_widget_show(infobuttons.infobutton2);
	} else {
		gtk_widget_hide(infobuttons.infobutton2);
	}

	if(purple_prefs_get_int(PLUGIN_PREFS_PREFIX "/sort3") == SORT_METHOD_ACCOUNT) {
		gtk_widget_show(infobuttons.infobutton3);
	} else {
		gtk_widget_hide(infobuttons.infobutton3);
	}
}

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

	update_infobuttons();
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

static void option_changed_cb() {
	pidgin_blist_refresh(purple_get_blist());
}

void init_prefs(void) {
	if(purple_utf8_strcasecmp(purple_prefs_get_string(PIDGIN_PREFS_ROOT "/blist/sort_type"), "ext_blist_sort")!=0) {
		purple_prefs_set_string(PLUGIN_PREFS_PREFIX "/old_sort_method", purple_prefs_get_string(PIDGIN_PREFS_ROOT "/blist/sort_type"));
	}
	
	purple_prefs_set_string(PIDGIN_PREFS_ROOT "/blist/sort_type", "ext_blist_sort");
	pidgin_blist_update_sort_methods();
	
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort1", option_changed_cb, NULL);
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort1_reverse", option_changed_cb, NULL);
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort2", option_changed_cb, NULL);
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort2_reverse", option_changed_cb, NULL);
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort3", option_changed_cb, NULL);
	purple_prefs_connect_callback(plugin, PLUGIN_PREFS_PREFIX "/sort3_reverse", option_changed_cb, NULL);
}

static void make_pref_row(
	GtkWidget *table, gint row, GtkListStore *model, GtkWidget **infbtn,
	gchar *title, gchar *sort_option, gchar *reverse_option
) {
	GtkWidget *label, *combo, *toggle, *infobutton = NULL;
	GtkCellRenderer *renderer = NULL;
	GtkTreeIter iter;

	label = gtk_label_new(title);
	gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, row, row + 1, GTK_FILL, 0, 0, 0);

	combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(combo), renderer, "text", PREF_LIST_COL_LABEL, NULL);

	if(get_iter_by_sort_method_id(GTK_TREE_MODEL(model), purple_prefs_get_int(sort_option), &iter)) {
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combo), &iter);
	}
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(combo_changed_cb), sort_option);

	gtk_misc_set_alignment(GTK_MISC(combo), 0, 0.5);
	gtk_table_attach(GTK_TABLE(table), combo, 1, 2, row, row + 1, GTK_FILL, 0, 0, 0);

	toggle = gtk_check_button_new_with_mnemonic(_("Reverse"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), purple_prefs_get_bool(reverse_option));
	g_signal_connect(G_OBJECT(toggle), "toggled", G_CALLBACK(toggle_cb), reverse_option);
	gtk_misc_set_alignment(GTK_MISC(toggle), 0, 0.5);
	gtk_table_attach(GTK_TABLE(table), toggle, 1, 2, row + 1, row + 2, GTK_FILL, 0, 0, 0);

	infobutton = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(infobutton), gtk_image_new_from_stock(GTK_STOCK_INFO, GTK_ICON_SIZE_MENU));
	gtk_table_attach(GTK_TABLE(table), infobutton, 2, 3, row, row + 1, 0, 0, 0, 0);

	g_signal_connect(G_OBJECT(infobutton), "clicked", G_CALLBACK(infobutton_clicked_cb), NULL);
	g_signal_connect(G_OBJECT(infobutton), "show", G_CALLBACK(update_infobuttons), NULL);

	*infbtn = infobutton;
}

GtkWidget *get_pref_frame(PurplePlugin *plugin) {
	GtkWidget *ret = NULL;
	GtkWidget *label = NULL;
	GtkWidget *table = NULL;
	
	GtkListStore *model = NULL;
	GtkTreeIter iter;
	
	gchar *markup;
	
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
				PREF_LIST_COL_LABEL, _("Last Name"),
				PREF_LIST_COL_VALUE, SORT_METHOD_LAST_NAME,
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
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	markup = g_markup_printf_escaped("<span weight=\"bold\">%s</span>", _("Choose your way to sort the Buddy List:"));
	gtk_label_set_markup(GTK_LABEL(label), markup);
	gtk_box_pack_start(GTK_BOX(ret), label, FALSE, FALSE, 0);
	
	table = gtk_table_new(3, 2, FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table), 5);
	gtk_table_set_col_spacings(GTK_TABLE(table), 5);
	gtk_box_pack_start(GTK_BOX(ret), table, FALSE, FALSE, 0);
	
	make_pref_row(
		table, 0, model, &infobuttons.infobutton1,
		_("First:"), PLUGIN_PREFS_PREFIX "/sort1", PLUGIN_PREFS_PREFIX "/sort1_reverse"
	);
	make_pref_row(
		table, 2, model, &infobuttons.infobutton2,
		_("Second:"), PLUGIN_PREFS_PREFIX "/sort2", PLUGIN_PREFS_PREFIX "/sort2_reverse"
	);
	make_pref_row(
		table, 4, model, &infobuttons.infobutton3,
		_("Last:"), PLUGIN_PREFS_PREFIX "/sort3", PLUGIN_PREFS_PREFIX "/sort3_reverse"
	);

	return ret;
}


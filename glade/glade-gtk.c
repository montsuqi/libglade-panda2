/* -*- Mode: C; c-basic-offset: 8 -*-
 * libglade - a library for building interfaces from XML files at runtime
 * Copyright (C) 1998-2001  James Henstridge <james@daa.com.au>
 *
 * glade-gtk.c: support for GTK+ widgets in libglade
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glade/glade-build.h>
#include <glade/glade-private.h>
#include <gmodule.h>
#include <gtk/gtk.h>

#ifdef	USE_PANDA
#include <gtkpanda/gtkpanda.h>
#endif

#ifndef ENABLE_NLS
/* a slight optimisation when gettext is off */
#define glade_xml_gettext(xml, msgid) (msgid)
#endif
#undef _
#define _(msgid) (glade_xml_gettext(xml, msgid))

/* functions to actually build the widgets */

static void
button_build_children (GladeXML *xml, GtkWidget *w, GladeWidgetInfo *info,
		       const char *longname)
{
	GtkWidget *child = glade_xml_build_widget(xml,
			(GladeWidgetInfo *)info->children->data, longname);
	guint key = glade_xml_get_parent_accel(xml);

	gtk_container_add(GTK_CONTAINER(w), child);
	if (key)
		gtk_widget_add_accelerator(w, "clicked",
					   glade_xml_ensure_accel(xml),
					   key, GDK_MOD1_MASK, 0);
}

static void
box_build_children (GladeXML *xml, GtkWidget *w, GladeWidgetInfo *info,
		    const char *longname)
{
	GList *tmp;

	for (tmp = info->children; tmp; tmp = tmp->next) {
		GladeWidgetInfo *cinfo = tmp->data;
		GtkWidget *child = glade_xml_build_widget (xml,cinfo,longname);
		GList *tmp2;
		gboolean expand = TRUE, fill = TRUE, start = TRUE;
		gint padding = 0;

		for (tmp2 = cinfo->child_attributes; tmp2; tmp2 = tmp2->next) {
			GladeAttribute *attr = tmp2->data;

			switch (attr->name[0]) {
			case 'e':
				if (!strcmp (attr->name, "expand"))
					expand = attr->value[0] == 'T';
				break;
			case 'f':
				if (!strcmp (attr->name, "fill"))
					fill = attr->value[0] == 'T';
				break;
			case 'p':
				if (!strcmp (attr->name, "padding"))
					padding = strtol(attr->value, NULL, 0);
				else if (!strcmp (attr->name, "pack"))
					start = strcmp (attr->value,
							"GTK_PACK_START") == 0;
				break;
			}
		}
		if (start)
			gtk_box_pack_start (GTK_BOX(w), child, expand,
					    fill, padding);
		else
			gtk_box_pack_end (GTK_BOX(w), child, expand,
					  fill, padding);
	}
}

static void
table_build_children (GladeXML *xml, GtkWidget *w, GladeWidgetInfo *info,
		      const char *longname) {
	GList *tmp;

	for (tmp = info->children; tmp; tmp = tmp->next) {
		GladeWidgetInfo *cinfo = tmp->data;
		GtkWidget *child = glade_xml_build_widget (xml,cinfo,longname);
		GList *tmp2;
		gint left_attach=0,right_attach=1,top_attach=0,bottom_attach=1;
		gint xpad=0, ypad=0, xoptions=0, yoptions=0;
    
		for (tmp2 = cinfo->child_attributes; tmp2; tmp2 = tmp2->next) {
			GladeAttribute *attr = tmp2->data;
			switch (attr->name[0]) {
			case 'b':
				if (!strcmp(attr->name, "bottom_attach"))
					bottom_attach = strtol(attr->value, NULL, 0);
				break;
			case 'l':
				if (!strcmp(attr->name, "left_attach"))
					left_attach = strtol(attr->value, NULL, 0);
				break;
			case 'r':
				if (!strcmp(attr->name, "right_attach"))
					right_attach = strtol(attr->value, NULL, 0);
				break;
			case 't':
				if (!strcmp(attr->name, "top_attach"))
					top_attach = strtol(attr->value, NULL, 0);
				break;
			case 'x':
				switch (attr->name[1]) {
				case 'e':
					if (!strcmp(attr->name, "xexpand") &&
					    attr->value[0] == 'T')
						xoptions |= GTK_EXPAND;
					break;
				case 'f':
					if (!strcmp(attr->name, "xfill") &&
					    attr->value[0] == 'T')
						xoptions |= GTK_FILL;
					break;
				case 'p':
					if (!strcmp(attr->name, "xpad"))
						xpad = strtol(attr->value, NULL, 0);
					break;
				case 's':
					if (!strcmp(attr->name, "xshrink") &&
					    attr->value[0] == 'T')
						xoptions |= GTK_SHRINK;
					break;
				}
				break;
			case 'y':
				switch (attr->name[1]) {
				case 'e':
					if (!strcmp(attr->name, "yexpand") &&
					    attr->value[0] == 'T')
						yoptions |= GTK_EXPAND;
					break;
				case 'f':
					if (!strcmp(attr->name, "yfill") &&
					    attr->value[0] == 'T')
						yoptions |= GTK_FILL;
					break;
				case 'p':
					if (!strcmp(attr->name, "ypad"))
						ypad = strtol(attr->value, NULL, 0);
					break;
				case 's':
					if (!strcmp(attr->name, "yshrink") &&
					    attr->value[0] == 'T')
						yoptions |= GTK_SHRINK;
					break;
				}
				break;
			}
		}
		gtk_table_attach(GTK_TABLE(w), child, left_attach,right_attach,
				 top_attach,bottom_attach, xoptions,yoptions,
				 xpad,ypad);
	}  
}

static void
fixed_build_children (GladeXML *xml, GtkWidget *w, GladeWidgetInfo *info,
		      const char *longname)
{
	GList *tmp;

	for (tmp = info->children; tmp; tmp = tmp->next) {
		GladeWidgetInfo *cinfo = tmp->data;
		GtkWidget *child = glade_xml_build_widget(xml, cinfo,longname);
		GList *tmp2;
		gint xpos = 0, ypos = 0;

		for (tmp2 = cinfo->attributes; tmp2; tmp2 = tmp2->next) {
			GladeAttribute *attr = tmp2->data;
			
			if (attr->name[0] == 'x' && attr->name[1] == '\0')
				xpos = strtol(attr->value, NULL, 0);
			else if (attr->name[0] == 'y' && attr->name[1] == '\0')
				ypos = strtol(attr->value, NULL, 0);
		}
		gtk_fixed_put(GTK_FIXED(w), child, xpos, ypos);
	}
}

#ifdef	USE_PANDA
static void
panda_clist_build_children (GladeXML *xml, GtkWidget *w, GladeWidgetInfo *info,
			    const char *longname)
{
	GList *tmp;
	GtkTreeViewColumn *column;
	GtkRequisition req;
	gint col = 0;
	gint csize;
	gint rsize;

#define COL_INSETS 11

	for (tmp = info->children; tmp; tmp = tmp->next) {
		GtkWidget *child = glade_xml_build_widget (xml, tmp->data,
			longname);
		gtk_widget_show(child);
		column = gtk_tree_view_get_column(GTK_TREE_VIEW(w), col);
		gtk_widget_size_request(child, &req);
        csize = gtk_tree_view_column_get_fixed_width(column);
		rsize = req.width + COL_INSETS;
		gtk_panda_clist_set_column_width(GTK_PANDA_CLIST(w), 
			col, rsize > csize ? rsize : csize);
		gtk_tree_view_column_set_widget(column, child);
		if (GTK_IS_MISC(child)) {
			gtk_tree_view_column_set_alignment(column, GTK_MISC(child)->xalign);
		}
		gtk_tree_view_column_set_clickable(column, FALSE);
		if (!GTK_PANDA_CLIST(w)->show_titles) {
			gtk_widget_hide(child);
		}
		col++;
	}
}
#endif	/* USE_PANDA */

static void
note_change_page(GtkWidget *child, GtkNotebook *notebook)
{
	gint page = gtk_notebook_page_num(notebook, child);

	gtk_notebook_set_page(notebook, page);
}

static void
note_page_mapped (GtkWidget *page, GtkAccelGroup *accel_group)
{
	GtkWidget *dialog = gtk_widget_get_toplevel (GTK_WIDGET (page));

	gtk_window_add_accel_group (GTK_WINDOW (dialog), accel_group);
}

static void
note_page_unmapped (GtkWidget *page, GtkAccelGroup *accel_group)
{
	GtkWidget *dialog = gtk_widget_get_toplevel (GTK_WIDGET (page));

	gtk_window_remove_accel_group (GTK_WINDOW (dialog), accel_group);
}

static void note_page_setup_signals (GtkWidget *page, GtkAccelGroup *accel)
{
	gtk_accel_group_ref(accel);
	g_signal_connect (G_OBJECT (page),
			    "map", G_CALLBACK (note_page_mapped), accel);
	gtk_accel_group_ref(accel);
	g_signal_connect (G_OBJECT (page),
			    "unmap", G_CALLBACK (note_page_unmapped), accel);
}

static void
notebook_build_children (GladeXML *xml, GtkWidget *w, GladeWidgetInfo *info,
			 const char *longname)
{

	/*
	 * the notebook tabs are listed after the pages, and have
	 * child_name set to Notebook:tab.  We store pages in a GList
	 * while waiting for tabs
	 */
	
	GList *pages = NULL;
	GList *tmp;

	for (tmp = info->children; tmp; tmp = tmp->next) {
		GladeWidgetInfo *cinfo = tmp->data;
		GtkWidget *child;
		GList *tmp2;
		GladeAttribute *attr = NULL;
		GtkAccelGroup *accel;

		accel = glade_xml_push_accel(xml);
		child = glade_xml_build_widget (xml,cinfo,longname);
		note_page_setup_signals(child, accel);
		glade_xml_pop_accel(xml);
		for (tmp2 = cinfo->attributes; tmp2; tmp2 = tmp2->next) {
			attr = tmp2->data;
			if (!strcmp(attr->name, "child_name"))
				break;
		}
		if (tmp2 == NULL || strcmp(attr->value, "Notebook:tab") != 0)
			pages = g_list_append (pages, child);
		else {
			GtkWidget *page;
			gint key = glade_xml_get_parent_accel(xml);

			if (pages) {
				page = pages->data;
				pages = g_list_remove (pages, page);
			} else {
				page = gtk_label_new("Unknown page");
				gtk_widget_show(page);
			}
			gtk_notebook_append_page (GTK_NOTEBOOK(w), page,child);
			if (key) {
				gtk_widget_add_accelerator(page, "grab_focus",
					     glade_xml_ensure_accel(xml),
					     key, GDK_MOD1_MASK, 0);
				g_signal_connect(G_OBJECT(page),
					"grab_focus",
					GTK_SIGNAL_FUNC(note_change_page), w);
			}
		}
	}
}

#ifdef	USE_PANDA
static void
panda_combo_build_children (GladeXML *xml, GtkWidget *w,
			    GladeWidgetInfo *info, const char *longname)
{
	GList *tmp;
	GladeWidgetInfo *cinfo = NULL;
	GtkEntry *entry;

	for (tmp = info->children; tmp; tmp = tmp->next) {
		GList *tmp2;
		gchar *child_name = NULL;
		cinfo = tmp->data;
		for (tmp2 = cinfo->attributes; tmp2; tmp2 = tmp2->next) {
			GladeAttribute *attr = tmp2->data;
			if (!strcmp(attr->name, "child_name")) {
				child_name = attr->value;
				break;
			}
		}
		if (child_name && !strcmp(child_name, "GtkPandaCombo:entry"))
			break;
	}
	if (!tmp)
		return;
#if 1
	entry = GTK_ENTRY(GTK_PANDA_COMBO(w)->entry);
	for (tmp = cinfo->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;
		if (!strcmp(attr->name, "editable"))
			gtk_entry_set_editable(entry, attr->value[0] == 'T');
		else if (!strcmp(attr->name, "text_visible"))
			gtk_entry_set_visibility(entry, attr->value[0] == 'T');
		else if (!strcmp(attr->name, "text_max_length"))
			gtk_entry_set_max_length(entry, strtol(attr->value,
							       NULL, 0));
		else if (!strcmp(attr->name, "text"))
			gtk_entry_set_text(entry, attr->value);
	}
#endif
	glade_xml_set_common_params(xml, GTK_PANDA_COMBO(w)->entry, cinfo, longname);
}
#endif

static void
misc_set (GtkMisc *misc, GladeWidgetInfo *info)
{
	GList *tmp;

	for (tmp = info->attributes; tmp; tmp = tmp->next){
		GladeAttribute *attr = tmp->data;

		switch (attr->name[0]) {
		case 'x':
			if (!strcmp (attr->name, "xalign")){
				gfloat align = g_strtod (attr->value, NULL);
				gtk_misc_set_alignment(misc, align,
						       misc->yalign);
			} else if (!strcmp(attr->name, "xpad")){
				gint pad = strtol(attr->value, NULL, 0);
				gtk_misc_set_padding(misc, pad, misc->ypad);
			} break;
			
		case 'y':
			if (!strcmp(attr->name, "yalign")){
				gfloat align = g_strtod (attr->value, NULL);
				gtk_misc_set_alignment(misc, misc->xalign,
						       align);
			} else if (!strcmp(attr->name, "ypad")){
				gint pad = strtol(attr->value, NULL, 0);
				gtk_misc_set_padding(misc, misc->xpad, pad);
			}
			break;
		}
	}
}

static GtkWidget *
label_new (GladeXML *xml, GladeWidgetInfo *info)
{
	GList *tmp;
	GtkWidget *label;
	guint key;
	gchar *string = NULL, *focus_target = NULL;
	GtkJustification just = GTK_JUSTIFY_CENTER;
	gboolean wrap = FALSE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "label")) {
			string = attr->value;
		} else if (!strcmp(attr->name, "justify")) {
			just = glade_enum_from_string(GTK_TYPE_JUSTIFICATION,
						      attr->value);
		} else if (!strcmp(attr->name, "default_focus_target")) {
			if (!focus_target) focus_target = attr->value;
		} else if (!strcmp(attr->name, "focus_target"))
			focus_target = attr->value;
		else if (!strcmp(attr->name, "wrap"))
			wrap = attr->value[0] == 'T';
	}

	label = gtk_label_new("");
	if (string){
		char *s = string [0] ? _(string) : "";
		
		key = gtk_label_parse_uline(GTK_LABEL(label), s);
	} else
		key = 0;
	
	if (key)
		glade_xml_handle_label_accel(xml, focus_target, key);
// for Gtk+1.2 compatibility
// in Gtk+2.x Justify > Align
#if 0
	if (just != GTK_JUSTIFY_CENTER)
		gtk_label_set_justify(GTK_LABEL(label), just);
#endif
	if (wrap)
		gtk_label_set_line_wrap(GTK_LABEL(label), wrap);
	if (GTK_IS_MISC(label))
		misc_set (GTK_MISC(label), info);
	return label;
}

static GtkWidget *
accellabel_new (GladeXML *xml, GladeWidgetInfo *info)
{
	GList *tmp;
	GtkWidget *label;
	gchar *string = NULL;
	GtkJustification just = GTK_JUSTIFY_CENTER;
	gboolean wrap = FALSE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "label")) {
			string = attr->value;
		} else if (!strcmp(attr->name, "justify"))
			just = glade_enum_from_string(GTK_TYPE_JUSTIFICATION,
						      attr->value);
		else if (!strcmp(attr->name, "wrap"))
			wrap = attr->value[0] == 'T';
	}

	label = gtk_accel_label_new(_(string));
	if (just != GTK_JUSTIFY_CENTER)
		gtk_label_set_justify(GTK_LABEL(label), just);
	if (wrap)
		gtk_label_set_line_wrap(GTK_LABEL(label), wrap);
	if (GTK_IS_MISC(label))
		misc_set(GTK_MISC(label), info);

	return label;
}

static GtkWidget *
entry_new (GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *entry;
	GList *tmp;
	char *text = NULL;
	gint text_max_length = -1;
	gboolean editable = TRUE, text_visible = TRUE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		switch (attr->name[0]) {
		case 'e':
			if (!strcmp(attr->name, "editable"))
				editable = attr->value[0] == 'T';
			break;
		case 't':
			if (!strcmp(attr->name, "text"))
				text = attr->value;
			else if (!strcmp(attr->name, "text_visible"))
				text_visible = attr->value[0] == 'T';
			else if (!strcmp(attr->name, "text_max_length"))
				text_max_length = strtol(attr->value, NULL, 0);
			break;
		}
	}
	if (text_max_length >= 0)
		entry = gtk_entry_new_with_max_length(text_max_length);
	else
		entry = gtk_entry_new();

	if (text)
		gtk_entry_set_text(GTK_ENTRY(entry), _(text));

	gtk_entry_set_editable(GTK_ENTRY(entry), editable);
	gtk_entry_set_visibility(GTK_ENTRY(entry), text_visible);
	return entry;
}

#ifdef	USE_PANDA
static GtkWidget *
panda_entry_new (GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *entry = gtk_panda_entry_new();
	GList *tmp;
	char *text = NULL;
	gint text_max_length = -1;
	gboolean editable = TRUE, 
		text_visible = TRUE, 
		im_enabled = FALSE;
	enum gtk_panda_entry_input_mode input_mode = GTK_PANDA_ENTRY_IM_MODE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		switch (attr->name[0]) {
		case 'e':
			if (!strcmp(attr->name, "editable"))
				editable = attr->value[0] == 'T';
			break;
		case 'i':
			if (!strcmp(attr->name, "input_mode"))
				switch (attr->value[0]) {
				case 'A': /* ASCII */
					input_mode = GTK_PANDA_ENTRY_ASCII_MODE;
					break;
				case 'K': /* KANA */
					input_mode = GTK_PANDA_ENTRY_KANA_MODE;
					break;
				default: /* XIM */
					input_mode = GTK_PANDA_ENTRY_IM_MODE;
					break;
				}
			break;
		case 't':
			if (!strcmp(attr->name, "text"))
				text = attr->value;
			else if (!strcmp(attr->name, "text_visible"))
				text_visible = attr->value[0] == 'T';
			else if (!strcmp(attr->name, "text_max_length"))
				text_max_length = strtol(attr->value, NULL, 0);
			break;
		case 'u':
			/* for backward compatibility - removed soon */
			if (!strcmp(attr->name, "use_xim"))
				input_mode = attr->value[0] == 'T'
					? GTK_PANDA_ENTRY_IM_MODE
					: GTK_PANDA_ENTRY_ASCII_MODE;
			break;
		case 'x':
			if (!strcmp(attr->name, "xim_enabled"))
				im_enabled = attr->value[0] == 'T' ? 1 : 0;
		}
	}

	if (text_max_length >= 0)
		gtk_entry_set_max_length (GTK_ENTRY(entry), text_max_length);

	if (text)
		gtk_entry_set_text(GTK_ENTRY(entry), _(text));

	gtk_entry_set_editable(GTK_ENTRY(entry), editable);
	gtk_entry_set_visibility(GTK_ENTRY(entry), text_visible);
	gtk_panda_entry_set_input_mode(GTK_PANDA_ENTRY(entry), input_mode);
	gtk_panda_entry_set_im_enabled(GTK_PANDA_ENTRY(entry), im_enabled);
	return entry;
}

static GtkWidget *
number_entry_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *entry;
	GList *tmp;
	char *format = NULL;
	gboolean editable = TRUE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		switch (attr->name[0]) {
		  case 'e':
			if (!strcmp(attr->name, "editable"))
					editable = attr->value[0] == 'T';
			break;
		  case 'f':
			if (!strcmp(attr->name, "format"))
					format = attr->value;
			break;
		}
	}

	if (format) {
			entry = gtk_number_entry_new_with_format(format);
	} else {
			entry = gtk_number_entry_new();
	}

	gtk_entry_set_editable(GTK_ENTRY(entry), editable);
	return entry;
}

static GtkWidget *
panda_html_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *widget;
	GList *tmp;
	char *uri = NULL;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;
		if (strcmp (attr->name, "uri") == 0) {
			uri = attr->value;
		}
	}

	widget = gtk_panda_html_new();
	gtk_panda_html_set_uri (GTK_PANDA_HTML (widget), uri);
	return widget;
}

static GtkWidget *
panda_preview_new(GladeXML *xml, GladeWidgetInfo *info)
{
	return gtk_panda_pdf_new();
}

#endif

#ifdef USE_PANDA
static GtkWidget *
panda_text_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *wid;
	GList *tmp;
	char *text = NULL;
	gboolean editable = TRUE;
	gboolean im = FALSE;
    GtkTextBuffer *buffer;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;
		switch (attr->name[0]) {
		case 'e':
			if (!strcmp(attr->name, "editable"))
				editable = attr->value[0] == 'T';
			break;
		case 't':
			if (!strcmp(attr->name, "text"))
				text = attr->value;
			break;
		case 'x':
			if (!strcmp(attr->name, "xim_enabled"))
				im = attr->value[0] == 'T';
			break;
		}
	}
	//wid = gtk_panda_text_new(NULL, NULL);
	wid = gtk_panda_text_new();
	if (text) {
		char *tmp = _(text);
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(wid));
        gtk_text_buffer_set_text(buffer, tmp, strlen(tmp));
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(wid), buffer);
	}
	gtk_panda_text_set_im_enabled(GTK_PANDA_TEXT(wid), im);
	return wid;
}

static GtkWidget *
panda_timer_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *wid;
	GList *tmp;
	char *duration = NULL;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;
		switch (attr->name[0]) {
		case 'd':
			if (!strcmp(attr->name, "duration"))
				duration = attr->value;
			break;
		}
	}

	wid = gtk_panda_timer_new();
	if (duration) {
		gtk_panda_timer_set(GTK_PANDA_TIMER(wid), atoi(duration));
	}
	gtk_panda_timer_reset(GTK_PANDA_TIMER(wid));
	return wid;
}

static GtkWidget *
panda_download_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *wid;

	wid = gtk_panda_download_new();
	return wid;
}
#endif

static GtkWidget *
button_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *button;
	GList *tmp;
	char *string = NULL;
  
	/*
	 * This should really be a container, but GLADE is weird in this
	 * respect.  If the label property is set for this widget, insert
	 * a label.  Otherwise, allow a widget to be packed
	 */
	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;
		if (!strcmp(attr->name, "label"))
			string = attr->value;
	}
	if (string != NULL) {
		guint key;
		
		button = gtk_button_new_with_label("");
		key = gtk_label_parse_uline(GTK_LABEL(GTK_BIN(button)->child),
					    string[0] ? _(string) : "");
		if (key)
			gtk_widget_add_accelerator(button, "clicked",
						   glade_xml_ensure_accel(xml),
						   key, GDK_MOD1_MASK, 0);
	} else
		button = gtk_button_new();
	return button;
}

static GtkWidget *
togglebutton_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *button;
	GList *tmp;
	char *string = NULL;
	gboolean active = FALSE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "label"))
			string = attr->value;
		else if (!strcmp(attr->name, "active"))
			active = attr->value[0] == 'T';
	}
	if (string != NULL) {
		guint key;
		
		button = gtk_toggle_button_new_with_label("");
		key = gtk_label_parse_uline(GTK_LABEL(GTK_BIN(button)->child),
					    string[0] ? _(string) : "");
		if (key)
			gtk_widget_add_accelerator(button, "clicked",
						   glade_xml_ensure_accel(xml),
						   key, GDK_MOD1_MASK, 0);
	} else
		button = gtk_toggle_button_new();
	gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(button), active);
	return button;
}

static GtkWidget *
checkbutton_new (GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *button;
	GList *tmp;
	char *string = NULL;
	gboolean active = FALSE, draw_indicator = TRUE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "label"))
			string = attr->value;
		else if (!strcmp(attr->name, "active"))
			active = attr->value[0] == 'T';
		else if (!strcmp(attr->name, "draw_indicator"))
			draw_indicator = attr->value[0] == 'T';
	}
	if (string != NULL) {
		guint key;
		
		button = gtk_check_button_new_with_label("");
		key = gtk_label_parse_uline(GTK_LABEL(GTK_BIN(button)->child),
					    string[0] ? _(string) : "");
		if (key)
			gtk_widget_add_accelerator(button, "clicked",
						   glade_xml_ensure_accel(xml),
						   key, GDK_MOD1_MASK, 0);
	} else
		button = gtk_check_button_new();

	gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(button), active);
	gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(button), draw_indicator);
	return button;
}

static GtkWidget *
radiobutton_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *button;
	GList *tmp;
	char *string = NULL;
	gboolean active = FALSE, draw_indicator = TRUE;
	GSList *group = NULL;
	char *group_name = NULL;
	
	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "label"))
			string = attr->value;
		else if (!strcmp(attr->name, "active"))
			active = attr->value[0] == 'T';
		else if (!strcmp(attr->name, "draw_indicator"))
			draw_indicator = attr->value[0] == 'T';
		else if (!strcmp(attr->name, "group")){
			group_name = attr->value;
			group = g_hash_table_lookup (xml->priv->radio_groups,
						     group_name);
			if (!group)
				group_name = g_strdup(group_name);
		}
	}
	if (string != NULL) {
		guint key;
		
		button = gtk_radio_button_new_with_label(group, "");
		key = gtk_label_parse_uline(GTK_LABEL(GTK_BIN(button)->child),
					    string[0] ? _(string) : "");
		if (key)
			gtk_widget_add_accelerator(button, "clicked",
						   glade_xml_ensure_accel(xml),
						   key, GDK_MOD1_MASK, 0);
	} else
		button = gtk_radio_button_new (group);

	if (group_name) {
		GtkRadioButton *radio = GTK_RADIO_BUTTON (button);
		
		g_hash_table_insert (xml->priv->radio_groups,
				     group_name,
				     gtk_radio_button_group (radio));
	} 

	gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(button), active);
	gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(button), draw_indicator);
	return button;
}

#ifdef	USE_PANDA
static GtkWidget *
panda_combo_new (GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *combo = gtk_panda_combo_new();
	GList *tmp;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		switch (attr->name[0]) {
		case 'c':
			if (!strcmp(attr->name, "case_sensitive"))
				gtk_panda_combo_set_case_sensitive(GTK_PANDA_COMBO(combo),
							attr->value[0] == 'T');
			break;
		case 'i':
			if (!strcmp(attr->name, "items")) {
				GList *item_list = NULL;
				gchar **items = g_strsplit(attr->value,"\n",0);
				int i = 0;
				for (i = 0; items[i] != NULL; i++)
					item_list = g_list_append(item_list,
								  _(items[i]));
				if (item_list)
					gtk_panda_combo_set_popdown_strings(
						GTK_PANDA_COMBO(combo), item_list);
				g_list_free(item_list);
				g_strfreev(items);
			}
			break;
		case 'u':
			if (!strcmp(attr->name, "use_arrows"))
				gtk_panda_combo_set_use_arrows(GTK_PANDA_COMBO(combo),
							attr->value[0] == 'T');
			else if (!strcmp(attr->name, "use_arrows_always"))
				gtk_panda_combo_set_use_arrows_always(
					GTK_PANDA_COMBO(combo), attr->value[0]=='T');
			break;
		}
	}

	return combo;
}
#endif

#ifdef	USE_PANDA
static GtkWidget *
panda_clist_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *clist;
	GList *tmp;
	int cols = 1;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;
		if (!strcmp(attr->name, "columns")) {
			cols = strtol(attr->value, NULL, 0);
			break;
		}
	}

	clist = gtk_panda_clist_new(cols);

	cols = 0;
	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		switch (attr->name[0]) {
		case 'c':
			if (!strcmp(attr->name, "column_widths")) {
				char *pos = attr->value;
				while (pos && *pos != '\0') {
					int width = strtol(pos, &pos, 0);
					if (*pos == ',') pos++;
	   				gtk_panda_clist_set_column_width(
						GTK_PANDA_CLIST(clist), cols++, width);
				}
			}
			break;
		case 's':
			if (!strcmp(attr->name, "selection_mode"))
				gtk_panda_clist_set_selection_mode(
					GTK_PANDA_CLIST(clist),
					glade_enum_from_string(
						GTK_TYPE_SELECTION_MODE,
						attr->value));
			else if (!strcmp(attr->name, "show_titles")) {
				if (attr->value[0] == 'T')
					gtk_panda_clist_titles_show(GTK_PANDA_CLIST(clist));
				else
					gtk_panda_clist_titles_hide(GTK_PANDA_CLIST(clist));
			}
#if 0
			else if (!strcmp(attr->name, "shadow_type"))
				gtk_panda_clist_set_shadow_type(GTK_PANDA_CLIST(clist),
					glade_enum_from_string(
						GTK_TYPE_SHADOW_TYPE,
						attr->value));
#endif
			break;
		}
	}
	return clist;
}
#endif	/* USE_PANDA */

static GtkWidget *
progressbar_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *ret = gtk_progress_bar_new();
	GList *tmp;
	gfloat value = 0, lower = 0, upper = 100;
	gfloat xalign = 0.5, yalign = 0.5;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "value"))
			value = g_strtod(attr->value, NULL);
		else if (!strcmp(attr->name, "lower"))
			lower = g_strtod(attr->value, NULL);
		else if (!strcmp(attr->name, "upper"))
			upper = g_strtod(attr->value, NULL);
		else if (!strcmp(attr->name, "activity_mode"))
			gtk_progress_set_activity_mode(GTK_PROGRESS(ret),
						       attr->value[0]=='T');
		else if (!strcmp(attr->name, "bar_style"))
			gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(ret),
			    glade_enum_from_string(GTK_TYPE_PROGRESS_BAR_STYLE,
						   attr->value));
		else if (!strcmp(attr->name, "orientation"))
			gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(ret),
			    glade_enum_from_string(
			      GTK_TYPE_PROGRESS_BAR_ORIENTATION, attr->value));
		else if (!strcmp(attr->name, "show_text"))
			gtk_progress_set_show_text(GTK_PROGRESS(ret),
						   attr->value[0] == 'T');
		else if (!strcmp(attr->name, "text_xalign"))
			xalign = g_strtod(attr->value, NULL);
		else if (!strcmp(attr->name, "text_yalign"))
			yalign = g_strtod(attr->value, NULL);
		else if (!strcmp(attr->name, "format"))
			gtk_progress_set_format_string(GTK_PROGRESS(ret),
						       attr->value);
	}
	gtk_progress_configure(GTK_PROGRESS(ret), value, lower, upper);
	gtk_progress_set_text_alignment(GTK_PROGRESS(ret), xalign, yalign);
	return ret;
}

static GtkWidget *
hseparator_new(GladeXML *xml, GladeWidgetInfo *info)
{
	return gtk_hseparator_new();
}

static GtkWidget *
vseparator_new(GladeXML *xml, GladeWidgetInfo *info)
{
	return gtk_vseparator_new();
}

static GtkWidget *
hbox_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GList *tmp;
	int spacing = 0;
	gboolean homog = FALSE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "homogeneous"))
			homog = attr->value[0] == 'T';
		else if (!strcmp(attr->name, "spacing"))
			spacing = strtol(attr->value, NULL, 0);
	}
	return gtk_hbox_new (homog, spacing);
}

static GtkWidget *vbox_new (GladeXML *xml, GladeWidgetInfo *info)
{
	GList *tmp;
	int spacing = 0;
	gboolean homog = FALSE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;
		
		if (!strcmp(attr->name, "homogeneous"))
			homog = attr->value[0] == 'T';
		else if (!strcmp(attr->name, "spacing"))
			spacing = strtol(attr->value, NULL, 0);
	}
	return gtk_vbox_new(homog, spacing);
}

static GtkWidget *
table_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *table;
	GList *tmp;
	int rows = 1, cols = 1, rspace = 0, cspace = 0;
	gboolean homog = FALSE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		switch (attr->name[0]) {
		case 'c':
			if (!strcmp(attr->name, "columns"))
				cols = strtol(attr->value, NULL, 0);
			else if (!strcmp(attr->name, "column_spacing"))
				cspace = strtol(attr->value, NULL, 0);
			break;
		case 'h':
			if (!strcmp(attr->name, "homogeneous"))
				homog = attr->value[0] == 'T';
			break;
		case 'r':
			if (!strcmp(attr->name, "rows"))
				rows = strtol(attr->value, NULL, 0);
			else if (!strcmp(attr->name, "row_spacing"))
				rspace = strtol(attr->value, NULL, 0);
			break;
		}
	}

	table = gtk_table_new(rows, cols, homog);
	gtk_table_set_row_spacings(GTK_TABLE(table), rspace);
	gtk_table_set_col_spacings(GTK_TABLE(table), cspace);
	return table;
}

static GtkWidget *
fixed_new(GladeXML *xml, GladeWidgetInfo *info)
{
	return gtk_fixed_new();
}

static GtkWidget *
frame_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *frame;
	GList *tmp;
	char *label = NULL;
	gdouble label_xalign = 0;
	GtkShadowType shadow_type = GTK_SHADOW_ETCHED_IN;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		switch (attr->name[0]) {
		case 'l':
			if (!strcmp(attr->name, "label"))
				label = attr->value;
			else if (!strcmp(attr->name, "label_xalign"))
				label_xalign = g_strtod(attr->value, NULL);
			break;
		case 's':
			if (!strcmp(attr->name, "shadow_type"))
				shadow_type = glade_enum_from_string(
					GTK_TYPE_SHADOW_TYPE, attr->value);
			break;
		}
	}
	if (label)
		frame = gtk_frame_new(_(label));
	else
		frame = gtk_frame_new(NULL);
	gtk_frame_set_label_align(GTK_FRAME(frame), label_xalign, 0.5);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), shadow_type);

	return frame;
}

static GtkWidget *
notebook_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *notebook = gtk_notebook_new();
	GList *tmp;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "popup_enable")) {
			if (attr->value[0] == 'T')
				gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
			else
				gtk_notebook_popup_disable(GTK_NOTEBOOK(notebook));
		} else if (!strcmp(attr->name, "scrollable"))
			gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook),
						    attr->value[0] == 'T');
		else if (!strcmp(attr->name, "show_border"))
			gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook),
						     attr->value[0] == 'T');
		else if (!strcmp(attr->name, "show_tabs"))
			gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook),
						   attr->value[0] == 'T');
		else if (!strcmp(attr->name, "tab_pos"))
			gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook),
				glade_enum_from_string(GTK_TYPE_POSITION_TYPE,
						       attr->value));
	}
	return notebook;
}

static GtkWidget *
scrolledwindow_new (GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *win = gtk_scrolled_window_new(NULL, NULL);
	GList *tmp;
	GtkPolicyType hpol = GTK_POLICY_ALWAYS, vpol = GTK_POLICY_ALWAYS;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "hscrollbar_policy"))
			hpol = glade_enum_from_string(GTK_TYPE_POLICY_TYPE,
						      attr->value);
		else if (!strcmp(attr->name, "hupdate_policy"))
			gtk_range_set_update_policy(
			       GTK_RANGE(GTK_SCROLLED_WINDOW(win)->hscrollbar),
			       glade_enum_from_string(GTK_TYPE_UPDATE_TYPE,
						      attr->value));
		else if (!strcmp(attr->name, "shadow_type"))
			gtk_viewport_set_shadow_type(GTK_VIEWPORT(win),
				glade_enum_from_string(GTK_TYPE_SHADOW_TYPE,
						       attr->value));
		else if (!strcmp(attr->name, "vscrollbar_policy"))
			vpol = glade_enum_from_string(GTK_TYPE_POLICY_TYPE,
						      attr->value);
		else if (!strcmp(attr->name, "vupdate_policy"))
			gtk_range_set_update_policy(
			       GTK_RANGE(GTK_SCROLLED_WINDOW(win)->vscrollbar),
			       glade_enum_from_string(GTK_TYPE_UPDATE_TYPE,
						      attr->value));
	}
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(win), hpol, vpol);
	return win;
}

static GtkWidget *
viewport_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *port = gtk_viewport_new(NULL, NULL);
	GList *tmp;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "shadow_type"))
			gtk_viewport_set_shadow_type(GTK_VIEWPORT(port),
				glade_enum_from_string(GTK_TYPE_SHADOW_TYPE,
						       attr->value));
	}
	return port;
}

static GtkWidget *
calendar_new (GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *wid = gtk_calendar_new();
	GList *tmp;
	GtkCalendarDisplayOptions dopt = 0;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		if (!strcmp(attr->name, "show_heading")) {
			if (attr->value[0] == 'T')
				dopt |= GTK_CALENDAR_SHOW_HEADING;
		} else if (!strcmp(attr->name, "show_day_names")) {
			if (attr->value[0] == 'T')
				dopt |= GTK_CALENDAR_SHOW_DAY_NAMES;
		} else if (!strcmp(attr->name, "no_month_change")) {
			if (attr->value[0] == 'T')
				dopt |= GTK_CALENDAR_NO_MONTH_CHANGE;
		} else if (!strcmp(attr->name, "show_week_numbers")) {
			if (attr->value[0] == 'T')
				dopt |= GTK_CALENDAR_SHOW_WEEK_NUMBERS;
		} else if (!strcmp(attr->name, "week_start_monday")) {
			if (attr->value[0] == 'T')
				dopt |= GTK_CALENDAR_WEEK_START_MONDAY;
		}
	}
	gtk_calendar_display_options(GTK_CALENDAR(wid), dopt);
	return wid;
}

static void
window_build_children(GladeXML *self, GtkWidget *w,
			      GladeWidgetInfo *info, const char *longname)
{
	GList *tmp;

	for (tmp = info->children; tmp != NULL; tmp = tmp->next) {
		GtkWidget *child = glade_xml_build_widget(self, tmp->data,
							  longname);
		g_object_set_data(G_OBJECT(w), "child", child);
	}
}

static GtkWidget *
window_new (GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *win;
	GList *tmp;
	GtkWindowType type = GTK_WINDOW_TOPLEVEL;
	char *title = NULL;

	info->visible = FALSE;

	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;

		switch (attr->name[0]) {
		case 't':
			if (!strcmp(attr->name, "title"))
				title = attr->value;
			else if (!strcmp(attr->name, "type"))
				type = glade_enum_from_string(
					GTK_TYPE_WINDOW_TYPE, attr->value);
			break;
		}
	}
	win = gtk_window_new(type);
	gtk_window_set_title(GTK_WINDOW(win), _(title));
	glade_xml_set_window_props(GTK_WINDOW(win), info);
	glade_xml_set_toplevel(xml, GTK_WINDOW(win));
	return win;
}

static GtkWidget *
pixmap_new(GladeXML *xml, GladeWidgetInfo *info)
{
	GtkWidget *wid;
	GList *tmp;
	gchar *filename = NULL;
	
	for (tmp = info->attributes; tmp; tmp = tmp->next) {
		GladeAttribute *attr = tmp->data;
		
		if (!strcmp(attr->name, "filename")) {
			g_free(filename);
			filename = glade_xml_relative_file(xml, attr->value);
		}
	}
	if (filename)
		wid = gtk_image_new_from_file(filename);
	else
		wid = gtk_image_new();
	g_free(filename);
	return wid;
}


static GtkWidget *
fileentry_new(GladeXML *xml, GladeWidgetInfo *info)
{
    GtkWidget *wid;
    GList *tmp;
    gchar *history_id = NULL, *title = NULL;
    gboolean directory = FALSE, modal = FALSE;

        for (tmp = info->attributes; tmp; tmp = tmp->next) {
        GladeAttribute *attr = tmp->data;

        if (!strcmp(attr->name, "history_id"))
            history_id = attr->value;
        else if (!strcmp(attr->name, "title"))
            title = attr->value;
        else if (!strcmp(attr->name, "directory"))
            directory = (attr->value[0] == 'T');
        else if (!strcmp(attr->name, "modal"))
            modal = (attr->value[0] == 'T');
    }
    wid = gtk_panda_fileentry_new();
    return wid;
}

static void
fileentry_build_children (GladeXML *xml, GtkWidget *w,
              GladeWidgetInfo *info, const char *longname)
{
    GList *tmp;
    GladeWidgetInfo *cinfo = NULL;
    GtkEntry *entry;

    for (tmp = info->children; tmp; tmp = tmp->next) {
        GList *tmp2;
        gchar *child_name = NULL;
        cinfo = tmp->data;
        for (tmp2 = cinfo->attributes; tmp2; tmp2 = tmp2->next) {
            GladeAttribute *attr = tmp2->data;
            if (!strcmp(attr->name, "child_name")) {
                child_name = attr->value;
                break;
            }
        }
        if (child_name && !strcmp(child_name, "GnomeEntry:entry"))
            break;
    }
    if (!tmp)
        return;
    entry = GTK_ENTRY(GTK_PANDA_FILEENTRY(w)->entry);

    for (tmp = cinfo->attributes; tmp; tmp = tmp->next) {
        GladeAttribute *attr = tmp->data;
        if (!strcmp(attr->name, "editable"))
            gtk_entry_set_editable(entry, attr->value[0] == 'T');
        else if (!strcmp(attr->name, "text_visible"))
            gtk_entry_set_visibility(entry, attr->value[0] == 'T');
        else if (!strcmp(attr->name, "text_max_length"))
            gtk_entry_set_max_length(entry, strtol(attr->value,
                                   NULL, 0));
        else if (!strcmp(attr->name, "text"))
            gtk_entry_set_text(entry, attr->value);
    }
    glade_xml_set_common_params(xml, GTK_WIDGET(entry), cinfo, longname);
}

static const GladeWidgetBuildData widget_data[] = {
	/*generalwidgets*/
	{"GtkLabel",			label_new,			NULL},
	{"GtkAccelLabel",		accellabel_new,		NULL},
	{"GtkEntry",			entry_new,			NULL},
#ifdef	USE_PANDA
	{"GtkPandaEntry",		panda_entry_new,	NULL},
	{"GtkNumberEntry",		number_entry_new,	NULL},
	{"GtkPandaHTML",		panda_html_new,		NULL},
	{"GtkPandaPS",			panda_preview_new,		NULL},
#endif
	{"GtkButton",			button_new,			button_build_children},
	{"GtkToggleButton",		togglebutton_new,	button_build_children},
	{"GtkCheckButton",		checkbutton_new,	button_build_children},
	{"GtkRadioButton",		radiobutton_new,	button_build_children},
#ifdef	USE_PANDA
	{"GtkPandaCombo",		panda_combo_new,	panda_combo_build_children},
	{"GtkPandaCList",		panda_clist_new,	panda_clist_build_children},
	{"GtkPandaText",		panda_text_new,		NULL},
	{"GtkPandaTimer",		panda_timer_new,	NULL},
	{"GtkPandaDownload",	panda_download_new,	NULL},
#endif
	{"GtkProgressBar",		progressbar_new,	NULL},
	{"GtkHSeparator",		hseparator_new,		NULL},
	{"GtkVSeparator",		vseparator_new,		NULL},
	{"GtkHBox",				hbox_new,			box_build_children},
	{"GtkVBox",				vbox_new,			box_build_children},
	{"GtkTable",			table_new,			table_build_children},
	{"GtkFixed",			fixed_new,			fixed_build_children},
	{"GtkFrame",			frame_new,			glade_standard_build_children},
	{"GtkNotebook",			notebook_new,		notebook_build_children},
	{"GtkScrolledWindow",	scrolledwindow_new,	glade_standard_build_children},
	{"GtkViewport",			viewport_new,		glade_standard_build_children},
	{"GtkCalendar",			calendar_new,		NULL},
	{"GtkWindow",			window_new,			window_build_children},
/* Gnome widgets in previouse version */
	{"GnomePixmap",			pixmap_new,			NULL},
	{"GnomeFileEntry",		fileentry_new,		fileentry_build_children},
	{NULL,NULL,NULL}
};

void glade_init_gtk_widgets (void) 
{
	glade_register_widgets(widget_data);
}

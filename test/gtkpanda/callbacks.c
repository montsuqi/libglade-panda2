#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "callbacks.h"

gboolean
map_event                              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  fprintf(stderr, "call map_event\n");
  return FALSE;
}

void
set_focus                              (GtkWindow       *window,
                                        GtkWidget       *widget,
                                        gpointer         user_data)
{
  fprintf(stderr, "call set_focus\n");
}


void
destroy                                (GtkObject       *object,
                                        gpointer         user_data)
{
  fprintf(stderr, "call destroy\n");
}

void
on_pandaentry1_activate                 (GtkPandaEntry     *entry,
                                        gpointer         user_data)
{
  fprintf(stderr, "pandaentry1_activate:%s\n", 
    gtk_entry_get_text (GTK_ENTRY (entry)));
}

void
on_pandaentry1_changed                 (GtkPandaEntry     *entry,
                                        gpointer         user_data)
{
  fprintf(stderr, "pandaentry1_changed:%s\n", 
    gtk_entry_get_text (GTK_ENTRY (entry)));
}

void
on_numberentry1_activate                 (GtkNumberEntry     *entry,
                                        gpointer         user_data)
{
  fprintf(stderr, "numberentry1_activate:%s\n", 
    gtk_entry_get_text (GTK_ENTRY (entry)));
}

void
on_numberentry1_changed                 (GtkNumberEntry     *entry,
                                        gpointer         user_data)
{
  fprintf(stderr, "numberentry1_changed:%s\n", 
    gtk_entry_get_text (GTK_ENTRY (entry)));
}

void
on_pandaentry2_activate                 (GtkPandaEntry     *entry,
                                        gpointer         user_data)
{
  fprintf(stderr, "pandaentry2_activate:%s\n", 
    gtk_entry_get_text (GTK_ENTRY (entry)));
}

void
on_pandaentry2_changed                 (GtkPandaEntry     *entry,
                                        gpointer         user_data)
{
  fprintf(stderr, "pandaentry2_changed:%s\n", 
    gtk_entry_get_text (GTK_ENTRY (entry)));
}

void
on_comboentry1_activate                 (GtkPandaEntry     *entry,
                                        gpointer         user_data)
{
  fprintf(stderr, "combo-entry1_activate:%s\n", 
    gtk_entry_get_text (GTK_ENTRY (entry)));
}

void
on_comboentry1_changed                 (GtkPandaEntry     *entry,
                                        gpointer         user_data)
{
  fprintf(stderr, "combo-entry1_changed:%s\n", 
    gtk_entry_get_text (GTK_ENTRY (entry)));
}

void 
on_pandaclist1_select_row               (GtkPandaCList   *clist, 
                                        int row,
                                        int column,
                                        gpointer    user_data)
{
  fprintf(stderr, "clist1_select_row row:%d column:%d\n", row, column);
}

void 
on_pandaclist1_unselect_row             (GtkPandaCList   *clist, 
                                        int row,
                                        int column,
                                        gpointer    user_data)
{
  fprintf(stderr, "clist1_unselect_row row:%d column:%d\n", row, column);
}

void
on_pandatext1_activate                 (GtkPandaText     *text,
                                        gpointer         user_data)
{
  GtkTextBuffer *buffer;
  GtkTextIter start,end;

  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  fprintf(stderr,"pandatext1_activate:%s\n",
    gtk_text_buffer_get_text(buffer, &start, &end, FALSE));
}

void
on_pandatext1_changed                   (GtkPandaText     *text,
                                        gpointer         user_data)
{
  GtkTextBuffer *buffer;
  GtkTextIter start,end;

  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  fprintf(stderr,"pandatext1_changed:%s\n",
    gtk_text_buffer_get_text(buffer, &start, &end, FALSE));
}

void
on_pandatimer1_timeout                 (GtkPandaTimer     *timer,
                                        gpointer         user_data)
{
  fprintf(stderr,"pandatimer1_timeout\n");
}


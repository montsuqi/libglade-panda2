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
on_entry1_activate                     (GtkEditable     *editable,
                                        gpointer         user_data)
{
  fprintf(stderr, "call on_entry1_activate\n");
  fprintf(stderr, "%s\n", gtk_editable_get_chars(editable,0,-1));
}


void
button_toggled                         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  fprintf(stderr, "call button_toggled\n");
}


void
on_button1_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  fprintf(stderr, "call on_button1_clicked\n");
}


#ifndef _CALLBACKS_H
#define _CALLBACKS_H


#include <gtk/gtk.h>

gboolean
map_event                              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
set_focus                              (GtkWindow       *window,
                                        GtkWidget       *widget,
                                        gpointer         user_data);

void
destroy                                (GtkObject       *object,
                                        gpointer         user_data);

void
on_entry1_activate                     (GtkEditable     *editable,
                                        gpointer         user_data);

void
button_toggled                         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_button1_clicked                     (GtkButton       *button,
                                        gpointer         user_data);
#endif

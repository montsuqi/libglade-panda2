#ifndef _CALLBACKS_H
#define _CALLBACKS_H


#include <gtk/gtk.h>
#include <gtkpanda/gtkpanda.h>

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
on_pandaentry1_activate                 (GtkPandaEntry     *entry,
                                        gpointer         user_data);

void
on_pandaentry1_changed                 (GtkPandaEntry     *entry,
                                        gpointer         user_data);

void
on_numberentry1_activate                 (GtkNumberEntry     *entry,
                                        gpointer         user_data);

void
on_numberentry1_changed                 (GtkNumberEntry     *entry,
                                        gpointer         user_data);

void
on_pandaentry2_activate                 (GtkPandaEntry     *entry,
                                        gpointer         user_data);

void
on_pandaentry2_changed                 (GtkPandaEntry     *entry,
                                        gpointer         user_data);

void
on_comboentry1_activate                 (GtkPandaEntry     *entry,
                                        gpointer         user_data);

void
on_comboentry1_changed                 (GtkPandaEntry     *entry,
                                        gpointer         user_data);

void 
on_pandaclist1_select_row               (GtkPandaCList   *clist, 
                                        int row,
                                        int column,
                                        gpointer    user_data);

void 
on_pandaclist1_unselect_row             (GtkPandaCList   *clist, 
                                        int row,
                                        int column,
                                        gpointer    user_data);
void
on_pandatext1_activate                 (GtkPandaText     *text,
                                        gpointer         user_data);

void
on_pandatext1_changed                   (GtkPandaText     *text,
                                        gpointer         user_data);

void
on_pandatimer1_timeout                 (GtkPandaTimer     *timer,
                                        gpointer         user_data);

#endif

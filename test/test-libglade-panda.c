/*
    test-libglade.c: a test program for the libglade library
    Copyright (C) 1998, 1999, 2000  James Henstridge <james@daa.com.au>
      GNOME option parsing code by Miguel de Icaza.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


  At your option, you may use this alternative X style licence:

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES
    OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
    THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include <config.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

const char *filename = NULL, *rootnode = NULL;
gboolean no_connect = FALSE;

int main (int argc, char **argv)
{
  GladeXML *xml;

  gtk_init(&argc, &argv);
  glade_init();

  if (argc < 3) {
    g_print("Usage: %s rootnode filename\n", argv[0]);
  }

  rootnode = argv[1];
  filename = argv[2];

  xml = glade_xml_new(filename, rootnode);

  if (!xml) {
    g_warning("something bad happened while creating the interface");
    return 1;
  }

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(window,1024,768);
  g_signal_connect(G_OBJECT(window), "destroy", 
	G_CALLBACK(gtk_main_quit), NULL);
  GtkWidget *widget = glade_xml_get_widget(xml, rootnode);
  GtkWidget *child = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "child");
  gtk_container_add(GTK_CONTAINER(window), child);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}

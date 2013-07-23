/**
  @file plugin.c

  Example plugin implementation
  <p>
  Copyright (c) 2005 INdT. All rights reserved.

  @author Andre Moreira Magalhaes <andre.magalhaes@indt.org.br>
*/

#include <stdio.h>
#include <gtk/gtk.h>
#include <hildon/hildon.h>
#include <startup_plugin.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include "../socket.h"

static void mode_callback(GtkWidget *widget, gpointer data);

GameStartupInfo gs;
GConfClient *gcc = NULL;
GtkWidget *hostname_entry = NULL;
GtkWidget *mode_picker;

static GtkWidget *
load_plugin (void) {
  GtkWidget *game_vbox;
  GtkWidget *mode_button;

  g_type_init();
  gcc = gconf_client_get_default();

  game_vbox = gtk_vbox_new (TRUE, 0);
  g_assert (game_vbox);

  mode_button = hildon_touch_selector_new_text ();
  hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (mode_button), "Single Player");
  hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (mode_button), "Multiplayer Host");
  hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (mode_button), "Multiplayer Client");
  mode_picker = hildon_picker_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH,
                                                     HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
  hildon_button_set_title (HILDON_BUTTON (mode_picker), "Game Mode");
  hildon_picker_button_set_selector (HILDON_PICKER_BUTTON(mode_picker),
                                     HILDON_TOUCH_SELECTOR(mode_button));
  g_signal_connect(G_OBJECT(mode_picker), "value-changed", G_CALLBACK(mode_callback), NULL);


  hostname_entry = hildon_entry_new (HILDON_SIZE_AUTO);
  gtk_entry_set_text (GTK_ENTRY (hostname_entry), "Enter a hostname or an IP address");
  gtk_widget_set_sensitive(GTK_WIDGET(hostname_entry), FALSE);

  if (gconf_client_get_bool(gcc, BATTLEGWELED_CREATESERVER, NULL))
    hildon_picker_button_set_active (HILDON_PICKER_BUTTON(mode_picker), 1);
  else if (!g_strcmp0("",gconf_client_get_string(gcc, BATTLEGWELED_SERVERIP, NULL))) {
    hildon_picker_button_set_active (HILDON_PICKER_BUTTON(mode_picker), 0);
  }
  else {
    hildon_picker_button_set_active (HILDON_PICKER_BUTTON(mode_picker), 2);
    gtk_entry_set_text(GTK_ENTRY (hostname_entry), gconf_client_get_string(gcc, BATTLEGWELED_SERVERIP, NULL));
    gtk_widget_set_sensitive(GTK_WIDGET(hostname_entry), TRUE);
  }

  gtk_button_set_alignment (GTK_BUTTON (mode_picker), 0.0, 0.5);

  gtk_box_pack_start (GTK_BOX (game_vbox), mode_picker, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (game_vbox), hostname_entry, FALSE, FALSE, 0);


  return game_vbox;
}

static void
write_config (gchar *nick, gchar *ip) {

  if (hildon_picker_button_get_active(HILDON_PICKER_BUTTON(mode_picker)) == 1)
    gconf_client_set_bool(gcc, BATTLEGWELED_CREATESERVER, TRUE, NULL);
  else
    gconf_client_set_bool(gcc, BATTLEGWELED_CREATESERVER, FALSE, NULL);

  if (hildon_picker_button_get_active(HILDON_PICKER_BUTTON(mode_picker)) == 2)
    gconf_client_set_string(gcc, BATTLEGWELED_SERVERIP, gtk_entry_get_text(GTK_ENTRY(hostname_entry)) , NULL);
  else
    gconf_client_set_string(gcc, BATTLEGWELED_SERVERIP, "", NULL);

}

static void mode_callback(GtkWidget *widget, gpointer data) {
  if (hildon_picker_button_get_active (HILDON_PICKER_BUTTON(widget)) == 2)
    gtk_widget_set_sensitive(GTK_WIDGET(hostname_entry), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(hostname_entry), FALSE);
}


static StartupPluginInfo plugin_info = {
  load_plugin,
  NULL,
  write_config,
  NULL,
  NULL,
  NULL
};

STARTUP_INIT_PLUGIN(plugin_info, gs, FALSE, FALSE)


/**
  @file plugin.c

  Example plugin implementation
  <p>
  Copyright (c) 2005 INdT. All rights reserved.

  @author Andre Moreira Magalhaes <andre.magalhaes@indt.org.br>
*/

#include <stdio.h>
#include <gtk/gtk.h>
#include <startup_plugin.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include "../socket.h"

GameStartupInfo gs;
GConfClient *gcc = NULL;

static GtkWidget *
load_plugin (void) {
  GtkWidget *game_vbox;

  g_type_init();
  gcc = gconf_client_get_default();

  game_vbox = gtk_vbox_new (TRUE, 0);
  g_assert (game_vbox);

  return game_vbox;
}

static void
set_multiplayer_data(gchar *nick, gchar *ip) {
  gconf_client_set_bool(gcc, BATTLEGWELED_CREATESERVER, TRUE, NULL);
  gconf_client_set_string(gcc, BATTLEGWELED_SERVERIP, "", NULL);
}

static StartupPluginInfo plugin_info = {
  load_plugin,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  set_multiplayer_data
};

STARTUP_INIT_PLUGIN(plugin_info, gs, FALSE, FALSE)


plugin_PROGRAMS = battlegweled_single_plugin.so #battlegweled_mpcreate_plugin.so battlegweled_mpjoin_plugin.so
plugindir = @datadir@/battlegweled

battlegweled_single_plugin_so_SOURCES = plugin_single.c
#battlegweled_mpcreate_plugin_so_SOURCES = plugin_mpcreate.c
#battlegweled_mpjoin_plugin_so_SOURCES = plugin_mpjoin.c

battlegweled_single_plugin_so_CFLAGS = $(GTK_CFLAGS) $(HILDON_CFLAGS) $(GCONF_CFLAGS) $(SDL_CFLAGS)
#battlegweled_mpcreate_plugin_so_CFLAGS = $(GTK_CFLAGS) $(HILDON_CFLAGS) $(GCONF_CFLAGS) $(SDL_CFLAGS)
#battlegweled_mpjoin_plugin_so_CFLAGS = $(GTK_CFLAGS) $(HILDON_CFLAGS) $(GCONF_CFLAGS) $(SDL_CFLAGS)

battlegweled_single_plugin_so_LDFLAGS = -shared
#battlegweled_mpcreate_plugin_so_LDFLAGS = -shared
#battlegweled_mpjoin_plugin_so_LDFLAGS = -shared


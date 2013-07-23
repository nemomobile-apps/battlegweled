SUBDIRS = launcher plugins

bin_PROGRAMS = battlegweled

battlegweled_SOURCES = 	\
	callbacks.c \
	images.c \
	level.c \
	main.c \
	socket.c

battlegweled_CFLAGS = 					\
	-DMAEMO						\
	-DHGW_FUNC=1					\
	$(SDL_CFLAGS) 					\
	$(GCONF_CFLAGS)					\
	-DPIXMAPSPREFIX=\"$(datadir)/battlegweled/pixmaps/\"

battlegweled_LDADD = $(SDL_LIBS) $(SDL_MIXER_LIBS) $(HGW_LIBS) $(GCONF_LIBS) $(SDL_NET_LIBS) -losso


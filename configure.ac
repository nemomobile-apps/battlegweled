AC_PREREQ(2.59)
AC_INIT

AM_INIT_AUTOMAKE(battlegweled, 0.9)

AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

CFLAGS="$CFLAGS -Wall -Wmissing-prototypes -Wmissing-declarations"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h locale.h memory.h stdlib.h string.h unistd.h hgw/hgw.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_STDBOOL
AC_TYPE_UID_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_SETVBUF_REVERSED
AC_CHECK_FUNCS([atexit memset setlocale])

dnl Check for sdl-config
AC_CHECK_PROG(HAVE_SDL_CONFIG, sdl-config, true, false)
AM_CONDITIONAL(HAVE_SDL_CONFIG,$HAVE_SDL_CONFIG)

dnl Give error and exit if we don't have sdl
if test "x$HAVE_SDL_CONFIG" = "xno"; then
  AC_MSG_ERROR(you need SDL development packages installed!)
fi

SDL_CFLAGS=`sdl-config --cflags`
SDL_LIBS=`sdl-config --libs`
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)

AC_CHECK_LIB(SDL_net, 
  SDLNet_Init, 
  SDL_NET_LIBS="-lSDL_net",
  AC_MSG_ERROR([you need SDL_net development packages installed!]) 
)
AC_SUBST(SDL_NET_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.2.0)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(HILDON, hildon-1 >= 0.8.1 hildon-fm-2 >= 0.8.1 libosso >= 0.8.1)
AC_SUBST(HILDON_CFLAGS)
AC_SUBST(HILDON_LIBS)
AC_SUBST(OSSO_CFLAGS)
AC_SUBST(OSSO_LIBS)

PKG_CHECK_MODULES(GCONF, gconf-2.0 >= 2.2.0)
AC_SUBST(GCONF_CFLAGS)
AC_SUBST(GCONF_LIBS)

dnl Check for hildon-games-startup.
AC_CHECK_LIB(hgw, 
   hgw_context_compat_init, 
   HGW_LIBS="-lhgw",
   AC_MSG_ERROR([you need hildon-games-wrapper development packages installed!]) 
)

AC_SUBST(HGW_LIBS)

wrapperdir=/usr/games/wrapper
AC_SUBST(wrapperdir)

dbuslibpathdir=`pkg-config --variable=libdir dbus-1`
dbusconfdir=/etc/dbus-1/system.d
dbusservdir=${dbuslibpathdir}/dbus-1.0/services
AC_SUBST(dbuslibpathdir)
AC_SUBST(dbusconfdir)
AC_SUBST(dbusservdir)

AC_CONFIG_FILES([Makefile
src/Makefile
src/launcher/Makefile
src/launcher/battlegweled.c
src/plugins/Makefile
data/Makefile
data/br.org.indt.battlegweled.service
data/battlegweled.game
data/pixmaps/Makefile
])
AC_OUTPUT

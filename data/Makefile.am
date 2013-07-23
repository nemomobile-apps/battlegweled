SUBDIRS = pixmaps

bin_SCRIPTS = battlegweled.sh

dbusdir = $(datadir)/dbus-1/services
dbus_DATA = \
	    br.org.indt.battlegweled.service \
	    br.org.indt.battlegweled_startup.service

configdir = $(datadir)/battlegweled
config_DATA = battlegweled.conf

desktopdir = $(datadir)/applications/hildon
desktop_DATA = battlegweled.desktop

gamedir = $(wrapperdir)/games
game_DATA = battlegweled.game

EXTRA_DIST = \
	$(dbus_DATA) \
	$(game_DATA) \
	$(desktop_DATA) \
	$(bin_SCRIPTS)

edit = sed \
 	-e 's,@datadir\@,$(datadir),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@VERSION\@,$(VERSION),g'

battlegweled.sh: battlegweled.sh.in
	@$(edit) $< > $@

battlegweled.conf: battlegweled.conf.in
	@$(edit) $< > $@

battlegweled.desktop: battlegweled.desktop.in
	@$(edit) $< > $@

br.org.indt.battlegweled.service: br.org.indt.battlegweled.service.in
	@$(edit) $< > $@

br.org.indt.battlegweled_startup.service: br.org.indt.battlegweled_startup.service.in
	@$(edit) $< > $@

CLEANFILES = $(bin_SCRIPTS) $(config_DATA) $(desktop_DATA) $(dbus_DATA)

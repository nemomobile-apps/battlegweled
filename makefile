
TARGET = battlegweled
SOURCES = $(wildcard src/*.c)
PIXMAPS = $(wildcard data/pixmaps/*)

PKGS = sdl2 glesv1_cm

CFLAGS += $(shell pkg-config --cflags $(PKGS))
LIBS += $(shell pkg-config --libs $(PKGS))

DESTDIR ?=
PIXMAPSPREFIX ?= /opt/$(TARGET)/pixmaps

CFLAGS += -DPIXMAPSPREFIX=\"$(PIXMAPSPREFIX)/\"

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)

install: $(TARGET)
	install -d $(DESTDIR)/opt/$(TARGET)
	install -m755 $(TARGET) $(DESTDIR)/opt/$(TARGET)
	install -d $(DESTDIR)$(PIXMAPSPREFIX)
	install -m644 $(PIXMAPS) $(DESTDIR)$(PIXMAPSPREFIX)
	install -d $(DESTDIR)/usr/share/applications
	install -m644 $(TARGET).desktop $(DESTDIR)/usr/share/applications

clean:
	rm -f $(TARGET)

.PHONY: all clean


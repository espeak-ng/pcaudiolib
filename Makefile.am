AUTOMAKE_OPTIONS = subdir-objects

localedir  = $(datadir)/locale

AM_CFLAGS = \
	-Isrc/include

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES =

EXTRA_DIST = config.rpath
CLEANFILES =

############################# libtool #########################################

EXTRA_DIST += config.guess config.sub ltmain.sh

# Increment if the interface has changed and is not backward compatible
CURRENT=0

# Increment  if source files have changed
# Reset to 0 if the interface has changed
REVISION=0

# Increment  if the interface is backward compatible (superset)
# Reset to 0 if the interface is not backward compatible
AGE=0

LIBPCAUDIO_VERSION=$(CURRENT):$(REVISION):$(AGE)

############################# ChangeLog #######################################

ChangeLog:
	git log > ChangeLog

dist-hook: ChangeLog

.PHONY: ChangeLog

EXTRA_DIST += ChangeLog

############################# libpcaudio ######################################

libpcaudio_includedir = $(includedir)/pcaudiolib
libpcaudio_include_HEADERS = \
	src/include/pcaudiolib/audio.h

lib_LTLIBRARIES += src/libpcaudio.la

src_libpcaudio_la_LDFLAGS = -version-info $(LIBPCAUDIO_VERSION)

src_libpcaudio_la_CFLAGS = ${AM_CFLAGS}

src_libpcaudio_la_SOURCES = \
	src/audio.c

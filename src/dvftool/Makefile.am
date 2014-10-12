#

DVFTOOL_SOURCES = \
	dvftool.c

DVFTOOL_LIBS = \
	$(top_builddir)/src/dvf/libdvf.la\
    -L/usr/lib/x86_64-linux-gnu -lSDL2

DVFTOOL_CFLAGS = \
	-I$(top_srcdir)/src/dvf \
	-I/usr/include/SDL2 -D_REENTRANT

bin_PROGRAMS = dvftool
dvftool_SOURCES = $(DVFTOOL_SOURCES)
dvftool_LDADD = $(DVFTOOL_LIBS)
dvftool_CFLAGS = $(DVFTOOL_CFLAGS)

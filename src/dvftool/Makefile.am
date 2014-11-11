#

DVFTOOL_SOURCES = \
	dvftool.c

DVFTOOL_LIBS = \
	$(top_builddir)/src/file/libdvf_file.la \
	$(SDL2_LIBS)

DVFTOOL_CFLAGS = \
	-I$(top_srcdir)/src/file \
	$(SDL2_CFLAGS)

bin_PROGRAMS = dvftool
dvftool_SOURCES = $(DVFTOOL_SOURCES)
dvftool_LDADD = $(DVFTOOL_LIBS)
dvftool_CFLAGS = $(DVFTOOL_CFLAGS)

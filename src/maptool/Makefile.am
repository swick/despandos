
MAPTOOL_SOURCES = \
	maptool.c

MAPTOOL_LIBS = \
	$(top_builddir)/src/file/libdvm_file.la \
	$(top_builddir)/src/file/libdvd_file.la \
	$(SDL2_LIBS)

MAPTOOL_CFLAGS = \
	-I$(top_srcdir)/src/file \
	$(SDL2_CFLAGS)

bin_PROGRAMS = maptool
maptool_SOURCES = $(MAPTOOL_SOURCES)
maptool_LDADD = $(MAPTOOL_LIBS)
maptool_CFLAGS = $(MAPTOOL_CFLAGS)


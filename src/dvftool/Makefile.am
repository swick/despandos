#

DVFTOOL_SOURCES = \
	dvftool.c

DVFTOOL_CFLAGS = \
	-I$(top_srcdir)/src/dvf

DVFTOOL_LIBS = \
	$(top_builddir)/src/dvf/libdvf.a

bin_PROGRAMS = dvftool
dvftool_SOURCES = $(DVFTOOL_SOURCES)
dvftool_LDADD = $(DVFTOOL_LIBS)
dvftool_CFLAGS = $(DVFTOOL_CFLAGS)


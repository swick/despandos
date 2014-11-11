

LIBDVF_SOURCES = \
    file.c \
	dvf.c

LIBDVM_SOURCES = \
    file.c \
	dvm.c

LIBDVD_SOURCES = \
    file.c \
    dvd.c

LIBDVM_CFLAGS = \
    $(BZIP2_CFLAGS) \
    $(ZLIB_CFLAGS)

LIBDVM_LIBS = \
    $(BZIP2_LIBS) \
    $(ZLIB_LIBS)

noinst_LTLIBRARIES =

if NEED_DVF_FILE
noinst_LTLIBRARIES += libdvf_file.la
libdvf_file_la_SOURCES = $(LIBDVF_SOURCES)
endif

if NEED_DVM_FILE
noinst_LTLIBRARIES += libdvm_file.la
libdvm_file_la_SOURCES = $(LIBDVM_SOURCES)
libdvm_file_la_LIBADD = $(LIBDVM_LIBS)
libdvm_file_la_CFLAGS = $(LIBDVM_CFLAGS)
endif

if NEED_DVD_FILE
noinst_LTLIBRARIES += libdvd_file.la
libdvd_file_la_SOURCES = $(LIBDVD_SOURCES)

noinst_PROGRAMS = dvdtest
dvdtest_SOURCES = dvdtest.c
dvdtest_LDADD = libdvd_file.la
endif

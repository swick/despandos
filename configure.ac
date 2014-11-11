AC_INIT([despandos], [0.1], [sebastian@sebastianwick.net])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CC

AC_PROG_RANLIB
AM_PROG_AR
LT_INIT
PKG_PROG_PKG_CONFIG

AC_ARG_ENABLE([dvftool],
    [AS_HELP_STRING([--enable-dvftool],
        [enable dvftool @<:@default=enabled@:>@])],
    [enable_dvftool="$enableval"],
    [enable_dvftool=yes])

AC_ARG_ENABLE([maptool],
    [AS_HELP_STRING([--enable-maptool],
        [enable maptool @<:@default=enabled@:>@])],
    [enable_maptool="$enableval"],
    [enable_maptool=yes])

AS_IF([test "x$enable_dvftool" = xyes], [
    HAVE_DVFTOOL=yes
    NEED_DVF_FILE=yes
    NEED_SDL2=yes
])

AS_IF([test "x$enable_maptool" = xyes], [
    HAVE_MAPTOOL=yes
    NEED_DVM_FILE=yes
    NEED_DVD_FILE=yes
    NEED_SDL2=yes
])

AS_IF([test "x$NEED_SDL2" = xyes], [
    PKG_CHECK_MODULES([SDL2], [sdl2])
])

AS_IF([test "x$NEED_DVM_FILE" = xyes], [
    # find lib bzip2
    AC_CHECK_HEADERS([bzlib.h],, [AC_MSG_ERROR(["libbz2 not found"])])
    AC_CHECK_LIB([bz2], [BZ2_bzopen],, [AC_MSG_ERROR(["libbz2 not found"])])
    BZIP2_LIBS="-L${libdir} -lbz2"
    BZIP2_CFLAGS="-I${includedir} -DBZ_IMPORT"
    AC_SUBST([BZIP2_LIBS])
    AC_SUBST([BZIP2_CFLAGS])

    # find zlib
    PKG_CHECK_MODULES([ZLIB], [zlib])
])

AM_CONDITIONAL(HAVE_DVFTOOL, test "x$HAVE_DVFTOOL" = xyes)
AM_CONDITIONAL(HAVE_MAPTOOL, test "x$HAVE_MAPTOOL" = xyes)

AM_CONDITIONAL(NEED_DVF_FILE, test "x$NEED_DVF_FILE" = xyes)
AM_CONDITIONAL(NEED_DVM_FILE, test "x$NEED_DVM_FILE" = xyes)
AM_CONDITIONAL(NEED_DVD_FILE, test "x$NEED_DVD_FILE" = xyes)

AC_CONFIG_FILES([
  Makefile
  src/file/Makefile
  src/dvftool/Makefile
  src/maptool/Makefile
])
AC_OUTPUT

echo ""
echo "    dvftool: $enable_dvftool"
echo "    maptool: $enable_maptool"
echo ""
echo "    Run '${Make-make}' to build despandos"
echo ""

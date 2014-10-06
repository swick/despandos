AC_INIT([despandos], [0.1], [sebastian@sebastianwick.net])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CC

AC_PROG_RANLIB
AM_PROG_AR

AC_ARG_ENABLE([dvftool],
    [AS_HELP_STRING([--enable-dvftool],
        [enable dvftool @<:@default=enabled@:>@])],
    [enable_dvftool="$enableval"],
    [enable_dvftool=yes])

AM_CONDITIONAL(HAVE_DVFTOOL, test "x$enable_dvftool" = xyes)

AC_CONFIG_FILES([
  Makefile
  src/dvf/Makefile
  src/dvftool/Makefile
])
AC_OUTPUT

echo ""
echo "    dvftool: $enable_dvftool"
echo ""
echo "    Run '${Make-make}' to build despandos"
echo ""

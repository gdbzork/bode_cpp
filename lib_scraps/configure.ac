AC_PREREQ([2.59])
AC_INIT([BodeScraps], [1.0.0], [gdbzork@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror tar-pax])
AC_CONFIG_SRCDIR([src/testWrite.c])

AC_PROG_CXX

AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

AC_CHECK_FUNCS([memmove strchr strdup])

AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug data generation (def=no)])],
    [debugit="$enableval"],
    [debugit=no])
AC_MSG_RESULT([$debugit])

if test x"$debugit" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    CXXFLAGS="$CXXFLAGS -g -Wall -Werror -Wno-uninitialized -O0"
fi

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

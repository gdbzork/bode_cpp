AC_PREREQ([2.59])
AC_INIT([BodeIO], [1.0.0], [gdbzork@gmail.com])
AC_CONFIG_AUX_DIR([auxdir])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror tar-pax])
AC_CONFIG_SRCDIR([src/bodeio.cpp])
AC_CONFIG_HEADERS([src/bode/config.h])

m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR
AC_PROG_CXX
AC_PROG_LIBTOOL

AC_CHECK_PROGS([DOXYGEN],[doxygen],[zork])
AM_CONDITIONAL([HAVE_DOXYGEN],[test $DOXYGEN != zork])

AC_CHECK_PROGS([CXXTESTGEN],[cxxtestgen],[zork])
AM_CONDITIONAL([HAVE_CXXTESTGEN],[test $CXXTESTGEN != zork])

AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug data generation (def=no)])],
    [debugit="$enableval"],
    [debugit=no])
AC_MSG_RESULT([$debugit])

if test x"$debugit" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -g -Wall -Werror -Wno-uninitialized -O0"
    CXXFLAGS=""
fi

AC_CHECK_LIB([hts],[bam_init1])
AC_CHECK_HEADERS([libhts/hts.h])

AC_SUBST([AM_CXXFLAGS])
AC_SUBST([CXXFLAGS])

AC_HEADER_STDC
AC_CHECK_HEADERS([string.h])

AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

AC_FUNC_STAT
AC_FUNC_ERROR_AT_LINE

AC_CONFIG_FILES([Makefile src/Makefile src/bode/Makefile test/Makefile])
AC_OUTPUT

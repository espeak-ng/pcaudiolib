AC_PREREQ([2.65])
AC_INIT([Portable C Audio Library], [0.1], [https://github.com/rhdunn/pcaudiolib/issues], [], [https://github.com/rhdunn/pcaudiolib])
AM_INIT_AUTOMAKE()

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

dnl ================================================================
dnl Program checks.
dnl ================================================================

AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([

    Configuration for Portable C Audio Library complete.

        Source code location:          ${srcdir}

        Compiler:                      ${CC}
        Compiler flags:                ${CFLAGS}
])

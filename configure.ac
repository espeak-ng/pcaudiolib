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
dnl PulseAudio checks.
dnl ================================================================

AC_ARG_WITH([pulseaudio],
    [AS_HELP_STRING([--with-pulseaudio], [support for PulseAudio output @<:@default=yes@:>@])],
    [])

if test "$with_pulseaudio" = "no"; then
    echo "Disabling PulseAudio output support"
    have_pulseaudio=no
else
    PKG_CHECK_MODULES(PULSEAUDIO, [libpulse-simple >= 0.9],
        [
            AC_DEFINE(HAVE_PULSE_SIMPLE_H, [], [Do we have pulse/simple.h])
            have_pulseaudio=yes
        ],[
            have_pulseaudio=no
        ])
fi

AC_SUBST(PULSEAUDIO_CFLAGS)
AC_SUBST(PULSEAUDIO_LIBS)

dnl ================================================================
dnl ALSA checks.
dnl ================================================================

AC_ARG_WITH([alsa],
    [AS_HELP_STRING([--with-alsa], [support for ALSA audio output @<:@default=yes@:>@])],
    [])

if test "$with_alsa" = "no"; then
    echo "Disabling ALSA audio output support"
    have_alsa=no
else
    PKG_CHECK_MODULES(ALSA, [alsa],
        [
            AC_DEFINE(HAVE_ALSA_ASOUNDLIB_H, [], [Do we have ALSA])
            have_alsa=yes
        ],[
            have_alsa=no
        ])
fi

AC_SUBST(ALSA_CFLAGS)
AC_SUBST(ALSA_LIBS)

dnl ================================================================
dnl OSS checks.
dnl ================================================================

AC_ARG_WITH([oss],
    [AS_HELP_STRING([--with-oss], [support for OSS audio output @<:@default=yes@:>@])],
    [])

if test "$with_oss" = "no"; then
    echo "Disabling OSS audio output support"
    have_oss=no
else
    AC_CHECK_HEADERS([sys/soundcard.h],[
        have_oss=yes
    ],[
        AC_CHECK_HEADERS([linux/soundcard.h],[
            have_oss=yes
        ],[
            have_oss=no
        ])
    ])
fi

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

	PulseAudio support:            ${have_pulseaudio}
	ALSA support:                  ${have_alsa}
	OSS support:                   ${have_oss}
])

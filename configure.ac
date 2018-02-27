AC_PREREQ([2.65])
AC_INIT([pcaudiolib], [1.1], [https://github.com/rhdunn/pcaudiolib/issues], [], [https://github.com/rhdunn/pcaudiolib])
AM_INIT_AUTOMAKE()

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST

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

AS_IF([test "x$with_pulseaudio" = "xno"],
    [
        echo "Disabling PulseAudio output support";
        have_pulseaudio=no
    ], [
        PKG_CHECK_MODULES(PULSEAUDIO, [libpulse-simple >= 0.9],
        [
            AC_DEFINE(HAVE_PULSE_SIMPLE_H, [], [Do we have pulse/simple.h])
            have_pulseaudio=yes
        ],[
            have_pulseaudio=no
])
])

AC_SUBST(PULSEAUDIO_CFLAGS)
AC_SUBST(PULSEAUDIO_LIBS)

dnl ================================================================
dnl ALSA checks.
dnl ================================================================

AC_ARG_WITH([alsa],
    [AS_HELP_STRING([--with-alsa], [support for ALSA audio output @<:@default=yes@:>@])],
    [])

AS_IF([test "x$with_alsa" = "xno"], [
    echo "Disabling ALSA audio output support";
    have_alsa=no
    ], [
        PKG_CHECK_MODULES(ALSA, [alsa],
        [
            AC_DEFINE(HAVE_ALSA_ASOUNDLIB_H, [], [Do we have ALSA])
            have_alsa=yes
        ],[
            have_alsa=no
        ])])

AC_SUBST(ALSA_CFLAGS)
AC_SUBST(ALSA_LIBS)

dnl ================================================================
dnl QSA checks.
dnl ================================================================

AC_ARG_WITH([qsa],
    [AS_HELP_STRING([--with-qsa], [support for QSA audio output @<:@default=yes@:>@])],
    [])

if test "$with_qsa" = "no"; then
    echo "Disabling QSA audio output support"
    have_qsa=no
else
    AC_CHECK_HEADERS([sys/asoundlib.h],[
        AC_CHECK_HEADERS([sys/asound.h],[
            have_qsa=yes
            QSA_LIBS=-lasound
        ],[
            have_qsa=no
        ])
    ],[
        have_qsa=no
    ])
fi

AC_SUBST(QSA_LIBS)

dnl ================================================================
dnl coreaudio checks.
dnl ================================================================

AC_ARG_WITH([coreaudio],
    [AS_HELP_STRING([--with-coreaudio], [support for coreaudio audio output on Mac OS @<:@default=yes@:>@])],
    [])

if test "$with_coreaudio" = "no";then
    echo "Disabling coreaudio audio output support"
    have_coreaudio=no
else
    case $host_os in
        *darwin*)
            COREAUDIO_CFLAGS="-dynamiclib -fvisibility=default"
            COREAUDIO_LIBS="-framework CoreAudio -framework AudioToolbox -framework AudioUnit"
            have_coreaudio=yes
            AC_DEFINE(HAVE_COREAUDIO_H, [], [Do we have coreaudio?])
            ;;
        *)
            have_coreaudio=no
            ;;
    esac
fi

AC_SUBST(COREAUDIO_CFLAGS)
AC_SUBST(COREAUDIO_LIBS)
AM_CONDITIONAL([HAVE_COREAUDIO], [test "x${have_coreaudio}" = "xyes"])

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
        AC_CHECK_LIB(ossaudio, _oss_ioctl)
        have_oss=yes
    ],[
        AC_CHECK_HEADERS([linux/soundcard.h],[
            AC_CHECK_LIB(ossaudio, _oss_ioctl)
            have_oss=yes
        ],[
            AC_CHECK_HEADERS([soundcard.h],[
                AC_CHECK_LIB(ossaudio, _oss_ioctl)
                have_oss=yes
            ],[
                have_oss=no
            ])
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
	QSA support:                   ${have_qsa}
	Coreaudio support:             ${have_coreaudio} 
	OSS support:                   ${have_oss}
])

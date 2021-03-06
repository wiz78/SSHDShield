dnl Process this file with autoconf to produce a configure script.

dnl
dnl                              configure.in
dnl                          -------------------
dnl copyright            : (C) 2005 by Simone Tellini
dnl email                : tellini@users.sourceforge.net
dnl revision             : $Id: configure.in,v 1.1.1.1 2005/10/04 10:50:23 wiz Exp $
dnl

dnl -----------------------------------
dnl Setup
dnl -----------------------------------

AC_INIT(SSHDShield, 1.2, tellini@users.sourceforge.net)
AC_CONFIG_SRCDIR(src/Makefile.am)

AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(src/include/config.h:src/include/config-h.in)

AM_INIT_AUTOMAKE([dist-bzip2])

AC_LANG([C++])

AX_CXX_COMPILE_STDCXX([11])

AC_PREFIX_DEFAULT(/usr/local)

dnl -----------------------------------
dnl Checks for programs.
dnl -----------------------------------
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_CHECK_TOOL( SED, sed )

dnl -----------------------------------
dnl Check for headers
dnl -----------------------------------
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME

AC_CHECK_HEADERS(string.h strings.h unistd.h syslog.h sys/syslog.h)

dnl -----------------------------------
dnl Checks for typedefs, structures and
dnl compiler characteristics
dnl -----------------------------------
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T

dnl -----------------------------------
dnl Check for libraries functions
dnl -----------------------------------
AC_CHECK_FUNCS(memset memcpy strdup strncmp)

dnl -----------------------------------
dnl Misc functions checks
dnl -----------------------------------

AC_CACHE_CHECK([whether vsyslog accepts a stdarg va_list], [sshdshield_cv_var_VSYSLOG],
[AC_TRY_LINK(
	[#ifdef HAVE_SYSLOG_H
	 #include <syslog.h>
	 #endif
	 #ifdef HAVE_SYS_SYSLOG_H
	 #include <sys/syslog.h>
	 #endif
	 #include <stdarg.h>],
	[ void foo( int a, char *b, ... ) { va_list ap; va_start( ap, b ); vsyslog( a, b, ap ); va_end( ap ); } ],
	[logminerac_cv_var_VSYSLOG=yes],
	[logminerac_cv_var_VSYSLOG=no])
])
if test "$sshdshield_cv_var_VSYSLOG" = yes ; then
	AC_DEFINE( HAVE_VSYSLOG, 1, [Define if there is a stdarg-compatible vsyslog function] )
fi

dnl -----------------------------------
dnl Substitute symbols
dnl -----------------------------------
AC_SUBST(ac_aux_dir)
AC_SUBST(sysconfdir)
AC_SUBST(CPPFLAGS)
AC_SUBST(CXXFLAGS)

dnl -----------------------------------
dnl Genereate the output files
dnl -----------------------------------
AC_OUTPUT([Makefile src/Makefile src/sshdshield/Makefile src/include/Makefile],
		  [test -z "$CONFIG_HEADERS" || echo timestamp > src/include/stamp-h.in])


/***************************************************************************
	revision             : $Id: logger.h,v 1.1.1.1 2005/09/05 17:19:03 tellini Exp $
    copyright            : (C) 2005 by Simone Tellini
    email                : tellini@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SYSLOG_H
#	include <syslog.h>
#else
#	ifdef HAVE_SYS_SYSLOG_H
#		include <sys/syslog.h>
#	endif
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdarg.h>

class Logger
{
public:
	static void		Setup( void );
	static void		Cleanup( void );

	static void		Log( int level, char *fmt, ... );
	static void		VLog( int level, char *fmt, va_list ap );

	static void		SetLogToStdErr( bool log )	{ LogToStdErr = log; }
	static void		SetLogToSysLog( bool log )	{ LogToSysLog = log; }

private:
	static bool		LogToStdErr;
	static bool		LogToSysLog;
};

#endif

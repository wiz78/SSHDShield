/***************************************************************************
	revision             : $Id: logger.cpp,v 1.1.1.1 2005/09/05 17:19:03 tellini Exp $
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

#include "main.h"

#ifndef HAVE_VSYSLOG
#include <stdio.h>
#endif

//--------------------------------------------------------------------------
bool Logger::LogToStdErr;
bool Logger::LogToSysLog;

//--------------------------------------------------------------------------
void Logger::Setup( void )
{
	LogToStdErr = false;
	LogToSysLog = true;

	openlog( PACKAGE, LOG_PID | LOG_NDELAY, LOG_AUTHPRIV );
}
//--------------------------------------------------------------------------
void Logger::Cleanup( void )
{
	closelog();
}
//--------------------------------------------------------------------------
void Logger::Log( int level, const char *fmt, ... )
{
	va_list		ap;

	va_start( ap, fmt );

	VLog( level, fmt, ap );

	va_end( ap );
}
//--------------------------------------------------------------------------
void Logger::VLog( int level, const char *fmt, va_list ap )
{
#ifdef HAVE_VSYSLOG
	if( LogToSysLog )
		vsyslog( level, fmt, ap );

	if( LogToStdErr ) {
		vfprintf( stderr, fmt, ap );
		fprintf( stderr, "\n" );
	}
#else
	char	buf[ 512 ];

	vsnprintf( buf, sizeof( buf ), fmt, ap );
	
	if( LogToSysLog )
		syslog( level, "%s", buf );

	if( LogToStdErr )
		fprintf( stderr, "%s\n", buf );
#endif
}
//--------------------------------------------------------------------------

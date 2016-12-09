/***************************************************************************
	revision             : $Id: logfile.cpp,v 1.1.1.1 2005/10/04 10:50:23 wiz Exp $
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
#include "utils.h"
#include "failedPwd.h"
#include "logger.h"

//---------------------------------------------------------------------------
FailedPwd::FailedPwd( const string& host )
{
	Host = host;
}
//---------------------------------------------------------------------------
bool FailedPwd::CheckExpire( time_t expireTime )
{
	while(( Failures.size() > 0 ) && ( Failures[ 0 ] < expireTime )) {
		char tm[ 256 ];

		strftime( tm, sizeof( tm ), "%c", localtime( &Failures[ 0 ] ));

		Logger::Log( LOG_INFO, "Removing failure occurred at %s for host %s (%d left)", 
					 tm, Host.c_str(), Failures.size() - 1 );
			
		Failures.pop_front();
	}

	return( Failures.size() <= 0 );
}
//---------------------------------------------------------------------------
int FailedPwd::AddFailure( void )
{
	time_t	tm;

	time( &tm );
	Failures.push_back( tm );

	Logger::Log( LOG_INFO, "Watched host %s has %d failure(s)",
				 Host.c_str(), Failures.size() );

	return( Failures.size() );
}
//---------------------------------------------------------------------------

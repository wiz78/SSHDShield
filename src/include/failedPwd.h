/***************************************************************************
	revision             : $Id: logfile.h,v 1.1.1.1 2005/10/04 10:50:23 wiz Exp $
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

#ifndef FAILEDPWD_H
#define FAILEDPWD_H

#include <string>
#include <deque>

using namespace std;

class FailedPwd
{
public:
						FailedPwd( const string& host );

	const string&		GetHost( void ) const 					{ return( Host ); }

	int					AddFailure( void );

	bool				CheckExpire( time_t expireTime );

private:
	string				Host;
	deque<time_t>		Failures;
};

#endif /* FAILEDPWD_H */

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

#ifndef BANNEDHOST_H
#define BANNEDHOST_H

#include <string>

using namespace std;

class BannedHost
{
public:
						BannedHost( const string& host )
						{
							Host = host;

							time( &InsertTime );
						}

	const string&		GetHost( void ) const 					{ return( Host ); }
	time_t				GetInsertTime( void ) const				{ return( InsertTime ); }

private:
	string				Host;
	time_t				InsertTime;
};

#endif /* BANNEDHOST_H */

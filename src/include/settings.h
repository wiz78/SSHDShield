/***************************************************************************
	revision             : $Id: settings.h,v 1.1.1.1 2005/10/04 10:50:23 wiz Exp $
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>
#include <string>
#include <unordered_map>

using namespace std;    

class Settings
{
public:
	void							Load( const string &file );

	string							GetString( const string &section, const string &key, const string& def = "" );
	vector<string>					GetStringVector( const string &section, const string &key );
	int								GetInt( const string &section, const string &key, int def = 0 );
	
	const vector<string> 			&GetSections( void ) const { return( Sections ); }

private:
	unordered_map<string, string>	Cfg;
	vector<string>					Sections;
	
	string							GetHashKey( const string& section, const string& key ) const;
};

#endif /* SETTINGS_H */

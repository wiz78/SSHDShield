/***************************************************************************
	revision             : $Id: settings.cpp,v 1.1.1.1 2005/10/04 10:50:23 wiz Exp $
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
#include "settings.h"
#include "utils.h"

#include <fstream>
#include <algorithm>
#include <cctype>

//---------------------------------------------------------------------------
void Settings::Load( const string &file )
{
	ifstream	fh( file.c_str() );
	
	if( fh ) {
	
		fh.exceptions( ifstream::badbit );
		
		try {
			int 	ln = 1;
			string	section;

			while( !fh.eof() ) {
				string line;
				
				getline( fh, line );

				line = Utils::Trim( line );

				while( !fh.eof() && ( line[ line.length() - 1 ] == '\\' )) {
					string s;

					getline( fh, s );

					line.erase( line.length() - 1, 1 );
					line.append( Utils::Trim( s ));
				}
				
				if( line[0] == '[' ) {

					section = line.substr( 1, line.length() - 2 );
					
					Sections.push_back( section );
						
				} else if( !line.empty() && ( line[0] != '#' ) && ( line[0] != ';' )) {
					string::size_type pos = line.find( "=" );
					
					if( pos == string::npos )
						fprintf( stderr, "Configuration error at line %d: no value specified\n", ln );
					else {
						string key = Utils::Trim( line.substr( 0, pos ));
						string val = Utils::Trim( line.substr( pos + 1 ));

						Cfg[ GetHashKey( section, key ) ] = val;
					}
				}
				
				ln++;
			}
		}
		catch( ifstream::failure& e ) {
			fprintf( stderr, "Error while reading the configuration: %s\n", e.what() );
		}

		fh.close();
		
	} else
		fprintf( stderr, "Configuration file not found!\n" );
}
///---------------------------------------------------------------------------
string Settings::GetString( string const& section, string const& key, const string& def )
{
	string hashKey = GetHashKey( section, key );
	string ret = def;

	if( Cfg.find( hashKey ) != Cfg.end() )
		ret = Cfg[ hashKey ];

	return( ret );
}
//---------------------------------------------------------------------------
vector<string> Settings::GetStringVector( string const& section, string const& key )
{
	vector<string>	ret;
	string			str = GetString( section, key );

	while( !str.empty() ) {
		bool	inQuotes = false, escaped = false, eos = false;
		int		i;
		string	item;

		for( i = 0; !eos && ( i < str.length() ); i++ ) {
			char ch = str[ i ];
			
			switch( ch ) {

				case '\\':
					if( escaped )
						item += '\\';

					escaped = !escaped;
					break;

				case '"':
					if( escaped ) {
							
						item    += '"';
						escaped  = false;
						
					} else
						inQuotes = !inQuotes;
					break;

				case ' ':
					if( inQuotes )
						item += ' ';
					else
						eos = true;
					break;

				case 'r':
					item    += escaped ? '\r' : 'r';
					escaped  = false;
					break;

				case 'n':
					item    += escaped ? '\n' : 'n';
					escaped  = false;
					break;

				default:
					item    += ch;
					escaped  = false;
					break;
			}
		}

		str.erase( 0, i );
		
		item = Utils::Trim( item );

		if( !item.empty() )
			ret.push_back( item );
	}

	return( ret );
}
//---------------------------------------------------------------------------
int Settings::GetInt( string const& section, string const& key, int def )
{
	string hashKey = GetHashKey( section, key );

	if( Cfg.find( hashKey ) != Cfg.end() )
		def = atoi( Cfg[ hashKey ].c_str() );
		
	return( def );
}
//---------------------------------------------------------------------------
string Settings::GetHashKey( string const& section, const string& key ) const
{
	string hash = section + "/" + key;
	
	transform( hash.begin(), hash.end(), hash.begin(), static_cast<int(*)(int)>( tolower ));
	
	return( hash );
}
//---------------------------------------------------------------------------

/***************************************************************************
	revision             : $Id: hash_map.h,v 1.1.1.1 2005/10/04 10:50:23 wiz Exp $
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

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <string>
#if HAVE_HASH_MAP
#include <hash_map>
#else
#include <ext/hash_map>
#endif

using namespace std;
using namespace __gnu_cxx;

namespace __gnu_cxx
{
	template<>
	struct hash<string>
	{
		size_t operator()( const string& s ) const
		{
			const collate<char>& c = use_facet<collate<char> >( locale::classic() );
			
			return c.hash( s.c_str(), s.c_str() + s.size() );
		}
	};
}

#endif /* HASH_MAP_H */

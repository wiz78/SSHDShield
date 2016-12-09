/***************************************************************************
	revision             : $Id: utils.h,v 1.1.1.1 2005/10/04 10:50:23 wiz Exp $
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

#ifndef UTILS_H
#define UTILS_H

#include <string>

class Utils
{
public:
	static std::string	Trim( std::string const& source, char const* delims = " \t\r\n" );
};

#endif /* UTILS_H */

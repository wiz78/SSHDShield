/***************************************************************************
	revision             : $Id: logsplitter.h,v 1.1.1.1 2005/10/04 10:50:23 wiz Exp $
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

#ifndef SSHDSHIELD_H
#define SSHDSHIELD_H

#include "settings.h"
#include "failedPwd.h"
#include "bannedHost.h"

#include <list>
#include <string>
#include <unordered_map>

using namespace std;

class SSHDShield
{
public:
						SSHDShield( int argc, const char *argv[] );
						~SSHDShield();

	void				Run( void );
	void				ReopenLog( void )	{ Reopen = true; }
	void				Quit( void )		{ Running = false; }

private:
	typedef unordered_map<string, FailedPwd *>	failuresHash;
	typedef unordered_map<string, bool>			bannedHash;
	
	string				CfgName;
	Settings			Cfg;
	bool				Running;
	bool				Reopen;
	char				Buffer[ 1024 * 64 ];
	bool				Overflowed;
	int					BufUsed;
	list<BannedHost *>	BannedHosts;
	bannedHash			BannedHash;
	failuresHash		FailedHash;
	list<FailedPwd *>	FailedPwds;
	time_t				LastCheck;
	string				SSHIdent;
	int					MaxFailedPwds;
	int					FailedPwdsInterval;
	int					BanTime;
	string				BanCmd;
	string				UnbanCmd;
	string				InvalidUserMsg;
	string				WrongPwdMsg;
	string				HostPrefix;

	bool				Daemonize( void );
	bool				CheckPid( void );

	int					OpenLog( void );
	void				Setup( void );

	void				ReadData( int fd ); 
	void				CheckLine( void );

	void				ParseLine( string line );
	bool				CheckIdent( string line );
	string				GetHost( string line );

	void				Ban( string host );
	void				SpawnCmd( string cmd, string host );
	void				AddFailedPwd( string host );

	void				CheckExpires( void );
	void				ExpireFailedPwds( time_t now );
	void				ExpireBans( time_t now );
};

#endif /* SSHDSHIELD_H */

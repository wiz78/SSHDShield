/***************************************************************************
	revision             : $Id: logsplitter.cpp,v 1.2 2005/10/05 14:21:15 wiz Exp $
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
#include "sshdshield.h"
#include "exceptions.h"
#include "logger.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sys/poll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>

#define DEFAULT_PID_FILE "/var/run/sshdshield.pid"

//---------------------------------------------------------------------------
SSHDShield::SSHDShield( int argc, const char *argv[] )
{
	if( argc < 2 )
		throw EWrongArgs();
		
	CfgName    = argv[ 1 ];
	Running    = true;
	Reopen     = false;
	BufUsed    = 0;
	Overflowed = false;
	
	time( &LastCheck );

	Cfg.Load( CfgName );
	Logger::Setup();
}
//---------------------------------------------------------------------------
SSHDShield::~SSHDShield()
{
	for( list<FailedPwd *>::iterator i = FailedPwds.begin(); i != FailedPwds.end(); ++i )
		delete *i;
	
	for( list<BannedHost *>::iterator i = BannedHosts.begin(); i != BannedHosts.end(); ++i ) {
		BannedHost *host = *i;

		SpawnCmd( UnbanCmd, host->GetHost() );
			
		delete host;
	}

	Logger::Cleanup();
}
//---------------------------------------------------------------------------
bool SSHDShield::Daemonize( void )
{
	bool	child = false;
	pid_t	pid = fork();

	if( pid == -1 )
		Logger::Log( LOG_CRIT, "fork() failed - %s", strerror( errno ));

	else if( pid == 0 ) { // child

		if( CheckPid() ) {

			child = true;

			// we don't need the standard file descriptors, but
			// closing them is a *bad* idea: some libraries
			// we use may think it could be nice to use them,
			// causing havoc
			freopen( "/dev/null", "r", stdin  );
			freopen( "/dev/null", "w", stdout );
			freopen( "/dev/null", "w", stderr );

			setsid();
			umask( 027 );
			chdir( "/" );
		}
	}

	return( child ); 
}
//--------------------------------------------------------------------------
bool SSHDShield::CheckPid( void )
{
	bool		ret = true;
	string		pidFile = Cfg.GetString( "General", "PIDFile", DEFAULT_PID_FILE );
	ifstream	ifh( pidFile.c_str() );

	if( ifh ) {

		ifh.close();

		ret = false;

		printf( PACKAGE" is already running\n" );

	} else {
		ofstream	ofh( pidFile.c_str() );

		if( ofh ) {

			ofh << getpid();

			ofh.close();
		}
	}

	return( ret );
} 
//---------------------------------------------------------------------------
void SSHDShield::Run( void )
{
	if( Daemonize() ) {
		int fd = -1;

		try {
			fd = OpenLog();
				
			Setup();

			while( Running ) {

				sleep( 1 );

				ReadData( fd );

				CheckExpires();

				if( Reopen ) {

					close( fd );

					fd     = OpenLog();
					Reopen = false;
				}
			}
		}
		catch( runtime_error& e ) {
			Logger::Log( LOG_ERR, "%s", e.what() );
		}
		catch(...) {
			Logger::Log( LOG_ERR, "Unhandled exception caught" );
		}

		if( fd >= 0 )
			close( fd );

		unlink( Cfg.GetString( "General", "PIDFile", DEFAULT_PID_FILE ).c_str() );
	}
}
//---------------------------------------------------------------------------
int SSHDShield::OpenLog( void )
{
	string	file = Cfg.GetString( "General", "LogPath", "" );
	int		fd;

	fd = open( file.c_str(), O_RDONLY );

	if( fd < 0 )
		throw runtime_error( "Couldn't open the log file \"" + file + "\"" );

	lseek( fd, 0, SEEK_END );

	fcntl( fd, F_SETFL, fcntl( fd, F_GETFL, 0 ) | O_NONBLOCK );

	return( fd );
}
//---------------------------------------------------------------------------
void SSHDShield::Setup( void )
{
	SSHIdent    	   = Cfg.GetString( "General", "SSHIdent", "sshd" );
	
	MaxFailedPwds      = Cfg.GetInt( "PasswordFailures", "MaxAllowed", 3 );
	FailedPwdsInterval = Cfg.GetInt( "PasswordFailures", "Interval", 60 );

	BanTime            = Cfg.GetInt( "Ban", "BanTime", 7200 );
	BanCmd             = Cfg.GetString( "Ban", "BanCmd", "iptables -A banned -s %h -j DROP" );
	UnbanCmd           = Cfg.GetString( "Ban", "UnbanCmd", "iptables -D banned -s %h -j DROP" );

	InvalidUserMsg     = Cfg.GetString( "Strings", "InvalidUser", "invalid user" );
	WrongPwdMsg        = Cfg.GetString( "Strings", "WrongPassword", "password" );
	HostPrefix         = Cfg.GetString( "Strings", "HostPrefix", "from" );

	transform( SSHIdent.begin(), SSHIdent.end(), SSHIdent.begin(), static_cast<int(*)(int)>( tolower ));
	transform( InvalidUserMsg.begin(), InvalidUserMsg.end(), InvalidUserMsg.begin(), static_cast<int(*)(int)>( tolower ));
	transform( WrongPwdMsg.begin(), WrongPwdMsg.end(), WrongPwdMsg.begin(), static_cast<int(*)(int)>( tolower ));
	transform( HostPrefix.begin(), HostPrefix.end(), HostPrefix.begin(), static_cast<int(*)(int)>( tolower ));
}
//---------------------------------------------------------------------------
void SSHDShield::ReadData( int fd )
{
	int avail = sizeof( Buffer ) - BufUsed, len;
	
	// should never happen, but if it does, swallow the first part of the line
	if( avail < 1 ) {
	
		avail      = sizeof( Buffer );
		BufUsed    = 0;
		Overflowed = true;
	}
	
	len = read( fd, &Buffer[ BufUsed ], avail );
	
	if( len > 0 ) {

		BufUsed += len;
	
		CheckLine();
	}
}
//---------------------------------------------------------------------------
void SSHDShield::CheckLine( void )
{
	bool gotLine;
	
	do {
		gotLine = false;
	
		for( int i = 0; i < BufUsed; i++ )
			if( Buffer[ i ] == '\n' ) {
				
				Buffer[ i ] = '\0';
				
				if(( i > 0 ) && ( Buffer[ i - 1 ] == '\r' ))
					Buffer[ i - 1 ] = '\0';
				
				ParseLine( string( Buffer ));
			
				BufUsed -= i + 1;
	
				if( BufUsed )
					memcpy( Buffer, &Buffer[ i + 1 ], BufUsed );
			
				i          = BufUsed;
				Overflowed = false;
				gotLine    = true;
			}

	} while( gotLine );
}
//---------------------------------------------------------------------------
void SSHDShield::ParseLine( string line )
{
	transform( line.begin(), line.end(), line.begin(), static_cast<int(*)(int)>( tolower ));

	if( CheckIdent( line )) {

		if( line.find( InvalidUserMsg ) != string::npos )
			Ban( GetHost( line ));
		else if( line.find( WrongPwdMsg ) != string::npos )
			AddFailedPwd( GetHost( line ));	
	}
}
//---------------------------------------------------------------------------
void SSHDShield::SpawnCmd( string cmd, string host )
{
	if( !host.empty() && ( fork() == 0 )) {
		string::size_type	pos = 0;
		
		while(( pos = cmd.find( "%h", pos )) != string::npos )
			cmd.replace( pos, 2, host );

		execl( "/bin/sh", "sh", "-c", cmd.c_str(), NULL );

		exit( 0 );
	}

}
//---------------------------------------------------------------------------
void SSHDShield::AddFailedPwd( string host )
{
	failuresHash::iterator	i = FailedHash.find( host );
	FailedPwd				*pwd;

	if( i != FailedHash.end() )
		pwd = i->second;
	else {
			
		pwd = new FailedPwd( host );

		FailedHash[ host ] = pwd;
	
		FailedPwds.push_back( pwd );
	}

	if( pwd->AddFailure() >= MaxFailedPwds )
		Ban( host );
}
//---------------------------------------------------------------------------
bool SSHDShield::CheckIdent( string line )
{
	string::size_type 	i = 0;
	bool				ok = true;

	for( int j = 0; ok && ( j < 4 ); j++ ) {

		i = line.find( ' ', i );

		if( i != string::npos )
			i = line.find_first_not_of( ' ', i );
		else
			ok = false;
	}

	ok  = ( i != string::npos ) && ( line.compare( i, SSHIdent.length(), SSHIdent ) == 0 );
	i  += SSHIdent.length();

	return( ok && (( line[ i ] == '[' ) || ( line[ i ] == ':' )));
}
//---------------------------------------------------------------------------
string SSHDShield::GetHost( string line )
{
	string::size_type	i = line.find( HostPrefix );
	string				ret;

	if( i != string::npos ) {

		i   = line.find_first_not_of( ' ', i + HostPrefix.length() );
		ret = line.substr( i );
		i   = ret.find( ' ' );

		if( i != string::npos )
			ret.erase( i );
	}

	return( ret );
}
//---------------------------------------------------------------------------
void SSHDShield::Ban( string host )
{
	if( BannedHash.find( host ) == BannedHash.end() ) {
			
		SpawnCmd( BanCmd, host );
		BannedHosts.push_back( new BannedHost( host ));

		BannedHash[ host ] = true;
	}
}
//---------------------------------------------------------------------------
void SSHDShield::CheckExpires( void )
{
	time_t	now;
	
	time( &now );

	if( LastCheck != now ) {

		ExpireFailedPwds( now );
		ExpireBans( now );

		LastCheck = now;
	}
}
//---------------------------------------------------------------------------
void SSHDShield::ExpireFailedPwds( time_t now )
{
	list<FailedPwd *>::iterator	i = FailedPwds.begin();
	time_t						expire = now - FailedPwdsInterval;

	while( i != FailedPwds.end() ) {
		list<FailedPwd *>::iterator	next = i;
		FailedPwd					*pwd = *i;

		++next;

		if( pwd->CheckExpire( expire )) {

			Logger::Log( LOG_INFO, "Removing watched host: %s", pwd->GetHost().c_str() );
			
			FailedPwds.erase( i );
			FailedHash.erase( pwd->GetHost() );
						
			delete pwd;
		}

		i = next;
	}
}
//---------------------------------------------------------------------------
void SSHDShield::ExpireBans( time_t now )
{
	list<BannedHost *>::iterator	i = BannedHosts.begin();
	time_t							expire = now - BanTime;

	while(( i != BannedHosts.end() ) && (( *i )->GetInsertTime() < expire )) {
		list<BannedHost *>::iterator	next = ++i;
		BannedHost						*host = *( --i );

		SpawnCmd( UnbanCmd, host->GetHost() );
		
		BannedHosts.erase( i );
		BannedHash.erase( host->GetHost() );	
		
		delete host;

		i = next;
	}
}
//---------------------------------------------------------------------------

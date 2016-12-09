/***************************************************************************
	revision             : $Id: main.cpp,v 1.1.1.1 2005/10/04 10:50:23 wiz Exp $
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

#include <signal.h>
#include <sys/wait.h>

//---------------------------------------------------------------------------
static void SigChildExit( int signum );
static void SigTerm( int signum );
static void SigHUP( int signum );

static SSHDShield *App = NULL;

//---------------------------------------------------------------------------
int main( int argc, const char *argv[] )
{
    signal( SIGCHLD, SigChildExit );
    signal( SIGTERM, SigTerm      );
    signal( SIGINT,  SigTerm      );
    signal( SIGHUP,  SigHUP       );
	
	try {
		App = new SSHDShield( argc, argv );
			
		App->Run();
	}
	catch( const EWrongArgs& e ) {
		fprintf( stderr,
				 PACKAGE " v" VERSION " - (c) by Simone Tellini <tellini@users.sourceforge.net>\n"
			     "\n"
				 "This program is free software; you can redistribute it and/or modify\n"
				 "it under the terms of the GNU General Public License as published by\n"
				 "the Free Software Foundation; either version 2 of the License, or\n"
				 "(at your option) any later version.\n"
				 "\n"
				 "Wrong arguments.\n"
				 "\n"
				 "Usage:\n"
				 "\n"
				 "\t%s <config file>\n",
				 argv[ 0 ] );
	}
	catch( const char *p ) {
		fprintf( stderr, "ERROR: %s", p );
	}
	catch(...) {
		fprintf( stderr, "Unhandled exception!" );
	}

	delete App;

	return( EXIT_SUCCESS );
}
//--------------------------------------------------------------------------
static void SigChildExit( int signum )
{
    int status;

    while( waitpid( -1, &status, WNOHANG ) > 0 );

    signal( SIGCHLD, SigChildExit );
}
//--------------------------------------------------------------------------
static void SigTerm( int signum )
{
	if( App )
		App->Quit();
}
//--------------------------------------------------------------------------
static void SigHUP( int signum )
{
	if( App )
		App->ReopenLog();

	signal( SIGHUP, SigHUP );
}
//--------------------------------------------------------------------------

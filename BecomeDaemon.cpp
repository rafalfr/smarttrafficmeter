#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include "BecomeDaemon.h"

using namespace std;

/** @brief BecomeDaemon
  *
  * @description Returns 0 on success, -1 on error
  * @todo: document this function
  */
int BecomeDaemon( int flags )
{
	int maxfd, fd;

	switch ( fork() )                   /* Become background process */
	{
	case -1:
		return -1;

	case 0:
		break;                     /* Child falls through... */

	default:
		_exit( EXIT_SUCCESS );     /* while parent terminates */
	}

	if ( setsid() == -1 )               /* Become leader of new session */
		return -1;

	switch ( fork() )                   /* Ensure we are not session leader */
	{
	case -1:
		return -1;

	case 0:
		break;

	default:
		_exit( EXIT_SUCCESS );
	}

	if ( !( flags & BD_NO_UMASK0 ) )
		umask( 0 );                     /* Clear file mode creation mask */

	if ( !( flags & BD_NO_CHDIR ) )
	{
		if ( chdir( "/" ) != 0 )
		{
			cout << "can't change working directory" << endl;
		}
	}

	if ( !( flags & BD_NO_CLOSE_FILES ) ) /* Close all open files */
	{
		maxfd = sysconf( _SC_OPEN_MAX );

		if ( maxfd == -1 )              /* Limit is indeterminate... */
			maxfd = BD_MAX_CLOSE;       /* so take a guess */

		for ( fd = 0; fd < maxfd; fd++ )
			close( fd );
	}

	if ( !( flags & BD_NO_REOPEN_STD_FDS ) )
	{
		close( STDIN_FILENO );          /* Reopen standard fd's to /dev/null */

		fd = open( "/dev/null", O_RDWR );

		if ( fd != STDIN_FILENO )       /* 'fd' should be 0 */
			return -1;

		if ( dup2( STDIN_FILENO, STDOUT_FILENO ) != STDOUT_FILENO )
			return -1;

		if ( dup2( STDIN_FILENO, STDERR_FILENO ) != STDERR_FILENO )
			return -1;
	}

	return 0;
}

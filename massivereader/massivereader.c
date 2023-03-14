#include "massivereader.h"

int main( int argc, char* argv[] )
{
	signal( SIGPIPE, SIG_IGN );

	int opt;
	
	port = strtol( argv[1], NULL, 10 );

	while( ( opt = getopt( argc, argv, "O:" ) ) != -1 )
	{
		switch(opt)
		{
			case 'O':
				strcpy( pathToFile, optarg );
				break;
			default:
				break;
		}
	}

	pathLength = strlen(pathToFile);

	if( pathToFile[pathLength - 1] != '/' )
		pathToFile[pathLength + 1] = '/';

	createFileName();

	struct sigaction act;

	memset( &act, '\0', sizeof(act) );

	act.sa_handler = handlerSIGUSR1;

	if( sigaction( SIGUSR1, &act, NULL ) == -1 )
		errorExit("sigaction");

	waitForTCP();	

	exit(EXIT_SUCCESS);
}



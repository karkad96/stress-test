#include "multiwriter.h"

int main( int argc, char* argv[] )
{
	int opt;	

	while( ( opt = getopt( argc, argv, "S:p:d:T:" ) ) != -1 )
	{
		switch(opt)
		{
			case 'S':
				connections = strtol( optarg, NULL, 10 );
				break;
			
			case 'p':
				port = strtol( optarg, NULL, 10 );
				break;
			
			case 'd':
				delay = strtof( optarg, NULL );
				break;
			
			case 'T':
				workTime = strtof( optarg, NULL );
				break;
			
			default:
				break;
		}
	}

	connectToServer();

	exit(EXIT_SUCCESS);
}



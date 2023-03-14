#include "multiwriter.h"

int port;
int connections;
float delay;
float workTime;

sockConn sockets[SIZE];


void errorExit(const char* str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

int isValidChar(const char c)
{
	switch(c)
	{
		case '/':
		case '>':
		case '<':
		case '|':
		case ':':
		case '&':
		case '@':
			return 0;
		
		default:
			break;
	}

	return 1;
}

const char* generatePath()
{
	srand( (unsigned int)time(NULL) ); 

	static char path[SIZE] = {0};

	int pathLength = rand()%100 + 1, i;

	for( i = 0; i < pathLength; i++ )
	{
		int c;

		do
		{
			c = rand() % 95 + 32;
		}
		while( !isValidChar( c ) );

		path[i] = c;
	}

	return path;
	
}


void connectToServer()
{
	int sockfd;
	struct sockaddr_in servaddr;

        if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
                errorExit("socket sockfd");

        memset( &servaddr, '\0', sizeof(servaddr) );

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

        if( connect( sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr) )  == -1 )
                errorExit("connect");

	localSocket(sockfd);

}

void localSocket( int sockfd )
{
	struct sockaddr_un cliaddr, conn;

	int sockun;

	char name[SIZE] = {"/tmp/"};

	strcat( name, generatePath() );

	if( ( sockun = socket( AF_LOCAL, SOCK_STREAM, 0 ) ) == -1 )
		errorExit("socket sockun");

	memset(&cliaddr, '\0', sizeof(cliaddr));

	cliaddr.sun_family = AF_LOCAL;
	strncpy( cliaddr.sun_path, name, sizeof(cliaddr.sun_path) - 1 );

	if( bind( sockun, (struct sockaddr*)&cliaddr, sizeof(struct sockaddr_un) ) == -1 )
		errorExit("bind sockun");

	if( listen( sockun, connections ) == -1 )
		errorExit("listen");

	int sfd;
	int i = 0;

	while( connections-- )
	{
		if( write( sockfd, &cliaddr, sizeof(cliaddr) ) == -1 )
			errorExit("write msgsock");
	
		if( ( sfd = accept( sockun, 0, 0 ) ) == -1 ) 
			errorExit("accept");
		
		if( read( sockfd, &conn, sizeof(conn) ) == -1 )
			errorExit("read msgsock");

		if( conn.sun_family != -1 )
		{
			sockets[i].sockfd = sfd;
			sockets[i].addr = conn;
			i++;
		}
	}

	if( shutdown( sockfd, SHUT_RDWR ) == -1 )
		errorExit("shutdown");

	if( close(sockun) == -1 )
		errorExit("close sockun");

	sendData( i );

	if( remove(name) == -1 )
		errorExit("remove");
}

void sendData( int size )
{
	srand( (unsigned int)time(NULL) );

	delay = delay / 1000000.0;
	workTime /= 10.0;

	float procTime = 0.0;
	float procTimestamp = 0.0;
	float timeOfService = 0.0;
	float work = 0.0;
	float toNextCycle = 0.0;
	float toNextCycleMax = 0.0;
	float toNextCycleMin = 1238714.0;

	int draw, sockfd, flag = 0;

	struct timespec tim, tim2, processTimestamp, processTime;

	if( clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &processTimestamp ) == -1 )
		errorExit("clock_gettime processTimestamp");

	procTimestamp = (float)processTimestamp.tv_sec + processTimestamp.tv_nsec / 1000000000.0;

	timChars message;

	while(1)
	{
		if( clock_gettime( CLOCK_REALTIME, &tim ) == -1 )
			errorExit("clock_gettime");

		formatTimestamp( tim );

		draw = rand() % size;
		sockfd = sockets[draw].sockfd;

		struct pollfd pll = { sockfd, 0, 0 };

		poll( &pll, 1, 0 );		
	
		if( pll.revents & POLLNVAL )
			continue;

		strcpy(message.timestamp, Timestamp);
		strcpy(message.path, sockets[draw].addr.sun_path);
		message.tim = tim;	
	
		if( write( sockfd, &message, sizeof(timChars) ) == -1 )
			errorExit("write sockun");

		if( clock_gettime( CLOCK_REALTIME, &tim2 ) == -1 )
			errorExit("clock_gettime");

		work = (float)( tim2.tv_sec - tim.tv_sec ) + ( tim2.tv_nsec - tim.tv_nsec ) / 1000000000.0;

		timeOfService += work;
		toNextCycle = delay - work;

		if( toNextCycle <= 0 )
		{
			toNextCycle *= -1.0;
			flag = 1;
		}

		if( toNextCycleMin > toNextCycle )
			toNextCycleMin = toNextCycle;

		if( toNextCycleMax < toNextCycle )
			toNextCycleMax = toNextCycle;
				
		
		if( !flag )
		{
			struct timespec cycleWait = { 1000000000L * toNextCycle / 1000000000L, 1000000000L * ( toNextCycle - (long)toNextCycle ) };
			
			nanosleep( &cycleWait, NULL );
		}

		if( clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &processTime ) == -1 )
			errorExit("clock_gettime processTimes");

		procTime = (float)processTime.tv_sec + processTime.tv_nsec / 1000000000.0 - procTimestamp;

		if( workTime < procTime )
		{
			printf("Total time of servicing: %fs\n",timeOfService);
			printf("Minimal period of time before/after cycle: %fs\n",toNextCycleMin);
			printf("Maximal period of time before/after cycle: %fs\n",toNextCycleMax);
			break;
		}
	}
}

int power(int a, int b)
{
	int res = 1;

	for( int i = 0; i < b; i++ )
		res *= a;
	
	return res;
}

int length(int n)
{
	int sum = 0;

	if( n == 0 ) 
		return 1;

	while( n )
	{
		n /= 10;
		sum++;
	}

	return sum;
}

void formatTimestamp( struct timespec tim )
{
	int sec = tim.tv_sec;
	int nsec = tim.tv_nsec;

	int minutes = sec / 60;
	int seconds = sec % 60;
	int nanoseconds = nsec;

	memset(Timestamp, '\0', sizeof(Timestamp));

	int i = 0;
	int dot = 0;

	int mLength = ( length(minutes) < 2 ) ? 2 : length(minutes);
	int sLength = 2;
	int nsLength = 9;

	while(mLength)
	{
		Timestamp[i++] = ( minutes / power( 10, mLength - 1 ) ) % 10 + '0';
		mLength--;
	}

	Timestamp[i++] = ':';	
	
	while(sLength)
	{
		Timestamp[i++] = ( seconds / power( 10, sLength - 1 ) ) % 10 + '0';
		sLength--;
	}

	Timestamp[i++] = ',';

	while(nsLength)
	{
		Timestamp[i++] = ( nanoseconds / power( 10, nsLength - 1 ) ) % 10 + '0'; 
		nsLength--;
		
		if( nsLength >= 3 && ( ++dot ) % 2 == 0 )
			Timestamp[i++] = '.';
	}
}

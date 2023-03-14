#include "massivereader.h"

long port;

int fd = -1;
int file;
int pathLength;

char fileName[SIZE];
char pathToFile[SIZE];
char Timestamp[SIZE];

sockConn sockets[SIZE];

void handlerSIGUSR1(int n)
{
	createFileName();
}

void createFileName()
{
	if( fd != -1 )
	{
		if( close(fd) == -1 )
			errorExit("close fd");
	}
	
	int exist = 1;

	while(exist)
	{
		for( int i = pathLength, j = 2; j >= 0 ; i++, j-- )
			pathToFile[i] =  ( file / power( 10, j ) ) % 10 + '0'; 

		errno = 0;
		
		if( ( fd = open( pathToFile, O_WRONLY | O_CREAT | O_TRUNC, 0777 ) ) != -1 && errno != EACCES )
			exist = 0;

		file = ( file + 1 ) % 1000;
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

void client( int sockun, struct sockaddr_un conn )
{
	timChars message;

	if( read( sockun, &message, sizeof(timChars) ) == -1 )
		errorExit("read sockun message");

	struct timespec tim;

	if( clock_gettime( CLOCK_REALTIME, &tim ) == -1 )
		errorExit("clock_gettime");

	struct timespec delay = { tim.tv_sec - message.tim.tv_sec, tim.tv_nsec - message.tim.tv_nsec };

	if( strcmp( conn.sun_path, message.path ) != 0 )
		return;

	char formattedDelay[SIZE] = {0};
	char formattedTimestamp[SIZE] = {0};

	formatTimestamp( tim );

	strcpy( formattedTimestamp, Timestamp );

	formatTimestamp( delay );

	strcpy( formattedDelay, Timestamp );

	char logs[3 * SIZE] = {0};

	strcat( logs, formattedTimestamp );
	strcat( logs, " : " );
	strcat( logs, message.timestamp );
	strcat( logs, " : " );
	strcat( logs, formattedDelay );
	strcat( logs, "\n" );

	if( write( fd, logs, strlen(logs) ) == -1 )
		errorExit("write to file");
}

void errorExit(const char* str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

void waitForTCP()
{
	int sockfd, sockun;
	int connfd, iter = 0; 
    	struct sockaddr_in servaddr, cli; 
	struct sockaddr_un conn;
	struct timespec tim = { 0, 10000000 };
	
	if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	       errorExit("socket");

	memset( &servaddr, '\0', sizeof(servaddr) );

    	servaddr.sin_family = AF_INET;
    	servaddr.sin_port = htons(port);
    	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
   
	if ( ( bind( sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr) ) ) == -1 )
		errorExit("bind");
  
    	if ( ( listen( sockfd, 5 ) ) != 0 )
		errorExit("listen");

	socklen_t len = sizeof(cli);
	
	printf("Waiting for connection\n");

	if( ( connfd = accept(sockfd, (struct sockaddr*)&cli, &len) ) == -1 )
		errorExit("accept");

	while( 1 )
	{
		if( ( sockun = socket( AF_LOCAL, SOCK_STREAM, 0 ) ) == -1 )
			errorExit("socket AF_LOCAL");
	       
		if( read( connfd, &conn, sizeof(conn) ) == -1 )
			errorExit("read connfd");

		if( connect( sockun, (struct sockaddr*)&conn, sizeof(conn) ) != -1 )
		{
			sockets[iter].sockfd = sockun;
			sockets[iter].addr = conn;
			iter++;
				
			if( write( connfd, &conn, sizeof(conn) ) == -1 )
				errorExit("write connect");
		}

		else
		{
			conn.sun_family = -1;
			
			if( write( connfd, &conn, sizeof(conn) ) == -1 )
			{
				if( errno == EPIPE )
					break;

				errorExit("write cannot connect");
			}
		}

		nanosleep(&tim, NULL);
	}

	printf("Established %d connection(s) to local domain!\n", iter);

	receiveMessages( iter );

	if( close(sockfd) == -1 )
		errorExit("close sockfd");
}

void receiveMessages( int iter )
{
	struct pollfd fds[iter];

	for( int i = 0; i < iter; i++ )
	{
		fds[i].fd = sockets[i].sockfd;
		fds[i].events = POLLIN;
		fds[i].revents = 0;
	}

	char path[108] = {0};

	strcpy( path, sockets[1].addr.sun_path );	

	while( 1 )
	{
		if( access( path, F_OK ) == -1 )
			break;

		if( poll( fds, iter, 10 ) )
		{
			for( int i = 0; i < iter; i++ )
			{
				//                                     	 ====== tak, wiem, tu dzieja sie dziwne rzeczy, ale wtedy dziala :)
				//                                    	||
				//					\/
				//
				if( ( fds[i].revents & POLLIN ) && ( fds[i].revents - 17 != 0 ) )
				{
					client( sockets[i].sockfd, sockets[i].addr );
					fds[i].revents = 0;
				}
			}
		}
	}
}

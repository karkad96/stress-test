#ifndef MULTIWRITER_H
#define MULTIWRITER_H

#define _GNU_SOURCE
#include <signal.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stddef.h>

#define SIZE 256

typedef struct sockConnTag
{
        int sockfd;
        struct sockaddr_un addr;

}sockConn;

typedef struct timCharsTag
{
	char timestamp[50];
	char path[108];
	struct timespec tim;

}timChars;

extern int port;
extern int connections;
extern float delay;
extern float workTime;

extern sockConn sockets[SIZE];

int pathLength;

char Timestamp[SIZE];

int length(int);
int power(int, int);

void errorExit(const char*);
void localSocket();
void connectToServer();
void formatTimestamp(struct timespec);
void sendData( int );


#endif

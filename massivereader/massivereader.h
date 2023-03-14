#ifndef MASSIVEREADER_H
#define MASSIVEREADER_H

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

extern long port;

extern int fd;
extern int file;
extern int pathLength;

extern char fileName[SIZE];
extern char pathToFile[SIZE];
extern char Timestamp[SIZE];

extern sockConn sockets[SIZE];

int length(int);
int power(int, int);

void errorExit(const char*);
void waitForTCP();
void client(int, struct sockaddr_un);
void createFileName();
void formatTimestamp(struct timespec);
void handlerSIGUSR1(int n);
void receiveMessages(int);

#endif

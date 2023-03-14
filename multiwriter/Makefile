OBJS	= multiwriter.o multiwriterFuncs.o
SOURCE	= multiwriter.c multiwriterFuncs.c
HEADER	= multiwriter.h
OUT	= multiwriter
CC	 = gcc
FLAGS	 = -g -c -Wall

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

multiwriter.o: multiwriter.c
	$(CC) $(FLAGS) multiwriter.c 

multiwriterFuncs.o: multiwriterFuncs.c
	$(CC) $(FLAGS) multiwriterFuncs.c 


clean:
	rm -f $(OBJS) $(OUT)

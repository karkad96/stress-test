OBJS	= massivereader.o massivereaderFuncs.o
SOURCE	= massivereader.c massivereaderFuncs.c
HEADER	= massivereader.h
OUT	= massivereader
CC	 = gcc
FLAGS	 = -g -c -Wall

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

massivereader.o: massivereader.c
	$(CC) $(FLAGS) massivereader.c 

massivereaderFuncs.o: massivereaderFuncs.c
	$(CC) $(FLAGS) massivereaderFuncs.c 


clean:
	rm -f $(OBJS) $(OUT)

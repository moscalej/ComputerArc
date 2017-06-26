# Makefile for the Bank
CC = g++
CFLAGS =  -g -std=c++0x -Wall
CCLINK = $(CC)
OBJS =   cacheSim.o Way.o Cache.o DRam.o
RM = rm -f
TARGET=cacheSim
# Creating the  executable
cacheSim: $(OBJS)
	$(CCLINK) $(CFLAGS) -o cacheSim $(OBJS)
# Creating the object files
cacheSim.o: cacheSim.cpp DRam.h
DRam.o: DRam.cpp DRam.h HelpFunc.h
Cache.o: Cache.cpp Cache.h
Way.o: Way.cpp Way.h


# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* cacheSim.o
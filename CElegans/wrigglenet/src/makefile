# wrigglenet makefile.

CC = gcc

CCFLAGS = -I../../../src -DTHREADS -O3 -c 
LINKLIBS = -lm -lpthread -lstdc++

all: ../bin/wrigglenet

../bin/wrigglenet: wrigglenet.o evolveWrigglenet.o wriggle.o
	$(CC) -o ../bin/wrigglenet wrigglenet.o evolveWrigglenet.o wriggle.o $(LINKLIBS)

wrigglenet.o: wrigglenet.cpp
	$(CC) $(CCFLAGS) wrigglenet.cpp

evolveWrigglenet.o: evolveWrigglenet.hpp evolveWrigglenet.cpp
	$(CC) $(CCFLAGS) evolveWrigglenet.cpp

wriggle.o: wriggle.hpp wriggle.cpp
	$(CC) $(CCFLAGS) wriggle.cpp

clean:
	/bin/rm -f *.o


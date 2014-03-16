# wrigglenet makefile.

CC = gcc

CCFLAGS = -I../../../src -DTHREADS -O3 -c 
LINKLIBS = -lm -lpthread -lstdc++

all: ../bin/wrigglenet

../bin/wrigglenet: wrigglenet.o evolveWriggle.o wriggle.o networkHomomorph.o networkMorph.o network.o neuron.o behavior.o mutableParm.o random.o fileio.o gettime.o
	$(CC) -o ../bin/wrigglenet wrigglenet.o evolveWriggle.o wriggle.o networkHomomorph.o networkMorph.o network.o neuron.o behavior.o mutableParm.o random.o fileio.o gettime.o $(LINKLIBS)

wrigglenet.o: wrigglenet.cpp
	$(CC) $(CCFLAGS) wrigglenet.cpp

evolveWriggle.o: evolveWriggle.hpp evolveWriggle.cpp
	$(CC) $(CCFLAGS) evolveWriggle.cpp

wriggle.o: wriggle.hpp wriggle.cpp
	$(CC) $(CCFLAGS) wriggle.cpp

networkHomomorph.o: ../../../src/networkHomomorph.hpp ../../../src/networkHomomorph.cpp
	$(CC) $(CCFLAGS) ../../../src/networkHomomorph.cpp

networkMorph.o: ../../../src/networkMorph.hpp ../../../src/networkMorph.cpp
	$(CC) $(CCFLAGS) ../../../src/networkMorph.cpp

network.o: ../../../src/network.hpp ../../../src/network.cpp ../../../src/neuron.hpp
	$(CC) $(CCFLAGS) ../../../src/network.cpp

neuron.o: ../../../src/neuron.hpp ../../../src/neuron.cpp
	$(CC) $(CCFLAGS) ../../../src/neuron.cpp

behavior.o: ../../../src/behavior.hpp ../../../src/behavior.cpp ../../../src/network.hpp
	$(CC) $(CCFLAGS) ../../../src/behavior.cpp

mutableParm.o: ../../../src/mutableParm.hpp ../../../src/mutableParm.cpp
	$(CC) $(CCFLAGS) ../../../src/mutableParm.cpp

random.o: ../../../src/random.hpp ../../../src/random.cpp
	$(CC) $(CCFLAGS) ../../../src/random.cpp

fileio.o: ../../../src/fileio.h ../../../src/fileio.cpp
	$(CC) $(CCFLAGS) ../../../src/fileio.cpp

gettime.o: ../../../src/gettime.h ../../../src/gettime.cpp
	$(CC) $(CCFLAGS) ../../../src/gettime.cpp

clean:
	/bin/rm -f *.o


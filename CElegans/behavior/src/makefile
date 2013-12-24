# CElegansBody makefile.

CC = gcc

CCFLAGS = -I../include -I../../../src -O3 -c
LINKLIBS = -L../lib -lsxmlgui -lglpng -lglut -lGLU -lGL -lm -lstdc++

all: ../bin/CElegansBody

../bin/CElegansBody: CElegansBody.o network.o neuron.o behavior.o random.o fileio.o gettime.o
	$(CC) -o ../bin/CElegansBody CElegansBody.o network.o neuron.o behavior.o \
        random.o fileio.o gettime.o $(LINKLIBS)

CElegansBody.o: CElegansBody.cpp
	$(CC) $(CCFLAGS) CElegansBody.cpp

network.o: ../../../src/network.hpp ../../../src/network.cpp ../../../src/neuron.hpp
	$(CC) $(CCFLAGS) ../../../src/network.cpp

neuron.o: ../../../src/neuron.hpp ../../../src/neuron.cpp
	$(CC) $(CCFLAGS) ../../../src/neuron.cpp

behavior.o: ../../../src/behavior.hpp ../../../src/behavior.cpp ../../../src/network.hpp
	$(CC) $(CCFLAGS) ../../../src/behavior.cpp

random.o: ../../../src/random.hpp ../../../src/random.cpp
	$(CC) $(CCFLAGS) ../../../src/random.cpp

fileio.o: ../../../src/fileio.h ../../../src/fileio.cpp
	$(CC) $(CCFLAGS) ../../../src/fileio.cpp

gettime.o: ../../../src/gettime.h ../../../src/gettime.cpp
	$(CC) $(CCFLAGS) ../../../src/gettime.cpp

clean:
	/bin/rm -f *.o

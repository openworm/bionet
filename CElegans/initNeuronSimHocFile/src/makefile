# Initialize NEURON simulation hoc file makefile.

CC = gcc

CCFLAGS = -I../../../src -O3 -c
LINKLIBS = -lm -lstdc++

all: ../bin/initNeuronSimHocFile

../bin/initNeuronSimHocFile: initNeuronSimHocFile.o neuronHocDoc.o random.o fileio.o
	$(CC) -o ../bin/initNeuronSimHocFile initNeuronSimHocFile.o neuronHocDoc.o random.o fileio.o $(LINKLIBS)

initNeuronSimHocFile.o: initNeuronSimHocFile.cpp
	$(CC) $(CCFLAGS) initNeuronSimHocFile.cpp

neuronHocDoc.o: ../../../src/neuronHocDoc.hpp ../../../src/neuronHocDoc.cpp
	$(CC) $(CCFLAGS) ../../../src/neuronHocDoc.cpp

random.o: ../../../src/random.hpp ../../../src/random.cpp
	$(CC) $(CCFLAGS) ../../../src/random.cpp

fileio.o: ../../../src/fileio.h ../../../src/fileio.cpp
	$(CC) $(CCFLAGS) ../../../src/fileio.cpp

clean:
	/bin/rm -f *.o


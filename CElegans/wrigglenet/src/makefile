# wrigglenet makefile.

CC = gcc

CCFLAGS = -I../../../src -DTHREADS -O3 -c 
LINKLIBS = -lm -lpthread -lstdc++

all: ../bin/wrigglenet

../bin/wrigglenet: wrigglenet.o evolveWriggle.o wriggle.o \
neuron.o network.o behavior.o networkMorph.o networkMorphoGenesis.o \
networkHomomorph.o undulationNetworkHomomorph.o neuronSimNetworkHomomorph.o c302SimNetworkHomomorph.o networkHomomorphoGenesis.o \
neuronSim.o neuronHocDoc.o c302Sim.o mutableParm.o random.o fileio.o gettime.o
	$(CC) -o ../bin/wrigglenet wrigglenet.o evolveWriggle.o wriggle.o \
        neuron.o network.o behavior.o \
        networkMorph.o networkMorphoGenesis.o \
        networkHomomorph.o undulationNetworkHomomorph.o neuronSimNetworkHomomorph.o c302SimNetworkHomomorph.o networkHomomorphoGenesis.o \
        neuronSim.o neuronHocDoc.o c302Sim.o mutableParm.o \
        random.o fileio.o gettime.o $(LINKLIBS)

wrigglenet.o: wrigglenet.cpp
	$(CC) $(CCFLAGS) wrigglenet.cpp

evolveWriggle.o: evolveWriggle.hpp evolveWriggle.cpp
	$(CC) $(CCFLAGS) evolveWriggle.cpp

wriggle.o: wriggle.hpp wriggle.cpp
	$(CC) $(CCFLAGS) wriggle.cpp

neuron.o: ../../../src/neuron.hpp ../../../src/neuron.cpp
	$(CC) $(CCFLAGS) ../../../src/neuron.cpp

network.o: ../../../src/network.hpp ../../../src/network.cpp ../../../src/neuron.hpp
	$(CC) $(CCFLAGS) ../../../src/network.cpp

behavior.o: ../../../src/behavior.hpp ../../../src/behavior.cpp ../../../src/network.hpp
	$(CC) $(CCFLAGS) ../../../src/behavior.cpp

networkMorph.o: ../../../src/networkMorph.hpp ../../../src/networkMorph.cpp ../../../src/network.hpp
	$(CC) $(CCFLAGS) ../../../src/networkMorph.cpp

networkHomomorph.o: ../../../src/networkHomomorph.hpp ../../../src/networkHomomorph.cpp ../../../src/networkMorph.hpp ../../../src/network.hpp
	$(CC) $(CCFLAGS) ../../../src/networkHomomorph.cpp

undulationNetworkHomomorph.o: ../../../src/undulationNetworkHomomorph.hpp ../../../src/undulationNetworkHomomorph.cpp ../../../src/networkHomomorph.hpp ../../../src/network.hpp
	$(CC) $(CCFLAGS) ../../../src/undulationNetworkHomomorph.cpp

neuronSimNetworkHomomorph.o: ../../../src/neuronSimNetworkHomomorph.hpp ../../../src/neuronSimNetworkHomomorph.cpp ../../../src/networkHomomorph.hpp ../../../src/network.hpp
	$(CC) $(CCFLAGS) ../../../src/neuronSimNetworkHomomorph.cpp

c302SimNetworkHomomorph.o: ../../../src/c302SimNetworkHomomorph.hpp ../../../src/c302SimNetworkHomomorph.cpp ../../../src/networkHomomorph.hpp ../../../src/network.hpp
	$(CC) $(CCFLAGS) ../../../src/c302SimNetworkHomomorph.cpp

networkMorphoGenesis.o: ../../../src/networkMorphoGenesis.hpp ../../../src/networkMorphoGenesis.cpp
	$(CC) $(CCFLAGS) ../../../src/networkMorphoGenesis.cpp

networkHomomorphoGenesis.o: ../../../src/networkHomomorphoGenesis.hpp ../../../src/networkHomomorphoGenesis.cpp
	$(CC) $(CCFLAGS) ../../../src/networkHomomorphoGenesis.cpp

neuronHocDoc.o: ../../../src/neuronHocDoc.hpp ../../../src/neuronHocDoc.cpp
	$(CC) $(CCFLAGS) ../../../src/neuronHocDoc.cpp

neuronSim.o: ../../../src/neuronSim.hpp ../../../src/neuronSim.cpp
	$(CC) $(CCFLAGS) ../../../src/neuronSim.cpp

c302Sim.o: ../../../src/c302Sim.hpp ../../../src/c302Sim.cpp
	$(CC) $(CCFLAGS) ../../../src/c302Sim.cpp

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


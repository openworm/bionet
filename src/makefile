# bionet makefile.

CC = gcc

CCFLAGS = -DTHREADS -O3 -c
LINKLIBS = -lm -lpthread -lstdc++

all: ../bin/bionet

# Fourier Transform undulation behavior evaluation.
CCFLAGS_FFT = -I../../fftw3/unix/api -DFFT_UNDULATION_EVAL -DTHREADS -O3 -c
LINKLIBS_FFT = -L../../fftw3/unix/.libs -lfftw3 -lm -lpthread -lstdc++

fft_undulation_eval: bionet.o neuron.o network.o behavior.o networkMorph.o networkMorphoGenesis.o \
networkHomomorph.o undulationNetworkHomomorph.o neuronSimNetworkHomomorph.o c302SimNetworkHomomorph.o networkHomomorphoGenesis.o \
networkIsomorph.o networkIsomorphoGenesis.o \
neuronSim.o neuronHocDoc.o c302Sim.o mutableParm.o random.o fileio.o gettime.o
	$(CC) $(CCFLAGS_FFT) networkHomomorph.cpp
	$(CC) -o ../bin/bionet bionet.o neuron.o network.o behavior.o \
        networkMorph.o networkMorphoGenesis.o \
        networkHomomorph.o undulationNetworkHomomorph.o neuronSimNetworkHomomorph.o c302SimNetworkHomomorph.o networkHomomorphoGenesis.o \
        networkIsomorph.o networkIsomorphoGenesis.o \
        neuronSim.o neuronHocDoc.o c302Sim.o mutableParm.o \
        random.o fileio.o gettime.o $(LINKLIBS_FFT)

../bin/bionet: bionet.o neuron.o network.o behavior.o networkMorph.o networkMorphoGenesis.o \
networkHomomorph.o undulationNetworkHomomorph.o neuronSimNetworkHomomorph.o c302SimNetworkHomomorph.o networkHomomorphoGenesis.o \
networkIsomorph.o networkIsomorphoGenesis.o \
neuronSim.o neuronHocDoc.o c302Sim.o mutableParm.o random.o fileio.o gettime.o
	$(CC) -o ../bin/bionet bionet.o neuron.o network.o behavior.o \
        networkMorph.o networkMorphoGenesis.o \
        networkHomomorph.o undulationNetworkHomomorph.o neuronSimNetworkHomomorph.o c302SimNetworkHomomorph.o networkHomomorphoGenesis.o \
        networkIsomorph.o networkIsomorphoGenesis.o \
        neuronSim.o neuronHocDoc.o c302Sim.o mutableParm.o \
        random.o fileio.o gettime.o $(LINKLIBS)

bionet.o: bionet.cpp
	$(CC) $(CCFLAGS) bionet.cpp

neuron.o: neuron.hpp neuron.cpp
	$(CC) $(CCFLAGS) neuron.cpp

network.o: network.hpp network.cpp neuron.hpp
	$(CC) $(CCFLAGS) network.cpp

behavior.o: behavior.hpp behavior.cpp network.hpp
	$(CC) $(CCFLAGS) behavior.cpp

networkMorph.o: networkMorph.hpp networkMorph.cpp network.hpp
	$(CC) $(CCFLAGS) networkMorph.cpp

networkHomomorph.o: networkHomomorph.hpp networkHomomorph.cpp networkMorph.hpp network.hpp
	$(CC) $(CCFLAGS) networkHomomorph.cpp

undulationNetworkHomomorph.o: undulationNetworkHomomorph.hpp undulationNetworkHomomorph.cpp networkHomomorph.hpp network.hpp
	$(CC) $(CCFLAGS) undulationNetworkHomomorph.cpp

neuronSimNetworkHomomorph.o: neuronSimNetworkHomomorph.hpp neuronSimNetworkHomomorph.cpp networkHomomorph.hpp network.hpp
	$(CC) $(CCFLAGS) neuronSimNetworkHomomorph.cpp

c302SimNetworkHomomorph.o: c302SimNetworkHomomorph.hpp c302SimNetworkHomomorph.cpp networkHomomorph.hpp network.hpp
	$(CC) $(CCFLAGS) c302SimNetworkHomomorph.cpp

networkIsomorph.o: networkIsomorph.hpp networkIsomorph.cpp networkMorph.hpp network.hpp
	$(CC) $(CCFLAGS) networkIsomorph.cpp

networkMorphoGenesis.o: networkMorphoGenesis.hpp networkMorphoGenesis.cpp
	$(CC) $(CCFLAGS) networkMorphoGenesis.cpp

networkHomomorphoGenesis.o: networkHomomorphoGenesis.hpp networkHomomorphoGenesis.cpp
	$(CC) $(CCFLAGS) networkHomomorphoGenesis.cpp

networkIsomorphoGenesis.o: networkIsomorphoGenesis.hpp networkIsomorphoGenesis.cpp
	$(CC) $(CCFLAGS) networkIsomorphoGenesis.cpp

neuronHocDoc.o: neuronHocDoc.hpp neuronHocDoc.cpp
	$(CC) $(CCFLAGS) neuronHocDoc.cpp

neuronSim.o: neuronSim.hpp neuronSim.cpp
	$(CC) $(CCFLAGS) neuronSim.cpp

c302Sim.o: c302Sim.hpp c302Sim.cpp
	$(CC) $(CCFLAGS) c302Sim.cpp

mutableParm.o: mutableParm.hpp mutableParm.cpp
	$(CC) $(CCFLAGS) mutableParm.cpp

random.o: random.hpp random.cpp
	$(CC) $(CCFLAGS) random.cpp

fileio.o: fileio.h fileio.cpp
	$(CC) $(CCFLAGS) fileio.cpp

gettime.o: gettime.h gettime.cpp
	$(CC) $(CCFLAGS) gettime.cpp

clean:
	/bin/rm -f *.o


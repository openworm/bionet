# bionet makefile.

CC = gcc

CCFLAGS = -I../../fftw3/unix/api -DTHREADS -O3 -c
LINKLIBS = -L../../fftw3/unix/.libs -lfftw3 -lm -lpthread -lstdc++

all: ../bin/bionet

../bin/bionet: bionet.o network.o neuron.o behavior.o networkMorph.o networkHomomorph.o \
networkIsomorph.o mutableParm.o random.o fileio.o gettime.o
	$(CC) -o ../bin/bionet bionet.o network.o neuron.o behavior.o networkMorph.o \
        networkHomomorph.o networkIsomorph.o mutableParm.o random.o fileio.o gettime.o $(LINKLIBS)

bionet.o: bionet.cpp
	$(CC) $(CCFLAGS) bionet.cpp

network.o: network.hpp network.cpp neuron.hpp
	$(CC) $(CCFLAGS) network.cpp

neuron.o: neuron.hpp neuron.cpp
	$(CC) $(CCFLAGS) neuron.cpp

behavior.o: behavior.hpp behavior.cpp network.hpp
	$(CC) $(CCFLAGS) behavior.cpp

networkMorph.o: networkMorph.hpp networkMorph.cpp network.hpp
	$(CC) $(CCFLAGS) networkMorph.cpp

networkHomomorph.o: networkHomomorph.hpp networkHomomorph.cpp networkMorph.hpp network.hpp
	$(CC) $(CCFLAGS) networkHomomorph.cpp

networkIsomorph.o: networkIsomorph.hpp networkIsomorph.cpp networkMorph.hpp network.hpp
	$(CC) $(CCFLAGS) networkIsomorph.cpp

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


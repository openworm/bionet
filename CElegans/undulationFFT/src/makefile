# undulationFFT makefile.

CC = gcc

CCFLAGS = -I../../../src -I../../../../fftw3/unix/api -O3 -c
LINKLIBS = -L../../../../fftw3/unix/.libs -lfftw3 -lm -lstdc++

all: ../bin/undulationFFT

../bin/undulationFFT: undulationFFT.o mutableParm.o random.o fileio.o
	$(CC) -o ../bin/undulationFFT undulationFFT.o mutableParm.o random.o fileio.o $(LINKLIBS)

undulationFFT.o: undulationFFT.cpp
	$(CC) $(CCFLAGS) undulationFFT.cpp

mutableParm.o: ../../../src/mutableParm.hpp ../../../src/mutableParm.cpp
	$(CC) $(CCFLAGS) ../../../src/mutableParm.cpp

random.o: ../../../src/random.hpp ../../../src/random.cpp
	$(CC) $(CCFLAGS) ../../../src/random.cpp

fileio.o: ../../../src/fileio.h ../../../src/fileio.cpp
	$(CC) $(CCFLAGS) ../../../src/fileio.cpp

clean:
	/bin/rm -f *.o


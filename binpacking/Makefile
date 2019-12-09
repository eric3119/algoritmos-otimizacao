# Compiler binary:
CPP= g++

# Recommended compiler flags for speed:
#	OpenMP enabled
#	full binary code optimization
#	full error and warning reports
#	no range checking within BRKGA:
CFLAGS= -O3 -fopenmp -Wextra -Wall -Weffc++ -ansi -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith

# Compiler flags for debugging; uncomment if needed:
#	range checking enabled in the BRKGA API
#	OpenMP disabled
#	no binary code optimization
#CFLAGS= -DRANGECHECK -Wextra -Wall -Weffc++ -ansi -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith

# Objects:
OBJECTS= Population.o BPDecoder.o binpacking.o

# Targets:
all: binpacking

binpacking: $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -o binpacking.run

binpacking.o:
	$(CPP) $(CFLAGS) -c binpacking.cpp

BPDecoder.o:
	$(CPP) $(CFLAGS) -c BPDecoder.cpp

Population.o:
	$(CPP) $(CFLAGS) -c brkgaAPI/Population.cpp

clean:
	rm -f binpacking.run $(OBJECTS)

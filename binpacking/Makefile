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
OBJECTS= Population.o SampleDecoder.o binpacking.o

# Targets:
all: binpacking

binpacking: $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -o binpacking

binpacking.o:
	$(CPP) $(CFLAGS) -c binpacking.cpp

SampleDecoder.o:
	$(CPP) $(CFLAGS) -c SampleDecoder.cpp

Population.o:
	$(CPP) $(CFLAGS) -c brkgaAPI/Population.cpp

clean:
	rm -f binpacking $(OBJECTS)

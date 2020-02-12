# Compiler binary:
CPP= g++

# Recommended compiler flags for speed:
#	OpenMP enabled
#	full binary code optimization
#	full error and warning reports
#	no range checking within BRKGA:
CFLAGS= -O3 -fopenmp -Wextra -Wall -Weffc++ -std=c++11 -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith
ALLEGROFLAGS= -lallegro -lallegro_primitives -lallegro_dialog -lm
# Compiler flags for debugging; uncomment if needed:
#	range checking enabled in the BRKGA API
#	OpenMP disabled
#	no binary code optimization
#CFLAGS= -DRANGECHECK -Wextra -Wall -Weffc++ -ansi -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith

# Objects:
OBJECTS= draw_bin.o Population.o BPDecoder.o binpacking.o

# Targets:
all: binpacking

binpacking: $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -o binpacking.run $(ALLEGROFLAGS)

draw_bin.o:
	$(CPP) $(CFLAGS) -c draw_bin.cpp $(ALLEGROFLAGS)

binpacking.o:
	$(CPP) $(CFLAGS) -c binpacking.cpp $(ALLEGROFLAGS)

BPDecoder.o:
	$(CPP) $(CFLAGS) -c BPDecoder.cpp $(ALLEGROFLAGS)

Population.o:
	$(CPP) $(CFLAGS) -c brkgaAPI/Population.cpp $(ALLEGROFLAGS)

clean:
	rm -f binpacking.run $(OBJECTS)

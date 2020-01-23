#ifndef BPDECODER_H
#define BPDECODER_H

#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <queue>
#include <limits>
#include <math.h>

typedef struct Space{
    unsigned x, y, X, Y;
    unsigned size;
	unsigned bin_number;

    Space(unsigned x, unsigned y, unsigned X, unsigned Y, unsigned bin_number) 
        : x(x), y(y), X(X), Y(Y), size((X-x) * (Y-y)), bin_number(bin_number){}

    Space(){}

    bool operator>(const struct Space& other) const {
        return size > other.size;
    }
}Space;

typedef struct Box{
    unsigned x, y, X, Y;

    Box(unsigned x, unsigned y, unsigned w, unsigned h) : x(x), y(y), X(x + w), Y(y + h){}
}Box;

class BPDecoder
{
public:
    unsigned bin_w, bin_h;
	
    std::vector< std::pair<unsigned, unsigned> > boxes;

	BPDecoder();  // Constructor
	~BPDecoder(); // Destructor

	// Decode a chromosome, returning its fitness as a double-precision floating point:
	double decode(const std::vector<double> &chromosome) const;
	std::vector<unsigned> DFTRC(std::list<unsigned> &permutation, std::list < Box > &packedBoxes, unsigned &number_of_bins) const;
    std::list < Box > getPackedBoxes(std::list<unsigned> &permutation);
    double fitness(std::list<unsigned> &permutation) const;
    std::list < Space > differenceProcess(std::list < Space > &empty_spaces, Box box) const;
    void eliminationProcess(std::list < Space > &empty_spaces) const;
private:
};

#endif

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
    unsigned size = 0;
	unsigned bin_number;

    Space(unsigned x, unsigned y, unsigned X, unsigned Y, unsigned bin_number) 
        : x(x), y(y), X(X), Y(Y), size((X-x) * (Y-y)), bin_number(bin_number){}

    Space(){}

    bool operator>(const struct Space& other) const {
        return size > other.size;
    }
}Space;

typedef struct Box{
    unsigned x, y, X, Y, bin_number;

    Box(unsigned x, unsigned y, unsigned w, unsigned h, unsigned bin_number) : x(x), y(y), X(x + w), Y(y + h), bin_number(bin_number){}
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
    std::list< Space > eliminationProcess(std::list < Space > &new_spaces) const;
private:
};

#endif

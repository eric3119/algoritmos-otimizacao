#ifndef BPDECODER_H
#define BPDECODER_H

#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <queue>
#include <limits>

typedef struct Space{
    int size;
    int width;
    int height;
	int bin_number;

    Space(int w, int h, int bin_number) : size(w * h), width(w), height(h), bin_number(bin_number){}

    bool operator<(const struct Space& other) const {
        return size < other.size;
    }
}Space;

class BPDecoder
{
public:
	BPDecoder();  // Constructor
	~BPDecoder(); // Destructor

	// Decode a chromosome, returning its fitness as a double-precision floating point:
	double decode(const std::vector<double> &chromosome) const;
	double DFTRC(std::list<unsigned> &permutation) const;
private:
};

#endif

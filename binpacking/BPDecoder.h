#ifndef BPDECODER_H
#define BPDECODER_H

#include <list>
#include <vector>
#include <algorithm>

class BPDecoder
{
public:
	BPDecoder();  // Constructor
	~BPDecoder(); // Destructor

	// Decode a chromosome, returning its fitness as a double-precision floating point:
	double decode(const std::vector<double> &chromosome) const;

private:
};

#endif

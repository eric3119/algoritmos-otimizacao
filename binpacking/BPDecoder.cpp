#include "BPDecoder.h"

int boxes[5][2] = {
	{2, 2},
	{2, 2},
	{2, 2},
	{2, 2},
	{2, 2}
};

BPDecoder::BPDecoder() {}

BPDecoder::~BPDecoder() {}

double BPDecoder::DFTRC(const std::list<unsigned> &permutation) const{

	//PriorityQueue

}

// Runs in O(n \log n):
double BPDecoder::decode(const std::vector<double> &chromosome) const
{
	double myFitness = 0.0;
	typedef std::pair<double, unsigned> ValueKeyPair;
	std::vector<ValueKeyPair> rank(chromosome.size());

	for (unsigned i = 0; i < chromosome.size(); ++i)
	{
		rank[i] = ValueKeyPair(chromosome[i], i);
		myFitness += (double(i + 1) * chromosome[i]);
	}

	// Here we sort 'permutation', which will then produce a permutation of [n]
	// stored in ValueKeyPair::second:
	std::sort(rank.begin(), rank.end());

	// permutation[i].second is in {0, ..., n - 1}; a permutation can be obtained as follows
	std::list<unsigned> permutation;
	for (std::vector<ValueKeyPair>::const_iterator i = rank.begin(); i != rank.end(); ++i)
	{
		permutation.push_back(i->second);
	}
	
	// myFitness = DFTRC(permutation);

	// Return the fitness:
	return myFitness;
}

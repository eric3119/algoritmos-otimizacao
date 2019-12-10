#include "BPDecoder.h"

BPDecoder::BPDecoder() {}

BPDecoder::~BPDecoder() {}

double BPDecoder::DFTRC(std::list<unsigned> &permutation) const{

	std::priority_queue < Space > pq;

	int bin_w = 10, bin_h = 10;
	int NB = 1;

	pq.push(Space(bin_w, bin_h, NB));

	std::vector< std::pair<int, int> > boxes;
	
	std::vector<int> bin_capacity;
	bin_capacity.push_back(bin_w * bin_h);

	boxes.push_back(std::make_pair(2,2));
	boxes.push_back(std::make_pair(2,2));
	boxes.push_back(std::make_pair(2,2));
	boxes.push_back(std::make_pair(2,2));
	boxes.push_back(std::make_pair(2,2));
	boxes.push_back(std::make_pair(2,2));
	boxes.push_back(std::make_pair(2,2));
	boxes.push_back(std::make_pair(2,2));
	boxes.push_back(std::make_pair(2,2));
	boxes.push_back(std::make_pair(2,2));

	for (std::list<unsigned>::iterator it=permutation.begin(); it != permutation.end(); ++it){
		Space ems = pq.top();
		pq.pop();

		int box_w = boxes[*it].first, 
			box_h = boxes[*it].second;

		if(ems.width >= box_w && ems.height >= box_h){
			pq.push(Space(ems.width - box_w, ems.height, ems.bin_number));
			pq.push(Space(ems.width, ems.height - box_h, ems.bin_number));

			bin_capacity[ems.bin_number - 1] -= box_w * box_h;

		} else {
			NB++;
			pq.push(Space(bin_w - box_w, bin_h, NB));
			pq.push(Space(bin_w, bin_h - box_h, NB));

			bin_capacity.push_back(bin_w * bin_h - box_w * box_h);
		}

		double least_load = std::numeric_limits<double>::max();
		for (int i = 0; i < NB; i++){
			if(bin_capacity[i] < least_load){
				least_load = bin_capacity[i];
			}
		}

		return NB + least_load / (bin_w * bin_h);
	}


	
	std::cout << std::endl;
	

	return 0;
}

// Runs in O(n \log n):
double BPDecoder::decode(const std::vector<double> &chromosome) const
{
	typedef std::pair<double, unsigned> ValueKeyPair;
	std::vector<ValueKeyPair> rank(chromosome.size());

	for (unsigned i = 0; i < chromosome.size(); ++i)
	{
		rank[i] = ValueKeyPair(chromosome[i], i);
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
	
	// return fitness
	return DFTRC(permutation);
}

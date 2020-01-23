#include "BPDecoder.h"

BPDecoder::BPDecoder() {}

BPDecoder::~BPDecoder() {}

void BPDecoder::eliminationProcess(
	std::list< Space > &emptySpaces
) const{

	for (std::list<Space>::iterator sp=emptySpaces.begin(); sp != emptySpaces.end(); ++sp){
		for (std::list<Space>::iterator v=emptySpaces.begin(); v != emptySpaces.end(); ++v){
			if(v == sp) continue;

			if((*v).x >= (*sp).x && (*v).y >= (*sp).y && (*v).X <= (*sp).X && (*v).Y <= (*sp).Y){
				v = emptySpaces.erase(v);
				--v;
			}

		}
	}
}

bool compare(const Space &s1, const Space &s2){
	return s1.size > s2.size;
}

std::list < Space > BPDecoder::differenceProcess(
	std::list < Space > &emptySpaces, Box box)
	const{

	std::list<Space> new_spaces;


	
	for (std::list<Space>::iterator sp=emptySpaces.begin(); sp != emptySpaces.end(); ++sp){

		Space space = Space((*sp).x, (*sp).y, box.x,   (*sp).Y, (*sp).bin_number);
		
		if(space.size && space.x < space.X && space.y < space.Y)
			new_spaces.push_back(space);
		
		space = Space((*sp).x, (*sp).y, (*sp).X, box.y,   (*sp).bin_number);
		if(space.size && space.x < space.X && space.y < space.Y)
			new_spaces.push_back(space);
		
		space = Space((*sp).x, box.Y,   (*sp).X, (*sp).Y, (*sp).bin_number);
		if(space.size && space.x < space.X && space.y < space.Y)
			new_spaces.push_back(space);
		
		space = Space(box.X,   (*sp).y, (*sp).X, (*sp).Y, (*sp).bin_number);
		if(space.size && space.x < space.X && space.y < space.Y)
			new_spaces.push_back(space);

	}
	
	eliminationProcess(new_spaces);
	new_spaces.sort(compare);

	/*for (std::list<Space>::iterator sp=new_spaces.begin(); sp != new_spaces.end(); ++sp){
		std::cout << (*sp).x << ", " << (*sp).y << " " << (*sp).X << ", " << (*sp).Y << "; ";
	}
	if(!new_spaces.empty())
		std::cout << std::endl;*/

	// new_spaces.sort();

	return new_spaces;
}

std::vector<unsigned> BPDecoder::DFTRC(std::list<unsigned> &permutation, std::list < Box > &packedBoxes, unsigned &NB) const{

	std::list < Space > emptySpaces;
	NB = 1;

	emptySpaces.push_back(Space(0, 0, bin_w, bin_h, NB));
	
	std::vector<unsigned> bin_capacity;
	bin_capacity.push_back(bin_w * bin_h);

	for (std::list<unsigned>::iterator it=permutation.begin(); it != permutation.end(); ++it){
		
		unsigned box_w = boxes[*it].first, 
				box_h = boxes[*it].second;

		unsigned distance = 0;
		Space spaceMax;
		
		for (std::list<Space>::iterator sp=emptySpaces.begin(); sp != emptySpaces.end(); ++sp){
			Space ems = *sp;

			if((ems.X - ems.x) >= box_w && (ems.Y - ems.y) >= box_h){

				unsigned d = std::pow(ems.X - box_w, 2) + std::pow(ems.Y - box_h, 2);
				if (d >= distance){
					distance = d;
					spaceMax = (*sp);
				}
			}
		}
		Box box(spaceMax.x,spaceMax.y,box_w,box_h);

		//std::cout << "Box: " << box.x << ", " << box.y << " " << box.X << ", " << box.Y << std::endl;
		
		bin_capacity[spaceMax.bin_number - 1] -= box_w * box_h;
		// std::cout << "bin capacity " << bin_capacity[ems.bin_number - 1] << std::endl;

		emptySpaces = differenceProcess(emptySpaces, box);
		packedBoxes.push_back(box);
		break;
		
	}
	/*for (std::list<unsigned>::iterator it=permutation.begin(); it != permutation.end(); ++it){
		std::cout << *it << " ";
	}
	std::cout << std::endl;*/
	
	return bin_capacity;
}

double BPDecoder::fitness(std::list<unsigned> &permutation) const{

	unsigned NB = 1;

	std::list < Box > packedBoxes;
	
	std::vector<unsigned> bin_capacity = DFTRC(permutation, packedBoxes, NB);
	
	double least_load = std::numeric_limits<double>::max();
	for (unsigned i = 0; i < NB; i++){
		if(bin_capacity[i] < least_load){
			least_load = bin_capacity[i];
		}
	}
	return NB + least_load / (bin_w * bin_h);
}

std::list < Box > BPDecoder::getPackedBoxes(std::list<unsigned> &permutation){

	unsigned NB = 1;

	std::list < Box > packedBoxes;
	DFTRC(permutation, packedBoxes, NB);

	return packedBoxes;
}

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
	return fitness(permutation);
}

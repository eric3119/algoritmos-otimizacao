#include "BPDecoder.h"
#include "draw_bin.h"
bool draw = false;

BPDecoder::BPDecoder() {}

BPDecoder::~BPDecoder() {}

list< Space > BPDecoder::eliminationProcess(
	list< Space > &new_spaces
) const{
	bool next = true;
	for (list<Space>::iterator n_sp=new_spaces.begin(); n_sp != new_spaces.end();){
		if((*n_sp).size == 0){
			// self elimination
			n_sp = new_spaces.erase(n_sp);
			next = false;
		}else{
			// cross checking
			bool next2 = true;
			for (list<Space>::iterator e_sp=new_spaces.begin(); e_sp != new_spaces.end();){
				if((*e_sp).bin_number == (*n_sp).bin_number && e_sp != n_sp){
					if((*e_sp).x >= (*n_sp).x && (*e_sp).y >= (*n_sp).y && (*e_sp).X <= (*n_sp).X && (*e_sp).Y <= (*n_sp).Y){
						e_sp = new_spaces.erase(e_sp);
						next2 = false;
					}
				}
				if(next2){
					++e_sp;
				}else{
					next2 = true;
				}
			}
		}
		if(next){
			++n_sp;
		}else{
			next = true;
		}
	}
	return new_spaces;
}

bool compare(const Space &s1, const Space &s2){
	return s1.size > s2.size;
}

bool box_intersect_space(Space sp, Box box){
	return	(box.x < sp.X && box.Y > sp.y )||//&& box.x > sp.x && box.Y < sp.Y) ||
			(box.X > sp.x && box.Y > sp.y )||//&& box.X < sp.X && box.Y < sp.Y) ||
			(box.x < sp.X && box.y < sp.Y && box.x > sp.x && box.y > sp.y) ||
			(box.X > sp.x && box.y < sp.Y && box.X < sp.X && box.y > sp.y);
}

list < Space > BPDecoder::differenceProcess(
	list < Space > &empty_spaces, Box box)
	const{

	list<Space> new_spaces;
	
	for (list<Space>::iterator sp=empty_spaces.begin(); sp != empty_spaces.end(); ++sp){

		if(box.bin_number == (*sp).bin_number && box_intersect_space(*sp, box)){

			if((*sp).x < box.x && box.x > (*sp).X){
				// adiciona o lado esquerdo do novo espaço
				new_spaces.push_back(Space((*sp).x, (*sp).y, box.x,   (*sp).Y, (*sp).bin_number));
			}
			if((*sp).X > box.X && box.X > (*sp).x){
				// adiciona o lado direito do novo espaço
				new_spaces.push_back(Space(box.X,   (*sp).y, (*sp).X, (*sp).Y, (*sp).bin_number));
			}
			if((*sp).y < box.y && box.y < (*sp).Y){
				// adiciona o fundo do novo espaço	
				new_spaces.push_back(Space((*sp).x, (*sp).y, (*sp).X, box.y,   (*sp).bin_number));
			}
			if((*sp).Y > box.Y && box.Y > (*sp).y){
				// adiciona o topo do novo espaço	
				new_spaces.push_back(Space((*sp).x, box.Y,   (*sp).X, (*sp).Y, (*sp).bin_number));
			}
		}else{
			new_spaces.push_back(*sp);
		}
		
	}
	
	new_spaces = eliminationProcess(new_spaces);

	return new_spaces;
}

vector<unsigned> BPDecoder::DFTRC(list<unsigned> &permutation, list < Box > &packedBoxes, unsigned &number_of_bins) const{

	list < Space > empty_spaces;
	vector<unsigned> bin_capacity;

	number_of_bins = 1;

	empty_spaces.push_back(Space(0, 0, bin_w, bin_h, number_of_bins));
	
	bin_capacity.push_back(bin_w * bin_h);

	for (list<unsigned>::iterator it=permutation.begin(); it != permutation.end(); ++it){
		
		unsigned box_w = boxes[*it].first, 
				box_h = boxes[*it].second;

		unsigned distance = 0;
		Space maximalSpace;
		
		for (list<Space>::iterator sp=empty_spaces.begin(); sp != empty_spaces.end(); ++sp){
			Space ems = *sp;

			if((ems.X - ems.x) >= box_w && (ems.Y - ems.y) >= box_h){
				if(draw)cout << ems.X << " " << ems.x << " " << box_w << " " << ems.Y << " " << ems.y << " " << box_h << endl;

				unsigned d = pow(ems.X - box_w, 2) + pow(ems.Y - box_h, 2);
				if (d >= distance){
					distance = d;
					maximalSpace = (*sp);
				}
			}
		}
		
		if (maximalSpace.size == 0) {
			++number_of_bins;
			empty_spaces.push_back(Space(0, 0, bin_w, bin_h, number_of_bins));
			bin_capacity.push_back(bin_w * bin_h);
			maximalSpace = empty_spaces.back();
		}

		Box box(maximalSpace.x,maximalSpace.y,box_w,box_h, maximalSpace.bin_number);

		bin_capacity[maximalSpace.bin_number - 1] -= box_w * box_h;

		empty_spaces = differenceProcess(empty_spaces, box);
		packedBoxes.push_back(box);
		if(draw){
			for (unsigned i = 1; i <= number_of_bins; ++i){
				clear_display();
				draw_boxes(packedBoxes, i);
				draw_spaces(empty_spaces, i);
			}
		}
	}

	
	return bin_capacity;
}

double BPDecoder::fitness(list<unsigned> &permutation) const{

	unsigned number_of_bins = 1;

	list < Box > packedBoxes;
	
	vector<unsigned> bin_capacity = DFTRC(permutation, packedBoxes, number_of_bins);
	
	double least_load = numeric_limits<double>::max();
	for (unsigned i = 0; i < number_of_bins; i++){
		if(bin_capacity[i] < least_load){
			least_load = bin_capacity[i];
		}
	}
	return number_of_bins + (least_load / (this->bin_w * this->bin_h));
}

list < Box > BPDecoder::getPackedBoxes(list<unsigned> &permutation){

	unsigned number_of_bins = 1;
	draw= true;

	list < Box > packedBoxes;
	DFTRC(permutation, packedBoxes, number_of_bins);

	return packedBoxes;
}

double BPDecoder::decode(const vector<double> &chromosome) const
{
	typedef pair<double, unsigned> ValueKeyPair;
	vector<ValueKeyPair> rank(chromosome.size());

	for (unsigned i = 0; i < chromosome.size(); ++i)
	{
		rank[i] = ValueKeyPair(chromosome[i], i);
	}

	// Here we sort 'permutation', which will then produce a permutation of [n]
	// stored in ValueKeyPair::second:
	sort(rank.begin(), rank.end());

	// permutation[i].second is in {0, ..., n - 1}; a permutation can be obtained as follows
	list<unsigned> permutation;
	for (vector<ValueKeyPair>::const_iterator i = rank.begin(); i != rank.end(); ++i)
	{
		permutation.push_back(i->second);
	}
	
	// return fitness
	return fitness(permutation);
}

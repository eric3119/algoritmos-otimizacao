#include "BPDecoder.h"
#include "draw_bin.h"
bool draw = false;
unsigned box_min_w, box_min_h;

BPDecoder::BPDecoder() {}

BPDecoder::~BPDecoder() {}

void BPDecoder::eliminationProcess(
	list< Space > &new_spaces
) const{

	auto it = new_spaces.begin();
	while (it != new_spaces.end()){
		if(it->size == 0 
			|| box_min_w > it->X - it->x
			|| box_min_h > it->Y - it->y){
			// self elimination
			it = new_spaces.erase(it);
		} else {
			auto cross = it;
			++cross;
			while (cross != new_spaces.end()){
				if(cross->x >= it->x && 
					cross->y >= it->y && 
					cross->X <= it->X && 
					cross->Y <= it->Y){
					// cross checking
					cross = new_spaces.erase(cross);
				}else{
					++cross;
				}
			}
	        
	        ++it;
	    }
	}		
}

bool compare_space_size(const Space &s1, const Space &s2){
	return s1.size < s2.size;
}

bool sort_by_bin_number(const Space &s1, const Space &s2){
	return s1.bin_number < s2.bin_number;
}

bool box_intersect_space(Space sp, Box box){
	if (box.x > sp.X || box.X < sp.x ) return false;
	if (box.y > sp.Y || box.Y < sp.y ) return false;

	return true;
}

void BPDecoder::differenceProcess(list < Space > &empty_spaces, Box box) const{

	list<Space> new_spaces;
	
	auto sp = empty_spaces.begin();
	while (sp != empty_spaces.end()){

		if(box_intersect_space(*sp, box)){

			if(sp->x < box.x){
				// adiciona o lado esquerdo do novo espaço
				new_spaces.push_back(Space(sp->x, sp->y, box.x,   sp->Y, sp->bin_number));
			}
			if(sp->X > box.X){
				// adiciona o lado direito do novo espaço
				new_spaces.push_back(Space(box.X,   sp->y, sp->X, sp->Y, sp->bin_number));
			}
			if(sp->y < box.y){
				// adiciona o fundo do novo espaço	
				new_spaces.push_back(Space(sp->x, sp->y, sp->X, box.y,   sp->bin_number));
			}
			if(sp->Y > box.Y){
				// adiciona o topo do novo espaço	
				new_spaces.push_back(Space(sp->x, box.Y,   sp->X, sp->Y, sp->bin_number));
			}
			sp = empty_spaces.erase(sp);
		}else{
			++sp;
		}
	}
	eliminationProcess(new_spaces);

	empty_spaces.splice(empty_spaces.end(), new_spaces);
}

vector<Space> BPDecoder::DFTRC (list<Space> &bin_spaces_list, pair<unsigned, unsigned> box_to_pack) const{

	vector<Space> result;

	unsigned max_distance = 0,
			box_w = box_to_pack.first,
			box_h = box_to_pack.second;

	for (auto &es : bin_spaces_list){
		unsigned space_w = es.X - es.x;
		unsigned space_h = es.Y - es.y;

		if(space_w >= box_to_pack.first && space_h >= box_to_pack.second){
			unsigned d2 = pow(this->bin_w - es.x - box_w, 2) 
						+ pow(this->bin_h - es.y - box_h, 2);
			if (d2 == max_distance){
				result.push_back(es);
			}else if(d2 > max_distance){
				result.clear();
				result.push_back(es);
				max_distance = d2;
			}
		}
	}
	return result;
}

// BPS -> box packing sequence
vector<unsigned> BPDecoder::placement(list<unsigned> &BPS, vector<unsigned> &empate) const{
	
	list<Box> packedBoxes;
	vector< unsigned > BinLoad;
	vector< list<Space> > Bins;
	unsigned NB = 0;

	for (auto &gene : BPS){
		vector<Space> bestEMSv;
		pair<unsigned, unsigned> box_to_pack = this->boxes[gene];

		for (auto &bin_spaces_list : Bins){
			
			if(bin_spaces_list.size() == 0)
				continue;
			
			bestEMSv = DFTRC(bin_spaces_list, box_to_pack);
			
			if(bestEMSv.size() > 0)
				break;
		}

		Space *spaceToPack = NULL;

		if(bestEMSv.size() == 0){
			++NB;
			
			list<Space> es_list;
			es_list.push_back(Space(0, 0, this->bin_w, this->bin_h, NB));
			
			BinLoad.push_back(0);

			Bins.push_back(es_list);
			spaceToPack = &Bins[Bins.size()-1].back();
		}else{
			unsigned random_es = empate[gene] % bestEMSv.size();
			spaceToPack = &bestEMSv[random_es];
		}

		Box box(spaceToPack->x,spaceToPack->y, box_to_pack.first, box_to_pack.second, spaceToPack->bin_number);

		BinLoad[spaceToPack->bin_number - 1] += box_to_pack.first * box_to_pack.second;

		differenceProcess(Bins[spaceToPack->bin_number - 1], box);

		packedBoxes.push_back(box);
	}

	if (draw) draw_solution(packedBoxes, Bins);

	return BinLoad;
}

double BPDecoder::fitness(list<unsigned> &permutation, vector<unsigned> &empate) const{

	vector<unsigned> bin_capacity = placement(permutation, empate);

	double number_of_bins = bin_capacity.size();
	
	double least_load = 0;
	vector<unsigned>::iterator it = min_element(bin_capacity.begin(), bin_capacity.end());
	least_load = *it;
	
	return number_of_bins + (least_load / (this->bin_w * this->bin_h));
}

void BPDecoder::setDraw(bool value) {
	draw = value;
}


list<unsigned> BPDecoder::make_permutation(const vector<double>& chromosome) const {
	typedef pair<double, unsigned> ValueKeyPair;
	vector<ValueKeyPair> rank(chromosome.size() / 2);

	for (unsigned i = 0; i < chromosome.size() / 2; ++i)
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

	return permutation;
}

vector<unsigned> BPDecoder::make_empate(const vector<double>& chromosome) const {
	vector<unsigned> empate;
	for (unsigned i = chromosome.size() / 2; i < chromosome.size(); ++i) {
		if (chromosome[i] < 1 / 2)
			empate.push_back(0);
		else
			empate.push_back(1);
	}

	return empate;
}

double BPDecoder::decode(const vector<double> &chromosome) const{

	list<unsigned> permutation = make_permutation(chromosome);
	vector<unsigned> empate = make_empate(chromosome);

	if (draw && !start_allegro(this->bin_w, this->bin_h)) {
		std::cout << "ERROR: start allegro\n";
	}
	return fitness(permutation, empate);
}

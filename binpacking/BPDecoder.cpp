#include "BPDecoder.h"
#include "draw_bin.h"
bool draw = false;
unsigned box_min_w, box_min_h;

BPDecoder::BPDecoder() {}

BPDecoder::~BPDecoder() {}

void BPDecoder::eliminationProcess(
	list< Space > &new_spaces
) const{
	bool next = true;

	list<Space>::iterator n_sp=new_spaces.begin();
	while (n_sp != new_spaces.end()){
		if((*n_sp).size == 0 
			|| box_min_w > (*n_sp).X - (*n_sp).x
			|| box_min_h > (*n_sp).Y - (*n_sp).y){
			// self elimination
			n_sp = new_spaces.erase(n_sp);
			next = false;
		}else{
			// cross checking
			bool next2 = true;
			for (list<Space>::iterator e_sp=new_spaces.begin(); e_sp != new_spaces.end();){
				if(e_sp != n_sp){
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
	
	list<Space>::iterator sp=empty_spaces.begin();
	while (sp != empty_spaces.end()){

		if(box_intersect_space(*sp, box)){

			if((*sp).x < box.x){
				// adiciona o lado esquerdo do novo espaço
				new_spaces.push_back(Space((*sp).x, (*sp).y, box.x,   (*sp).Y, (*sp).bin_number));
			}
			if((*sp).X > box.X){
				// adiciona o lado direito do novo espaço
				new_spaces.push_back(Space(box.X,   (*sp).y, (*sp).X, (*sp).Y, (*sp).bin_number));
			}
			if((*sp).y < box.y){
				// adiciona o fundo do novo espaço	
				new_spaces.push_back(Space((*sp).x, (*sp).y, (*sp).X, box.y,   (*sp).bin_number));
			}
			if((*sp).Y > box.Y){
				// adiciona o topo do novo espaço	
				new_spaces.push_back(Space((*sp).x, box.Y,   (*sp).X, (*sp).Y, (*sp).bin_number));
			}
			sp = empty_spaces.erase(sp);
		}else{
			++sp;
		}
		
	}
	eliminationProcess(new_spaces);

	empty_spaces.splice(empty_spaces.end(), new_spaces);
}

vector<Space> getBestEMSv (list<Space> bin_spaces_list, pair<unsigned, unsigned> box_to_pack){

	vector<Space> result;
	for (auto &es : bin_spaces_list){
		unsigned space_w = es.X - es.x;
		unsigned space_h = es.Y - es.y;

		// double max_distance = 0;

		if(space_w >= box_to_pack.first && space_h >= box_to_pack.second){

			//if(result.size() == 0 || result[0].size == es.size){
				result.push_back(es);
			// }else if(result[0].size > es.size){
			// 	result.clear();
			// 	result.push_back(es);
				// unsigned box_X = box_w + es.x, 
				// 		box_Y = box_h + es.y;

				// double d2 = sqrt(pow(this->bin_w - box_X, 2) + pow(this->bin_h - box_Y, 2));

				// if (d2 == max_distance){
				// 	max_distance = d2;
				// 	bestEMSv.push_backes;
				// }else if(d2 > max_distance){
				// 	bestEMSv.clear();

				// 	max_distance = d2;
				// }
			// }
		}
	}
	return result;
}

vector<unsigned> BPDecoder::DFTRC(list<unsigned> &permutation, list < Box > &packedBoxes) const{
	
	unsigned number_of_bins = 0;
	vector< list<Space> > empty_spaces_vlS;
	vector< unsigned > bin_load_vu;

	box_min_w = numeric_limits<unsigned>::max();
	box_min_h = numeric_limits<unsigned>::max();

	for (auto &gene : permutation){
		vector< vector<Space> > bestEMSv(number_of_bins);
		pair<unsigned, unsigned> box_to_pack = this->boxes[gene];
		
		// unsigned box_w = this->boxes[gene].first, 
		// 		box_h = this->boxes[gene].second;

		for (unsigned i = packedBoxes.size(); i < permutation.size(); ++i){
			
			if(box_min_w > boxes[i].first)
				box_min_w = boxes[i].first;

			if(box_min_h > boxes[i].second)
				box_min_h = boxes[i].second;
		}

		for (auto &bin_spaces_list : empty_spaces_vlS){
			
			unsigned bin_number = (*bin_spaces_list.begin()).bin_number;
			
			bestEMSv[bin_number -1] = getBestEMSv(bin_spaces_list, box_to_pack);
		}

		Space *spaceToPack = NULL;

		vector<pair<unsigned, unsigned>> rank(bin_load_vu.size());

		for (unsigned i = 0; i < bin_load_vu.size(); ++i)
		{
			rank[i] = pair<unsigned, unsigned> (bin_load_vu[i], i);
		}
		sort(rank.begin(), rank.end(), greater< pair<unsigned, unsigned> >());
		
		for(auto &bin_index : rank){
			unsigned index = bin_index.second;
			if (bestEMSv[index].size() > 0) {
				unsigned random_index = rand() % bestEMSv[index].size();
				if(!spaceToPack || (*spaceToPack).size < bestEMSv[index][0].size){
					spaceToPack = &bestEMSv[index][random_index];
					break;
				}
			}
		}

		if(!spaceToPack){
			++number_of_bins;
			
			spaceToPack = new Space(0, 0, this->bin_w, this->bin_h, number_of_bins);
			bin_load_vu.push_back(0);

			list<Space> es_list;
			es_list.push_back(*spaceToPack);
			empty_spaces_vlS.push_back(es_list);
		}
		Box box((*spaceToPack).x,(*spaceToPack).y, box_to_pack.first, box_to_pack.second, (*spaceToPack).bin_number);

		bin_load_vu[(*spaceToPack).bin_number - 1] += box_to_pack.first * box_to_pack.second;

		differenceProcess(empty_spaces_vlS[(*spaceToPack).bin_number - 1], box);

		packedBoxes.push_back(box);
	}

	if(draw){
		cout << "Number of bins " << number_of_bins << endl;
		for(auto &es : empty_spaces_vlS){
			draw_bin(packedBoxes, es, (*es.begin()).bin_number);
		}
		finalize_allegro();
	}

	return bin_load_vu;
}

double BPDecoder::fitness(list<unsigned> &permutation) const{

	list < Box > packedBoxes;
	
	vector<unsigned> bin_capacity = DFTRC(permutation, packedBoxes);

	unsigned number_of_bins = bin_capacity.size();
	
	double least_load = 0;
	if(number_of_bins > 1){
		vector<unsigned>::iterator it = min_element(bin_capacity.begin(), bin_capacity.end());
		least_load = *it;
	}

	return number_of_bins + (least_load / (this->bin_w * this->bin_h));
}

list < Box > BPDecoder::getPackedBoxes(list<unsigned> &permutation){

	list < Box > packedBoxes;
	DFTRC(permutation, packedBoxes);

	return packedBoxes;
}

void BPDecoder::setDraw(bool value){
	draw = value;
}

double BPDecoder::decode(const vector<double> &chromosome) const{
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

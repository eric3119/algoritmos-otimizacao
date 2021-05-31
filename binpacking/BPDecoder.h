#ifndef BPDECODER_H
#define BPDECODER_H

#include <list>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>
#include <iostream>
#include <queue>
#include <limits>
#include <math.h>

using namespace std;

typedef struct ProblemInstance {
    unsigned problem_class = 0;
    unsigned n_items = 0;
    unsigned relative = 0;
    unsigned absolute = 0;
    unsigned hbin = 0;
    unsigned wbin = 0;
    vector< pair<unsigned, unsigned> > boxes;
}ProblemInstance;

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
    double bin_w, bin_h;
	
    vector< pair<unsigned, unsigned> > boxes;

	BPDecoder();  // Constructor
	~BPDecoder(); // Destructor

	// Decode a chromosome, returning its fitness as a double-precision floating point:
	double decode(const vector<double> &chromosome) const;
    list<unsigned> make_permutation(const vector<double>& chromosome) const;
    vector<unsigned> make_empate(const vector<double>& chromosome) const;
	
    vector<unsigned> placement(list<unsigned> &permutation, vector<unsigned> &empate) const;    
    vector<Space> DFTRC(list<Space> &bin_spaces_list, pair<unsigned, unsigned> box_to_pack) const;
    void differenceProcess(list < Space > &empty_spaces, Box box) const;
    void eliminationProcess(list < Space > &new_spaces) const;
    
    double fitness(list<unsigned> &permutation, vector<unsigned> &empate) const;
    
    list < Box > getPackedBoxes(list<unsigned> &permutation, vector<unsigned> &empate);
    void setDraw(bool value);
private:
};

#endif

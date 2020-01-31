#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "BPDecoder.h"
#include "draw_bin.h"

using namespace std;

std::list<unsigned> getPackingOrder(const std::vector<double> &chromosome){
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

    return permutation;
}

int main()
{
    unsigned n;               // size of chromosomes
    const unsigned p = 100;   // size of population
    const double pe = 0.10;   // fraction of population to be the elite-set
    const double pm = 0.10;   // fraction of population to be replaced by mutants
    const double rhoe = 0.70; // probability that offspring inherit an allele from elite parent
    const unsigned K = 3;     // number of independent populations
    const unsigned MAXT = 1;  // number of threads for parallel decoding

    BPDecoder decoder; // initialize the decoder

    unsigned box_w, box_h;
    
    string str;

    getline(cin, str); // PROBLEM CLASS

    cin >> n;
    getline(cin, str);
    
    getline(cin, str); //RELATIVE AND ABSOLUTE N. OF INSTANCE

    cin >> decoder.bin_w >> decoder.bin_h;
    getline(cin, str);


    for (unsigned i = 0; i < n; ++i){
        cin >> box_h >> box_w;
        getline(cin, str);

        decoder.boxes.push_back(std::make_pair(box_w, box_h));
    }
//2 5 4 0 6 7 8 9 3 1
    /*for (unsigned i = 0; i < n; ++i){
        std::cout << decoder.boxes[i].first << " " << decoder.boxes[i].second << std::endl;
    }*/
    const long unsigned rngSeed = 0; // seed to the random number generator
    MTRand rng(rngSeed);             // initialize the random number generator

    // initialize the BRKGA-based heuristic
    BRKGA<BPDecoder, MTRand> algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);

    unsigned generation = 0;        // current generation
    const unsigned X_INTVL = 100;   // exchange best individuals at every 100 generations
    const unsigned X_NUMBER = 2;    // exchange top 2 best
    const unsigned MAX_GENS = 1000; // run for 1000 gens
    std::cout << "Running for " << MAX_GENS << " generations..." << std::endl;
    do
    {
        algorithm.evolve(); // evolve the population for one generation

        if ((++generation) % X_INTVL == 0)
        {
            algorithm.exchangeElite(X_NUMBER); // exchange top individuals
        }
    } while (generation < MAX_GENS);

    // print the fitness of the top 10 individuals of each population:
    std::cout << "Fitness of the top 10 individuals of each population:" << std::endl;
    const unsigned bound = std::min(p, unsigned(10)); // makes sure we have 10 individuals
    for (unsigned i = 0; i < K; ++i)
    {
        std::cout << "Population #" << i << ":" << std::endl;
        for (unsigned j = 0; j < bound; ++j)
        {
            std::cout << "\t" << j << ") "
                      << algorithm.getPopulation(i).getFitness(j) << std::endl;
        }
    }

    std::cout << "Best solution found has objective value = "
              << algorithm.getBestFitness() << std::endl;
    
    std::cout << "Best chromosome ";

    std::list<unsigned> packing_sequence = getPackingOrder(algorithm.getBestChromosome());

    for (std::list<unsigned>::iterator it=packing_sequence.begin(); it != packing_sequence.end(); ++it){
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    if (!start_allegro(decoder.bin_w, decoder.bin_h)){
        std::cout << "ERROR: start allegro\n";
    }else{
        std::list < Box > packedBoxes = decoder.getPackedBoxes(packing_sequence);
        //draw_bin(packedBoxes, decoder.bin_w, decoder.bin_h);
    }

    return 0;
}
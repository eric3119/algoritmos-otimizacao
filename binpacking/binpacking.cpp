#include <iostream>
#include <vector>
#include <algorithm>
#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "BPDecoder.h"

using namespace std;

typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<ii> vii;

int main()
{
    cout << "Welcome to the BRKGA API sample driver.\nFinding a (heuristic) minimizer for "
              << " f(x) = sum_i (x_i * i) where x \\in [0,1)^n." << endl;

    const unsigned n = 10;    // size of chromosomes
    const unsigned p = 100;   // size of population
    const double pe = 0.10;   // fraction of population to be the elite-set
    const double pm = 0.10;   // fraction of population to be replaced by mutants
    const double rhoe = 0.70; // probability that offspring inherit an allele from elite parent
    const unsigned K = 3;     // number of independent populations
    const unsigned MAXT = 1;  // number of threads for parallel decoding

    BPDecoder decoder; // initialize the decoder

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

    return 0;
}
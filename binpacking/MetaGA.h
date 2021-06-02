#pragma once

#include <omp.h>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include "brkgaAPI/Population.h"

#define PE 0
#define PM 1

template< class BRKGA, class RNG >
class MetaGA {
public:

	MetaGA(BRKGA& brkga, RNG& rng);
	
	~MetaGA();
	/**
	 * Resets all populations with brand new keys
	 */
	void reset();

	/**
	 * Evolve the current populations following the guidelines of BRKGAs
	 * @param generations number of generations (must be even and nonzero)
	 * @param J interval to exchange elite chromosomes (must be even; 0 ==> no synchronization)
	 * @param M number of elite chromosomes to select from each population in order to exchange
	 */
	void evolve(unsigned generations = 1);

	/**
	 * Returns the current population
	 */
	const Population& getPopulation() const;

	/**
	 * Returns the chromosome with best fitness so far among all populations
	 */
	const std::vector< double >& getBestChromosome() const;

	std::vector<double> fitness_history;

	/**
	 * Returns the best fitness found so far among all populations
	 */
	double getBestFitness() const;

	// Return copies to the internal parameters:
	unsigned getN() const;
	unsigned getP() const;
	unsigned getPe() const;
	unsigned getPm() const;
	void setPe(double val);
	void setPm(double val);
	unsigned getPo() const;
	double getRhoe() const;
	unsigned getK() const;
	unsigned getMAX_THREADS() const;

private:
	// I don't see any reason to pimpl the internal methods and data, so here they are:
	// Hyperparameters:
	const unsigned n = 2;	// genes in the chromosome = 2 -> pair(pe, pm)
	const unsigned p = 10;	// number of elements in the population = 10
	const unsigned pe = 2;	// number of elite items in the population
	const unsigned pm = 2;	// number of mutants introduced at each generation into the population
	const double rhoe = 0.7;	// probability that an offspring inherits the allele of its elite parent

	// Templates:
	RNG& refRNG;			// reference to the random number generator
	BRKGA& refBRKGA;		// reference to the Genetic Algorithm
	// Data:
	Population* previous;	// previous populations
	Population* current;	// current populations

	// Local operations:
	void initialize();		// initialize current population with random keys
	void evolution(Population& curr, Population& next);
	//bool isRepeated(const std::vector< double >& chrA, const std::vector< double >& chrB) const;
};

template<class BRKGA, class RNG>
inline MetaGA<BRKGA, RNG>::MetaGA(BRKGA& brkga, RNG& rng) : 
	refRNG(rng), refBRKGA(brkga) {

	// Allocate:
	current = new Population(n, p);
	
	initialize();
	// Then just copy to previous:
	previous = new Population(*current);
}

template<class BRKGA, class RNG>
inline MetaGA<BRKGA, RNG>::~MetaGA()
{
	delete current;
	delete previous;
}

template< class BRKGA, class RNG >
const Population& MetaGA< BRKGA, RNG >::getPopulation() const {
	return (*current);
}

template< class BRKGA, class RNG >
double MetaGA< BRKGA, RNG >::getBestFitness() const {
	return current->fitness[0].first;
}

template< class BRKGA, class RNG >
const std::vector< double >& MetaGA< BRKGA, RNG >::getBestChromosome() const {
	return current->getChromosome(0);	// The top one :-)
}

template< class BRKGA, class RNG >
void MetaGA< BRKGA, RNG >::reset() {
	initialize();
}


template< class BRKGA, class RNG >
void MetaGA< BRKGA, RNG >::evolve(unsigned generations) {
	evolution(*current, *previous);	// First evolve the population (curr, next)
	std::swap(current, previous);	// Update (prev = curr; curr = prev == next)
}

template< class BRKGA, class RNG >
inline void MetaGA< BRKGA, RNG >::initialize() {
	for (unsigned j = 0; j < p; ++j) {
		for (unsigned k = 0; k < n; ++k) { (*current)(j, k) = 0.5; }
	}

	for (int j = 0; j < int(p); ++j) {
		current->setFitness(j, std::numeric_limits<double>::max());
	}

	// Sort:
	current->sortFitness();
}

template< class BRKGA, class RNG >
inline void MetaGA< BRKGA, RNG >::evolution(Population& curr, Population& next) {
	// We now will set every chromosome of 'current', iterating with 'i':
	unsigned i = 0;	// Iterate chromosome by chromosome
	unsigned j = 0;	// Iterate allele by allele

	// 2. The 'pe' best chromosomes are maintained, so we just copy these into 'current':
	while (i < pe) {
		for (j = 0; j < n; ++j) { next(i, j) = curr(curr.fitness[i].second, j); }

		next.fitness[i].first = curr.fitness[i].first;
		next.fitness[i].second = i;
		++i;
	}

	// 3. We'll mate 'p - pe - pm' pairs; initially, i = pe, so we need to iterate until i < p - pm:
	while (i < p - pm) {
		// Select an elite parent:
		const unsigned eliteParent = (refRNG.randInt(pe - 1));

		// Select a non-elite parent:
		const unsigned noneliteParent = pe + (refRNG.randInt(p - pe - 1));

		// Mate:
		for (j = 0; j < n; ++j) {
			const unsigned& sourceParent = ((refRNG.rand() < rhoe) ? eliteParent : noneliteParent);

			next(i, j) = curr(curr.fitness[sourceParent].second, j);
		}

		++i;
	}

	// We'll introduce 'pm' mutants:
	while (i < p) {
		for (j = 0; j < n; ++j) { next(i, j) = refRNG.rand(); }
		++i;
	}

	// Time to compute fitness, in parallel:
#ifdef _OPENMP
#pragma omp parallel for num_threads(MAX_THREADS)
#endif
	fitness_history.clear();
	for (int i = int(pe); i < int(p); ++i) {
		vector<double> crm = next.population[i];

		refBRKGA.setPe(crm[PE]);
		refBRKGA.setPm(crm[PM]);

		if (refBRKGA.getPe() == 0) {
			refBRKGA.setPe(0.15);
		}
		if (refBRKGA.getPm() == 0) {
			refBRKGA.setPm(0.25);
		}

		for (int j = 0; j < 5; ++j) {
			refBRKGA.evolve();
			fitness_history.push_back(refBRKGA.getBestFitness());
		}
		
		next.setFitness(i, refBRKGA.getBestFitness());
	}

	// Now we must sort 'current' by fitness, since things might have changed:
	next.sortFitness();
}

template< class BRKGA, class RNG >
unsigned MetaGA< BRKGA, RNG >::getN() const { return n; }

template< class BRKGA, class RNG >
unsigned MetaGA< BRKGA, RNG >::getP() const { return p; }

template< class BRKGA, class RNG >
unsigned MetaGA< BRKGA, RNG >::getPe() const { return pe; }

template< class BRKGA, class RNG >
unsigned MetaGA< BRKGA, RNG >::getPm() const { return pm; }

template< class BRKGA, class RNG >
inline void MetaGA< BRKGA, RNG >::setPe(double val) { pe = unsigned(val * p); }

template< class BRKGA, class RNG >
inline void MetaGA< BRKGA, RNG >::setPm(double val) { pm = unsigned(val * p); }

template< class BRKGA, class RNG >
unsigned MetaGA< BRKGA, RNG >::getPo() const { return p - pe - pm; }

template< class BRKGA, class RNG >
double MetaGA< BRKGA, RNG >::getRhoe() const { return rhoe; }

template< class BRKGA, class RNG >
unsigned MetaGA< BRKGA, RNG >::getK() const { return K; }

template< class BRKGA, class RNG >
unsigned MetaGA< BRKGA, RNG >::getMAX_THREADS() const { return MAX_THREADS; }

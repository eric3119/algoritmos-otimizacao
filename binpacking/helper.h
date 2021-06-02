#pragma once
typedef struct AG_CONFIG {
	unsigned n;         // size of chromosomes
	unsigned p;         // size of population
	double pe;          // fraction of population to be the elite-set
	double pm;          // fraction of population to be replaced by mutants
	double rhoe;        // probability that offspring inherit an allele from elite parent
	unsigned K;         // number of independent populations
	unsigned MAXT;      // number of threads for parallel decoding
	unsigned MAX_GENS;  // run for MAX_GENS
	unsigned P_FACTOR;  // population multiplier
} AG_CONFIG;

typedef struct SETTINGS {
	bool draw_best;
	bool best_cromo;
	bool disable_counter;
	unsigned gotoabsolute;
	const char* path;
	const char* save_path;
	bool chart;
	unsigned n_times;
} SETTINGS;

typedef struct SOLUTION {
	double best_fitness;
	unsigned num_bins;
} SOLUTION;
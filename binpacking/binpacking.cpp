#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>
#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "BPDecoder.h"
#include "draw_bin.h"

using namespace std;

bool sortbyarea(const pair<unsigned,unsigned> &a, 
              const pair<unsigned,unsigned> &b) { 
    return (a.first * a.second > b.first * b.second);
} 
bool sortbywidth(const pair<unsigned,unsigned> &a, 
              const pair<unsigned,unsigned> &b) { 
    return (a.first > b.first);
} 
bool sortbyheight(const pair<unsigned,unsigned> &a, 
              const pair<unsigned,unsigned> &b) { 
    return (a.second > b.second);
} 

vector<int> extractIntegerWords(string str)
{
    stringstream ss;

    /* Storing the whole string into string stream */
    ss << str;

    /* Running loop till the end of the stream */
    string temp;
    vector<int> found;
    while (!ss.eof()) {

        /* extracting word by word from stream */
        ss >> temp;
        int val;
        /* Checking the given word is integer or not */
        if (stringstream(temp) >> val)
            found.push_back(val);

        /* To save from space at the end of string */
        temp = "";
    }

    return found;
}

int main()
{
    unsigned n;               // size of chromosomes
    unsigned p;               // size of population
    const double pe = 0.10;   // fraction of population to be the elite-set
    const double pm = 0.15;   // fraction of population to be replaced by mutants
    const double rhoe = 0.70; // probability that offspring inherit an allele from elite parent
    const unsigned K = 3;     // number of independent populations
    const unsigned MAXT = 1;  // number of threads for parallel decoding

    list<ProblemInstance> problemInstances;
    
    string line;
    //ifstream myfile("Class_01.2bp");
    ifstream myfile("teste.in");
    if (myfile.is_open()) {
        while (getline(myfile, line)) {

            ProblemInstance prob;

            vector<int> res = extractIntegerWords(line);
            prob.problem_class = res[0];

            getline(myfile, line);
            res = extractIntegerWords(line);
            prob.n_items = res[0];

            getline(myfile, line);
            res = extractIntegerWords(line);
            prob.relative = res[0], prob.absolute = res[1];

            getline(myfile, line);
            res = extractIntegerWords(line);
            prob.hbin = res[0], prob.wbin = res[1];

            for (int i = 0; i < prob.n_items; ++i) {
                getline(myfile, line);
                res = extractIntegerWords(line);
                prob.boxes.push_back(std::make_pair(res[1], res[0]));                
            }
            problemInstances.push_back(prob);
            getline(myfile, line);
        }
        myfile.close();
    } else {
        cout << "Unable to open file";
        return 0;
    }
    
    double class_mean = 0;
    double class_bins_mean = 0;
    double best_fitness = numeric_limits<double>::max();
        
    for(auto &prob : problemInstances){
        BPDecoder decoder; // initialize the decoder
        n = prob.n_items * 2;
        p = 20*n;

        decoder.bin_h = prob.hbin;
        decoder.bin_w = prob.wbin;
            
        cout << "\nClass " << prob.problem_class << " #Itens " << n/2 << " bin size " << decoder.bin_w << " x " << decoder.bin_h << endl;
        cout << "Instance " << prob.relative << " / Absolute " << prob.absolute << endl;

        decoder.boxes = prob.boxes;
        // sort(decoder.boxes.begin(), decoder.boxes.end(), sortbyarea);
        // sort(decoder.boxes.begin(), decoder.boxes.end(), sortbywidth);
        // sort(decoder.boxes.begin(), decoder.boxes.end(), sortbyheight);

        const long unsigned rngSeed = 0; // seed to the random number generator
        MTRand rng(rngSeed);             // initialize the random number generator

        // initialize the BRKGA-based heuristic
        BRKGA<BPDecoder, MTRand> algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);

        unsigned generation = 0;        // current generation
        const unsigned X_INTVL = 15;   // exchange best individuals at every 15 generations
        const unsigned X_NUMBER = 2;    // exchange top 2 best
        const unsigned MAX_GENS = 200; // run for 200 gens
        std::cout << "Running for " << MAX_GENS << " generations..." << std::endl;
        do
        {
            algorithm.evolve(); // evolve the population for one generation

            // cout << "It = " << generation << " " 
            //    << algorithm.getBestFitness() << endl;

            if ((++generation) % X_INTVL == 0)
            {
                algorithm.exchangeElite(X_NUMBER); // exchange top individuals
            }
        } while (generation < MAX_GENS);

        // print the fitness of the top 10 individuals of each population:
        // std::cout << "Fitness of the top 10 individuals of each population:" << std::endl;
        // const unsigned bound = std::min(p, unsigned(10)); // makes sure we have 10 individuals
        // for (unsigned i = 0; i < K; ++i)
        // {
        //     std::cout << "Population #" << i << ":" << std::endl;
        //     for (unsigned j = 0; j < bound; ++j)
        //     {
        //         std::cout << "\t" << j << ") "
        //                 << algorithm.getPopulation(i).getFitness(j) << std::endl;
        //     }
        // }

        double solution_best_fitness = algorithm.getBestFitness();
        std::cout << "Best solution found has objective value = "
                << solution_best_fitness << std::endl;

        class_mean += solution_best_fitness;
        class_bins_mean += (unsigned) solution_best_fitness;

        if(solution_best_fitness < best_fitness){
            best_fitness = solution_best_fitness;
        }
            
        decoder.setDraw(true);
        decoder.decode(algorithm.getBestChromosome());
        decoder.setDraw(false);
    }
    cout << "\nZ " << class_bins_mean / 10.0 << endl;
    cout << "Class mean " << class_mean / 10.0 << endl;
    cout << "Min fitness " << best_fitness << endl << endl;

    return 0;
}
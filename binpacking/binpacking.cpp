#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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

void drawBest(const vector<double>& chromosome, ProblemInstance& pb) {
    BPDecoder decoder;
    decoder.bin_h = pb.hbin;
    decoder.bin_w = pb.wbin;
    decoder.boxes = pb.boxes;
    decoder.setDraw(true);
    decoder.decode(chromosome);
    decoder.setDraw(false);
}

int main(int argc, char **argv) {
    unsigned n;               // size of chromosomes
    unsigned p;               // size of population
    const double pe = 0.10;   // fraction of population to be the elite-set
    const double pm = 0.15;   // fraction of population to be replaced by mutants
    const double rhoe = 0.70; // probability that offspring inherit an allele from elite parent
    const unsigned K = 3;     // number of independent populations
    const unsigned MAXT = 2;  // number of threads for parallel decoding

    list<ProblemInstance> problemInstances;
    
    string line;
    ifstream myfile;
    bool draw_best = false;
    bool finish_relative = false;
    bool best_cromo = false;

    unsigned gotoabsolute = 0;

    if (argc > 1) {
        myfile.open(argv[1]);
        if (argc > 2) {
            for (int i = 2; i < argc; ++i) {
                if (strcmp(argv[i], "--draw") == 0) {
                    draw_best = true;
                }
                if (strcmp(argv[i], "--lastrel") == 0) {
                    finish_relative = true;
                }
                if (strcmp(argv[i], "--bestcromo") == 0) {
                    best_cromo = true;
                }
                if (strcmp(argv[i], "--inst") == 0) {
                    gotoabsolute = atoi(argv[i+1]);
                    ++i;
                }
            }
        }
    } else {
        draw_best = true;
        myfile.open("teste.in");
    }
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
                prob.boxes.push_back(make_pair(res[1], res[0]));                
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
    double num_bins = 0;
    double best_fitness = numeric_limits<double>::max();

    if (gotoabsolute) {
        vector<double> cromo;
        double c;
        while(cin >> c)
            cromo.push_back(c);

        for (auto& problem : problemInstances) {
            if (problem.absolute == gotoabsolute) {
                drawBest(cromo, problem);
                break;
            }
        }

    }
    else {
        for (auto& problem : problemInstances) {
            BPDecoder decoder; // initialize the decoder
            n = problem.n_items * 2;
            p = 30 * n;
            decoder.bin_h = problem.hbin;
            decoder.bin_w = problem.wbin;
            decoder.boxes = problem.boxes;

            cout << "\nClass " << problem.problem_class << "\n# Itens " << problem.n_items << "\nbin size " << problem.wbin << " x " << problem.hbin << endl;
            cout << "Instance " << problem.relative << " / Absolute " << problem.absolute << endl;


            sort(decoder.boxes.begin(), decoder.boxes.end(), sortbyarea);
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
            cout << "Running for " << MAX_GENS << " generations..." << endl;
            do
            {
                algorithm.evolve(); // evolve the population for one generation

                // cout << "It = " << generation << " " 
                //    << algorithm.getBestFitness() << endl;

                if ((++generation) % X_INTVL == 0)
                {
                    algorithm.exchangeElite(X_NUMBER); // exchange top individuals
                }
                cout << "\r" << generation << "/" << MAX_GENS << "        ";
            } while (generation < MAX_GENS);

            // print the fitness of the top 10 individuals of each population:
            // cout << "Fitness of the top 10 individuals of each population:" << endl;
            // const unsigned bound = min(p, unsigned(10)); // makes sure we have 10 individuals
            // for (unsigned i = 0; i < K; ++i)
            // {
            //     cout << "Population #" << i << ":" << endl;
            //     for (unsigned j = 0; j < bound; ++j)
            //     {
            //         cout << "\t" << j << ") "
            //                 << algorithm.getPopulation(i).getFitness(j) << endl;
            //     }
            // }

            double solution_best_fitness = algorithm.getBestFitness();
            cout << "\rBest solution found has objective value = "
                << solution_best_fitness << endl;

            if (best_cromo) {
                cout << "Best Chromosome ";
                for (auto& i : algorithm.getBestChromosome()) {
                    cout << i << " ";
                }
                cout << endl;
            }

            list<unsigned> perm = decoder.make_permutation(algorithm.getBestChromosome());
            vector<unsigned> emp = decoder.make_empate(algorithm.getBestChromosome());
            vector<unsigned> bin_capacity = decoder.placement(perm, emp);

            cout << "Best packing sequence ";
            for (auto& i : perm) {
                cout << i << " ";
            }
            cout << endl;

            class_mean += solution_best_fitness;
            num_bins += bin_capacity.size();

            if (solution_best_fitness < best_fitness) {
                best_fitness = solution_best_fitness;
            }

            if (draw_best) {
                drawBest(algorithm.getBestChromosome(), problem);
            }

            if (finish_relative) {
                cout << "\n" << "# Bins = " << num_bins << endl;
                cout << "Best fitness = " << best_fitness << endl << endl;
            }

            if (problem.relative == 10) {
                cout << "\n" << "# Bins (mean) = " << num_bins / problemInstances.size() << endl;
                cout << "Class (mean) = " << class_mean / problemInstances.size() << endl;
                cout << "Best fitness = " << best_fitness << endl << endl;

                class_mean = 0;
                num_bins = 0;
                best_fitness = numeric_limits<double>::max();
            }
        }
    }

    return 0;
}
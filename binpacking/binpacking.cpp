#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <json/json.h>
#include "MetaGA.h"
#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "BPDecoder.h"
#include "draw_bin.h"

#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

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

void plot_chart(const vector<double>& x, const vector<double>& y, int generations) {
    plt::plot(x, y);
    plt::xlim(0, generations);
    // Add graph title
    plt::title("2D Bin Packing");
    // Enable legend.
    plt::legend();
    plt::show();
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

list<ProblemInstance> loadProblemInstances(const char *path) {
    list<ProblemInstance> problemInstances;
    string line;
    ifstream myfile;
    myfile.open(path);

    if (myfile.is_open()) {
        while (std::getline(myfile, line)) {

            ProblemInstance prob;

            vector<int> res = extractIntegerWords(line);
            prob.problem_class = res[0];

            std::getline(myfile, line);
            res = extractIntegerWords(line);
            prob.n_items = res[0];

            std::getline(myfile, line);
            res = extractIntegerWords(line);
            prob.relative = res[0], prob.absolute = res[1];

            std::getline(myfile, line);
            res = extractIntegerWords(line);
            prob.hbin = res[0], prob.wbin = res[1];

            for (int i = 0; i < prob.n_items; ++i) {
                std::getline(myfile, line);
                res = extractIntegerWords(line);
                prob.boxes.push_back(make_pair(res[1], res[0]));
            }
            problemInstances.push_back(prob);
            std::getline(myfile, line);
        }
        myfile.close();
    }
    else {
        cout << "Unable to open file";
    }

    return problemInstances;
}

int main_ag(int argc, char** argv) {
    unsigned n;     // size of chromosomes
    unsigned p;     // size of population
    double pe;      // fraction of population to be the elite-set
    double pm;      // fraction of population to be replaced by mutants
    double rhoe;    // probability that offspring inherit an allele from elite parent
    unsigned K;     // number of independent populations
    unsigned MAXT;  // number of threads for parallel decoding

    unsigned MAX_GENS;
    unsigned P_FACTOR;
    
    ifstream config("config.json", std::ifstream::binary);

    if (config.is_open()) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(config, root);

        pe = root["pe"].asDouble();
        pm = root["pm"].asDouble();
        rhoe = root[""].asDouble();
        K = root["K"].asUInt();
        MAXT = root["MAXT"].asUInt();
        MAX_GENS = root["MAX_GENS"].asUInt();
        P_FACTOR = root["P_FACTOR"].asUInt();
    }
    else {
        pe = 0.15;
        pm = 0.25;
        rhoe = 0.70;
        K = 4;
        MAXT = 4;
        MAX_GENS = 200;
        P_FACTOR = 5;
    }

    list<ProblemInstance> problemInstances;

    string line;
    ifstream myfile;
    bool draw_best = false;
    bool finish_relative = false;
    bool best_cromo = false;
    bool disable_counter = false;

    unsigned gotoabsolute = 0;

    if (argc > 1) {
        problemInstances = loadProblemInstances(argv[1]);
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
                    gotoabsolute = atoi(argv[i + 1]);
                    ++i;
                }
                if (strcmp(argv[i], "--out") == 0) {
                    disable_counter = true;
                }
            }
        }
    }
    else {
        draw_best = true;
        problemInstances = loadProblemInstances("teste.in");
    }
    

    double class_mean = 0;
    double num_bins = 0;
    double best_fitness = numeric_limits<double>::max();

    if (gotoabsolute) {
        vector<double> cromo;
        double c;
        while (cin >> c)
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
            n = problem.n_items * 2;
            p = P_FACTOR * n;

            BPDecoder decoder; // initialize the decoder
            decoder.bin_h = problem.hbin;
            decoder.bin_w = problem.wbin;
            decoder.boxes = problem.boxes;

            cout << "Class " << problem.problem_class << "\n# Itens " << problem.n_items << "\nbin size " << problem.wbin << " x " << problem.hbin << endl;
            cout << "Instance " << problem.relative << " / Absolute " << problem.absolute << endl;


            // sort(decoder.boxes.begin(), decoder.boxes.end(), sortbyarea);
            // sort(decoder.boxes.begin(), decoder.boxes.end(), sortbywidth);
            // sort(decoder.boxes.begin(), decoder.boxes.end(), sortbyheight);

            const long unsigned rngSeed = 1; // seed to the random number generator
            MTRand rng(rngSeed);             // initialize the random number generator
            // initialize the BRKGA-based heuristic
            BRKGA<BPDecoder, MTRand> algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);

            unsigned generation = 0;        // current generation
            const unsigned X_INTVL = 15;   // exchange best individuals at every 15 generations
            const unsigned X_NUMBER = 5;    // exchange top X_NUMBER best
            //const unsigned MAX_GENS = 250; // run for MAX_GENS

            vector<double> x(MAX_GENS), y(MAX_GENS);
            cout << "Running for " << MAX_GENS << " generations..." << endl;
            do
            {
                algorithm.evolve(); // evolve the population for one generation
                
                if (!disable_counter) {
                    cout << "\r" << generation << "/" << MAX_GENS << "\t" 
                        << algorithm.getBestFitness();
                }
                
                //Chart data
                x.at(generation) = generation;
                y.at(generation) = algorithm.getBestFitness();

                if ((++generation) % X_INTVL == 0)
                {
                    algorithm.exchangeElite(X_NUMBER); // exchange top individuals
                }
                //cout << "\r" << generation << "/" << MAX_GENS << "        ";
            } while (generation < MAX_GENS);
            
            if (!disable_counter) cout << "\r";
            double solution_best_fitness = algorithm.getBestFitness();
            cout << "Best solution found has objective value = "
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

            best_fitness = min(best_fitness, solution_best_fitness);

            if (draw_best) {
                drawBest(algorithm.getBestChromosome(), problem);
            }

            if (finish_relative) {
                cout << "\n" << "# Bins = " << num_bins << endl;
                cout << "Best fitness = " << best_fitness << endl << endl;
            }

            if (problem.relative == 10) {
                cout << "\n" << "# Bins (mean) = " << num_bins / 10 << endl;
                cout << "Class (mean) = " << class_mean / 10 << endl;
                cout << "Best fitness = " << best_fitness << endl << endl;

                class_mean = 0;
                num_bins = 0;
                best_fitness = numeric_limits<double>::max();
            }

            plot_chart(x, y, MAX_GENS);
        }
    }

    return 0;
}

int meta_ag(int argc, char** argv) {
    unsigned n;               // size of chromosomes
    unsigned p;               // size of population
    double pe;   // fraction of population to be the elite-set
    double pm;   // fraction of population to be replaced by mutants
    double rhoe; // probability that offspring inherit an allele from elite parent
    unsigned K;     // number of independent populations
    unsigned MAXT;  // number of threads for parallel decoding
    unsigned MAX_GENS;  // run for MAX_GENS
    unsigned P_FACTOR;

    bool draw_best = false;

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--draw") == 0) {
            draw_best = true;
        }
    }

    ifstream config("config.json", std::ifstream::binary);
    if (config.is_open()) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(config, root);

        pe = root["pe"].asDouble();
        pm = root["pm"].asDouble();
        rhoe = root[""].asDouble();
        K = root["K"].asUInt();
        MAXT = root["MAXT"].asUInt();
        MAX_GENS = root["MAX_GENS"].asUInt();
        P_FACTOR = root["P_FACTOR"].asUInt();
    }
    else {
        pe = 0.15;
        pm = 0.25;
        rhoe = 0.70;
        K = 4;
        MAXT = 4;
        MAX_GENS = 200;
        P_FACTOR = 5;
    }
    list<ProblemInstance> problemInstances;
    if (argc > 1)
        problemInstances = loadProblemInstances(argv[1]);
    else
        problemInstances = loadProblemInstances("teste.in");

    for (auto& problem : problemInstances) {

        n = problem.n_items * 2;
        p = P_FACTOR * n;

        BPDecoder decoder; // initialize the decoder
        decoder.bin_h = problem.hbin;
        decoder.bin_w = problem.wbin;
        decoder.boxes = problem.boxes;

        cout << "Class " << problem.problem_class << "\n# Itens " << problem.n_items << "\nbin size " << problem.wbin << " x " << problem.hbin << endl;
        cout << "Instance " << problem.relative << " / Absolute " << problem.absolute << endl;

        const long unsigned rngSeed = 1; // seed to the random number generator
        MTRand rng(rngSeed);             // initialize the random number generator

        BRKGA<BPDecoder, MTRand> brkga(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);

        // initialize the BRKGA-based heuristic
        MetaGA<BRKGA<BPDecoder, MTRand>, MTRand> algorithm(brkga, rng);

        unsigned generation = 0;        // current generation
        
        vector<double> x(MAX_GENS), y(MAX_GENS);
        cout << "Running for " << MAX_GENS << " generations..." << endl;
        do
        {
            algorithm.evolve(); // evolve the population for one generation

            cout << "\r" << generation+1 << "/" << MAX_GENS 
                << "\t" << algorithm.getBestFitness() 
                << " pm: " << brkga.getPm() 
                << " pe: " << brkga.getPe() << "            ";

            //Chart data
            x.at(generation) = generation;
            y.at(generation) = algorithm.getBestFitness();

            ++generation;
        } while (generation < MAX_GENS);

        cout << "Best solution found has objective value = "
            << brkga.getBestFitness() << endl;

        list<unsigned> perm = decoder.make_permutation(brkga.getBestChromosome());
        vector<unsigned> emp = decoder.make_empate(brkga.getBestChromosome());
        vector<unsigned> bin_capacity = decoder.placement(perm, emp);

        cout << "Best packing sequence ";
        for (auto& i : perm) {
            cout << i << " ";
        }
        cout << endl;

        cout << "Possible best parameters (pm, pe): ";
        for (auto& i : algorithm.getBestChromosome()) {
            cout << i << " ";
        }
        cout << endl;
        
        plot_chart(x, y, MAX_GENS);
        if (draw_best)
            drawBest(brkga.getBestChromosome(), problem);
    }

    return 0;
}

int main(int argc, char **argv) {
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--auto") == 0) {
            return meta_ag(argc, argv);
        }
    }

    return main_ag(argc, argv);
}
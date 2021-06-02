#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <json/json.h>
#include "helper.h"
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

void plot_results(const vector<double>& x, const vector<double>& y, int generations, SETTINGS &sett) {
    if (sett.save_path || sett.chart) {
        plt::plot(x, y);
        plt::xlim(0, generations);
        // Add graph title
        plt::title("2D Bin Packing");
        // Enable legend.
        plt::legend();
        if(sett.save_path)
            plt::save(sett.save_path);
        if(sett.chart)
            plt::show();
    }
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

SOLUTION run_ag(ProblemInstance &problem, AG_CONFIG &ag, SETTINGS &sett) {
    SOLUTION sol;
    unsigned n = problem.n_items * 2;
    unsigned p = ag.P_FACTOR * n;

    BPDecoder decoder; // initialize the decoder
    decoder.bin_h = problem.hbin;
    decoder.bin_w = problem.wbin;
    decoder.boxes = problem.boxes;

    cout << "Class " << problem.problem_class << "\n# Itens " << problem.n_items << "\nbin size " << problem.wbin << " x " << problem.hbin << endl;
    cout << "Instance " << problem.relative << " / Absolute " << problem.absolute << endl;

    // sort(decoder.boxes.begin(), decoder.boxes.end(), sortbyarea);
    // sort(decoder.boxes.begin(), decoder.boxes.end(), sortbywidth);
    // sort(decoder.boxes.begin(), decoder.boxes.end(), sortbyheight);

    const long unsigned rngSeed = 1;    // seed to the random number generator
    MTRand rng(rngSeed);                // initialize the random number generator
    // initialize the BRKGA-based heuristic
    BRKGA<BPDecoder, MTRand> algorithm(n, p, ag.pe, ag.pm, ag.rhoe, decoder, rng, ag.K, ag.MAXT);

    unsigned generation = 0;        // current generation
    const unsigned X_INTVL = 15;    // exchange best individuals at every 15 generations
    const unsigned X_NUMBER = 5;    // exchange top X_NUMBER best

    vector<double> x(ag.MAX_GENS), y(ag.MAX_GENS);
    cout << "Running for " << ag.MAX_GENS << " generations..." << endl;
    do {
        algorithm.evolve(); // evolve the population for one generation

        if (!sett.disable_counter) {
            cout << "\r" << generation << "/" << ag.MAX_GENS << "\t"
                << algorithm.getBestFitness() << "                 ";
        }

        //Chart data
        x.at(generation) = generation;
        y.at(generation) = algorithm.getBestFitness();

        if ((++generation) % X_INTVL == 0)
        {
            algorithm.exchangeElite(X_NUMBER); // exchange top individuals
        }
    } while (generation < ag.MAX_GENS);

    if (!sett.disable_counter) cout << "\r";

    double solution_best_fitness = algorithm.getBestFitness();
    cout << "Best solution found has objective value = "
        << solution_best_fitness << endl;

    if (sett.best_cromo) {
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
    cout << "Number of bins " << bin_capacity.size() << endl << endl;

    sol.best_fitness = solution_best_fitness;
    sol.num_bins = bin_capacity.size();
    
    if (sett.draw_best) {
        drawBest(algorithm.getBestChromosome(), problem);
    }

    plot_results(x, y, ag.MAX_GENS, sett);

    return sol;
}

int main_ag(AG_CONFIG& ag, SETTINGS& sett) {

    list<ProblemInstance> problemInstances;

    problemInstances = loadProblemInstances(sett.path);

    if (sett.gotoabsolute) {
        //vector<double> cromo;
        //double c;
        //while (cin >> c)
        //    cromo.push_back(c);
        //drawBest(cromo, problem);

        for (auto& problem : problemInstances) {
            if (problem.absolute == sett.gotoabsolute) {
                run_ag(problem, ag, sett);
                return 0;
            }
        }

    } else {
        double class_mean = 0;
        double num_bins = 0;
        double best_fitness = numeric_limits<double>::max();

        for (auto& problem : problemInstances) {
            SOLUTION sol = run_ag(problem, ag, sett);
            class_mean += sol.best_fitness;
            num_bins += sol.num_bins;

            best_fitness = min(best_fitness, sol.best_fitness);
            if (problem.relative == 10) {
                cout << "\n" << "# Bins (mean) = " << num_bins / 10 << endl;
                cout << "Class (mean) = " << class_mean / 10 << endl;
                cout << "Best fitness = " << best_fitness << endl << endl;

                class_mean = 0;
                num_bins = 0;
                best_fitness = numeric_limits<double>::max();
            }
        }
    }

    return 0;
}

int meta_ag(AG_CONFIG &ag, SETTINGS &sett) {
   
    list<ProblemInstance> problemInstances;
    problemInstances = loadProblemInstances(sett.path);

    for (auto& problem : problemInstances) {

        unsigned n = problem.n_items * 2;
        unsigned p = ag.P_FACTOR * n;

        BPDecoder decoder; // initialize the decoder
        decoder.bin_h = problem.hbin;
        decoder.bin_w = problem.wbin;
        decoder.boxes = problem.boxes;

        cout << "Class " << problem.problem_class << "\n# Itens " << problem.n_items << "\nbin size " << problem.wbin << " x " << problem.hbin << endl;
        cout << "Instance " << problem.relative << " / Absolute " << problem.absolute << endl;

        const long unsigned rngSeed = 1; // seed to the random number generator
        MTRand rng(rngSeed);             // initialize the random number generator

        BRKGA<BPDecoder, MTRand> brkga(n, p, ag.pe, ag.pm, ag.rhoe, decoder, rng, ag.K, ag.MAXT);

        // initialize the BRKGA-based heuristic
        MetaGA<BRKGA<BPDecoder, MTRand>, MTRand> algorithm(brkga, rng);

        unsigned generation = 0;        // current generation
        
        vector<double> x(ag.MAX_GENS), y(ag.MAX_GENS);
        cout << "Running for " << ag.MAX_GENS << " generations..." << endl;
        do
        {
            algorithm.evolve(); // evolve the population for one generation

            cout << "\r" << generation+1 << "/" << ag.MAX_GENS 
                << "\t" << algorithm.getBestFitness() 
                << " pm: " << brkga.getPm() 
                << " pe: " << brkga.getPe() << "            ";

            //Chart data
            x.at(generation) = generation;
            y.at(generation) = algorithm.getBestFitness();

            ++generation;
        } while (generation < ag.MAX_GENS);

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
        
        plot_results(x, y, ag.MAX_GENS, sett);
        if (sett.draw_best)
            drawBest(brkga.getBestChromosome(), problem);
    }

    return 0;
}

int main(int argc, char **argv) {

    AG_CONFIG agconfig; // Genetic algorithm parameters
    SETTINGS settings = {false, false, false, 0, "teste.in", NULL, false}; // Fill flags from argv

    bool flag_auto_ga = false;

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "--auto") == 0) {
                flag_auto_ga = true;
            } else if (strcmp(argv[i], "--draw") == 0) {
                settings.draw_best = true;
            } else if (strcmp(argv[i], "--chart") == 0) {
                settings.chart = true;
            } else if (strcmp(argv[i], "--bestcromo") == 0) {
                settings.best_cromo = true;
            } else if (strcmp(argv[i], "--inst") == 0) {
                ++i;
                if (i >= argc) { cout << "Missing instance number\n"; return -1; }
                settings.gotoabsolute = atoi(argv[i]);
            } else if (strcmp(argv[i], "--save-chart") == 0) {
                ++i;
                if (i >= argc) { cout << "Missing chart path\n"; return -1; }
                settings.save_path = argv[i];
            } else if (strcmp(argv[i], "--out") == 0) {
                settings.disable_counter = true;
            } else {
                settings.path = argv[i];
                cout << "load instances from: " << settings.path << endl;
            }
        }
    }

    ifstream config("config.json", std::ifstream::binary);
    if (config.is_open()) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(config, root);

        agconfig.pe = root["pe"].asDouble();
        agconfig.pm = root["pm"].asDouble();
        agconfig.rhoe = root["rhoe"].asDouble();
        agconfig.K = root["K"].asUInt();
        agconfig.MAXT = root["MAXT"].asUInt();
        agconfig.MAX_GENS = root["MAX_GENS"].asUInt();
        agconfig.P_FACTOR = root["P_FACTOR"].asUInt();
    } else {
        agconfig.pe = 0.15;
        agconfig.pm = 0.25;
        agconfig.rhoe = 0.70;
        agconfig.K = 4;
        agconfig.MAXT = 4;
        agconfig.MAX_GENS = 200;
        agconfig.P_FACTOR = 5;
    }

    if (flag_auto_ga) {
        return meta_ag(agconfig, settings);
    } else {
        return main_ag(agconfig, settings);
    }
}
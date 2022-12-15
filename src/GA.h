#include <iostream>
#include "SFCGraph.h"
#include "GraphElements.h"
using namespace std;

class Gene
{
public:
    vector<int> gene;
    double weight;

    // init
    Gene(vector<int> _gene, double fitness)
    {
        gene = _gene;
        weight = fitness;
    }

    double Weight() const
    {
        return weight;
    }

    void PrintOut()
    {
        for (int i = 0; i < gene.size(); i++)
        {
            cout << gene[i] << " ";
        }
        cout << endl;
    }
};

class GA
{
public:
    SFCGraph *sfcGraph;
    vector<vector<BasePath *>> paths;
    vector<int> bounds;
    int n_population;
    int n_request;
    vector<Gene *> population;
    double total_fitness;

    double mutate_prob;
    int num_cross_mut;

    GA(SFCGraph *_sfcGraph, int k, int _n_population, double _mutate_prob, int _num_cross_mut);
    void one_round_evoluate();
    void n_round_evoluate(int n_round);
    void show_result();
private:
    void _init_population();
    void _crossover_mutate(Gene *dad, Gene *mom);
    void _mutate(vector<int> *g);
    void _survive();
};

#include <algorithm>
#include <iostream>
#include "SFCGraph.h"
#include "GraphElements.h"
#include "GA.h"
using namespace std;

bool compareGene(Gene *g1, Gene *g2)
{
    return g1->Weight() < g2->Weight();
}

GA::GA(SFCGraph *_sfcGraph, int k, int _n_population, double _mutate_prob, int _num_cross_mut)
{
    sfcGraph = _sfcGraph;
    n_request = _sfcGraph->requestNum;
    paths = _sfcGraph->k_paths(k);
    for (vector<BasePath *> p : paths)
    {
        bounds.push_back(p.size());
    }
    n_population = _n_population;
    mutate_prob = _mutate_prob;
    num_cross_mut = _num_cross_mut;

    _init_population();
}

void GA::show_result() {
    // cout << "Weight: " << population[n_population - 1]->Weight() << endl;
    int num = sfcGraph->count_satisfied(paths, population[n_population - 1]->gene);
    cout << "Num: " << num << endl;
}
void GA::_init_population()
{
    total_fitness = 0;
    for (int i = 0; i < n_population; i++)
    {
        vector<int> gene;
        for (int j = 0; j < n_request; j++)
        {
            int num = (rand() % (bounds[j] + 1)) - 1;
            gene.push_back(num);
        }

        double fitness = sfcGraph->fitness(paths, gene);

        total_fitness += fitness;
        Gene *_gene = new Gene(gene, fitness);
        population.push_back(_gene);
    }

    sort(population.begin(), population.end(), compareGene);
}

void GA::_crossover_mutate(Gene *dad, Gene *mom)
{
    vector<int> child_gene_1;
    vector<int> child_gene_2;

    int cut_point = rand() % n_request;

    for (int i = 0; i < cut_point; i++)
    {
        child_gene_1.push_back(dad->gene[i]);
        child_gene_2.push_back(mom->gene[i]);
    }
    for (int i = cut_point; i < n_request; i++)
    {
        child_gene_1.push_back(mom->gene[i]);
        child_gene_2.push_back(dad->gene[i]);
    }

    _mutate(&child_gene_1);
    _mutate(&child_gene_2);

    double fitness_1 = sfcGraph->fitness(paths, child_gene_1);
    double fitness_2 = sfcGraph->fitness(paths, child_gene_2);

    total_fitness += (fitness_1 + fitness_2);

    Gene *child_1 = new Gene(child_gene_1, fitness_1);
    Gene *child_2 = new Gene(child_gene_2, fitness_2);

    population.push_back(child_1);
    population.push_back(child_2);
}

void GA::_mutate(vector<int> *g)
{
    for (int i = 0; i < n_request; i++)
    {
        double u = double(rand()) / double(RAND_MAX);
        if (u <= mutate_prob)
        {
            int num = rand() % (bounds[i] + 1) - 1;
            (*g)[i] = num;
        }
    }
}

void GA::_survive()
{
    sort(population.begin(), population.end(), compareGene);

    bool *alive = new bool[population.size()];
    int new_gene_count = 0;

    double cur_prob = 0;
    for (int i = 0; i < population.size(); i++)
    {
        Gene *ind = population[i];
        double new_prob = cur_prob + double(ind->Weight()) / total_fitness;
        double r = double(rand()) / double(RAND_MAX);

        if (r >= cur_prob && r < new_prob && new_gene_count < n_population)
        {
            new_gene_count++;
            alive[i] = true;
        }
        else
        {
            alive[i] = false;
        }
    }

    for (int i = population.size() - 1; i >= 0; i--)
    {
        if (!alive[i])
        {
            alive[i] = true;
            new_gene_count++;
            if (new_gene_count == n_population)
            {
                break;
            }
        }
    }

    vector<Gene *> new_gen;
    total_fitness = 0;
    for (int i = 0; i < population.size(); i++)
    {
        if (alive[i])
        {
            new_gen.push_back(population[i]);
            total_fitness += population[i]->Weight();
        }
    }
    population.clear();

    for (int i = 0; i < n_population; i++)
    {
        population.push_back(new_gen[i]);
    }

    new_gen.clear();
    delete[] alive;

    sort(population.begin(), population.end(), compareGene);
}

void GA::one_round_evoluate()
{
    for (int i = 0; i < num_cross_mut; i++)
    {
        int dad_index = rand() % n_population;
        int mom_index = rand() % (n_population - 1);
        if (mom_index == dad_index)
            mom_index++;

        _crossover_mutate(population[dad_index], population[mom_index]);
    }
    _survive();
}

void GA::n_round_evoluate(int n_round)
{
    for (int i = 0; i < n_round; i++)
    {
        // cout << "Start round " << i << endl;
        one_round_evoluate();
        // cout << "Round " << i << ": " << population[n_population - 1]->Weight() << endl;
    }
}
#include "CGA.h"
#include <iostream>
#include <vector>    // for vector
#include <algorithm> // for copy() and assign()
#include <iterator>  // for back_inserter

CGA::CGA(SFCGraph *_sfcGraph, int _k, int _co_n_population_1, double _co_mutate_prob_1, int _co_num_cross_mut_1, int _co_n_population_2, double _co_mutate_prob_2, int _co_num_cross_mut_2, int _n_population, double _mutate_prob, int _num_cross_mut, double _rho)
{
    sfcGraph = _sfcGraph;
    paths = _sfcGraph->k_paths(_k);
    for (vector<BasePath *> p : paths)
    {
        bounds.push_back(p.size());
    }
    co_n_population_1 = _co_n_population_1;
    co_num_cross_mut_1 = _co_num_cross_mut_1;
    co_mutate_prob_1 = _co_mutate_prob_1;
    co_n_population_2 = _co_n_population_2;
    co_num_cross_mut_2 = _co_num_cross_mut_2;
    co_mutate_prob_2 = _co_mutate_prob_2;
    n_population = _n_population;
    num_cross_mut = _num_cross_mut;
    mutate_prob = _mutate_prob;
    rho = _rho;

    ga1 = new GA(_sfcGraph, paths, bounds, _co_n_population_1, _co_mutate_prob_1, _co_num_cross_mut_1, 1);
    ga2 = new GA(_sfcGraph, paths, bounds, _co_n_population_2, _co_mutate_prob_2, _co_num_cross_mut_2, 0);
}
void CGA::_coevoluate(int n_coevoluate_round)
{
    ga1->n_round_evoluate(n_coevoluate_round);
    cout << "GA1: ";
    ga1->show_result();
    ga2->n_round_evoluate(n_coevoluate_round);
    cout << "GA2: ";
    ga2->show_result();
}
void CGA::_merge()
{
    vector<Gene *> population;
    copy(ga1->population.begin(), ga1->population.end(), back_inserter(population));
    copy(ga2->population.begin(), ga2->population.end(), back_inserter(population));
    final_ga = new GA(sfcGraph, paths, bounds, n_population, mutate_prob, num_cross_mut, rho, population);
}
void CGA::_evoluate(int n_evoluate_round)
{
    final_ga->n_round_evoluate(n_evoluate_round);
}

void CGA::evoluate(int n_coevoluate_round, int n_evoluate_round)
{
    _coevoluate(n_coevoluate_round);
    _merge();
    _evoluate(n_evoluate_round);
}
void CGA::show_result()
{
    final_ga->show_result();
}

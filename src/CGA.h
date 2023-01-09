#include "GA.h"
#include "SFCGraph.h"
#include <vector>

using namespace std;
class CGA
{
public:
    SFCGraph *sfcGraph;
    GA *ga1;
    GA *ga2;
    GA *final_ga;
    vector<vector<BasePath *>> paths;
    vector<int> bounds;

    double rho;
    double co_mutate_prob_1;
    int co_num_cross_mut_1;
    int co_n_population_1;

    double co_mutate_prob_2;
    int co_num_cross_mut_2;
    int co_n_population_2;

    double mutate_prob;
    int num_cross_mut;
    int n_population;

    CGA(SFCGraph *_sfcGraph, int _k, int _co_n_population_1, double _co_mutate_prob_1, int _co_num_cross_mut_1, int _co_n_population_2, double _co_mutate_prob_2, int _co_num_cross_mut_2, int _n_population, double _mutate_prob, int _num_cross_mut, double _rho);

    void evoluate(int n_coevoluate_round, int n_evoluate_round);
    void show_result();

private:
    void _coevoluate(int n_coevoluate_round);
    void _merge();
    void _evoluate(int n_evoluate_round);
};

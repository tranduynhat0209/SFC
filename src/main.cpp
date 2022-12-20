/************************************************************************/
/* $Id: MainP.cpp 65 2010-09-08 06:48:36Z yan.qi.asu $                                                                 */
/************************************************************************/

#include <limits>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "GraphElements.h"
#include "Graph.h"
#include "DijkstraShortestPathAlg.h"
#include "YenTopKShortestPathsAlg.h"
#include "SFCGraph.h"
#include "GA.h"

using namespace std;

int main(int argc, char *argv[])
{
	string t1[3] = { "cogent", "nsf", "conus" };
	string t2[4] = { "centers", "rural", "uniform", "urban" };
	string t3[5] = { "0", "1", "2", "3", "4" };
	string t4[3] = { "10", "20", "30" };
	int k = 10;
	int n_population = 10000;
	double mutate_prob = 0.1;
	int num_cross_mut = 2000;
	double rho = 0.99;

	for (int i1 = 0; i1 < 3; i1++)
	{
		for (int i2 = 0; i2 < 4; i2++)
		{
			for (int i3 = 0; i3 < 5; i3++)
			{
				string network_file = t1[i1] + "_" + t2[i2] + "_" + t3[i3] + "_network.txt";
				for (int i4 = 0; i4 < 3; i4++)
				{
					string request_file = t1[i1] + "_" + t2[i2] + "_" + t3[i3] + "_" + t4[i4] + "requests.txt";
					SFCGraph *sfc = new SFCGraph("input/" + network_file, "input/" + request_file, rho);
					GA ga = GA(sfc, k, n_population, mutate_prob, num_cross_mut, FitnessType::FITNESS);
					ga.n_round_evoluate(20);
					ga.show_result();
				}
			}
		}
	}
}

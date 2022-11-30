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

using namespace std;

int main(int argc, char *argv[])
{

	SFCGraph *sfc = new SFCGraph("input/nsf_urban_4_network.txt", "input/nsf_urban_4_10requests.txt");

	vector<vector<BasePath*>> k_paths = sfc->k_paths(3);

	for(int i = 0; i < k_paths.size();i++){
		// for(int j = 0; j < k_paths[i].size(); i++){
		// 	k_paths[i][j]->PrintOut();
		// 	cout << endl;
		// }
		k_paths[i][0]->PrintOut();
		k_paths[i][1]->PrintOut();
		k_paths[i][2]->PrintOut();
		cout << "||---------------||" << endl;
	}
}

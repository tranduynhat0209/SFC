#pragma once
#include <map>
#include <vector>
#include <set>
#include "GraphElements.h"
#include "Graph.h"

using namespace std;

struct Request
{
	int sourceID;
	int sinkID;
	double bandwidth;
	double cpu;
	double memory;
	vector<int> VNFs;

	double Weight() const
	{
		return (bandwidth + cpu + memory) / 3;
	}
};

enum NodeType
{
	Server,
	PNF
};
class SFCGraph
{
public:
	// Basic information
	map<int, set<int> *> neighborVertices;
	vector<int> nodes;
	map<pair<int, int>, double> edgeWeight;
	map<int, double> node_caps;
	map<int, NodeType> node_type;
	vector<Request *> requests;
	map<int, set<int> *> VNF2NodeMap;
	int edgeNum;
	int nodeNum;
	int VNFNum;
	int requestNum;

	double rho;
private:
	void _sort_requests();
public:
	SFCGraph(const string &network_file_name, const string &request_file_name, double _rho);
	~SFCGraph(void);

	Graph *buildMultilayerGraph(Request *request);
	set<int> *get_neighbor_set(int node_id);
	set<int> *get_nodes_set(int vnf);
	Request *get_request(int index);
	void clone_layers(Graph *&graph, int num_layer, Request *request);
	bool consume_path(BasePath *base_path, Request *request, map<pair<int, int>, double> *acc_edge_weight, map<int, double> *acc_node_caps, double &R1, double &R2, double &R3);
	double fitness(vector<vector<BasePath *>> paths, vector<int> gene);
	int count_satisfied(vector<vector<BasePath *>> paths, vector<int> gene);
	double consume_node(Request *request, int node_id);
	vector<vector<BasePath *>> k_paths(int k);
};

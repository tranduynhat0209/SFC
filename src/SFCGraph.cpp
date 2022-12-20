#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "SFCGraph.h"
#include "GraphElements.h"
#include "Graph.h"
#include "YenTopKShortestPathsAlg.h"

using namespace std;

bool compareRequest(Request *r1, Request *r2)
{
    return r1->Weight() < r2->Weight();
}

SFCGraph::~SFCGraph()
{
    for (map<int, set<int> *>::iterator pos = neighborVertices.begin(); pos != neighborVertices.end(); ++pos)
    {
        delete pos->second;
    }
    neighborVertices.clear();

    for (map<int, set<int> *>::iterator pos = VNF2NodeMap.begin(); pos != VNF2NodeMap.end(); ++pos)
    {
        delete pos->second;
    }
    VNF2NodeMap.clear();

    edgeWeight.clear();

    for_each(requests.begin(), requests.end(), DeleteFunc<Request>());
    requests.clear();

    nodes.clear();

    node_caps.clear();

    node_type.clear();

    edgeNum = 0;
    nodeNum = 0;
    requestNum = 0;
}
set<int> *SFCGraph::get_neighbor_set(int node_id)
{
    map<int, set<int> *>::iterator pos = neighborVertices.find(node_id);
    if (pos == neighborVertices.end())
    {
        set<int> *neighbor_set = new set<int>();
        pair<map<int, set<int> *>::iterator, bool> ins_pos =
            neighborVertices.insert(make_pair(node_id, neighbor_set));

        pos = ins_pos.first;
    }
    return pos->second;
}
set<int> *SFCGraph::get_nodes_set(int vnf)
{
    map<int, set<int> *>::iterator pos = VNF2NodeMap.find(vnf);
    if (pos == VNF2NodeMap.end())
    {
        set<int> *nodes_set = new set<int>();
        pair<map<int, set<int> *>::iterator, bool> ins_pos =
            neighborVertices.insert(make_pair(vnf, nodes_set));

        pos = ins_pos.first;
    }
    return pos->second;
}
Request *SFCGraph::get_request(int index)
{
    if (index >= requests.size())
    {
        return NULL;
    }
    return requests[index];
}
Graph *SFCGraph::buildMultilayerGraph(Request *request)
{
    Graph *graph = new Graph();
    int num_layer = request->VNFs.size() + 1;
    clone_layers(graph, num_layer, request);

    for (int current_layer = 0; current_layer < num_layer - 1; current_layer++)
    {
        int vnf = request->VNFs[current_layer];
        set<int> *vnfNodes = get_nodes_set(vnf);

        for (set<int>::iterator node_pos = vnfNodes->begin(); node_pos != vnfNodes->end(); node_pos++)
        {
            int node_id = *node_pos;
            int end_vertex_id = node_id + (current_layer + 1) * nodeNum;
            double node_consumed = consume_node(request, node_id);

            set<int> *neighborSet = get_neighbor_set(*node_pos);
            for (set<int>::iterator neighbor_pos = neighborSet->begin(); neighbor_pos != neighborSet->end(); ++neighbor_pos)
            {
                int neighbor_id = *neighbor_pos;
                double neighbor_consumed = consume_node(request, neighbor_id);
                int start_vertex_id = neighbor_id + current_layer * nodeNum;
                pair<int, int> edge_id = node_id < neighbor_id ? make_pair(node_id, neighbor_id) : make_pair(neighbor_id, node_id);
                map<pair<int, int>, double>::iterator edge_weight_pos = edgeWeight.find(edge_id);
                if (edge_weight_pos != edgeWeight.end())
                {
                    double weight = 30 * (node_consumed + neighbor_consumed) + 40 * (request->bandwidth / edge_weight_pos->second);
                    graph->add_new_edge(start_vertex_id, end_vertex_id, weight);
                }
            }
        }
    }
    return graph;
}
double SFCGraph::consume_node(Request *request, int node_id)
{
    map<int, double>::iterator node_cap_pos = node_caps.find(node_id);
    if (node_cap_pos != node_caps.end())
    {
        double node_cap = node_cap_pos->second;
        map<int, NodeType>::iterator node_type_pos = node_type.find(node_id);
        if (node_type_pos != node_type.end())
        {
            NodeType type = node_type_pos->second;
            if (type == NodeType::Server)
            {
                return request->cpu / node_cap;
            }
            else
            {
                return request->memory / node_cap;
            }
        }
    }
    return 0;
}
void SFCGraph::clone_layers(Graph *&graph, int num_layer, Request *request)
{
    for (int layer = 0; layer < num_layer; layer++)
    {

        for (int node_id : nodes)
        {
            graph->add_new_vertex(node_id + layer * nodeNum);
        }
    }
    for (int layer = 0; layer < num_layer; layer++)
    {
        for (int node_id : nodes)
        {
            double node_consumed = consume_node(request, node_id);
            map<int, set<int> *>::iterator neighbors_it = neighborVertices.find(node_id);
            if (neighbors_it != neighborVertices.end())
            {
                set<int> *neighbors = neighbors_it->second;
                for (set<int>::iterator neighbor_pos = neighbors->begin(); neighbor_pos != neighbors->end(); ++neighbor_pos)
                {
                    int neighbor_id = *neighbor_pos;
                    double neighbor_consumed = consume_node(request, neighbor_id);
                    map<pair<int, int>, double>::iterator edge_weight_pos =
                        edgeWeight.find(node_id < neighbor_id ? make_pair(node_id, neighbor_id) : make_pair(neighbor_id, node_id));
                    if (edge_weight_pos != edgeWeight.end())
                    {
                        double edge_weight = edge_weight_pos->second;
                        double weight = 30 * (node_consumed + neighbor_consumed) + 40 * (request->bandwidth / edge_weight);
                        graph->add_new_edge(node_id + layer * nodeNum, neighbor_id + layer * nodeNum, weight);
                    }
                }
            }
        }
    }
}
vector<vector<BasePath *>> SFCGraph::k_paths(int k)
{
    vector<vector<BasePath *>> result;

    for (int i = 0; i < requestNum; i++)
    {
        vector<BasePath *> k_path;
        Request *request = get_request(i);
        int last_layer = request->VNFs.size();
        int source_id = request->sourceID;
        int sink_id = request->sinkID + nodeNum * last_layer;
        Graph *graph = buildMultilayerGraph(request);

        YenTopKShortestPathsAlg yenAlg(*graph, graph->get_vertex(source_id), graph->get_vertex(sink_id));
        yenAlg.get_shortest_paths(graph->get_vertex(source_id), graph->get_vertex(sink_id), k, k_path);

        result.push_back(k_path);
    }
    return result;
}
bool SFCGraph::consume_path(BasePath *base_path, Request *request, map<pair<int, int>, double> *acc_edge_weight, map<int, double> *acc_node_caps, double &R1, double &R2, double &R3)
{
    int previous_id = request->sourceID;

    double max_cpu = 0;
    double max_mem = 0;
    double max_band = 0;
    for (int i = 0; i < base_path->length(); i++)
    {
        double cpu_consumed = 0;
        double mem_consumed = 0;
        double band_consumed = 0;
        BaseVertex *vertex = base_path->GetVertex(i);

        int node_id = vertex->getID() % nodeNum;

        map<int, double>::iterator acc_node_pos = acc_node_caps->find(node_id);
        map<int, double>::iterator node_pos = node_caps.find(node_id);

        if (acc_node_pos != acc_node_caps->end() && node_pos != node_caps.end())
        {
            double node_cap = acc_node_pos->second;
            map<int, NodeType>::iterator node_type_pos = node_type.find(node_id);

            if (node_type_pos != node_type.end())
            {
                NodeType type = node_type_pos->second;
                if (type == NodeType::Server)
                {
                    node_cap -= request->cpu;
                    cpu_consumed = 1 - (double(node_cap) / node_pos->second);
                }
                else
                {
                    node_cap -= request->memory;
                    mem_consumed = 1 - (double(node_cap) / node_pos->second);
                }
            }
            if (node_cap < 0)
                return false;

            acc_node_pos->second = node_cap;
        }

        if (i == 0)
            continue;
        pair<int, int> edge_id = previous_id < node_id ? make_pair(previous_id, node_id) : make_pair(node_id, previous_id);
        map<pair<int, int>, double>::iterator acc_band_pos = acc_edge_weight->find(edge_id);
        map<pair<int, int>, double>::iterator band_pos = edgeWeight.find(edge_id);
        if (acc_band_pos != acc_edge_weight->end() && band_pos != edgeWeight.end())
        {
            if (acc_band_pos->second < request->bandwidth)
                return false;
            acc_band_pos->second = acc_band_pos->second - request->bandwidth;
            band_consumed = 1 - (double(acc_band_pos->second) / double(band_pos->second));
        }
        max_cpu = max_cpu > cpu_consumed ? max_cpu : cpu_consumed;
        max_mem = max_mem > mem_consumed ? max_mem : mem_consumed;
        max_band = max_band > band_consumed ? max_band : band_consumed;
        previous_id = node_id;
    }

    R1 = R1 > max_band ? R1 : max_band;
    R2 = R2 > max_cpu ? R2 : max_cpu;
    R3 = R3 > max_mem ? R3 : max_mem;
    return true;
}

int SFCGraph::count_satisfied(vector<vector<BasePath *>> paths, vector<int> gene)
{
    map<pair<int, int>, double> acc_edge_weight;
    map<int, double> acc_node_caps;

    acc_edge_weight.insert(edgeWeight.begin(), edgeWeight.end());
    acc_node_caps.insert(node_caps.begin(), node_caps.end());

    double R1 = 0, R2 = 0, R3 = 0;

    int satisfied = 0;
    for (int i = 0; i < gene.size(); i++)
    {
        int index = gene[i];
        if (index == -1)
        {
            continue;
        }

        if (consume_path(paths[i][index], requests[i], &acc_edge_weight, &acc_node_caps, R1, R2, R3))
        {
            satisfied++;
        }
    }
    return satisfied;
}

double SFCGraph::fitness(vector<vector<BasePath *>> paths, vector<int> gene)
{
    map<pair<int, int>, double> acc_edge_weight;
    map<int, double> acc_node_caps;

    acc_edge_weight.insert(edgeWeight.begin(), edgeWeight.end());
    acc_node_caps.insert(node_caps.begin(), node_caps.end());

    double R1 = 0, R2 = 0, R3 = 0;

    int satisfied = 0;
    for (int i = 0; i < gene.size(); i++)
    {
        int index = gene[i];
        if (index == -1)
        {
            continue;
        }

        if (consume_path(paths[i][index], requests[i], &acc_edge_weight, &acc_node_caps, R1, R2, R3))
        {
            satisfied++;
        }
    }
    double fitness = rho * (double(satisfied) / double(requestNum)) + (1 - rho) * (1 - (R1 + R2 + R3) / 3);
    return fitness;
}

double SFCGraph::fitness1(vector<vector<BasePath *>> paths, vector<int> gene)
{
    int satisfied = count_satisfied(paths, gene);
    return double(satisfied) / double(requestNum);
}
double SFCGraph::fitness2(vector<vector<BasePath *>> paths, vector<int> gene)
{
    map<pair<int, int>, double> acc_edge_weight;
    map<int, double> acc_node_caps;

    acc_edge_weight.insert(edgeWeight.begin(), edgeWeight.end());
    acc_node_caps.insert(node_caps.begin(), node_caps.end());

    double R1 = 0, R2 = 0, R3 = 0;

    for (int i = 0; i < gene.size(); i++)
    {
        int index = gene[i];
        if (index == -1)
        {
            continue;
        }
        else
        {
            consume_path(paths[i][index], requests[i], &acc_edge_weight, &acc_node_caps, R1, R2, R3);
        }
    }
    return 1 - (R1 + R2 + R3) / 3;
}
vector<string> line2words(string str)
{
    vector<string> words;
    string word = "";
    for (auto x : str)
    {
        if (x == ' ')
        {

            words.push_back(word);
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    if (word.length() > 0)
        ;
    words.push_back(word);

    return words;
}

SFCGraph::SFCGraph(const string &network_file_name, const string &request_file_name, double _rho)
{
    /*
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
    */
    fstream network_file;
    network_file.open(network_file_name, ios::in);
    if (network_file.is_open())
    {
        string line;

        // 1st line: number of vnfs
        getline(network_file, line);
        VNFNum = line2words(line).size();

        // 2nd line: number of nodes
        getline(network_file, line);
        nodeNum = stoi(line);

        // list of nodes
        for (int i = 3; i < 3 + nodeNum; i++)
        {
            getline(network_file, line);
            vector<string> words = line2words(line);

            int node_id = stoi(words[0]);
            nodes.push_back(node_id);

            float init_cap = stof(words[1]);
            float used_cap = stof(words[2]);
            float cap = init_cap - used_cap;
            node_caps.insert(make_pair(node_id, cap));

            if (words.size() == 3)
            {
                node_type.insert(make_pair(node_id, NodeType::PNF));
            }
            else if (words.size() == 4)
            {
                node_type.insert(make_pair(node_id, NodeType::Server));
                string w = "";
                for (auto x : words[3].substr(1, words[3].length() - 2))
                {
                    if (x == ',')
                    {
                        int vnf = stoi(w);
                        get_nodes_set(vnf)->insert(node_id);
                        w = "";
                    }
                    else
                    {
                        w = w + x;
                    }
                }
                if (w.length() > 0)
                {
                    int vnf = stoi(w);
                    get_nodes_set(vnf)->insert(node_id);
                }
            }
        }

        // (i + nodeNum)th line: number of edge
        getline(network_file, line);
        edgeNum = stoi(line);

        // list of edges
        for (int i = 3 + nodeNum; i < 3 + nodeNum + edgeNum; i++)
        {
            getline(network_file, line);
            vector<string> words = line2words(line);

            int start_node_id = stoi(words[0]);
            int end_node_id = stoi(words[1]);
            get_neighbor_set(start_node_id)->insert(end_node_id);
            get_neighbor_set(end_node_id)->insert(start_node_id);

            float init_cap = stof(words[2]);
            float used_cap = stof(words[3]);
            edgeWeight.insert(make_pair(start_node_id < end_node_id ? make_pair(start_node_id, end_node_id) : make_pair(end_node_id, start_node_id), init_cap - used_cap));
        }

        network_file.close();
    }

    fstream requests_file;
    requests_file.open(request_file_name, ios::in);
    if (requests_file.is_open())
    {
        string line;

        getline(requests_file, line);
        requestNum = stoi(line);

        for (int i = 1; i < requestNum + 1; i++)
        {
            getline(requests_file, line);
            vector<string> words = line2words(line);

            Request *request = new Request();
            request->bandwidth = stof(words[2]);
            request->memory = stof(words[3]);
            request->cpu = stof(words[4]);
            request->sourceID = stoi(words[5]);
            request->sinkID = stoi(words[6]);

            string w = "";
            for (auto x : words[7])
            {
                if (x == ',')
                {
                    int vnf = stoi(w);
                    request->VNFs.push_back(vnf);
                    w = "";
                }
                else
                {
                    w = w + x;
                }
            }
            if (w.length() > 0)
            {
                int vnf = stoi(w);
                request->VNFs.push_back(vnf);
            }
            requests.push_back(request);
        }
        requests_file.close();
    }

    rho = _rho;
    _sort_requests();
}

void SFCGraph::_sort_requests()
{
    sort(requests.begin(), requests.end(), compareRequest);
}

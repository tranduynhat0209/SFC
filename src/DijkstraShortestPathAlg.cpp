#include <set>
#include <map>
#include <vector>
#include "GraphElements.h"
#include "Graph.h"
#include "DijkstraShortestPathAlg.h"

BasePath* DijkstraShortestPathAlg::get_shortest_path( BaseVertex* source, BaseVertex* sink )
{
	determine_shortest_paths(source, sink, true);

	vector<BaseVertex*> vertex_list;
	map<BaseVertex*, double>::const_iterator pos = 
		startDistanceIndex.find(sink);
	double weight = pos != startDistanceIndex.end() ? pos->second : Graph::DISCONNECT;

	if (weight < Graph::DISCONNECT)
	{
		BaseVertex* cur_vertex_pt = sink;
		do 
		{
			vertex_list.insert(vertex_list.begin(), cur_vertex_pt);

			map<BaseVertex*, BaseVertex*>::const_iterator pre_pos = 
				predecessorVertex.find(cur_vertex_pt);

			if (pre_pos == predecessorVertex.end()) break;

			cur_vertex_pt = pre_pos->second;

		} while (cur_vertex_pt != source);

		vertex_list.insert(vertex_list.begin(), source);
	}
	return new BasePath(vertex_list, weight);
}

void DijkstraShortestPathAlg::determine_shortest_paths( BaseVertex* source, BaseVertex* sink, bool is_source2sink )
{
	//1. clear the intermediate variables
	clear();

	//2. initiate the local variables
	BaseVertex* end_vertex = is_source2sink ? sink : source;
	BaseVertex* start_vertex = is_source2sink ? source : sink;
	startDistanceIndex[start_vertex] = 0;
	start_vertex->Weight(0);
	candidateVertices.insert(start_vertex);

	//3. start searching for the shortest path
	while (!candidateVertices.empty())
	{
		multiset<BaseVertex*, WeightLess<BaseVertex> >::const_iterator pos = candidateVertices.begin();

		BaseVertex* cur_vertex_pt = *pos; //m_quCandidateVertices.top();
		candidateVertices.erase(pos);
	
		if (cur_vertex_pt == end_vertex) break;

		determinedVertices.insert(cur_vertex_pt->getID());

		improve2vertex(cur_vertex_pt, is_source2sink);
	}
}

void DijkstraShortestPathAlg::improve2vertex( BaseVertex* cur_vertex_pt, bool is_source2sink )
{
	// 1. get the neighboring vertices 
	set<BaseVertex*>* neighbor_vertex_list_pt = new set<BaseVertex*>();
		
	if(is_source2sink)
	{
		directGraph->get_adjacent_vertices(cur_vertex_pt, *neighbor_vertex_list_pt);
	}else
	{
		directGraph->get_precedent_vertices(cur_vertex_pt, *neighbor_vertex_list_pt);
	}

	// 2. update the distance passing on the current vertex
	for(set<BaseVertex*>::iterator cur_neighbor_pos=neighbor_vertex_list_pt->begin(); 
		cur_neighbor_pos!=neighbor_vertex_list_pt->end(); ++cur_neighbor_pos)
	{
		//2.1 skip if it has been visited before
		if (determinedVertices.find((*cur_neighbor_pos)->getID())!=determinedVertices.end())
		{
			continue;
		}

		//2.2 calculate the distance
		map<BaseVertex*, double>::const_iterator cur_pos = startDistanceIndex.find(cur_vertex_pt);
		double distance =  cur_pos != startDistanceIndex.end() ? cur_pos->second : Graph::DISCONNECT;

		distance += is_source2sink ? directGraph->get_edge_weight(cur_vertex_pt, *cur_neighbor_pos) : 
			directGraph->get_edge_weight(*cur_neighbor_pos, cur_vertex_pt);

		//2.3 update the distance if necessary
		cur_pos = startDistanceIndex.find(*cur_neighbor_pos);
		if (cur_pos == startDistanceIndex.end() || cur_pos->second > distance)
		{
			startDistanceIndex[*cur_neighbor_pos] = distance;
			predecessorVertex[*cur_neighbor_pos] = cur_vertex_pt;
			
			(*cur_neighbor_pos)->Weight(distance);

			multiset<BaseVertex*, WeightLess<BaseVertex> >::const_iterator pos = candidateVertices.begin();
			for(; pos != candidateVertices.end(); ++pos)
			{
				if ((*pos)->getID() == (*cur_neighbor_pos)->getID())
				{
					break;
				}
			}
			if(pos != candidateVertices.end())
			{
				candidateVertices.erase(pos);
			}
			candidateVertices.insert(*cur_neighbor_pos);
		}
	}
	delete neighbor_vertex_list_pt;
}

void DijkstraShortestPathAlg::clear()
{
	determinedVertices.clear();
	predecessorVertex.clear();
	startDistanceIndex.clear();
	candidateVertices.clear();
}

BasePath* DijkstraShortestPathAlg::update_cost_forward( BaseVertex* vertex )
{
	double cost = Graph::DISCONNECT;

 	// 1. get the set of successors of the input vertex
	set<BaseVertex*>* adj_vertex_set = new set<BaseVertex*>();
	directGraph->get_adjacent_vertices(vertex, *adj_vertex_set);
 
 	// 2. make sure the input vertex exists in the index
	map<BaseVertex*, double>::iterator pos4vertexInStartDistIndex = startDistanceIndex.find(vertex);
	if(pos4vertexInStartDistIndex == startDistanceIndex.end())
 	{
		pos4vertexInStartDistIndex = 
			(startDistanceIndex.insert(make_pair(vertex, Graph::DISCONNECT))).first;
 	}

 	// 3. update the distance from the root to the input vertex if necessary
 	for(set<BaseVertex*>::const_iterator pos=adj_vertex_set->begin(); pos!=adj_vertex_set->end();++pos)
 	{
 		// 3.1 get the distance from the root to one successor of the input vertex
		map<BaseVertex*, double>::const_iterator cur_vertex_pos = startDistanceIndex.find(*pos);
		double distance = cur_vertex_pos == startDistanceIndex.end() ?
			Graph::DISCONNECT : cur_vertex_pos->second;
 
 		// 3.2 calculate the distance from the root to the input vertex
		distance += directGraph->get_edge_weight(vertex, *pos);
	
 		// 3.3 update the distance if necessary 
		double cost_of_vertex = pos4vertexInStartDistIndex->second;
 		if(cost_of_vertex > distance)
 		{
			startDistanceIndex[vertex] = distance;
			predecessorVertex[vertex] = cur_vertex_pos->first;
 			cost = distance;
 		}
 	}

 	// 4. create the sub_path if exists
	BasePath* sub_path = NULL;
	if(cost < Graph::DISCONNECT) 
 	{
		vector<BaseVertex*> vertex_list;
		vertex_list.push_back(vertex);

		map<BaseVertex*, BaseVertex*>::const_iterator pos4PredVertexMap =
			predecessorVertex.find(vertex);
		
		while(pos4PredVertexMap != predecessorVertex.end())
		{
			BaseVertex* pred_vertex_pt = pos4PredVertexMap->second;
			vertex_list.push_back(pred_vertex_pt);
			pos4PredVertexMap = predecessorVertex.find(pred_vertex_pt);
		}

		sub_path = new BasePath(vertex_list, cost);
 	}
	delete adj_vertex_set;
	return sub_path;
}

void DijkstraShortestPathAlg::correct_cost_backward( BaseVertex* vertex )
{
 	// 1. initialize the list of vertex to be updated
	vector<BaseVertex*> vertex_pt_list;
	vertex_pt_list.push_back(vertex);

	// 2. update the cost of relevant precedents of the input vertex
	while(!vertex_pt_list.empty())
 	{
		BaseVertex* cur_vertex_pt = *(vertex_pt_list.begin());
		vertex_pt_list.erase(vertex_pt_list.begin());

 		double cost_of_cur_vertex = startDistanceIndex[cur_vertex_pt];

		set<BaseVertex*> pre_vertex_set;
		directGraph->get_precedent_vertices(cur_vertex_pt, pre_vertex_set);
		for(set<BaseVertex*>::const_iterator pos=pre_vertex_set.begin(); pos!=pre_vertex_set.end();++pos)
		{
			map<BaseVertex*,double>::const_iterator pos4StartDistIndexMap = 
				startDistanceIndex.find(*pos);
			double cost_of_pre_vertex = startDistanceIndex.end() == pos4StartDistIndexMap ?
				Graph::DISCONNECT : pos4StartDistIndexMap->second;

			double fresh_cost = cost_of_cur_vertex + directGraph->get_edge_weight(*pos, cur_vertex_pt);
			if(cost_of_pre_vertex > fresh_cost)
			{
				startDistanceIndex[*pos] = fresh_cost;
				predecessorVertex[*pos] = cur_vertex_pt;
				vertex_pt_list.push_back(*pos);
			}
		}
	}
}

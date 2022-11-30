///////////////////////////////////////////////////////////////////////////////
///  DijkstraShortestPathAlg.h
///  The implementation of Dijkstra algorithm to get the shortest path of 
///  a pair of vertices in a graph. 
///
///  @remarks <TODO: insert remarks here>
///
///  @author Yan Qi @date 5/30/2010
/// 
///  $Id: DijkstraShortestPathAlg.h 65 2010-09-08 06:48:36Z yan.qi.asu $
///
///////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace std;

class DijkstraShortestPathAlg
{
private: // members

	Graph* directGraph;

	map<BaseVertex*, double> startDistanceIndex; 
	map<BaseVertex*, BaseVertex*> predecessorVertex; 

	set<int> determinedVertices;
	
	multiset<BaseVertex*, WeightLess<BaseVertex> > candidateVertices;
	
public:
	DijkstraShortestPathAlg(Graph* pGraph):directGraph(pGraph){}
	~DijkstraShortestPathAlg(void){clear();}

	void clear();

	BasePath* get_shortest_path(BaseVertex* source, BaseVertex* sink);

	void set_predecessor_vertex(BaseVertex* vt1, BaseVertex* vt2)
	{
		predecessorVertex[vt1] = vt2;
	}

	double get_start_distance_at(BaseVertex* vertex)
	{
		return startDistanceIndex.find(vertex)->second;
	}

	void set_start_distance_at(BaseVertex* vertex, double weight)
	{
		startDistanceIndex[vertex] = weight;
	}

	void get_shortest_path_flower(BaseVertex* root)
	{
		determine_shortest_paths(NULL, root, false);
	}

	// The following two methods are prepared for the top-k shortest paths algorithm
	BasePath* update_cost_forward(BaseVertex* vertex);
	void correct_cost_backward(BaseVertex* vertex);

protected:

	void determine_shortest_paths(BaseVertex* source, BaseVertex* sink, bool is_source2sink);

	void improve2vertex(BaseVertex* cur_vertex_pt, bool is_source2sink);

};

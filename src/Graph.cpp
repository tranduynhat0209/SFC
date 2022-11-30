///////////////////////////////////////////////////////////////////////////////
///  Graph.cpp
///  <TODO: insert file description here>
///
///  @remarks <TODO: insert remarks here>
///
///  @author Yan Qi @date 8/18/2010
///
///  $Id: Graph.cpp 65 2010-09-08 06:48:36Z yan.qi.asu $
///////////////////////////////////////////////////////////////////////////////

#include <limits>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "GraphElements.h"
#include "Graph.h"
using namespace std;

const double Graph::DISCONNECT = (numeric_limits<double>::max)();

Graph::Graph(const string &file_name)
{
	_import_from_file(file_name);
}

Graph::Graph(const Graph &graph)
{
	vertexNum = graph.vertexNum;
	edgeNum = graph.edgeNum;
	vertices.assign(graph.vertices.begin(), graph.vertices.end());
	faninVertices.insert(graph.faninVertices.begin(), graph.faninVertices.end());
	fanoutVertices.insert(graph.fanoutVertices.begin(), graph.fanoutVertices.end());
	edgeCodeWeight.insert(graph.edgeCodeWeight.begin(), graph.edgeCodeWeight.end());
	vertexIndex.insert(graph.vertexIndex.begin(), graph.vertexIndex.end());
}
Graph::Graph(int nV, vector<tuple<int, int, double>> edges)
{
	vertexNum = nV;
	edgeNum = edges.size();
	for (tuple<int, int, double> edge : edges)
	{
		int start_vertex = get<0>(edge);
		int end_vertex = get<1>(edge);
		double edge_weight = get<2>(edge);

		/// 3.2.1 construct the vertices
		BaseVertex *start_vertex_pt = get_vertex(start_vertex);
		BaseVertex *end_vertex_pt = get_vertex(end_vertex);

		/// 3.2.2 add the edge weight
		//// note that the duplicate edge would overwrite the one occurring before.
		edgeCodeWeight[get_edge_code(start_vertex_pt, end_vertex_pt)] = edge_weight;

		/// 3.2.3 update the fan-in or fan-out variables
		//// Fan-in
		get_vertex_set_pt(end_vertex_pt, faninVertices)->insert(start_vertex_pt);

		//// Fan-out
		get_vertex_set_pt(start_vertex_pt, fanoutVertices)->insert(end_vertex_pt);
	}
}
Graph::~Graph(void)
{
	clear();
}

///////////////////////////////////////////////////////////////////////////////
///  public  _import_from_file
///  Construct the graph by importing the edges from the input file.
///
///  @param [in]       file_name const string &    The input graph file
///
///  This function doesn't return a value
///
///  @remarks The format of the file is as follows:
///   1. The first line has an integer as the number of vertices of the graph
///   2. Each line afterwards contains a directed edge in the graph:
///		     starting point, ending point and the weight of the edge.
///		 These values are separated by 'white space'.
///
///  @see <TODO: insert text here>
///
///  @author Yan Qi @date 5/29/2010
///////////////////////////////////////////////////////////////////////////////
void Graph::_import_from_file(const string &input_file_name)
{
	const char *file_name = input_file_name.c_str();

	// 1. Check the validity of the file
	ifstream ifs(file_name);
	if (!ifs)
	{
		cerr << "The file " << file_name << " can not be opened!" << endl;
		exit(1);
	}

	// 2. Reset the members of the class
	clear();

	// 3. Start to read information from the input file.
	/// Note the format of the data in the graph file.
	// 3.1 The first line has an integer as the number of vertices of the graph
	ifs >> vertexNum;

	// 3.2 In the following lines, each line contains a directed edge in the graph:
	///   the id of starting point, the id of ending point, the weight of the edge.
	///   These values are separated by 'white space'.
	int start_vertex, end_vertex;
	double edge_weight;
	int vertex_id = 0;

	while (ifs >> start_vertex)
	{
		if (start_vertex == -1)
		{
			break;
		}
		ifs >> end_vertex;
		ifs >> edge_weight;

		/// 3.2.1 construct the vertices
		BaseVertex *start_vertex_pt = get_vertex(start_vertex);
		BaseVertex *end_vertex_pt = get_vertex(end_vertex);

		/// 3.2.2 add the edge weight
		//// note that the duplicate edge would overwrite the one occurring before.
		edgeCodeWeight[get_edge_code(start_vertex_pt, end_vertex_pt)] = edge_weight;

		/// 3.2.3 update the fan-in or fan-out variables
		//// Fan-in
		get_vertex_set_pt(end_vertex_pt, faninVertices)->insert(start_vertex_pt);

		//// Fan-out
		get_vertex_set_pt(start_vertex_pt, fanoutVertices)->insert(end_vertex_pt);
	}

	if (vertexNum != vertices.size())
	{
		cerr << "The number of nodes in the graph is " << vertices.size() << " instead of " << vertexNum << endl;
		exit(1);
	}

	vertexNum = vertices.size();
	edgeNum = edgeCodeWeight.size();

	ifs.close();
}

BaseVertex *Graph::get_vertex(int node_id)
{
	if (removedVertexIds.find(node_id) != removedVertexIds.end())
	{
		// if node is removed, return null
		return NULL;
	}
	else
	{
		BaseVertex *vertex_pt = NULL;
		const map<int, BaseVertex *>::iterator pos = vertexIndex.find(node_id);
		if (pos == vertexIndex.end())
		{

			// if node doesn't exist, add node to graph
			int vertex_id = vertices.size();
			vertex_pt = new BaseVertex();
			vertex_pt->setID(node_id);
			vertexIndex[node_id] = vertex_pt;

			vertices.push_back(vertex_pt);
		}
		else
		{
			vertex_pt = pos->second;
		}

		return vertex_pt;
	}
}

void Graph::add_new_vertex(int node_id)
{
	BaseVertex *vertex_pt = NULL;
	const map<int, BaseVertex *>::iterator pos = vertexIndex.find(node_id);
	if (pos == vertexIndex.end())
	{

		// if node doesn't exist, add node to graph
		vertex_pt = new BaseVertex();
		vertex_pt->setID(node_id);
		vertexIndex[node_id] = vertex_pt;

		vertices.push_back(vertex_pt);
		vertexNum++;
	}
}
void Graph::add_new_edge(int start_vertex_id, int end_vertex_id, double edge_weight)
{
	const map<int, BaseVertex *>::iterator start_pos = vertexIndex.find(start_vertex_id);
	const map<int, BaseVertex *>::iterator end_pos = vertexIndex.find(end_vertex_id);
	if (start_pos != vertexIndex.end() && end_pos != vertexIndex.end())
	{
		//// Fan-in
		get_vertex_set_pt(end_pos->second, faninVertices)->insert(start_pos->second);

		//// Fan-out
		get_vertex_set_pt(start_pos->second, fanoutVertices)->insert(end_pos->second);

		edgeCodeWeight[get_edge_code(start_pos->second, end_pos->second)] = edge_weight;

		edgeNum++;
	}
}
void Graph::clear()
{
	edgeNum = 0;
	vertexNum = 0;

	for (map<BaseVertex *, set<BaseVertex *> *>::const_iterator pos = faninVertices.begin();
		 pos != faninVertices.end(); ++pos)
	{
		delete pos->second;
	}
	faninVertices.clear();

	for (map<BaseVertex *, set<BaseVertex *> *>::const_iterator pos = fanoutVertices.begin();
		 pos != fanoutVertices.end(); ++pos)
	{
		delete pos->second;
	}
	fanoutVertices.clear();

	edgeCodeWeight.clear();

	// clear the list of vertices objects
	for_each(vertices.begin(), vertices.end(), DeleteFunc<BaseVertex>());
	vertices.clear();
	vertexIndex.clear();

	removedVertexIds.clear();
	removedEdges.clear();
}

int Graph::get_edge_code(const BaseVertex *start_vertex_pt, const BaseVertex *end_vertex_pt) const
{
	/// Note that the computation below works only if
	/// the result is smaller than the maximum of an integer!
	return start_vertex_pt->getID() * vertexNum + end_vertex_pt->getID();
}

set<BaseVertex *> *Graph::get_vertex_set_pt(BaseVertex *vertex_, map<BaseVertex *, set<BaseVertex *> *> &vertex_container_index)
{
	BaseVertexPt2SetMapIterator pos = vertex_container_index.find(vertex_);

	if (pos == vertex_container_index.end())
	{
		set<BaseVertex *> *vertex_set = new set<BaseVertex *>();
		pair<BaseVertexPt2SetMapIterator, bool> ins_pos =
			vertex_container_index.insert(make_pair(vertex_, vertex_set));

		pos = ins_pos.first;
	}

	return pos->second;
}

double Graph::get_edge_weight(const BaseVertex *source, const BaseVertex *sink)
{
	int source_id = source->getID();
	int sink_id = sink->getID();

	if (removedVertexIds.find(source_id) != removedVertexIds.end() || removedVertexIds.find(sink_id) != removedVertexIds.end() || removedEdges.find(make_pair(source_id, sink_id)) != removedEdges.end())
	{
		return DISCONNECT;
	}
	else
	{
		return get_original_edge_weight(source, sink);
	}
}

void Graph::get_adjacent_vertices(BaseVertex *vertex, set<BaseVertex *> &vertex_set)
{
	int starting_vt_id = vertex->getID();

	if (removedVertexIds.find(starting_vt_id) == removedVertexIds.end())
	{
		set<BaseVertex *> *vertex_pt_set = get_vertex_set_pt(vertex, fanoutVertices);
		for (set<BaseVertex *>::const_iterator pos = (*vertex_pt_set).begin();
			 pos != (*vertex_pt_set).end(); ++pos)
		{
			int ending_vt_id = (*pos)->getID();
			if (removedVertexIds.find(ending_vt_id) != removedVertexIds.end() || removedEdges.find(make_pair(starting_vt_id, ending_vt_id)) != removedEdges.end())
			{
				continue;
			}
			//
			vertex_set.insert(*pos);
		}
	}
}

void Graph::get_precedent_vertices(BaseVertex *vertex, set<BaseVertex *> &vertex_set)
{
	if (removedVertexIds.find(vertex->getID()) == removedVertexIds.end())
	{
		int ending_vt_id = vertex->getID();
		set<BaseVertex *> *pre_vertex_set = get_vertex_set_pt(vertex, faninVertices);
		for (set<BaseVertex *>::const_iterator pos = (*pre_vertex_set).begin();
			 pos != (*pre_vertex_set).end(); ++pos)
		{
			int starting_vt_id = (*pos)->getID();
			if (removedVertexIds.find(starting_vt_id) != removedVertexIds.end() || removedEdges.find(make_pair(starting_vt_id, ending_vt_id)) != removedEdges.end())
			{
				continue;
			}
			//
			vertex_set.insert(*pos);
		}
	}
}

double Graph::get_original_edge_weight(const BaseVertex *source, const BaseVertex *sink)
{
	map<int, double>::const_iterator pos =
		edgeCodeWeight.find(get_edge_code(source, sink));

	if (pos != edgeCodeWeight.end())
	{
		return pos->second;
	}
	else
	{
		return DISCONNECT;
	}
}

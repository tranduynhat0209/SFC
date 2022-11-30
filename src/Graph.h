#pragma once
#include <map>
#include <vector>
#include <set>

using namespace std;

class Path : public BasePath
{
public: 

	Path(const vector<BaseVertex*>& vertex_list, double weight):BasePath(vertex_list,weight){}

	// display the content
	void PrintOut() const
	{
		cout << "Cost: " << weight << " Length: " << vertexList.size() << endl;
		for(vector<BaseVertex*>::const_iterator pos=vertexList.begin(); pos!=vertexList.end();++pos)
		{
			cout << (*pos)->getID() << " ";
		}
		cout << endl <<  "*********************************************" << endl;	
	}
};

class Graph
{
public: // members

	const static double DISCONNECT; 

	typedef set<BaseVertex*>::iterator VertexPtSetIterator;
	typedef map<BaseVertex*, set<BaseVertex*>*>::iterator BaseVertexPt2SetMapIterator;

public: // members

	// Basic information
	map<BaseVertex*, set<BaseVertex*>*> fanoutVertices;
	map<BaseVertex*, set<BaseVertex*>*> faninVertices;
	map<int, double> edgeCodeWeight; 
	vector<BaseVertex*> vertices;
	int edgeNum;
	int vertexNum;

	map<int, BaseVertex*> vertexIndex;

	// Members for graph modification
	set<int> removedVertexIds;
	set<pair<int,int> > removedEdges;

public:

	// Constructors and Destructor
	Graph() = default;
	Graph(const string& file_name);
	Graph(const Graph& rGraph);
	Graph(int nV, vector<tuple<int, int, double>> edges);
	~Graph(void);

	void clear();

	BaseVertex* get_vertex(int node_id);
	
	int get_edge_code(const BaseVertex* start_vertex_pt, const BaseVertex* end_vertex_pt) const;
	set<BaseVertex*>* get_vertex_set_pt(BaseVertex* vertex_, map<BaseVertex*, set<BaseVertex*>*>& vertex_container_index);

	double get_original_edge_weight(const BaseVertex* source, const BaseVertex* sink);

	double get_edge_weight(const BaseVertex* source, const BaseVertex* sink);
	void get_adjacent_vertices(BaseVertex* vertex, set<BaseVertex*>& vertex_set);
	void get_precedent_vertices(BaseVertex* vertex, set<BaseVertex*>& vertex_set);

	void add_new_vertex(int node_id);
	void add_new_edge(int start_vertex_id, int end_vertex_id, double edge_weight);

	/// Methods for changing graph
	void remove_edge(const pair<int,int> edge)
	{
		removedEdges.insert(edge);
	}

	void remove_vertex(const int vertex_id)
	{
		removedVertexIds.insert(vertex_id);
	}

	void recover_removed_edges()
	{
		removedEdges.clear();
	}

	void recover_removed_vertices()
	{
		removedVertexIds.clear();
	}

	void recover_removed_edge(const pair<int,int> edge)
	{
		removedEdges.erase(removedEdges.find(edge));
	}

	void recover_removed_vertex(int vertex_id)
	{
		removedVertexIds.erase(removedVertexIds.find(vertex_id));
	}
	
private:
	void _import_from_file(const string& file_name);

};

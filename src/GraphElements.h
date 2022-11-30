#pragma once

#include <string>
#include <vector>
#include <iostream>
using namespace std;

template<class T>
class WeightGreater
{
public:
	// Determine priority.
	bool operator()(const T& a, const T& b) const
	{
		return a.Weight() > b.Weight();
	}

	bool operator()(const T* a, const T* b) const
	{
		return a->Weight() > b->Weight();
	}
};

template<class T>
class WeightLess
{
public:
	// Determine priority.
	bool operator()(const T& a, const T& b) const
	{
		return a.Weight() < b.Weight();
	}

	bool operator()(const T* a, const T* b) const
	{
		return a->Weight() < b->Weight();
	}
};

//////////////////////////////////////////////////////////////////////////
// A class for the object deletion
//////////////////////////////////////////////////////////////////////////
template<class T>
class DeleteFunc
{
public:
	void operator()(const T* it) const
	{
		delete it;
	}
};


class BaseVertex
{
	int ID;
	double weight;

public:

	int getID() const { return ID; }
	void setID(int ID_) { ID = ID_; }

	double Weight() const { return weight; }
	void Weight(double val) { weight = val; }

	void PrintOut()
	{
		cout << ID;
	}
};

class BasePath
{
protected:

	int p_length;
	double weight;
	vector<BaseVertex*> vertexList;

public:
	BasePath(const vector<BaseVertex*>& vertex_list, double weight)
		:weight(weight)
	{
		vertexList.assign(vertex_list.begin(), vertex_list.end());
		p_length = vertexList.size();
	}
	~BasePath(void){}

	double Weight() const { return weight; }
	void Weight(double val) { weight = val; }

	int length() const { return p_length; }

	BaseVertex* GetVertex(int i) const
	{
		return vertexList.at(i);
	}

	bool SubPath(vector<BaseVertex*>& sub_path, BaseVertex* ending_vertex_pt) const
	{

		for (vector<BaseVertex*>::const_iterator pos = vertexList.begin();
			pos != vertexList.end(); ++pos)
		{
			if (*pos != ending_vertex_pt)
			{
				sub_path.push_back(*pos);
			}else
			{
				//break;
				return true;
			}
		}

		return false;
	}

	// display the content
	void PrintOut() const
	{
		cout << "Cost: " << weight << " Length: " << vertexList.size() << endl;
		for(vector<BaseVertex*>::const_iterator pos=vertexList.begin(); pos!=vertexList.end();++pos)
		{
			(*pos)->PrintOut();
			cout << "->";
		}
		cout << endl <<  "*********************************************" << endl;
	}
};

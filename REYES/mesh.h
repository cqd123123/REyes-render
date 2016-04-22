#ifndef MESH_H
#define MESH_H

#include <vector>
#include "Eigen/Eigen"
#include "util.h"
#include <iostream>

using namespace Eigen;


struct Vertex{

	Vector3f position;
	Vector3f Normal;
	Vector3f Color;
	Vector3f TextCord;
};

//store pointer to four vertexs, so if the vertex is transformed, the quad stay the same
//unless remeshing is necessary
struct Quad{
	std::vector<Vertex*> vertexs;
	Vector3f BoxMin;
	Vector3f BoxMax;
};


class Mesh{
friend class Mesh2D;
public:
	Mesh(){
		_totalV = 0;
		_totalQ = 0;
	}
	~Mesh(){
		vertexs.clear();
		Quads.clear();
		_totalV = 0;
		_totalQ = 0;
	}

	int _totalV;
	int _totalQ;

	int get_numof_vert() {return vertexs.size();}
	int get_numof_quad() {return Quads.size();}
	void Add_Vertexs(Vector3f pos, Vector3f Norm, Vector3f col, Vector3f texCord)
	{
		Vertex vet;
		vet.position = pos;
		vet.Normal = Norm;
		vet.Color = col;
		vet.TextCord = texCord;
		vertexs.push_back(vet);
		_totalV ++;
	}
	void BindColor_Uniform(Vector3f _color)
	{
		for(int i=0;i<vertexs.size();i++)
		{
			vertexs[i].Color = _color;
		}
	}
	void GetQuadBoundingBox()
	{
		for(int i=0;i<Quads.size();i++)
		{
			Quad _q = Quads[i];
			Vector3f boxmin(FLT_MAX, FLT_MAX, FLT_MAX);
			Vector3f boxmax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			for(int j=0;j<_q.vertexs.size();j++)
			{
				Vector3f _pos = _q.vertexs[j]->position;

				boxmax = max_comp(_pos, boxmax);
				boxmin = min_comp(_pos, boxmin);
			}
			Quads[i].BoxMax = boxmax;
			Quads[i].BoxMin = boxmin;
		}
	}
	void Add_Vertexs(Vertex vet)
	{
		vertexs.push_back(vet);
		_totalV++;
	}
	void Add_Quads(Quad quad)
	{
		Quads.push_back(quad);
		_totalQ ++;
	}
	//return reference enable modification
	inline Vertex& get_vertexs(int i)
	{
		assert(i<vertexs.size());
		return vertexs[i];
	}
	inline Vector3f& get_vert_pos(int i)
	{
		assert(i<vertexs.size());
		return vertexs[i].position;
	}
	inline Quad& get_quads(int i)
	{
		assert(i<Quads.size());
		return Quads[i];
	}
	Vector3f BoundMin;
	Vector3f BoundMax;
private:
	std::vector<Vertex> vertexs;
	std::vector<Quad> Quads;
};

class Mesh2D : public Mesh
{
public:
	inline Vertex& get_vertexs(int i, int j)
	{
		assert(i<=xRes&&i>=0&&j<=yRes&&j>=0);
		return vertexs[i + j*(xRes+1)];
	}
	inline Quad& get_quads(int i, int j)
	{
		assert(i<xRes&&i>=0&&j<yRes&&j>=0);
		return Quads[i + j*xRes];
	}
	inline Vector3f get_vector_pos(int i, int j)
	{
		assert(i<=xRes&&i>=0&&j<=yRes&&j>=0);
		return vertexs[i + j*(xRes+1)].position;
	}
	int xRes;
	int yRes;
	void Debug()
	{
		for(int i=0;i<_totalQ;i++)
			std::cout<<Quads[i].vertexs[0]->position<<std::endl;
	}
};


#endif
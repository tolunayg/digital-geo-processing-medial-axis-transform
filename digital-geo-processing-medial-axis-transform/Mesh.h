#pragma once
#pragma once

#include <iostream>
#include <vector>

using namespace std;

struct Vertex
{
	float* coords, * normals; //3d coordinates etc
	int idx; //who am i; verts[idx]

	vector< int > vertList; //adj vvertices;
	vector< int > triList;
	vector< int > edgeList;

	Vertex(int i, float* c) : idx(i), coords(c) {};
};

struct Edge
{
	int idx; //edges[idx]
	int v1i, v2i; //endpnts
	float length;
	Edge(int id, int v1, int v2) : idx(id), v1i(v1), v2i(v2) { computeLength(); };

	void computeLength()
	{
		length = 7;
	}
};

struct Triangle {
	int idx;     // tris[idx]
	int v1i, v2i, v3i;

	Triangle(int id, int v1, int v2, int v3) : idx(id), v1i(v1), v2i(v2), v3i(v3) {};

	// Function to check if the triangle contains a given edge
	bool hasEdge(Edge* edge) {
		// Check if any of the triangle's edges matches the given edge
		return (edge->v1i == v1i && edge->v2i == v2i) ||
			(edge->v1i == v2i && edge->v2i == v3i) ||
			(edge->v1i == v3i && edge->v2i == v1i);
	}
};

struct Point {
	double coords[3]; // Array to store x, y, z coordinates
	double winding;   // Variable to store winding number
};

class Mesh
{
private:
	void addTriangle(int v1, int v2, int v3);
	void addEdge(int v1, int v2);
	void addVertex(float x, float y, float z);
	bool makeVertsNeighbor(int v1i, int v2i);
public:
	vector< Vertex* > verts;
	vector< Triangle* > tris;
	vector< Edge* > edges;


	Mesh() {};
	void createCube(float side);
	void loadOff(char* name);
	void windingNumberByYusufSahillioglu(Point* pnt);
};

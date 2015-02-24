#include <stdio.h>
#include <windows.h>
#include "Mesh.h"

Mesh::Mesh(int numVert)
{
	Vertices = 0;
	Normals = 0;

	Vertices = new float[numVert * 3];
	Normals = new float[numVert * 3];

	VertCount = 0;
}

Mesh::~Mesh()
{
	delete [] Vertices;
	delete [] Normals;
}

void Mesh::AddVertex(float *Vertex, float *Normal)
{

	memcpy(&Vertices[VertCount*3], Vertex, sizeof(float) * 3);
	memcpy(&Normals[VertCount*3], Normal, sizeof(float) * 3);

	VertCount++;

}
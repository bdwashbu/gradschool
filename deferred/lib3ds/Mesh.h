#ifndef _MESH_H_
#define _MESH_H_

#include <lib3ds/lib3ds.h>

class Mesh
{
	public:
		Mesh(int numVert);
		~Mesh();
		void AddVertex(float *Vertex, float *Normal);
		int numElements;
		int VertCount;
		float *Vertices;
		float *Normals;
		float *Colors;
};

#endif
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <windows.h>
#include "Utilities\glew.h"
#include <gl\gl.h>
#include <lib3ds/lib3ds.h>
#include "Mesh.h"

class Material
{
	public:
		Material();
		~Material();
		void allocateMaterial(int numVert);
		void drawMaterialMesh();
		void finalizeMesh();
		Mesh *MaterialMesh;
		float diffuse[4];
		float specular[3];
		float ambient[3];
		float shininess;
		float transparency;
		int twoSided;
		GLuint m_VertexVBO;
		GLuint m_NormalVBO;
};

#endif
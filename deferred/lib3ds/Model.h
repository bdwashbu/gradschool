#ifndef _MODEL_H_
#define _MODEL_H_

#include <lib3ds/lib3ds.h>
#include "Mesh.h"
#include "Material.h"

class Model
{
	public:
		Model(char *filename);
		virtual void Draw(bool drawTransparent);
		virtual void CreateVBO();
		virtual ~Model();
	protected:
		Material *Materials;
		Lib3dsFile *m_model;
		char *filename;
		int numMeshes;
		int numMaterials;
};

#endif
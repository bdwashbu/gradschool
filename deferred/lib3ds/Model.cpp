
#include <windows.h>
#include "Utilities\glew.h"
#include <gl\gl.h>
#include <stdio.h>
#include "Model.h"
#include "Material.h"
#include <stdio.h>
#include <lib3ds/lib3ds.h>

int lib3ds_file_mesh_by_name(Lib3dsFile *file, const char *name);
void lib3ds_mesh_calculate_vertex_normals(Lib3dsMesh *mesh, float (*normals)[3]);


Model::Model(char * filename)
{
	int i = 0;
	int j = 0;
	int matCount = 0;
	numMeshes = 0;
	int numNodes = 0;
	Lib3dsNode* node = 0;
	this->filename = filename;
}

Model::~Model()
{
	if(m_model != NULL)
	{
		lib3ds_file_free(m_model);
	}
}

// Copy vertices and normals to the memory of the GPU
void Model::CreateVBO()
{
	m_model = lib3ds_file_open(filename);

	int meshCount = 0;
	int matCount = 0;
	int vertexCount = 0;
	Lib3dsMesh * mesh = 0;
	float blah[3] = {1.0, 0.0, 1.0};
	unsigned int FinishedFaces = 0;
	Lib3dsFace * face = 0;
	int numVert = 0;
	int meshNumber = 0;
	Lib3dsMeshInstanceNode *l;
	float Vertex[3] = {0};

	Materials = new Material[m_model->nmaterials];
	numMaterials = m_model->nmaterials;

	// Count the number of verts per material, then allocate Materials

	for (matCount = 0; matCount < m_model->nmaterials; matCount++)
	{
		for (int i = 0; i < 3; i++) {
			Materials[matCount].diffuse[i] = m_model->materials[matCount]->diffuse[i];
			Materials[matCount].specular[i] = m_model->materials[matCount]->specular[i];
			Materials[matCount].ambient[i] = m_model->materials[matCount]->ambient[i];		
		}
		Materials[matCount].diffuse[3] = m_model->materials[matCount]->transparency;
		Materials[matCount].transparency = m_model->materials[matCount]->transparency;
		Materials[matCount].twoSided = m_model->materials[matCount]->two_sided;

		Materials[matCount].shininess = m_model->materials[matCount]->shininess * 128.0f;

		numVert = 0;
		for(meshCount = 0; meshCount < m_model->nmeshes; meshCount++)
		{
			mesh = m_model->meshes[meshCount];
			for(unsigned int cur_face = 0; cur_face < mesh->nfaces; cur_face++)
			{
				face = &mesh->faces[cur_face];
				if (face->material == matCount) {
					numVert += 3;
				}
			}
		}

		Materials[matCount].allocateMaterial(numVert);
	}

	Lib3dsNode *Node;
	Lib3dsNode *Node2;
	int breakEarly = 0;
	for (Node2 = m_model->nodes; Node2 != 0 && breakEarly == 0; Node2 = Node2->next) {

		if (Node2->childs != 0) {
			Node = Node2->childs;
		} else {
			breakEarly = 1;
			Node = Node2;
		}

		while (Node != 0) {

			l = (Lib3dsMeshInstanceNode*)Node;	

			meshNumber = lib3ds_file_mesh_by_name(m_model, Node->name);

			if (meshNumber == -1) { // skip this mesh
				continue;
			}
			vertexCount = 0;

			float (*normals)[3] = new float[m_model->meshes[meshNumber]->nfaces * 3][3];

			mesh = m_model->meshes[meshNumber];

			lib3ds_mesh_calculate_vertex_normals(mesh, normals);

			for(unsigned int cur_face = 0; cur_face < mesh->nfaces; cur_face++) // add the polygon
			{
				Lib3dsFace * face = &mesh->faces[cur_face];

				Vertex[0] = m_model->meshes[meshNumber]->vertices[face->index[0]][0] - l->pivot[0];
				Vertex[1] = m_model->meshes[meshNumber]->vertices[face->index[0]][1] - l->pivot[1];
				Vertex[2] = m_model->meshes[meshNumber]->vertices[face->index[0]][2] - l->pivot[2];
				Materials[face->material].MaterialMesh->AddVertex(&Vertex[0], &normals[3*cur_face][0]);

				Vertex[0] = m_model->meshes[meshNumber]->vertices[face->index[1]][0] - l->pivot[0];
				Vertex[1] = m_model->meshes[meshNumber]->vertices[face->index[1]][1] - l->pivot[1];
				Vertex[2] = m_model->meshes[meshNumber]->vertices[face->index[1]][2] - l->pivot[2];
				Materials[face->material].MaterialMesh->AddVertex(&Vertex[0], &normals[3*cur_face+1][0]);

				Vertex[0] = m_model->meshes[meshNumber]->vertices[face->index[2]][0] - l->pivot[0];
				Vertex[1] = m_model->meshes[meshNumber]->vertices[face->index[2]][1] - l->pivot[1];
				Vertex[2] = m_model->meshes[meshNumber]->vertices[face->index[2]][2] - l->pivot[2];
				Materials[face->material].MaterialMesh->AddVertex(&Vertex[0], &normals[3*cur_face+2][0]);

			}

			delete [] normals;

			Node = Node->next;
		}

	}

	for (matCount = 0; matCount < m_model->nmaterials; matCount++)
	{
		Materials[matCount].finalizeMesh();
	}
	lib3ds_file_free(m_model);
}

// Render the model using Vertex Buffer Objects
void Model::Draw(bool drawTransparent)
{
	int matCount = 0;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	for (matCount = 0; matCount < numMaterials; matCount++)
	{

		if (Materials[matCount].transparency == 0.0) {
			Materials[matCount].drawMaterialMesh();
		}
	}

	if (drawTransparent) { // render transparent meshes

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(0);

		for (matCount = 0; matCount < numMaterials; matCount++)
		{

			if (Materials[matCount].transparency != 0.0) {
				Materials[matCount].drawMaterialMesh();
			}
		}

		glDisable(GL_BLEND);
		glDepthMask(1);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}
#include "Material.h"
#include "Utilities\glew.h"
#include <gl\gl.h>
#include <stdio.h>

Material::Material()
{
	for (int i = 0; i < 3; i++) {
		diffuse[i] = 0.0;
		specular[i] = 0.0;
	}
	shininess = 0.0;
	twoSided = 0;
	transparency = 0.0;

	glGenBuffers(1, &m_VertexVBO);
	glGenBuffers(1, &m_NormalVBO);
}

void Material::allocateMaterial(int numVert)
{
	MaterialMesh = new Mesh(numVert);
}

void Material::drawMaterialMesh()
{
	float matAmb[4] = {ambient[0], ambient[1], ambient[2], 1.0}; //material properties
	float matDiff[4] = {diffuse[0], diffuse[1], diffuse[2], diffuse[3]};
	float matSpec[4] = {specular[0], specular[1], specular[2], 1.0};
	float matShiny = shininess;

	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight); //give LIGHT0 attributes
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec); //set up material
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
	glMaterialf(GL_FRONT, GL_SHININESS, matShiny);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, m_NormalVBO);
	glNormalPointer(GL_FLOAT, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, MaterialMesh->VertCount);

}

void Material::finalizeMesh()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * MaterialMesh->VertCount, MaterialMesh->Vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_NormalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * MaterialMesh->VertCount, MaterialMesh->Normals, GL_STATIC_DRAW);
}

Material::~Material()
{
	delete MaterialMesh;
	glDeleteBuffers(1, &m_VertexVBO);
	glDeleteBuffers(1, &m_NormalVBO);
}
#include "Render/PointLight.h"
#include <gl\gl.h>
#include <gl\glu.h>

PointLight::PointLight(Shader* const lightShader) : Light(lightShader)
{
	quadratic = gluNewQuadric();
	sphereDL = glGenLists(1);

	glNewList(sphereDL, GL_COMPILE);
		gluSphere(quadratic, 1.0, 9, 9);
	glEndList();

	lightType = "pointlight";
}

PointLight::~PointLight()
{
}

void PointLight::BeginShading(float camX, float camY, float camZ)
{

	if ((camX - position[0])*(camX - position[0]) +
		(camY - position[1])*(camY - position[1]) +
		(-camZ - position[2])*(-camZ - position[2]) > range * range * 1.2) { // in the volume

		glCullFace(GL_BACK);
		glStencilMask(1);

		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
		glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		glScalef(range*1.1f, range*1.1f, range*1.1f);
		glCallList(sphereDL);
		glPopMatrix();

		glCullFace(GL_FRONT);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

		glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		glScalef(range*1.1f, range*1.1f, range*1.1f);
		glCallList(sphereDL);
		glPopMatrix();

		glCullFace(GL_BACK);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilMask(0);
		glStencilFunc(GL_EQUAL, 0, 1);
		glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

		//glPushMatrix(); // to draw the bounding volumes
		//glTranslatef(position[0], position[1], position[2]);
		//glScalef(range, range, range);
		//glCallList(sphereDL);
		//glPopMatrix();
	}

	glUseProgram(lightShader->getProgram());
	lightShader->SetUniform1i("normalTexture", 2);
	lightShader->SetUniform1i("specularTexture", 3);
	lightShader->SetParameter("lightColor", color[0], color[1], color[2]);
	lightShader->SetParameter("lightSpecular", specular[0], specular[1], specular[2]);
	lightShader->SetParameter("LightCoord", position[0], position[1], position[2]);
	lightShader->SetUniform1f("lightIntensity", intensity);
	lightShader->SetUniform1f("lightRange", range);

}

void PointLight::EndShading()
{
	glUseProgram(0);
	
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilMask(1);

	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glScalef(range*1.1f, range*1.1f, range*1.1f);
	glCallList(sphereDL);
	glPopMatrix();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	
}
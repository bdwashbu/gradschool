#include "Render/GlobalLight.h"
#include <gl\gl.h>
#include <gl\glu.h>

GlobalLight::GlobalLight(Shader* const lightShader, const float x, const float y, const float z)
     : Light(lightShader)
{
	SetDirection(x, y, z);
	shadowFBO = 0;
	lightType = "globallight";
}

GlobalLight::~GlobalLight()
{
}

void GlobalLight::SetDirection(const float x, const float y, const float z)
{
	this->direction[0] = x;
	this->direction[1] = y;
	this->direction[2] = z;
}

void GlobalLight::BeginShading(float camX, float camY, float camZ)
{

	shadowFBO->BindTexture("shadow map", 4);

	glUseProgram(lightShader->getProgram());
	lightShader->SetUniform1i("normalTexture", 2);
	//lightShader->SetUniform1i("specularTexture", 3);
	lightShader->SetUniform1i("diffuseTexture", 1);
	lightShader->SetUniform1i("shadowMap", 4);
	lightShader->SetUniformMat("lightMatrix", lightTextureMatrix);
	lightShader->SetUniformMat("invViewMatrix", invViewMatrix);
	lightShader->SetParameter("lightColor", color[0], color[1], color[2]);
	//lightShader->SetParameter("lightSpecular", specular[0], specular[1], specular[2]);
	lightShader->SetParameter("LightCoord", position[0], position[1], position[2]);
	lightShader->SetParameter("lightDir", direction[0], direction[1], direction[2]);
	lightShader->SetUniform1f("lightIntensity", intensity);
	lightShader->SetUniform1f("lightRange", range);

	
}

void GlobalLight::EndShading(void)
{
	glUseProgram(0);
	//FBO::Unbind(); // ping is written to*/
}
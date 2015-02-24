#include "Light.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include <iostream>
#include <fstream>
using namespace std;

Light::Light(Shader* const lightShader)
{
	this->lightShader = lightShader;

	for (int lightindex = 0; lightindex < 3; lightindex++) {
		position[lightindex] = 0.0;
		specular[lightindex] = 1.0;
		color[lightindex] = 1.0;
	}

}

void Light::SetShadowFBO(FBO* const shadow)
{
	shadowFBO = shadow;
}

void Light::SetTextureMatrix(float *textureMatrix)
{
	for (int i = 0; i < 16; i++) {
		lightTextureMatrix[i] = textureMatrix[i];
	}
}

void Light::SetInvViewMatrix(float *invViewMatrix)
{
	for (int i = 0; i < 16; i++) {
		this->invViewMatrix[i] = invViewMatrix[i];
	}
}

void Light::SetPosition(const float x, const float y, const float z)
{
	this->position[0] = x;
	this->position[1] = y;
	this->position[2] = z;
}

void Light::SetIntensity(const float intensity)
{
		
	this->intensity = intensity;
}

void Light::SetRange(const float range)
{
	this->range = range;
}

void Light::SetColor(const float red, const float green, const float blue)
{
	//printf("red: %f", red);

	this->color[0] = red;
	this->color[1] = green;
	this->color[2] = blue;
}

void Light::SetSpecular(const float red, const float green, const float blue)
{

	this->specular[0] = red;
	this->specular[1] = green;
	this->specular[2] = blue;
}

void Light::WriteLightToFile(ofstream *outfile)
{
	*outfile << lightType << "\n";
	*outfile << color[0] << " " << color[1] << " " << color[2] << "\n";
	*outfile << position[0] << " " << position[1] << " " << position[2] << "\n";
	*outfile << specular[0] << " " << specular[1] << " " << specular[2] << "\n";
	*outfile << intensity << "\n";
	*outfile << range << "\n\n";
}

Light::~Light()
{
}
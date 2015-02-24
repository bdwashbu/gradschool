#include "lightManager.h"
#include "Render\GlobalLight.h"
#include "Render\PointLight.h"
#include "Render\RenderManager.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace OpenGL;

LightManager::LightManager()
{
	lightCounter = 0;
}

LightManager::~LightManager()
{
	ofstream myfile ("out_lights.txt");
	for(unsigned int i = 0; i < lights.size(); i++) //delete each texture
	{
		lights.at(i)->WriteLightToFile(&myfile);
		delete lights.at(i);
	}
	myfile.close();
}

void LightManager::Initialize(FBO *shadowMap, float *textureMatrix, float *invViewMatrix)
{
	pointLight.LoadVertexShader("Shaders/vert.point_light.glsl"); //shader for applying a point light
	pointLight.LoadFragmentShader("Shaders/frag.point_light.glsl");
	pointLight.LinkShaders();

	globalLight.LoadVertexShader("Shaders/vert.global_lighting.glsl"); //shader for applying a global light
	globalLight.LoadFragmentShader("Shaders/frag.global_lighting.glsl");
	globalLight.LinkShaders();

	this->shadowMap = shadowMap;
	this->textureMatrix = textureMatrix;
	this->invViewMatrix = invViewMatrix;
}

void LightManager::DrawLightsIntoBuffer(FBO *lightBuffer, const float camX, const float camY, const float camZ)
{

	glEnable(GL_BLEND); //add ping onto pong
	glBlendFunc(GL_ONE, GL_ONE);

	glDepthMask(0);
	lightBuffer->Bind();

	// render global lights

	for(unsigned int i = 0; i < lights.size(); i++)
	{
		if (lights.at(i)->lightType == "globallight") {
			lights.at(i)->SetShadowFBO(shadowMap);
			lights.at(i)->SetTextureMatrix(textureMatrix);
			lights.at(i)->SetInvViewMatrix(invViewMatrix);	

			OpenGL::go3D();
			OpenGL::SetCamera();

			lights.at(i)->BeginShading(camX, camY, camZ);

			OpenGL::drawFullscreenQuad();

			lights.at(i)->EndShading();
		}
	}

	//render point lights

	glEnable(GL_STENCIL_TEST); // get light volumes working

	OpenGL::go3D();
	OpenGL::SetCamera();

	for(unsigned int i = 0; i < lights.size(); i++)
	{
		if (lights.at(i)->lightType == "pointlight") {

			lights.at(i)->BeginShading(camX, camY, camZ);

			OpenGL::drawFullscreenQuad();
			OpenGL::go3D();
			OpenGL::SetCamera();	

			lights.at(i)->EndShading();

		}

	}

	FBO::Unbind();
	glUseProgram(0);

	glDisable(GL_STENCIL_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilMask(0);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);
}

void LightManager::AddPointLight(const float posX, const float posY, const float posZ)
{
	PointLight *temp = new PointLight(&pointLight);
	lightCounter++;
	if (lightCounter == 0) {
	temp->SetColor(1.5, 0.0, 0.0);
	temp->SetSpecular(0.75, 0.0, 0.0);
	} else if (lightCounter == 1) {
	temp->SetColor(0.0, 1.5, 0.0);
	temp->SetSpecular(0.0, 0.75, 0.0);
	} else {
	temp->SetColor(0.0, 0.0, 1.5);
	temp->SetSpecular(0.0, 0.0, 0.75);
	lightCounter = -1;
	}

	/*temp->SetColor(1.2, 1.2, 1.2);
	temp->SetSpecular(0.0, 0.0, 0.0);*/
	temp->SetPosition(posX, posY, posZ);

	temp->SetIntensity(2.0);
	temp->SetRange(10.0);
	lights.push_back(temp);


}

void LightManager::ReadLights()
{

	string line;

	ifstream myfile ("lights.txt");
	string input;
	string lightType;

	float position[3];
	float specular[3];
	float color[3];
	float intensity;
	float range;

	if (myfile.is_open())
	{
		while (! myfile.eof() )
		{
			myfile >> lightType;

			if (lightType == "pointlight") {
				PointLight *temp = new PointLight(&pointLight);
				myfile >> color[0] >> color[1] >> color[2];
				myfile >> position[0] >> position[1] >> position[2];
				myfile >> specular[0] >> specular[1] >> specular[2];
				myfile >> intensity;
				myfile >> range;

				temp->SetColor(color[0], color[1], color[2]);
				temp->SetPosition(position[0], position[1], position[2]);
				temp->SetSpecular(specular[0], specular[1], specular[2]);
				temp->SetIntensity(intensity);
				temp->SetRange(range);

				lights.push_back(temp);
			} else if (lightType == "globallight") {
				GlobalLight *temp = new GlobalLight(&globalLight, 0.0, -1.0, 0.0);
				myfile >> color[0] >> color[1] >> color[2];
				myfile >> position[0] >> position[1] >> position[2];
				myfile >> specular[0] >> specular[1] >> specular[2];
				myfile >> intensity;
				myfile >> range;

				temp->SetColor(color[0], color[1], color[2]);
				temp->SetPosition(position[0], position[1], position[2]);
				temp->SetSpecular(specular[0], specular[1], specular[2]);
				temp->SetIntensity(intensity);
				temp->SetRange(range);
				lights.push_back(temp);
			} 

			lightType = "";
		}
		myfile.close();
	} else cout << "Unable to open lights file"; 

}
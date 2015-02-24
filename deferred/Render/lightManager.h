#ifndef _LIGHT_MANAGER_H_
#define _LIGHT_MANAGER_H_

#include <vector>
#include "Light.h"
#include "FBO.h"

class LightManager
{
public:
	LightManager();
	~LightManager();
	void Initialize(FBO *shadowMap, float *textureMatrix, float *invViewMatrix);
	void ReadLights();
	void AddPointLight(const float posX, const float posY, const float posZ);
	void DrawLightsIntoBuffer(FBO *lightBuffer, const float camX, const float camY, const float camZ);

private:
	float *textureMatrix;
	float *invViewMatrix;
	FBO *shadowMap;
	int lightCounter;
	vector <Light*> lights;
	Shader pointLight, globalLight;
};

#endif;

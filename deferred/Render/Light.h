#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Render\Shader.h"
#include "Render\FBO.h"
using namespace std;

class Light
{

public:
	Light(Shader* const lightShader);
	virtual void BeginShading(float camX, float camY, float camZ) = 0;
	virtual void EndShading() = 0;
	void SetPosition(const float x, const float y, const float z);
	void SetIntensity(const float intensity);
	void SetRange(const float range);
	void SetTextureMatrix(float *textureMatrix);
	void SetInvViewMatrix(float *invViewMatrix);
	void SetShadowFBO(FBO* const shadow);
	void SetColor(const float red, const float green, const float blue);
	void SetSpecular(const float red, const float green, const float blue);
	void WriteLightToFile(ofstream *outfile);
	string lightType;
	virtual ~Light();
protected:
	Shader *lightShader;
	float position[3];
	float color[3];
	float intensity;
    float specular[3];
	float range;
	FBO frameBuffer;
	FBO *shadowFBO;
	float lightTextureMatrix[16];
	float invViewMatrix[16];
};

#endif
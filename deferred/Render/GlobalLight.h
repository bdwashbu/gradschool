#ifndef _GLOBAL_LIGHT_H_
#define _GLOBAL_LIGHT_H_

#include "Render\Shader.h"
#include "Render\Light.h"
#include <gl\gl.h>
#include <gl\glu.h>

class GlobalLight : public Light
{
	public:
		GlobalLight(Shader* const lightShader, const float x, const float y, const float z);
		void BeginShading(float camX, float camY, float camZ);
		void EndShading(void);
		void SetDirection(const float x, const float y, const float z);
		~GlobalLight();
	private:
		float direction[3];
};

#endif
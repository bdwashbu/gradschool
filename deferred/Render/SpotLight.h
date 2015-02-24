#ifndef _SPOT_LIGHT_H_
#define _SPOT_LIGHT_H_

#include "Render\Shader.h"
#include "Render\Light.h"
#include <gl\gl.h>
#include <gl\glu.h>

class SpotLight : public Light
{
	public:
		SpotLight(Shader* const lightShader, const float x, const float y, const float z);
		void BeginShading(FBO* const frameBuffer, float camX, float camY, float camZ);
		void EndShading();
		void SetDirection(const float x, const float y, const float z);
		~SpotLight();
	private:
		GLUquadricObj *quadratic;
		GLuint sphereDL;
		float direction[3];
};

#endif
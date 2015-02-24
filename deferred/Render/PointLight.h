#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_

#include "Render\Shader.h"
#include "Render\Light.h"
#include <gl\gl.h>
#include <gl\glu.h>

class PointLight : public Light
{
	public:
		PointLight(Shader* const lightShader);
		void BeginShading(float camX, float camY, float camZ);
		void EndShading();
		~PointLight();
	private:
		GLUquadricObj *quadratic;
		GLuint sphereDL;
};

#endif
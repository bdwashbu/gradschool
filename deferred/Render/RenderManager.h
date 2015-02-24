#ifndef _RENDER_MANAGER_H_
#define _RENDER_MANAGER_H_

#include <windows.h>
#include "Utilities\glew.h"
#include <gl\gl.h>

#include "Camera\glCamera.h"
#include "Render\shader.h"
#include "Utilities\frameRate.h"
#include <iostream>

using namespace std;

namespace OpenGL {

	GLvoid Initialize(GLvoid);
	GLvoid UpdateViewPitch(const GLfloat newPitch);
	GLvoid UpdateViewHeading(const GLfloat newHeading);
	GLvoid UpdateViewVelocity(const GLfloat newVelocity);
	GLvoid SetCamera(GLvoid);
	GLvoid Draw(GLvoid);
	GLvoid go2D(GLvoid);
	GLvoid go3D(GLvoid);
	GLvoid drawFullscreenQuad(GLvoid);
	GLvoid AddLight(GLvoid);
	GLvoid ResizeGLScene(const GLsizei width, const GLsizei height);
	void SetDepthDarkening(int enabled);
	void SetSoftShadows(int enabled);
	void SetMotionBlur(int enabled);
	void SetDepthOfField(int enabled);
	void setDepth(int mode);
	GLvoid setDisplayedTexture(string textureName);
}

#endif
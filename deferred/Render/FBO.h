#ifndef _FBO_H_
#define _FBO_H_

#include <windows.h>
#include "Utilities\glew.h"
#include <gl\gl.h>
#include <vector>
#include <string>
using std::string;

using namespace std;

class FBO
{

public:
	FBO();
	void AddTexture(const string textureName, const GLuint textureFormat, const GLuint width, const GLuint height, const int mipmaps);
	void AddDepthBuffer(const GLuint width, const GLuint height);
	void AddStencilBuffer(const GLuint width, const GLuint height);
	void drawFBO(const unsigned screenWidth, const unsigned screenHeight, const string textureName);
	void Bind();
	void GenMipMaps();
	void BindTexture(const string textureName, const GLuint textureUnit);
	static void Unbind();
	~FBO();
private:
	struct Texture;
	vector <Texture> Textures;
	GLuint FBO_ID;
	GLuint RB_ID;
	GLuint STENCIL_ID;
	unsigned numTextures; // number of textures associated with this FBO
};

#endif
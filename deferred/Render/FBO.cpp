/******************************************
 FBO.cpp                                                              
 ------------------                                                   
                                                                      
 This file contains functionality pertaining
    to Frame-buffer-objects  
                                                                      
 Brian Washburn (10/20/2009)                                          
******************************************/

#include "FBO.h"
#include <stdio.h>
#include <string>
#include "Utilities\glew.h"
#include <gl\gl.h>
using std::string;

struct FBO::Texture {
    string name;
	GLuint ID;
	int mipmapped;
};


FBO::FBO()
{
	numTextures = 0;
	FBO_ID = 0;
	RB_ID = 0;
}

FBO::~FBO()
{
	glDeleteRenderbuffersEXT(1, &RB_ID);
	glDeleteFramebuffersEXT(1, &FBO_ID);

	for(unsigned int i = 0; i < Textures.size(); i++) //delete each texture
	{
		glDeleteTextures(1, &Textures.at(i).ID);
	}
}

void FBO::AddDepthBuffer(const GLuint width, const GLuint height)
{
	glGenRenderbuffersEXT(1, &RB_ID); 

	//glGenTextures(1, &RB_ID);
	//glBindTexture(GL_TEXTURE_2D, RB_ID);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO_ID);
	//glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_NONE);
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_ATTACHMENT_EXT, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, RB_ID); 
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height); 
	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, RB_ID, 0);
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, RB_ID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glBindTexture(GL_TEXTURE_2D, RB_ID);
	//glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0); 
}

void FBO::AddStencilBuffer(const GLuint width, const GLuint height)
{
	/*glGenTextures(1, &STENCIL_ID);
	glBindTexture(GL_TEXTURE_2D, STENCIL_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO_ID);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_EXT, width, height, 0, GL_DEPTH_STENCIL_EXT,  GL_UNSIGNED_INT_24_8_EXT, NULL);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D , STENCIL_ID, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D , STENCIL_ID, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);*/

	GLuint packed_rb;
	glGenRenderbuffersEXT(1, &packed_rb);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO_ID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, packed_rb);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, width, height);
	//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, packed_rb);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, 0x821A, GL_RENDERBUFFER_EXT, packed_rb); // had to use hex value, no entry in glew
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

void FBO::AddTexture(const string textureName, const GLuint textureFormat, const GLuint width, const GLuint height, const int mipmaps)
{
	
	if (FBO_ID == 0) {
		glGenFramebuffersEXT(1, &FBO_ID);
	}
	
	Texture newTexture;
	newTexture.name = textureName;
	newTexture.mipmapped = 0;


	glGenTextures(1, &newTexture.ID);
	glBindTexture(GL_TEXTURE_2D, newTexture.ID);
	if (mipmaps == 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		newTexture.mipmapped = 1;
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, GL_RGBA, GL_FLOAT, 0); // generate texture
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + numTextures, GL_TEXTURE_2D, newTexture.ID, 0); // Bind texture to FBO
		
	numTextures += 1;
	Textures.push_back(newTexture);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void FBO::drawFBO(const unsigned screenWidth, const unsigned screenHeight, const string textureName)
{
	bool found = false;
	for(unsigned int i = 0;i < Textures.size(); i++)
	{
		if (Textures.at(i).name == textureName) {
			glMatrixMode(GL_MODELVIEW);	
			glLoadIdentity();
			glMatrixMode(GL_PROJECTION);						
			glLoadIdentity();								
			gluOrtho2D(0,screenWidth,0,screenHeight);

			//glDisable(GL_DEPTH_TEST);
			glDepthMask(0);

			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, Textures.at(i).ID);

			glBegin(GL_QUADS);
					glTexCoord2f(1.0f, 1.0f); glVertex2i(screenWidth, screenHeight);
					glTexCoord2f(0.0f, 1.0f); glVertex2i(0, screenHeight);
					glTexCoord2f(0.0f, 0.0f); glVertex2i(0, 0);
					glTexCoord2f(1.0f, 0.0f); glVertex2i(screenWidth, 0);
			glEnd();

			//glEnable(GL_LIGHTING);
			glDepthMask(1);
			//glEnable(GL_DEPTH_TEST);

			glDisable(GL_TEXTURE_2D);
			found = true;
			break;
		}
	}	

	if (!found) {
		printf("bad texture name!\n");
	}
}

void FBO::Bind()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO_ID);

	if (numTextures > 1) {
		GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
		glDrawBuffers(3, buffers);
	} else {
		GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT };
		glDrawBuffers(1, buffers);
	}
}

void FBO::BindTexture(const string textureName, const GLuint textureUnit)
{
	for(unsigned int i = 0; i < Textures.size(); i++) //delete each texture
	{
		if (Textures.at(i).name == textureName) {
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, Textures.at(i).ID);
			break;
		}
	}
}

void FBO::GenMipMaps()
{
	for(unsigned int i = 0; i < Textures.size(); i++) //delete each texture
	{
		if (Textures.at(i).mipmapped == 1) {
			glBindTexture(GL_TEXTURE_2D, Textures.at(i).ID);
			glGenerateMipmapEXT(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void FBO::Unbind()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
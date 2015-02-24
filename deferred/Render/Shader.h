/************************************************************************/
/* Shader                                                               */
/* ------------------                                                   */
/*                                                                      */
/* The shader class is used to load and execute a pixel                 */
/* and vertex shader.  The individual programs must first be loaded,    */
/*  and then linked.                                                    */
/*                                                                      */
/* By: Brian Washburn                                                   */
/************************************************************************/

#ifndef _SHADER_H_
#define _SHADER_H_

#include "Utilities\glew.h"
#include <gl\gl.h>

class Shader {

public:
	Shader();
	~Shader();
	void LoadFragmentShader(char *filename);
	void LoadVertexShader(char *filename);
	int SetParameter( char *paramName, float x, float y, float z);
	GLuint getProgram();
	void LinkShaders();
	void SetUniform1i(char *uniformname, int value);
	void SetUniform1f(char *uniformname, float value);
	void SetUniformMat(char *uniformname, float *value);
private:
	bool fragmentLoaded;
	bool vertexLoaded;
	GLuint Program;
	GLuint fragmentShader;
	GLuint vertexShader;
	char *filename;
	char *fragmentShaderSource;
	char *vertexShaderSource;
	char *ReturnFileAsString(char *filename);
};

#endif

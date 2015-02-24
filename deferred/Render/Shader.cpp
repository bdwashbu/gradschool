/******************************************
 Shader.cpp                                                              
 ------------------                                                   
                                                                      
 This class contains functionality
   used to load, link, and executed
   pixel or vertex shaders
                                                                      
 Brian Washburn (10/20/2009)                                          
******************************************/

#include <GL/glew.h>
#include <windows.h>
#include <gl\gl.h>
#include <stdio.h>
#include <iostream>
#include "Render\shader.h"

using namespace std;

Shader::Shader()
{
	Program = 0;
	fragmentShaderSource = 0;
	vertexShaderSource = 0;
	fragmentLoaded = 0;
	vertexLoaded = 0;
	filename = 0;
}

Shader::~Shader()
{
	if (fragmentLoaded != 0) {
		glDetachShader(Program, fragmentShader);
		glDeleteShader(fragmentShader);
	}
	if (vertexLoaded != 0) {
		glDetachShader(Program, vertexShader);
		glDeleteShader(vertexShader);
	}
}

void Shader::LoadFragmentShader(char *filename)
{
	GLint compiled = 0;
	char buf[4096];
	char input;
	this->filename = filename;

	if (Program == 0)
		Program = glCreateProgram();
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentShaderSource = ReturnFileAsString(filename);
	glShaderSource(fragmentShader, 1, (const char**)&fragmentShaderSource, 0);
	free(fragmentShaderSource);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLsizei theLen;
		printf("\n\n**** GLSL Error when compiling fragment shader ****\n");
		glGetShaderInfoLog(fragmentShader, 4095, &theLen, buf );
		printf("File: %s\n", filename);
		printf("Error:\n%s\n", buf);
		fragmentLoaded = 1;
		cout << "Enter a character to exit: ";
		cin  >> input;
	}
	glAttachShader(Program, fragmentShader);
}

void Shader::LoadVertexShader(char *filename)
{
	GLint compiled = 0;
	char buf[4096];
	char input;

	if (Program == 0)
		Program = glCreateProgram();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	vertexShaderSource = ReturnFileAsString(filename);
	glShaderSource(vertexShader, 1, (const char**)&vertexShaderSource, NULL);
	free(vertexShaderSource);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLsizei theLen2;
		printf("\n\n**** GLSL Error when compiling vertex shader ****\n");
		glGetShaderInfoLog(vertexShader, 4095, &theLen2, buf );
		printf("File: %s\n", filename);
		printf("Error:\n%s\n", buf);
		vertexLoaded = 1;
		cout << "Enter a character to exit: ";
		cin  >> input;
	}
	glAttachShader(Program, vertexShader);
}

void Shader::LinkShaders()
{
	GLint linked;
	glLinkProgram(Program);
	glGetProgramiv(Program, GL_LINK_STATUS, &linked);
	if (!linked)
		printf("%s: Failed linking shaders\n", filename);
}

int Shader::SetParameter( char *paramName, float x, float y, float z )
{
	GLint location = glGetUniformLocation(Program, paramName );
	if (location != -1)
		glUniform3f( location, x, y, z );
	return location;
}

void Shader::SetUniform1i(char *uniformname, int value)
{
	GLuint location = glGetUniformLocation(Program, uniformname);
	if (location != -1)
		glUniform1i(location, value);
}

void Shader::SetUniformMat(char *uniformname, float *value)
{
	GLuint location = glGetUniformLocation(Program, uniformname);
	if (location != -1)
		glUniformMatrix4fv(location, 1, 0, value);
	else
		printf("%s: bad shader param\n", uniformname);
}

void Shader::SetUniform1f(char *uniformname, float value)
{
	GLuint location = glGetUniformLocation(Program, uniformname);
	if (location != -1)
		glUniform1f(location, value);
}

/*int Shader::SetParameter( char *paramName, float x, float y, float z )
{
	GLint location = glGetUniformLocation(Program, paramName );
	if (location != -1)
		glUniform3f( location, x, y, z );
	else
		printf("bad uniform: %s\n", paramName);
	return location;
}

void Shader::SetUniform1i(char *uniformname, int value)
{
	GLuint location = glGetUniformLocation(Program, uniformname);
	if (location != -1)
		glUniform1f(location, value);
	else
		printf("bad uniform: %s\n", uniformname);
}

void Shader::SetUniform1f(char *uniformname, float value)
{
	GLuint location = glGetUniformLocation(Program, uniformname);
	if (location != -1)
		glUniform1f(location, value);
	else
		printf("bad uniform: %s\n", uniformname);
}*/

GLuint Shader::getProgram()
{
	return Program;
}

char *Shader::ReturnFileAsString( char *filename )
{
	/* open the file */
	FILE *file = fopen( filename, "r" );
	char input;

	if (!file)
	{
		printf("**** Error opening file '%s'\n", filename );
		cout << "Enter a character to exit: ";
		cin  >> input;
		exit(0);
	}

	/* get the length of the file */
	fseek( file, 0, SEEK_END );
	long size = ftell( file );
	rewind( file );

	/* allocate memory for the shader string */
	char *shaderMemory = (char *)calloc( (size+1), sizeof(char) );
	if (!shaderMemory) { fclose(file); return 0; }

	/* read shader data from the file */
	fread (shaderMemory,1,size,file);
	shaderMemory[size] = 0;

	/* clean up and return the data */
	fclose(file);
	return shaderMemory;
}
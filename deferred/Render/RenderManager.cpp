/******************************************
RenderManager.cpp

Purpose: This file serves as the interface
to all things opengl.  A namespace
is allocated to make it clear
all functionality is related to OpenGL.

By: Brian Washburn (12-12-09)
*******************************************/

#include "Render\RenderManager.h"
#include "Render\drawTextToGLWindow.h"
#include "lib3ds\Model.h"
#include "FBO.h"
#include <windows.h>
#include "Utilities\glew.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <math.h>
#include "Render/PointLight.h"
#include "Render/GlobalLight.h"
#include "Render/LightManager.h"
#include <iostream>

using namespace std;

namespace OpenGL {

	Model * currentModel;
	float rotateAngle;
	FrameRate FPS;
	GLdouble windowWidth, windowHeight;

	glCamera Camera(160.5, 21.1, 2.0, 78, 61);
	glCamera shadowCamera(103.2, 87.4, 22.48, 105.91, -11.6);

	LightManager lightManager; // manages all lighting

	Shader writeGBuffer; // fundamental shaders
	Shader pointLight, globalLight, writeShadowMap, shadow_gaussianX, shadow_gaussianY, applyLight; // lighting shaders
	Shader ssao_gaussianX, ssao_gaussianY, DOF_x, DOF_y, motionBlur; // post processing shaders
	Shader displayDepth;

	FBO G_buffer, ping, pong, pang; // FBO's
	FBO lightAccum;
	FBO shadowMap, shadowMap2;
	FBO result;

	int depthDarkeningEnabled = 1;
	int softShadowsEnabled = 1;
	int motionBlurEnabled = 1;
	int depthOfFieldEnabled = 1;
	int Depth_mode = 0;

	float prevCameraViewMatrix[16];
	float cameraViewMatrix[16];
	float proj2DMatrix[16];
	float proj3DMatrix[16];
	float lightViewMatrix[16];
	float shadowProjMatrix[16];
	float textureMatrix[16];
	float invViewMatrix[16];

	bool alreadyInit = false;

	string displayedTexture = "Result";

	glVector ftl, ftr, fbr, fbl;

	GLvoid AddLight(GLvoid)
	{
		lightManager.AddPointLight(Camera.GetXPosition(), Camera.GetYPosition(), -Camera.GetZPosition());
	}

	GLvoid SetCamera(void)
	{
		glMatrixMode(GL_MODELVIEW); 
		glLoadMatrixf(cameraViewMatrix);	
	}

	GLvoid go3D(GLvoid)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(proj3DMatrix);
		//glLoadIdentity();
		//gluPerspective(80.0,(float)windowWidth/windowHeight,0.1f,200);
	}

	GLvoid go2D(GLvoid)
	{
		glMatrixMode(GL_PROJECTION);	
		//glLoadMatrixf(proj2DMatrix);
		glLoadIdentity();	
		gluOrtho2D(0,windowWidth,0,windowHeight);
	}

	void drawFullscreenQuad()
	{
		go2D();

		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glNormal3f(ftr.i, ftr.j, ftr.k); glVertex2i(windowWidth, windowHeight);
		glTexCoord2f(0.0f, 1.0f); glNormal3f(ftl.i, ftl.j, ftl.k); glVertex2i(0, windowHeight);
		glTexCoord2f(0.0f, 0.0f); glNormal3f(fbl.i, fbl.j, fbl.k); glVertex2i(0, 0);
		glTexCoord2f(1.0f, 0.0f); glNormal3f(fbr.i, fbr.j, fbr.k); glVertex2i(windowWidth, 0);
		glEnd();
	}

	GLvoid ComputeFrustumCoords()
	{

		double Hfar = 2.0 * tan(80.0 * 0.0174532925 / 2.0) * 200.0;
		double Wfar = Hfar * (double)windowWidth/(double)windowHeight;

		glVector fc = (Camera.m_DirectionVector * 200.0);
		fc.i = 0.0;
		fc.j = 0.0;
		fc.k = -200.0;

		Camera.m_UpVector.i = 0.0;
		Camera.m_UpVector.j = 1.0;
		Camera.m_UpVector.k = 0.0;

		Camera.m_RightVector.i = 1.0;
		Camera.m_RightVector.j = 0.0;
		Camera.m_RightVector.k = 0.0;

		ftl = fc + (Camera.m_UpVector * (Hfar/2.0)) - (Camera.m_RightVector * (Wfar/2.0));
		ftr = fc + (Camera.m_UpVector * (Hfar/2.0)) + (Camera.m_RightVector * (Wfar/2.0));
		fbl = fc - (Camera.m_UpVector * (Hfar/2.0)) - (Camera.m_RightVector * (Wfar/2.0));
		fbr = fc - (Camera.m_UpVector * (Hfar/2.0)) + (Camera.m_RightVector * (Wfar/2.0));

	}

	void Combine_Matrix4(float MatrixA[16],float MatrixB[16], float *retM)
	{
		float NewMatrix[16];
		int i;

		for(i = 0; i < 4; i++){ //Cycle through each vector of first matrix.
			NewMatrix[i*4] = MatrixA[i*4] * MatrixB[0] + MatrixA[i*4+1] * MatrixB[4] + MatrixA[i*4+2] * MatrixB[8] + MatrixA[i*4+3] * MatrixB[12];
			NewMatrix[i*4+1] = MatrixA[i*4] * MatrixB[1] + MatrixA[i*4+1] * MatrixB[5] + MatrixA[i*4+2] * MatrixB[9] + MatrixA[i*4+3] * MatrixB[13];
			NewMatrix[i*4+2] = MatrixA[i*4] * MatrixB[2] + MatrixA[i*4+1] * MatrixB[6] + MatrixA[i*4+2] * MatrixB[10] + MatrixA[i*4+3] * MatrixB[14];
			NewMatrix[i*4+3] = MatrixA[i*4] * MatrixB[3] + MatrixA[i*4+1] * MatrixB[7] + MatrixA[i*4+2] * MatrixB[11] + MatrixA[i*4+3] * MatrixB[15];
		}
		memcpy(retM,NewMatrix,64);
	}

	void Inverse_Matrix4(float m[16], float *ret)
	{

		float inv[16]; // The inverse will go here

		inv[0] = m[0];
		inv[1] = m[4];
		inv[2] = m[8];
		inv[4] = m[1];
		inv[5] = m[5];
		inv[6] = m[9];
		inv[8] = m[2];
		inv[9] = m[6];
		inv[10] = m[10];

		inv[12] = inv[0]*-m[12]+inv[4]*-m[13]+inv[8]*-m[14];
		inv[13] = inv[1]*-m[12]+inv[5]*-m[13]+inv[9]*-m[14];
		inv[14] = inv[2]*-m[12]+inv[6]*-m[13]+inv[10]*-m[14];

		inv[3] = 0.0f;
		inv[7] = 0.0f;
		inv[11] = 0.0f;
		inv[15] = 1.0f;

		memcpy(ret,inv,64);
	}

	GLvoid Initialize(GLvoid)
	{
		glShadeModel(GL_SMOOTH);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glClearStencil(1);
		glDepthFunc(GL_LEQUAL);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable(GL_NORMALIZE); //normalize all normals

		glewInit();

		writeGBuffer.LoadVertexShader("Shaders/vert.g_buffer.glsl"); // shader for writing the G buffer
		writeGBuffer.LoadFragmentShader("Shaders/frag.g_buffer.glsl");
		writeGBuffer.LinkShaders();

		applyLight.LoadVertexShader("Shaders/vert.apply_light.glsl"); // shader for applying the light buffer
		applyLight.LoadFragmentShader("Shaders/frag.apply_light.glsl");
		applyLight.LinkShaders();

		pointLight.LoadVertexShader("Shaders/vert.point_light.glsl"); // shader for applying a point light
		pointLight.LoadFragmentShader("Shaders/frag.point_light.glsl");
		pointLight.LinkShaders();

		globalLight.LoadVertexShader("Shaders/vert.global_lighting.glsl"); // shader for applying a global light
		globalLight.LoadFragmentShader("Shaders/frag.global_lighting.glsl");
		globalLight.LinkShaders();

		writeShadowMap.LoadVertexShader("Shaders/vert.shadowmap.glsl"); // shader for writing the shadow map
		writeShadowMap.LoadFragmentShader("Shaders/frag.shadowmap.glsl");
		writeShadowMap.LinkShaders();

		shadow_gaussianX.LoadVertexShader("Shaders/vert.minimal.glsl"); // shader for the Shadow Y-Pass
		shadow_gaussianX.LoadFragmentShader("Shaders/frag.shadow_gaussian_x.glsl");
		shadow_gaussianX.LinkShaders();

		shadow_gaussianY.LoadVertexShader("Shaders/vert.minimal.glsl"); // shader for the Shadow Y-Pass
		shadow_gaussianY.LoadFragmentShader("Shaders/frag.shadow_gaussian_y.glsl");
		shadow_gaussianY.LinkShaders();

		ssao_gaussianX.LoadVertexShader("Shaders/vert.minimal.glsl"); // shader for the SSAO X-Pass
		ssao_gaussianX.LoadFragmentShader("Shaders/frag.ssao_gaussian_x.glsl");
		ssao_gaussianX.LinkShaders();

		ssao_gaussianY.LoadVertexShader("Shaders/vert.minimal.glsl"); // shader for the SSAO Y-Pass
		ssao_gaussianY.LoadFragmentShader("Shaders/frag.ssao_gaussian_y.glsl");
		ssao_gaussianY.LinkShaders();

		motionBlur.LoadVertexShader("Shaders/vert.minimal.glsl"); // shader for the SSAO X-Pass
		motionBlur.LoadFragmentShader("Shaders/frag.motion_blur.glsl");
		motionBlur.LinkShaders();

		DOF_x.LoadVertexShader("Shaders/vert.minimal.glsl"); 
		DOF_x.LoadFragmentShader("Shaders/frag.DOF_x.glsl");
		DOF_x.LinkShaders();

		DOF_y.LoadVertexShader("Shaders/vert.minimal.glsl"); 
		DOF_y.LoadFragmentShader("Shaders/frag.DOF_y.glsl");
		DOF_y.LinkShaders();

		displayDepth.LoadVertexShader("Shaders/vert.minimal.glsl"); 
		displayDepth.LoadFragmentShader("Shaders/frag.display_depth.glsl");
		displayDepth.LinkShaders();

		currentModel = new Model("Models/russian_church.3ds");
		currentModel->CreateVBO();

		lightManager.Initialize(&shadowMap, textureMatrix, invViewMatrix);
		lightManager.ReadLights();

		lightAccum.Bind(); // clear the stencil initially
		glClear(GL_STENCIL_BUFFER_BIT);
		FBO::Unbind();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(80.0,(float)windowWidth/windowHeight,0.1f,200);

		glGetFloatv(GL_PROJECTION_MATRIX, proj3DMatrix);

		glLoadIdentity();	
		gluOrtho2D(0,windowWidth,0,windowHeight);

		glGetFloatv(GL_PROJECTION_MATRIX, proj2DMatrix);

	}

	// Render all graphics!

	GLvoid Draw(GLvoid)
	{

		go3D();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		Camera.MoveCamera();
		Camera.SetCamera(); //set the direction you're looking

		/************** Write G buffer *****************/

		G_buffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(writeGBuffer.getProgram());

		float matDiff[4] = {0.7, 0.7, 0.7, 0.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matDiff);

		glBegin(GL_QUADS); //draw the floor!
		glNormal3f(0.0, 1.0, 0.0);  glVertex3f(-100.0, 0.0, 100.0);
		glVertex3f(100.0, 0.0, 100.0);
		glVertex3f(100.0, 0.0, -100.0);
		glVertex3f(-100.0, 0.0, -100.0);
		glEnd();

		for (int i = 0; i < 16; i++) { 
			prevCameraViewMatrix[i] = cameraViewMatrix[i];
		}
		glGetFloatv(GL_MODELVIEW_MATRIX, cameraViewMatrix);
		glTranslatef(-50.0, 39.5, 30.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0); // rotate because 3ds coords are different	

		currentModel->Draw(false);

		glUseProgram(0);
		FBO::Unbind();

		/************** Write Shadowmap *****************/

		glViewport(0, 0, windowWidth, windowHeight);
		shadowMap.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);

		glGetFloatv(GL_PROJECTION_MATRIX, shadowProjMatrix);
		glMatrixMode(GL_MODELVIEW);

		glUseProgram(writeShadowMap.getProgram());

		glLoadIdentity();

		if (FPS.GetLastFrameRate() != 0.0) {
			rotateAngle += (1.0/FPS.GetLastFrameRate())*0.5;
		}

		shadowCamera.SetCamera(); // set global light position
		glGetFloatv(GL_MODELVIEW_MATRIX, lightViewMatrix);

		glBegin(GL_QUADS); //draw the floor!
		glNormal3f(0.0, 1.0, 0.0);  glVertex3f(-100.0, 0.0, 100.0);
		glVertex3f(100.0, 0.0, 100.0);
		glVertex3f(100.0, 0.0, -100.0);
		glVertex3f(-100.0, 0.0, -100.0);
		glEnd();

		glTranslatef(-50.0, 39.5, 30.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);	

		currentModel->Draw(false);
		glUseProgram(0);

		FBO::Unbind();

		glMatrixMode(GL_MODELVIEW);	// Reset the modelview
		glLoadIdentity();

		/************** Log Filter Shadowmap *****************/
		//shadowMap.drawFBO(windowWidth, windowHeight, "shadow map");
		
		if (softShadowsEnabled == 1) {

				shadowMap.BindTexture("shadow map", 2);
				shadowMap2.Bind();

				glUseProgram(shadow_gaussianX.getProgram());
				shadow_gaussianX.SetUniform1i("theTexture", 2);
				shadow_gaussianX.SetUniform1f("step_x", 1.0/float(windowWidth*1.0));
				shadow_gaussianX.SetUniform1i("kernelSize", 4);

				drawFullscreenQuad();
	
				FBO::Unbind();
				glUseProgram(0);

				shadowMap2.BindTexture("shadow map", 1);

				shadowMap.Bind();
				glUseProgram(shadow_gaussianY.getProgram());
				shadow_gaussianY.SetUniform1i("theTexture", 1);
				shadow_gaussianY.SetUniform1f("step_y", 1.0/float(windowHeight*1.0));
				shadow_gaussianY.SetUniform1i("kernelSize", 4);
				drawFullscreenQuad();
				FBO::Unbind();
				glUseProgram(0);

		}

		glViewport(0, 0, windowWidth, windowHeight);

		//shadowMap.GenMipMaps(); // lets not do mipmaps for now

		/********************************
		*     Matrix Calculations       *
		********************************/

		float prevInvViewMatrix[16];

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.5f,0.5f,0.5f);
		glScalef(0.5f,0.5f,0.5f);
		glMultMatrixf(shadowProjMatrix);
		glMultMatrixf(lightViewMatrix);

		glGetFloatv(GL_TEXTURE_MATRIX, textureMatrix);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);

		Inverse_Matrix4(cameraViewMatrix,invViewMatrix);
		Inverse_Matrix4(prevCameraViewMatrix,prevInvViewMatrix);

		ComputeFrustumCoords();

		/********************************
		*         Light Shading         *
		********************************/

		go3D();

		glMatrixMode(GL_MODELVIEW);
		Camera.SetCamera();

		lightAccum.Bind();
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColorMask(0,0,0,0);

		glBegin(GL_QUADS); //draw the floor!
		glNormal3f(0.0, 1.0, 0.0);  glVertex3f(-100.0, 0.0, 100.0);
		glVertex3f(100.0, 0.0, 100.0);
		glVertex3f(100.0, 0.0, -100.0);
		glVertex3f(-100.0, 0.0, -100.0);
		glEnd();

		glTranslatef(-50.0, 39.5, 30.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);	

		currentModel->Draw(false);
		glColorMask(1,1,1,1);

		FBO::Unbind();

		//****** Render lights to accumulation buffer ******/

		G_buffer.BindTexture("normal texture", 2);
		G_buffer.BindTexture("specular texture", 3);
		G_buffer.BindTexture("diffuse texture", 1);

		lightManager.DrawLightsIntoBuffer(&lightAccum, Camera.GetXPosition(), Camera.GetYPosition(), Camera.GetZPosition());

		//****** Combine light accumulation buffer with G buffer ******/

		result.Bind();
		G_buffer.BindTexture("diffuse texture", 2);
		lightAccum.BindTexture("scene", 1);
		G_buffer.BindTexture("normal texture", 3);

		glUseProgram(applyLight.getProgram());
		applyLight.SetUniform1i("lightMap", 1);
		applyLight.SetUniform1i("diffuseTexture", 2);
		applyLight.SetUniform1i("depthTexture", 3);
		lightAccum.drawFBO(windowWidth, windowHeight, "scene");
		glUseProgram(0);

		FBO::Unbind();

		//****** Depth Darkening ******/

		if (depthDarkeningEnabled == 1) {

			G_buffer.BindTexture("normal texture", 1);

			glUseProgram(ssao_gaussianX.getProgram());
			ssao_gaussianX.SetUniform1i("theTexture", 1);
			ssao_gaussianX.SetUniform1f("xSize", windowWidth);

			ping.Bind();
			drawFullscreenQuad();
			FBO::Unbind();

			pang.Bind();

			ping.BindTexture("scene", 1);
			result.BindTexture("scene", 3);
			G_buffer.BindTexture("normal texture", 2); // pass in depth (its in the normal map)
			G_buffer.BindTexture("diffuse texture", 4);

			glUseProgram(ssao_gaussianY.getProgram());
			ssao_gaussianY.SetUniform1i("theTexture", 1);
			ssao_gaussianY.SetUniform1i("original", 2);
			ssao_gaussianY.SetUniform1i("litScene", 3);
			ssao_gaussianY.SetUniform1i("diffuse", 4);
			ssao_gaussianY.SetUniform1f("ySize", windowHeight);

			drawFullscreenQuad();
			glUseProgram(0);
			FBO::Unbind();

			result.Bind();
			pang.drawFBO(windowWidth, windowHeight, "scene");
			FBO::Unbind();

		}

		//****** Motion Blur ******/

		if (motionBlurEnabled == 1) {
	
			result.BindTexture("scene", 1);

			G_buffer.BindTexture("normal texture", 2);
			G_buffer.BindTexture("specular texture", 3);

			ping.Bind();

			glUseProgram(motionBlur.getProgram());
			motionBlur.SetUniform1i("depthTex", 2);
			motionBlur.SetUniform1f("sizeX", 1.0/windowWidth);
			motionBlur.SetUniform1f("sizeY", 1.0/windowHeight);
			motionBlur.SetUniform1i("currentScene", 1);
			motionBlur.SetUniformMat("invViewMatrix", invViewMatrix);
			motionBlur.SetUniformMat("prevInvViewMatrix", prevInvViewMatrix);

			drawFullscreenQuad();

			glUseProgram(0);
			FBO::Unbind();

			result.Bind();
			ping.drawFBO(windowWidth, windowHeight, "scene");
			FBO::Unbind();

		}

		//****** Depth Of Field ******/

		if (depthOfFieldEnabled == 1) {

			pong.Bind();

			result.BindTexture("scene", 3);

			glUseProgram(DOF_x.getProgram());
			DOF_x.SetUniform1i("linearDepth", 2);
			DOF_x.SetUniform1i("currentFrame", 3);
			DOF_x.SetUniform1f("stepX", 1.0/windowWidth);
			DOF_x.SetUniform1i("kernelSize", 3);

			drawFullscreenQuad();

			glUseProgram(0);

			FBO::Unbind();

			ping.Bind();

			pong.BindTexture("scene", 3);

			glUseProgram(DOF_y.getProgram());
			DOF_y.SetUniform1i("linearDepth", 2);
			DOF_y.SetUniform1i("currentFrame", 3);
			DOF_y.SetUniform1f("stepY", 1.0/windowWidth);
			DOF_y.SetUniform1i("kernelSize", 3);

			drawFullscreenQuad();
			glUseProgram(0);

			result.Bind();
			ping.drawFBO(windowWidth, windowHeight, "scene");
			FBO::Unbind();

		}

		if (displayedTexture ==  "Result")
			result.drawFBO(windowWidth, windowHeight, "scene");
		if (displayedTexture ==  "Diffuse")
			G_buffer.drawFBO(windowWidth, windowHeight, "diffuse texture");
		else if (displayedTexture ==  "Normal")
			G_buffer.drawFBO(windowWidth, windowHeight, "normal texture");
		else if (displayedTexture ==  "Specular")
			G_buffer.drawFBO(windowWidth, windowHeight, "specular texture");
		else if (displayedTexture ==  "Depth") {
			G_buffer.BindTexture("normal texture", 2);
			glUseProgram(displayDepth.getProgram());
			displayDepth.SetUniform1i("depthTexture", 2);
			drawFullscreenQuad();
			glUseProgram(0);
		} else if (displayedTexture ==  "Shadowmap")
			shadowMap.drawFBO(windowWidth, windowHeight, "shadow map");
		else if (displayedTexture ==  "Lightmap")
			lightAccum.drawFBO(windowWidth, windowHeight, "scene");

		ping.Bind();
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		FBO::Unbind();

		FPS.EndFrame();
		DisplayTimer(FPS.GetLastFrameRate(),windowWidth, windowHeight);
		FPS.StartFrame();

	}

	// NOTE: Resizing doesnt work

	GLvoid ResizeGLScene(const GLsizei width, const GLsizei height)		
	{
		windowWidth = width;
		windowHeight = height;



		glViewport(0, 0, width, height);
		// Reset the projection and modelview matrix
		glMatrixMode(GL_PROJECTION);
		//glLoadMatrixf(proj3DMatrix);
		glLoadIdentity();
		gluPerspective(80.0,(float)windowWidth/windowHeight,0.1f,200);
		glGetFloatv(GL_PROJECTION_MATRIX, proj3DMatrix);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		if (alreadyInit != true) {
			G_buffer.AddTexture("diffuse texture", GL_RGBA, width, height, 0);
			G_buffer.AddTexture("normal texture", GL_RGBA16, width, height, 0);
			G_buffer.AddTexture("specular texture", GL_RGBA, width, height, 0);
			G_buffer.AddDepthBuffer(width, height);

			lightAccum.AddTexture("scene", GL_RGBA32F_ARB, width, height, 0);
			lightAccum.AddStencilBuffer(width, height);

			shadowMap.AddTexture("shadow map", GL_RGBA16, width, height, 1);
			shadowMap2.AddTexture("shadow map", GL_RGBA16, width, height, 1);
			shadowMap.AddDepthBuffer(width, height);

			result.AddTexture("scene", GL_RGBA32F_ARB, width, height, 0);

			ping.AddTexture("scene", GL_RGBA32F_ARB, width, height, 0);
			pong.AddTexture("scene", GL_RGBA16, width, height, 0);
			pang.AddTexture("scene", GL_RGBA16, width, height, 0);			

			alreadyInit = true;
		}

	}

	GLvoid UpdateViewPitch(const GLfloat newPitch)
	{
		Camera.ChangePitch(10.0 * newPitch*FPS.GetLastFrameTime());
	}

	GLvoid UpdateViewHeading(const GLfloat newHeading)
	{
		Camera.ChangeHeading(10.0 * newHeading*FPS.GetLastFrameTime());
	}

	GLvoid UpdateViewVelocity(const GLfloat newVelocity)
	{
		Camera.ChangeVelocity(newVelocity*FPS.GetLastFrameTime());
	}

	GLvoid setDisplayedTexture(string textureName)
	{
		displayedTexture = textureName;
	}

	void SetDepthDarkening(int enabled)
	{
		depthDarkeningEnabled = enabled;
	}

	void SetSoftShadows(int enabled)
	{
		softShadowsEnabled = enabled;
	}

	void SetMotionBlur(int enabled)
	{
		motionBlurEnabled = enabled;
	}

	void SetDepthOfField(int enabled)
	{
		depthOfFieldEnabled = enabled;
	}

	void setDepth(int mode)
	{
		Depth_mode = mode;
	}


}
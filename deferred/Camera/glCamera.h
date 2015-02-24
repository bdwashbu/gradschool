
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "glQuaternion.h"
#include "glPoint.h"
#include "glVector.h"

#include <windows.h>
#include <gl\gl.h>

class glCamera  
{
private:
	GLdouble m_MaxPitchRate;
	GLdouble m_MaxHeadingRate;
	GLdouble m_HeadingDegrees;
	GLdouble m_PitchDegrees;
	GLdouble m_ForwardVelocity;
	glQuaternion m_qHeading;
	
	bool mirror;
	bool render;
public:
	glVector m_DirectionVector;
	glVector m_UpVector;
	glVector m_RightVector;
	glPoint m_Position;
	glQuaternion m_qPitch;
	void CreateUpVector();
	void CreateRightVector();
	void ChangeVelocity(GLfloat vel);
	void ChangeHeading(GLfloat degrees);
	void ChangePitch(GLfloat degrees);
	void MoveCamera(void);
	void DrawDirectionArrow();
	void SetMirrorPrespective(void);
	void SetCamera(void);
	float GetXPosition(void);
	float GetYPosition(void);
	float GetZPosition(void);
	glCamera(GLfloat defaultHeading, GLfloat defaultPitch, GLfloat xPos, GLfloat yPos, GLfloat zPos);
	//glCamera(glPoint *objectPos);
	virtual ~glCamera();
};

#endif 

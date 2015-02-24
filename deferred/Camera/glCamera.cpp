#include "Camera\glPoint.h"
#include "Camera\glVector.h"
#include "Camera\glCamera.h"
#include <math.h>
#include <stdio.h>

glCamera::glCamera(GLfloat defaultHeading, GLfloat defaultPitch, GLfloat xPos, GLfloat yPos, GLfloat zPos)
{
	// Initalize all our member varibles.
	m_MaxPitchRate			= 5.0;
	m_MaxHeadingRate		= 5.0;
	m_HeadingDegrees		= defaultHeading;
	m_PitchDegrees			= defaultPitch;
	m_ForwardVelocity		= 0.0;
	m_Position.x = xPos;
	m_Position.y = yPos;
	m_Position.z = zPos;
	render = false;
}

glCamera::~glCamera()
{	
}

void glCamera::SetCamera(void)
{
	glQuaternion q;
	glQuaternion pitch2;
	glQuaternion heading2;
	GLfloat Matrix[16];

	pitch2.CreateFromAxisAngle(1.0f, 0.0f, 0.0f, m_PitchDegrees);
	heading2.CreateFromAxisAngle(0.0f, 1.0f, 0.0f, m_HeadingDegrees);

	q = pitch2 * heading2;
	q.CreateMatrix(Matrix);
	glMultMatrixf(Matrix);

	glTranslatef(-m_Position.x, -m_Position.y, m_Position.z);
	//printf("%f %f %f\n", m_Position.x, m_Position.y, m_Position.z);
	//printf("pitch: %f heading:%f\n", m_PitchDegrees, m_HeadingDegrees);

}

void glCamera::DrawDirectionArrow()
{
	glBegin(GL_LINES);
		glVertex3f(m_Position.x, m_Position.y, m_Position.z);
		glVertex3f(m_Position.x+m_DirectionVector.i*1.5f, m_Position.y+m_DirectionVector.j*1.5f, m_Position.z+m_DirectionVector.k*1.5f);
	glEnd();
}

void glCamera::MoveCamera()
{
	GLfloat Matrix[16];
	glQuaternion q;

	m_qPitch.CreateFromAxisAngle(1.0f, 0.0f, 0.0f, m_PitchDegrees);
	m_qHeading.CreateFromAxisAngle(0.0f, 1.0f, 0.0f, m_HeadingDegrees);
	
	q = m_qPitch * m_qHeading;
	q.CreateMatrix(Matrix);

	m_qPitch.CreateMatrix(Matrix);
	m_DirectionVector.j = Matrix[9];

	q = m_qHeading * m_qPitch;
	q.CreateMatrix(Matrix);
	m_DirectionVector.i = Matrix[8];
	m_DirectionVector.k = Matrix[10];
	m_DirectionVector.Normalize();

	m_Position.x += m_DirectionVector.i * m_ForwardVelocity;
	m_Position.y += m_DirectionVector.j * m_ForwardVelocity;
	m_Position.z += m_DirectionVector.k * m_ForwardVelocity;

	CreateUpVector();
	//CreateRightVector();
	m_RightVector.i = m_UpVector.j * m_DirectionVector.k - m_UpVector.k * m_DirectionVector.j;
	m_RightVector.j = m_UpVector.k * m_DirectionVector.i - m_UpVector.i * m_DirectionVector.k;
	m_RightVector.k = m_UpVector.i * m_DirectionVector.j - m_UpVector.j * m_DirectionVector.i;

	m_ForwardVelocity = 0.0f;
}

void glCamera::CreateUpVector()
{
	GLfloat Matrix[16];
	glQuaternion q;

	m_qPitch.CreateFromAxisAngle(1.0f, 0.0f, 0.0f, m_PitchDegrees+90.0f);
	m_qHeading.CreateFromAxisAngle(0.0f, 1.0f, 0.0f, m_HeadingDegrees);
	
	q = m_qPitch * m_qHeading;
	q.CreateMatrix(Matrix);

	m_qPitch.CreateMatrix(Matrix);
	m_UpVector.j = Matrix[9];

	q = m_qHeading * m_qPitch;
	q.CreateMatrix(Matrix);
	m_UpVector.i = Matrix[8];
	m_UpVector.k = Matrix[10];
	m_UpVector.Normalize();
}

void glCamera::CreateRightVector()
{
	GLfloat Matrix[16];
	glQuaternion q;

	m_qPitch.CreateFromAxisAngle(1.0f, 0.0f, 0.0f, m_PitchDegrees);
	m_qHeading.CreateFromAxisAngle(0.0f, 1.0f, 0.0f, m_HeadingDegrees-90.0);
	
	q = m_qPitch * m_qHeading;
	q.CreateMatrix(Matrix);

	m_qPitch.CreateMatrix(Matrix);
	m_RightVector.j = Matrix[9];

	q = m_qHeading * m_qPitch;
	q.CreateMatrix(Matrix);
	m_RightVector.i = Matrix[8];
	m_RightVector.k = Matrix[10];
	m_RightVector.Normalize();

}

void glCamera::ChangePitch(GLfloat degrees)
{
	if(fabs(degrees) < fabs(m_MaxPitchRate))
	{
		// Our pitch is less than the max pitch rate that we 
		// defined so lets increment it.
		m_PitchDegrees += degrees;
	}
	else
	{
		// Our pitch is greater than the max pitch rate that
		// we defined so we can only increment our pitch by the 
		// maximum allowed value.
		if(degrees < 0)
		{
			// We are pitching down so decrement
			m_PitchDegrees -= m_MaxPitchRate;
		}
		else
		{
			// We are pitching up so increment
			m_PitchDegrees += m_MaxPitchRate;
		}
	}

	// We don't want our pitch to run away from us. Although it
	// really doesn't matter I prefer to have my pitch degrees
	// within the range of -360.0f to 360.0f
	if(m_PitchDegrees > 360.0f)
	{
		m_PitchDegrees -= 360.0f;
	}
	else if(m_PitchDegrees < -360.0f)
	{
		m_PitchDegrees += 360.0f;
	}
}

void glCamera::ChangeHeading(GLfloat degrees)
{
	if(fabs(degrees) < fabs(m_MaxHeadingRate))
	{
		// Our Heading is less than the max heading rate that we 
		// defined so lets increment it but first we must check
		// to see if we are inverted so that our heading will not
		// become inverted.
		if(m_PitchDegrees > 90 && m_PitchDegrees < 270 || (m_PitchDegrees < -90 && m_PitchDegrees > -270))
		{
			m_HeadingDegrees -= degrees;
		}
		else
		{
			m_HeadingDegrees += degrees;
		}
	}
	else
	{
		// Our heading is greater than the max heading rate that
		// we defined so we can only increment our heading by the 
		// maximum allowed value.
		if(degrees < 0)
		{
			// Check to see if we are upside down.
			if((m_PitchDegrees > 90 && m_PitchDegrees < 270) || (m_PitchDegrees < -90 && m_PitchDegrees > -270))
			{
				// Ok we would normally decrement here but since we are upside
				// down then we need to increment our heading
				m_HeadingDegrees += m_MaxHeadingRate;
			}
			else
			{
				// We are not upside down so decrement as usual
				m_HeadingDegrees -= m_MaxHeadingRate;
			}
		}
		else
		{
			// Check to see if we are upside down.
			if(m_PitchDegrees > 90 && m_PitchDegrees < 270 || (m_PitchDegrees < -90 && m_PitchDegrees > -270))
			{
				// Ok we would normally increment here but since we are upside
				// down then we need to decrement our heading.
				m_HeadingDegrees -= m_MaxHeadingRate;
			}
			else
			{
				// We are not upside down so increment as usual.
				m_HeadingDegrees += m_MaxHeadingRate;
			}
		}
	}
	
	// We don't want our heading to run away from us either. Although it
	// really doesn't matter I prefer to have my heading degrees
	// within the range of -360.0f to 360.0f
	if(m_HeadingDegrees > 360.0f)
	{
		m_HeadingDegrees -= 360.0f;
	}
	else if(m_HeadingDegrees < -360.0f)
	{
		m_HeadingDegrees += 360.0f;
	}
}

void glCamera::ChangeVelocity(GLfloat vel)
{
		m_ForwardVelocity = vel;
}

float glCamera::GetXPosition(void)
{
	return m_Position.x;
}
float glCamera::GetYPosition(void)
{
	return m_Position.y;
}

float glCamera::GetZPosition(void)
{
	return m_Position.z;
}

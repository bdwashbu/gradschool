#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#define PI			3.14159265358979323846


class glQuaternion  
{
public:
	glQuaternion operator *(glQuaternion q);
	void CreateMatrix(float *pMatrix);
	void CreateFromAxisAngle(float x, float y, float z, float degrees);
	glQuaternion();
	virtual ~glQuaternion();
	float m_z;
	float m_y;
	float m_x;
private:
	float m_w;
	
};

#endif 

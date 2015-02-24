#ifndef _POINT_H_
#define _POINT_H_

class glPoint  
{
public:
	glPoint();
	glPoint(float x1, float y1, float z1);
	virtual ~glPoint();

	float z;
	float y;
	float x;
};

#endif

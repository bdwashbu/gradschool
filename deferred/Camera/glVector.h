#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "glPoint.h"

class glVector  
{
public:
	void operator *=(double scalar);
	glVector operator * (double scalar);
	glVector operator + (glVector v2);
	glVector operator - (glVector v2);
	glVector operator + (glPoint point);
	glVector();
	virtual ~glVector();
	void Normalize();

	double k;
	double j;
	double i;
};

#endif

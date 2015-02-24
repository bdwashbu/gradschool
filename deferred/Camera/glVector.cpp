#include "glVector.h"
#include <math.h>

glVector::glVector()
{
	i = j = k = 0.0f;
}

glVector::~glVector()
{

}

void glVector::operator *=(double scalar)
{
	i *= scalar;
	j *= scalar;
	k *= scalar;
}

glVector glVector::operator * (double scalar)
{
	glVector temp;
	temp.i = i * scalar;
	temp.j = j * scalar;
	temp.k = k * scalar;
	return temp;
}

glVector glVector::operator + (glVector v2)
{
	glVector temp;
	temp.i = i + v2.i;
	temp.j = j + v2.j;
	temp.k = k + v2.k;
	return temp;
}

glVector glVector::operator - (glVector v2)
{
	glVector temp;
	temp.i = i - v2.i;
	temp.j = j - v2.j;
	temp.k = k - v2.k;
	return temp;
}

glVector glVector::operator + (glPoint point)
{
	glVector temp;
	temp.i = i + point.x;
	temp.j = j + point.y;
	temp.k = k + point.z;
	return temp;
}

void glVector::Normalize()
{
	float length = sqrtf(k*k+j*j+i*i);
	i /= length;
	j /= length;
	k /= length;
}
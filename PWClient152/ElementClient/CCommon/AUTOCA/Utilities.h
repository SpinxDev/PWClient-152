#ifndef _UTILITIES_H_
#define _UTILITIES_H_


#include <stdlib.h>   // for rand, etc.
#include <float.h>    // for FLT_MAX, etc.
#include <math.h>     // for sqrt, etc.
#include "A3D.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4305)


#ifndef PI
#define PI 3.14159265358979323846f
#endif


template<class T> inline T interpolate (float alpha, const T& x0, const T& x1)
{
	return x0 + ((x1 - x0) * alpha);
}

inline float frandom01 (void)
{
	return (((float) rand ()) / ((float) RAND_MAX));
}

inline A3DVECTOR3 randomVectorInUnitSphere()
{
	return A3DVECTOR3((frandom01()-0.5), (frandom01()-0.5), (frandom01()-0.5));
}

inline A3DVECTOR3 randomUnitVector()
{
	A3DVECTOR3 v = randomVectorInUnitSphere();
	v.Normalize();
	return v;		
}

inline float clip (float x, float min, float max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

inline A3DVECTOR3 clip (const A3DVECTOR3& v, float min, float max)
{
	A3DVECTOR3 ret = v;
	float l = ret.Normalize();
	if(l>max)	return ret*max;
	if(l<min)	return ret*min;
	return v;
}

inline int intervalComparison (float x, float lowerBound, float upperBound)
{
	if (x < lowerBound) return -1;
	if (x > upperBound) return +1;
	return 0;
}

inline float scalarRandomWalk (const float initial, 
							   const float walkspeed,
							   const float min,
							   const float max)
{
	float next = initial + (((frandom01() * 2) - 1) * walkspeed);
	if (next < min) return min;
	if (next > max) return max;
	return next;
}


inline A3DVECTOR3 parallelComponent (const A3DVECTOR3& v, const A3DVECTOR3& unitBasis)
{
	const float projection = DotProduct(v, unitBasis);
	return unitBasis * projection;
}

inline A3DVECTOR3 perpendicularComponent (const A3DVECTOR3& v, const A3DVECTOR3& unitBasis)
{
	return v - parallelComponent(v, unitBasis);
}

inline A3DVECTOR3 truncateLength (const A3DVECTOR3& v, float length)
{
	return clip(v, 0, length);
}

template<class T>
inline void blendIntoAccumulator (const float smoothRate,
								  const T& newValue,
								  T& smoothedAccumulator)
{
	smoothedAccumulator = interpolate (clip (smoothRate, 0, 1),
		smoothedAccumulator,
		newValue);
}

#endif

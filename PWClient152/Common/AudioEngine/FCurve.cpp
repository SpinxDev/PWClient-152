#include "FCurve.h"
#include <cmath>

using namespace AudioEngine;

#define AE_PI	3.1415926f

Curve::Curve(void)
{
	m_CurveType = CURVE_LINEAR;
}

Curve::~Curve(void)
{
}

CURVE_TYPE Curve::GetType() const
{
	return m_CurveType;
}

void Curve::SetType(CURVE_TYPE type)
{
	m_CurveType = type;
}

float Curve::GetValue(float x, bool bIncreasing) const
{
	switch(m_CurveType)
	{
	case CURVE_LINEAR:
		return getLinearValue(x, bIncreasing);
	case CURVE_SIN:
		return getSinValue(x, bIncreasing);
	case CURVE_COS:
		return getCosValue(x, bIncreasing);
	case CURVE_VERT_SIN:
		return getVertSinValue(x, bIncreasing);
	case CURVE_POW:
		return getPowValue(x, bIncreasing);
	}
	return 0;
}

float Curve::GetValue(float sx, float sy, float ex, float ey, float x) const
{
	x = (x-sx)/(ex-sx);
	float val = GetValue(x, ey>sy);
	if (ey>sy)
		return (ey-sy)*val+sy;
	return (sy-ey)*val+ey;
}

float Curve::getLinearValue(float x, bool bIncreasing) const
{
	if(x < 0)
		x = 0;
	if(x > 1)
		x = 1;

	if(bIncreasing)
		return x;

	return 1-x;
}

float Curve::getSinValue(float x, bool bIncreasing) const
{
	if(x < 0)
		x = 0;
	if(x > 1)
		x = 1;

	if(!bIncreasing)
		return 0.5f * cosf(AE_PI * x ) + 0.5f;
	else
		return 0.5f * cosf(AE_PI * x + AE_PI) + 0.5f;
}

float Curve::getCosValue(float x, bool bIncreasing) const
{
	if(x < 0)
		x = 0;
	if(x > 1)
		x = 1;

	if(bIncreasing)
	{
		return sinf(AE_PI*0.5f*x);
	}
	else
	{
		return cosf(AE_PI*0.5f*x);
	}
}

float Curve::getVertSinValue(float x, bool bIncreasing) const
{
	if(x < 0)
		x = 0;
	if(x > 1)
		x = 1;

	if (bIncreasing)
		return 0.5f - asin((x-0.5f)/0.5f)/(-AE_PI);
	else
		return 1 - acos((-x+0.5f)/0.5f)/AE_PI;
}

float Curve::getPowValue(float x, bool bIncreasing) const
{
	float k = 1.0f;
	float c = 0;

	float a = 0;
	float b = 1.0f;
	if(bIncreasing)
	{
		a = 1.0f;
		b *= -1;
	}

	return k * powf(0.1f, 2*(x-a)/b) + c;
}
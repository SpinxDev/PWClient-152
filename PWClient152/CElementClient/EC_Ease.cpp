// File		: EC_Ease.cpp
// Creator	: Xu Wenbin
// Date		: 2013/10/11

#include "EC_Ease.h"

#include <cmath>

#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923

namespace Ease
{
	//	以下加速度效果参考自 cocos2d-x 	
	float ExponentialIn(float t)
	{
		return t == 0 ? 0 : powf(2, 10 * (t - 1)) - 0.001f;
	}
	
	float ExponentialOut(float t)
	{
		return t == 1 ? 1 : (-powf(2, -10 * t) + 1);
	}
	float ExponentialInOut(float t)
	{
		t /= 0.5f;
		if (t < 1)
		{
			t = 0.5f * powf(2, 10 * (t - 1));
		}
		else
		{
			t = 0.5f * (-powf(2, -10 * (t - 1)) + 2);
		}
		return t;
	}
	
	float SineIn(float t)
	{
		return -1 * cosf(t * (float)M_PI_2) + 1;
	}
	float SineOut(float t)
	{
		return sinf(t * (float)M_PI_2);
	}
	float SineInOut(float t)
	{
		return -0.5f * (cosf((float)M_PI * t) - 1);
	}
}
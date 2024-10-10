#ifndef _GFXCOMMONTYPES_H_
#define _GFXCOMMONTYPES_H_

#include "vector.h"

typedef __int64 clientid_t;
struct model_handle;
typedef model_handle* modelh_t;

struct GfxPixelShaderTargetValue
{
	DWORD			interval;
	A3DCOLORVALUE	value;
};

typedef abase::vector<GfxPixelShaderTargetValue> GfxPixelShaderTargetValueVec;

struct GfxPixelShaderConst
{
	int								index;			// pixel shader const index
	A3DCOLORVALUE					init_val;		// initial value
	int								loop_count;
	DWORD							total_time;
	GfxPixelShaderTargetValueVec	target_vals;	// target values

	GfxPixelShaderConst& operator = (const GfxPixelShaderConst& src)
	{
		index		= src.index;
		init_val	= src.init_val;
		loop_count	= src.loop_count;
		total_time	= src.total_time;
		const size_t sz = src.target_vals.size();
		target_vals.reserve(sz);

		for (size_t i = 0; i < sz; i++)
			target_vals.push_back(src.target_vals[i]);

		return *this;
	}

	GfxPixelShaderConst()
	{
	}

	GfxPixelShaderConst(const GfxPixelShaderConst& src)
	{
		*this = src;
	}

	void CalcTotalTime()
	{
		total_time = 0;
		const size_t sz = target_vals.size();

		for (size_t i = 0; i < sz; i++)
		{
			const GfxPixelShaderTargetValue& tar = target_vals[i];
			
			if (tar.interval == 0)
			{
				total_time = 0;
				break;
			}

			if (int(tar.interval) < 0)
				total_time -= tar.interval;
			else
				total_time += tar.interval;
		}
	}
};

typedef abase::vector<GfxPixelShaderConst> GfxPSConstVec;

struct A3DGFX_CAMERA_BLUR_INFO
{
	float	fFrameCo;	// Ä£ºýÏµÊý
	DWORD	dwFadeTime;
	float	fMaxPixelOffset;

	A3DGFX_CAMERA_BLUR_INFO()
	{
		Reset();		
	}

	A3DGFX_CAMERA_BLUR_INFO(const A3DGFX_CAMERA_BLUR_INFO& src)
	{
		*this = src;
	}

	A3DGFX_CAMERA_BLUR_INFO& operator = (const A3DGFX_CAMERA_BLUR_INFO& src)
	{
		fFrameCo = src.fFrameCo;
		dwFadeTime = src.dwFadeTime;
		fMaxPixelOffset = src.fMaxPixelOffset;
		return *this;
	}

	void Reset()
	{
		fFrameCo = 0;
		dwFadeTime = 0;
		fMaxPixelOffset = 0;
	}
};

#endif

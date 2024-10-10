#pragma once

#include "FExportDecl.h"

namespace AudioEngine
{
	typedef enum
	{
		CURVE_LINEAR,		
		CURVE_SIN,
		CURVE_COS,
		CURVE_VERT_SIN,
		CURVE_POW
	}CURVE_TYPE;	

	class _EXPORT_DLL_ Curve
	{
	public:
		Curve(void);
		virtual ~Curve(void);
	public:
		// x[0, 1]; y[0, 1]
		float				GetValue(float x, bool bIncreasing) const;

		// x[sx, ex]; y[sy, ey]
		float				GetValue(float sx, float sy, float ex, float ey, float x) const;
		CURVE_TYPE			GetType() const;
		void				SetType(CURVE_TYPE type);
	protected:
		float				getLinearValue(float x, bool bIncreasing) const;
		float				getSinValue(float x, bool bIncreasing) const;
		float				getCosValue(float x, bool bIncreasing) const;
		float				getVertSinValue(float x, bool bIncreasing) const;
		float				getPowValue(float x, bool bIncreasing) const;
	protected:
		CURVE_TYPE			m_CurveType;
	};
}

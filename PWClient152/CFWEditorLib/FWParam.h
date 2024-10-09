#ifndef _FWPARAM_H_
#define _FWPARAM_H_

#include "A3DGFXEditorInterface.h"
#include "FWConfig.h"
#include <AArray.h>
#include <AAssist.h>

// this is used for FWDialogParam
class FWParam
{
	GFX_PROPERTY m_Val;
	GFX_PROPERTY m_Min;
	GFX_PROPERTY m_Max;
	ACString m_Name;
public:
	FWParam(){}
	FWParam(const GFX_PROPERTY &val, const GFX_PROPERTY &min, const GFX_PROPERTY &max, const ACString &name)
	{
		m_Val = val;
		m_Min = min;
		m_Max = max;
		m_Name = name;
	}
	FWParam(const FWParam &src)
	{
		*this = src;
	}

	const GFX_PROPERTY & GetVal() const { return m_Val; }
	const GFX_PROPERTY & GetMin() const { return m_Min; }
	const GFX_PROPERTY & GetMax() const { return m_Max; }
	const ACString & GetName() const { return m_Name; }
	void SetVal(const GFX_PROPERTY & val) { m_Val = val; }
	GfxValueType GetType() { return m_Val.GetType(); }
};

typedef AArray<FWParam, FWParam &> FWParamArray;

#endif 
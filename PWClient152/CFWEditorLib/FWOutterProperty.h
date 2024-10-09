#ifndef _FWOUTTERPROPERTYLIST_H_
#define _FWOUTTERPROPERTYLIST_H_

#include "FWInfoSet.h"
#include "A3DGFXEditorInterface.h"
#include "CodeTemplate.h"
#include "FWParam.h"
#include "FWConfig.h"

class AScriptFile;

struct FWOutterProperty : public FWInfo
{
public:
	GFX_PROPERTY Data;
	GFX_PROPERTY DataMin;
	GFX_PROPERTY DataMax;
	FWOutterProperty(){}
	FWOutterProperty(const FWOutterProperty & src){ *this = src; }
	FWOutterProperty & operator = (const FWOutterProperty &src)
	{
		FWInfo::operator = (src);
		Data = src.Data;
		DataMin = src.DataMin;
		DataMax = src.DataMax;
		return *this;
	}
	IMPLEMENT_CLONE_INLINE(FWOutterProperty, FWInfo);
	operator GFX_PROPERTY() const { return Data; }
	bool Load(AScriptFile *pFile, int nID);
};


class FWOutterPropertyList : public FWInfoSet<FWOutterProperty>
{
public:
	FWOutterPropertyList();
	virtual ~FWOutterPropertyList();
	FWOutterPropertyList & operator = (const FWOutterPropertyList &src);
	bool Load(AScriptFile *pFile);
	void FillParamArray(FWParamArray &arrayParam) const;
	void UpdateFromParamArray(const FWParamArray &arrayParam);
};

#endif 
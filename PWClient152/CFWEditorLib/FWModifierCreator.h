#ifndef _FWMODIFIERCREATOR_H_
#define _FWMODIFIERCREATOR_H_

#include "FWInfoSet.h"
#include "CodeTemplate.h"
#include "FWConfig.h"

class FWModifier;
class FWRuntimeClass;

struct FWModifierInfo : public FWInfo
{
	const FWRuntimeClass * pClass;
};

class FWModifierCreator : public FWInfoSet<FWModifierInfo>
{
	DECLARE_SINGLETON(FWModifierCreator)
protected:
	FWModifierCreator();
	virtual ~FWModifierCreator();

	bool m_bInited;
public:
	enum ID_MODIFIER 
	{
		ID_MODIFIER_DEFAULT = 0,
		ID_MODIFIER_PARABOLA = ID_MODIFIER_DEFAULT,
		ID_MODIFIER_RING,
		ID_MODIFIER_MAX
	};
	
	FWModifier * CreateModifier(int nID);
	virtual bool Init();
};



#endif 
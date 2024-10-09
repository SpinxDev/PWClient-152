#ifndef _FWMODIFIER_H_
#define _FWMODIFIER_H_

#include "CodeTemplate.h"
#include <AList2.h>
#include "FWParam.h"
#include "FWConfig.h"
#include "FWObject.h"


class FWParticleSystem;
class ADynPropertyObject;
class FWGfxWithModifier;

class FWModifier : public FWObject
{
	FW_DECLARE_DYNAMIC(FWModifier)
protected:
	// start pos
	PROPERTY_VEC3(Offset);
	// time to wait before start calculating
	PROPERTY_DWORD(TimeWait);
	// time elasped since the first tick of current playing 
	DWORD m_dwElapsed;
public:
	FWModifier();
	virtual ~FWModifier();
	FWModifier & operator = (const FWModifier & src);
	virtual FWModifier * Clone() const = 0;
	
	virtual int GetID() = 0;
	const char * GetName();

	virtual void InitParam() {}

	// radom param that should be same for all
	// element firework
	virtual void RadomParamForAll() {}
	
	// radom param that used locally by each
	// elemnet firework
	virtual void RadomParamForSingle() {}

	virtual bool Tick(DWORD dwTickTime);
	virtual void Serialize(FWArchive &ar);
	virtual bool Play(bool bForceStop) { if (bForceStop) Stop(); return true; }
	virtual bool Stop() { m_dwElapsed = 0; m_vecOffset.Clear(); return true; }
	
	// fill an array of params which is used by UI
	virtual void FillParamArray(FWParamArray &arrayParam) const {}
	// upate content from an array of params that passed by UI
	virtual void UpdateFromParamArray(const FWParamArray &arrayParam) {}
};

#endif 
#ifndef _FWMODIFIERRING_H_
#define _FWMODIFIERRING_H_

#include "FWModifier.h"

class FWModifierRing : public FWModifier  
{
	FW_DECLARE_SERIAL(FWModifierRing)
	PROPERTY_FLOAT(BlastVel);
	PROPERTY_FLOAT(ResistanceFactor);

	// readonly properties
	// their values will be set radomly by the modifier
protected:
	PROPERTY_DEFINE_VEC3(BlastDir);
	PROPERTY_DEFINE_VEC3(NormalDir);
public:
	PROPERTY_GET_VEC3(BlastDir);
	PROPERTY_GET_VEC3(NormalDir);

protected:
	float m_fCurVel;
public:
	FWModifierRing();
	virtual ~FWModifierRing();
	FWModifierRing & operator = (const FWModifierRing &src);
	IMPLEMENT_CLONE_INLINE(FWModifierRing, FWModifier);

	virtual int GetID();
	virtual void InitParam();
	virtual void RadomParamForAll();
	virtual void RadomParamForSingle();
	virtual bool Tick(DWORD dwTickTime);
	virtual bool Play(bool bForceStop);
	virtual bool Stop();
	virtual void Serialize(FWArchive &ar);
	virtual void FillParamArray(FWParamArray &arrayParam) const;
	virtual void UpdateFromParamArray(const FWParamArray &arrayParam);

};

#endif 
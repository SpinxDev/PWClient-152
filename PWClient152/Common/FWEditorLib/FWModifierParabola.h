#ifndef _FWMODIFIERPARABOLA_H_
#define _FWMODIFIERPARABOLA_H_

#include "FWModifier.h"

class FWModifierParabola : public FWModifier  
{
	FW_DECLARE_SERIAL(FWModifierParabola)
	PROPERTY_FLOAT(BlastVel);
	PROPERTY_FLOAT(ResistanceFactor);
	PROPERTY_VEC3(DirDown);
protected:
	// its value will be set radomly by the modifier
	A3DVECTOR3 m_vecBlastDir;
protected:
	// used for computing
	float m_fCurBlastVel;
	float m_fCurVertVel;
public:
	FWModifierParabola();
	virtual ~FWModifierParabola();
	FWModifierParabola & operator = (const FWModifierParabola &src);
	IMPLEMENT_CLONE_INLINE(FWModifierParabola, FWModifier);

	const A3DVECTOR3 & GetBlastDir() const { return m_vecBlastDir; }

	virtual int GetID();
	virtual void InitParam();
	virtual void RadomParamForSingle();
	virtual bool Tick(DWORD dwTickTime);
	virtual void Serialize(FWArchive &ar);
	virtual bool Play(bool bForceStop);
	virtual bool Stop();
	virtual void FillParamArray(FWParamArray &arrayParam) const;
	virtual void UpdateFromParamArray(const FWParamArray &arrayParam);

};

#endif 
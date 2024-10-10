/*
 * FILE: A3DPhysEmitter.h
 *
 * DESCRIPTION: Ribbon Effect
 *
 * CREATED BY: , 2012/4/9
 *
 * HISTORY:
 *
 */

#ifndef _A3DRIBBON_H_
#define _A3DRIBBON_H_

#include "A3DGFXElement.h"
#include "A3DTrail.h"
#include "TrailList.h"

class A3DRibbon : public A3DTrail
{
public:
	explicit A3DRibbon(A3DGFXEx* pGfx);
	virtual ~A3DRibbon();

protected:

	int			m_nTimeToGravity;	
	float			m_fVelocityToGravity;
	GRAVITY_DATA	m_GravityData;

protected:

	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
	virtual bool IsParamEnable(int nParamId) const;
	virtual void EnableParam(int nParamId, bool bEnable);
	virtual bool NeedUpdateParam(int nParamId) const;
	virtual void UpdateParam(int nParamId, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetParam(int nParamId) const;
	virtual GfxValueType GetParamType(int nParamId) const;

public:
	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual bool TickAnimation(DWORD dwTickTime);


public:

	A3DRibbon& operator = (const A3DRibbon& src);

	bool ShouldEnableGravity(DWORD dwTick, float velocity);
	float	GetCurrentVelocity();
	A3DVECTOR3	m_vLast;
	DWORD	m_dwStaticCount;
};



#endif
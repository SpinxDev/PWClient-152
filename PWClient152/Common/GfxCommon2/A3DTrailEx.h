/*
 * FILE: A3DTrailEx.h
 *
 * DESCRIPTION: Extension version of 3D Trail
 *
 * CREATED BY: Yaochunhui, 2012/8/28
 *
 * HISTORY:
 *
 */

#ifndef _A3DTRAIL_EX_H_
#define _A3DTRAIL_EX_H_

#include "A3DTrail.h"

class A3DTrailEx : public A3DTrail
{

public:

    explicit A3DTrailEx(A3DGFXEx* pGfx);
    virtual ~A3DTrailEx();

protected:
    bool IsSplineMode() const { return true; }

    // interfaces of A3DTrail
    virtual float GetPortionFactor(int iRenderIdx);
    virtual float GetPortionAlpha(float fPortion);
    virtual A3DTrail::TRAILMODE GetDefaultMode() const;

    virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
    virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
    virtual bool Save(AFile* pFile);
    virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
    virtual GFX_PROPERTY GetProperty(int nOp) const;

public:
    bool Init(A3DDevice* pDevice);
    A3DTrailEx& operator = (const A3DTrailEx& src);
protected:
    int m_nShrinkTime;
};

#endif

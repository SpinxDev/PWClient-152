/*
 * FILE: A3DGFXLightningEx.h
 *
 * DESCRIPTION: A3DGFXLightningEx provides additional ability besides A3DGFXLightning
 *
 * CREATED BY: ZhangYachuan, 2008/12/9
 *
 * HISTORY:
 *
 */

#include "A3DGFXLightning.h"
#include "RotList.h"
#include <deque>

#ifndef _A3DGFXLIGHTNING_EX_H_
#define _A3DGFXLIGHTNING_EX_H_

class A3DGFXLightningEx : public A3DGFXLightning
{
	// Types
public:
	enum RenderSideType {
		RST_MID		= 0,
		RST_UP		= 1,
		RST_DOWN	= 2,
	};

	// LightningEx curve point
	struct LnCurvePoint {
		LnCurvePoint(int index_ = -1, int life_ = 0) : nIndex(index_), nLife(life_), nAge(0) {}
		inline bool IsDead() const { return nAge == nLife; }
		inline void Tick(DWORD dwTickTime) { if (IsDead()) return; nAge += dwTickTime; a_ClampRoof(nAge, nLife); }
		int nIndex;
		int nLife;
		int nAge;
	};

	typedef A3DGFXLightning base_class;

	// Constructor / Destructor
public:
	// Disable implicit construction
	explicit A3DGFXLightningEx(A3DGFXEx* pGfx);
	// Virtual destructor
	virtual ~A3DGFXLightningEx() {}

private:
	// None copy-construct-able
	A3DGFXLightningEx(const A3DGFXLightningEx& rhs);

	// Public operations
public:

	// Operator = as there is a copy constructor
	A3DGFXLightningEx& operator = (const A3DGFXLightningEx& src);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;

	// Private operations
private:
	// Protected operations
protected:
	// Override virtual functions
	virtual bool Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual bool Play();
	virtual bool Stop();
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;

	virtual void ResumeLoop();
	virtual int GetRenderCount() const;
	virtual void RenderCurveMethod(const int i, 
									const int nRenderCount,
									const int nBase, 
									float fRatio,
									const float fWidthStart,
									const float fWidthEnd,
									const float fWidthMid,
									float& fAlpha, 
									int& nCount,
									const A3DMATRIX4& matTran,
									const A3DVECTOR3& vNormal, 
									const A3DVECTOR3& vCamPos,
									A3DVECTOR3& vLast, 
									A3DVECTOR3& vLastViewUp,
									A3DGFXVERTEX* pVerts);
	virtual A3DVECTOR3 GetRenderPos1(const A3DVECTOR3& vViewUp, const A3DVECTOR3& vPos) const;
	virtual A3DVECTOR3 GetRenderPos2(const A3DVECTOR3& vViewUp, const A3DVECTOR3& vPos) const;

	void TickList(DWORD dwTickTime);
	void TickAdd(DWORD dwTickTime);

	// Attributes
private:
	// Need to Save variables
	bool m_bIsAppendly;
	bool m_bIsUseVertsLife;
	bool m_bIsTailFadeout;
	int m_nRenderSide;
	int m_nVertsLife;

	// Runtime variables
	int m_nTimeSpan;
	int m_nTimeStep;
	int m_nCurrentIndex;
	RotList<LnCurvePoint> m_lstCurvePoints;
};

#endif
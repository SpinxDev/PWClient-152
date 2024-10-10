/*
 * FILE: A3DPhysPointEmitter.h
 *
 * DESCRIPTION: Physical Particle Emitter using PointSprite
 *
 * CREATED BY: ZhangYachuan, 2008/11/21
 *
 * HISTORY:
 *
 */

#ifndef _A3DPHYSPOINTEMITTER_H_
#define _A3DPHYSPOINTEMITTER_H_

#include "A3DPhysFluidEmitter.h"

class A3DPhysPointEmitter : public A3DPhysFluidEmitter
{
	// Constructor / destructor
public:
	explicit A3DPhysPointEmitter(A3DGFXEx* pGfx);
	virtual ~A3DPhysPointEmitter();

	// Public operations
public:	
	// clone & operator =
	A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	A3DPhysPointEmitter& operator = (const A3DPhysPointEmitter& src);

	// this pair of function is used for saving / loading the gfx properties
	virtual bool Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	
	// GFX element tick state
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual bool Play();
	virtual bool Stop();
	
	// GFX element render (register render)
	virtual bool Render(A3DViewport*);
	// GFX element render (real buffer filling)
	virtual int GetVertsCount();
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView);

	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
	
	// Protected operations
protected:
	// Override A3DGFXElement::GetPointSpriteInfo
	virtual void GetPointSpriteInfo(PointSpriteInfo* pInfo) const;

	// Attributes
private:

	// Is fade in & out
	bool m_bIsFade;
	// Time for fade
	float m_fFadeTime;

	// Point Sprite Info
	float m_fPointSize;
	float m_fScaleA, m_fScaleB, m_fScaleC;
};



#endif

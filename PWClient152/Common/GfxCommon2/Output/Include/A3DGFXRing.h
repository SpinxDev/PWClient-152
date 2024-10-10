/*
 * FILE: A3DGFXRing.h
 *
 * DESCRIPTION: GFX Ring
 *
 * CREATED BY: ZhangYu, 2004/8/17
 *
 * HISTORY:
 *
 */

#ifndef _A3DGFXRING_H_
#define _A3DGFXRING_H_

#include "A3DGFXElement.h"
#include "A3DGFXEditorInterface.h"

struct A3DGFXVERTEX;

class A3DGFXRing : public A3DGFXElement
{
public:
	A3DGFXRing(A3DGFXEx* pGfx) : A3DGFXElement(pGfx)
	{
		m_nEleType	= ID_ELE_TYPE_RING;
		m_pDevice	= NULL;
		m_fRadius	= 0.0f;
		m_fHeight	= 0.0f;
		m_fPitch	= 0.0f;
		m_pVerts	= NULL;
		m_nVerts	= 0;
		m_nSects	= 0;
		m_bNoRadScale	= false;
		m_bNoHeiScale	= false;
		m_bOrgAtCent	= true;
	}
	virtual ~A3DGFXRing() {}

protected:
	float			m_fRadius;
	float			m_fHeight;
	float			m_fPitch;
	A3DVECTOR3*		m_pVerts;
	int				m_nVerts;
	int				m_nSects;
	bool			m_bNoRadScale;
	bool			m_bNoHeiScale;
	bool			m_bOrgAtCent;

protected:
	bool InitStreamBuffer();
	void ReleaseStreamBuffer()
	{
		if (m_pVerts)
		{
			delete[] m_pVerts;
			m_pVerts = NULL;
			m_nVerts = 0;
		}
	}

public:
	A3DGFXRing& operator = (const A3DGFXRing& src);

	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual bool Play();
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual void Release();
	virtual bool Render(A3DViewport*);
	virtual int GetVertsCount();
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView);
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch(nOp)
		{
		case ID_GFXOP_RING_RADIUS:
			m_fRadius = prop;
			break;
		case ID_GFXOP_RING_HEIGHT:
			m_fHeight = prop;
			break;
		case ID_GFXOP_RING_PITCH:
			m_fPitch = DEG2RAD((float)prop);
			break;
		case ID_GFXOP_RING_SECTS:
			m_nSects = prop;
			break;
		case ID_GFXOP_RING_NORADSCALE:
			m_bNoRadScale = prop;
			break;
		case ID_GFXOP_RING_NOHEISCALE:
			m_bNoHeiScale = prop;
			break;
		case ID_GFXOP_RING_ORGATCENTER:
			m_bOrgAtCent = prop;
			break;
		default:
			return A3DGFXElement::SetProperty(nOp, prop);
		}

		return true;
	}
	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch(nOp)
		{
		case ID_GFXOP_RING_RADIUS:
			return GFX_PROPERTY(m_fRadius);
		case ID_GFXOP_RING_HEIGHT:
			return GFX_PROPERTY(m_fHeight);
		case ID_GFXOP_RING_PITCH:
			return GFX_PROPERTY(RAD2DEG(m_fPitch));
		case ID_GFXOP_RING_SECTS:
			return GFX_PROPERTY(m_nSects);
		case ID_GFXOP_RING_NORADSCALE:
			return GFX_PROPERTY(m_bNoRadScale);
		case ID_GFXOP_RING_NOHEISCALE:
			return GFX_PROPERTY(m_bNoHeiScale);
		case ID_GFXOP_RING_ORGATCENTER:
			return GFX_PROPERTY(m_bOrgAtCent);
		}
		return A3DGFXElement::GetProperty(nOp);
	}
};

inline bool A3DGFXRing::Play()
{
	if (!IsInit())
	{
		InitBaseData();

		if (InitStreamBuffer())
			SetInit(true);
	}

	return true;
}

#endif

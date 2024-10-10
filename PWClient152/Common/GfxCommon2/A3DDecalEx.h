/*
 * FILE: A3DDecalEx.h
 *
 * DESCRIPTION: 3D Animation Decal
 *
 * CREATED BY: ZhangYu, 2004/7/12
 *
 * HISTORY:
 *
 */

#ifndef _A3DDECALEX_H_
#define _A3DDECALEX_H_

#include "A3DGFXElement.h"
#include "A3DTypes.h"

class A3DGFXEx;
class A3DDecalEx : public A3DGFXElement
{
public:

	A3DDecalEx(A3DGFXEx* pGfx, int nType);

	virtual ~A3DDecalEx()
	{
		delete[] m_pVerts;
		delete[] m_pIndices;
	}

protected:

	float		m_fWidth;		//	Half width
	float		m_fHeight;		//	Half height
	bool		m_bRotFromView;
	int			m_nRectNum;
	bool		m_bGrndNormOnly;
	bool		m_bNoWidthScale;
	bool		m_bNoHeightScale;
	float		m_fOrgPtWidth;
	float		m_fOrgPtHeight;
	float		m_fWidth1;
	float		m_fWidth2;
	float		m_fHeight1;
	float		m_fHeight2;
	float		m_fZOffset;
	bool		m_bMatchSurface;
	bool		m_bSurfaceUseParentDir;
	float		m_fMaxExtent;
	A3DVECTOR3*	m_pVerts;
	WORD*		m_pIndices;
	int			m_nVertCount;
	int			m_nIndexCount;
	A3DVECTOR3	m_vOldCenter;
	float		m_fOldRadius;
	bool		m_bCenterUpdate;
	// rotation axis Y will take effect when m_bGroundNormal is true
	bool		m_bYawEffectOnGrndNorm;
	bool		m_b2DScreenDimension;
	
	// for compatible, runtime variable
	bool		m_bMatchSurfaceUVEffects;

public:

	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual bool Play();
	virtual bool Render(A3DViewport*);
	virtual int GetVertsCount();
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView);
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_GFXOP_DECAL_WIDTH:
			m_fWidth = (float)prop / 2.0f;
			break;
		case ID_GFXOP_DECAL_HEIGHT:
			m_fHeight = (float)prop / 2.0f;
			break;
		case ID_GFXOP_DECAL_ROTFROMVIEW:
			m_bRotFromView = prop;
			break;
		case ID_GFXOP_DECAL_GRNDNORM_ONLY:
			m_bGrndNormOnly = prop;
			break;
		case ID_GFXOP_DECAL_NO_WID_SCALE:
			m_bNoWidthScale = prop;
			break;
		case ID_GFXOP_DECAL_NO_HEI_SCALE:
			m_bNoHeightScale = prop;
			break;
		case ID_GFXOP_DECAL_ORG_PT_WID:
			m_fOrgPtWidth = prop;
			break;
		case ID_GFXOP_DECAL_ORG_PT_HEI:
			m_fOrgPtHeight = prop;
			break;
		case ID_GFXOP_DECAL_Z_OFFSET:
			m_fZOffset = prop;
			break;
		case ID_GFXOP_DECAL_MATCH_SURFACE:
			m_bMatchSurface = prop;
			break;
		case ID_GFXOP_DECAL_SURFACE_USE_PARENT_DIR:
			m_bSurfaceUseParentDir = prop;
			break;
		case ID_GFXOP_DECAL_YAWEFFECT_GRNDNORM:
			m_bYawEffectOnGrndNorm = prop;
			break;
		case ID_GFXOP_DECAL_SCREEN_DIMENSION:
			m_b2DScreenDimension = prop;
			break;
		default:
			return A3DGFXElement::SetProperty(nOp, prop);
		}

		SetSize(m_fWidth, m_fHeight);
		return true;
	}
	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_GFXOP_DECAL_WIDTH:
			return GFX_PROPERTY(m_fWidth*2.0f);
		case ID_GFXOP_DECAL_HEIGHT:
			return GFX_PROPERTY(m_fHeight*2.0f);
		case ID_GFXOP_DECAL_ROTFROMVIEW:
			return GFX_PROPERTY(m_bRotFromView);
		case ID_GFXOP_DECAL_GRNDNORM_ONLY:
			return GFX_PROPERTY(m_bGrndNormOnly);
		case ID_GFXOP_DECAL_NO_WID_SCALE:
			return GFX_PROPERTY(m_bNoWidthScale);
		case ID_GFXOP_DECAL_NO_HEI_SCALE:
			return GFX_PROPERTY(m_bNoHeightScale);
		case ID_GFXOP_DECAL_ORG_PT_WID:
			return GFX_PROPERTY(m_fOrgPtWidth);
		case ID_GFXOP_DECAL_ORG_PT_HEI:
			return GFX_PROPERTY(m_fOrgPtHeight);
		case ID_GFXOP_DECAL_Z_OFFSET:
			return GFX_PROPERTY(m_fZOffset);
		case ID_GFXOP_DECAL_MATCH_SURFACE:
			return GFX_PROPERTY(m_bMatchSurface);
		case ID_GFXOP_DECAL_SURFACE_USE_PARENT_DIR:
			return GFX_PROPERTY(m_bSurfaceUseParentDir);
		case ID_GFXOP_DECAL_YAWEFFECT_GRNDNORM:
			return GFX_PROPERTY(m_bYawEffectOnGrndNorm);
		case ID_GFXOP_DECAL_SCREEN_DIMENSION:
			return GFX_PROPERTY(m_b2DScreenDimension);
		}
		return A3DGFXElement::GetProperty(nOp);
	}

protected:

	bool Update_3D(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex);
	bool Update_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts);
	bool Update_Billboard(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex);
	bool Fill_Verts_3D(const KEY_POINT& kp, A3DViewport* pView, A3DLVERTEX* pVerts);
	bool Fill_Verts_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts, float& fWidth, float& fHeight);
	bool Update(A3DViewport* pView);
	A3DDecalEx& operator = (const A3DDecalEx& src);
	bool SetupSurfaceData(const A3DVECTOR3& vCenter, float fRadius);

public:

	bool Init(A3DDevice* pDevice);
	// operations
	void SetSize(float fWidth, float fHeight)
	{
		m_fWidth = fWidth;
		m_fHeight = fHeight;
		fWidth *= 2.0f;
		fHeight *= 2.0f;
		m_fWidth1 = m_fOrgPtWidth * fWidth;
		m_fWidth2 = fWidth - m_fWidth1;
		m_fHeight1 = m_fOrgPtHeight * fHeight;
		m_fHeight2 = fHeight - m_fHeight1;
	}
	int DrawToBuffer(A3DViewport* pView, A3DTLWARPVERTEX* pVerts, int nMaxVerts, float rw, float rh);
	int DrawToBuffer(A3DViewport* pView, A3DWARPVERTEX* pVerts, int nMaxVerts);
};

#endif

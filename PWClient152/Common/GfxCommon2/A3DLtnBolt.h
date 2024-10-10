#ifndef _A3DLTNBOLT_H_
#define _A3DLTNBOLT_H_

#include "A3DVector.h"
#include "APerlinNoise1D.h"
#include "A3DGFXLightning.h"

class A3DGFXLtnBolt : public A3DGFXElement
{
public:
	A3DGFXLtnBolt(A3DGFXEx* pGfx) : A3DGFXElement(pGfx)
	{
		m_nEleType		= ID_ELE_TYPE_LTNBOLT;
		m_fDeviation	= DEG2RAD(30.f);
		m_fStepLenMin	= 5.f;
		m_fStepLenMax	= 6.f;
		m_fWidthStart	= .15f;
		m_fWidthEnd		= .03f;
		m_fStartAlpha	= 1.f;
		m_fEndAlpha		= 0.f;
		m_fStrokeAmp	= .4f;
		m_nMaxSteps		= 1;
		m_nMaxBraches	= 1;
		m_dwInterval	= 100;
		m_nPerBolts		= 3;
		m_nCircles		= 3;
		m_pRandDir		= NULL;
		m_pRandUp		= NULL;
		m_nTotalBolts	= 0;
		m_dwTimeSpan  = 0;
	}
	virtual ~A3DGFXLtnBolt() {}

protected:
	float			m_fDeviation;
	float			m_fStepLenMin;
	float			m_fStepLenMax;
	float			m_fWidthStart;
	float			m_fWidthEnd;
	float			m_fStartAlpha;
	float			m_fEndAlpha;
	float			m_fStrokeAmp;
	int				m_nMaxSteps;
	int				m_nMaxBraches;
	DWORD			m_dwInterval;
	int				m_nPerBolts;
	int				m_nCircles;

	A3DVECTOR3		m_vDir;
	A3DVECTOR3		m_vUp;
	A3DVECTOR3*		m_pRandDir;
	A3DVECTOR3*		m_pRandUp;
	int				m_nTotalBolts;

	abase::vector<A3DGFXLightning*> m_Strokes;
	APerlinNoise1D	m_Perlin;
	DWORD			m_dwTimeSpan;

protected:
	void BuildStrokeDir();
	void BuildStrokeDir2D();
	void BuildStrokeDir3D();
	void BuildPath(float fScale);
	void AddPath(int nStep, int nBranch, const A3DVECTOR3& vStart, float fScale, int nNoisePos);
	void ReleaseStrokes()
	{
		for (size_t i = 0; i < m_Strokes.size(); i++)
		{
			m_Strokes[i]->Release();
			delete m_Strokes[i];
		}

		m_Strokes.clear();
	}

public:
	void Resume()
	{
	#ifndef _ANGELICA22
		m_Perlin.Release();
	#endif
		m_Perlin.Init(256, 2.0f, 16, 0.85f, 6, rand());
		m_dwTimeSpan = 0;
		ReleaseStrokes();
	}

public:
	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual void Release()
	{
		m_Perlin.Release();
		ReleaseStrokes();

		if (m_pRandDir)
		{
			delete[] m_pRandDir;
			m_pRandDir = NULL;
		}

		if (m_pRandUp)
		{
			delete[] m_pRandUp;
			m_pRandUp = NULL;
		}
	}
	virtual bool Render(A3DViewport*);
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual bool Play();
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch(nOp)
		{
		case ID_GFXOP_BOLT_DEVIATION:
			m_fDeviation = DEG2RAD((float)prop);
			break;
		case ID_GFXOP_BOLT_STEP_MIN:
			m_fStepLenMin = prop;
			break;
		case ID_GFXOP_BOLT_STEP_MAX:
			m_fStepLenMax = prop;
			break;
		case ID_GFXOP_BOLT_WIDTH_START:
			m_fWidthStart = prop;
			break;
		case ID_GFXOP_BOLT_WIDTH_END:
			m_fWidthEnd = prop;
			break;
		case ID_GFXOP_BOLT_ALPHA_START:
			m_fStartAlpha = prop;
			break;
		case ID_GFXOP_BOLT_ALPHA_END:
			m_fEndAlpha = prop;
			break;
		case ID_GFXOP_BOLT_AMP:
			m_fStrokeAmp = prop;
			break;
		case ID_GFXOP_BOLT_STEPS:
			m_nMaxSteps = prop;
			break;
		case ID_GFXOP_BOLT_BRANCHES:
			m_nMaxBraches = prop;
			break;
		case ID_GFXOP_BOLT_INTERVAL:
			m_dwInterval = prop;
			break;
		case ID_GFXOP_BOLT_PER_BOLTS:
			m_nPerBolts = prop;
			break;
		case ID_GFXOP_BOLT_CIRCLES:
			m_nCircles = prop;
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
		case ID_GFXOP_BOLT_DEVIATION:
			return GFX_PROPERTY(RAD2DEG(m_fDeviation));
		case ID_GFXOP_BOLT_STEP_MIN:
			return GFX_PROPERTY(m_fStepLenMin);
		case ID_GFXOP_BOLT_STEP_MAX:
			return GFX_PROPERTY(m_fStepLenMax);
		case ID_GFXOP_BOLT_WIDTH_START:
			return GFX_PROPERTY(m_fWidthStart);
		case ID_GFXOP_BOLT_WIDTH_END:
			return GFX_PROPERTY(m_fWidthEnd);
		case ID_GFXOP_BOLT_ALPHA_START:
			return GFX_PROPERTY(m_fStartAlpha);
		case ID_GFXOP_BOLT_ALPHA_END:
			return GFX_PROPERTY(m_fEndAlpha);
		case ID_GFXOP_BOLT_AMP:
			return GFX_PROPERTY(m_fStrokeAmp);
		case ID_GFXOP_BOLT_STEPS:
			return GFX_PROPERTY(m_nMaxSteps);
		case ID_GFXOP_BOLT_BRANCHES:
			return GFX_PROPERTY(m_nMaxBraches);
		case ID_GFXOP_BOLT_INTERVAL:
			return GFX_PROPERTY((int)m_dwInterval);
		case ID_GFXOP_BOLT_PER_BOLTS:
			return GFX_PROPERTY(m_nPerBolts);
		case ID_GFXOP_BOLT_CIRCLES:
			return GFX_PROPERTY(m_nCircles);
		}
		return A3DGFXElement::GetProperty(nOp);
	}

public:
	A3DGFXLtnBolt& operator = (const A3DGFXLtnBolt& src);
};

inline bool A3DGFXLtnBolt::Play()
{
	if (!IsInit())
	{
		InitBaseData();
		BuildStrokeDir();
		SetInit(true);
	}

	BuildPath(m_KeyPointSet.GetKeyPoint(0)->GetKPOrg().m_fScale);
	return true;
}

#endif

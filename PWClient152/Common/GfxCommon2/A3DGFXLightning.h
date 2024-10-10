/*
 * FILE: A3DLightning.h
 *
 * DESCRIPTION: Lightning Effect
 *
 * CREATED BY: ZhangYu, 2004/8/30
 *
 * HISTORY:
 *
 */

#ifndef _A3DGFXLIGHTNING_H_
#define _A3DGFXLIGHTNING_H_

#include "A3DGFXElement.h"
#include "A3DGFXEditorInterface.h"

class A3DGFXAnimable;

class A3DGFXLightning : public A3DGFXElement
{
public:
	// Lightning Filter
	enum FilterType
	{
		FT_TRIANGLE = 0,
		FT_RECTANGLE,
		FT_SIN,
		FT_TRAPEZIA,
	};
	
public:

	A3DGFXLightning(A3DGFXEx* pGfx);
	virtual ~A3DGFXLightning();

protected:
	A3DVECTOR3			m_vStartPos;
	A3DVECTOR3			m_vEndPos;
	A3DVECTOR3			m_vStartCur;
	A3DVECTOR3			m_vEndCur;
	A3DVECTOR3			m_vPosProc[2];
	A3DVECTOR3*			m_pIntPos;
	A3DVECTOR3*			m_pIntDir;
	A3DVECTOR3*			m_pIntUp;
	A3DVECTOR3*			m_pRandPos;

	/*
	 *	0 - portion
	 *	1 - Rand Angle
	 *	2 - Rand Amplitude
	 */
	float*				m_pData[3];
	A3DGFXCtrlNoiseBase	m_NoiseCtrl;
	int					m_nSegs;
	int					m_nLightNum;
	int					m_nVertPairNum;
	float				m_fWaveLen;
	float				m_fWidthStart;
	float				m_fWidthEnd;
	float				m_fWidthMid;
	float				m_fAlphaStart;
	float				m_fAlphaEnd;
	float				m_fAlphaMid;
	int					m_nInterval;
	int					m_nTimeSpan;
	float				m_fAmplitude;
	bool				m_bPos1Enable;
	bool				m_bPos2Enable;
	bool				m_bCurvedMethod;
	float				m_fCurvedLen;
	float				m_fDeviation;
	bool				m_bUseNormal;
	A3DVECTOR3			m_vNormal;
	int					m_nFilterType;
	bool				m_bOutParamChanged;

	A3DGFXAnimable*		m_pAnimableAmiplitude;

	//	
	bool				m_bWaveMoving;
	float				m_fWaveMovingSpeed;
	bool				m_bFixWaveLength;
	float				m_fNumWaves;

protected:
	virtual void BuildPath(const A3DMATRIX4& matTran);
	virtual bool UpdateVerts(A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex);
	virtual bool InitBuf();
	virtual void ReleaseBuf()
	{
		if (m_pIntPos)
		{
			delete[] m_pIntPos;
			m_pIntPos = NULL;
		}

		if (m_pIntDir)
		{
			delete[] m_pIntDir;
			m_pIntDir = NULL;
		}

		if (m_pIntUp)
		{
			delete[] m_pIntUp;
			m_pIntUp = NULL;
		}

		if (m_pRandPos)
		{
			delete[] m_pRandPos;
			m_pRandPos = NULL;
		}

		for (int i = 0; i < sizeof(m_pData) / sizeof(float*); i++)
		{
			delete[] m_pData[i];
			m_pData[i] = NULL;
		}
	}
	virtual int GetRenderCount() const;
	virtual A3DVECTOR3 GetRenderPos1(const A3DVECTOR3& vViewUp, const A3DVECTOR3& vPos) const;
	virtual A3DVECTOR3 GetRenderPos2(const A3DVECTOR3& vViewUp, const A3DVECTOR3& vPos) const;
	virtual void RenderCurveMethod( const int i, 
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

	void UpdatePath();

public:
	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual void Release();
	virtual bool Render(A3DViewport*);
	virtual int GetVertsCount();
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView);
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual bool Play();
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
	virtual bool IsParamEnable(int nParamId) const;
	virtual void EnableParam(int nParamId, bool bEnable);
	virtual void UpdateParam(int nParamId, const GFX_PROPERTY& prop);
	virtual GfxValueType GetParamType(int nParamId) const;

public:
	A3DGFXLightning& operator = (const A3DGFXLightning& src);
	void SetData(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, int nSegs, int nLightNum, float fWaveLen,
		int nInterval, float fWidthStart, float fWidthEnd, float fStartAlpha, float fEndAlpha, float fAmp)
	{
		m_vStartPos		= vStart;
		m_vEndPos		= vEnd;
		m_nSegs			= nSegs;
		m_nLightNum		= nLightNum;
		m_fWaveLen		= fWaveLen;
		m_nInterval		= nInterval;
		m_fWidthStart	= fWidthStart;
		m_fWidthEnd		= fWidthEnd;
		m_fWidthMid		= (m_fWidthStart + m_fWidthEnd) * .5f;
		m_fAlphaStart	= fStartAlpha;
		m_fAlphaEnd		= fEndAlpha;
		m_fAlphaMid		= (m_fAlphaStart + m_fAlphaEnd) * .5f;
		m_fAmplitude	= fAmp;
	}
	void SetNoiseData(int nBufLen, float fAmp, int nWaveLen, float fPers, int nOctNum)
	{
		m_NoiseCtrl.SetData(
			nBufLen,
			fAmp,
			nWaveLen,
			fPers,
			nOctNum);
	}
	void SetDeviation(float f) { m_fDeviation = f; }
};

inline bool A3DGFXLightning::Play()
{
	m_nTimeSpan = 0;

	if (!IsInit())
	{
		InitBaseData();

		if (!InitBuf())
			return true;
		else
			SetInit(true);
	}

	assert(m_KeyPointSet.GetKeyPointCount());
	BuildPath(GetTranMatrix(m_KeyPointSet.GetKeyPoint(0)->GetKPOrg()));

	return true;
}

#endif

#include "StdAfx.h"
#include "A3DGFXLightning.h"
#include "A3DConfig.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DGFXMan.h"
#include "A3DCDS.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "A3DGFXExMan.h"
#include "A3DGFXAnimable.h"
#include "A3DGFXFloatValueTrans.h"

static const char _format_start_pos[]	= "StartPos: %f, %f, %f";
static const char _format_end_pos[]		= "EndPos: %f, %f, %f";
static const char _format_segs[]		= "Segs: %d";
static const char _format_light_num[]	= "LightNum: %d";
static const char _format_wave_len[]	= "WaveLen: %f";
static const char _format_interval[]	= "Interval: %d";
static const char _format_width[]		= "Width: %f";
static const char _format_alpha[]		= "Alpha: %f";
static const char _format_ampitude[]	= "Amplitude: %f";
static const char _format_pos1_enable[]	= "Pos1Enable: %d";
static const char _format_pos2_enable[]	= "Pos2Enable: %d";
static const char _format_use_normal[]	= "UseNormal: %d";
static const char _format_normal[]		= "Normal: %f, %f, %f";
static const char _format_filter[]		= "Filter: %d";
static const char _format_wavemoving[]	= "WaveMoving: %d";
static const char _format_wavemovingspeed[] = "WaveMovingSpeed: %f";
static const char _format_fixwavelength[] = "FixWaveLength: %d";
static const char _format_numwaves[]	= "NumWaves: %f";

A3DGFXLightning::A3DGFXLightning(A3DGFXEx* pGfx) :
A3DGFXElement(pGfx),
m_vStartPos(0),
m_vEndPos(0),
m_vNormal(0),
m_nFilterType(FT_RECTANGLE)
{
	m_nEleType		= ID_ELE_TYPE_LIGHTNING;
	m_nSegs			= 0;
	m_nLightNum		= 0;
	m_nVertPairNum	= 0;
	m_fWaveLen		= 0;
	m_fWidthStart	= 0;
	m_fWidthEnd		= 0;
	m_fWidthMid		= 0;
	m_fAlphaStart	= 1.f;
	m_fAlphaEnd		= 1.f;
	m_fAlphaMid		= 1.f;
	m_pIntPos		= NULL;
	m_pIntDir		= NULL;
	m_pIntUp		= NULL;
	m_pRandPos		= NULL;
	m_nInterval		= 100;
	m_nTimeSpan		= 0;
	m_fAmplitude	= 0;
	m_bPos1Enable	= false;
	m_bPos2Enable	= false;
	m_bCurvedMethod	= false;
	m_fDeviation	= 0;
	m_bUseNormal	= false;
	m_bOutParamChanged = false;
	
	m_pAnimableAmiplitude = A3DGFXAnimable::CreateAnimable(A3DGFXAnimable::ANIMTYPE_FLOATTRANS);

	m_bWaveMoving		= false;
	m_fWaveMovingSpeed	= 1.0f;
	m_bFixWaveLength	= false;
	m_fNumWaves		=	5.0f;

	for (int i = 0; i < sizeof(m_pData) / sizeof(float*); i++)
		m_pData[i]	= NULL;
}

A3DGFXLightning::~A3DGFXLightning()
{
	A3DGFXAnimable::DestroyAnimable(m_pAnimableAmiplitude);
	m_pAnimableAmiplitude = NULL;
}

bool A3DGFXLightning::InitBuf()
{
	if (m_nSegs < 2 || m_nLightNum < 1) return false;

	assert(m_KeyPointSet.GetKeyPointCount());
	A3DGFXKeyPoint* pKeyPoint = m_KeyPointSet.GetKeyPoint(0);
	A3DGFXCtrlCurveMove* pCurveCtrl = static_cast<A3DGFXCtrlCurveMove*>(pKeyPoint->GetCtrlMethodByType(ID_KPCTRL_CURVE_MOVE));

	if (pCurveCtrl)
	{
		m_bCurvedMethod = true;
		m_fCurvedLen = pCurveCtrl->GetTotalLen();
		m_nLightNum = 1;
	}
	else
		m_bCurvedMethod = false;

	m_nVertPairNum = m_nSegs * m_nLightNum + 1;
	int i;

	for (i = 0; i < sizeof(m_pData) / sizeof(float*); i++)
	{
		delete[] m_pData[i];
		m_pData[i] = new float[m_nSegs * m_nLightNum];
	}

	int nCount = 0;
	float fSegs = (float)m_nSegs;

	for (i = 0; i < m_nLightNum; i++)
	{
		for (int j = 1; j < m_nSegs; j++)
		{
			if (i % 2 == 0) m_pData[0][nCount] = j / fSegs;
			else m_pData[0][nCount] = (m_nSegs - j) / fSegs;
			nCount++;
		}
	}

	delete[] m_pIntPos;
	m_pIntPos = NULL;

	delete[] m_pIntDir;
	m_pIntDir = NULL;

	delete[] m_pIntUp;
	m_pIntUp = NULL;

	delete[] m_pRandPos;
	m_pRandPos = NULL;

	if (m_bCurvedMethod)
	{
		m_pIntPos = new A3DVECTOR3[m_nSegs+1];
		m_pIntDir = new A3DVECTOR3[m_nSegs+1];
		m_pIntUp = new A3DVECTOR3[m_nSegs+1];
		m_pRandPos = new A3DVECTOR3[m_nSegs];

		A3DVECTOR3 vPos;
		A3DQUATERNION vDir;
		size_t index;

		for (i = 0; i <= m_nSegs; i++)
		{
			index = 0;

			if (!pCurveCtrl->GetInterpPoint(i / fSegs, m_pIntPos[i], vDir, index))
			{
				m_pIntPos[i].Clear();
				vDir.Normalize();
			}

			if (i > 0)
			{
				m_pIntDir[i-1] = Normalize(m_pIntPos[i] - m_pIntPos[i-1]);
				m_pIntUp[i-1] = CalcVertVec(m_pIntDir[i-1]);
			}
		}

		m_pIntDir[m_nSegs] = m_pIntDir[m_nSegs-1];
		m_pIntUp[m_nSegs] = CalcVertVec(m_pIntDir[m_nSegs]);
	}

	m_NoiseCtrl.Resume();

	m_dwRenderSlot = AfxGetGFXExMan()->GetRenderMan().RegisterSlot(
		m_nRenderLayer,
		A3DFVF_GFXVERTEX,
		sizeof(A3DGFXVERTEX),
		A3DPT_TRIANGLESTRIP,
		m_strTexture,
		m_Shader,
		m_bTileMode,
		m_bTexNoDownSample,
		m_pGfx->IsZTestEnable() && m_bZEnable,
		HasPixelShader(),
        m_bSoftEdge,
		m_bAbsTexPath);

	return true;
}

A3DGFXElement* A3DGFXLightning::Clone(A3DGFXEx* pGfx) const
{
	A3DGFXLightning* p = new A3DGFXLightning(pGfx);
	return &(*p = *this);
}

A3DGFXLightning& A3DGFXLightning::operator = (const A3DGFXLightning& src)
{
	if (&src == this)
		return *this;
	
	_CloneBase(&src);
	Init(src.m_pDevice);
	m_NoiseCtrl._CloneBase(&src.m_NoiseCtrl);
	m_vStartPos		= src.m_vStartPos;
	m_vEndPos		= src.m_vEndPos;
	m_nSegs			= src.m_nSegs;
	m_nLightNum		= src.m_nLightNum;
	m_fWaveLen		= src.m_fWaveLen;
	m_fWidthStart	= src.m_fWidthStart;
	m_fWidthEnd		= src.m_fWidthEnd;
	m_fWidthMid		= src.m_fWidthMid;
	m_fAlphaStart	= src.m_fAlphaStart;
	m_fAlphaEnd		= src.m_fAlphaEnd;
	m_fAlphaMid		= src.m_fAlphaMid;
	m_nInterval		= src.m_nInterval;
	m_fAmplitude	= src.m_fAmplitude;
	m_bPos1Enable	= src.m_bPos1Enable;
	m_bPos2Enable	= src.m_bPos2Enable;
	m_fDeviation	= src.m_fDeviation;
	m_bUseNormal	= src.m_bUseNormal;
	m_vNormal		= src.m_vNormal;
	m_bOutParamChanged = src.m_bOutParamChanged;

	A3DGFXAnimable::DestroyAnimable(m_pAnimableAmiplitude);
	m_pAnimableAmiplitude = src.m_pAnimableAmiplitude->Clone();

	m_bWaveMoving	= src.m_bWaveMoving;
	m_fWaveMovingSpeed		= src.m_fWaveMovingSpeed;
	m_bFixWaveLength	= src.m_bFixWaveLength;
	m_fNumWaves	= src.m_fNumWaves;


	return *this;
}

bool A3DGFXLightning::TickAnimation(DWORD dwTickTime)
{
	if (!A3DGFXElement::TickAnimation(dwTickTime)) return false;

	if (m_bWaveMoving)
	{
		UpdatePath();
		return true;
	}
	
	m_nTimeSpan += dwTickTime;
	if (m_nTimeSpan >= m_nInterval)
	{		
		m_nTimeSpan -= m_nInterval * (m_nTimeSpan / m_nInterval);

		UpdatePath();
	}
	else if (m_bOutParamChanged)
	{
		// 如果这一帧中存在外部参数更新，则立即更新
		// 这是为了避免出现帧不同步的情况
		UpdatePath();

		m_bOutParamChanged = false;
	}

	return true;
}


bool A3DGFXLightning::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch(nOp)
	{
	case ID_GFXOP_LN_START_POS:
		m_vStartPos = prop;
		break;
	case ID_GFXOP_LN_END_POS:
		m_vEndPos = prop;
		break;
	case ID_GFXOP_LN_SEGS:
		m_nSegs = prop;
		break;
	case ID_GFXOP_LN_NUM:
		m_nLightNum = prop;
		break;
	case ID_GFXOP_LN_WAVELEN:
		m_fWaveLen = prop;
		break;
	case ID_GFXOP_LN_INTERVAL:
		m_nInterval = prop;
		break;
	case ID_GFXOP_LN_WIDTH_START:
		m_fWidthStart = prop;
		break;
	case ID_GFXOP_LN_WIDTH_END:
		m_fWidthEnd = prop;
		break;
	case ID_GFXOP_LN_WIDTH_MID:
		m_fWidthMid = prop;
		break;
	case ID_GFXOP_LN_ALPHA_START:
		m_fAlphaStart = prop;
		break;
	case ID_GFXOP_LN_ALPHA_END:
		m_fAlphaEnd = prop;
		break;
	case ID_GFXOP_LN_ALPHA_MID:
		m_fAlphaMid = prop;
		break;
	case ID_GFXOP_LN_AMPLITUDE:
		//m_fAmplitude = prop;
		break;
	case ID_GFXOP_LN_USE_NORMAL:
		m_bUseNormal = prop;
		break;
	case ID_GFXOP_LN_NORMAL:
		m_vNormal = prop;
		m_vNormal.Normalize();
		break;
	case ID_GFXOP_LN_FILTER:
		m_nFilterType = prop;
		break;
	case ID_GFXOP_LN_WAVEMOVE:
		m_bWaveMoving = prop;
		break;
	case ID_GFXOP_LN_WAVEMOVESPEED:
		m_fWaveMovingSpeed = prop;
		break;
	case ID_GFXOP_LN_FIXWAVELENGTH:
		m_bFixWaveLength = prop;
		break;
	case ID_GFXOP_LN_NUMWAVES:
		m_fNumWaves = prop;
		break;
	default:
		if (A3DGFXElement::SetProperty(nOp, prop)) return true;
		return m_NoiseCtrl.SetProperty(nOp, prop);
	}
	return true;
}

GFX_PROPERTY A3DGFXLightning::GetProperty(int nOp) const
{
	switch(nOp)
	{
	case ID_GFXOP_LN_START_POS:
		return GFX_PROPERTY(m_vStartPos);
	case ID_GFXOP_LN_END_POS:
		return GFX_PROPERTY(m_vEndPos);
	case ID_GFXOP_LN_SEGS:
		return GFX_PROPERTY(m_nSegs);
	case ID_GFXOP_LN_NUM:
		return GFX_PROPERTY(m_nLightNum);
	case ID_GFXOP_LN_WAVELEN:
		return GFX_PROPERTY(m_fWaveLen);
	case ID_GFXOP_LN_INTERVAL:
		return GFX_PROPERTY(m_nInterval);
	case ID_GFXOP_LN_WIDTH_START:
		return GFX_PROPERTY(m_fWidthStart);
	case ID_GFXOP_LN_WIDTH_END:
		return GFX_PROPERTY(m_fWidthEnd);
	case ID_GFXOP_LN_WIDTH_MID:
		return GFX_PROPERTY(m_fWidthMid);
	case ID_GFXOP_LN_ALPHA_START:
		return GFX_PROPERTY(m_fAlphaStart);
	case ID_GFXOP_LN_ALPHA_END:
		return GFX_PROPERTY(m_fAlphaEnd);
	case ID_GFXOP_LN_ALPHA_MID:
		return GFX_PROPERTY(m_fAlphaMid);
	case ID_GFXOP_LN_AMPLITUDE:
		return GFX_PROPERTY(m_pAnimableAmiplitude);
	case ID_GFXOP_LN_USE_NORMAL:
		return GFX_PROPERTY(m_bUseNormal);
	case ID_GFXOP_LN_NORMAL:
		return GFX_PROPERTY(m_vNormal);
	case ID_GFXOP_LN_FILTER:
		return GFX_PROPERTY(m_nFilterType).SetType(GFX_VALUE_LIGHTNING_FILTER);
	case ID_GFXOP_LN_WAVEMOVE:
		return GFX_PROPERTY(m_bWaveMoving);
	case ID_GFXOP_LN_WAVEMOVESPEED:
		return GFX_PROPERTY(m_fWaveMovingSpeed);
	case ID_GFXOP_LN_FIXWAVELENGTH:
		return GFX_PROPERTY(m_bFixWaveLength);
	case ID_GFXOP_LN_NUMWAVES:
		return GFX_PROPERTY(m_fNumWaves);
	}
	GFX_PROPERTY gp = A3DGFXElement::GetProperty(nOp);
	if (gp.GetType() == GFX_VALUE_UNKNOWN)
		return m_NoiseCtrl.GetProperty(nOp);
	return gp;
}

bool A3DGFXLightning::IsParamEnable(int nParamId) const
{
	switch (nParamId)
	{
	case ID_PARAM_LTN_POS1:
		return m_bPos1Enable;
	case ID_PARAM_LTN_POS2:
		return m_bPos2Enable;
	}
	return false;
}

void A3DGFXLightning::EnableParam(int nParamId, bool bEnable)
{
	switch (nParamId)
	{
	case ID_PARAM_LTN_POS1:
		m_bPos1Enable = bEnable;
		break;
	case ID_PARAM_LTN_POS2:
		m_bPos2Enable = bEnable;
		break;
	}
}

void A3DGFXLightning::UpdateParam(int nParamId, const GFX_PROPERTY& prop)
{
	switch (nParamId)
	{
	case ID_PARAM_LTN_POS1:
		{
			A3DVECTOR3 vNewStartPos = prop;
			if (vNewStartPos != m_vStartPos)
			{
				m_vStartPos = vNewStartPos;
				m_bOutParamChanged = true;
			}
		}
		
		break;
	case ID_PARAM_LTN_POS2:
		{
			A3DVECTOR3 vNewEndPos = prop;
			if (vNewEndPos != m_vEndPos)
			{
				m_vEndPos = vNewEndPos;
				m_bOutParamChanged = true;
			}
		}
		break;
	}
}

GfxValueType A3DGFXLightning::GetParamType(int nParamId) const
{
	switch (nParamId)
	{
	case ID_PARAM_LTN_POS1:
	case ID_PARAM_LTN_POS2:
		return GFX_VALUE_VECTOR3;
	}
	return GFX_VALUE_UNKNOWN;
}

void A3DGFXLightning::Release()
{
	A3DGFXElement::Release();
	ReleaseBuf();
}

bool A3DGFXLightning::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;

	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}

bool A3DGFXLightning::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	m_NoiseCtrl.Load(pFile, dwVersion);

	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;

	if (pFile->IsBinary())
	{
		pFile->Read(m_vStartPos.m, sizeof(m_vStartPos.m), &dwReadLen);
		pFile->Read(m_vEndPos.m, sizeof(m_vEndPos.m), &dwReadLen);
		pFile->Read(&m_nSegs, sizeof(m_nSegs), &dwReadLen);
		pFile->Read(&m_nLightNum, sizeof(m_nLightNum), &dwReadLen);
		pFile->Read(&m_fWaveLen, sizeof(m_fWaveLen), &dwReadLen);
		pFile->Read(&m_nInterval, sizeof(m_nInterval), &dwReadLen);
		pFile->Read(&m_fWidthStart, sizeof(m_fWidthStart), &dwReadLen);
		pFile->Read(&m_fWidthEnd, sizeof(m_fWidthEnd), &dwReadLen);
		pFile->Read(&m_fWidthMid, sizeof(m_fWidthMid), &dwReadLen);
		pFile->Read(&m_fAlphaStart, sizeof(m_fAlphaStart), &dwReadLen);
		pFile->Read(&m_fAlphaEnd, sizeof(m_fAlphaEnd), &dwReadLen);
		pFile->Read(&m_fAlphaMid, sizeof(m_fAlphaMid), &dwReadLen);
		pFile->Read(&m_fAmplitude, sizeof(m_fAmplitude), &dwReadLen);
		pFile->Read(&m_bPos1Enable, sizeof(m_bPos1Enable), &dwReadLen);
		pFile->Read(&m_bPos2Enable, sizeof(m_bPos2Enable), &dwReadLen);
		pFile->Read(&m_bUseNormal, sizeof(m_bUseNormal), &dwReadLen);
		pFile->Read(m_vNormal.m, sizeof(m_vNormal.m), &dwReadLen);
		pFile->Read(&m_nFilterType, sizeof(m_nFilterType), &dwReadLen);
	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_start_pos, VECTORADDR_XYZ(m_vStartPos));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_end_pos, VECTORADDR_XYZ(m_vEndPos));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_segs, &m_nSegs);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_light_num, &m_nLightNum);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_wave_len, &m_fWaveLen);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_interval, &m_nInterval);

		if (dwVersion <= 5)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_width, &m_fWidthStart);
			m_fWidthEnd = m_fWidthStart;
		}
		else
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_width, &m_fWidthStart);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_width, &m_fWidthEnd);
		}

		if (dwVersion >= 7)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_alpha, &m_fAlphaStart);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_alpha, &m_fAlphaEnd);
		}

		if (dwVersion >= 35)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_width, &m_fWidthMid);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_alpha, &m_fAlphaMid);
		}
		else
		{
			m_fWidthMid = (m_fWidthStart + m_fWidthEnd) * .5f;
			m_fAlphaMid = (m_fAlphaStart + m_fAlphaEnd) * .5f;
		}

		if (dwVersion >= 102)
		{
			((A3DGFXFloatValueTrans*)m_pAnimableAmiplitude)->Load(pFile, dwVersion);
		}
		else
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_ampitude, &m_fAmplitude);

			((A3DGFXFloatValueTrans*)m_pAnimableAmiplitude)->SetInitValue(m_fAmplitude);
			((A3DGFXFloatValueTrans*)m_pAnimableAmiplitude)->SetDestNum(0);
		}
		
		int nRead = 0;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_pos1_enable, &nRead);
		m_bPos1Enable = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_pos2_enable, &nRead);
		m_bPos2Enable = (nRead != 0);

		if (dwVersion >= 39)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_use_normal, &nRead);
			m_bUseNormal = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_normal, VECTORADDR_XYZ(m_vNormal));
		}

		if (dwVersion >= 73)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_filter, &m_nFilterType);
		}

		if (dwVersion >= 102)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_wavemoving, &nRead);
			m_bWaveMoving = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_wavemovingspeed, &m_fWaveMovingSpeed);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_fixwavelength, &nRead);
			m_bFixWaveLength = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_numwaves, &m_fNumWaves);
		}
	}

	if (m_nInterval < 50) m_nInterval = 50;

	Init(pDevice);
	return true;
}

bool A3DGFXLightning::Save(AFile* pFile)
{
	m_NoiseCtrl.Save(pFile);

	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(m_vStartPos.m, sizeof(m_vStartPos.m), &dwWrite);
		pFile->Write(m_vEndPos.m, sizeof(m_vEndPos.m), &dwWrite);
		pFile->Write(&m_nSegs, sizeof(m_nSegs), &dwWrite);
		pFile->Write(&m_nLightNum, sizeof(m_nLightNum), &dwWrite);
		pFile->Write(&m_fWaveLen, sizeof(m_fWaveLen), &dwWrite);
		pFile->Write(&m_nInterval, sizeof(m_nInterval), &dwWrite);
		pFile->Write(&m_fWidthStart, sizeof(m_fWidthStart), &dwWrite);
		pFile->Write(&m_fWidthEnd, sizeof(m_fWidthEnd), &dwWrite);
		pFile->Write(&m_fWidthMid, sizeof(m_fWidthMid), &dwWrite);
		pFile->Write(&m_fAlphaStart, sizeof(m_fAlphaStart), &dwWrite);
		pFile->Write(&m_fAlphaEnd, sizeof(m_fAlphaEnd), &dwWrite);
		pFile->Write(&m_fAlphaMid, sizeof(m_fAlphaMid), &dwWrite);
		pFile->Write(&m_fAmplitude, sizeof(m_fAmplitude), &dwWrite);
		pFile->Write(&m_bPos1Enable, sizeof(m_bPos1Enable), &dwWrite);
		pFile->Write(&m_bPos2Enable, sizeof(m_bPos2Enable), &dwWrite);
		pFile->Write(&m_bUseNormal, sizeof(m_bUseNormal), &dwWrite);
		pFile->Write(m_vNormal.m, sizeof(m_vNormal.m), &dwWrite);
		pFile->Write(&m_nFilterType, sizeof(m_nFilterType), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_start_pos, VECTOR_XYZ(m_vStartPos));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_end_pos, VECTOR_XYZ(m_vEndPos));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_segs, m_nSegs);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_light_num, m_nLightNum);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_wave_len, m_fWaveLen);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_interval, m_nInterval);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_width, m_fWidthStart);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_width, m_fWidthEnd);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_alpha, m_fAlphaStart);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_alpha, m_fAlphaEnd);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_width, m_fWidthMid);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_alpha, m_fAlphaMid);
		pFile->WriteLine(szLine);

		m_pAnimableAmiplitude->Save(pFile);
		/*
		sprintf(szLine, _format_ampitude, m_fAmplitude);
		pFile->WriteLine(szLine);
		*/

		sprintf(szLine, _format_pos1_enable, (int)m_bPos1Enable);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_pos2_enable, (int)m_bPos2Enable);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_use_normal, (int)m_bUseNormal);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_normal, VECTOR_XYZ(m_vNormal));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_filter, m_nFilterType);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_wavemoving, (int)m_bWaveMoving);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_wavemovingspeed, m_fWaveMovingSpeed);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_fixwavelength, (int)m_bFixWaveLength);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_numwaves, m_fNumWaves);
		pFile->WriteLine(szLine);
	}

	return true;
}

int A3DGFXLightning::GetRenderCount() const
{
	// For Original lightning, just return the number of pairs of vertice
	// But for LightningEx, this function will be overrided to return the appending number of pairs of vertice
	return m_nVertPairNum;
}

A3DVECTOR3 A3DGFXLightning::GetRenderPos1(const A3DVECTOR3& vViewUp, const A3DVECTOR3& vPos) const
{
	return vPos - vViewUp;
}

A3DVECTOR3 A3DGFXLightning::GetRenderPos2(const A3DVECTOR3& vViewUp, const A3DVECTOR3& vPos) const
{
	return vPos + vViewUp;
}

int A3DGFXLightning::GetVertsCount()
{
	return GetRenderCount() * 2;
}

int A3DGFXLightning::FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView)
{
	return UpdateVerts(pView, (A3DGFXVERTEX*)pBuffer, nMatrixIndex) ? GetRenderCount() * 2 : 0;
}

void A3DGFXLightning::RenderCurveMethod(const int i, 
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
										A3DGFXVERTEX* pVerts)
{
	if (i == 0)
	{
		A3DVECTOR3 vIntStart(matTran * m_pIntPos[0]);
		A3DVECTOR3 vDir(RotateVec(GetParentDir(), m_pIntDir[0]));
		vLast = vIntStart;

		A3DVECTOR3 vViewUp(CrossProduct(vDir, m_bUseNormal ? vNormal : vIntStart - vCamPos));
		vViewUp.Normalize();
		vLastViewUp = vViewUp;
		vViewUp *= fWidthStart;

		A3DVECTOR3 vStart1(GetRenderPos1(vViewUp, vIntStart));
		A3DVECTOR3 vStart2(GetRenderPos2(vViewUp, vIntStart));

		SET_VERT(pVerts[nBase], vStart1);
		SET_VERT(pVerts[nBase+1], vStart2);
		fAlpha = m_fAlphaStart;
	}
	else if (i + 1 == nRenderCount)
	{
		A3DVECTOR3 vIntEnd(matTran * m_pIntPos[i]);
		A3DVECTOR3 vDir(vIntEnd - vLast);
		A3DVECTOR3 vViewUp(CrossProduct(vDir, m_bUseNormal ? vNormal : vIntEnd - vCamPos));
		vViewUp.Normalize();

		if (DotProduct(vViewUp, vLastViewUp) < 0)
			vViewUp *= -1;

		vViewUp *= fWidthEnd;
		A3DVECTOR3 vEnd1(GetRenderPos1(vViewUp, vIntEnd));
		A3DVECTOR3 vEnd2(GetRenderPos2(vViewUp, vIntEnd));

		SET_VERT(pVerts[nBase], vEnd1);
		SET_VERT(pVerts[nBase+1], vEnd2);
		fAlpha = m_fAlphaEnd;
	}
	else
	{
		A3DVECTOR3 vPos(matTran * m_pRandPos[nCount]);
		A3DVECTOR3 vDir(vPos - vLast);
		vLast = vPos;
	
		A3DVECTOR3 vViewUp(CrossProduct(vDir, m_bUseNormal ? vNormal : vPos - vCamPos));
		vViewUp.Normalize();
		
		if (DotProduct(vViewUp, vLastViewUp) < 0)
			vViewUp *= -1;
		vLastViewUp = vViewUp;

		if (fRatio < .5f)
		{
			fRatio *= 2.0f;
			vViewUp *= fWidthStart * (1.f - fRatio) + fWidthMid * fRatio;
			fAlpha = m_fAlphaStart * (1.f - fRatio) + m_fAlphaMid * fRatio;
		}
		else
		{
			fRatio = (fRatio - .5f) * 2.0f;
			vViewUp *= fWidthMid * (1.f - fRatio) + fWidthEnd * fRatio;
			fAlpha = m_fAlphaMid * (1.f - fRatio) + m_fAlphaEnd * fRatio;
		}

		A3DVECTOR3 v1(GetRenderPos1(vViewUp, vPos));
		A3DVECTOR3 v2(GetRenderPos2(vViewUp, vPos));

		SET_VERT(pVerts[nBase], v1);
		SET_VERT(pVerts[nBase+1], v2);
		nCount++;
	}
}

bool A3DGFXLightning::UpdateVerts(A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex)
{
	if (m_nSegs == 0 || m_nLightNum == 0) return false;

	KEY_POINT kp;

	if (!GetCurKeyPoint(&kp))
		return false;

	if (m_pGfx->IsUsingOuterColor())
		kp.m_color = (A3DCOLORVALUE(kp.m_color) * m_pGfx->GetOuterColor()).ToRGBAColor();

	if (A3DGFXRenderSlotMan::g_RenderMode == GRMSoftware)
		m_pDevice->SetWorldMatrix(_identity_mat4);
	else if (A3DGFXRenderSlotMan::g_RenderMode == GRMVertexShader)
		m_pDevice->SetVertexShaderConstants(nMatrixIndex * 3 + GFX_VS_CONST_BASE, &_identity_mat4, 3);
	else
		m_pDevice->SetIndexedVertexBlendMatrix(nMatrixIndex, _identity_mat4);

	A3DMATRIX4 matTran;

	if (m_bPos1Enable || m_bPos2Enable)
		matTran.Identity();
	else if (m_bCurvedMethod)
		matTran = GetParentTM();
	else
		matTran = GetTranMatrix(kp) * GetParentTM();

	A3DVECTOR3 vStart, vEnd, vDir, vUp;
	float fDist;

	if (!m_bCurvedMethod)
	{
		vStart = matTran * m_vStartCur;
		vEnd = matTran * m_vEndCur;
		vDir = vEnd - vStart;
		fDist = vDir.Normalize();
		vUp = CalcVertVec(vDir);
	}

	static const A3DCOLOR spec = A3DCOLORRGBA(0, 0, 0, 255);
	A3DVECTOR3 vViewUp, vLastViewUp, vLast(0);
	A3DVECTOR3 vCamPos = pView->GetCamera()->GetPos();

	float fScale = m_pGfx->GetActualScale() * kp.m_fScale;
	float fWidthStart = m_fWidthStart * fScale;
	float fWidthEnd = m_fWidthEnd * fScale;
	float fWidthMid = m_fWidthMid * fScale;
	float fAmplitude = m_fAmplitude;

	int nStep = m_nSegs * 2;
	int nCount = 0;
	float fSegs = (float)m_nSegs;

	float fParentAlpha = m_pGfx->GetActualAlpha() * A3DCOLOR_GETALPHA(kp.m_color) / 255.0f;
	A3DCOLOR clParent = kp.m_color & A3DCOLORRGBA(255, 255, 255, 0);
	A3DVECTOR3 vNormal;

	if (m_bUseNormal)
		vNormal = RotateVec(GetParentDir(), m_vNormal);

#ifdef GFX_EDITOR
	m_AABB.Clear();
#endif

	int nRenderCount = GetRenderCount();
	for (int i = 0; i < nRenderCount; i++)
	{
		int nBase = i * 2;
		float fRatio = (float)i / (nRenderCount-1);
		float fU = m_fTexWid * (m_bUReverse ? 1.0f - fRatio : fRatio) + m_fTexU;
		float fAlpha = 1.0f;

		if (m_bCurvedMethod)
		{
			RenderCurveMethod(
				i, 
				nRenderCount, 
				nBase, 
				fRatio, 
				fWidthStart, 
				fWidthEnd, 
				fWidthMid,
				fAlpha, 
				nCount, 
				matTran, 
				vNormal, 
				vCamPos, 
				vLast, 
				vLastViewUp, 
				pVerts);

		}
		else
		{
			int nMod = i % nStep;
			int nRemain = i / m_nSegs;
			int nModSeg = i % m_nSegs;

			if (nMod == 0)
			{
				A3DVECTOR3 vView(vStart - vCamPos);
				vViewUp = CrossProduct(vDir, vView);
				vViewUp.Normalize();
				vViewUp *= fWidthStart;
				A3DVECTOR3 vStart1(vStart - vViewUp);
				A3DVECTOR3 vStart2(vStart + vViewUp);

				SET_VERT(pVerts[nBase], vStart1);
				SET_VERT(pVerts[nBase+1], vStart2);
				fAlpha = m_fAlphaStart;
			}
			else if (nMod == m_nSegs)
			{
				A3DVECTOR3 vView(vEnd - vCamPos);
				vViewUp = CrossProduct(vDir, vView);
				vViewUp.Normalize();
				vViewUp *= fWidthEnd;
				A3DVECTOR3 vEnd1(vEnd - vViewUp);
				A3DVECTOR3 vEnd2(vEnd + vViewUp);

				SET_VERT(pVerts[nBase], vEnd1);
				SET_VERT(pVerts[nBase+1], vEnd2);
				fAlpha = m_fAlphaEnd;
			}
			else
			{
				A3DVECTOR3 vPos;
				if (!m_bWaveMoving)
				{
					A3DQUATERNION q(vDir, m_pData[1][nCount]);
					A3DVECTOR3 vRandUp(RotateVec(q, vUp));
					vPos = (vStart + vDir * (m_pData[0][nCount] * fDist) + vRandUp * (m_pData[2][nCount] * fAmplitude));
					A3DVECTOR3 vView(vPos - vCamPos);
					vViewUp = CrossProduct(vDir, vView);
					vViewUp.Normalize();
				}
				else
				{
					A3DQUATERNION q(vDir, m_pData[1][nCount]);
					A3DVECTOR3 vRandUp(RotateVec(q, vUp));
					vPos = vStart + vDir * (m_pData[0][nCount] * fDist);
					A3DVECTOR3 vView(vPos - vCamPos);
					vViewUp = CrossProduct(vDir, vView);
					vViewUp.Normalize();
					vPos +=  vViewUp * (m_pData[2][nCount] * fAmplitude);
				}
				
				fRatio = ((nRemain % 2) ? (m_nSegs - nModSeg) : nModSeg) / fSegs;

				if (fRatio < .5f)
				{
					fRatio *= 2.0f;
					vViewUp *= fWidthStart * (1.f - fRatio) + fWidthMid * fRatio;
					fAlpha = m_fAlphaStart * (1.f - fRatio) + m_fAlphaMid * fRatio;
				}
				else
				{
					fRatio = (fRatio - .5f) * 2.0f;
					vViewUp *= fWidthMid * (1.f - fRatio) + fWidthEnd * fRatio;
					fAlpha = m_fAlphaMid * (1.f - fRatio) + m_fAlphaEnd * fRatio;
				}

				A3DVECTOR3 v1(vPos - vViewUp);
				A3DVECTOR3 v2(vPos + vViewUp);

				SET_VERT(pVerts[nBase], v1);
				SET_VERT(pVerts[nBase+1], v2);
				nCount++;
			}
		}

		fAlpha *= fParentAlpha;
		A3DCOLOR cl = clParent | A3DCOLORRGBA(0, 0, 0, (BYTE)(fAlpha * 255.f));

		float u1, u2, v1, v2;

		if (m_bVReverse)
		{
			v1 = m_fTexV;
			v2 = m_fTexV + m_fTexHei;
		}
		else
		{
			v1 = m_fTexV + m_fTexHei;
			v2 = m_fTexV;
		}

		if (m_bUVInterchange)
		{
			u1 = v1;
			u2 = v2;
			v1 = fU;
			v2 = fU;
		}
		else
		{
			u1 = fU;
			u2 = fU;
		}

		pVerts[nBase].dwMatIndices = nMatrixIndex;
		pVerts[nBase].tu		= u1;
		pVerts[nBase].tv		= v1;
		pVerts[nBase].diffuse	= cl;
		pVerts[nBase].specular	= spec;
		pVerts[nBase+1].dwMatIndices = nMatrixIndex;
		pVerts[nBase+1].tu		= u2;
		pVerts[nBase+1].tv		= v2;
		pVerts[nBase+1].diffuse = cl;
		pVerts[nBase+1].specular= spec;

#ifdef GFX_EDITOR
		{
			for (int i = 0; i < 2; i++)
			{
				A3DGFXVERTEX* pVert = &pVerts[nBase+i];
				A3DAABB& aabb = m_AABB;
				if (pVert->x < aabb.Mins.x) aabb.Mins.x = pVert->x;
				if (pVert->x > aabb.Maxs.x) aabb.Maxs.x = pVert->x;
				if (pVert->y < aabb.Mins.y) aabb.Mins.y = pVert->y;
				if (pVert->y > aabb.Maxs.y) aabb.Maxs.y = pVert->y;
				if (pVert->z < aabb.Mins.z) aabb.Mins.z = pVert->z;
				if (pVert->z > aabb.Maxs.z) aabb.Maxs.z = pVert->z;
			}
		}
#endif
	}

	return true;
}

inline float filter(int nFilterType, float fDist, float fValue)
{
	if (fDist < 0.f)
		return fValue;

	if (fDist > 1.f)
		fDist = (fDist - (int)fDist);

	switch(nFilterType)
	{
	case A3DGFXLightning::FT_TRIANGLE:
		return 2 * (fDist > 0.5 ? 1 - fDist : fDist);
	case A3DGFXLightning::FT_RECTANGLE:
		return fValue;
	case A3DGFXLightning::FT_TRAPEZIA:
		return fValue * (fDist < 0.2 ? fDist * 5 : fDist > 0.8 ? (1 - fDist) * 5 : 1);
	case A3DGFXLightning::FT_SIN:
		return fValue * sinf(fDist * A3D_PI);
	}
	return fValue;
}

void A3DGFXLightning::BuildPath(const A3DMATRIX4& matTran)
{
	float fDist;

	if (m_bCurvedMethod)
		fDist = m_fCurvedLen;
	else
	{
		A3DVECTOR3 vDir(m_vEndPos - m_vStartPos);
		fDist = vDir.Normalize();

		A3DVECTOR3 vUp(RotateVec(A3DQUATERNION(vDir, _UnitRandom() * A3D_2PI), CalcVertVec(vDir)));
		A3DVECTOR3 vNewDir(RotateVec(A3DQUATERNION(vUp, _UnitRandom() * m_fDeviation), vDir));
		m_vStartCur = m_vStartPos;
		m_vEndCur = m_vStartCur + vNewDir * fDist;
		A3DVECTOR3 vStart(matTran * m_vStartCur);
		A3DVECTOR3 vEnd(matTran * m_vEndCur);
		fDist = (vStart - vEnd).Magnitude();	
	}

	float fWaves = fDist / m_fWaveLen;
	float fSeg = fWaves / m_nSegs;
	float fStart = (float)a_Random(0, 1022);
	int nCount = m_nSegs * m_nLightNum;
	float fRand[2];
	float fSegs = (float)m_nSegs;

	for (int i = 0; i < nCount; i++)
	{
		float fInv = i / fSegs;

		if (!m_bWaveMoving)
		{
			m_NoiseCtrl.GetNoise().GetValue(
				fStart + i * fSeg,
				fRand,
				2);

			m_pData[1][i] = fRand[0] * A3D_2PI;
			m_pData[2][i] = filter(m_nFilterType, fInv, fRand[1]);

			if (m_bCurvedMethod)
			{
				A3DVECTOR3 vRandUp(RotateVec(A3DQUATERNION(m_pIntDir[i+1], m_pData[1][i]), m_pIntUp[i]));
				m_pRandPos[i] = m_pIntPos[i+1] + vRandUp * (m_pData[2][i] * m_fAmplitude);
			}
		}
		else
		{
			float fLoop = m_bFixWaveLength ? fWaves : m_fNumWaves;
			float fDelta = ::GetTickCount() * 0.01f * m_fWaveMovingSpeed;

			m_pData[1][i] = 0;
			m_pData[2][i] = sinf(fInv * A3D_PI * fLoop) * sinf(fDelta);

			if (m_bCurvedMethod)
			{
				m_pRandPos[i] = m_pIntPos[i+1] + (m_pData[2][i] * m_fAmplitude);
			}
		}
	}

}

void A3DGFXLightning::UpdatePath()
{
	KEY_POINT kp;
	GetCurKeyPoint(&kp);

	DWORD dwTimeElapsed = m_KeyPointSet.GetTimeElapsed();
	m_pAnimableAmiplitude->GetValue(dwTimeElapsed).GetValue(m_fAmplitude);

	BuildPath(GetTranMatrix(kp));
}
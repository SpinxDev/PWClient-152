#include "StdAfx.h"
#include "A3DTrail.h"
#include "A3DConfig.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DGFXMan.h"
#include "A3DCDS.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DGFXExMan.h"
#include "TrailList.h"

static const float TRAIL_LERP_ANGLE = DEG2RAD(6.f);
static const DWORD _inner_tick = 40;

static const char _format_orgpos1[]	= "OrgPos1: %f, %f, %f";
static const char _format_orgpos2[]	= "OrgPos2: %f, %f, %f";
static const char _format_en_mat[]	= "EnableMat: %d";
static const char _format_en_pos1[]	= "EnableOrgPos1: %d";
static const char _format_en_pos2[]	= "EnableOrgPos2: %d";
static const char _format_seglife[]	= "SegLife: %d";
static const char _format_bind[]	= "Bind: %d";
static const char _format_face_cam[]	= "FaceCamera: %d";
static const char _format_splinemode[] = "Spline: %d";
static const char _format_samplefreq[] = "SampleFreq: %d";
static const char _format_perturb_mode[] = "PerturbMode: %d";
static const char _format_perturb_disappear_delay[] = "DisappearDelay: %f";
static const char _format_perturb_disappear_speed[] = "DisappearSpeed: %f";
static const char _format_perturb_disappear_accel[] = "DisappearAcceleration: %f";
static const char _format_perturb_spread_speed[] = "SpreadSpeed: %f";
static const char _format_perturb_spread_delay[] = "SpreadDelay: %f";
static const char _format_perturb_spread_seg_count[] = "SpreadSegCount: %d";
static const char _format_perturb_spread_accel[] = "SpreadAcceleration: %f";
static const char _format_perturb_spread_dir_range_min[] = "SpreadDirRangeMin: %f, %f, %f";
static const char _format_perturb_spread_dir_range_max[] = "SpreadDirRangeMax: %f, %f, %f";


//////////////////////////////////////////////////////////////////////////
//
//	Implement A3DTrail
//
//////////////////////////////////////////////////////////////////////////

A3DTrail::A3DTrail(A3DGFXEx* pGfx) 
: A3DGFXElement(pGfx)
{
	m_nEleType			= ID_ELE_TYPE_TRAIL;
	m_nSegLife			= 0;
	m_bBind				= false;
	m_bMatrixEnable		= false;
	m_bOrgPos1Enable	= false;
	m_bOrgPos2Enable	= false;
	m_bFirstSeg			= true;
	m_bOrgPos1Set		= false;
	m_bOrgPos2Set		= false;
	m_bOrgMatSet		= false;
	m_bHostMatSet		= false;
	m_vOrgPos[0].Clear();
	m_vOrgPos[1].Clear();
	m_dwDeltaTm			= 0;
	m_bAddOutData		= false;
	m_bTLVert			= false;
	m_iSplineMode		= SPLINE_MODE;
	m_iSampleFreq		= 5;
    m_dwLife            = 0xFFFFFFFF;
    m_dwKeyTickTime     = 0;
#ifdef GFX_EDITOR
	// 只有编辑器里才需要在构造函数里create（添加一个新元素的时候需要这样）
	m_pTrailImpl		= ITrailList::CreateTrailList(IsSplineMode());
	m_pTrailImpl->SetSampleFreq(m_iSampleFreq);
#else
	// 正常情况下，对象会在创建之后Load，于是会在Load中创建TrailImpl对象
	m_pTrailImpl		= NULL;
#endif

}

A3DTrail::~A3DTrail()
{
	ITrailList::DestroyTrailList(m_pTrailImpl);
	m_pTrailImpl = NULL;
}

bool A3DTrail::Init(A3DDevice* pDevice)
{
	A3DGFXElement::Init(pDevice);
	return true;
}

A3DGFXElement* A3DTrail::Clone(A3DGFXEx* pGfx) const
{
	A3DTrail* p = new A3DTrail(pGfx);
	return &(*p = *this);
}

A3DTrail& A3DTrail::operator = (const A3DTrail& src)
{
	if (&src == this)
		return *this;
	
	_CloneBase(&src);
	Init(src.m_pDevice);
	m_vOrgPos[0]	= src.m_vOrgPos[0];
	m_vOrgPos[1]	= src.m_vOrgPos[1];
	m_nSegLife		= src.m_nSegLife;
	m_bBind			= src.m_bBind;
	m_bOrgPos1Enable= src.m_bOrgPos1Enable;
	m_bOrgPos2Enable= src.m_bOrgPos2Enable;
	m_bMatrixEnable	= src.m_bMatrixEnable;
	m_iSampleFreq	= src.m_iSampleFreq;

	if (m_iSplineMode != src.m_iSplineMode)
	{
		m_iSplineMode	= src.m_iSplineMode;
		ITrailList::DestroyTrailList(m_pTrailImpl);
		m_pTrailImpl = ITrailList::CreateTrailList(IsSplineMode());
		assert(m_pTrailImpl);
	}
	else if (m_pTrailImpl == NULL)
	{
		m_pTrailImpl = ITrailList::CreateTrailList(IsSplineMode());
		assert(m_pTrailImpl);
	}

	if (m_pTrailImpl)
		m_pTrailImpl->SetSampleFreq(m_iSampleFreq);

	return *this;
}

bool A3DTrail::Play()
{
	if (!IsInit())
	{
		InitBaseData();

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

		SetInit(true);
	}

	assert(m_pTrailImpl);
	if (m_pTrailImpl)
		m_pTrailImpl->Reset();

	m_bFirstSeg = true;
	m_bOrgPos1Set = false;
	m_bOrgPos2Set = false;
	m_bOrgMatSet = false;
	m_bHostMatSet = false;
	m_bAddOutData = false;
    m_dwKeyTickTime = 0;
	return true;
}

void A3DTrail::AddTrailSeg(
	const A3DMATRIX4& matTran,
	DWORD dwTickTime,
	A3DCOLOR cl,
	float fScale)
{
	assert(m_pTrailImpl);
	if (!m_pTrailImpl)
		return;

	A3DVECTOR3 vCenterOrg = (m_vOrgPos[0] + m_vOrgPos[1]) * .5f;
	A3DVECTOR3 vHalf = (m_vOrgPos[1] - m_vOrgPos[0]) * (fScale / 2.f);
	A3DVECTOR3 vPos0 = vCenterOrg - vHalf;
	A3DVECTOR3 vPos1 = vCenterOrg + vHalf;
	
	// 平滑曲线模式下，不再需要考虑与上一帧的角度差，并添加多余的点（只添加关键点即可）
	switch (m_iSplineMode)
	{
	case SPLINE_MODE:
        m_dwKeyTickTime += dwTickTime;
        if (m_dwKeyTickTime > 10)
        {
		    m_pTrailImpl->AddKeyData(TRAIL_DATA(matTran * vPos0, matTran * vPos1, m_nSegLife, cl));
            m_dwKeyTickTime = 0;
        }
		break;
	case LINE_KP_MODE:
		if (m_KeyPointSet.GetActKeyPoint() != m_iCurActKeyPoint)
		{
			m_iCurActKeyPoint = m_KeyPointSet.GetActKeyPoint();
			m_pTrailImpl->AddKeyData(TRAIL_DATA(matTran * vPos0, matTran * vPos1, m_nSegLife, cl));
		}
		break;
	case LINE_MODE:
	default:
		{
			// 非平滑曲线模式下，为了保持和以前的表现一致，保留下面的逻辑
			A3DQUATERNION quNow;
			A3DMATRIX4 matTemp;
			matTemp.SetRow(0, Normalize(matTran.GetRow(0)));
			matTemp.SetRow(1, Normalize(matTran.GetRow(1)));
			matTemp.SetRow(2, Normalize(matTran.GetRow(2)));
			quNow.ConvertFromMatrix(matTemp);
			A3DVECTOR3 vNow = matTran.GetRow(3);

			if (m_bFirstSeg)
			{
				A3DVECTOR3 vPos[2] = { matTran * vPos0, matTran * vPos1 };
				m_bFirstSeg = false;
				m_pTrailImpl->AddKeyData(TRAIL_DATA(
					vPos[0],
					vPos[1],
					m_nSegLife,
					cl));
			}
			else
			{
				float fAngle = fabsf(DotProduct(m_quLastDir, quNow));
				
				if (fAngle >= 1.f)
					fAngle = 0.f;
				else
					fAngle = (float)acos(fAngle);
				
				int nSegs = (int) (fAngle / TRAIL_LERP_ANGLE) + 1;
				
				if (nSegs == 1)
				{
					A3DVECTOR3 vPos[2] = { matTran * vPos0, matTran * vPos1 };
					m_pTrailImpl->AddKeyData(TRAIL_DATA(
						vPos[0],
						vPos[1],
						m_nSegLife,
						cl));
				}
				else
				{
					float fSegs = (float)nSegs;
					for (int i = 1; i <= nSegs; i++)
					{
						float f = i / fSegs;
						A3DVECTOR3 v = m_vLastPos * (1.0f - f) + vNow * f;
						A3DQUATERNION q = SLERPQuad(m_quLastDir, quNow, f);
						A3DMATRIX4 mat;
						q.ConvertToMatrix(mat);
						
						if (!m_bBind)
							mat = mat * m_pGfx->GetScaleMat();
						
						mat.SetRow(3, v);
						
						int nSegLife = m_nSegLife - (int)((nSegs - i) / fSegs * dwTickTime);
						if (nSegLife < 0) nSegLife = 0;
						A3DVECTOR3 vPos[2] = { mat * vPos0, mat * vPos1 };
						m_pTrailImpl->AddKeyData(TRAIL_DATA(
							vPos[0],
							vPos[1],
							nSegLife,
							cl));
					}
				}
			}

			m_quLastDir		= quNow;
			m_vLastPos		= vNow;
		}
	}

}

bool A3DTrail::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	TRAILMODE	iSplineMode = GetDefaultMode();

	if (pFile->IsBinary())
	{
		pFile->Read(m_vOrgPos[0].m, sizeof(m_vOrgPos[0].m), &dwReadLen);
		pFile->Read(m_vOrgPos[1].m, sizeof(m_vOrgPos[1].m), &dwReadLen);
		pFile->Read(&m_bMatrixEnable, sizeof(m_bMatrixEnable), &dwReadLen);
		pFile->Read(&m_bOrgPos1Enable, sizeof(m_bOrgPos1Enable), &dwReadLen);
		pFile->Read(&m_bOrgPos2Enable, sizeof(m_bOrgPos2Enable), &dwReadLen);
		pFile->Read(&m_nSegLife, sizeof(m_nSegLife), &dwReadLen);
		pFile->Read(&m_bBind, sizeof(m_bBind), &dwReadLen);

		if (dwVersion >= 122)
		{
			pFile->Read(&m_perturbParam.m_iPerturbMode, sizeof(m_perturbParam.m_iPerturbMode), &dwReadLen);

			if(m_perturbParam.m_iPerturbMode == eTrailPerturbMode_Spreading)
			{
				pFile->Read(&m_perturbParam.m_fDisappearSpeed, sizeof(m_perturbParam.m_fDisappearSpeed), &dwReadLen);
				pFile->Read(&m_perturbParam.m_fSpreadSpeed, sizeof(m_perturbParam.m_fSpreadSpeed), &dwReadLen);
				pFile->Read(&m_perturbParam.m_iSegCntToShareDir, sizeof(m_perturbParam.m_iSegCntToShareDir), &dwReadLen);
				pFile->Read(&m_perturbParam.m_fSpreadAccel, sizeof(m_perturbParam.m_fSpreadAccel), &dwReadLen);
				pFile->Read(&m_perturbParam.m_vSpreadDirMinRange, sizeof(m_perturbParam.m_vSpreadDirMinRange), &dwReadLen);
				pFile->Read(&m_perturbParam.m_vSpreadDirMaxRange, sizeof(m_perturbParam.m_vSpreadDirMaxRange), &dwReadLen);
				pFile->Read(&m_perturbParam.m_fDisappearAccel, sizeof(m_perturbParam.m_fDisappearAccel), &dwReadLen);
				pFile->Read(&m_perturbParam.m_fSpreadDelay, sizeof(m_perturbParam.m_fSpreadDelay), &dwReadLen);
				pFile->Read(&m_perturbParam.m_fDisappearDelay, sizeof(m_perturbParam.m_fDisappearDelay), &dwReadLen);
			}
		}

		if (dwVersion >= 123)
		{
			pFile->Read(&m_bFaceCamera, sizeof(m_bFaceCamera), &dwReadLen);
		}
		else
		{
			m_bFaceCamera = false;
		}
	}
	else
	{
		int nRead;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_orgpos1, VECTORADDR_XYZ(m_vOrgPos[0]));
		
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_orgpos2, VECTORADDR_XYZ(m_vOrgPos[1]));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_en_mat, &nRead);
		m_bMatrixEnable = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_en_pos1, &nRead);
		m_bOrgPos1Enable = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_en_pos2, &nRead);
		m_bOrgPos2Enable = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_seglife, &m_nSegLife);

		if (dwVersion >= 18)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_bind, &nRead);
			m_bBind = (nRead != 0);
		}

		if (dwVersion < 80)
		{
			// 空间扭曲从版本80开始起作用
			m_bWarp = false;
		}

		if (dwVersion >= 87)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_splinemode, &iSplineMode);
		}
		else
		{
			// 版本87以前的都是不支持SplineMode的，将其值置为默认
			iSplineMode = GetDefaultMode();
		}

		if (dwVersion >= 111)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_samplefreq, &m_iSampleFreq);
		}
		else
		{
			// 默认值为5ms
			m_iSampleFreq = 5;
		}

		if (dwVersion >= 122)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_perturb_mode, &m_perturbParam.m_iPerturbMode);

			if(m_perturbParam.m_iPerturbMode == eTrailPerturbMode_Spreading)
			{
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_perturb_disappear_speed, &m_perturbParam.m_fDisappearSpeed);

				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_perturb_spread_speed, &m_perturbParam.m_fSpreadSpeed);

				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_perturb_spread_seg_count, &m_perturbParam.m_iSegCntToShareDir);

				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_perturb_spread_accel, &m_perturbParam.m_fSpreadAccel);

				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_perturb_spread_dir_range_min, VECTORADDR_XYZ(m_perturbParam.m_vSpreadDirMinRange));

				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_perturb_spread_dir_range_max, VECTORADDR_XYZ(m_perturbParam.m_vSpreadDirMaxRange));

				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_perturb_disappear_accel, &m_perturbParam.m_fDisappearAccel);

				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_perturb_spread_delay, &m_perturbParam.m_fSpreadDelay);

				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_perturb_disappear_delay, &m_perturbParam.m_fDisappearDelay);
			}
		}

		if (dwVersion >= 123)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_face_cam, &nRead);
			m_bFaceCamera = nRead != 0;
		}
		else
		{
			m_bFaceCamera = false;
		}
	}
	
	// 如果读到的类型与默认不同，则销毁（仅针对编辑器）
	if (m_iSplineMode != iSplineMode)
	{
		m_iSplineMode = iSplineMode;
		if (m_pTrailImpl)
		{
			ITrailList::DestroyTrailList(m_pTrailImpl);
			m_pTrailImpl = NULL;
		}
	}
	
	// 如果m_pTrailImpl为空，则创建一个新的，针对客户端
	if (m_pTrailImpl == NULL)
	{
		m_iSplineMode = iSplineMode;
		m_pTrailImpl = ITrailList::CreateTrailList(IsSplineMode());
	}

	m_pTrailImpl->SetSampleFreq(m_iSampleFreq);

	assert(m_pTrailImpl);
	return true;
}

bool A3DTrail::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(m_vOrgPos[0].m, sizeof(m_vOrgPos[0].m), &dwWrite);
		pFile->Write(m_vOrgPos[1].m, sizeof(m_vOrgPos[1].m), &dwWrite);
		pFile->Write(&m_bMatrixEnable, sizeof(m_bMatrixEnable), &dwWrite);
		pFile->Write(&m_bOrgPos1Enable, sizeof(m_bOrgPos1Enable), &dwWrite);
		pFile->Write(&m_bOrgPos2Enable, sizeof(m_bOrgPos2Enable), &dwWrite);
		pFile->Write(&m_nSegLife, sizeof(m_nSegLife), &dwWrite);
		pFile->Write(&m_bBind, sizeof(m_bBind), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_orgpos1, VECTOR_XYZ(m_vOrgPos[0]));
		pFile->WriteLine(szLine);
		
		sprintf(szLine, _format_orgpos2, VECTOR_XYZ(m_vOrgPos[1]));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_en_mat, (int)m_bMatrixEnable);
		pFile->WriteLine(szLine);
 
		sprintf(szLine, _format_en_pos1, (int)m_bOrgPos1Enable);
		pFile->WriteLine(szLine);
		
		sprintf(szLine, _format_en_pos2, (int)m_bOrgPos2Enable);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_seglife, m_nSegLife);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_bind, (int)m_bBind);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_splinemode, (int)m_iSplineMode);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_samplefreq, m_iSampleFreq);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DTrail::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;

	assert(m_pTrailImpl);
	if (!m_pTrailImpl)
		return true;

	// how many key control points do we have?
	// if there is only 1 or less, we have nothing to render.
	int nSegNum = m_pTrailImpl->GetCount();
	if (nSegNum <= 1) return true;

	m_pTrailImpl->PrepareRenderData();
	// For space wrap support
	if (m_bWarp)
	{
		AfxGetGFXExMan()->AddWarpEle(this);
		return false;
	}

	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}

bool A3DTrail::InnerTick(DWORD dwTickTime)
{
	assert(m_pTrailImpl);
	
	if (!A3DGFXElement::TickAnimation(dwTickTime))
		return false;

	if (m_bMatrixEnable && !m_bAddOutData)
		return true;
	else if (m_bOrgPos1Enable || m_bOrgPos2Enable)
	{
		KEY_POINT kp;
		GetCurKeyPoint(&kp);

		// we suppose m_pTrailImpl is always valid after load
		// but there may be someone new such an element, and call tick without ?loading?, in order to
		// avoid that ridiculous thing, we add this test
		if (m_pTrailImpl)
			m_pTrailImpl->AddKeyData(TRAIL_DATA(m_vOrgPos[0],	m_vOrgPos[1], m_nSegLife, kp.m_color));

		//m_TrailLst.AddData(TRAIL_DATA(m_vOrgPos[0],	m_vOrgPos[1], m_nSegLife, kp.m_color));

		return true;
	}
	else
	{
		KEY_POINT kp, kpSelf;
		A3DMATRIX4 matTran;

		GetCurKeyPoint(&kp);
		matTran = GetTranMatrix(kp);

		if (m_pBind)
		{
			GetSelfKeyPoint(&kpSelf);
			matTran = GetTranMatrix(kpSelf) * matTran;
		}

		AddTrailSeg(
			m_bBind ? matTran : matTran * GetParentTM(),
			dwTickTime,
			kp.m_color,
			kp.m_fScale);

		return false;
	}
}

bool A3DTrail::TickAnimation(DWORD dwTickTime)
{
	if (dwTickTime)
	{
		assert(m_pTrailImpl);
		if (m_pTrailImpl)
			m_pTrailImpl->Tick(dwTickTime);
		InnerTick(dwTickTime);
	}
	return true;
}

void A3DTrail::ResumeLoop() 
{
	A3DGFXElement::ResumeLoop();
	m_bFirstSeg = true;

	assert (m_pTrailImpl);
	if (m_pTrailImpl)
		m_pTrailImpl->Reset();
}

// Space wrap support
// Only 3D trail, no TLVert mode
int A3DTrail::DrawToBuffer(A3DViewport* pView, A3DTLWARPVERTEX* pVerts, int nMaxVerts, float rw, float rh)
{
	return 0;
}

int A3DTrail::DrawToBuffer(A3DViewport* pView, A3DWARPVERTEX* pVerts, int nMaxVerts)
{
	assert(m_pTrailImpl);
	if (!m_pTrailImpl || m_pTrailImpl->GetCount() <= 1)
		return 0;

	if (GetVertsCount() > nMaxVerts)
		return 0;

	//int nCount = m_TrailLst.GetDataCount();
	A3DMATRIX4 matWorld;

	if (m_bMatrixEnable)
	{
		if (m_bHostMatSet)
			matWorld = m_matHost;
		else
			matWorld.Identity();
	}
	else if (!m_bBind || m_bOrgPos1Enable || m_bOrgPos2Enable)
		matWorld.Identity();
	else
		matWorld = GetParentTM();

	float fInvSegLife = 1.0f / (float)m_nSegLife;
	float fParentAlpha = m_pGfx->GetActualAlpha();
	float uS, uE;

	if (m_bUReverse)
	{
		uS = m_fTexU + m_fTexWid;
		uE = m_fTexU;
	}
	else
	{
		uS = m_fTexU;
		uE = m_fTexU + m_fTexWid;
	}

	const float f1Div255 = 1.0f / 255.0f;

	//TRAIL_DATA& tdStart = m_TrailLst[0];
	TRAIL_DATA tdStart;
	m_pTrailImpl->GetRenderTrailData(0, tdStart);
	
	//float fCurPortion = tdStart.m_nSegLife * fInvSegLife;
    float fCurPortion = GetPortionFactor(0);

    float fCurAlpha = A3DCOLOR_GETALPHA(tdStart.m_Color) * f1Div255;
	fCurAlpha *= GetPortionAlpha(fCurPortion) * fParentAlpha;
		
	float fCurU1, fCurU2, fCurV1, fCurV2;
	
	fCurU1 = uS;
	fCurU2 = uE;
	fCurV1 = fCurV2 = m_fTexV + (m_bVReverse ? 1.0f - fCurPortion : fCurPortion) * m_fTexHei;
	
	if (m_bUVInterchange)
	{
		gfx_swap(fCurU1, fCurV1);
		gfx_swap(fCurU2, fCurV2);
	}

#ifdef GFX_EDITOR
	
		m_AABB.Clear();

#endif
	
	int nRenderCount = m_pTrailImpl->GetRenderCount();
	for (int i = 0; i < nRenderCount - 1; i++)
	{
		//TRAIL_DATA& td = m_TrailLst[i];
		//TRAIL_DATA& td2 = m_TrailLst[i + 1];
		TRAIL_DATA td, td2;
		m_pTrailImpl->GetRenderTrailData(i, td);
		m_pTrailImpl->GetRenderTrailData(i + 1, td2);

		int nBase = i * 4;
		if (nBase + 3 >= nMaxVerts)
			break;

		float fPortion = GetPortionFactor(i + 1); //td2.m_nSegLife * fInvSegLife;
		float fAlpha = A3DCOLOR_GETALPHA(td2.m_Color) * f1Div255;
		fAlpha *= GetPortionAlpha(fPortion) * fParentAlpha;

		float u1, u2, v1, v2;

		u1 = uS;
		u2 = uE;
		v1 = v2 = m_fTexV + (m_bVReverse ? 1.0f - fPortion : fPortion) * m_fTexHei;

		if (m_bUVInterchange)
		{
			gfx_swap(u1, v1);
			gfx_swap(u2, v2);
		}

		pVerts[nBase].pos		= td.m_vSeg1 * matWorld;
		pVerts[nBase].u1		= fCurU1;
		pVerts[nBase].v1		= fCurV1;
		pVerts[nBase].u3		= fCurAlpha;
		pVerts[nBase].v3		= fCurAlpha;

		pVerts[nBase + 1].pos	= td2.m_vSeg1 * matWorld;
		pVerts[nBase + 1].u1	= u1;
		pVerts[nBase + 1].v1	= v1;
		pVerts[nBase + 1].u3	= fAlpha;
		pVerts[nBase + 1].v3	= fAlpha;

		pVerts[nBase + 2].pos	= td.m_vSeg2 * matWorld;
		pVerts[nBase + 2].u1	= fCurU2;
		pVerts[nBase + 2].v1	= fCurV2;
		pVerts[nBase + 2].u3	= fCurAlpha;
		pVerts[nBase + 2].v3	= fCurAlpha;


		pVerts[nBase + 3].pos	= td2.m_vSeg2 * matWorld;
		pVerts[nBase + 3].u1	= u2;
		pVerts[nBase + 3].v1	= v2;
		pVerts[nBase + 3].u3	= fAlpha;
		pVerts[nBase + 3].v3	= fAlpha;

		fCurPortion = fPortion;
		fCurAlpha = fAlpha;
		fCurU1 = u1;
		fCurV1 = v1;
		fCurU2 = u2;
		fCurV2 = v2;

#ifdef GFX_EDITOR
			
		A3DAABB& aabb = m_AABB;
		aabb.AddVertex(td.m_vSeg1 * matWorld);
		aabb.AddVertex(td.m_vSeg2 * matWorld);
		aabb.AddVertex(td2.m_vSeg1 * matWorld);
		aabb.AddVertex(td2.m_vSeg2 * matWorld);
		
#endif
	}

	return (nRenderCount - 1) * 4;
}

void A3DTrail::DummyTick(DWORD dwTick)
{
	assert(m_pTrailImpl && m_pDummyMat);

	if (m_pTrailImpl)
		m_pTrailImpl->Tick(dwTick);

	AddTrailSeg(
		*m_pDummyMat,
		dwTick,
		m_clDummy,
		m_fDummyScale
		);
}

int A3DTrail::GetVertsCount()
{
	assert(m_pTrailImpl);
	if (!m_pTrailImpl)
		return 0;

	return m_pTrailImpl->GetCount() > 1 ? m_pTrailImpl->GetRenderCount() * 2 : 0;
	//return m_TrailLst.GetBufSize() * 2;
}

int A3DTrail::FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView)
{
	assert(m_pTrailImpl);

	if (!m_pTrailImpl || m_pTrailImpl->GetCount() <= 1)
		return 0;

	A3DGFXVERTEX* pVerts = (A3DGFXVERTEX*)pBuffer;
	A3DMATRIX4 matWorld;

	if (m_bMatrixEnable)
	{
		if (m_bHostMatSet)
			matWorld = m_matHost;
		else
			matWorld.Identity();
	}
	else if (!m_bBind || m_bOrgPos1Enable || m_bOrgPos2Enable)
		matWorld.Identity();
	else
		matWorld = GetParentTM();

	if (A3DGFXRenderSlotMan::g_RenderMode == GRMSoftware)
		m_pDevice->SetWorldMatrix(matWorld);
	else if (A3DGFXRenderSlotMan::g_RenderMode == GRMVertexShader)
	{
		matWorld.Transpose();
		m_pDevice->SetVertexShaderConstants(nMatrixIndex * 3 + GFX_VS_CONST_BASE, &matWorld, 3);
	}
	else
		m_pDevice->SetIndexedVertexBlendMatrix(nMatrixIndex, matWorld);

	const float fSegLife = (float)m_nSegLife;
	float fParentAlpha = m_pGfx->GetActualAlpha();
	float uS, uE;

	if (m_bUReverse)
	{
		uS = m_fTexU + m_fTexWid;
		uE = m_fTexU;
	}
	else
	{
		uS = m_fTexU;
		uE =  m_fTexU + m_fTexWid;
	}

#ifdef GFX_EDITOR

	m_AABB.Clear();

#endif

	int iRenderIdx, nRenderCount = m_pTrailImpl->GetRenderCount();
	for (iRenderIdx = 0; iRenderIdx < nRenderCount; ++iRenderIdx)
	{
		const int nBase = iRenderIdx * 2;
		TRAIL_DATA td;
		m_pTrailImpl->GetRenderTrailData(iRenderIdx, td);
		float fPortion = GetPortionFactor(iRenderIdx); // td.m_nSegLife / fSegLife;
		float fAlpha = A3DCOLOR_GETALPHA(td.m_Color) * (1.f / 255.0f);
		fAlpha *= GetPortionAlpha(fPortion) * fParentAlpha;
		
		A3DVECTOR3& vPos1 = td.m_vSeg1;
		A3DVECTOR3& vPos2 = td.m_vSeg2;
		float u1, u2, v1, v2;

        //float fTexPortion = iRenderIdx / (float) nRenderCount;
		u1 = uS;
		u2 = uE;
		v1 = v2 = m_fTexV + (m_bVReverse ? 1.0f - fPortion : fPortion) * m_fTexHei;

		if (m_bUVInterchange)
		{
			gfx_swap(u1, v1);
			gfx_swap(u2, v2);
		}

		A3DCOLOR diffuse = SET_ALPHA(td.m_Color, (int)(fAlpha*255));
		if (m_Shader.SrcBlend == A3DBLEND_ONE)
		{
			int r = int(A3DCOLOR_GETRED(diffuse) * fAlpha);
			int g = int(A3DCOLOR_GETGREEN(diffuse) * fAlpha);
			int b = int(A3DCOLOR_GETBLUE(diffuse) * fAlpha);
			diffuse = A3DCOLORRGBA(r, g, b, A3DCOLOR_GETALPHA(diffuse));
		}

		if (m_pGfx->IsUsingOuterColor())
			diffuse = (A3DCOLORVALUE(diffuse) * m_pGfx->GetOuterColor()).ToRGBAColor();

		pVerts[nBase].x			= vPos1.x;
		pVerts[nBase].y			= vPos1.y;
		pVerts[nBase].z			= vPos1.z;
		pVerts[nBase].dwMatIndices = nMatrixIndex;
		pVerts[nBase].tu		= u1;
		pVerts[nBase].tv		= v1;
		pVerts[nBase].diffuse	= diffuse;
		pVerts[nBase].specular	= _white_spec;
		pVerts[nBase+1].x		= vPos2.x;
		pVerts[nBase+1].y		= vPos2.y;
		pVerts[nBase+1].z		= vPos2.z;
		pVerts[nBase+1].dwMatIndices = nMatrixIndex;
		pVerts[nBase+1].tu		= u2;
		pVerts[nBase+1].tv		= v2;
		pVerts[nBase+1].diffuse	= diffuse;
		pVerts[nBase+1].specular= _white_spec;

#ifdef GFX_EDITOR
		
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
		
#endif

	}

	return nRenderCount * 2;
}

bool A3DTrail::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch(nOp)
	{
	case ID_GFXOP_TRAIL_ORGPOS1:
		m_vOrgPos[0] = prop;
		break;
	case ID_GFXOP_TRAIL_ORGPOS2:
		m_vOrgPos[1] = prop;
		break;
	case ID_GFXOP_TRAIL_SEGLIFE:
		m_nSegLife = prop;
		break;
	case ID_GFXOP_TRAIL_BIND:
		m_bBind = prop;
		break;
	case ID_GFXOP_TRAIL_SPLINEMODE: 
		{
		int iNewSplineMode = prop;
		if (m_iSplineMode != iNewSplineMode)
		{
			m_iSplineMode = iNewSplineMode;
			ITrailList::DestroyTrailList(m_pTrailImpl);
			m_pTrailImpl = ITrailList::CreateTrailList(IsSplineMode());
			m_pTrailImpl->SetSampleFreq(m_iSampleFreq);
		}

		// 只要开启了平滑模式，就必须将MatrixEnable关掉
		// 不然如果美术按照刀光的模式使用平滑模式，表现上就会出错了
		if (m_iSplineMode == SPLINE_MODE)
		{
			m_bMatrixEnable = false;
		}

		break;
		}
	case ID_GFXOP_TRAIL_SAMPLEFREQ:
		m_iSampleFreq = prop;
		if (m_pTrailImpl)
			m_pTrailImpl->SetSampleFreq(m_iSampleFreq);
		break;
	default:
		return A3DGFXElement::SetProperty(nOp, prop);
	}
	return true;
}

GFX_PROPERTY A3DTrail::GetProperty(int nOp) const
{
	switch(nOp)
	{
	case ID_GFXOP_TRAIL_ORGPOS1:
		return GFX_PROPERTY(m_vOrgPos[0]);
	case ID_GFXOP_TRAIL_ORGPOS2:
		return GFX_PROPERTY(m_vOrgPos[1]);
	case ID_GFXOP_TRAIL_SEGLIFE:
		return GFX_PROPERTY(m_nSegLife);
	case ID_GFXOP_TRAIL_BIND:
		return GFX_PROPERTY(m_bBind);
	case ID_GFXOP_TRAIL_SPLINEMODE:
		return GFX_PROPERTY(m_iSplineMode).SetType(GFX_VALUE_TRAIL_TYPE);
	case ID_GFXOP_TRAIL_SAMPLEFREQ:
		return GFX_PROPERTY(m_iSampleFreq);
	}
	return A3DGFXElement::GetProperty(nOp);
}

bool A3DTrail::IsParamEnable(int nParamId) const
{ 
	switch (nParamId)
	{
	case ID_PARAM_TRAIL_MATRIX:
		return m_bMatrixEnable;
	case ID_PARAM_TRAIL_POS1:
		return m_bOrgPos1Enable;
	case ID_PARAM_TRAIL_POS2:
		return m_bOrgPos2Enable;
	}
	return false; 
}

void A3DTrail::EnableParam(int nParamId, bool bEnable) 
{
	switch (nParamId)
	{
	case ID_PARAM_TRAIL_MATRIX:
		if (IsSplineMode())
			m_bMatrixEnable = false;
		else
			m_bMatrixEnable = bEnable;
		break;
	case ID_PARAM_TRAIL_POS1:
		m_bOrgPos1Enable = bEnable;
		break;
	case ID_PARAM_TRAIL_POS2:
		m_bOrgPos2Enable = bEnable;
		break;
	}
}

bool A3DTrail::NeedUpdateParam(int nParamId) const
{
	switch (nParamId)
	{
	case ID_PARAM_TRAIL_MATRIX:
		return true;
	case ID_PARAM_TRAIL_POS1:
		return !m_bOrgPos1Set;
	case ID_PARAM_TRAIL_POS2:
		return !m_bOrgPos2Set;
	case ID_PARAM_TRAIL_ORGMAT:
		return !m_bOrgMatSet;
	}
	return true; 
}

void A3DTrail::UpdateParam(int nParamId, const GFX_PROPERTY& prop)
{
	assert(m_pTrailImpl);
	switch (nParamId)
	{
	case ID_PARAM_TRAIL_DELTATM:
		m_dwDeltaTm = prop;
		break;
	case ID_PARAM_TRAIL_MATRIX:	
		{

		if (IsSplineMode())
			break;

		KEY_POINT kp;
		GetCurKeyPoint(&kp);
		const A3DMATRIX4& mat = prop;
		if (m_pTrailImpl)
			m_pTrailImpl->AddKeyData(TRAIL_DATA(mat * m_vOrgPos[0], mat * m_vOrgPos[1], m_nSegLife - (int)m_dwDeltaTm, kp.m_color));

		break; 
		}
	case ID_PARAM_TRAIL_POS1:
		m_vOrgPos[0] = prop;
		m_bOrgPos1Set = m_bMatrixEnable;
		break;
	case ID_PARAM_TRAIL_POS2:
		m_vOrgPos[1] = prop;
		m_bOrgPos2Set = m_bMatrixEnable;
		break;
	case ID_PARAM_TRAIL_ORGMAT:	{
		A3DMATRIX4 mat = prop;
		mat.InverseTM();
		m_vOrgPos[0] = mat * m_vOrgPos[0];
		m_vOrgPos[1] = mat * m_vOrgPos[1];
		m_bOrgMatSet = true;
		break; }
	case ID_PARAM_TRAIL_TIMESPAN:
		GetKeyPoint(0)->SetTimeSpan(prop);
		break;
	case ID_PARAM_TRAIL_PARENTTM:
		m_matHost = prop;
		m_bHostMatSet = true;
		m_bAddOutData = false;
		break;
	case ID_PARAM_TRAIL_ADD_DATA: {
		m_bOrgPos1Set = false;
		m_bOrgPos2Set = false;
		m_bHostMatSet = false;
		m_bAddOutData = true;
		break; }
    case ID_PARAM_TRAIL_LIFE:
        m_dwLife = prop;
        break;
	}
}


GFX_PROPERTY A3DTrail::GetParam(int nParamId) const 
{ 
	switch (nParamId)
	{
	case ID_PARAM_TRAIL_POS1:
		return GFX_PROPERTY(m_vOrgPos[0]);
	case ID_PARAM_TRAIL_POS2:
		return GFX_PROPERTY(m_vOrgPos[1]);
	}
	return GFX_PROPERTY(); 
}

GfxValueType A3DTrail::GetParamType(int nParamId) const
{
	switch (nParamId)
	{
	case ID_PARAM_TRAIL_MATRIX:
		return GFX_VALUE_MATRIX4;
	case ID_PARAM_TRAIL_POS1:
	case ID_PARAM_TRAIL_POS2:
		return GFX_VALUE_VECTOR3;
	}
	return GFX_VALUE_UNKNOWN;
}

float A3DTrail::GetPortionFactor(int iRenderIdx)
{
    assert(m_pTrailImpl);
    TRAIL_DATA td;
    m_pTrailImpl->GetRenderTrailData(iRenderIdx, td);
    return  td.m_nSegLife / (float) m_nSegLife;
}

float A3DTrail::GetPortionAlpha(float fPortion)
{
    return fPortion;
}

A3DTrail::TRAILMODE A3DTrail::GetDefaultMode() const
{
    return LINE_MODE;
}

bool A3DTrail::IsSplineMode() const
{
    return m_iSplineMode == SPLINE_MODE;
}
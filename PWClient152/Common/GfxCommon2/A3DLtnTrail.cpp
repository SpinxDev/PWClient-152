#include "StdAfx.h"
#include "A3DLtnTrail.h"
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

static const float _tex_delta = .2f;
static const DWORD _inner_tick = 40;

static const char* _format_pos		= "Pos: %f, %f, %f";
static const char* _format_seglife	= "SegLife: %d";
static const char* _format_amp		= "Amp: %f";
static const char* _format_bind		= "Bind: %d";
static const char* _format_en_mat	= "EnableMat: %d";
static const char* _format_en_pos	= "EnablePos: %d";

A3DLtnTrail::A3DLtnTrail(A3DGFXEx* pGfx, int nBufSize) :
A3DGFXElement(pGfx),
m_TrailLst(nBufSize),
m_vPos1(0),
m_vPos2(0)
{
	m_nEleType		= ID_ELE_TYPE_LTNTRAIL;
	m_nSegLife		= 1000;
	m_fMinAmp		= .05f;
	m_fMaxAmp		= .08f;
	m_bBind			= true;
	m_bMatrixEnable	= false;
	m_bPos1Enable	= false;
	m_bPos2Enable	= false;
	m_bPos1Set		= false;
	m_bPos2Set		= false;
}

bool A3DLtnTrail::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	int		nRead;

	if (pFile->IsBinary())
	{
		pFile->Read(m_vPos1.m, sizeof(m_vPos1.m), &dwReadLen);
		pFile->Read(m_vPos2.m, sizeof(m_vPos2.m), &dwReadLen);
		pFile->Read(&m_nSegLife, sizeof(m_nSegLife), &dwReadLen);
		pFile->Read(&m_fMinAmp, sizeof(m_fMinAmp), &dwReadLen);
		pFile->Read(&m_fMaxAmp, sizeof(m_fMaxAmp), &dwReadLen);
		pFile->Read(&m_bBind, sizeof(m_bBind), &dwReadLen);
		pFile->Read(&m_bMatrixEnable, sizeof(m_bMatrixEnable), &dwReadLen);
		pFile->Read(&m_bPos1Enable, sizeof(m_bPos1Enable), &dwReadLen);
		pFile->Read(&m_bPos2Enable, sizeof(m_bPos2Enable), &dwReadLen);
	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_pos, VECTORADDR_XYZ(m_vPos1));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_pos, VECTORADDR_XYZ(m_vPos2));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_seglife, &m_nSegLife);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_amp, &m_fMinAmp);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_amp, &m_fMaxAmp);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_bind, &nRead);
		m_bBind = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_en_mat, &nRead);
		m_bMatrixEnable = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_en_pos, &nRead);
		m_bPos1Enable = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_en_pos, &nRead);
		m_bPos2Enable = (nRead != 0);
	}

	return true;
}

bool A3DLtnTrail::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(m_vPos1.m, sizeof(m_vPos1.m), &dwWrite);
		pFile->Write(m_vPos2.m, sizeof(m_vPos2.m), &dwWrite);
		pFile->Write(&m_nSegLife, sizeof(m_nSegLife), &dwWrite);
		pFile->Write(&m_fMinAmp, sizeof(m_fMinAmp), &dwWrite);
		pFile->Write(&m_fMaxAmp, sizeof(m_fMaxAmp), &dwWrite);
		pFile->Write(&m_bBind, sizeof(m_bBind), &dwWrite);
		pFile->Write(&m_bMatrixEnable, sizeof(m_bMatrixEnable), &dwWrite);
		pFile->Write(&m_bPos1Enable, sizeof(m_bPos1Enable), &dwWrite);
		pFile->Write(&m_bPos2Enable, sizeof(m_bPos2Enable), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_pos, VECTOR_XYZ(m_vPos1));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_pos, VECTOR_XYZ(m_vPos2));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_seglife, m_nSegLife);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_amp, m_fMinAmp);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_amp, m_fMaxAmp);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_bind, (int)m_bBind);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_en_mat, (int)m_bMatrixEnable);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_en_pos, (int)m_bPos1Enable);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_en_pos, (int)m_bPos2Enable);
		pFile->WriteLine(szLine);
	}
	
	return true;
}

A3DGFXElement* A3DLtnTrail::Clone(A3DGFXEx* pGfx) const
{
	A3DLtnTrail* p = new A3DLtnTrail(pGfx);
	return &(*p = *this);
}

bool A3DLtnTrail::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;
	
	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);
	
	return true;
}

inline bool A3DLtnTrail::InnerTick(DWORD dwTickTime)
{
	if (!A3DGFXElement::TickAnimation(dwTickTime)) return false;
	else if (m_bMatrixEnable) return true;
	else
	{
		KEY_POINT kp;
		GetCurKeyPoint(&kp);
		AddPoint(m_bBind ? GetTranMatrix(kp) : GetTranMatrix(kp) * GetParentTM());
		return false;
	}
}

bool A3DLtnTrail::TickAnimation(DWORD dwTickTime)
{
	int nCount = m_TrailLst.GetDataCount();
	for (int i = nCount-1; i >= 0; i--)
	{
		m_TrailLst[i].m_nSegLife -= dwTickTime;
		if (m_TrailLst[i].m_nSegLife <= 0)
		{
			m_TrailLst.RemoveOldData(i+1);
			break;
		}
	}

	while (dwTickTime > _inner_tick)
	{
		if (InnerTick(_inner_tick))
			return true;

		dwTickTime -= _inner_tick;
	}

	if (dwTickTime) InnerTick(dwTickTime);
	return true;
}

bool A3DLtnTrail::Init(A3DDevice* pDevice)
{
	A3DGFXElement::Init(pDevice);
	return true;
}

A3DLtnTrail& A3DLtnTrail::operator = (const A3DLtnTrail& src)
{	
	if (&src == this)
		return *this;
	
	_CloneBase(&src);
	Init(src.m_pDevice);
	m_vPos1			= src.m_vPos1;
	m_vPos2			= src.m_vPos2;
	m_nSegLife		= src.m_nSegLife;
	m_fMinAmp		= src.m_fMinAmp;
	m_fMaxAmp		= src.m_fMaxAmp;
	m_bBind			= src.m_bBind;
	m_bMatrixEnable	= src.m_bMatrixEnable;
	m_bPos1Enable	= src.m_bPos1Enable;
	m_bPos2Enable	= src.m_bPos2Enable;
	return *this;
}

bool A3DLtnTrail::Play()
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

	return true;
}

bool A3DLtnTrail::Stop()
{
	A3DGFXElement::Stop();
	m_TrailLst.RemoveAll();
	m_bPos1Set = false;
	m_bPos2Set = false;
	return true;
}

int A3DLtnTrail::GetVertsCount()
{
	return m_TrailLst.GetBufSize() * 2;
}

void A3DLtnTrail::AddPoint(const A3DMATRIX4& matTran)
{
	int nCount = m_TrailLst.GetDataCount();

	if (nCount == 0)
	{
		LTNTRAIL_DATA& ldStart = m_TrailLst.AddData(LTNTRAIL_DATA(
			matTran * m_vPos1,
			matTran * m_vPos2,
			A3DQUATERNION(matTran),
			matTran.GetRow(3),
			m_nSegLife,
			1.0f
			));

		ldStart.m_fU = 0;
	}
	else
	{
		if (nCount > 1)
		{
			m_TrailLst.RemoveHead();
			nCount--;
		}

		LTNTRAIL_DATA& ldStart = m_TrailLst[nCount-1];
		A3DVECTOR3 vCenter = matTran.GetRow(3);
		A3DQUATERNION qDir(matTran);
		float fPortion = _UnitRandom();
		float fR = 1.0f - fPortion;

		A3DVECTOR3 vDir = Normalize(vCenter - ldStart.m_vCenter);
		A3DVECTOR3 vInter = ldStart.m_vCenter * fR + vCenter * fPortion;
		A3DQUATERNION qInter = SLERPQuad(ldStart.m_qDir, qDir, fPortion);

		A3DVECTOR3 vAmp = RotateVec(
			A3DQUATERNION(vDir, _UnitRandom() * A3D_2PI),
			CalcVertVec(vDir));
		vAmp *= m_fMinAmp + (m_fMaxAmp - m_fMinAmp) * _UnitRandom();
		vInter += vAmp;

		A3DMATRIX4 matInter;
		qInter.ConvertToMatrix(matInter);
		matInter.SetRow(3, vInter);

		LTNTRAIL_DATA& ldCenter = m_TrailLst.AddData(LTNTRAIL_DATA(
			matInter * m_vPos1,
			matInter * m_vPos2,
			qInter,
			vInter,
			m_nSegLife,
			1.0f
			));

		ldCenter.m_fU = ldStart.m_fU + _tex_delta;
		if (ldCenter.m_fU > 1.0f) ldCenter.m_fU -= 1.0f;

		LTNTRAIL_DATA& ldEnd = m_TrailLst.AddData(LTNTRAIL_DATA(
			matTran * m_vPos1,
			matTran * m_vPos2,
			qDir,
			vCenter,
			m_nSegLife,
			1.0f
			));

		ldEnd.m_fU = ldCenter.m_fU + _tex_delta;
		if (ldEnd.m_fU > 1.0f) ldEnd.m_fU -= 1.0f;
	}
}

int A3DLtnTrail::FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView)
{
	KEY_POINT kp;
	A3DGFXVERTEX* pVerts = (A3DGFXVERTEX*)pBuffer;

	if (!GetCurKeyPoint(&kp))
		return 0;

	int nCount = m_TrailLst.GetDataCount(), i;
	if (nCount < 2) return 0;

	if (m_pGfx->IsUsingOuterColor())
		kp.m_color = (A3DCOLORVALUE(kp.m_color) * m_pGfx->GetOuterColor()).ToRGBAColor();

	A3DMATRIX4 matWorld = m_bBind ? GetParentTM() : _identity_mat4;

	if (A3DGFXRenderSlotMan::g_RenderMode == GRMSoftware)
		m_pDevice->SetWorldMatrix(matWorld);
	else if (A3DGFXRenderSlotMan::g_RenderMode == GRMVertexShader)
	{
		matWorld.Transpose();
		m_pDevice->SetVertexShaderConstants(nMatrixIndex * 3 + GFX_VS_CONST_BASE, &matWorld, 3);
	}
	else
		m_pDevice->SetIndexedVertexBlendMatrix(nMatrixIndex, matWorld);

	static const A3DCOLOR spec = A3DCOLORRGBA(0, 0, 0, 255);

#ifdef GFX_EDITOR
	m_AABB.Clear(); 
#endif

	for (i = 0; i < nCount; i++)
	{
		int nBase = i * 2;
		LTNTRAIL_DATA& ld = m_TrailLst[i];
		float fAlpha = (float)ld.m_nSegLife / m_nSegLife;
		A3DCOLOR diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha() * fAlpha);

		SET_VERT(pVerts[nBase],   ld.m_vPos1);
		SET_VERT(pVerts[nBase+1], ld.m_vPos2);

		pVerts[nBase].dwMatIndices = pVerts[nBase+1].dwMatIndices = nMatrixIndex;
		pVerts[nBase].tu		= pVerts[nBase+1].tu		= ld.m_fU;
		pVerts[nBase].specular	= pVerts[nBase+1].specular	= spec;
		pVerts[nBase].diffuse	= pVerts[nBase+1].diffuse	= diffuse;

		pVerts[nBase].tv = 0;
		pVerts[nBase+1].tv = 1.0f;

#ifdef GFX_EDITOR
//		if (m_pGfx->IsCalcingAABB())
		{
			for (int i = 0; i < 2; i++)
			{
				A3DGFXVERTEX* pVert = &pVerts[nBase + i];
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

	return nCount * 2;
}

bool A3DLtnTrail::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch(nOp)
	{
	case ID_GFXOP_LTNTRAIL_POS1:
		m_vPos1 = prop;
		break;
	case ID_GFXOP_LTNTRAIL_POS2:
		m_vPos2 = prop;
		break;
	case ID_GFXOP_LTNTRAIL_SEGLIFE:
		m_nSegLife = prop;
		break;
	case ID_GFXOP_LTNTRAIL_MIN_AMP:
		m_fMinAmp = prop;
		break;
	case ID_GFXOP_LTNTRAIL_MAX_AMP:
		m_fMaxAmp = prop;
		break;
	case ID_GFXOP_LTNTRAIL_BIND:
		m_bBind = prop;
		break;
	default:
		return A3DGFXElement::SetProperty(nOp, prop);
	}

	return true;
}

GFX_PROPERTY A3DLtnTrail::GetProperty(int nOp) const
{
	switch(nOp)
	{
	case ID_GFXOP_LTNTRAIL_POS1:
		return GFX_PROPERTY(m_vPos1);
	case ID_GFXOP_LTNTRAIL_POS2:
		return GFX_PROPERTY(m_vPos2);
	case ID_GFXOP_LTNTRAIL_SEGLIFE:
		return GFX_PROPERTY(m_nSegLife);
	case ID_GFXOP_LTNTRAIL_MIN_AMP:
		return GFX_PROPERTY(m_fMinAmp);
	case ID_GFXOP_LTNTRAIL_MAX_AMP:
		return GFX_PROPERTY(m_fMaxAmp);
	case ID_GFXOP_LTNTRAIL_BIND:
		return GFX_PROPERTY(m_bBind);
	}
	return A3DGFXElement::GetProperty(nOp);
}

bool A3DLtnTrail::IsParamEnable(int nParamId) const
{ 
	switch (nParamId)
	{
	case ID_PARAM_LTNTRAIL_MATRIX:
		return m_bMatrixEnable;
	case ID_PARAM_LTNTRAIL_POS1:
		return m_bPos1Enable;
	case ID_PARAM_LTNTRAIL_POS2:
		return m_bPos2Enable;
	}
	return false; 
}

void A3DLtnTrail::EnableParam(int nParamId, bool bEnable) 
{
	switch (nParamId)
	{
	case ID_PARAM_LTNTRAIL_MATRIX:
		m_bMatrixEnable = bEnable;
		break;
	case ID_PARAM_LTNTRAIL_POS1:
		m_bPos1Enable = bEnable;
		break;
	case ID_PARAM_LTNTRAIL_POS2:
		m_bPos2Enable = bEnable;
		break;
	}
}

bool A3DLtnTrail::NeedUpdateParam(int nParamId) const
{
	switch (nParamId)
	{
	case ID_PARAM_LTNTRAIL_MATRIX:
		return true;
	case ID_PARAM_LTNTRAIL_POS1:
		return !m_bPos1Set;
	case ID_PARAM_LTNTRAIL_POS2:
		return !m_bPos2Set;
	}
	return true; 
}

void A3DLtnTrail::UpdateParam(int nParamId, const GFX_PROPERTY& prop)
{ 
	switch (nParamId)
	{
	case ID_PARAM_LTNTRAIL_MATRIX:
		AddPoint(prop);
		break;
	case ID_PARAM_LTNTRAIL_POS1:
		m_vPos1 = prop;
		m_bPos1Set = true;
		break;
	case ID_PARAM_LTNTRAIL_POS2:
		m_vPos2 = prop;
		m_bPos2Set = true;
		break;
	}
}

GFX_PROPERTY A3DLtnTrail::GetParam(int nParamId) const 
{ 
	switch (nParamId)
	{
	case ID_PARAM_LTNTRAIL_POS1:
		return GFX_PROPERTY(m_vPos1);
	case ID_PARAM_LTNTRAIL_POS2:
		return GFX_PROPERTY(m_vPos2);
	}
	return GFX_PROPERTY(); 
}

GfxValueType A3DLtnTrail::GetParamType(int nParamId) const
{
	switch (nParamId)
	{
	case ID_PARAM_LTNTRAIL_MATRIX:
		return GFX_VALUE_MATRIX4;
	case ID_PARAM_LTNTRAIL_POS1:
	case ID_PARAM_LTNTRAIL_POS2:
		return GFX_VALUE_VECTOR3;
	}
	return GFX_VALUE_UNKNOWN;
}

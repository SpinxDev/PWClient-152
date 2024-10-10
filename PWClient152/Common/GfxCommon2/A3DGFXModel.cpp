#include "StdAfx.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DCDS.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "A3DConfig.h"
#include "A3DGFXExMan.h"
#include "A3DLight.h"
#include "A3DGFXModel.h"
#include "A3DSkinModelAct.h"

static const char* _format_model_path	= "ModelPath: %s";
static const char* _format_act_name		= "ModelActName: %s";
static const char* _format_loops		= "Loops: %d";
static const char* _format_alpha_cmp	= "AlphaCmp: %d";
static const char* _format_write_z		= "WriteZ: %d";
static const char* _format_use_3dcamera	= "Use3DCam: %d";
static const char* _format_is_facing_dir = "FacingDir: %d";
static const char* _format_delay_for_fb = "DelayRender: %d";
static const A3DCOLOR _white_cl			= A3DCOLORRGBA(255, 255, 255, 0);
static const A3DCOLOR _white_alpha		= A3DCOLORRGBA(0, 0, 0, 255);
static const A3DCOLORVALUE _org_scale(1.f, 1.f, 1.f, 1.f);

bool A3DGFXModel::m_bOldZwriteEnable = true;
bool A3DGFXModel::m_bOldZTest = true;


A3DGFXModel::A3DGFXModel(A3DGFXEx* pGfx) :
A3DGFXElement(pGfx),
m_pModelRef(0),
m_nLoops(1),
m_bAlphaCmp(false),
m_bZWriteEnable(false),
m_bStart(false),
m_bIsUse3DCamera(false),
m_bIsFacingDir(false),
m_iMatMethod(A3DSkinModel::MTL_ORIGINAL),
m_MatScale(_org_scale),
m_fTransparent(-1.0f),
m_dwTickTime(0)
{
	m_nEleType = ID_ELE_TYPE_MODEL;
	m_matTran.Identity();
}

bool A3DGFXModel::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	Init(pDevice);

	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->ReadString(m_strModelFile);
		pFile->ReadString(m_strActName);
		pFile->Read(&m_nLoops, sizeof(m_nLoops), &dwRead);
		pFile->Read(&m_bAlphaCmp, sizeof(m_bAlphaCmp), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		char	szBuf[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		szBuf[0] = '\0';
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_model_path, szBuf);
		m_strModelFile = szBuf;

		if (dwVersion >= 19)
		{
			szBuf[0] = '\0';
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_act_name, szBuf);
			m_strActName = szBuf;
		}

		if (dwVersion >= 31)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_loops, &m_nLoops);
		}

		if (dwVersion >= 29)
		{
			int nRead;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_alpha_cmp, &nRead);
			m_bAlphaCmp = (nRead != 0);
		}

		if (dwVersion >= 49)
		{
			int nRead;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_write_z, &nRead);
			m_bZWriteEnable = (nRead != 0);
		}

		if (dwVersion >= 77)
		{
			int nRead;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_use_3dcamera, &nRead);
			m_bIsUse3DCamera = (nRead != 0);
		}

		if (dwVersion >= 99)
		{
			int nRead = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_is_facing_dir, &nRead);
			m_bIsFacingDir = (nRead != 0);
		}
	}

	return true;
}

bool A3DGFXModel::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->WriteString(m_strModelFile);
		pFile->WriteString(m_strActName);
		pFile->Write(&m_nLoops, sizeof(m_nLoops), &dwWrite);
		pFile->Write(&m_bAlphaCmp, sizeof(m_bAlphaCmp), &dwWrite);
		pFile->Write(&m_bZWriteEnable, sizeof(m_bZWriteEnable), &dwWrite);
		pFile->Write(&m_bIsUse3DCamera, sizeof(m_bIsUse3DCamera), &dwWrite);
		pFile->Write(&m_bIsFacingDir, sizeof(m_bIsFacingDir), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_model_path, m_strModelFile);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_act_name, m_strActName);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_loops, m_nLoops);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_alpha_cmp, m_bAlphaCmp);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_write_z, m_bZWriteEnable);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_use_3dcamera, m_bIsUse3DCamera);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_is_facing_dir, m_bIsFacingDir);
		pFile->WriteLine(szLine);
	}

	return true;
}

A3DGFXModel& A3DGFXModel::operator = (const A3DGFXModel& src)
{
	if (&src == this)
		return *this;

	_CloneBase(&src);
	Init(src.m_pDevice);
	m_strModelFile	= src.m_strModelFile;
	m_strActName	= src.m_strActName;
	m_nLoops		= src.m_nLoops;
	m_bAlphaCmp		= src.m_bAlphaCmp;
	m_bZWriteEnable	= src.m_bZWriteEnable;
	m_bIsUse3DCamera = src.m_bIsUse3DCamera;
	m_bIsFacingDir = src.m_bIsFacingDir;
	return *this;
}

A3DGFXElement* A3DGFXModel::Clone(A3DGFXEx* pGfx) const
{
	A3DGFXModel* p = new A3DGFXModel(pGfx);
	return &(*p = *this);
}

void A3DGFXModel::Release()
{
	A3DGFXElement::Release();
	ReleaseModel();
}

bool A3DGFXModel::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;

	if (IsEleShouldBeDelayRendered())
	{
		AfxGetGFXExMan()->AddDelayedEle(this);
		return false;
	}

	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}

void A3DGFXModel::DummyTick(DWORD dwTick)
{
	assert(m_pDummyMat);

	if (GetKeyPointCount() > 0)
		UpdateModel(GetKeyPointMat(0) * (*m_pDummyMat), m_clDummy, dwTick);
	else
		UpdateModel(*m_pDummyMat, m_clDummy, dwTick);
}

bool A3DGFXModel::TickAnimation(DWORD dwTickTime)
{
	if (!A3DGFXElement::TickAnimation(dwTickTime)) return false;

	if (GetPriority() > AfxGetGFXExMan()->GetPriority())
		return true;

	KEY_POINT kp;
	GetCurKeyPoint(&kp);
	A3DCOLOR diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha());
	A3DMATRIX4 matScale;
	matScale.Scale(kp.m_fScale, kp.m_fScale, kp.m_fScale);

	UpdateModel(matScale * GetTranMatrix(kp) * GetParentTM(), diffuse, dwTickTime);
	return true;
}

void A3DGFXModel::UpdateModel(const A3DMATRIX4& matTran, A3DCOLOR diffuse, DWORD dwTickTime)
{
	if (m_pModelRef)
	{
		A3DSkinModel* pSkinModel = m_pModelRef->GetSkinModel();

		if (!m_bStart)
		{
			m_bStart = true;
			m_iMatMethod = A3DSkinModel::MTL_ORIGINAL;
			m_MatScale = _org_scale;
			m_fTransparent = -1.0f;

			if (!m_strActName.IsEmpty())
				pSkinModel->PlayActionByNameDC(m_strActName, m_nLoops, 0);
		}


		m_matTran = matTran;
		m_dwTickTime = dwTickTime;

		if (m_pGfx->IsUsingOuterColor())
			diffuse = (A3DCOLORVALUE(diffuse) * m_pGfx->GetOuterColor()).ToRGBAColor();

		if ((diffuse & _white_cl) != _white_cl)
		{
			m_iMatMethod = A3DSkinModel::MTL_SCALED;
			m_MatScale.r = A3DCOLOR_GETRED	(diffuse) / 255.f;
			m_MatScale.g = A3DCOLOR_GETGREEN(diffuse) / 255.f;
			m_MatScale.b = A3DCOLOR_GETBLUE	(diffuse) / 255.f;
		}
		else
			m_iMatMethod = A3DSkinModel::MTL_ORIGINAL;

		if ((diffuse & _white_alpha) != _white_alpha)
		{
			float fAlpha = A3DCOLOR_GETALPHA(diffuse) / 255.f;
			m_fTransparent = 1.0f - fAlpha;
		}
		else
			m_fTransparent = -1.0f;

		const A3DLIGHTPARAM& LightParams = AfxGetLightparam();
		A3DSkinModel::LIGHTINFO LightInfo;
		memset(&LightInfo, 0, sizeof (LightInfo));

		LightInfo.colAmbient	= m_pDevice->GetAmbientValue();	//	Ambient color
		LightInfo.vLightDir		= LightParams.Direction;		//	Direction of directional light
		LightInfo.colDirDiff	= LightParams.Diffuse;			//	Directional light's diffuse color
		LightInfo.colDirSpec	= LightParams.Specular;			//	Directional light's specular color
		LightInfo.bPtLight		= false;						//	false, disable dynamic point light,
		pSkinModel->SetLightInfo(LightInfo);

		if (!m_pModelRef->IsShared())
		{
			pSkinModel->SetMaterialMethod(m_iMatMethod);
			pSkinModel->SetMaterialScale(m_MatScale);
			pSkinModel->SetTransparent(m_fTransparent);
			pSkinModel->SetAbsoluteTM(m_matTran);
			pSkinModel->Update(m_dwTickTime);
		}

#ifdef GFX_EDITOR

		{
			// Do not need to Add Trans Offset
			// For the Model's AABB has already represents its position
			A3DAABB r(pSkinModel->GetModelAABB().Mins, pSkinModel->GetModelAABB().Maxs);

			A3DVECTOR3 vCenter = r.Center;
			vCenter = vCenter * matTran;
			A3DVECTOR3 mins = vCenter - r.Extents;
			A3DVECTOR3 maxs = vCenter + r.Extents;

			m_AABB.Clear();
			A3DAABB& aabb = m_AABB;
			if (mins.x < aabb.Mins.x) aabb.Mins.x = mins.x;
			if (mins.y < aabb.Mins.y) aabb.Mins.y = mins.y;
			if (mins.z < aabb.Mins.z) aabb.Mins.z = mins.z;
			if (maxs.x > aabb.Maxs.x) aabb.Maxs.x = maxs.x;
			if (maxs.y > aabb.Maxs.y) aabb.Maxs.y = maxs.y;
			if (maxs.z > aabb.Maxs.z) aabb.Maxs.z = maxs.z;
		}

#endif
	}
}

bool A3DGFXModel::Play()
{
	if (!IsInit())
	{
		InitBaseData();

		if (!ChangeModel())
			return true;

		A3DSHADER sh;
		sh.SrcBlend = A3DBLEND_SRCALPHA;
		sh.DestBlend = A3DBLEND_INVSRCALPHA;
		m_dwRenderSlot = AfxGetGFXExMan()->GetRenderMan().RegisterSlot(
            m_nRenderLayer, 0, 0, GFX_MODEL_PRIM_TYPE, AString(), sh, 
            false, false, m_pGfx->IsZTestEnable() && m_bZEnable, HasPixelShader(), m_bSoftEdge, m_bAbsTexPath);
		SetInit(true);
	}

	m_bStart = false;
	return true;
}

void A3DGFXModel::PrepareRenderSkinModel()
{
	if (m_pModelRef && m_pModelRef->IsShared())
	{
		A3DSkinModel* pSkinModel = m_pModelRef->GetSkinModel();
		pSkinModel->SetMaterialMethod(m_iMatMethod);
		pSkinModel->SetMaterialScale(m_MatScale);
		pSkinModel->SetTransparent(m_fTransparent);
		pSkinModel->SetSrcBlend(m_Shader.SrcBlend);
		pSkinModel->SetDstBlend(m_Shader.DestBlend);
		pSkinModel->EnableAlphaComp(m_bAlphaCmp);
		pSkinModel->SetAbsoluteTM(m_matTran);
		pSkinModel->Update(m_dwTickTime);

		if (m_bIsFacingDir)
		{
			if (!m_bStart)
			{
				m_vOldPos = m_matTran.GetRow(3);
				m_vOldDir = pSkinModel->GetDir();
			}
			else
			{
				A3DVECTOR3 vDir = m_matTran.GetRow(3) - m_vOldPos;
	
				if (vDir.Normalize() != 0.0f)
				{
					vDir = m_vOldDir*0.2f + vDir*0.8f;
					pSkinModel->SetDirAndUp(vDir, m_matTran.GetRow(1));
					m_vOldDir = vDir;
					m_vOldPos = m_matTran.GetRow(3);
				}
				else
				{
					pSkinModel->SetDirAndUp(m_vOldDir, m_matTran.GetRow(1));
				}
				
			}
		}
	}

	m_bOldZwriteEnable = m_pDevice->GetZWriteEnable();
	if(m_bOldZwriteEnable != m_bZWriteEnable)
		m_pDevice->SetZWriteEnable(m_bZWriteEnable);

	m_bOldZTest = m_pDevice->GetZTestEnable();
	bool zTest = m_pGfx->IsZTestEnable() && m_bZEnable;
	if (m_bOldZTest != zTest)
	{
		m_pDevice->SetZTestEnable(m_bZEnable);

#ifdef _ANGELICA21
		if (m_pGfx->Is2DRender() && m_bZEnable)
			m_pDevice->Clear(D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
#endif
	}
}

void A3DGFXModel::RestoreRenderSkinModel()
{
	if(m_bOldZwriteEnable != m_pDevice->GetZWriteEnable())
		m_pDevice->SetZWriteEnable(m_bOldZwriteEnable);
	if(m_bOldZTest != m_pDevice->GetZTestEnable())
		m_pDevice->SetZTestEnable(m_bOldZTest);
}

bool A3DGFXModel::ChangeModel()
{
	if (m_strModelFile.IsEmpty())
		return false;

	A3DGFXModelMan& ModelMan = AfxGetGFXExMan()->GetModelMan();
	A3DGFXModelRef* pModelRef = ModelMan.LoadModel("gfx\\Models\\" + m_strModelFile, m_strActName.IsEmpty());

	if (!pModelRef)
		return false;

	if (m_pModelRef)
		ModelMan.ReleaseModel(m_pModelRef);

	m_pModelRef = pModelRef;
	A3DSkinModel* pSkinModel = pModelRef->GetSkinModel();

#ifdef _ANGELICA21

	if (!m_strActName.IsEmpty())
	{
		A3DSkinModelActionCore* pActionCore = pSkinModel->GetAction(m_strActName);

		if (pActionCore)
			pActionCore->LoadActionTrackData();
	}

#endif

	ModelMan.Lock();

	const A3DLIGHTPARAM& LightParams = AfxGetLightparam();
	A3DSkinModel::LIGHTINFO LightInfo;
	memset(&LightInfo, 0, sizeof (LightInfo));

	LightInfo.colAmbient	= m_pDevice->GetAmbientValue();	//	Ambient color
	LightInfo.vLightDir		= LightParams.Direction;		//	Direction of directional light
	LightInfo.colDirDiff	= LightParams.Diffuse;			//	Directional light's diffuse color
	LightInfo.colDirSpec	= LightParams.Specular;			//	Directional light's specular color
	LightInfo.bPtLight		= false;						//	false, disable dynamic point light,

	pSkinModel->SetLightInfo(LightInfo);
	pSkinModel->SetSrcBlend(m_Shader.SrcBlend);
	pSkinModel->SetDstBlend(m_Shader.DestBlend);
	pSkinModel->EnableAlphaComp(m_bAlphaCmp);
	pSkinModel->EnableSpecular(true);
	pSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);

	//	Set model's position
	pSkinModel->SetPos(_unit_zero);
	//	Set model's direction
	pSkinModel->SetDirAndUp(_unit_z, _unit_y);

	ModelMan.Unlock();
	return true;
}

void A3DGFXModel::ReleaseModel()
{
	if (m_pModelRef)
	{
		AfxGetGFXExMan()->GetModelMan().ReleaseModel(m_pModelRef);
		m_pModelRef = NULL;
	}
}

bool A3DGFXModel::IsForceUse3DCamera() const
{
	return false/*m_bIsUse3DCamera*/;
}

bool A3DGFXModel::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch(nOp)
	{
	case ID_GFXOP_MODEL_PATH:
		m_strModelFile = prop;
		return true;
	case ID_GFXOP_MODEL_ACT_NAME:
		m_strActName = prop;
		return true;
	case ID_GFXOP_MODEL_LOOPS:
		m_nLoops = prop;
		return true;
	case ID_GFXOP_MODEL_ALPHA_CMP:
		m_bAlphaCmp = prop;
		return true;
	case ID_GFXOP_MODEL_WRITE_Z:
		m_bZWriteEnable = prop;
		return true;
	case ID_GFXOP_MODEL_USE_3DCAMERA:
		//m_bIsUse3DCamera = prop;
		return true;
	case ID_GFXOP_MODEL_FACE_DIR:
		m_bIsFacingDir = prop;
		return true;
	}
	return A3DGFXElement::SetProperty(nOp, prop);
}

GFX_PROPERTY A3DGFXModel::GetProperty(int nOp) const
{
	switch(nOp)
	{
	case ID_GFXOP_MODEL_PATH:
		return GFX_PROPERTY(m_strModelFile, GFX_VALUE_PATH_MOD);
	case ID_GFXOP_MODEL_ACT_NAME:
		return GFX_PROPERTY(m_strActName);
	case ID_GFXOP_MODEL_LOOPS:
		return GFX_PROPERTY(m_nLoops);
	case ID_GFXOP_MODEL_ALPHA_CMP:
		return GFX_PROPERTY(m_bAlphaCmp);
	case ID_GFXOP_MODEL_WRITE_Z:
		return GFX_PROPERTY(m_bZWriteEnable);
	case ID_GFXOP_MODEL_USE_3DCAMERA:
		return GFX_PROPERTY(/*m_bIsUse3DCamera*/false);
	case ID_GFXOP_MODEL_FACE_DIR:
		return GFX_PROPERTY(m_bIsFacingDir);
	}
	return A3DGFXElement::GetProperty(nOp);
}

A3DSkinModel* A3DGFXModel::GetSkinModel() 
{ 
	return m_pModelRef ? m_pModelRef->GetSkinModel() : NULL; 
}
/*
* FILE: A3DGFXECModel.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/01/13
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "A3DGFXECModel.h"
#include "A3DGFXECMLoader.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

static const char* _format_ecmodel_path		= "ECMPath: %s";
static const char* _format_act_name			= "ECMActName: %s";
static const char* _format_use_caster_ski	= "UseCasterSki: %d";
static const char* _format_ecm_loader_type	= "LDType: %d";
static const char* _format_ecm_user_cmd		= "UsrCmd: %s";

volatile int g_nGfxCusECModelCount = 0;
int g_nMaxGfxCusECModelCount = 1000000;

void SetMaxGfxCusECModelCount(int nMaxCount)
{
	g_nMaxGfxCusECModelCount = nMaxCount;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DGFXECModel
//	
///////////////////////////////////////////////////////////////////////////

A3DGFXECModel::A3DGFXECModel(A3DGFXEx* pGfx)
: A3DGFXElement(pGfx)
, m_pECModel(NULL)
, m_bUseCasterSkin(false)
{
	m_nEleType = ID_ELE_TYPE_ECMODEL;
	m_pLoader = new NormalLoader(this);
}

A3DGFXECModel::~A3DGFXECModel(void)
{
	ECMLoader::Destroy(m_pLoader);
}

void A3DGFXECModel::Release()
{
	A3DGFXElement::Release();
	
	if (m_pECModel)
	{
		if (m_pLoader->GetLoaderType() == ECMLoader::TYPE_CUSTOM_ECM && m_pECModel->GetA3DSkinModel())
			g_nGfxCusECModelCount--;

		m_pECModel->StopAllActions();
		A3DRELEASE(m_pECModel);
	}
}

bool A3DGFXECModel::Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion)
{
	if (!Init(pDevice))
		return false;

	if (pFileToLoad->IsBinary())
	{
		ASSERT(pFileToLoad->IsText());
		a_LogOutput(1, "Error in A3DGFXECModel::Load, Not able to load file in binary way.");
		return false;
	}

	char szLine[AFILE_LINEMAXLEN];
	DWORD dwRead = 0;

	char szContent[AFILE_LINEMAXLEN] = {0};
	pFileToLoad->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_ecmodel_path, szContent);
	m_strECMFilename = szContent;

	szContent[0] = 0;
	pFileToLoad->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_act_name, szContent);
	m_strActionName = szContent;

	if (dwVersion <= 84)
	{
		int iReadInt = 0;
		pFileToLoad->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_use_caster_ski, &iReadInt);
		m_bUseCasterSkin = (iReadInt != 0);
		
		if (m_bUseCasterSkin)
		{
			ECMLoader::Destroy(m_pLoader);
			m_pLoader = ECMLoader::Create(ECMLoader::TYPE_CUSTOM_SKIN, this);
		}
	}
	else
	{
		int iReadInt = 0;
		pFileToLoad->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_ecm_loader_type, &iReadInt);

		ECMLoader::Destroy(m_pLoader);
		m_pLoader = ECMLoader::Create(iReadInt, this);
	}

	if (dwVersion >= 85)
	{
		char szContent[1024] = {0};
		pFileToLoad->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_ecm_user_cmd, szContent);
		m_strUserCmd = szContent;
	}

	return true;
}

bool A3DGFXECModel::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		ASSERT(pFile->IsText());
		a_LogOutput(1, "Error in A3DGFXECModel::Save, Not able to save file in binary way.");
		return false;
	}

	char szLine[AFILE_LINEMAXLEN];

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_ecmodel_path, m_strECMFilename);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_act_name, m_strActionName);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_ecm_loader_type, m_pLoader->GetLoaderType());
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_ecm_user_cmd, m_strUserCmd);
	pFile->WriteLine(szLine);

	return true;
}

bool A3DGFXECModel::Play()
{
	if (!IsInit())
	{
		InitBaseData();

		if (m_pGfx->IsCreatedByGFXECM() || !InitECModelFile(m_strECMFilename))
			return true;

		A3DSHADER sh;
		sh.SrcBlend = A3DBLEND_SRCALPHA;
		sh.DestBlend = A3DBLEND_INVSRCALPHA;
		m_dwRenderSlot = AfxGetGFXExMan()->GetRenderMan().RegisterSlot
			(m_nRenderLayer
			, 0
			, 0
			, GFX_MODEL_PRIM_TYPE
			, AString()
			, sh
			, false
			, false
			, m_pGfx->IsZTestEnable() && m_bZEnable
			, HasPixelShader()
            , m_bSoftEdge
			, m_bAbsTexPath);

		SetInit(true);
	}

	//////////////////////////////////////////////////////////////////////////
	// 这里的意图是，第一次Play时，加载模型，并设置一些初始化属性(在Loader的OnInit中)
	// 然后反复的调用Play和Stop都不会导致模型的重新加载
	// 但是可能存在问题：
	// 如果该Gfx被CacheRelease了，然后又被Load回来，如果过程中Gfx并没有被真正释放，这里的IsInit判断还是为真
	// 于是，InitModelFile函数不会被调用，在某些场合存在出现问题的可能：
	// 如果被Load回来的Gfx，被挂载于某个ECModel，该Model有一些需要传递的属性(GFXUseLod, GfxShakeCam)设置给此Gfx
	// 但是，此时就无法将Gfx上改变了的继承属性提交给下层的ECModel了，因此需要做以修改：
	// 即每次OnPlay的时候都把继承的属性传递一次
	//////////////////////////////////////////////////////////////////////////

	if (m_pECModel && m_pGfx->IsResourceReady())
	{
		if (!m_pLoader->OnPlay(m_pECModel))
		{
			if (m_pLoader->GetLoaderType() == ECMLoader::TYPE_CUSTOM_ECM && m_pECModel->GetA3DSkinModel())
				g_nGfxCusECModelCount--;

			A3DRELEASE(m_pECModel);
			return false;
		}

		if (m_pECModel->IsLoaded())
		{
			m_pECModel->ShowCoGfx(true);

			if (!m_strActionName.IsEmpty())
			{
				m_pECModel->PlayActionByName(m_strActionName, 1.0f, true, 0);
			}
		}
	}

	return true;
}

bool A3DGFXECModel::Stop()
{
	if (m_pECModel)
	{
		m_pECModel->StopAllActions(true);
		m_pECModel->ShowCoGfx(false);
		m_pLoader->Clean(m_pECModel);
	}

	return A3DGFXElement::Stop();
}

void A3DGFXECModel::PrepareRenderSkinModel()
{
	if (m_pECModel)
	{
		m_pECModel->SetSrcBlend(m_Shader.SrcBlend);
		m_pECModel->SetDstBlend(m_Shader.DestBlend);
	}
	
	m_pDevice->SetZWriteEnable(true);
}

A3DSkinModel* A3DGFXECModel::GetSkinModel() 
{ 
	return m_pECModel ? m_pECModel->GetA3DSkinModel() : NULL;
}

bool A3DGFXECModel::Render(A3DViewport* pA3DViewport)
{
	if (m_pGfx->IsCreatedByGFXECM())
		return true;

	if (!CanRender())
		return true;

	if (m_pECModel)
	{
		m_pECModel->Render(pA3DViewport, false, false);
	}

	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}

void A3DGFXECModel::UpdateECModel(A3DMATRIX4 matTran, DWORD dwDiffuse, DWORD dwTickTime)
{
	if (!m_pECModel || !m_pECModel->IsLoaded() || !m_pECModel->GetA3DSkinModel())
		return;

	m_pECModel->SetAbsoluteTM(matTran);
	m_pECModel->SetColor(dwDiffuse);
	m_pECModel->SetTransparent(1 - A3DCOLOR_GETALPHA(dwDiffuse) / 255.f);
	m_pECModel->Tick(dwTickTime);


#ifdef GFX_EDITOR
	
	// Do not need to Add Trans Offset
	// For the Model's AABB has already represents its position
	A3DAABB r(m_pECModel->GetModelAABB().Mins, m_pECModel->GetModelAABB().Maxs);
	if (m_pECModel->HasCHAABB())
		r.Merge(m_pECModel->GetCHAABB());
	
	A3DVECTOR3 vCenter = r.Center;
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
	
#endif
}

void A3DGFXECModel::DummyTick(DWORD dwTick)
{
	assert(m_pDummyMat);

	if (GetKeyPointCount() > 0)
		UpdateECModel(GetKeyPointMat(0) * (*m_pDummyMat), m_clDummy, dwTick);
	else
		UpdateECModel(*m_pDummyMat, m_clDummy, dwTick);
}

bool A3DGFXECModel::TickAnimation(DWORD dwTickTime)
{
	if (!A3DGFXElement::TickAnimation(dwTickTime))
		return false;

	if (GetPriority() > AfxGetGFXExMan()->GetPriority())
		return true;

	if (m_pECModel)
	{
		if (m_pLoader)
			m_pLoader->Update(m_pECModel, m_strActionName);

		KEY_POINT kp;
		GetCurKeyPoint(&kp);
		A3DCOLOR diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha());
		A3DMATRIX4 matScale;
		matScale.Scale(kp.m_fScale, kp.m_fScale, kp.m_fScale);
		UpdateECModel(matScale * GetTranMatrix(kp) * GetParentTM(), diffuse, dwTickTime);
	}

	return true;
}

bool A3DGFXECModel::InitECModelFile(const char* szFilename)
{
#ifdef _ANGELICA21

	if (g_nGfxCusECModelCount >= g_nMaxGfxCusECModelCount && m_pLoader->GetLoaderType() == ECMLoader::TYPE_CUSTOM_ECM)
		return false;

#endif

	std::auto_ptr<CECModel> pNewECModel(new CECModel);

	if (!m_pLoader->OnInit(pNewECModel.get()))
		return false;

	if (m_pECModel)
	{
		if (m_pLoader->GetLoaderType() == ECMLoader::TYPE_CUSTOM_ECM && m_pECModel->GetA3DSkinModel())
			g_nGfxCusECModelCount--;

		A3DRELEASE(m_pECModel);
	}

	m_pECModel = pNewECModel.release();

#ifdef _ANGELICA21

	if (m_pECModel && m_pECModel->IsLoaded() && !m_strActionName.IsEmpty())
	{
		A3DCombinedAction* pComAct = m_pECModel->GetComActByName(m_strActionName);

		if (pComAct)
		{
			A3DSkinModel* pSkinModel = m_pECModel->GetA3DSkinModel();

			if (pSkinModel)
			{
				int nBaseActCount = pComAct->GetBaseActCount();

				for (int i = 0; i < nBaseActCount; i++)
				{
					PACTION_INFO pInfo = pComAct->GetBaseAct(i);
					A3DSkinModelActionCore* pActionCore = pSkinModel->GetAction(pInfo->GetName());

					if (pActionCore)
						pActionCore->LoadActionTrackData();
				}
			}
		}
	}

#endif

	return true;
}

A3DGFXECModel& A3DGFXECModel::operator = (const A3DGFXECModel& src)
{
	if (this == &src)
		return *this;
	
	_CloneBase(&src);
	Init(src.m_pDevice);

	m_bUseCasterSkin = src.m_bUseCasterSkin;
	m_strActionName = src.m_strActionName;
	m_strUserCmd = src.m_strUserCmd;

	m_strECMFilename = src.m_strECMFilename;

	if (m_pECModel)
	{
		if (m_pLoader->GetLoaderType() == ECMLoader::TYPE_CUSTOM_ECM && m_pECModel->GetA3DSkinModel())
			g_nGfxCusECModelCount--;

		A3DRELEASE(m_pECModel);
	}

	if (m_pLoader->GetLoaderType() != src.m_pLoader->GetLoaderType())
	{
		ECMLoader::Destroy(m_pLoader);
		m_pLoader = src.m_pLoader->Clone(this);
	}

	return *this;
}

A3DGFXElement* A3DGFXECModel::Clone(A3DGFXEx* pGfx) const
{
	A3DGFXECModel* pNewEle = new A3DGFXECModel(pGfx);
	pNewEle->Init(m_pDevice);
	*pNewEle = *this;
	return pNewEle;
}

bool A3DGFXECModel::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch(nOp)
	{
	case ID_GFXOP_ECMODEL_PATH:
		m_strECMFilename = prop;
		return true;
	case ID_GFXOP_ECMODEL_USECASTERSKIN:
		m_bUseCasterSkin = prop;
		return true;
	case ID_GFXOP_ECMODEL_ACT_NAME:
		m_strActionName = prop;
		return true;
	case ID_GFXOP_ECMODEL_LDTYPE:
		{
			int iLdType = (int)prop;
			if (m_pLoader->GetLoaderType() != iLdType)
			{
				ECMLoader::Destroy(m_pLoader);
				m_pLoader = ECMLoader::Create(iLdType, this);
			}
		}
		return true;
	case ID_GFXOP_ECMODEL_USERCMD:
		m_strUserCmd = prop;
		return true;
	}
	return A3DGFXElement::SetProperty(nOp, prop);
}

GFX_PROPERTY A3DGFXECModel::GetProperty(int nOp) const
{
	switch(nOp)
	{
	case ID_GFXOP_ECMODEL_PATH:
		return GFX_PROPERTY(m_strECMFilename, GFX_VALUE_PATH_FILE);
	case ID_GFXOP_ECMODEL_USECASTERSKIN:
		return GFX_PROPERTY(m_bUseCasterSkin);
	case ID_GFXOP_ECMODEL_ACT_NAME:
		return GFX_PROPERTY(m_strActionName);
	case ID_GFXOP_ECMODEL_LDTYPE:
		return GFX_PROPERTY(m_pLoader->GetLoaderType()).SetType(GFX_VALUE_ECM_LDTYPE);
	case ID_GFXOP_ECMODEL_USERCMD:
		return GFX_PROPERTY(m_strUserCmd);
	}
	return A3DGFXElement::GetProperty(nOp);
}
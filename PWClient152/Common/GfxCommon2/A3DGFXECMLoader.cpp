/*
* FILE: A3DGFXECMLoader.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/01
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "A3DGFXECMLoader.h"
#include "A3DGFXECModel.h"
#include "A3DGFXECMInterface.h"

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

extern volatile int g_nGfxCusECModelCount;

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static bool Call_LoadCustomSkin(GFX_LOAD_ECM_PARAM* pParam)
{
	IGFXECMInterface* pGFXLoadECM = AfxGetGFXExMan()->GetGfxECMInterface();
	if (pGFXLoadECM)
	{
		return pGFXLoadECM->OnLoadCustomSkin(pParam);
	}

	if (!AfxGetGFXExMan()->GetGfxLoadSkinByID())
	{
		a_LogOutput(1, "Error in Call_LoadCustomSkin, unable to call back client because neither the IGFXLoadECM interface"
			"or the ON_GFX_LOAD_ECM_BY_ID is set");
		return false;
	}

	if (!(*AfxGetGFXExMan()->GetGfxLoadSkinByID())(pParam))
	{
		a_LogOutput(1, "Error in Call_LoadCustomSkin, failed to load skin by id %I64d and user cmd %s.", pParam->nId, pParam->strUserCmd);
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement ECMLoader
//	
///////////////////////////////////////////////////////////////////////////

ECMLoader::ECMLoader(A3DGFXECModel* pECMEle)
: m_pECMEle(pECMEle),m_iECMLoaderType(ECMLoader::TYPE_NORMAL)
{
	ASSERT(m_pECMEle);
}

ECMLoader* ECMLoader::Create(int iLdType, A3DGFXECModel* pECMEle)
{
	switch ( iLdType )
	{
	case TYPE_NORMAL:
		return new NormalLoader(pECMEle);
	case TYPE_CUSTOM_SKIN:
#ifdef _ANGELICA21
		// 应该不存在不同模型共用皮肤的情况，所以统一复制整个模型
		return new CustomECMLoader(pECMEle);
#else
		return new CustomSkinLoader(pECMEle);
#endif
	case TYPE_CUSTOM_ECM:
		return new CustomECMLoader(pECMEle);
	default:
		ASSERT(FALSE && "Unexpected error happened.");
		return new NormalLoader(pECMEle);
	}
}

void ECMLoader::Destroy(ECMLoader* pLoader)
{
	delete pLoader;
}

bool ECMLoader::OnInit(CECModel* pModel)
{
	if (!pModel)
		return false;

	TransferProperties(pModel);
	return true;
}

void ECMLoader::TransferProperties(CECModel* pModel)
{
	A3DGFXEx* pGFX = m_pECMEle->m_pGfx;
	if (!pGFX)
		return;

	pModel->SetCreatedByGfx(true);
	pModel->SetGfxUseLOD(pGFX->GetUseLOD(), false);
	pModel->SetDisableCamShake(pGFX->IsDisableCamShake());
	pModel->SetId(pGFX->GetId());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement NormalLoader
//	
///////////////////////////////////////////////////////////////////////////


NormalLoader::NormalLoader(A3DGFXECModel* pECMEle)
: ECMLoader(pECMEle)
{
	m_iECMLoaderType = ECMLoader::TYPE_NORMAL;
}

ECMLoader* NormalLoader::Clone(A3DGFXECModel* pECMEle) const
{
	return new NormalLoader(pECMEle);
}

bool NormalLoader::OnInit(CECModel* pModel)
{
	if (!ECMLoader::OnInit(pModel))
		return false;

#ifdef GFX_EDITOR
	const bool bLoadAdditionalSkin = true;
#else
	const bool bLoadAdditionalSkin = false;
#endif

	if (!pModel->Load(m_pECMEle->GetECMFileName(), true, A3DSkinModel::LSF_DEFAULT, true, bLoadAdditionalSkin, false))
	{
		a_LogOutput(1, "NormalLoader::OnInit, failed to load ecmodel file %s.", m_pECMEle->m_strECMFilename);
		return false;
	}

	return true;
}

bool NormalLoader::OnPlay(CECModel* pModel)
{
	TransferProperties(pModel);
	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CustomECMLoader
//	
///////////////////////////////////////////////////////////////////////////

CustomECMLoader::CustomECMLoader(A3DGFXECModel* pECMEle)
: ECMLoader(pECMEle), m_bStartLoadECM(false)
{
	m_iECMLoaderType = ECMLoader::TYPE_CUSTOM_ECM;
}

ECMLoader* CustomECMLoader::Clone(A3DGFXECModel* pECMEle) const
{
	return new CustomECMLoader(pECMEle);
}

bool CustomECMLoader::OnInit(CECModel* pModel)
{
	if (!ECMLoader::OnInit(pModel))
		return false;

#if defined(GFX_EDITOR) && !defined(_ANGELICA21)	// In GFXEDITOR

	const bool bLoadAdditionalSkin = true;

	if (!pModel->Load(m_pECMEle->GetECMFileName(), true, A3DSkinModel::LSF_DEFAULT, true, bLoadAdditionalSkin, false))
	{
		a_LogOutput(1, "CustomECMLoader::OnInit, failed to load ecmodel file %s.", m_pECMEle->m_strECMFilename);
		return false;
	}

#endif

	return true;
}

bool CustomECMLoader::OnPlay(CECModel* pModel)
{
#ifdef _ANGELICA21

	m_bStartLoadECM = false;
	return true;

#else

	ASSERT(pModel);

	IGFXECMInterface* pGfxLoadECM = AfxGetGFXExMan()->GetGfxECMInterface();
#ifdef GFX_EDITOR
	
	// 加入此处的 #ifdef / #endif 的理由是
	// 希望GFX编辑器中可以看到美术添加的默认ECM文件，而在ECM编辑器中可以看到用户命令产生的效果 —— 不同的行为
	// GFX编辑器中把该接口设置为NULL，在OnInit中加载模型，并在此直接返回，则可以看到默认的模型
	// ECM编辑器中该接口设置不为NULL，则会进入回调，按照需求加载模型，客户端的逻辑与此类似，只不过无需此判断
	if (!pGfxLoadECM)
		return true;
	
#else 

	if (!pGfxLoadECM)
	{
		a_LogOutput(1, "CustomECMLoader::OnPlay, Set IGFXECMInterface by A3DGFXExMan::SetGfxECMInterface");
		return false;
	}

#endif

	return LoadCustomECModel(pGfxLoadECM, pModel);

#endif
}

bool CustomECMLoader::LoadCustomECModel(IGFXECMInterface* pGfxLoadECM, CECModel* pModel)
{
	ASSERT(pGfxLoadECM);

	//	Transfer properties each time plays
	TransferProperties(pModel);

	A3DGFXEx* pGFX = m_pECMEle->m_pGfx;
	ASSERT(pGFX);
	GFX_LOAD_ECM_PARAM param;
	param.nId = pGFX->GetId();
	param.pECModel = pModel;
	param.strUserCmd = m_pECMEle->GetUserCmd();
	int nOldECMCount = m_pECMEle->GetSkinModel() ? 1 : 0;
	bool bRet = pGfxLoadECM->OnLoadCustomECModel(&param);
	int nNewECMCount = m_pECMEle->GetSkinModel() ? 1 : 0;
	extern volatile int g_nGfxCusECModelCount;
	g_nGfxCusECModelCount += (nNewECMCount - nOldECMCount);
	return bRet;
}

void CustomECMLoader::Clean(CECModel* pModel)
{
	ASSERT(pModel);

	if (pModel->GetA3DSkinModel())
		g_nGfxCusECModelCount--;

	pModel->Release();
}

void CustomECMLoader::Update(CECModel* pModel, const AString& strActName)
{
#if defined(_ANGELICA21) && !defined(GFX_EDITOR)

	if (m_bStartLoadECM)
		return;

	A3DGFXExMan* pGfxMan = AfxGetGFXExMan();

	if (GetCurrentThreadId() != pGfxMan->GetMainThreadID())
		return;

	m_bStartLoadECM = true;
	IGFXECMInterface* pGfxLoadECM = AfxGetGFXExMan()->GetGfxECMInterface();

	if (pGfxLoadECM)
	{
		if (LoadCustomECModel(pGfxLoadECM, pModel))
		{
			pModel->ShowCoGfx(true);

			if (!strActName.IsEmpty())
			{
				pModel->PlayActionByName(strActName, 1.0f, true, 0);
			}
		}
	}

#endif
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CustomSkinLoader
//	
///////////////////////////////////////////////////////////////////////////

CustomSkinLoader::CustomSkinLoader(A3DGFXECModel* pECMEle)
: ECMLoader(pECMEle)
{
	m_iECMLoaderType = ECMLoader::TYPE_CUSTOM_SKIN;
}

ECMLoader* CustomSkinLoader::Clone(A3DGFXECModel* pECMEle) const
{
	return new CustomSkinLoader(pECMEle);
}
	
bool CustomSkinLoader::OnInit(CECModel* pModel)
{
	if (!ECMLoader::OnInit(pModel))
		return false;

#ifdef GFX_EDITOR
	const bool bLoadAdditionalSkin = true;
#else
	const bool bLoadAdditionalSkin = false;
#endif

#ifdef _ANGELICA21

	if (!pModel->Load(m_pECMEle->GetECMFileName(), true, A3DSkinModel::LSF_NOSKIN, false, bLoadAdditionalSkin, false))
	{
		a_LogOutput(1, "CustomSkinLoader::OnInit, failed to load ecmodel file %s.", m_pECMEle->m_strECMFilename);
		return false;
	}

#else

	if (!pModel->Load(m_pECMEle->GetECMFileName(), true, A3DSkinModel::LSF_DEFAULT, true, bLoadAdditionalSkin, false))
	{
		a_LogOutput(1, "CustomSkinLoader::OnInit, failed to load ecmodel file %s.", m_pECMEle->m_strECMFilename);
		return false;
	}

#endif
		
	return true;
}

bool CustomSkinLoader::OnPlay(CECModel* pModel)
{
	//	Transfer properties each time plays
	TransferProperties(pModel);

	A3DGFXEx* pGFX = m_pECMEle->m_pGfx;
	ASSERT(pGFX);
	GFX_LOAD_ECM_PARAM param;
	param.nId = pGFX->GetId();
	param.pECModel = pModel;
	param.strUserCmd = m_pECMEle->GetUserCmd();

	return Call_LoadCustomSkin(&param);
}

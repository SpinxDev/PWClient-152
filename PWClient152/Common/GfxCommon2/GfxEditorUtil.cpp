/*
* FILE: GfxEditorUtil.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/22
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "GfxEditorUtil.h"
#include "EC_Model.h"
#include "A3DSkillGfxComposer.h"
#include <AFI.h>

#ifdef GFX_EDITOR

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

extern char* ECMPathFindFileNameA(const char* szPath);

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

static void _InsertFile(ExpFileContainer& aFiles, const char* szFile)
{
	aFiles[szFile]++;
}

//	Collect .ski and related files , currently used as helper function
static bool a_ExpCollectSKIFiles(A3DSkin* pSkin, bool bExpPhysics, ExpFileContainer& aFiles)
{
	ASSERT( pSkin );
	
	//	.ski file
	_InsertFile(aFiles, pSkin->GetFileName());

	if (bExpPhysics)
	{
		AString strPhysSyncFile(pSkin->GetFileName());
		af_ChangeFileExt(strPhysSyncFile, ".sphy");
		if (af_IsFileExist(strPhysSyncFile))
			_InsertFile(aFiles, strPhysSyncFile);
	}

	for (int iTextureIdx = 0; iTextureIdx < pSkin->GetTextureNum(); ++iTextureIdx)
	{
		A3DTexture* pTex = pSkin->GetTexture(iTextureIdx);
		ASSERT( pTex );

		//	texture files
		_InsertFile(aFiles, pTex->GetMapFile());
	}

	return true;
}

static bool a_ExpCollectSMDFiles(A3DSkinModel* pSkinModel, bool bExpPhysics, ExpFileContainer& aFiles)
{
	ASSERT( pSkinModel );
	A3DSkinModel& kSkinModel = *pSkinModel;

	//	.smd file
	_InsertFile(aFiles, pSkinModel->GetFileName());

	//	.bon file
	A3DSkeleton* pSkeleton = kSkinModel.GetSkeleton();
	_InsertFile(aFiles, pSkeleton->GetFileName());

	//	.phy file
	char szTmpBuf[MAX_PATH];
	strcpy(szTmpBuf, pSkinModel->GetFileName());
	af_ChangeFileExt(szTmpBuf, MAX_PATH, ".phy");
	if (af_IsFileExist(szTmpBuf))
		_InsertFile(aFiles, szTmpBuf);

	//	.stck file
	af_ChangeFileExt(szTmpBuf, MAX_PATH, ".stck");
	if (af_IsFileExist(szTmpBuf))
		_InsertFile(aFiles, szTmpBuf);

	//	.stck files
	for (A3DSkinModelActionCore* pAction = kSkinModel.GetFirstAction()
		; pAction
		; pAction = kSkinModel.GetNextAction())
	{
		_InsertFile(aFiles, pAction->GetTrackSetFileName());
	}

	for (int iSkinIdx = 0; iSkinIdx < kSkinModel.GetSkinNum(); ++iSkinIdx)
	{
		A3DSkin* pSkin = kSkinModel.GetA3DSkin(iSkinIdx);
		ASSERT( pSkin );

		//	.ski files
		if (!a_ExpCollectSKIFiles(pSkin, bExpPhysics, aFiles))
		{
			a_LogOutput(1, "Error a_ExpCollectSMDFiles, Failed to collect .ski files from %s", pSkin->GetFileName());
		}
	}

	return true;
}

static bool a_ExpCollectECMFiles(CECModel* pECModel, bool bExpChildModels, bool bExpAdditionalSkin, bool bExpPhysics, A3DEngine* pEngine, ExpFileContainer& aFiles)
{
	ASSERT( pECModel );
	CECModel& kECModel = *pECModel;

	AString strRelPath;
	af_GetRelativePathNoBase(pECModel->GetFilePath(), af_GetBaseDir(), strRelPath);

	// .ecm file
	_InsertFile(aFiles, strRelPath);
	
	// .smd related files
	if (!a_ExpCollectSMDFiles(kECModel.GetA3DSkinModel(), bExpPhysics, aFiles))
	{
		a_LogOutput(1, "Error a_ExpCollectECMFiles, Failed to collect ecm's smd files %s", pECModel->GetFilePath());
		return false;
	}

	if (bExpPhysics)
	{
		if (kECModel.GetModelPhysics() && !kECModel.GetStaticData()->GetPhysFileName().IsEmpty())
		{
			char szTmpBuf[MAX_PATH];
			strcpy(szTmpBuf, strRelPath);
			strcpy(ECMPathFindFileNameA(szTmpBuf), kECModel.GetStaticData()->GetPhysFileName());

			//	.mphy file
			_InsertFile(aFiles, szTmpBuf);
		}
	}

	if (bExpAdditionalSkin)
	{
		for (int iAdditionalSkinIdx = 0; iAdditionalSkinIdx < kECModel.GetAdditionalSkinCount(); ++iAdditionalSkinIdx)
		{
			AString strAddiSkiPath = "Models\\" + kECModel.GetAdditionalSkin(iAdditionalSkinIdx);

			for (int iSkiIdx = 0; iSkiIdx < kECModel.GetSkinNum(); ++iSkiIdx)
			{
				A3DSkin* pSkin = kECModel.GetA3DSkin(iSkiIdx);
				ASSERT( pSkin );

				if (strAddiSkiPath.CompareNoCase(pSkin->GetFileName()) != 0)
					continue;

				if (!a_ExpCollectSKIFiles(pSkin, bExpPhysics, aFiles))
				{
					a_LogOutput(1, "Error a_ExpCollectECMFiles, Failed to collect .ski files from %s", pSkin->GetFileName());
				}

				break;
			}
		}
	}

	if (bExpChildModels)
	{
		for (int iChildModelIdx = 0; iChildModelIdx < kECModel.GetChildCount(); ++iChildModelIdx)
		{
			CECModel* pChild = kECModel.GetChildModel(iChildModelIdx);
			ASSERT( pChild );

			if (!a_ExpCollectECMFiles(pChild, bExpChildModels, bExpAdditionalSkin, bExpPhysics, pEngine, aFiles))
			{
				a_LogOutput(1, "Error a_ExpCollectECMFiles, Failed to collect child ecm files %s", pChild->GetFilePath());
			}
		}
	}

	for (int iComActIdx = 0; iComActIdx < kECModel.GetComActCount(); ++iComActIdx)
	{
		A3DCombinedAction* pComAct =  kECModel.GetComActByIndex(iComActIdx);

		ASSERT( pComAct );

		for (int iEventIdx = 0; iEventIdx < pComAct->GetEventCount(); ++iEventIdx)
		{
			EVENT_INFO* pInfo = pComAct->GetEventInfo(iEventIdx);

			ASSERT( pInfo );

			if (pInfo->GetType() == EVENT_TYPE_GFX)
			{
				GFX_INFO* pGFXINFO = (GFX_INFO*)pInfo;

				for (int iIdx = 0; iIdx < pGFXINFO->GetFilePathCount(); ++iIdx)
				{
					EXPFILEDATA recurExp(pGFXINFO->GetFilePathByIndex(iIdx), pEngine, bExpChildModels, bExpAdditionalSkin, bExpPhysics, aFiles);
					if (!a_ExpCollectGFXFiles(&recurExp))
					{
						a_LogOutput(1, "Error in a_ExpCollectECMFiles, Failed to collect gfx related files %s", pGFXINFO->GetFilePath());
					}
				}
			}

			if (pInfo->GetType() == EVENT_TYPE_SFX)
			{
				SFX_INFO* pSFXINFO = (SFX_INFO*)pInfo;
				for (int iIdx = 0; iIdx < pSFXINFO->GetFilePathCount(); ++iIdx)
				{
					_InsertFile(aFiles, AString("Sfx\\") + pSFXINFO->GetFilePathByIndex(iIdx));
				}
			}

			if (pInfo->GetType() == EVENT_TYPE_ATT_PT)
			{
				SGCAttackPointMark* pATTINFO = (SGCAttackPointMark*)pInfo;

				EXPFILEDATA recurExp(pATTINFO->GetAtkFile(), pEngine, bExpChildModels, bExpAdditionalSkin, bExpPhysics, aFiles);
				if (!a_ExpCollectATTFiles(&recurExp))
				{
					a_LogOutput(1, "Error in a_ExpCollectECMFiles, Failed to collect att related files %s", pATTINFO->GetAtkFile());
				}
			}
		}
	}

	return true;
}

//	Collect files the .smd file needed (for export)
//	EXPFILEDATA::strRelFile is a path relative to Angelica base dir
//	Return true for successfully collected files, false for error
//	This function is only defined in the editor version of GFX library
bool a_ExpCollectSMDFiles(EXPFILEDATA* pExpData)
{
	if (!pExpData || !pExpData->m_pEngine || !af_CheckFileExt(pExpData->m_strRelFile, ".smd"))
	{
		a_LogOutput(1, "Error a_ExpCollectSMDFiles, Invalid parameter");
		return false;
	}

	AString strRelPath;
	af_GetRelativePath(pExpData->m_strRelFile, af_GetBaseDir(), strRelPath);

	A3DSkinModel kSkinModel;
	if (!kSkinModel.Init(pExpData->m_pEngine) || !kSkinModel.Load(strRelPath))
	{
		a_LogOutput(1, "Error a_ExpCollectSMDFiles, Failed to load a3dskinmodel %s", strRelPath);
		return false;
	}

	if (!a_ExpCollectSMDFiles(&kSkinModel, pExpData->m_bExpPhysics, pExpData->m_aFiles))
	{
		a_LogOutput(1, "Error a_ExpCollectSMDFiles, Failed to collect smd related files %s", pExpData->m_strRelFile);
		return false;
	}

	kSkinModel.Release();

	return true;
}

//	Collect files the .ecm file needed (for export)
//	EXPFILEDATA::strRelFile is a path relative to Angelica base dir
//	Return true for successfully collected files, false for error
//	This function is only defined in the editor version of GFX library
bool a_ExpCollectECMFiles(EXPFILEDATA* pExpData)
{
	if (!pExpData || !pExpData->m_pEngine || !af_CheckFileExt(pExpData->m_strRelFile, ".ecm"))
	{
		a_LogOutput(1, "Error a_ExpCollectECMFiles, Invalid parameter");
		return false;
	}

	AString strRelPath;
	af_GetRelativePath(pExpData->m_strRelFile, af_GetBaseDir(), strRelPath);

	CECModel kECModel;
	if (!kECModel.Load(strRelPath
		, true
		, 0
		, pExpData->m_bExpChildModels
		, pExpData->m_bExpAdditionalSkin
		, pExpData->m_bExpPhysics))
	{
		a_LogOutput(1, "Error a_ExpCollectECMFiles, Failed to load ecm file %s", strRelPath);
		return false;
	}

	if (!a_ExpCollectECMFiles(&kECModel
		, pExpData->m_bExpChildModels
		, pExpData->m_bExpAdditionalSkin
		, pExpData->m_bExpPhysics
		, pExpData->m_pEngine
		, pExpData->m_aFiles))
	{
		a_LogOutput(1, "Error a_ExpCollectECMFiles, Failed to collect .ecm related files %s", strRelPath);
		return false;
	}

	return true;
}

//	Collect files the .gfx file needed (for export)
//	EXPFILEDATA::strRelFile is a path relative to Angelica base dir
//	Return true for successfully collected files, false for error
//	This function is only defined in the editor version of GFX library
bool a_ExpCollectGFXFiles(EXPFILEDATA* pExpData)
{
	if (!pExpData || !pExpData->m_pEngine || !af_CheckFileExt(pExpData->m_strRelFile, ".gfx"))
	{
		a_LogOutput(1, "Error a_ExpCollectGFXFiles, Invalid parameter");
		return false;
	}

	A3DGFXEx* pGFX = AfxGetGFXExMan()->LoadGfx(pExpData->m_pEngine->GetA3DDevice(), pExpData->m_strRelFile);
	if (!pGFX)
	{
		a_LogOutput(1, "Error in a_ExpCollectGFXFiles, Failed to load gfx file %s", pExpData->m_strRelFile);
		return false;
	}

	//	.gfx file
	_InsertFile(pExpData->m_aFiles, "Gfx\\" + pExpData->m_strRelFile);
	
	for (int iEleIdx = 0; iEleIdx < pGFX->GetElementCount(); ++iEleIdx)
	{
		const A3DGFXElement* pEle = pGFX->GetElement(iEleIdx);
		if (pEle->GetEleTypeId() == ID_ELE_TYPE_GFX_CONTAINER)
		{
			AString strPath = pEle->GetProperty(ID_GFXOP_CONTAINER_GFX_PATH);
			EXPFILEDATA recurExpData(strPath
				, pExpData->m_pEngine
				, pExpData->m_bExpChildModels
				, pExpData->m_bExpAdditionalSkin
				, pExpData->m_bExpPhysics
				, pExpData->m_aFiles);

			a_ExpCollectGFXFiles(&recurExpData);
		}
		else if (pEle->GetEleTypeId() == ID_ELE_TYPE_MODEL)
		{
			AString strPath = pEle->GetProperty(ID_GFXOP_MODEL_PATH);
			EXPFILEDATA recurExpData("Gfx\\Models\\" + strPath
				, pExpData->m_pEngine
				, pExpData->m_bExpChildModels
				, pExpData->m_bExpAdditionalSkin
				, pExpData->m_bExpPhysics
				, pExpData->m_aFiles);

			a_ExpCollectSMDFiles(&recurExpData);
		}
		else if (pEle->GetEleTypeId() == ID_ELE_TYPE_ECMODEL)
		{
			AString strPath = pEle->GetProperty(ID_GFXOP_ECMODEL_PATH);
			EXPFILEDATA recurExpData(strPath
				, pExpData->m_pEngine
				, pExpData->m_bExpChildModels
				, pExpData->m_bExpAdditionalSkin
				, pExpData->m_bExpPhysics
				, pExpData->m_aFiles);

			a_ExpCollectECMFiles(&recurExpData);
		}
		else if (pEle->GetEleTypeId() == ID_ELE_TYPE_SOUND)
		{
			AString strSound = pEle->GetProperty(ID_GFXOP_SOUND_FILE);
			
			_InsertFile(pExpData->m_aFiles, "Sfx\\" + strSound);
		}
		
		AString strTex = pEle->GetProperty(ID_GFXOP_TEX_PATH);
		if (!strTex.IsEmpty())
			_InsertFile(pExpData->m_aFiles, "Gfx\\Textures\\" + strTex);
		
	}

	pGFX->Release();
	delete pGFX;

	return true;
}

//	Collect files the .att file needed (for export)
//	EXPFILEDATA::strRelFile is a path relative to Angelica base dir
//	Return true for successfully collected files, false for error
//	This function is only defined in the editor version of GFX library
bool a_ExpCollectATTFiles(EXPFILEDATA* pExpData)
{
	using namespace _SGC;
	if (!pExpData || !pExpData->m_pEngine || !af_CheckFileExt(pExpData->m_strRelFile, ".att"))
	{
		a_LogOutput(1, "Error a_ExpCollectATTFiles, Invalid parameter");
		return false;
	}

	AString strAttRelPath = "Gfx\\SkillAttack\\" + pExpData->m_strRelFile;
	A3DSkillGfxComposer kComposer;
	if (!kComposer.Load(strAttRelPath))
	{
		a_LogOutput(1, "Error a_ExpCollectATTFiles, Failed to load att file %s", strAttRelPath);
		return false;
	}

	_InsertFile(pExpData->m_aFiles, strAttRelPath);

	if (kComposer.m_szFlyGfx[0])
	{
		EXPFILEDATA recurExpData(kComposer.m_szFlyGfx
			, pExpData->m_pEngine
			, pExpData->m_bExpChildModels
			, pExpData->m_bExpAdditionalSkin
			, pExpData->m_bExpPhysics
			, pExpData->m_aFiles);

		if (!a_ExpCollectGFXFiles(&recurExpData))
		{
			a_LogOutput(1, "Error a_ExpCollectATTFiles, Failed to collect fly gfx %s related files", kComposer.m_szFlyGfx);
		}
	}

	if (kComposer.m_szHitGfx[0])
	{
		EXPFILEDATA recurExpData(kComposer.m_szHitGfx
			, pExpData->m_pEngine
			, pExpData->m_bExpChildModels
			, pExpData->m_bExpAdditionalSkin
			, pExpData->m_bExpPhysics
			, pExpData->m_aFiles);

		if (!a_ExpCollectGFXFiles(&recurExpData))
		{
			a_LogOutput(1, "Error a_ExpCollectATTFiles, Failed to collect hit gfx %s related files", kComposer.m_szHitGfx);
		}
	}

	if (kComposer.m_szHitGrndGfx[0])
	{
		EXPFILEDATA recurExpData(kComposer.m_szHitGrndGfx
			, pExpData->m_pEngine
			, pExpData->m_bExpChildModels
			, pExpData->m_bExpAdditionalSkin
			, pExpData->m_bExpPhysics
			, pExpData->m_aFiles);

		if (!a_ExpCollectGFXFiles(&recurExpData))
		{
			a_LogOutput(1, "Error a_ExpCollectATTFiles, Failed to collect hit ground gfx %s related files", kComposer.m_szHitGrndGfx);
		}
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////
//
//	Implement EXPFILEDATA
//
///////////////////////////////////////////////////////////////////////////

EXPFILEDATA::EXPFILEDATA(const char* strRelFile
						 , A3DEngine* pEngine
						 , bool bExpChildModels
						 , bool bExpAdditionalSkin
						 , bool bExpPhysics
						 , ExpFileContainer& aFiles)
: m_strRelFile(strRelFile)
, m_pEngine(pEngine)
, m_bExpChildModels(bExpChildModels)
, m_bExpAdditionalSkin(bExpAdditionalSkin)
, m_bExpPhysics(bExpPhysics)
, m_aFiles(aFiles)
{

}


#endif


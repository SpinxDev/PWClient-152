/*
 * FILE: EC_Matter.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Matter.h"
#include "EC_Viewport.h"
#include "EC_GameRun.h"
#include "EC_GFXCaster.h"
#include "EC_World.h"
#include "EC_HostPlayer.h"
#include "EC_Resource.h"
#include "EC_Model.h"
#include "EC_PateText.h"
#include "EC_Configs.h"
#include "EC_SceneLoader.h"
#include "EC_Utility.h"
#include "elementdataman.h"

#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"
#include "A3DTerrain2.h"
#include "A3DLight.h"
#include "A3DCamera.h"
#include "A3DViewport.h"
#include "A3DSkinMan.h"
#include "A3DCollision.h"
#include "EC_UIManager.h"
#include <AFI.h>
#include <AUIManager.h>

#define new A_DEBUG_NEW

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

//	Load matter model
CECModel* CECMatter::LoadMatterModel(const char* szFile)
{
//	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("armor.sdr", A3DSkinMan::SHADERREPLACE_REFLECTPREFIX);
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile((const char*)glb_ArmorReplaceShader_ReflectPrefix, A3DSkinMan::SHADERREPLACE_USERDEFINE);

	CECModel* pModel = new CECModel;
	if (!pModel)
	{
		g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
		return NULL;
	}

	if (!pModel->Load(szFile))
	{
		a_LogOutput(1, "CECMatter::LoadMatterModel, Failed to load matter model %s", szFile);

		if (!pModel->Load("models\\error\\error.ecm"))
		{
			g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
			delete pModel;
			return NULL;
		}
	}

	//	If this model hasn't skin, add one here
	A3DSkinModel* pSkinModel = pModel->GetA3DSkinModel();
	if (pSkinModel && !pSkinModel->GetSkinNum())
	{
		char szSkinFile[MAX_PATH];
		strcpy(szSkinFile, pSkinModel->GetFileName());
		af_ChangeFileExt(szSkinFile, MAX_PATH, ".ski");
		pSkinModel->AddSkinFile(szSkinFile, true);
	}
	
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
	return pModel;
}

//	Release matter model
void CECMatter::ReleaseMatterModel(CECModel* pModel)
{
	if (pModel)
	{
		QueueECModelForRelease(pModel);
	//	pModel->Release();
	//	delete pModel;
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECMatter
//	
///////////////////////////////////////////////////////////////////////////

CECMatter::CECMatter(CECMatterMan* pMatterMan)
{
	m_iCID			= OCID_MATTER;
	m_pMatterMan	= pMatterMan;
	m_pModel		= NULL;
	m_pGfx			= NULL;
	m_szModelFile	= NULL;
	m_szName		= NULL;
	m_fDistToHost	= 0.0f;
	m_fDistToCamera	= 0.0f;
	m_fDistToHostH	= 0.0f;
	m_bRenderName	= false;
	m_bDynModelObj	= false;
	m_pPateName		= NULL;
	m_dwMatterType	= 0;
	m_iLevelReq		= 0;
	m_nBrushes		= 0;
	m_ppBrushes		= NULL;
	m_bMonsterSpiritMine = false;
	m_fGatherDist = 3.f;
	
	if ((m_pPateName = new CECPateText))
		m_pPateName->EnableBorder(true);

	m_aabb.Center	= g_vOrigin;
	m_aabb.Extents.Set(0.3f, 0.3f, 0.3f);
	m_aabb.CompleteMinsMaxs();

	memset(&m_MatterInfo, 0, sizeof (m_MatterInfo));
}

CECMatter::~CECMatter()
{
}

//	Initialize object
bool CECMatter::Init(const S2C::info_matter& Info)
{
	m_MatterInfo.mid	= Info.mid;
	m_MatterInfo.tid	= Info.tid & 0x0000ffff;
	m_bDynModelObj		= (Info.state & 0x1) != 0;
	m_bMonsterSpiritMine = (Info.state & 0x2) != 0;
	//	Read data from database
	if (!m_bDynModelObj)
		ReadDataFromDatabase(m_MatterInfo.tid);

	if (Info.tid & 0x80000000)
		m_dwMatterType |= ITEMFLAG_EXTPROP;

	//	Submit matter model loading request to loading thread
	if (m_bDynModelObj)
	{
		const char* szFile = g_pGame->GetDynObjectPath(Info.tid);
		if (!szFile || strlen(szFile) < 1)
		{
			a_LogOutput(1, "CECMatter::Init, empty file path for id=%d", Info.tid);
			g_dwFatalErrorFlag = FATAL_ERROR_LOAD_BUILDING;
			return false;
		}
		if (af_CheckFileExt(szFile, ".ecm"))
		{
			if( !af_IsFileExist(szFile) )
			{
				a_LogOutput(1, "CECMatter::Init, File not exist: %s", szFile);
				g_dwFatalErrorFlag = FATAL_ERROR_LOAD_BUILDING;
				return false;
			}

			QueueECModelForLoad(MTL_ECM_MATTER, Info.mid, 0, Info.pos, szFile);
		}
		else if (af_CheckFileExt(szFile, ".gfx"))
			LoadGFXFromFile(szFile, Info.value);
	}
	else
		QueueECModelForLoad(MTL_ECM_MATTER, Info.mid, 0, Info.pos, m_szModelFile);

	//	Ensure matter won't be dropped underground
	A3DVECTOR3 vPos, vNormal;
	if (VertRayTrace(Info.pos + g_vAxisY * 0.2f, vPos, vNormal, 10.0f))
	{
		if (vPos.y < Info.pos.y)
			vPos.y = Info.pos.y;
	}
	else
		vPos = Info.pos;

	A3DVECTOR3 vAxis = a3d_DecompressDir(Info.dir0, Info.dir1);
	A3DQUATERNION q(vAxis, Info.rad / 255.0f * A3D_2PI);
	A3DMATRIX4 matTran;
	q.ConvertToMatrix(matTran);

	SetPos(vPos);
	SetDirAndUp(matTran.GetRow(2), matTran.GetRow(1));

	return true;
}

//	Release object
void CECMatter::Release()
{
	//	Release model
	if (m_pModel)
	{
		ReleaseMatterModel(m_pModel);
		m_pModel = NULL;
	}

	//	Release GFX
	A3DRELEASE(m_pGfx);

	if (m_pPateName)
	{
		delete m_pPateName;
		m_pPateName = NULL;
	}

	if (m_ppBrushes)
	{
		for (int i = 0; i < m_nBrushes; i++)
			delete m_ppBrushes[i];

		delete[] m_ppBrushes;
		m_ppBrushes = NULL;
	}

	m_nBrushes = 0;
}

//	Set loaded model to matter object, this function is used in multithread loading process
bool CECMatter::SetLoadedMatterModel(CECModel* pModel)
{
	if (m_pModel)
	{
		//	This case is seldom happed, but we have to handle it. See
		//	CECNPC::SetNPCLoadedResult() for detail information.
		ReleaseMatterModel(m_pModel);
	}

	m_pModel = pModel;

	if (m_bDynModelObj)
	{
		m_nBrushes = 0;
		m_ppBrushes = NULL;

		// now construct a CCDBrush from CConvexhullData;
		if( m_pModel && m_pModel->HasCHAABB() )
		{
			ConvexHullDataArray& cdArray = m_pModel->GetConvexHullData();

			A3DMATRIX4 matTM = TransformMatrix(GetDir(), GetUp(), GetPos());
			if( int(cdArray.size()) != m_nBrushes )
			{
				m_nBrushes = cdArray.size();
				m_ppBrushes = new CCDBrush *[cdArray.size()];

				for(int i=0; i<m_nBrushes; i++)
				{
					CConvexHullData chData = *cdArray[i];
					chData.Transform(matTM);

					CQBrush qBrush;
					qBrush.AddBrushBevels(&chData);
					CCDBrush * pCDBrush = new CCDBrush();
					qBrush.Export(pCDBrush);

					m_ppBrushes[i] = pCDBrush;
				}
			}
		}
	}
	else if (m_pModel->GetA3DSkinModel())
	{
		//	Select a action according to metter's environment
		AString strAct;
		A3DVECTOR3 vPos = GetPos();
		CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();

		if (vPos.y < pWorld->GetWaterHeight(vPos) - 0.2f)
			strAct = "掉落保护_水中";
		else if (vPos.y > pWorld->GetTerrainHeight(vPos, NULL) + 0.2f)
			strAct = "掉落保护_空中";
		else
			strAct = "掉落保护";

		m_pModel->PlayActionByName(strAct, 1.0f);
		//	Force to use mesh aabb
		m_pModel->SetAABBType(CECModel::AABB_INITMESH);
	}

	SetPos(GetPos());
	SetDirAndUp(GetDir(), GetUp());

	m_pModel->SetAutoUpdateFlag(false);
	m_pModel->Tick(1);
	m_pModel->SetAutoUpdateFlag(true);

	if (m_pModel->GetA3DSkinModel())
	{
		//	Set matter's aabb and limit it to a proper extent
		m_aabb.Extents = m_pModel->GetA3DSkinModel()->GetModelAABB().Extents;
		a_Clamp(m_aabb.Extents.x, 0.3f, 1.0f);
		a_Clamp(m_aabb.Extents.y, 0.3f, 1.0f);
		a_Clamp(m_aabb.Extents.z, 0.3f, 1.0f);
		m_aabb.CompleteMinsMaxs();
	}

	return true;
}

//	Process message
bool CECMatter::ProcessMessage(const ECMSG& Msg) 
{ 
	return true; 
}

//	Read matter data from database
bool CECMatter::ReadDataFromDatabase(int tid)
{
	if (!tid)
	{
		ASSERT(0);
		return true;
	}

	//	Set default value
	m_dwMatterType = (tid == 3044) ? MATTER_MONEY : MATTER_ITEM;

	DATA_TYPE DataType;
	const void* pDataPtr = g_pGame->GetElementDataMan()->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	int iFixProps = 0;

	switch (DataType)
	{
	case DT_WEAPON_ESSENCE:
	{
		const WEAPON_ESSENCE* pData = (const WEAPON_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		iFixProps		= pData->fixed_props;
		break;
	}
	case DT_ARMOR_ESSENCE:
	{
		const ARMOR_ESSENCE* pData = (const ARMOR_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		iFixProps		= pData->fixed_props;
		break;
	}
	case DT_DECORATION_ESSENCE:
	{
		const DECORATION_ESSENCE* pData = (const DECORATION_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		iFixProps		= pData->fixed_props;
		break;
	}
	case DT_FASHION_ESSENCE:
	{
		const FASHION_ESSENCE* pData = (const FASHION_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_MEDICINE_ESSENCE:
	{
		const MEDICINE_ESSENCE* pData = (const MEDICINE_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_MATERIAL_ESSENCE:
	{
		const MATERIAL_ESSENCE* pData = (const MATERIAL_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_DAMAGERUNE_ESSENCE:
	{
		const DAMAGERUNE_ESSENCE* pData = (const DAMAGERUNE_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_ARMORRUNE_ESSENCE:
	{
		const ARMORRUNE_ESSENCE* pData = (const ARMORRUNE_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_SKILLTOME_ESSENCE:
	{
		const SKILLTOME_ESSENCE* pData = (const SKILLTOME_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_FLYSWORD_ESSENCE:
	{
		const FLYSWORD_ESSENCE* pData = (const FLYSWORD_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_TOWNSCROLL_ESSENCE:
	{
		const TOWNSCROLL_ESSENCE* pData = (const TOWNSCROLL_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_UNIONSCROLL_ESSENCE:
	{
		const UNIONSCROLL_ESSENCE* pData = (const UNIONSCROLL_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_REVIVESCROLL_ESSENCE:
	{
		const REVIVESCROLL_ESSENCE* pData = (const REVIVESCROLL_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_ELEMENT_ESSENCE:
	{
		const ELEMENT_ESSENCE* pData = (const ELEMENT_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_TOSSMATTER_ESSENCE:
	{
		const TOSSMATTER_ESSENCE* pData = (const TOSSMATTER_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
/*	case DT_TASKMATTER_ESSENCE:
	{
		//	Note: Task matter shouldn't be dropped on ground !!
		const TASKMATTER_ESSENCE* pData = (const TASKMATTER_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
	}
*/	case DT_PROJECTILE_ESSENCE:
	{
		const PROJECTILE_ESSENCE* pData = (const PROJECTILE_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_STONE_ESSENCE:
	{
		const STONE_ESSENCE* pData = (const STONE_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_WINGMANWING_ESSENCE:
	{
		const WINGMANWING_ESSENCE* pData = (const WINGMANWING_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_TASKDICE_ESSENCE:
	{
		const TASKDICE_ESSENCE* pData = (const TASKDICE_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_TASKNORMALMATTER_ESSENCE:
	{
		const TASKNORMALMATTER_ESSENCE* pData = (const TASKNORMALMATTER_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_MINE_ESSENCE:
	{
		const MINE_ESSENCE* pData = (const MINE_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_model;
		m_dwMatterType	= MATTER_MINE;
		m_iLevelReq		= pData->level_required;
		m_fGatherDist	= pData->gather_dist > 3.f ? pData->gather_dist - 1.f : 3.f;
		break;
	}
	case DT_FACETICKET_ESSENCE:
	{
		const FACETICKET_ESSENCE* pData = (const FACETICKET_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_FACEPILL_ESSENCE:
	{
		const FACEPILL_ESSENCE* pData = (const FACEPILL_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_GM_GENERATOR_ESSENCE:
	{
		const GM_GENERATOR_ESSENCE* pData = (const GM_GENERATOR_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_PET_EGG_ESSENCE:
	{
		const PET_EGG_ESSENCE* pData = (const PET_EGG_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_PET_FOOD_ESSENCE:
	{
		const PET_FOOD_ESSENCE* pData = (const PET_FOOD_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_PET_FACETICKET_ESSENCE:
	{
		const PET_FACETICKET_ESSENCE* pData = (const PET_FACETICKET_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_FIREWORKS_ESSENCE:
	{
		const FIREWORKS_ESSENCE* pData = (const FIREWORKS_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_WAR_TANKCALLIN_ESSENCE:
	{
		const WAR_TANKCALLIN_ESSENCE* pData = (const WAR_TANKCALLIN_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_SKILLMATTER_ESSENCE:
	{
		const SKILLMATTER_ESSENCE* pData = (const SKILLMATTER_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_INC_SKILL_ABILITY_ESSENCE:
	{
		const INC_SKILL_ABILITY_ESSENCE* pData = (const INC_SKILL_ABILITY_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_REFINE_TICKET_ESSENCE:
	{
		const REFINE_TICKET_ESSENCE* pData = (const REFINE_TICKET_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_DESTROYING_ESSENCE:
	{
		const DESTROYING_ESSENCE* pData = (const DESTROYING_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}	
	case DT_BIBLE_ESSENCE:
	{
		const BIBLE_ESSENCE* pData = (const BIBLE_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}	
	case DT_SPEAKER_ESSENCE:
	{
		const SPEAKER_ESSENCE* pData = (const SPEAKER_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}	
	case DT_AUTOHP_ESSENCE:
	{
		const AUTOHP_ESSENCE* pData = (const AUTOHP_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}	
	case DT_AUTOMP_ESSENCE:
	{
		const AUTOMP_ESSENCE* pData = (const AUTOMP_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}	
	case DT_DOUBLE_EXP_ESSENCE:
	{
		const DOUBLE_EXP_ESSENCE* pData = (const DOUBLE_EXP_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_TRANSMITSCROLL_ESSENCE:
	{
		const TRANSMITSCROLL_ESSENCE* pData = (const TRANSMITSCROLL_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}	
	case DT_DYE_TICKET_ESSENCE:
	{
		const DYE_TICKET_ESSENCE* pData = (const DYE_TICKET_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}	
	case DT_GOBLIN_ESSENCE:
	{
		const GOBLIN_ESSENCE* pData = (const GOBLIN_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}	
	case DT_GOBLIN_EQUIP_ESSENCE:
	{
		const GOBLIN_EQUIP_ESSENCE* pData = (const GOBLIN_EQUIP_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
//		m_szModelFile	= pData->file_matter;  //  name...
		break;
	}	
	case DT_GOBLIN_EXPPILL_ESSENCE:
	{
		const GOBLIN_EXPPILL_ESSENCE* pData = (const GOBLIN_EXPPILL_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}	
	case DT_SELL_CERTIFICATE_ESSENCE:
	{
		const SELL_CERTIFICATE_ESSENCE* pData = (const SELL_CERTIFICATE_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}	
	case DT_TARGET_ITEM_ESSENCE:
	{
		const TARGET_ITEM_ESSENCE* pData = (const TARGET_ITEM_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}
	case DT_LOOK_INFO_ESSENCE:
	{
		const LOOK_INFO_ESSENCE* pData = (const LOOK_INFO_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}

	case DT_WEDDING_BOOKCARD_ESSENCE:
	{
		const WEDDING_BOOKCARD_ESSENCE* pData = (const WEDDING_BOOKCARD_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}

	case DT_WEDDING_INVITECARD_ESSENCE:
	{
		const WEDDING_INVITECARD_ESSENCE* pData = (const WEDDING_INVITECARD_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}

	case DT_SHARPENER_ESSENCE:
	{
		const SHARPENER_ESSENCE* pData = (const SHARPENER_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}

	case DT_FACTION_MATERIAL_ESSENCE:
	{
		const FACTION_MATERIAL_ESSENCE* pData = (const FACTION_MATERIAL_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}

	case DT_CONGREGATE_ESSENCE:
	{
		const CONGREGATE_ESSENCE* pData = (const CONGREGATE_ESSENCE*)pDataPtr;
		m_szName		= pData->name;
		m_szModelFile	= pData->file_matter;
		break;
	}

	case DT_FORCE_TOKEN_ESSENCE:
		{
			const FORCE_TOKEN_ESSENCE* pData = (const FORCE_TOKEN_ESSENCE*)pDataPtr;
			m_szName		= pData->name;
			m_szModelFile	= pData->file_matter;
			break;
		}

	case DT_DYNSKILLEQUIP_ESSENCE:
		{
			const DYNSKILLEQUIP_ESSENCE* pData = (const DYNSKILLEQUIP_ESSENCE*)pDataPtr;
			m_szName		= pData->name;
			m_szModelFile	= pData->file_matter;
			break;
		}
		
	case DT_MONEY_CONVERTIBLE_ESSENCE:
		{
			const MONEY_CONVERTIBLE_ESSENCE* pData = (const MONEY_CONVERTIBLE_ESSENCE*)pDataPtr;
			m_szName		= pData->name;
			m_szModelFile	= pData->file_matter;
			break;
		}
	case DT_POKER_ESSENCE:
		{
			const POKER_ESSENCE* pData = (const POKER_ESSENCE*)pDataPtr;
			m_szName = pData->name;
			m_szModelFile = pData->file_matter;
			break;
		}
	case DT_POKER_DICE_ESSENCE:
		{
			const POKER_DICE_ESSENCE* pData = (const POKER_DICE_ESSENCE*)pDataPtr;
			m_szName = pData->name;
			m_szModelFile = pData->file_matter;
			break;
		}
	case DT_SHOP_TOKEN_ESSENCE:
		{
			const SHOP_TOKEN_ESSENCE* pData = (const SHOP_TOKEN_ESSENCE*)pDataPtr;
			m_szName = pData->name;
			m_szModelFile = pData->file_matter;
			break;
		}
	case DT_UNIVERSAL_TOKEN_ESSENCE:
		{
			const UNIVERSAL_TOKEN_ESSENCE* pData = (const UNIVERSAL_TOKEN_ESSENCE*)pDataPtr;
			m_szName = pData->name;
			m_szModelFile = pData->file_matter;
			break;
		}

	default:

		//ASSERT(0);
		break;
	}

	switch (iFixProps)
	{
	case 1:	m_dwMatterType |= ITEMFLAG_SUIT;		break;
	case 2:	m_dwMatterType |= ITEMFLAG_RARE;		break;
	case 3:	m_dwMatterType |= ITEMFLAG_UNIQUE;		break;
//	case 3:	m_dwMatterType |= ITEMFLAG_MYTHICAL;	break;
	}

	if (!m_szModelFile || !m_szModelFile[0])
		m_szModelFile = res_ModelFile(RES_MOD_TEMP);

	//	Set name
	if (m_pPateName)
		m_pPateName->SetText(m_szName, false);

	//	test code ...
//	m_szModelFile = res_ModelFile(RES_MOD_TEMP);

	return true;
}

//	Set absolute position
void CECMatter::SetPos(const A3DVECTOR3& vPos)
{
	CECObject::SetPos(vPos);

	m_aabb.Center = vPos + A3DVECTOR3(0.0f, m_aabb.Extents.y, 0.0f);
	m_aabb.CompleteMinsMaxs();

	if (m_pModel)
		m_pModel->SetPos(vPos);

	if (m_pGfx)
		m_pGfx->SetPos(vPos);
}

//	Set absolute forward and up direction
void CECMatter::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	CECObject::SetDirAndUp(vDir, vUp);

	if (m_pModel)
		m_pModel->SetDirAndUp(vDir, vUp);

	if (m_pGfx)
		m_pGfx->SetDirAndUp(vDir, vUp);
}

//	Tick routine
bool CECMatter::Tick(DWORD dwDeltaTime) 
{
	if (m_pModel)
		m_pModel->Tick(dwDeltaTime);

	if (m_pGfx)
		m_pGfx->TickAnimation(dwDeltaTime);

	//	Calculate distance to host player
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost && pHost->IsSkeletonReady())
	{
		m_fDistToHost  = CalcDist(pHost->GetPos(), true);
		m_fDistToHostH = CalcDist(pHost->GetPos(), false);
	}

	return true; 
}

//	Interval render routine
bool CECMatter::InternalRender(CECViewport* pViewport, bool bHighLight)
{
	A3DCameraBase* pCamera = pViewport->GetA3DCamera();
	m_fDistToCamera	= CalcDist(pCamera->GetPos(), true);

	if (!m_bVisible)
		return true;

	if (m_pModel)
	{
		if (bHighLight)
			g_pGame->RenderHighLightModel(pViewport, m_pModel);
		else
			m_pModel->Render(pViewport->GetA3DViewport());
	}

	if (m_pGfx)
		g_pGame->GetGFXCaster()->GetGFXExMan()->RegisterGfx(m_pGfx);

	if (m_bRenderName)
		RenderName(pViewport);

	return true;
}

//	Render titles / names / talks above player's header
bool CECMatter::RenderName(CECViewport* pViewport)
{
	if (!g_pGame->GetConfigs()->GetVideoSettings().bGoodsName)
		return true;

	A3DFont* pFont = g_pGame->GetFont(RES_FONT_TITLE);
	if (!pFont || !m_pPateName)
	{
		ASSERT(0);
		return false;
	}

	A3DVIEWPORTPARAM* pViewPara = pViewport->GetA3DViewport()->GetParam();
	A3DFontMan* pFontMan = g_pGame->GetA3DEngine()->GetA3DFontMan();

	//	Calculate basic center position on screen
	A3DVECTOR3 vPos;
	if (m_pModel)
	{
		const A3DAABB& aabb = m_pModel->GetA3DSkinModel()->GetModelAABB();
		vPos = aabb.Center + g_vAxisY * (aabb.Extents.y * 1.3f);
	}
	else
		vPos = m_aabb.Center + g_vAxisY * (m_aabb.Extents.y * 1.3f);

	A3DVECTOR3 vScrPos, vTemp;
	pViewport->GetA3DViewport()->Transform(vPos, vScrPos);
	if (vScrPos.z < pViewPara->MinZ || vScrPos.z > pViewPara->MaxZ)
		return true;

	//	Decide matter name color
	DWORD dwNameCol;
	int iColIdx = g_pGame->GetItemNameColorIdx(m_MatterInfo.tid);
	if (iColIdx > 0)
	{
		dwNameCol = g_pGame->GetItemNameColor(m_MatterInfo.tid);
	}
	else
	{
		dwNameCol = A3DCOLORRGB(255, 255, 255);
		if (m_dwMatterType & ~MATTER_TYPEMASK)
		{
			if (m_dwMatterType & ITEMFLAG_MYTHICAL)
				dwNameCol = A3DCOLORRGB(170, 50, 255);
			else if (m_dwMatterType & ITEMFLAG_UNIQUE)
			//	dwNameCol = A3DCOLORRGB(206, 188, 134);
				dwNameCol = A3DCOLORRGB(255, 96, 0);
			else if (m_dwMatterType & ITEMFLAG_SUIT)
				dwNameCol = A3DCOLORRGB(108, 251, 75);
			else if (m_dwMatterType & ITEMFLAG_RARE)
				dwNameCol = A3DCOLORRGB(255, 220, 80);
			else if (m_dwMatterType & ITEMFLAG_EXTPROP)
				dwNameCol = A3DCOLORRGB(128, 128, 255);
		}
	}	
	
	AUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr();
	if (!pUIMan){
		ASSERT(0);
		return false;
	}
	float fScale = CECPateText::GetRenderScale();

	int x, y=(int)vScrPos.y, cx, cy;

	//	Draw name string
	m_pPateName->GetExtent(&cx, &cy);
	cx = int(cx * fScale);
	cy = int(cy * fScale);
	x = (int)(vScrPos.x - (cx >> 1));
	y -= (cy >> 1);
//	m_pPateName->Render(pViewport, x, y, dwNameCol, vScrPos.z);
	m_pPateName->RegisterRender(x, y, dwNameCol, vScrPos.z);

	return true;
}

bool CECMatter::TraceWithBrush(BrushTraceInfo * pInfo)
{
	if( !m_pModel || !m_pModel->HasCHAABB() )
		return false;

	// Expand the AABB of the model to handle the case that
	// convex hulls of the model protrude the AABB!
	A3DAABB aabb = m_pModel->GetCHAABB();
	aabb.Extents+=A3DVECTOR3(8.0f, 8.0f, 8.0f);
	aabb.Center += GetPos();
	aabb.CompleteMinsMaxs();

	A3DVECTOR3 vPoint, vNormal;
	float		fFraction;		//	Fraction
	
	if (pInfo->bIsPoint && !CLS_RayToAABB3(pInfo->vStart, pInfo->vDelta, aabb.Mins, aabb.Maxs, vPoint, &fFraction, vNormal ) ) {
		return false;
	}
	
	if (!pInfo->bIsPoint && !CLS_AABBAABBOverlap(aabb.Center, aabb.Extents, pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents) ) {
		return false;
	}
	
	//save original result
	bool		bStartSolid = pInfo->bStartSolid;	//	Collide something at start point
	bool		bAllSolid = pInfo->bAllSolid;		//	All in something
	int			iClipPlane = pInfo->iClipPlane;		//	Clip plane's index
	
	fFraction = 100.0f;
	vNormal = pInfo->ClipPlane.GetNormal(); //clip plane normal
	float       fDist = pInfo->ClipPlane.GetDist();	//clip plane dist

	// Detect the collision with Convex Hull
	bool bCollide = false;
	for(int i=0;i<m_nBrushes;i++)
	{
		if (m_ppBrushes[i]->Trace(pInfo) && (pInfo->fFraction < fFraction)) {
				//update the saving info
				bStartSolid = pInfo->bStartSolid;
				bAllSolid = pInfo->bAllSolid;
				iClipPlane = pInfo->iClipPlane;
				fFraction = pInfo->fFraction;
				vNormal = pInfo->ClipPlane.GetNormal();
				fDist = pInfo->ClipPlane.GetDist();

				bCollide = true;
		}
	}	
	if (bCollide) {
		pInfo->bStartSolid = bStartSolid;
		pInfo->bAllSolid = bAllSolid;
		pInfo->iClipPlane = iClipPlane;
		pInfo->fFraction = fFraction;
		pInfo->ClipPlane.SetNormal(vNormal);
		pInfo->ClipPlane.SetD(fDist);
	}

	return bCollide;
}

//	Load gfx from file
bool CECMatter::LoadGFXFromFile(const char* szFile, BYTE byScale)
{
	m_pGfx = g_pGame->GetGFXCaster()->GetGFXExMan()->LoadGfx(g_pGame->GetA3DDevice(), szFile, false);
	if (!m_pGfx)
		return false;

	float fScale = byScale * 16.0f / 256.0f;
	m_pGfx->SetScale(fScale);
	m_pGfx->Start();

	return true;
}



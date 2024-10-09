/*
 * FILE: EC_Pet.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/12/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Pet.h"
#include "EC_Game.h"
#include "EC_Model.h"
#include "EC_Utility.h"
#include "EC_PateText.h"
#include "EC_SceneLoader.h"
#include "EC_GameRun.h"
#include "EC_FixedMsg.h"
#include "EC_HostPlayer.h"
#include "EC_ManPlayer.h"
#include "EC_Player.h"
#include "EC_World.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "elementdataman.h"
#include "EC_NPCModel.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECMonster
//	
///////////////////////////////////////////////////////////////////////////

CECPet::CECPet(CECNPCMan* pNPCMan) : CECNPC(pNPCMan)
{
	m_iCID			= OCID_PET;
	m_pDBEssence	= NULL;
	m_fTouchRad		= 1.0f;
}

CECPet::~CECPet()
{
}

//	Initlaize object
bool CECPet::Init(int tid, const S2C::info_npc& Info)
{
	if (!CECNPC::Init(tid, Info))
		return false;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	const void* pDBData = pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	if (!pDBData)
	{
		ASSERT(pDBData);
		return false;
	}

	const char* szModelFile = NULL;

	if (DataType == DT_MONSTER_ESSENCE)
	{
		const MONSTER_ESSENCE* pMonsterData = (const MONSTER_ESSENCE*)pDBData;
		szModelFile = pMonsterData->file_model;

		const PET_EGG_ESSENCE* pEggData = (const PET_EGG_ESSENCE*)pDB->get_data_ptr(pMonsterData->id_pet_egg_captured, ID_SPACE_ESSENCE, DataType);
		if (!pEggData)
		{
			ASSERT(pEggData);
			return false;
		}

		m_pDBEssence = (PET_ESSENCE*)pDB->get_data_ptr(pEggData->id_pet, ID_SPACE_ESSENCE, DataType);
		m_NPCInfo.tid = pEggData->id_pet;
	}
	else
	{
		m_pDBEssence = (PET_ESSENCE*)pDBData;
		szModelFile = m_pDBEssence->file_model;
	}

	if (!m_pDBEssence)
	{
		ASSERT(m_pDBEssence);
		return false;
	}

	SetUseGroundNormal(m_pDBEssence->stand_mode == 0 ? true : false);
	m_fTouchRad	= m_pDBEssence->size;
	m_BasicProps.iLevel = 1;
	
	//	Submit EC model loading request to loading thread
	assert(m_dwBornStamp);
	QueueLoadNPCModel();

	float fExt = m_fTouchRad * 1.5f;
	m_cdr.vExts.Set(fExt, fExt, fExt);
	m_pNPCModelPolicy->SetDefaultPickAABBExt(m_cdr.vExts);

	//	If NPC doesn't have specific name, use the name in database
	if (!(Info.state & GP_STATE_NPC_NAME))
	{
		m_strName = m_pDBEssence->name;
		g_pGame->GetGameRun()->GetUIManager()->FilterBadWords(m_strName);
		//	Try to build complete name
		BuildCompleteName();
	}

	if (Info.state & GP_STATE_NPC_PET)
	{
		if (!IsPlantPet())
		{
			//	非植物宠时，能够从宠物面板和玩家面板中相互追踪
			CECPlayer* pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(m_idMaster);
			if(pPlayer)
			{
				pPlayer->SetCurPetID(Info.nid);
			}
		}
	}

	if (m_pDBEssence->combined_switch & PCS_FORBID_SELECTION){
		SetSelectable(false);
	}

	A3DVECTOR3 vPos = Info.pos;
	SetPos(vPos);
	SetDirAndUp(glb_DecompressDirH(Info.dir), g_vAxisY);

	StartWork(WT_NOTHING, WORK_STAND);

	return true;
}

//	Build complete name
void CECPet::BuildCompleteName()
{
	if (!m_pPateName) return;
	const ACHAR* szMasterName = g_pGame->GetGameRun()->GetPlayerName(m_idMaster, false);
	if (!szMasterName) return;

	ACString strMasterName = szMasterName;
	
	CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
	if (pWorld->IsCountryWarMap() &&
		!CECUIConfig::Instance().GetGameUI().bShowNameInCountryWar)
	{
		//	国战场景中，不直接显示宠物主人名称时，要求查询到此主人信息
		//	此改动（if语句中的内容）将导致主人未出现时无法看到宠物名称
		//	if 括号中的判断是必要的，将增加看到宠物名称的机会

		CECPlayerMan* pPlayerMan = pWorld->GetPlayerMan();
		CECPlayer* pPlayer = pPlayerMan->GetPlayer(m_idMaster);
		if (!pPlayer)
		{
			//	还没看到此玩家，无法判断其国家，还不能确定如何显示
			return;
		}
		if (pPlayer->GetShowNameInCountryWar())
			strMasterName = pPlayer->GetNameInCountryWar();
	}
	
	m_strCompName.Format(g_pGame->GetFixedMsgTab()->GetWideString(FIXMSG_WHOSETHING), strMasterName, m_strName);
	m_pPateName->SetText(m_strCompName, false);
}

//	Tick routine
bool CECPet::Tick(DWORD dwDeltaTime)
{
	CECNPC::Tick(dwDeltaTime);

	if (!m_strCompName.GetLength())
	{
		//	Try to build complete name
		BuildCompleteName();
	}

	return true;
}

//	Render routine
bool CECPet::Render(CECViewport* pViewport, int iRenderFlag/* 0 */)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost->IsInForceNavigateState())
		return true;

	CECNPC::Render(pViewport, iRenderFlag);
	
	return true;
}

//	Is this host player's pet
bool CECPet::IsHostPet()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	return GetMasterID() == pHost->GetCharacterID();
}

//	Is this pet attackable;
bool CECPet::CanBeAttacked()
{
	return IsCombatPet()
		|| IsSummonPet()
		|| IsPlantPet()
		|| IsEvolutionPet();
}

//	Get NPC name color
DWORD CECPet::GetNameColor()
{
	CECPlayerMan* pPlayerMan = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	CECPlayer* pPlayer = pPlayerMan->GetPlayer(m_idMaster);
	if (!pPlayer)
		return CECNPC::GetNameColor();

	return pPlayer->GetNameColor();
}

//	Is a follow pet ?
bool CECPet::IsFollowPet()const
{
	if (m_pDBEssence)
		return m_pDBEssence->id_type == 8783;
	
	return false;
}

//	Is a combat pet ?
bool CECPet::IsCombatPet()const
{
	if (m_pDBEssence)
		return m_pDBEssence->id_type == 8782;
	
	return false;
}

bool CECPet::IsSummonPet()const
{
	return m_pDBEssence
		&& m_pDBEssence->id_type == 28752;
}

bool CECPet::IsPlantPet()const
{
	return m_pDBEssence
		&& m_pDBEssence->id_type == 28913;
}
bool CECPet::IsEvolutionPet() const 
{
	return m_pDBEssence && m_pDBEssence->id_type == 37698;
}

bool CECPet::IsImmuneDisable()
{
	if( m_pDBEssence && (m_pDBEssence->combined_switch & PCS_HIDE_IMMUNE) )
		return true;
	
	return false;
}

float CECPet::GetTransparentLimit()
{
	if( m_pDBEssence && (m_pDBEssence->combined_switch & PCS_FORBID_SELECTION) )
		return -1.0f;
	
	return CECNPC::GetTransparentLimit();
}

bool CECPet::ShouldHideName()const{
	return m_pDBEssence != NULL && (m_pDBEssence->combined_switch & PCS_HIDE_NAME) != 0;
}
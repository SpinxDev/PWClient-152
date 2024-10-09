/*
 * FILE: EC_Monster.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Monster.h"
#include "EC_Game.h"
#include "EC_Model.h"
#include "EC_Utility.h"
#include "EC_PateText.h"
#include "EC_SceneLoader.h"
#include "EC_HostPlayer.h"
#include "EC_GameRun.h"
#include "elementdataman.h"
#include "EC_NPCModel.h"

#include <A3DMacros.h>

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

CECMonster::CECMonster(CECNPCMan* pNPCMan) : CECNPC(pNPCMan)
{
	m_iCID			= OCID_MONSTER;
	m_pDBEssence	= NULL;
}

CECMonster::~CECMonster()
{
}

//	Initlaize object
bool CECMonster::Init(int tid, const S2C::info_npc& Info)
{
	if (!CECNPC::Init(tid, Info))
		return false;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (MONSTER_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	SetUseGroundNormal(m_pDBEssence->stand_mode == 0 ? true : false);
	m_fTouchRad	= m_pDBEssence->size;
	m_BasicProps.iLevel = m_pDBEssence->level;

	//	Submit EC model loading request to loading thread
	QueueLoadNPCModel();

	float fExt = m_fTouchRad * 1.5f;
	m_cdr.vExts.Set(fExt, fExt, fExt);
	m_pNPCModelPolicy->SetDefaultPickAABBExt(m_cdr.vExts);

	//	If NPC doesn't have specific name, use the name in database
	if (!(Info.state & GP_STATE_NPC_NAME))
	{
		m_strName = m_pDBEssence->name;
		if (m_pPateName)
			m_pPateName->SetText(m_strName, false);
	}

	if( m_pDBEssence->combined_switch & MCS_FORBID_SELECTION )
		SetSelectable(false);

	A3DVECTOR3 vPos = Info.pos;
	SetPos(vPos);
	SetDirAndUp(glb_DecompressDirH(Info.dir), g_vAxisY);

	StartWork(WT_NOTHING, WORK_STAND);

	return true;
}

//	Get NPC name color
DWORD CECMonster::GetNameColor()
{
	DWORD dwNameCol = A3DCOLORRGB(255, 255, 0);
	if (IsInBattleInvaderCamp())
	{
		//	In invader camp in battle
		dwNameCol = NAMECOL_BC_RED;
	}
	else if (IsInBattleDefenderCamp())
	{
		//	In defender camp in battle
		dwNameCol = NAMECOL_BC_BLUE;
	}
	else
	{
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		dwNameCol = pHost->DecideTargetNameCol(m_BasicProps.iLevel);
	}

	return dwNameCol;
}

//	Is monster in invader camp in battle ?
bool CECMonster::IsInBattleInvaderCamp()
{ 
	return (m_pDBEssence->faction & (1 << 11)) ? true : false; 
}

//	Is monster in defender camp in battle ?
bool CECMonster::IsInBattleDefenderCamp()
{ 
	return (m_pDBEssence->faction & (1 << 12)) ? true : false; 
}

//	Get role in battle
int CECMonster::GetRoleInBattle()
{
	return m_pDBEssence->role_in_war;
}

//  Is immune text disable ?
bool CECMonster::IsImmuneDisable()
{
	if( m_pDBEssence && (m_pDBEssence->combined_switch & MCS_HIDE_IMMUNE) )
		return true;

	return false;
}

// NPC不可选中的原本逻辑是不可选中的NPC为半透明，
// 应策划要求，现改为如果是策划指定的特殊的不可选中怪，用不透明渲染alpha=1.0
// 其他情况则用原本的逻辑
float CECMonster::GetTransparentLimit()
{
	if( m_pDBEssence && (m_pDBEssence->combined_switch & MCS_FORBID_SELECTION) )
		return -1.0f;

	return CECNPC::GetTransparentLimit();
}
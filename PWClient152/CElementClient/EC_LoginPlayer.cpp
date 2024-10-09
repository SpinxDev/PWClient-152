/*
 * FILE: EC_LoginPlayer.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_LoginPlayer.h"
#include "EC_IvtrTypes.h"
#include "EC_Model.h"
#include "EC_Resource.h"
#include "EC_ShadowRender.h"
#include "EC_Face.h"
#include "EC_Viewport.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrFashion.h"
#include "A3DSkinMan.h"
#include "EC_Game.h"
#include "EC_Sprite.h"
#include "elementdataman.h"
#include "EC_Goblin.h"
#include "EC_IvtrGoblin.h"

#include "roleinfo"

#include <A3DCombinedAction.h>

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
//	Implement CECLoginPlayer
//	
///////////////////////////////////////////////////////////////////////////

CECLoginPlayer::CECLoginPlayer(CECPlayerMan* pPlayerMan) : CECPlayer(pPlayerMan)
{
	m_iCID			= OCID_LOGINPLAYER;
	m_bCastShadow	= true;
	
	memset(m_aEquipItems, 0, sizeof (m_aEquipItems));
}

CECLoginPlayer::~CECLoginPlayer()
{
}

void CECLoginPlayer::Release()
{
	//	Release equipment items
	for (int i=0; i < IVTRSIZE_EQUIPPACK; i++)
	{
		if (m_aEquipItems[i])
		{
			delete m_aEquipItems[i];
			m_aEquipItems[i] = NULL;
		}
	}

	CECPlayer::Release();
}

//	Load player data
bool CECLoginPlayer::Load(GNET::RoleInfo& Info)
{
	using namespace GNET;
	
	m_PlayerInfo.cid		= Info.roleid;
	m_BasicProps.iLevel		= Info.level;
	m_BasicProps.iLevel2	= Info.level2;

	m_iGender		= Info.gender;
	m_iProfession	= Info.occupation;

	//	Get player name
	m_strName = ACString((const ACHAR*)Info.name.begin(), Info.name.size() / sizeof (ACHAR));

	if (Info.custom_data.size() >= 4)
		ChangeCustomizeData(PLAYER_CUSTOMIZEDATA::From(Info.custom_data.begin(), Info.custom_data.size()), false);

	int i;

	//	Create equipments
	for (i=0; i < (int)Info.equipment.size(); i++)
	{
		const GRoleInventory& Equip = Info.equipment[i];
		CECIvtrItem* pItem = CECIvtrItem::CreateItem(Equip.id, Equip.expire_date, Equip.count);
		if (!pItem)
		{
			a_LogOutput(1, "CECLoginPlayer::Load, Failed to create equipment.");
			continue;
		}

		if( Equip.data.begin() && Equip.data.size() )
			pItem->SetItemInfo((BYTE*)Equip.data.begin(), Equip.data.size());

		pItem->SetProcType(Equip.proctype);

		m_aEquipItems[Equip.pos] = pItem;
	}

	//  Create goblin
	if( m_aEquipItems[EQUIPIVTR_GOBLIN] )
	{
		m_pGoblin = new CECGoblin();
		CECIvtrGoblin* pIvtrGoblin = (CECIvtrGoblin*)m_aEquipItems[EQUIPIVTR_GOBLIN];
		m_pGoblin->Init(pIvtrGoblin->GetTemplateID(), pIvtrGoblin, this);
	}

	//	Build new equipments id array
	int aNewEquips[IVTRSIZE_EQUIPPACK];

	for (i=0; i < IVTRSIZE_EQUIPPACK; i++)
	{
		CECIvtrItem* pItem = m_aEquipItems[i];
		if (pItem)
		{
			aNewEquips[i] = pItem->GetTemplateID();
			if( ((i >= EQUIPIVTR_FASHION_BODY && i <= EQUIPIVTR_FASHION_WRIST) || i == EQUIPIVTR_FASHION_HEAD ) && 
				pItem->GetClassID() == CECIvtrItem::ICID_FASHION )
			{
				CECIvtrFashion* pFashionItem = (CECIvtrFashion*)pItem;
				aNewEquips[i] |= (pFashionItem->GetWordColor() << 16) & 0x7fffffff;
			}
			else
			{
				CECIvtrEquip * pEquip = (CECIvtrEquip *) pItem;
				WORD stoneStatus = pEquip ? pEquip->GetStoneMask() : 0;
				aNewEquips[i] |= (stoneStatus << 16) & 0x7fffffff;
			}
		}
		else
		{
			aNewEquips[i] = 0;
		}
	}

	//	Parse modes
	int* aModes = (int*)Info.charactermode.begin();
	int iNumMode = Info.charactermode.size() / 8;
	if (aModes && iNumMode)
	{
		for (i=0; i < iNumMode; i++)
		{
			int iKey = *aModes++;
			int iValue = *aModes++;

			switch (iKey)
			{
			case 1:	m_bFashionMode = iValue ? true : false;		break;
			default:
				ASSERT(0);
				break;
			}
		}
	}
	
	// prepared equip info
	memcpy(m_aEquips, aNewEquips, sizeof(m_aEquips));

	//	Try to load player's skeleton
	if (!LoadPlayerSkeleton(true))
	{
		a_LogOutput(1, "CECLoginPlayer::Load, Failed to load skeleton.");
		return false;
	}

	//	Parse custom status
	short* aStatus = (short*)Info.custom_status.begin();
	int iNumStatus = Info.custom_status.size() / sizeof (short);
	if (aStatus && iNumStatus)
	{
		for (i=0; i < iNumStatus; i++)
			ApplyEffect(aStatus[i], true);
	}

	return true;
}

//	Tick routine
bool CECLoginPlayer::Tick(DWORD dwDeltaTime)
{
	CECPlayer::Tick(dwDeltaTime);

	if (m_pPlayerModel)
		m_pPlayerModel->Tick(dwDeltaTime);

	if (m_pFaceModel)
		m_pFaceModel->Tick(dwDeltaTime);

	return true;
}

//	Render routine
bool CECLoginPlayer::Render(CECViewport* pViewport, int iRenderFlag/* 0 */)
{
	m_PateContent.iVisible = 0;
	
	if( !UpdateCurSkins() )
		return true;

	if (iRenderFlag == RD_REFLECT)
		return RenderForReflect(pViewport);

	if (m_pPlayerModel)
	{
		m_pPlayerModel->Render(pViewport->GetA3DViewport());
		
		if (m_bCastShadow && g_pGame->GetShadowRender())
		{
			CECPlayer * pPlayer = this;
			A3DAABB shadowAABB = GetShadowAABB();
			g_pGame->GetShadowRender()->AddShadower(shadowAABB.Center, shadowAABB, SHADOW_RECEIVER_ALL, PlayerRenderForShadow, pPlayer);
		}
	}

	if (m_pFaceModel)
	{
		m_pFaceModel->Render(pViewport, false, false);
	}

	RenderGoblinOrSprite(pViewport);

	return true;
}

//	Render for refelction
bool CECLoginPlayer::RenderForReflect(CECViewport* pViewport)
{
	return true;
}

void CECLoginPlayer::Stand(bool bRestart)
{
	PlayAction(CECPlayer::ACT_STAND, bRestart);
}

void CECLoginPlayer::StandNow()
{
	PlayAction(CECPlayer::ACT_STAND, true, 0);
}

void CECLoginPlayer::StandForCustomize()
{
	PlayAction(CECPlayer::ACT_CUSTOMIZE);
}

void CECLoginPlayer::Run()
{
	PlayAction(CECPlayer::ACT_RUN);
}

void CECLoginPlayer::StandUp()
{
	PlayAction(CECPlayer::ACT_STANDUP);
}

void CECLoginPlayer::SitDown()
{
	PlayAction(CECPlayer::ACT_SITDOWN);
}

bool CECLoginPlayer::TestShowDisplayAction(int displayActionWeapon)
{
	//	ÎäÆ÷Æ¥ÅäÊ±£¬²Å²¥·ÅÑÝÎä¶¯×÷
	if (GetProfession() == PROF_ARCHOR){
		return false;
	}
	int curWeaponType = GetShowingWeaponType();
	int displayWeaponType = 10;	//	¿ÕÊÖ
	if (displayWeaponType != 0){
		elementdataman* pDB = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		const void *pData = pDB->get_data_ptr(displayActionWeapon, ID_SPACE_ESSENCE, DataType);
		if (pData && DataType == DT_WEAPON_ESSENCE){
			const WEAPON_ESSENCE *pWeaponEssence = (const WEAPON_ESSENCE *)(pData);
			pData = pDB->get_data_ptr(pWeaponEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);
			if (pData){
				const WEAPON_SUB_TYPE * pWeaponSubType = (const WEAPON_SUB_TYPE *)(pData);
				displayWeaponType = pWeaponSubType->action_type;
			}
		}
	}
	return curWeaponType == displayWeaponType;
}

void CECLoginPlayer::ShowDisplayAction()
{
	if (!m_pPlayerModel){
		return;
	}
	if (GetProfession() == PROF_JIANLING){
		//	½£ÁéÑÝÎä¶¯×÷¡¢ÐèÒªÎäÆ÷·ÅÔÚÊÖÉÏ
		SetWeaponHangerPos(CECPlayer::WEAPON_HANGER_HAND);
	}
	if (PlayNonSkillActionWithName(ACT_MAX, "µÇÂ½ÑÝÎä")){
		return;
	}
	Stand();
}

void CECLoginPlayer::StopDisplayAction()
{
	if (!m_pPlayerModel){
		return;
	}
	A3DCombActDynData *pComAct = m_pPlayerModel->GetCurComAct();
	if (pComAct && !stricmp(pComAct->GetComAct()->GetName(), "µÇÂ½ÑÝÎä")){
		Stand(true);
	}
}

void CECLoginPlayer::MoveFront()
{
	PlayAction(CECPlayer::ACT_WALK);
}

void CECLoginPlayer::GoBack()
{
	PlayAction(CECPlayer::ACT_WALK);
}
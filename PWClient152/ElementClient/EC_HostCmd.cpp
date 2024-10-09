/*
 * FILE: EC_HostPlayer.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/1
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Utility.h"
#include "EC_World.h"
#include "EC_GameSession.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrTypes.h"
#include "EC_ElsePlayer.h"
#include "EC_Shortcut.h"
#include "EC_InputCtrl.h"
#include "EC_Inventory.h"
#include "EC_Matter.h"
#include "EC_ManMatter.h"
#include "EC_ManPlayer.h"
#include "EC_Team.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "EC_FixedMsg.h"
#include "EC_IvtrDecoration.h"

#include "EC_HPWorkStand.h"
#include "EC_HPWorkSit.h"
#include "EC_HPWorkTrace.h"
#include "EC_HPWorkFly.h"
#include "EC_HPWorkFall.h"
#include "defence/stackchecker.h"

#include "A3DMacros.h"
#include "DlgStorage.h"
#include "EC_ActionSwitcher.h"

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
//	Implement CECHostPlayer
//	
///////////////////////////////////////////////////////////////////////////

//	Sit down / Stand up
bool CECHostPlayer::CmdSitDown(bool bSitDown)
{
	if (!CanDo(CANDO_SITDOWN))
		return false;

	if (bSitDown)
		g_pGame->GetGameSession()->c2s_CmdSitDown();
	else
		g_pGame->GetGameSession()->c2s_CmdStandUp();

	return true;
}

//	Switch walk and run state
bool CECHostPlayer::CmdWalkRun(bool bRun)
{
	m_bWalkRun = bRun;
	return true;
}

//	Start normal attacking to selected target
bool CECHostPlayer::CmdNormalAttack(bool bMoreClose/* false */, bool bCombo/* false */,
							int idTarget/* 0 */, int iForceAtk/* -1 */)
{
	StackChecker::ACTrace(2);

	// first of all see if we need to cancel sitdown work.
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return false;
	}

	if (!CanDo(CANDO_MELEE))
		return false;

	if (InSlidingState())
		return false;

	if (!bCombo)
		ClearComboSkill();

	if (!idTarget)
		idTarget = m_idSelTarget;

	bool bForceAttack;
	if (iForceAtk < 0)
		bForceAttack = glb_GetForceAttackFlag(NULL);
	else
		bForceAttack = iForceAtk > 0 ? true : false;

	if (AttackableJudge(idTarget, bForceAttack) != 1)
		return false;

	return NormalAttackObject(idTarget, bForceAttack, bMoreClose);
}

//	Find a near target
bool CECHostPlayer::CmdFindTarget()
{
	return true;
}

//	Select other player's attacked target
bool CECHostPlayer::CmdAssistAttack()
{
	// first of all see if we need to cancel sitdown work.
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return false;
	}

	if (CanDo(CANDO_ASSISTSEL))
		g_pGame->GetGameSession()->c2s_CmdTeamAssistSel(m_idSelTarget);

	return true;
}

//	Start / Stop flying
bool CECHostPlayer::CmdFly(bool bForceFly)
{
	if (m_pActionSwitcher)
		m_pActionSwitcher->PostMessge(CECActionSwitcherBase::MSG_FLY);

	// first of all see if we need to cancel sitdown work.
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return false;
	}

	if (!CanDo(CANDO_FLY))
		return false;

	CECIvtrItem* pItem = m_pEquipPack->GetItem(EQUIPIVTR_FLYSWORD);
	if (!pItem)
		return false;
	
	if(((CECIvtrEquip*)pItem)->IsDestroying())
		return false;

	if (!IsFlying())
	{
		//	TODO: Maybe we should let server tell us whether we can fly or not
		bool bCanFly = true;

		if (m_iMoveEnv == MOVEENV_AIR)
			bCanFly = false;
		else if (m_iMoveEnv == MOVEENV_WATER && !CanTakeOffWater())
			bCanFly = false;

		if (!bCanFly)
			return false;
	}

	g_pGame->GetGameSession()->c2s_CmdUseItem(IVTRTYPE_EQUIPPACK, EQUIPIVTR_FLYSWORD, pItem->GetTemplateID(), 1);
	return true;
}

bool CECHostPlayer::CmdPickup()
{
	// first of all see if we need to cancel sitdown work.
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return false;
	}

	if (!CanDo(CANDO_PICKUP))
		return false;

	//	If host is going to pickup an item already, return directly
	if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_TRACEOBJECT))
	{
		if (dynamic_cast<CECHPWorkTrace*>(pWork)->GetTraceReason() == CECHPWorkTrace::TRACE_PICKUP)
			return true;
	}

	//	Find a matter which is near enough to us
	CECMatterMan* pMatterMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
	CECMatter* pMatter = pMatterMan->FindMatterNearHost(10.0f, true);
	if (pMatter)
		PickupObject(pMatter->GetMatterID(), false);

	return true;
}

bool CECHostPlayer::CmdGather()
{
	// first of all see if we need to cancel sitdown work.
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return false;
	}

	if (!CanDo(CANDO_GATHER))
		return false;

	//	If host is going to pickup an item already, return directly
	if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_TRACEOBJECT))
	{
		if (dynamic_cast<CECHPWorkTrace*>(pWork)->GetTraceReason() == CECHPWorkTrace::TRACE_GATHER)
			return true;
	}

	PickupObject(m_idSelTarget, true);

	return true;
}

bool CECHostPlayer::CmdRushFly()
{
	if (m_bAboutToDie || IsDead() || !IsFlying())
		return false;

	g_pGame->GetGameSession()->c2s_CmdActiveRushFly(!m_bRushFly);
	
	return true;
}

bool CECHostPlayer::CmdBindBuddy(int idTarget)
{
	if (!m_BindCmdCoolCnt.IsFull())
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
		return false;
	}
	
	if (!CanDo(CANDO_BINDBUDDY) || !ISPLAYERID(idTarget) || 
		idTarget == GetCharacterID())
		return false;

	CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(idTarget);
	if (!pPlayer || pPlayer->GetGender() == GetGender())
		return false;

	A3DVECTOR3 vDist = pPlayer->GetServerPos() - GetPos();
	if (vDist.Magnitude() >= 2.8f)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TARGETISFAR);
		return false;
	}

	if (GetGender() == GENDER_MALE)
		g_pGame->GetGameSession()->c2s_CmdBindPlayerInvite(idTarget);
	else
		g_pGame->GetGameSession()->c2s_CmdBindPlayerRequest(idTarget);

	//	Start cool time
	m_BindCmdCoolCnt.Reset();

	return true;
}

bool CECHostPlayer::CmdViewOtherEquips(int idTarget)
{
	if (GetCoolTime(GP_CT_VIEWOTHEREQUIP))
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
		return false;
	}

	if (!ISPLAYERID(idTarget) || idTarget == GetCharacterID())
		return false;

	CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(idTarget);
	if (!pPlayer)
		return false;

	A3DVECTOR3 vDist = pPlayer->GetServerPos() - GetPos();
	if (vDist.Magnitude() >= 50.0f)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TARGETISFAR);
		return false;
	}

	g_pGame->GetGameSession()->c2s_CmdGetOtherEquipDetail(idTarget);

	return true;
}

bool CECHostPlayer::CmdViewOtherInfo(int idTarget)
{
	if (GetCoolTime(GP_CT_QUERY_OTHER_PROPERTY))
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
		return false;
	}

	if (!ISPLAYERID(idTarget) || idTarget == GetCharacterID())
		return false;

	CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(idTarget);
	if (!pPlayer)
		return false;

	A3DVECTOR3 vDist = pPlayer->GetServerPos() - GetPos();
	if (vDist.Magnitude() >= 50.0f)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TARGETISFAR);
		return false;
	}
	
	// 查找装备中是否有丛林之眼的特殊属性
	CECInventory *pEquipPack = GetPack(IVTRTYPE_EQUIPPACK);
	for (int i(0); i<pEquipPack->GetSize(); ++ i)
	{
		CECIvtrItem *pItem = pEquipPack->GetItem(i);
		if (pItem && pItem->GetClassID() == CECIvtrItem::ICID_DECORATION)
		{
			CECIvtrDecoration *pDecoration = dynamic_cast<CECIvtrDecoration *>(pItem);
			if (pDecoration && pDecoration->HasViewProp())
			{
				return UseEquipment(i);
			}
		}
	}

	// 查找包裹中是否有洞察之眼
	int iIndex(0), nSize(m_pPack->GetSize());
	for (iIndex=0; iIndex<nSize; ++iIndex)
	{
		CECIvtrItem *pItem = m_pPack->GetItem(iIndex);
		if (pItem && pItem->GetClassID() == CECIvtrItem::ICID_LOOKINFOITEM)
			break;
	}
	if (iIndex >= nSize)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_NEEDTOOL);
		return false;
	}

	UseItemInPack(IVTRTYPE_PACK, iIndex);

	return true;
}
bool CECHostPlayer::CmdAskDuel(int idTarget)
{
	if (!CanDo(CANDO_DUEL))
	{
		if (IsFighting())
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_DUEL_INFIGHT);

		return false;
	}

	if (!ISPLAYERID(idTarget) || idTarget == GetCharacterID())
		return false;

	CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(idTarget);
	if (!pPlayer)
		return false;

	A3DVECTOR3 vDist = pPlayer->GetServerPos() - GetPos();
	if (vDist.Magnitude() >= 16.0f)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TARGETISFAR);
		return false;
	}
	
	g_pGame->GetGameSession()->c2s_CmdDuelRequest(idTarget);
	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_DUEL_ASKSENT);

	return true;
}

//	Invite selected player to join team
bool CECHostPlayer::CmdInviteToTeam()
{
	if (IsDead() || !ISPLAYERID(m_idSelTarget) || m_idSelTarget == GetCharacterID())
		return false;

	g_pGame->GetGameSession()->c2s_CmdTeamInvite(m_idSelTarget);
	return true;
}

//	Leave current team
bool CECHostPlayer::CmdLeaveTeam()
{
	if (IsDead() || !m_pTeam)
		return false;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	const ACHAR* szMsg = pGameUI->GetStringFromTable(235);
	pGameUI->MessageBox("Game_TeamDisband", szMsg, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
//	g_pGame->GetGameSession()->c2s_CmdTeamLeaveParty();
	return true;
}

//	Kick one member of team
bool CECHostPlayer::CmdKickTeamMember()
{
	return true;
}

//	Search for a team
bool CECHostPlayer::CmdFindTeam()
{
	return true;
}

//	Start trade with other selected player
bool CECHostPlayer::CmdStartTrade()
{
	// first of all see if we need to cancel sitdown work.
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return false;
	}

	if (!CanDo(CANDO_TRADE))
		return false;

	if (!ISPLAYERID(m_idSelTarget) || m_idSelTarget == m_PlayerInfo.cid)
		return false;

	g_pGame->GetGameSession()->trade_Start(m_idSelTarget);
	return true;
}

//	Open booth for selling items
bool CECHostPlayer::CmdSellBooth()
{
	if (IsInvisible())
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CANNOT_USE_WHEN_INVISIBLE);
		return false;
	}

	// first of all see if we need to cancel sitdown work.
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return false;
	}

	//	Stop following someone before selling items
	if (IsFollowing())
		m_pWorkMan->FinishAllWork(true);

	if (!CanDo(CANDO_BOOTH))
		return false;

	//	Ask server whether we can open booth
	g_pGame->GetGameSession()->c2s_CmdOpenBoothTest();
	return true;
}

//	Open booth for buying items
bool CECHostPlayer::CmdBuyBooth()
{
	return true;
}

//	Play a pose
bool CECHostPlayer::CmdStartPose(int iPose)
{
	// first of all see if we need to cancel sitdown work.
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return false;
	}

	if (!CanDo(CANDO_PLAYPOSE))
		return false;

	if (!m_pWorkMan->IsStanding())
		return false;

	if (iPose == ROLEEXP_SITDOWN)
		g_pGame->GetGameSession()->c2s_CmdSessionEmote(iPose);
	else if (iPose == ROLEEXP_KISS)
	{
		if (ISPLAYERID(m_idSelTarget))
			g_pGame->GetGameSession()->c2s_CmdConEmoteRequest(ROLEEXP_KISS, m_idSelTarget);
	}
	else
		g_pGame->GetGameSession()->c2s_CmdEmoteAction(iPose);

	return true;
}

//	Invite selected player to join faction
bool CECHostPlayer::CmdInviteToFaction()
{
	return true;
}
/*
 * FILE: EC_Team.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "gnet.h"
#include "EC_Team.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_World.h"
#include "EC_Utility.h"
#include "EC_FixedMsg.h"
#include "EC_HostPlayer.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"

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
//	Implement CECTeamMember
//	
///////////////////////////////////////////////////////////////////////////

CECTeamMember::CECTeamMember(CECTeam* pTeam, int cid) : m_vPos(INVALIDPOS)
{
	m_bInfoReady	= false;
	m_pTeam			= pTeam;
	m_cid			= cid;
	m_iLevel		= 0;
	m_iCurHP		= 0;
	m_iCurMP		= 0;
	m_iMaxHP		= 0;
	m_iMaxMP		= 0;
	m_bFight		= false;
	m_idInst		= 0;
	m_bSameInstance = false;
	m_iProfitLevel = -1;
	m_iReincarnationCount = 0;

	m_iProfession	= PROF_WARRIOR;
	m_iGender		= GENDER_MALE;
}

//	Get member name
const ACHAR* CECTeamMember::GetName()
{
	return g_pGame->GetGameRun()->GetPlayerName(m_cid, true);
}

bool CECTeamMember::IsSameInstanceTeamMember()
{
	//	判断是否是 HostPlayer 同副本队友（有机会相互看到、或通过图标指示出队长大概位置）
	CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
	return POSISVALID(GetPos().x)
		&& GetInstanceID() == pWorld->GetInstanceID()
		&& GetCharacterID() != pWorld->GetHostPlayer()->GetCharacterID()
		&& GetSameInstance();
}

//	Reset icon states
void CECTeamMember::ResetIconStates(const S2C::IconStates &s)
{
	m_aIconStates = s;
	if (m_aIconStates.size() > 1)
		qsort(m_aIconStates.begin(), m_aIconStates.size(), sizeof (m_aIconStates[0]), glb_IconStateCompare);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECTeam
//	
///////////////////////////////////////////////////////////////////////////

CECTeam::CECTeam(int idLeader)
{
	m_idLeader	= idLeader;
	m_wPickFlag	= 0;
}

CECTeam::~CECTeam()
{
	//	Release all members	
	for (int i=0; i < m_aMembers.GetSize(); i++)
		delete m_aMembers[i];
	m_aMembers.RemoveAll();
}

//	Get member by player ID
CECTeamMember* CECTeam::GetMemberByID(int id, int* piIndex/* NULL */)
{
	for (int i=0; i < m_aMembers.GetSize(); i++)
	{
		CECTeamMember* pMember = m_aMembers[i];
		if (pMember->GetCharacterID() == id)
		{
			if (piIndex)
				*piIndex = i;

			return pMember;
		}
	}

	return NULL;
}

//	Update team data
void CECTeam::UpdateTeamData(const S2C::cmd_team_member_data& Data)
{
	using namespace S2C;

	int idHost = g_pGame->GetGameSession()->GetCharacterID();
	int i, iIndex;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (Data.member_count == Data.data_count)
	{
		//	Reset all members' information
		APtrArray<CECTeamMember*> aNewMembers;
		aNewMembers.SetSize(Data.data_count, 10);

		for (i=0; i < aNewMembers.GetSize(); i++)
			aNewMembers[i] = NULL;

		for (i=0; i < Data.data_count; i++)
		{
			const cmd_team_member_data::MEMBER& Member = Data.data[i];
			CECTeamMember* pMember = GetMemberByID(Member.idMember, &iIndex);
			if (!pMember)
			{
				//	Create new member
				if (!(pMember = new CECTeamMember(this, Member.idMember)))
					continue;
			}
			
			if (pMember->GetCharacterID() != g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID()
				&& pMember->GetProfitLevel() != Member.profit_level)
			{
				if (Member.profit_level == PROFIT_LEVEL_NONE)
				{
					ACString str;
					str.Format(pGameUI->GetStringFromTable(9667), pMember->GetName());
					pGameUI->AddChatMessage(str, GP_CHAT_SYSTEM);
				}
				else if (Member.profit_level == PROFIT_LEVEL_NORMAL)
				{
				}
			}

			aNewMembers[i] = pMember;

			pMember->SetCurHP(Member.hp);
			pMember->SetCurMP(Member.mp);
			pMember->SetLevel(Member.level);
			pMember->SetLevel2(Member.level2);
			pMember->SetWallowLevel(Member.wallow_level);
			pMember->SetMaxHP(Member.max_hp);
			pMember->SetMaxMP(Member.max_mp);
			pMember->SetFightFlag(Member.state ? true : false);
			pMember->SetForce(Member.force_id);
			pMember->SetProfitLevel(Member.profit_level);
			pMember->SetReincarnationCount(Member.reincarnation_times);
		}

		//	Reset player array
		for (i=0; i < m_aMembers.GetSize(); i++)
		{
			if (aNewMembers.Find(m_aMembers[i]) < 0)
				delete m_aMembers[i];
		}

		m_aMembers.RemoveAll(false);
		
		for (i=0; i < aNewMembers.GetSize(); i++)
			m_aMembers.Add(aNewMembers[i]);
	}
	else
	{
		for (i=0; i < Data.data_count; i++)
		{
			const cmd_team_member_data::MEMBER& Member = Data.data[i];
			CECTeamMember* pMember = GetMemberByID(Member.idMember, &iIndex);
			if (!pMember)
			{
				//	Create new member
				if (!(pMember = new CECTeamMember(this, Member.idMember)))
					continue;

				m_aMembers.Add(pMember);
			}
			if (pMember->GetProfitLevel() != Member.profit_level)
			{
				ACString str;
				if (Member.profit_level == PROFIT_LEVEL_NONE)
					str.Format(pGameUI->GetStringFromTable(9667), pMember->GetName());
				else if (Member.profit_level == PROFIT_LEVEL_YELLOW)
					str.Format(pGameUI->GetStringFromTable(9673), pMember->GetName());				

				if (!str.IsEmpty()) pGameUI->AddChatMessage(str, GP_CHAT_SYSTEM);
			}
			pMember->SetCurHP(Member.hp);
			pMember->SetCurMP(Member.mp);
			pMember->SetLevel(Member.level);
			pMember->SetLevel2(Member.level2);
			pMember->SetWallowLevel(Member.wallow_level);
			pMember->SetMaxHP(Member.max_hp);
			pMember->SetMaxMP(Member.max_mp);
			pMember->SetFightFlag(Member.state ? true : false);
			pMember->SetForce(Member.force_id);
			pMember->SetProfitLevel(Member.profit_level);
		}
	}
}

//	Add a new member
void CECTeam::AddMember(int idMember)
{
	if (GetMemberByID(idMember))
	{
		//	Team member has been exist
		ASSERT(0);
		return;
	}

	CECTeamMember* pMember = new CECTeamMember(this, idMember);
	if (!pMember)
		return;

	m_aMembers.Add(pMember);
}

//	Remove member
void CECTeam::RemoveMember(int idMember)
{
	int iIndex;
	CECTeamMember* pMember = GetMemberByID(idMember, &iIndex);
	if (pMember)
	{
		delete pMember;
		m_aMembers.RemoveAt(iIndex);
	}
}

//	Update team member information
void CECTeam::UpdateMemberInfo(GNET::GetPlayerBriefInfo_Re& p)
{
	CECGameRun* pGameRun = g_pGame->GetGameRun();

	//	Update host's team member information
	for (int i=0; i < (int)p.playerlist.size(); i++)
	{
		const PlayerBriefInfo& Info = p.playerlist[i];
		CECTeamMember* pMember = GetMemberByID(Info.roleid);
		if (!pMember)
			continue;
		
		pMember->SetProfession(Info.occupation);
		pMember->SetGender(Info.gender);
		pMember->SetInfoReadyFlag(true);
	
		//	Name-id pair got here always be right, so we force to
		//	overwrite old pair
		ACString strName((const ACHAR*)Info.name.begin(), Info.name.size() / sizeof (ACHAR));
		pGameRun->AddPlayerName(Info.roleid, strName, true);
			
		//	Remove player from unknown ID list
		for (int j=0; j < m_aUnknownIDs.GetSize(); j++)
		{
			if (m_aUnknownIDs[j] == Info.roleid)
			{
				m_aUnknownIDs.RemoveAtQuickly(j);
				pGameRun->AddFixedMessage(FIXMSG_JOINTEAM, strName);
				break;
			}
		}
	}
}

//	Get team leader
CECTeamMember* CECTeam::GetLeader()
{
	if (!m_aMembers.GetSize())
		return NULL;

	CECTeamMember* pMember = m_aMembers[0];
	if (pMember && m_idLeader == pMember->GetCharacterID())
		return pMember;
	else
		return NULL;
}

void CECTeam::ChangeTeamLeader(int idLeader)
{ 
	m_idLeader = idLeader; 
	CECTeamMember* pLeader = GetMemberByID(idLeader);
	if (pLeader) {
		APtrArray<CECTeamMember*> aNewMembers;
		aNewMembers.Add(pLeader);
		for (int i=0; i < m_aMembers.GetSize(); i++){
			CECTeamMember* pMember = m_aMembers[i];
			if (pMember && pMember->GetCharacterID() != idLeader) {
				aNewMembers.Add(pMember);
			}
		}
		m_aMembers.RemoveAll();
		for (i=0; i < aNewMembers.GetSize(); i++)
			m_aMembers.Add(aNewMembers[i]);
	}
}

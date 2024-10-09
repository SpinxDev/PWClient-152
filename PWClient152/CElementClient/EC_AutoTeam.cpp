// Filename	: EC_AutoTeam.cpp
// Creator	: Shizhenhua
// Date		: 2013/8/13

#include "EC_AutoTeam.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Configs.h"
#include "EC_GameSession.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EC_UIConfigs.h"
#include <A3DFuncs.h>

#include "DlgActivity.h"

#include "autoteamsetgoal_re.hpp"
#include "autoteamplayerleave.hpp"

#define new A_DEBUG_NEW


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define TASK_TEAM_DISTANCE 300.0f	// 超出300米取消任务组队


///////////////////////////////////////////////////////////////////////////
//  
//  Implement the class CECAutoTeam
//  
///////////////////////////////////////////////////////////////////////////

CECAutoTeam::CECAutoTeam()
{
	m_iType = TYPE_UNKNOWN;
	m_iCurGoal = 0;
	m_vTaskPos.Clear();
	m_cntCancel.SetPeriod(500);
	m_cntCancel.Reset();
}

void CECAutoTeam::Tick(DWORD dwDeltaTime)
{
	if( m_iType == TYPE_TASK && m_iCurGoal && m_cntCancel.IncCounter(dwDeltaTime) )
	{
		A3DVECTOR3 vPos = GetHostPlayer()->GetPos();
		if( a3d_MagnitudeH(m_vTaskPos - vPos) > TASK_TEAM_DISTANCE )
		{
			m_vTaskPos = vPos;
			Cancel(TYPE_TASK);
		}

		m_cntCancel.Reset();
	}
}

bool CECAutoTeam::DoAutoTeam(int type, int iGoalID)
{
	if( GetHostPlayer()->GetTeam() )
		return false;

	if( type == TYPE_TASK && !g_pGame->GetConfigs()->GetGameSettings().bAutoTeamForTask )
		return false;

	// 如果正在进行活动组队请求，则忽略其他请求
	if( m_iType == TYPE_ACTIVITY && m_iCurGoal )
		return false;

	// 记录此时的位置
	if( type == TYPE_TASK )
		m_vTaskPos = GetHostPlayer()->GetPos();

	if( m_iType == type && m_iCurGoal == iGoalID )
		return false;

	GetGameSession()->c2s_CmdAutoTeamSetGoal(type, iGoalID, 1);
	return true;
}

void CECAutoTeam::Cancel(int type)
{
	if( (m_iType == type) && m_iCurGoal )
		GetGameSession()->c2s_CmdAutoTeamSetGoal(type, m_iCurGoal, 0);
}

void CECAutoTeam::OnPrtcAutoTeamSetGoalRe(GNET::Protocol* p)
{
	enum
	{
		ERR_INVALID_GOAL = -1,
		ERR_INVALID_ROLE_STATUS = -2,
	};
	
	using namespace GNET;
	AutoTeamSetGoal_Re* pProto = (AutoTeamSetGoal_Re*)p;
	if( pProto->retcode == 0 )
	{
		if( pProto->op == 1 )
		{
			m_iType = (int)pProto->goal_type;
			m_iCurGoal = pProto->goal_id;

			if( pProto->goal_type == TYPE_ACTIVITY )
				GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(10864), GP_CHAT_MISC);

			// 更新界面
			CDlgActivity* pDlg = dynamic_cast<CDlgActivity*>(GetGameUIMan()->GetDialog("Win_Activity"));
			if( pDlg ) pDlg->UpdateActivityList(true);
		}
	}
	else
	{
		if( pProto->goal_type == TYPE_ACTIVITY )
		{
			if( pProto->op == 1 && pProto->retcode == ERR_INVALID_GOAL )
				GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10860), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			else if( pProto->op == 0 && pProto->retcode == ERR_INVALID_ROLE_STATUS )
				GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10861), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}
}

void CECAutoTeam::OnPrtcPlayerLeave(GNET::Protocol* p)
{
	enum
	{
		REASON_COMPOSE_TEAM = 0,
		REASON_INVALID_STATUS,
		REASON_PLAYER_OP,
		REASON_JOIN_TEAM,
		REASON_TIMEOUT,
	};
	
	using namespace GNET;
	AutoTeamPlayerLeave* pProto = (AutoTeamPlayerLeave*)p;
	if( m_iType == TYPE_ACTIVITY )
	{
		switch(pProto->reason)
		{
		case REASON_INVALID_STATUS:
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10862), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			break;
		case REASON_TIMEOUT:
			GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(10863), GP_CHAT_MISC);
			break;
		}
	}

	m_iCurGoal = 0;
	m_iType = TYPE_UNKNOWN;

	// 更新界面
	CDlgActivity* pDlg = dynamic_cast<CDlgActivity*>(GetGameUIMan()->GetDialog("Win_Activity"));
	if( pDlg ) pDlg->UpdateActivityList(true);
}

void CECAutoTeam::OnWorldChanged()
{
	if( !IsMatchingForActivity() )
		return;

	bool bFound = false;
	int world_id = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
	for( size_t i=0;i<CECUIConfig::Instance().GetGameUI().nAutoTeamTransmitEnabledMap.size();i++ )
	{
		if( world_id == CECUIConfig::Instance().GetGameUI().nAutoTeamTransmitEnabledMap[i] )
		{
			bFound = true;
			break;
		}
	}

	if( !bFound )
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10865), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		Cancel(TYPE_ACTIVITY);
	}
}

CECGameUIMan* CECAutoTeam::GetGameUIMan()
{
	return g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
}

CECGameSession* CECAutoTeam::GetGameSession()
{
	return g_pGame->GetGameSession();
}

CECHostPlayer* CECAutoTeam::GetHostPlayer()
{
	return g_pGame->GetGameRun()->GetHostPlayer();
}
// Filename	: EC_PendingAction.cpp
// Creator	: Xu Wenbin
// Date		: 2011/11/09

#include "EC_PendingAction.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"

//	CECPendingAction
CECPendingAction::CECPendingAction(DWORD dwTimerPeriod)
: m_bNeedExecute(true)
, m_bTriggered(false)
, m_bExecuteResult(false)
{
	m_Counter.SetPeriod(dwTimerPeriod);
}

CECPendingAction::~CECPendingAction()
{
}

void CECPendingAction::Update(DWORD dwElapsedTime)
{
	if (GetNeedExecute())
	{
		//	每次都尝试执行
		if (m_Counter.IncCounter(dwElapsedTime))
		{
			//	计时器到

			//	强制执行
			m_bExecuteResult = Execute();

			//	不需要再进行尝试
			m_bNeedExecute = false;
		}
		//	else 计时
	}
}

void CECPendingAction::Trigger()
{
	if (!GetNeedExecute())
	{
		//	计时器已经强制执行，不需要再触发执行
		return;
	}

	if (GetTriggered())
	{
		//	不允许多次触发
		return;
	}

	m_bTriggered = true;
	m_bExecuteResult = Execute();
	m_bNeedExecute = false;
}

bool CECPendingAction::IsInGame()const
{
	return GetGameSession() != NULL
		&& GetGameSession()->IsConnected()
		&& GetGameRun() != NULL
		&& GetGameRun()->GetGameState() == CECGameRun::GS_GAME
		&& GetHostPlayer() != NULL
		&& GetHostPlayer()->HostIsReady();
}

CECGame * CECPendingAction::GetGame()const
{
	return g_pGame;
}

CECGameRun * CECPendingAction::GetGameRun()const
{
	return (GetGame() != NULL) ? GetGame()->GetGameRun() : NULL;
}

CECGameSession * CECPendingAction::GetGameSession()const
{
	return GetGame()!=NULL ? GetGame()->GetGameSession()  : NULL;
}

CECHostPlayer * CECPendingAction::GetHostPlayer()const
{
	return (GetGameRun() != NULL) ? GetGameRun()->GetHostPlayer() : NULL;
}

//	CECPendingLogoutHalf
bool CECPendingLogoutHalf::Execute()
{
	bool bSuccess(false);

	if (IsInGame())
	{
		GetGameSession()->c2s_CmdLogout(_PLAYER_LOGOUT_HALF);
		bSuccess = true;
	}

	return bSuccess;
}

//	CECPendingLogoutFull
bool CECPendingLogoutFull::Execute()
{
	bool bSuccess(false);

	if (IsInGame())
	{
		GetGameSession()->c2s_CmdLogout(_PLAYER_LOGOUT_FULL);
		bSuccess = true;
	}

	return bSuccess;
}

//	CECPendingSellingRole
bool CECPendingSellingRole::Execute()
{
	bool bSuccess(false);

	if (IsInGame())
	{
		GetGameSession()->c2s_CmdLogout(_PLAYER_LOGOUT_HALF);
		GetGameRun()->SetSellingRoleID(GetHostPlayer()->GetCharacterID());
		bSuccess = true;
	}

	return bSuccess;
}

//	CECPendingLogoutCrossServer
bool CECPendingLogoutCrossServer::Execute()
{
	GetGameRun()->SetLogoutFlag(2);	
	return true;
}

//	CECPendingCrossServerGetInOut
bool CECPendingCrossServerGetInOut::Execute()
{
	bool bSuccess(false);

	if (IsInGame())
	{
		if (m_bGetIn)
			g_pGame->GetGameSession()->c2s_CmdNPCSevCrossServerGetIn();
		else
			g_pGame->GetGameSession()->c2s_CmdNPCSevCrossServerGetOut();
	}
	g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->EndNPCService();

	return bSuccess;
}


//	CECPendingActionArray
CECPendingActionArray::CECPendingActionArray(bool bAllowMultiActions)
: m_bAllowMultiActions(bAllowMultiActions)
{
}

CECPendingActionArray::~CECPendingActionArray()
{
	Clear();
}

void CECPendingActionArray::Append(CECPendingAction *pAction)
{
	//	添加新事件
	if (!pAction)
	{
		ASSERT(false);
		return;
	}
	if (!pAction->GetNeedExecute())
	{
		delete pAction;
		ASSERT(false);
		return;
	}

	if (!GetAllowMultiAction())		Clear();
	m_actions.push_back(pAction);
}

void CECPendingActionArray::AppendForSaveConfig(CECPendingAction *pAction)
{
	if (!pAction)
	{
		ASSERT(false);
		return;
	}
	if (!pAction->GetNeedExecute())
	{
		delete pAction;
		ASSERT(false);
		return;
	}
	if (g_pGame->GetGameRun()->SaveConfigsToServer() != 2)
	{
		pAction->Trigger();
		delete pAction;
		return;
	}
	Append(pAction);
}

void CECPendingActionArray::Update(DWORD dwElapsedTime)
{
	if (m_actions.empty())	return;

	for (PendingActionList::iterator it = m_actions.begin(); it != m_actions.end();)
	{
		CECPendingAction *pAction = *it;
		pAction->Update(dwElapsedTime);
		if (!pAction->GetNeedExecute())
		{
			it = m_actions.erase(it);
			delete pAction;
		}
		else it++;
	}
}

void CECPendingActionArray::TriggerAll()
{
	if (m_actions.empty())	return;

	for (PendingActionList::iterator it = m_actions.begin(); it != m_actions.end(); ++ it)
	{
		CECPendingAction *pAction = *it;
		if (pAction->GetNeedExecute())
			pAction->Trigger();
	}
}

void CECPendingActionArray::Clear()
{
	//	清除当前所有事件
	if (m_actions.empty())	return;

	for (PendingActionList::iterator it = m_actions.begin(); it != m_actions.end(); ++ it)
	{
		CECPendingAction *pAction = *it;
		delete pAction;
	}
	m_actions.clear();
}

void CECPendingActionArray::SetAllowMultiAction(bool bAllow)
{
	if (GetAllowMultiAction() == bAllow)	return;

	m_bAllowMultiActions = bAllow;

	//	按时间先后顺序，只保留队列最后一个
	if (!bAllow && m_actions.size() > 1)
	{
		PendingActionList::iterator itBegin = m_actions.begin();
		PendingActionList::iterator itEnd = m_actions.end()-1;
		for (PendingActionList::iterator it = itBegin; it != itEnd; ++ it)
		{
			CECPendingAction *pAction = *it;
			delete pAction;
		}
		m_actions.erase(itBegin, itEnd);
	}
}
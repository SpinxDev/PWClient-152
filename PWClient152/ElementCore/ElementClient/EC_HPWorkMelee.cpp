/*
 * FILE: EC_HPWorkMelee.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_HPWorkMelee.h"
#include "EC_HostPlayer.h"
#include "EC_ElsePlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_ManPlayer.h"
#include "EC_HPWorkTrace.h"
#include "EC_HPWork.h"
#include "EC_GameSession.h"
#include "EC_PlayerWrapper.h"

#include "AC.h"

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
//	Implement CECHPWorkMelee
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkMelee::CECHPWorkMelee(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_HACKOBJECT, pWorkMan)
{
	m_dwMask		= MASK_HACKOBJECT;
	m_dwTransMask	= MASK_STAND | MASK_MOVETOPOS | MASK_TRACEOBJECT;

	CECHPWorkMelee::Reset();
}

CECHPWorkMelee::~CECHPWorkMelee()
{
}

//	Reset work
void CECHPWorkMelee::Reset()
{
	CECHPWork::Reset();

	m_idTarget		= 0;
	m_iIdleTime		= 0;
	m_iIdleTimeCnt	= 0;
}

//	Copy work data
bool CECHPWorkMelee::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkMelee* pSrc = (CECHPWorkMelee*)pWork;

	m_idTarget		= pSrc->m_idTarget;
	m_iIdleTime		= pSrc->m_iIdleTime;
	m_iIdleTimeCnt	= pSrc->m_iIdleTimeCnt;

	return true;
}

//	On first tick
void CECHPWorkMelee::OnFirstTick()
{
	m_pHost->m_iMoveMode = CECHostPlayer::MOVE_STAND;
	m_pHost->PlayAction(CECHostPlayer::ACT_ATTACK_1 + a_Random(0, 3));

	m_idTarget	= m_pHost->m_idSelTarget;
}

//	Set idle time
void CECHPWorkMelee::SetIdleTime(int iTime)
{
	m_iIdleTime		= iTime;
	m_iIdleTimeCnt	= 0;
}

//	Tick routine
bool CECHPWorkMelee::Tick(DWORD dwDeltaTime)
{
	CECHPWork::Tick(dwDeltaTime);

	if (!m_idTarget || m_idTarget == m_pHost->m_PlayerInfo.cid)
	{
		m_bFinished = true;
		return true;
	}

	//	If target turn to be un-attackable, cancel action
	if (!m_pHost->AttackableJudge(m_idTarget, true))
	{
		g_pGame->GetGameSession()->c2s_CmdCancelAction();
		m_bFinished = true;
		return true;
	}

	if (m_iIdleTimeCnt < m_iIdleTime)
	{
		m_iIdleTimeCnt += (int)dwDeltaTime;
		if (m_iIdleTimeCnt < m_iIdleTime)
			return true;

		m_iIdleTimeCnt = m_iIdleTime;
	}

	//	Face to target
	m_pHost->TurnFaceTo(m_idTarget);

	int idTraceTarget = 0;

	if (ISPLAYERID(m_idTarget))
	{
		CECElsePlayer* pPlayer = m_pHost->m_pPlayerMan->GetElsePlayer(m_idTarget);
		if (!pPlayer || pPlayer->IsDead())	//	Taget has missed ?
			return true;

	//	if (!m_pHost->CanTouchTarget(pPlayer->GetServerPos(), pPlayer->GetTouchRadius(), 1, 0.8f))
		if (!m_pHost->CanTouchTarget(pPlayer->GetPos(), pPlayer->GetTouchRadius(), 1))
			idTraceTarget = m_idTarget;
	}
	else if (ISNPCID(m_idTarget))
	{
		CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(m_idTarget);
		if (!pNPC || pNPC->IsDead())	//	Taget has missed ?
			return true;

		if (!m_pHost->CanTouchTarget(pNPC->GetPos(), pNPC->GetTouchRadius(), 1))
			idTraceTarget = m_idTarget;
	}

	if (idTraceTarget && !m_pHost->IsRooting())
	{
		CECHPWorkTrace* pWork = (CECHPWorkTrace*)m_pWorkMan->CreateWork(WORK_TRACEOBJECT);
		pWork->SetTraceTarget(pWork->CreatTraceTarget(idTraceTarget,CECHPWorkTrace::TRACE_ATTACK));

		class CECHPWorkPostTickCommand1 : public CECHPWorkPostTickRunWorkCommand{
		public:
			CECHPWorkPostTickCommand1(CECHPWork *pWork)
				: CECHPWorkPostTickRunWorkCommand(pWork, true)
			{}
			virtual bool Run(CECHPWorkMan *pWorkMan){
				if (!CECHPWorkPostTickRunWorkCommand::Run(pWorkMan)){
					return false;
				}
				AP_ActionEvent(AP_EVENT_MELEEOUTOFRANGE);
				return true;
			}
		};
		m_pWorkMan->SetPostTickCommand(new CECHPWorkPostTickCommand1(pWork));
		return true;
	}

	return true;
}

void CECHPWorkMelee::Cancel(){
	m_pHost->TurnFaceTo(0);
}

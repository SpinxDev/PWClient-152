/*
 * FILE: EC_HPWorkSit.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_HPWorkSit.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_AssureMove.h"
#include "EC_ManPlayer.h"
#include "EC_AutoPolicy.h"
#include "EC_PlayerWrapper.h"
#include "EC_ElementDataHelper.h"

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
//	Implement CECHPWorkSit
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkSit::CECHPWorkSit(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_SIT, pWorkMan)
{
	m_dwMask		= MASK_SIT;
	m_dwTransMask	= MASK_STAND;

	CECHPWorkSit::Reset();
}

CECHPWorkSit::~CECHPWorkSit()
{
}

//	Reset work
void CECHPWorkSit::Reset()
{
	CECHPWork::Reset();

	m_bBeSitting = false;
}

//	Copy work data
bool CECHPWorkSit::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkSit* pSrc = (CECHPWorkSit*)pWork;

	m_bBeSitting = pSrc->m_bBeSitting;

	return true;
}

//	On first tick
void CECHPWorkSit::OnFirstTick()
{
	m_pHost->m_iMoveMode = CECHostPlayer::MOVE_STAND;

	if (m_bBeSitting)
		m_pHost->PlayAction(CECHostPlayer::ACT_SITDOWN_LOOP);
	else
	{
		m_pHost->PlayAction(CECHostPlayer::ACT_SITDOWN);
		m_pHost->PlayAction(CECHostPlayer::ACT_SITDOWN_LOOP, true, 200, true);
	}
}

//	Tick routine
bool CECHPWorkSit::Tick(DWORD dwDeltaTime)
{
	CECHPWork::Tick(dwDeltaTime);

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkPick
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkPick::CECHPWorkPick(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_PICKUP, pWorkMan)
{
	m_dwMask		= MASK_PICKUP;
	m_dwTransMask	= MASK_STAND | MASK_MOVETOPOS;

	m_TimeCnt.SetPeriod(500);

	CECHPWorkPick::Reset();
}

void CECHPWorkPick::SetGather(bool bGather, int gatherItemID){
	m_bIsGather = bGather;
	m_gatherItemID = gatherItemID;
}

CECHPWorkPick::~CECHPWorkPick()
{
}

//	Reset work
void CECHPWorkPick::Reset()
{
	CECHPWork::Reset();

	m_TimeCnt.Reset();
}

//	Copy work data
bool CECHPWorkPick::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkPick* pSrc = (CECHPWorkPick*)pWork;

	m_bIsGather	= pSrc->m_bIsGather;
	m_gatherItemID = pSrc->m_gatherItemID;

	m_TimeCnt.SetPeriod(pSrc->m_TimeCnt.GetPeriod());
	m_TimeCnt.SetCounter(pSrc->m_TimeCnt.GetCounter());

	return true;
}

//	On first tick
void CECHPWorkPick::OnFirstTick()
{
	if (IsGather())
	{
		m_pHost->PlayActionWithConfig(CECPlayer::ACT_PICKUP, CECElementDataHelper::GetGatherStartActionConfig(m_gatherItemID));
		m_pHost->PlayActionWithConfig(CECPlayer::ACT_PICKUP_LOOP, CECElementDataHelper::GetGatherLoopActionConfig(m_gatherItemID), false, 200, true);
	}
	else
	{
		m_pHost->PlayAction(CECPlayer::ACT_PICKUP_MATTER);
		m_pHost->PlayAction(CECPlayer::ACT_STAND, false, 200, true);
	}
}

//  Work is cancel
void CECHPWorkPick::Cancel()
{
	CECHPWork::Cancel();
	if( !IsGather() )
	{
		AP_ActionEvent(AP_EVENT_PICKUPOK);
	}
}

//	Tick routine
bool CECHPWorkPick::Tick(DWORD dwDeltaTime)
{
	DWORD dwRealTime = g_pGame->GetRealTickTime();

	CECHPWork::Tick(dwRealTime);

	if (!IsGather() && m_TimeCnt.IncCounter(dwRealTime))
	{
		m_bFinished = true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkConcentrate
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkConcentrate::CECHPWorkConcentrate(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_CONCENTRATE, pWorkMan)
{
	m_dwMask		= MASK_CONCENTRATE;
	m_dwTransMask	= MASK_STAND;
	m_iDoWhat		= DO_SUMMONPET;

	CECHPWorkConcentrate::Reset();
}

CECHPWorkConcentrate::~CECHPWorkConcentrate()
{
}

//	Reset work
void CECHPWorkConcentrate::Reset()
{
	CECHPWork::Reset();
}

//	Copy work data
bool CECHPWorkConcentrate::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkConcentrate* pSrc = (CECHPWorkConcentrate*)pWork;
	m_iDoWhat = pSrc->m_iDoWhat;

	return true;
}

//	On first tick
void CECHPWorkConcentrate::OnFirstTick()
{
}

//	Tick routine
bool CECHPWorkConcentrate::Tick(DWORD dwDeltaTime)
{
	DWORD dwRealTime = g_pGame->GetRealTickTime();

	CECHPWork::Tick(dwRealTime);

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkBeBound
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkBeBound::CECHPWorkBeBound(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_BEBOUND, pWorkMan)
{
	m_dwMask		= MASK_BEBOUND;
	m_dwTransMask	= MASK_STAND;

	CECHPWorkBeBound::Reset();
}

CECHPWorkBeBound::~CECHPWorkBeBound()
{
}

//	Reset work
void CECHPWorkBeBound::Reset()
{
	CECHPWork::Reset();
}

//	Copy work data
bool CECHPWorkBeBound::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkBeBound* pSrc = (CECHPWorkBeBound*)pWork;

	return true;
}

//	On first tick
void CECHPWorkBeBound::OnFirstTick()
{
}

//	Tick routine
bool CECHPWorkBeBound::Tick(DWORD dwDeltaTime)
{
	DWORD dwRealTime = g_pGame->GetRealTickTime();

	CECHPWork::Tick(dwRealTime);

	if( g_pGame->GetGameRun()->GetWorld()->GetAssureMove() )
	{
		g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->AssureMove(m_pHost->GetPos(), m_pHost->GetPos());
		CECPlayer * pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(m_pHost->GetBuddyID());

		if( pPlayer && pPlayer->IsFlying() )
			g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->NoAssureMove();
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkCongregate
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkCongregate::CECHPWorkCongregate(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_CONGREGATE, pWorkMan)
,m_ConType(-1)
{
	m_dwMask		= MASK_CONGREGATE;
	m_dwTransMask	= MASK_STAND;

	CECHPWorkCongregate::Reset();
}

CECHPWorkCongregate::~CECHPWorkCongregate()
{
}

//	Reset work
void CECHPWorkCongregate::Reset()
{
	CECHPWork::Reset();

	SetTimeout(-1, 0);
}

//	Copy work data
bool CECHPWorkCongregate::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkCongregate* pSrc = (CECHPWorkCongregate*)pWork;

	m_TimeCnt.SetPeriod(pSrc->m_TimeCnt.GetPeriod());
	m_TimeCnt.SetCounter(pSrc->m_TimeCnt.GetCounter());

	m_ConType = pSrc->m_ConType;

	return true;
}

void CECHPWorkCongregate::SetTimeout(int iType, int iTimeout)
{
	m_TimeCnt.SetPeriod(iTimeout);
	m_TimeCnt.SetCounter(0);

	m_ConType = iType;
}

CECCounter CECHPWorkCongregate::GetCounter()
{
	return m_TimeCnt;
}

int CECHPWorkCongregate::GetConType() const
{
	return m_ConType;
}

//	On first tick
void CECHPWorkCongregate::OnFirstTick()
{
	m_pHost->PlayAction(CECPlayer::ACT_SITDOWN);
	m_pHost->PlayAction(CECPlayer::ACT_SITDOWN_LOOP, false, 200, true);
}

//	Tick routine
bool CECHPWorkCongregate::Tick(DWORD dwDeltaTime)
{
	DWORD dwRealTime = g_pGame->GetRealTickTime();
	
	CECHPWork::Tick(dwRealTime);
	
	if (m_TimeCnt.IncCounter(dwRealTime))
	{
		m_bFinished = true;
	}
	
	return true;
}

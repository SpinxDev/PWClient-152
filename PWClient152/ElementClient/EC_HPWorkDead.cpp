/*
 * FILE: EC_HPWorkDead.cpp
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
#include "EC_HPWorkDead.h"
#include "EC_HostPlayer.h"

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
//	Implement CECHPWorkDead
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkDead::CECHPWorkDead(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_DEAD, pWorkMan)
{
	m_dwMask		= MASK_DEAD;
	m_dwTransMask	= MASK_STAND;
	
	CECHPWorkDead::Reset();
}

CECHPWorkDead::~CECHPWorkDead()
{
}

//	Reset work
void CECHPWorkDead::Reset()
{
	CECHPWork::Reset();

	m_bBeDead = false;
}

//	Copy work data
bool CECHPWorkDead::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkDead* pSrc = (CECHPWorkDead*)pWork;
	
	m_bBeDead = pSrc->m_bBeDead;

	return true;
}

//	On first tick
void CECHPWorkDead::OnFirstTick()
{
	m_pHost->m_iMoveMode = CECHostPlayer::MOVE_STAND;

	A3DVECTOR3 vPos = m_pHost->GetPos();

	if (m_bBeDead)
	{
		if (m_pHost->IsUnderWater())
			m_pHost->PlayAction(CECHostPlayer::ACT_WATERDIE_LOOP);
		else if (vPos.y >= m_pHost->m_GndInfo.fGndHei + 0.5f)
			m_pHost->PlayAction(CECHostPlayer::ACT_AIRDIE);
		else
			m_pHost->PlayAction(CECHostPlayer::ACT_GROUNDDIE_LOOP);
	}
	else
	{
		A3DVECTOR3 vPos = m_pHost->GetPos();
		if (m_pHost->IsUnderWater())
		{
			m_pHost->PlayAction(CECHostPlayer::ACT_WATERDIE);
			m_pHost->PlayAction(CECHostPlayer::ACT_WATERDIE_LOOP, true, 200, true);
		}
		else if (vPos.y >= m_pHost->m_GndInfo.fGndHei + 0.5f)
		{
			m_pHost->PlayAction(CECHostPlayer::ACT_AIRDIE_ST);
			m_pHost->PlayAction(CECHostPlayer::ACT_AIRDIE, true, 200, true);
		}
		else
		{
			m_pHost->PlayAction(CECHostPlayer::ACT_GROUNDDIE);
			m_pHost->PlayAction(CECHostPlayer::ACT_GROUNDDIE_LOOP, true, 200, true);
		}
	}
	
	m_pHost->ShowWing(false);
}

//	Tick routine
bool CECHPWorkDead::Tick(DWORD dwDeltaTime)
{
	CECHPWork::Tick(dwDeltaTime);

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkRevive
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkRevive::CECHPWorkRevive(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_REVIVE, pWorkMan)
{
	m_dwMask		= MASK_REVIVE;
	m_dwTransMask	= MASK_STAND | MASK_REVIVE;

	CECHPWorkRevive::Reset();
}

CECHPWorkRevive::~CECHPWorkRevive()
{
}

//	Reset work
void CECHPWorkRevive::Reset()
{
	CECHPWork::Reset();

	m_dwOTCnt = 0;
}

//	Copy work data
bool CECHPWorkRevive::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkRevive* pSrc = (CECHPWorkRevive*)pWork;
	
	m_dwOTCnt	= pSrc->m_dwOTCnt;

	return true;
}

//	On first tick
void CECHPWorkRevive::OnFirstTick()
{
	m_pHost->m_iMoveMode = CECHostPlayer::MOVE_STAND;

	m_pHost->PlayAction(CECHostPlayer::ACT_REVIVE);
}

//	Tick routine
bool CECHPWorkRevive::Tick(DWORD dwDeltaTime)
{
	CECHPWork::Tick(dwDeltaTime);

	if ((m_dwOTCnt += dwDeltaTime) >= 15000)
		m_bFinished = true;

	return true;
}






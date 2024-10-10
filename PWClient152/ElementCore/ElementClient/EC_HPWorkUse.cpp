/*
 * FILE: EC_HPWorkUse.cpp
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
#include "EC_HPWorkUse.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_IvtrItem.h"
#include "EC_ManMatter.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_Matter.h"
#include "elementdataman.h"
#include "EC_AutoPolicy.h"
#include "EC_PlayerWrapper.h"

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
//	Implement CECHPWorkUse
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkUse::CECHPWorkUse(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_USEITEM, pWorkMan)
{
	m_dwMask		= MASK_USEITEM;
	m_dwTransMask	= MASK_STAND | MASK_MOVETOPOS;

	CECHPWorkUse::Reset();
}

CECHPWorkUse::~CECHPWorkUse()
{
}

//	Reset work
void CECHPWorkUse::Reset()
{
	CECHPWork::Reset();

	m_idItem	= 0;
	m_idTarget	= 0;
	m_bWorkForMonsterSpirit = false;

	m_UseTimeCnt.SetPeriod(100);
	m_UseTimeCnt.Reset();
}

//	Copy work data
bool CECHPWorkUse::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkUse* pSrc = (CECHPWorkUse*)pWork;

	m_idItem	= pSrc->m_idItem;
	m_idTarget	= pSrc->m_idTarget;
	m_bWorkForMonsterSpirit = pSrc->m_bWorkForMonsterSpirit;
		
	m_UseTimeCnt.SetPeriod(pSrc->m_UseTimeCnt.GetPeriod());
	m_UseTimeCnt.SetCounter(pSrc->m_UseTimeCnt.GetCounter());

	return true;
}

//	On first tick
void CECHPWorkUse::OnFirstTick()
{
	m_pHost->m_iMoveMode = CECPlayer::MOVE_STAND;

	CECIvtrItem* pItem = CECIvtrItem::CreateItem(m_idItem, 0, 1);
	if (!pItem) 
		return;

	if (m_bWorkForMonsterSpirit) m_pHost->PlayGatherMonsterSpiritAction();
	else m_pHost->PlayStartUseItemAction(m_idItem);

	delete pItem;
}

//  Work is cancel
void CECHPWorkUse::Cancel()
{
	if (m_idTarget){
		m_pHost->TurnFaceTo(0);
	}
	CECHPWork::Cancel();
	AP_ActionEvent(AP_EVENT_STOPUSEITEM);
}

//	Set parameters
void CECHPWorkUse::SetParams(int idItem, DWORD dwUseTime, int idTarget, bool bWorkForMonsterSpirit)
{
	m_idItem	= idItem;
	m_idTarget	= idTarget;
	m_bWorkForMonsterSpirit = bWorkForMonsterSpirit;

	m_UseTimeCnt.SetPeriod(dwUseTime);
	m_UseTimeCnt.Reset();
}

//	Tick routine
bool CECHPWorkUse::Tick(DWORD dwDeltaTime)
{
	CECHPWork::Tick(dwDeltaTime);

	if (m_UseTimeCnt.IncCounter(dwDeltaTime))
	{
		m_UseTimeCnt.Reset(true);
		m_bFinished = true;
	}

	if (m_idTarget)
		m_pHost->TurnFaceTo(m_idTarget);

	return true;
}


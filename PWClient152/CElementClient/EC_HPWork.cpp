/*
 * FILE: EC_HPWork.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/2/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_HPWork.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_NPC.h"

#include "EC_HPWorkUse.h"
#include "EC_HPWorkDead.h"
#include "EC_HPWorkMelee.h"
#include "EC_HPWorkStand.h"
#include "EC_HPWorkTrace.h"
#include "EC_HPWorkFollow.h"
#include "EC_HPWorkMove.h"
#include "EC_HPWorkFly.h"
#include "EC_HPWorkFall.h"
#include "EC_HPWorkSit.h"
#include "EC_HPWorkSpell.h"
#include "EC_HPWorkForceNavigate.h"
#include "EC_Skill.h"
#include "EC_CastSkillWhenMove.h"
#include "EC_GameRun.h"
#include "EC_HostInputFilter.h"

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
//	Implement CECHPWork
//	
///////////////////////////////////////////////////////////////////////////

CECHPWork::CECHPWork(int iWorkID, CECHPWorkMan* pWorkMan) : CECObjectWork(iWorkID)
{
	ASSERT(pWorkMan);
	m_pWorkMan	= pWorkMan;
	m_pHost		= pWorkMan->GetHostPlayer();
}

//	Copy work data
bool CECHPWork::CopyData(CECHPWork* pWork)
{
	ASSERT(pWork && m_iWorkID == pWork->GetWorkID());
	m_bFinished	 = pWork->m_bFinished;
	m_bFirstTick = pWork->m_bFirstTick;
	return true;
}

void CECHPWork::Cancel(){
}

const char *CECHPWork::GetWorkName()const{
	return GetWorkName(GetWorkID());
}

bool CECHPWork::Validate(int iWorkID){
	return iWorkID >= 0 && iWorkID < NUM_WORK;
}

const char * CECHPWork::GetWorkName(int iWorkID){
	static const char *l_WorkName[NUM_WORK] = {
		"WORK_STAND",
		"WORK_MOVETOPOS",
		"WORK_TRACEOBJECT",
		"WORK_HACKOBJECT",
		"WORK_SPELLOBJECT",
		"WORK_USEITEM",	
		"WORK_DEAD",		
		"WORK_FOLLOW",
		"WORK_FLYOFF",	
		"WORK_FREEFALL",	
		"WORK_SIT",		
		"WORK_PICKUP",
		"WORK_CONCENTRATE",
		"WORK_REVIVE",	
		"WORK_FLASHMOVE",
		"WORK_BEBOUND",	
		"WORK_PASSIVEMOVE",
		"WORK_CONGREGATE",
		"WORK_SKILLSTATEACT",
		"WORK_FORCENAVIGATEMOVE"
	};
	if (Validate(iWorkID)){
		return l_WorkName[iWorkID];
	}else if (iWorkID == WORK_INVALID){
		return "WORK_INVALID";
	}else{
		return "Unknown";
	}
}


unsigned int CECHPWork::GetWorkMask(int iWorkID){
	if (!Validate(iWorkID)){
		ASSERT(false);
		return 0;
	}
	static unsigned int l_workMask[NUM_WORK] = {
		MASK_STAND,
		MASK_MOVETOPOS,
		MASK_TRACEOBJECT,
		MASK_HACKOBJECT,
		MASK_SPELLOBJECT,
		MASK_USEITEM,
		MASK_DEAD,
		MASK_FOLLOW,
		MASK_FLYOFF,
		MASK_FREEFALL,
		MASK_SIT,
		MASK_PICKUP,
		MASK_CONCENTRATE,
		MASK_REVIVE,
		MASK_FLASHMOVE,
		MASK_BEBOUND,
		MASK_CONGREGATE,
		MASK_SKILLSTATEACT,
		MASK_FORCENAVIGATE,
	};
	return l_workMask[iWorkID];
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkMan
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkMan::CECHPWorkMan(CECHostPlayer* pHost)
{
	m_pHost		= pHost;
	m_iCurPriority	= -1;
	memset(&m_Delayed, 0, sizeof (m_Delayed));
	m_pPostTickCommand = NULL;
}

CECHPWorkMan::~CECHPWorkMan()
{
	for (int i(0); i < NUM_PRIORITY; ++ i){
		WorkList &workList = m_WorkStack[i];
		for (size_t j(0); j < workList.size(); ++ j){
			delete workList[j];
		}
		workList.clear();
	}
	if (m_Delayed.pWork){
		delete m_Delayed.pWork;
		m_Delayed.pWork = NULL;
	}
	if (m_pPostTickCommand){
		delete m_pPostTickCommand;
		m_pPostTickCommand = NULL;
	}
}

//	Create a work object
CECHPWork* CECHPWorkMan::CreateWork(int idWork)
{
	CECHPWork* pWork = NULL;

	switch (idWork)
	{
	case CECHPWork::WORK_STAND:			pWork = new CECHPWorkStand(this);		break;
	case CECHPWork::WORK_MOVETOPOS:		pWork = new CECHPWorkMove(this);		break;
	case CECHPWork::WORK_TRACEOBJECT:	pWork = new CECHPWorkTrace(this);		break;
	case CECHPWork::WORK_HACKOBJECT:	pWork = new CECHPWorkMelee(this);		break;
	case CECHPWork::WORK_SPELLOBJECT:	pWork = new CECHPWorkSpell(this);		break;
	case CECHPWork::WORK_USEITEM:		pWork = new CECHPWorkUse(this);			break;
	case CECHPWork::WORK_DEAD:			pWork = new CECHPWorkDead(this);		break;
	case CECHPWork::WORK_FOLLOW:		pWork = new CECHPWorkFollow(this);		break;
	case CECHPWork::WORK_FLYOFF:		pWork = new CECHPWorkFly(this);			break;
	case CECHPWork::WORK_FREEFALL:		pWork = new CECHPWorkFall(this);		break;
	case CECHPWork::WORK_SIT:			pWork = new CECHPWorkSit(this);			break;
	case CECHPWork::WORK_PICKUP:		pWork = new CECHPWorkPick(this);		break;
	case CECHPWork::WORK_CONCENTRATE:	pWork = new CECHPWorkConcentrate(this);	break;
	case CECHPWork::WORK_REVIVE:		pWork = new CECHPWorkRevive(this);		break;
	case CECHPWork::WORK_FLASHMOVE:		pWork = new CECHPWorkFMove(this);		break;
	case CECHPWork::WORK_BEBOUND:		pWork = new CECHPWorkBeBound(this);		break;
	case CECHPWork::WORK_PASSIVEMOVE:	pWork = new CECHPWorkPassiveMove(this);	break;
	case CECHPWork::WORK_CONGREGATE	:	pWork = new CECHPWorkCongregate(this);	break;
	case CECHPWork::WORK_SKILLSTATEACT: pWork = new CECHPWorkSkillStateAction(this); break;
	case CECHPWork::WORK_FORCENAVIGATEMOVE: pWork = new CECHPWorkNavigate(this); break;
	default:
		ASSERT(0);
		return NULL;
	}

	return pWork;
}

CECHPWorkTrace *CECHPWorkMan::CreateNPCTraceWork(CECNPC *pNPC, int relatedTaskID){
	CECHPWorkTrace * result = NULL;
	if (pNPC && pNPC->IsServerNPC()){
		result = (CECHPWorkTrace*)CreateWork(CECHPWork::WORK_TRACEOBJECT);
		if (relatedTaskID > 0){
			CECTracedTaskNPC* pTraceTaskNPC = new CECTracedTaskNPC(TRACE_TASKNPC, pNPC->GetNPCID(), m_pHost, relatedTaskID);
			result->SetTraceTarget(pTraceTaskNPC, true);	
		}else{
			result->SetTraceTarget(result->CreatTraceTarget(pNPC->GetNPCID(), CECHPWorkTrace::TRACE_TALK),true);
		}
	}
	return result;
}

bool CECHPWorkMan::CanCastSkillImmediately(int idSkill)const{
	return !IsSpellingMagic() && !HasWorkRunningOnPriority(PRIORITY_2);
}

bool CECHPWorkMan::ValidatePriority(int iPriority)const{
	return iPriority >= 0 && iPriority < NUM_PRIORITY;
}

bool CECHPWorkMan::StartWork(int iPriority, CECHPWork* pWork, bool bNoDelay/* false */){
	if (!pWork){
		ASSERT(false);
		return false;
	}
	if (!ValidatePriority(iPriority)){
		ASSERT(false);
		delete pWork;
		return false;
	}
	if (pWork->GetWorkID() == CECHPWork::WORK_TRACEOBJECT){
		if (g_pGame->GetGameRun()->GetHostInputFilter()->IsMoveUsagePressed()){
			CECHPWorkTrace *pWorkTrace = dynamic_cast<CECHPWorkTrace *>(pWork);
			if (!pWorkTrace->CanTouch()){	//	2014-8-16 当还在通过键盘操纵移动时、有条件忽略 CECHPWorkTrace,
				delete pWorkTrace;			//	否则会因 CECHPWorkTrace 中临时转向目标位置、而后又被键盘操纵调整移动方向导致方向瞬间抖动
				return false;				//	当 CECHPWorkTrace 中目标可立即接触时，不忽略 CECHPWorkTrace，以实现键盘控制移动中、对某怪应用技能时，立刻转向怪施放技能
			}
		}
	}
	if (!bNoDelay && DelayWork(iPriority, pWork)){
		return true;
	}
	return InternallyStartWork(iPriority, pWork);
}

bool CECHPWorkMan::CanSpellWhileMoving(const CECHPWorkSpell *pWorkSpell)const{
	return pWorkSpell->GetSkill()
		&& CECCastSkillWhenMove::Instance().IsSkillSupported(pWorkSpell->GetSkill()->GetSkillID(), m_pHost);
}

bool CECHPWorkMan::CanRunSimultaneously(const CECHPWork *pWork1, const CECHPWork *pWork2)const{
	if (pWork1->GetWorkID() == CECHPWork::WORK_MOVETOPOS &&
		pWork2->GetWorkID() == CECHPWork::WORK_SPELLOBJECT){
		return CanSpellWhileMoving(dynamic_cast<const CECHPWorkSpell *>(pWork2));
	}
	if (pWork2->GetWorkID() == CECHPWork::WORK_MOVETOPOS &&
		pWork1->GetWorkID() == CECHPWork::WORK_SPELLOBJECT){
		return CanSpellWhileMoving(dynamic_cast<const CECHPWorkSpell *>(pWork1));
	}
	return false;
}

bool CECHPWorkMan::CanRunSimultaneouslyWithCurrentWork(int iPriority, const CECHPWork *pWork)const{
	if (!ValidatePriority(iPriority) || iPriority != m_iCurPriority){
		return false;
	}
	const WorkList &workList = m_WorkStack[iPriority];
	if (workList.empty()){
		return false;
	}
	for (size_t i(0); i < workList.size(); ++ i){
		if (!CanRunSimultaneously(workList[i], pWork)){
			return false;
		}
	}
	return true;
}

bool CECHPWorkMan::InternallyStartWork(int iPriority, CECHPWork *pWork){
	bool bStarted(false);
	if (CanRunSimultaneouslyWithCurrentWork(iPriority, pWork)){
		WorkList &workList = m_WorkStack[iPriority];
		workList.push_back(pWork);
		bStarted = true;
		LOG_DEBUG_INFO(AString().Format("CECHPWork::%s started simultaneously, priority=%d", pWork->GetWorkName(), iPriority));
	}else{
		if (pWork->GetWorkID() == CECHPWork::WORK_DEAD){
			for (int i = iPriority; i < NUM_PRIORITY; i++){
				FinishWorkAtPriority(i);
			}
			ClearDelayedWork();
			WorkList &workList = m_WorkStack[iPriority];
			workList.push_back(pWork);
			m_iCurPriority = iPriority;
			bStarted = true;
		}else{
			bool shouldStart = (iPriority >= m_iCurPriority);
			FinishWorkAtPriority(iPriority);
			WorkList &workList = m_WorkStack[iPriority];
			workList.push_back(pWork);
			if (shouldStart){
				if (iPriority > m_iCurPriority){
					CancelWorkAtPriority(m_iCurPriority);
				}
				m_iCurPriority = iPriority;
				bStarted = true;
			}
		}
		if (bStarted){
			LOG_DEBUG_INFO(AString().Format("CECHPWork::%s started, priority=%d", pWork->GetWorkName(), iPriority));
		}else{
			LOG_DEBUG_INFO(AString().Format("CECHPWork::%s add to priority queue %d", pWork->GetWorkName(), iPriority));
		}
	}
	return bStarted;
}

bool CECHPWorkMan::DelayWork(int iPriority, CECHPWork* pWork){
	if (!pWork){
		ASSERT(false);
		return false;
	}
	if (!ValidatePriority(iPriority)){
		ASSERT(false);
		return false;
	}
	if (!IsAnyWorkRunning()){
		return false;
	}
	if (CanRunSimultaneouslyWithCurrentWork(iPriority, pWork)){
		return false;
	}
	bool bDelay = false;
	if (m_pHost->IsMeleeing()){
		g_pGame->GetGameSession()->c2s_CmdCancelAction();
		bDelay = true;
	}else if (IsPickingUp() || IsSpellingMagic()){
		bDelay = true;
	}
	if (bDelay){
		ClearDelayedWork();
		m_Delayed.iPriority	= iPriority;
		m_Delayed.pWork		= pWork;
		LOG_DEBUG_INFO(AString().Format("CECHPWork::%s delayed, priority=%d", pWork->GetWorkName(), iPriority));
	}
	return bDelay;
}

void CECHPWorkMan::StartDelayedWork(){
	if (!HasDelayedWork()){
		return;
	}
	CECHPWork *pWork = m_Delayed.pWork;
	m_Delayed.pWork = NULL;
	LOG_DEBUG_INFO(AString().Format("CECHPWork:: start delayed work %s, priority=%d", pWork->GetWorkName(), m_Delayed.iPriority));
	InternallyStartWork(m_Delayed.iPriority, pWork);
}

void CECHPWorkMan::ClearDelayedWork(){	
	if (!HasDelayedWork()){
		return;
	}
	LOG_DEBUG_INFO(AString().Format("delayed CECHPWork::%s priority=%d cleared", m_Delayed.pWork->GetWorkName(), m_Delayed.iPriority));
	delete m_Delayed.pWork;		
	m_Delayed.pWork = NULL;
}

void CECHPWorkMan::FinishWork(const CECHPWorkMatcher & matcher){
	bool bCurrentWorkFinished(false);
	for (int i = NUM_PRIORITY-1; i >= 0; -- i){
		WorkList &workList = m_WorkStack[i];
		if (workList.empty()){
			continue;
		}
		for (size_t j(0); j < workList.size();){
			if (!matcher(workList[j], i, false)){
				++ j;
				continue;
			}
			KillWork(i, j);
			if (i == m_iCurPriority && workList.empty()){
				bCurrentWorkFinished = true;
			}
		}
	}
	if (GetDelayedWork() && matcher(m_Delayed.pWork, m_Delayed.iPriority, true)){
		ClearDelayedWork();
	}
	if (bCurrentWorkFinished){
		StartAwaitingWorks();
	}
}

void CECHPWorkMan::FinishRunningWork(int idWork){
	if (!IsWorkRunning(idWork)){
		return;
	}
	WorkList &workList = m_WorkStack[m_iCurPriority];
	for (size_t i(0); i < workList.size();){
		if (workList[i]->GetWorkID() != idWork){
			++ i;
			continue;
		}
		KillWork(m_iCurPriority, i);
	}
	if (workList.empty()){
		StartAwaitingWorks();
	}
}

void CECHPWorkMan::FinishAllWork(bool bGotoStand){
	for (int i(0); i < NUM_PRIORITY; ++ i){
		FinishWorkAtPriority(i);
	}
	ClearDelayedWork();
	if (bGotoStand){
		StartWork_p0(CreateWork(CECHPWork::WORK_STAND));
	}
}

void CECHPWorkMan::FinishWorkAtPriority(int iPriority){
	if (!ValidatePriority(iPriority)){
		ASSERT(false);
		return;
	}
	WorkList &workList = m_WorkStack[iPriority];
	while (!workList.empty()){
		KillWork(iPriority, 0);
	}
	if (m_iCurPriority == iPriority){
		m_iCurPriority = -1;
	}
}

void CECHPWorkMan::CancelWork(CECHPWork *pWork){
	if (!pWork){
		ASSERT(false);
		return;
	}
	pWork->Cancel();
}

void CECHPWorkMan::KillWork(CECHPWork *pWork){
	if (!pWork){
		ASSERT(false);
		return;
	}
	if (pWork->GetWorkID() == CECHPWork::WORK_MOVETOPOS){
		CECHPWorkMove *pWorkMove = dynamic_cast<CECHPWorkMove *>(pWork);
		if (pWorkMove){
			if (pWorkMove->GetAutoMove()){
				pWorkMove->Finish();
			}else{
				pWorkMove->Cancel();
			}
		}
	}else{
		pWork->Cancel();
	}
}

void CECHPWorkMan::CancelWorkAtPriority(int iPriority){
	if (!ValidatePriority(iPriority)){
		return;
	}
	WorkList &workList = m_WorkStack[iPriority];
	for (size_t i(0); i < workList.size(); ++ i){
		CancelWork(workList[i]);
		LOG_DEBUG_INFO(AString().Format("CECHPWork::%s priority=%d cancelled", workList[i]->GetWorkName(), iPriority));
	}
}

bool CECHPWorkMan::IsWorkRunning(int iWorkID)const{
	bool result(false);
	if (ValidatePriority(m_iCurPriority)){
		const WorkList &workList = m_WorkStack[m_iCurPriority];
		for (size_t i(0); i < workList.size(); ++ i){
			if (iWorkID == workList[i]->GetWorkID()){
				result = true;
				break;
			}
		}
	}
	return result;
}

bool CECHPWorkMan::IsAnyWorkRunning()const{
	return HasWorkRunningOnPriority(m_iCurPriority);
}

bool CECHPWorkMan::IsSpellingMagic()const{
	return IsWorkRunning(CECHPWork::WORK_SPELLOBJECT);
}

bool CECHPWorkMan::IsPickingUp()const{
	return IsWorkRunning(CECHPWork::WORK_PICKUP);
}

bool CECHPWorkMan::IsSitting()const{
	return IsWorkRunning(CECHPWork::WORK_SIT);
}

bool CECHPWorkMan::IsStanding()const{
	return IsWorkRunning(CECHPWork::WORK_STAND);
}

bool CECHPWorkMan::IsUsingItem()const{
	return IsWorkRunning(CECHPWork::WORK_USEITEM);
}

bool CECHPWorkMan::IsFlashMoving()const{
	return IsWorkRunning(CECHPWork::WORK_FLASHMOVE);
}

bool CECHPWorkMan::IsPassiveMoving()const{
	return IsWorkRunning(CECHPWork::WORK_PASSIVEMOVE);
}

bool CECHPWorkMan::IsFlyingOff()const{
	return IsWorkRunning(CECHPWork::WORK_FLYOFF);
}

bool CECHPWorkMan::IsReviving()const{
	return IsWorkRunning(CECHPWork::WORK_REVIVE);
}

bool CECHPWorkMan::IsFollowing()const{
	return IsWorkRunning(CECHPWork::WORK_FOLLOW);
}

bool CECHPWorkMan::IsTracing()const{
	return IsWorkRunning(CECHPWork::WORK_TRACEOBJECT);
}

bool CECHPWorkMan::IsOperatingPet()const{
	return IsWorkRunning(CECHPWork::WORK_CONCENTRATE);
}

bool CECHPWorkMan::IsCongregating()const{
	return IsWorkRunning(CECHPWork::WORK_CONGREGATE);
}

bool CECHPWorkMan::IsPlayingSkillAction()const{
	return IsWorkRunning(CECHPWork::WORK_SKILLSTATEACT);
}

bool CECHPWorkMan::IsMovingToPosition()const{
	return IsWorkRunning(CECHPWork::WORK_MOVETOPOS);
}

bool CECHPWorkMan::IsFreeFalling()const{
	return IsWorkRunning(CECHPWork::WORK_FREEFALL);
}

bool CECHPWorkMan::IsDead()const{
	return IsWorkRunning(CECHPWork::WORK_DEAD);
}

bool CECHPWorkMan::IsBeingBound()const{
	return IsWorkRunning(CECHPWork::WORK_BEBOUND);
}

bool CECHPWorkMan::IsMoving()const{	
	bool result(false);
	if (ValidatePriority(m_iCurPriority)){
		const WorkList &workList = m_WorkStack[m_iCurPriority];
		for (size_t i(0); i < workList.size(); ++ i){
			if (workList[i]->IsMoving()){
				result = true;
				break;
			}
		}
	}
	return result;
}

CECHPWork* CECHPWorkMan::GetRunningWork(int iWorkID)const{
	CECHPWork *result = NULL;
	if (ValidatePriority(m_iCurPriority)){
		const WorkList &workList = m_WorkStack[m_iCurPriority];
		for (size_t i(0); i < workList.size(); ++ i){
			if (iWorkID == workList[i]->GetWorkID()){
				result = workList[i];
				break;
			}
		}
	}
	return result;
}

CECHPWork* CECHPWorkMan::GetWork(const CECHPWorkMatcher &matcher)const{
	CECHPWork *result = NULL;
	for (int i(NUM_PRIORITY-1); i >= 0; -- i){
		const WorkList &workList = m_WorkStack[i];
		for (size_t j(0); j < workList.size(); ++ j){
			CECHPWork *pWork = workList[j];
			if (matcher(pWork, i, false)){
				result = pWork;
				break;
			}
		}
	}
	if (!result && GetDelayedWork() && matcher(m_Delayed.pWork, m_Delayed.iPriority, true)){
		result = GetDelayedWork();
	}
	return result;
}

CECHPWork* CECHPWorkMan::GetWork(int iWorkID, int iPriority, bool bIncludeDelayedWork)const{
	return GetWork(CECHPWorkGeneralMatcher(iWorkID, iPriority, bIncludeDelayedWork));
}

bool CECHPWorkMan::HasDelayedWork()const{
	return GetDelayedWork() != NULL;
}

bool CECHPWorkMan::HasWorkOnPriority(int iPriority)const{
	return ValidatePriority(iPriority) && !m_WorkStack[iPriority].empty();
}

bool CECHPWorkMan::HasWorkRunningOnPriority(int iPriority)const{
	return HasWorkOnPriority(iPriority);
}

bool CECHPWorkMan::CanStartWork(int iWorkID, int iPriority /* = PRIORITY_1 */)const{
	if (!ValidatePriority(iPriority)){
		ASSERT(false);
		return false;
	}
	if (GetWork(iWorkID)){
		return false;
	}
	if (!HasWorkOnPriority(iPriority)){
		return true;
	}
	const WorkList &workList = m_WorkStack[iPriority];
	for (size_t i(0); i < workList.size(); ++ i){
		if (!workList[i]->CanTransferTo(CECHPWork::GetWorkMask(iWorkID))){
			return false;
		}
	}
	return true;
}

void CECHPWorkMan::KillWork(int iPriority, int index){
	if (!ValidatePriority(iPriority)){
		ASSERT(false);
		return;
	}
	WorkList &workList = m_WorkStack[iPriority];
	if (index < 0 || index >= workList.size()){
		ASSERT(false);
		return;
	}
	CECHPWork *pWork = workList[index];
	KillWork(pWork);
	LOG_DEBUG_INFO(AString().Format("CECHPWork::%s priority=%d killed", pWork->GetWorkName(), iPriority));
	delete pWork;
	workList.erase(workList.begin() + index);
}

void CECHPWorkMan::StartAwaitingWorks(){
	if (ValidatePriority(m_iCurPriority)){
		for (-- m_iCurPriority; m_iCurPriority >= 0; -- m_iCurPriority){
			WorkList &workList = m_WorkStack[m_iCurPriority];
			if (workList.empty()){
				continue;
			}
			for (size_t j(0); j < workList.size(); ++ j){
				workList[j]->OnWorkShift();
				LOG_DEBUG_INFO(AString().Format("CECHPWork::%s started by decrease priority, priority=%d", workList[j]->GetWorkName(), m_iCurPriority));
			}
			break;
		}
	}
	StartDelayedWork();
}

void CECHPWorkMan::Tick(DWORD dwDeltaTime){
	if (!IsAnyWorkRunning()){
		return;
	}
	WorkList &workList = m_WorkStack[m_iCurPriority];
	for (size_t i(0); i < workList.size();){
		CECHPWork *pWork = workList[i];

		SetPostTickCommand(NULL);
		pWork->Tick(dwDeltaTime);

		if (!m_pPostTickCommand){
			if (!pWork->IsFinished()){
				++ i;
				continue;
			}
			KillWork(m_iCurPriority, i);
		}else{
			m_pPostTickCommand->Run(this);
			SetPostTickCommand(NULL);
			break;	//	不确定 m_pPostTickCommand 执行什么内容，此处跳出
		}
	}
	if (workList.empty()){
		StartAwaitingWorks();
	}
}

void CECHPWorkMan::SetPostTickCommand(CECHPWorkPostTickCommand *command){
	delete m_pPostTickCommand;
	m_pPostTickCommand = command;
}

//	class CECHPWorkPostTickRunWorkCommand
CECHPWorkPostTickRunWorkCommand::CECHPWorkPostTickRunWorkCommand(CECHPWork *pWork, bool bNoDelay/* =false */, int iPriority/* =CECHPWorkMan::PRIORITY_1 */, bool bShouldTick/* =false */, DWORD dwTickTime/* =0 */)
: m_pWork(pWork)
, m_iPriority(iPriority)
, m_bNoDelay(bNoDelay)
, m_bShouldTick(bShouldTick)
, m_dwTickTime(dwTickTime)
{
}

CECHPWorkPostTickRunWorkCommand::~CECHPWorkPostTickRunWorkCommand(){
	if (m_pWork){
		delete m_pWork;
		m_pWork = NULL;
	}
}

bool CECHPWorkPostTickRunWorkCommand::Run(CECHPWorkMan *pWorkMan){
	if (!m_pWork || !pWorkMan){
		return false;
	}
	if (!pWorkMan->StartWork(m_iPriority, m_pWork, m_bNoDelay)){
		m_pWork = NULL;
		return false;
	}
	if (m_bShouldTick){
		m_pWork->Tick(m_dwTickTime);
	}
	m_pWork = NULL;
	return true;
}
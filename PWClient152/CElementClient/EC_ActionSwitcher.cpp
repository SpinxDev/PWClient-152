/*
 * FILE: EC_ActionSwitcher.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD, 2013/11/07
 *
 * HISTORY: 
 *
 * Copyright (c)
 */

#include "EC_ActionSwitcher.h"
#include "EC_HostPlayer.h"
#include "EC_HPWork.h"
#include "EC_HPWorkFall.h"
#include "EC_PetCorral.h"
#include "EC_HPWorkMove.h"

class FlagKeeper
{
public:
	FlagKeeper(CECActionSwitcherBase* pSwitcher):m_pSwitcher(pSwitcher){ pSwitcher->SetPostMessageFlag(false);}
	~FlagKeeper(){ m_pSwitcher->SetPostMessageFlag(true);}
protected:
	CECActionSwitcherBase* m_pSwitcher;
};
//////////////////////////////////////////////////////////////////////////
bool CECActionSequencedRelationFilter::CanAddAction(CECActionContext* pContext,CECActionBase* pAct)
{
	// 如果action队列里空且acton能被执行，则加入
	if (pContext->GetActionCount()==0 && pAct->CanDo())
		return true;
	
	// action 队列里前后顺序关系判断
	CECActionBase* pLastAct = pContext ? pContext->GetLastAction():NULL;
	CECActionSequencedRelationFilter* filter = pLastAct ? pLastAct->GetActionFilter():NULL;
	if (filter && filter->IsPredecessorTo(pAct->GetActionFilter()))
		return true;	
	
	return false;
}
///
CECActionBase::CECActionBase(CECHostPlayer* pHost):
m_pHostPlayer(pHost),
m_iFirstDoRes(FD_NONE),
m_ActionFilter(NULL)
{};
bool CECActionBase::NeedBeRemoved()
{
	return (m_iFirstDoRes != FD_NONE);
}
bool CECActionBase::Update(DWORD dt)
{
	FlagKeeper keeper(m_pHostPlayer->GetActionSwitcher());

	// do once
	if (CanDo() && m_iFirstDoRes == FD_NONE)
		m_iFirstDoRes = DoOnce() ? FD_SUCC:FD_FAIL;		
	
	return true;
}
bool CECActionBase::CanAddTo(CECActionContext* pContext)
{
	return m_ActionFilter && m_ActionFilter->CanAddAction(pContext,this);
}
//////////////////////////////////////////////////////////////////////////

bool CECActionCancelRide::CanDo()
{
//	if(m_pHostPlayer->IsRidingOnPet() && !m_pHostPlayer->IsPlayerMoving())
	if(m_pHostPlayer->IsRidingOnPet() && !m_pHostPlayer->InSlidingState() && !m_pHostPlayer->IsJumping() && m_pHostPlayer->GetMoveMode() != CECHostPlayer::MOVE_FREEFALL)
		return true;
	return false;
}
bool CECActionCancelRide::DoOnce()
{	
	return m_pHostPlayer->RecallPet();	
};

//////////////////////////////////////////////////////////////////////////

bool CECActionFly::CanDo()
{
	return m_pHostPlayer->CanDo(CECHostPlayer::CANDO_FLY);
}
bool CECActionFly::DoOnce()
{
	FlagKeeper keeper(m_pHostPlayer->GetActionSwitcher());

	return m_pHostPlayer->CmdFly();

};

//////////////////////////////////////////////////////////////////////////
bool CECActionLanding::CanDo()
{
	return (m_pHostPlayer->IsFlying() && m_pHostPlayer->CanDo(CECHostPlayer::CANDO_FLY));
}
bool CECActionLanding::DoOnce()
{
	return m_pHostPlayer->CmdFly();
};

//////////////////////////////////////////////////////////////////////////
bool CECActionRide::CanDo()
{
	bool ret = m_pHostPlayer->CanDo(CECHostPlayer::CANDO_SUMMONPET) && !m_pHostPlayer->IsFlying() && !m_pHostPlayer->InSlidingState();
	if(!ret ) return false;

	if (m_pHostPlayer->GetWorkMan()->GetWork(CECHPWork::WORK_FREEFALL))	// 下落过程	
		return false;

	return true;
}
bool CECActionRide::DoOnce()
{
	return m_pHostPlayer->SummonPet(m_index);
};
//////////////////////////////////////////////////////////////////////////

CECActionSkill::CECActionSkill(CECHostPlayer* pHost,int skill,bool bCom,int iSel,int iForceAtk):
CECActionBase(pHost),
m_iSkill(skill),
m_bCommbo(bCom),
m_idSelect(iSel),
m_iForceAtk(iForceAtk)
{
	m_ActionFilter = new CECSkillActionFilter();
}
bool CECActionSkill::CanDo()
{
	return m_pHostPlayer->CanDo(CECHostPlayer::CANDO_SPELLMAGIC) && !m_pHostPlayer->InSlidingState();
}
bool CECActionSkill::DoOnce()
{
	return m_pHostPlayer->ApplySkillShortcut(m_iSkill,m_bCommbo,m_idSelect,m_iForceAtk);
};
//////////////////////////////////////////////////////////////////////////
CECActionUseSkillItem::CECActionUseSkillItem(CECHostPlayer* pHost,int iPack,int iSlot,int tid):
CECActionBase(pHost),
m_iPack(iPack),
m_iSlot(iSlot),
m_tid(tid)
{
	m_ActionFilter = new CECUseSkillItemActionFilter();
}
bool CECActionUseSkillItem::CanDo() // 只做简单判断
{
	return m_pHostPlayer->CanDo(CECHostPlayer::CANDO_USEITEM) && m_pHostPlayer->CanDo(CECHostPlayer::CANDO_SPELLMAGIC)
		&& !m_pHostPlayer->InSlidingState() && m_pHostPlayer->GetSelectedTarget()!=0;
}
bool CECActionUseSkillItem::DoOnce()
{
	return m_pHostPlayer->UseItemInPack(m_iPack,m_iSlot,true);
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
CECActionContext::~CECActionContext()
{
	for (int i=0;i<m_actions.GetSize();i++)
	{
		CECActionBase* pAct = m_actions.GetAt(i);
		if (pAct)
			delete pAct;
	}
	m_actions.RemoveAll();

	if(m_ErrorCtr)
		delete m_ErrorCtr;
}
bool CECActionContext::AddAction(CECActionBase* pAct)
{
	if (pAct && pAct->CanAddTo(this))
	{
		m_actions.Add(pAct);
		return true;
	}
	else
		return false;
}
bool CECActionContext::NeedBeRemoved()
{
	if(m_bForceRemove) return true;

	bool ret = true;
	for (int i=0;i<m_actions.GetSize();i++)
	{
		CECActionBase* pAct = m_actions.GetAt(i);
		if (pAct && !pAct->NeedBeRemoved())
		{
			ret = false;
			break;
		}
	}
	
	return ret;
}
bool CECActionContext::Update(DWORD dt)
{
	for (int i=0;i<m_actions.GetSize();i++)
	{
		CECActionBase* pAct = m_actions.GetAt(i);
		if (pAct)
			pAct->Update(dt);		
	}

	if (m_ErrorCtr)	
		m_ErrorCtr->Update(dt);	

	return true;
}
bool CECActionContext::CanBeAddedTo(CECActionSwitcherBase* pSwitcher)
{
	CECActionBase* pAct = GetFirstAction();

	if(pSwitcher->GetContextCount() == 0 && pAct->CanDo())
		return true;
	
	return pAct && pAct->CanAddTo(pSwitcher->GetLastContext());
}
CECActionBase* CECActionContext::GetFirstAction()
{
	if(m_actions.GetSize()>0)
		return m_actions.GetAt(0);

	return NULL;
}
CECActionBase* CECActionContext::GetLastAction()
{
	return GetAction(GetActionCount()-1);
}
CECActionBase* CECActionContext::GetAction(int index)
{
	if (index>=0 && index<GetActionCount())
		return m_actions.GetAt(index);	
	else 
		return NULL;
}

void CECContextValidCheckerForRide::Update(DWORD dt)
{	// 客户端发送上马和下马命令后，马上发送移动消息，服务器的上马、下马会失败，但是没有通知客户端。所有这里需要做一个确认。
	if (m_pActionContext->GetActionCount() && m_pActionContext->NeedCheckValid())
	{
		error_correct += dt;
			
		if (error_correct > MAX_ERRORTIME && !m_pActionContext->GetHostPlayer()->GetWorkMan()->IsOperatingPet())
		{
			m_pActionContext->SetForceRemove(true);
			error_correct = 0;
		}
	}
}

bool CECRideToFlyContext::Init()
{
	CECActionBase* pAct = NULL;
	
	do 
	{
		pAct = new CECActionCancelRide(m_pHostPlayer);			
		if(!AddAction(pAct))
			break;
		
		pAct = new CECActionFly(m_pHostPlayer);	
		if(!AddAction(pAct))
			break;

		m_ErrorCtr = new CECContextValidCheckerForRide(this);

		return true;

	} while (0);
	
	if(pAct)
		delete pAct;

	return false;
}
bool CECRideToFlyContext::NeedCheckValid()
{
	return GetActionCount()>1 && GetAction(0)->NeedBeRemoved() && !GetAction(1)->NeedBeRemoved();
}
bool CECRideToSkillContext::Init()
{
	CECActionBase* pAct = NULL;
	
	do 
	{
		pAct = new CECActionCancelRide(m_pHostPlayer);			
		if(!AddAction(pAct))
			break;
		
		pAct = new CECActionSkill(m_pHostPlayer,m_skill,m_bCom,m_iSel,m_iForceAtk);			
		if(!AddAction(pAct))
			break;	
		
		m_ErrorCtr = new CECContextValidCheckerForRide(this);

		return true;
	} while (0);

	if(pAct)
		delete pAct;

	return false;
}
bool CECRideToSkillContext::NeedCheckValid()
{
	return GetActionCount()>1 && GetAction(0)->NeedBeRemoved() && !GetAction(1)->NeedBeRemoved();
}
bool CECFlyToRideContext::Init()
{
	CECActionBase* pAct = NULL;
	
	do 
	{
		pAct = new CECActionLanding(m_pHostPlayer);
		if(!AddAction(pAct))
			break;
		
		pAct = new CECActionRide(m_pHostPlayer,m_iPetIndex);		
		if(!AddAction(pAct))
			break;

		return true;
	} while (0);

	if(pAct)
		delete pAct;

	return false;
}
bool CECRideToUseTargetItemContext::Init()
{
	CECActionBase* pAct = NULL;
	
	do 
	{
		pAct = new CECActionCancelRide(m_pHostPlayer);			
		if(!AddAction(pAct))
			break;
	
		pAct = new CECActionUseSkillItem(m_pHostPlayer,m_iPack,m_iSlot,m_tid);
		if(!AddAction(pAct))
			break;

		m_ErrorCtr = new CECContextValidCheckerForRide(this);
	
		return true;

	} while (0);

	if(pAct)
		delete pAct;

	return false;
}
bool CECRideToUseTargetItemContext::NeedCheckValid()
{
	return GetActionCount()>1 && GetAction(0)->NeedBeRemoved() && !GetAction(1)->NeedBeRemoved();
}
//////////////////////////////////////////////////////////////////////////

void CECActionSwitcherBase::Clear()
{
	ALISTPOSITION pos = m_actionContexts.GetHeadPosition();
	while (pos)
	{
		ALISTPOSITION posCur = pos;
		CECActionContext* pContext = m_actionContexts.GetNext(pos);
		
		if(pContext)
		{
			delete pContext;
			m_actionContexts.RemoveAt(posCur);		
		}
	}
	m_actionContexts.RemoveAll();
}
CECActionContext* CECActionSwitcherBase::GetContext(int index)
{
	if (index>=0 && index<GetContextCount())
		return m_actionContexts.GetByIndex(index);	
	else 
		return NULL;
}
CECActionContext* CECActionSwitcherBase::GetLastContext()
{
	return GetContext(GetContextCount()-1);
}
void CECActionSwitcherBase::PostMessge(int msg)
{
	if (CanAddMessage())	
		m_msgs.UniquelyAdd(msg);	
}
//////////////////////////////////////////////////////////////////////////
bool CECActionSwitcher::AddActionContext(CECActionContext* pContext)
{
	if (pContext && pContext->CanBeAddedTo(this))
	{
		m_actionContexts.AddTail(pContext);
		return true;
	}
	else
		return false;
}

bool CECActionSwitcher::OnRideToFlyAction()
{
	if(!m_pHostPlayer->PlayerLimitTest(CECHostPlayer::PLAYER_LIMIT_NOFLY)) return false;

	if (!m_pHostPlayer->IsRidingOnPet())
		return false;

	
	// 如果当前处于自动飞行状态，则先停止移动
	if (CECHPWork* pWork = m_pHostPlayer->GetWorkMan()->GetRunningWork(CECHPWork::WORK_MOVETOPOS)){
		CECHPWorkMove* move = dynamic_cast<CECHPWorkMove*>(pWork);
		if (move && move->IsAutoFly())
		{
			m_pHostPlayer->NaturallyStopMoving();
		}
	}

	CECActionContext* context = new CECRideToFlyContext(m_pHostPlayer);
	if (context->Init() && context->CanBeAddedTo(this))	
		m_actionContexts.AddTail(context);	
	else
		delete context;

	return true;
}
bool CECActionSwitcher::OnRideToSkillAction(int skill,bool bCom,int iSel,int iForceAtk)
{
	if (!m_pHostPlayer->IsRidingOnPet())
		return false;

	CECActionContext* context = new CECRideToSkillContext(m_pHostPlayer,skill,bCom,iSel,iForceAtk);
	if (context->Init() && context->CanBeAddedTo(this))
		m_actionContexts.AddTail(context);
	else
		delete context;

	return true;
}	
bool CECActionSwitcher::OnFlyToRideAction(int petIndex)
{
	if(!m_pHostPlayer->PlayerLimitTest(CECHostPlayer::PLAYER_LIMIT_NOMOUNT)) return false;

	CECPetCorral* corral = m_pHostPlayer->GetPetCorral();
	if (!corral)
		return false;

	CECPetData* pPet = corral->GetPetData(petIndex);
	if (!pPet || !pPet->IsMountPet() || !m_pHostPlayer->IsFlying())
		return false;

	CECActionContext* context = new CECFlyToRideContext(m_pHostPlayer,petIndex);
	if (context->Init() && context->CanBeAddedTo(this))	
		m_actionContexts.AddTail(context);	
	else
		delete context;

	return true;
}
bool CECActionSwitcher::OnRideToUseTargetItem(int iPack,int iSlot, int tid)
{
	if (!m_pHostPlayer->IsRidingOnPet())
		return false;
	
	CECActionContext* context = new CECRideToUseTargetItemContext(m_pHostPlayer,iPack,iSlot,tid);
	if (context->Init() && context->CanBeAddedTo(this))		
		m_actionContexts.AddTail(context);		
	else
		delete context;

	return true;
}

void CECActionSwitcher::Tick(DWORD dt)
{
	ProcessMessage();

	ALISTPOSITION pos = m_actionContexts.GetHeadPosition();
	while (pos)
	{
		ALISTPOSITION posCur = pos;
		CECActionContext* pContext = m_actionContexts.GetNext(pos);
		
		if(pContext)
		{
			pContext->Update(dt);
			if (pContext->NeedBeRemoved())
			{
				delete pContext;
				m_actionContexts.RemoveAt(posCur);
			}			
		}
		else
			m_actionContexts.RemoveAt(posCur);
	}
}
void CECActionSwitcher::ProcessMessage()
{
	for (int i=0;i<m_msgs.GetSize();i++)
	{
		int msg = m_msgs.GetAt(i);
		switch(msg)
		{
		case MSG_FLY:
			OnFly();
			break;
		case MSG_MOUNTPET:
			OnRide();
			break;
		case MSG_CASTSKILL:
			OnCastSkill();
			break;
		}
	}

	m_msgs.RemoveAll();
}
void CECActionSwitcher::OnFly()
{
	RemoveRideFlyRelatedContext();
}
void CECActionSwitcher::OnRide()
{
	RemoveRideFlyRelatedContext();
}
void CECActionSwitcher::OnCastSkill()
{
	RemoveRideFlyRelatedContext();
}

void CECActionSwitcher::RemoveRideFlyRelatedContext()
{
	ALISTPOSITION pos = m_actionContexts.GetHeadPosition();
	while (pos)
	{
		ALISTPOSITION posCur = pos;
		CECActionContext* pContext = m_actionContexts.GetNext(pos);
		if (pContext)
		{
			bool bRelated = pContext->IsContext(CECActionContext::AC_FLYTORIDE)
				|| pContext->IsContext(CECActionContext::AC_RIDETOSKILL)
				|| pContext->IsContext(CECActionContext::AC_RIDETOFLY)
				|| pContext->IsContext(CECActionContext::AC_RIDETOUSETARGETITEM);

			if( bRelated)
			{
				delete pContext;
				m_actionContexts.RemoveAt(posCur);			
			}
		}		
	}
}
//////////////////////////////////////////////////////////////////////////
//  

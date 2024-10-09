/*
* FILE: CECPlayerWrapper.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Shizhenhua, 2013/8/21
*
* HISTORY: 
*
* Copyright (c) 2011 ACE Studio, All Rights Reserved.
*/


#include "EC_Global.h"
#include "EC_PlayerWrapper.h"
#include "EC_AutoPolicy.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_Inventory.h"
#include "EC_Manager.h"
#include "EC_World.h"
#include "EC_ManPlayer.h"
#include "EC_ManMatter.h"
#include "EC_ManNPC.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrArrow.h"
#include "EC_Matter.h"
#include "EC_NPC.h"
#include "EC_ElsePlayer.h"
#include "EC_GPDataType.h"
#include "EC_ComboSkill.h"
#include "EC_Skill.h"

#include <A3DFuncs.h>


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

static const int MAX_ATTACK_ERROR = 3;
static const int INVALIDOBJ_TIMEOUT = 30000;
static const float MATTER_SEARCH_RANGE = 60.0f;


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

struct ObjectSorter
{
	bool operator () (CECObject* p1, CECObject* p2)
	{
		float f1 = GetDistToHost(p1);
		float f2 = GetDistToHost(p2);

		if( fabs(f1 - f2) > 0.00001f )
			return f1 < f2;
		else
		{
			if( p1->GetClassID() == p2->GetClassID() )
				return p1 < p2;
			else
				return p1->IsMatter();
		}
	}

	// 获取与玩家的距离
	float GetDistToHost(CECObject* p)
	{
		if( !p ) return 0.0f;
		if( p->IsMonsterNPC() )
		{
			CECNPC* pNPC = dynamic_cast<CECNPC*>(p);
			return pNPC ? pNPC->GetDistToHost() : 0.0f;
		}
		else if( p->IsMatter() )
		{
			CECMatter* pMatter = dynamic_cast<CECMatter*>(p);
			return pMatter ? pMatter->GetDistToHost() : 0.0f;
		}
		else
		{
			ASSERT(0);
			return 0.0f;
		}
	}
};

// 响应自动策略行为的一些事件
void AP_ActionEvent(int iEvent, int iParam)
{
	if( !CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
		return;

	CECPlayerWrapper* pWrapper = CECAutoPolicy::GetInstance().GetPlayerWrapper();
	if( pWrapper ) pWrapper->OnActionEvent(iEvent, iParam);

#ifdef _SHOW_AUTOPOLICY_DEBUG
	a_LogOutput(1, "AP_ActionEvent: %s, Param = %d", AP_GetEventName(iEvent), iParam);
//	g_pGame->RuntimeDebugInfo(0xff0000ff, _AL("AP_ActionEvent: %s, Param = %d"), EVENT_NAME[iEvent], iParam);
#endif
}

const char* AP_GetActionName(int iAction)
{
#define CONVERT_TYPE_TO_TEXT(iAction) case CECPlayerWrapper::##iAction: return #iAction;

	switch(iAction)
	{
		CONVERT_TYPE_TO_TEXT(ACTION_IDLE);
		CONVERT_TYPE_TO_TEXT(ACTION_MOVE);
		CONVERT_TYPE_TO_TEXT(ACTION_CASTSKILL);
		CONVERT_TYPE_TO_TEXT(ACTION_USEITEM);
		CONVERT_TYPE_TO_TEXT(ACTION_PICKUP);
		CONVERT_TYPE_TO_TEXT(ACTION_COMBOSKILL);
		CONVERT_TYPE_TO_TEXT(ACTION_MELEE);
		default: return "Unknown";
	}

#undef CONVERT_TYPE_TO_TEXT
}

const char* AP_GetEventName(int iEvent)
{
#define CONVERT_TYPE_TO_TEXT(iEvent) case iEvent: return #iEvent;

	switch(iEvent)
	{
		CONVERT_TYPE_TO_TEXT(AP_EVENT_CANNOTMOVE);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_MOVEFINISHED);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_TRACEOK);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_STARTSKILL);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_STOPSKILL);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_STARTUSEITEM);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_STOPUSEITEM);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_PICKUPOK);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_CANNOTPICKUP);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_STARTMELEE);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_STOPMELEE);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_COMBOCONTINUE);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_COMBOFINISH);
		CONVERT_TYPE_TO_TEXT(AP_EVENT_MELEEOUTOFRANGE);
		default: return "Unknown";
	}
	
#undef CONVERT_TYPE_TO_TEXT
}


///////////////////////////////////////////////////////////////////////////
//  
//  Implement Action classes
//  
///////////////////////////////////////////////////////////////////////////

class IdleAction : public CECPlayerWrapper::Action
{
protected:
	int m_iTime;

public:
	IdleAction(CECPlayerWrapper* pPlayer, int iTime)
		: Action(pPlayer, CECPlayerWrapper::ACTION_IDLE), m_iTime(iTime)
	{
	}

	bool Tick(DWORD dwDeltaTime)
	{
		if( HaveNextAction() )
			return true;

		if( m_iTime > 0 )
		{
			m_iTime -= dwDeltaTime;
			return m_iTime <= 0;
		}

		return true;
	}
};

class MoveAction : public CECPlayerWrapper::Action
{
protected:
	bool m_bFinish;
	int m_iTimeOut;

public:
	MoveAction(CECPlayerWrapper* pPlayer)
		: Action(pPlayer, CECPlayerWrapper::ACTION_MOVE), m_bFinish(false)
	{
		m_iTimeOut = 30000;
	}

	bool CanBreak() const
	{
		return false;
	}

	bool Tick(DWORD dwDeltaTime)
	{
		m_iTimeOut -= dwDeltaTime;
		if( m_iTimeOut <= 0 )
			return true;

		return m_bFinish;
	}

	void OnEvent(int iEvent, int iParam)
	{
		if( iEvent == AP_EVENT_CANNOTMOVE ||
			iEvent == AP_EVENT_MOVEFINISHED )
			m_bFinish = true;
	}
};

class CastSkillAction : public CECPlayerWrapper::Action
{
	enum { TRACE, SPELL };

protected:
	int m_iStep;
	bool m_bFinish;
	int m_iTimeOut;
	bool m_bSuccess;

public:
	CastSkillAction(CECPlayerWrapper* pPlayer)
		: Action(pPlayer, CECPlayerWrapper::ACTION_CASTSKILL), m_bFinish(false)
	{
		m_iStep = TRACE;
		m_iTimeOut = 30000;
		m_bSuccess = false;
	}

	void EndAction()
	{
		// 施法成功
		if( m_bSuccess )
			host->ResetAttackError();
	}

	bool CanBreak() const
	{
		return m_iStep == TRACE && !host->GetHostPlayer()->IsPlayerMoving();
	}

	bool Tick(DWORD dwDeltaTime)
	{
		m_iTimeOut -= dwDeltaTime;
		if( m_iTimeOut <= 0 )
			return true;

		return m_bFinish;
	}

	void OnEvent(int iEvent, int iParam)
	{
		if( m_iStep == TRACE )
		{
			if( iEvent == AP_EVENT_TRACEOK )
			{
				m_iStep = SPELL;
				m_iTimeOut = 2000;	// 发出技能，等待服务器回应（2秒超时）
			}
			else if( iEvent == AP_EVENT_MOVEFINISHED )
			{
				// 追踪失败，没有放出技能，终止
				m_bFinish = true;
			}
		}
		else if( m_iStep == SPELL )
		{
			if( iEvent == AP_EVENT_STARTSKILL )
			{
				// 设超时时间为技能执行时间的2倍
				m_iTimeOut = iParam * 2;

				// 开始吟唱，技能施放成功
				m_bSuccess = true;
			}
			else if( iEvent == AP_EVENT_STOPSKILL )
			{
				// 技能成功结束
				m_bFinish = true;
			}
		}
	}
};

class UseItemAction : public CECPlayerWrapper::Action
{
protected:
	bool m_bFinish;
	int m_iTimeOut;

public:
	UseItemAction(CECPlayerWrapper* pPlayer)
		: Action(pPlayer, CECPlayerWrapper::ACTION_USEITEM), m_bFinish(false)
	{
		m_iTimeOut = 2000;
	}

	bool CanBreak() const
	{
		return false;
	}

	bool Tick(DWORD dwDeltaTime)
	{
		m_iTimeOut -= dwDeltaTime;
		if( m_iTimeOut <= 0 )
			return true;

		return m_bFinish;
	}

	void OnEvent(int iEvent, int iParam)
	{
		if( iEvent == AP_EVENT_STARTUSEITEM )
		{
			// 开始使用物品，设超时时间为吟唱时间的2倍
			m_iTimeOut = iParam * 2;
		}
		else if( iEvent == AP_EVENT_STOPUSEITEM )
		{
			// 物品使用结束
			m_bFinish = true;
		}
	}
};

class PickupAction : public CECPlayerWrapper::Action
{
	enum { TRACE, PICKUP };

protected:
	int m_iStep;
	bool m_bFinish;
	int m_iTimeOut;
	bool m_bSuccess;
	int m_iMatterID;

public:
	PickupAction(CECPlayerWrapper* pPlayer, int iMatterID)
		: Action(pPlayer, CECPlayerWrapper::ACTION_PICKUP), m_bFinish(false), m_iMatterID(iMatterID)
	{
		m_iTimeOut = 30000;
		m_iStep = TRACE;
		m_bSuccess = false;
	}

	void EndAction()
	{
		// 捡取成功
		if( m_bSuccess )
			host->ResetPickupError();
	}

	bool CanBreak() const
	{
		return m_iStep == TRACE && !host->GetHostPlayer()->IsPlayerMoving();
	}

	bool Tick(DWORD dwDeltaTime)
	{
		m_iTimeOut -= dwDeltaTime;
		if( m_iTimeOut <= 0 )
			return true;

		return m_bFinish;
	}

	void OnEvent(int iEvent, int iParam)
	{
		if( m_iStep == TRACE )
		{
			if( iEvent == AP_EVENT_TRACEOK )
			{
				m_iStep = PICKUP;
				m_iTimeOut = 2000;	// 发送捡取命令，等待服务器回应2秒
			}
			else if( iEvent == AP_EVENT_MOVEFINISHED )
			{
				// 追踪失败，没有拣到物品，终止
				m_bFinish = true;
			}
		}
		else if( m_iStep == PICKUP )
		{
			if( iEvent == AP_EVENT_PICKUPOK )
			{
				// 成功捡取物品
				m_bFinish = true;
				m_bSuccess = true;
			}
			else if( iEvent == AP_EVENT_CANNOTPICKUP )
			{
				// 他人物品，捡取失败
				m_bFinish = true;
				host->SetInvalidObject(m_iMatterID);
				host->ResetPickupError();
			}
		}
	}
};

class MeleeAction : public CECPlayerWrapper::Action
{
	enum { TRACE, MELEE };

protected:
	bool m_bFinish;
	int m_iStep;
	int m_iTimeOut;
	bool m_bMeleeing;
	bool m_bLastMeleeStopped;	// 用于超出范围时的追踪，标识上一个Melee行为是否已经停止

public:
	MeleeAction(CECPlayerWrapper* pPlayer)
		: Action(pPlayer, CECPlayerWrapper::ACTION_MELEE), m_bFinish(false)
	{
		m_iStep = TRACE;
		m_iTimeOut = 30000;
		m_bMeleeing = false;
		m_bLastMeleeStopped = true;
	}

	bool CanBreak() const
	{
		return m_iStep == TRACE && !host->GetHostPlayer()->IsPlayerMoving();
	}

	bool Tick(DWORD dwDeltaTime)
	{
		if( !m_bMeleeing )
		{
			m_iTimeOut -= dwDeltaTime;
			if( m_iTimeOut <= 0 )
				return true;

			return m_bFinish;
		}

		// 战斗不结束，Action就不停止
		return false;
	}

	void OnEvent(int iEvent, int iParam)
	{
		if( iEvent == AP_EVENT_TRACEOK )
		{
			if( m_iStep == TRACE )
			{
				// 普攻开始协议发送成功，等待两秒钟
				m_iStep = MELEE;
				m_iTimeOut = 2000;
			}
		}
		else if( iEvent == AP_EVENT_MOVEFINISHED )
		{
			// 追踪失败，停止
			if( m_iStep == TRACE )
				m_bFinish = true;
		}
		else if( iEvent == AP_EVENT_STARTMELEE )
		{
			// 普攻成功，进入敲击状态
			if( m_iStep == MELEE )
			{
				m_bMeleeing = true;
				m_iTimeOut = 10000;
				host->ResetAttackError();
			}
		}
		else if( iEvent == AP_EVENT_STOPMELEE )
		{
			if( !m_bLastMeleeStopped )
			{
				m_bMeleeing = false;
				m_bLastMeleeStopped = true;
				return;
			}

			if( m_iStep == MELEE && m_bMeleeing )
			{
				// 普攻结束
				m_bMeleeing = false;
				m_bFinish = true;
				host->ResetAttackError();
			}
		}
		else if( iEvent == AP_EVENT_MELEEOUTOFRANGE )
		{
			if( !m_bLastMeleeStopped )
			{
				m_bLastMeleeStopped = true;
				return;
			}

			if( m_iStep == MELEE )
			{
				// 怪物超出范围，转入追踪
				m_iStep = TRACE;
				m_iTimeOut = 30000;
				m_bLastMeleeStopped = (iParam == 1);
			}
		}
	}
};

class ComboSkillAction : public CECPlayerWrapper::Action
{
	enum { TRACE, SPELL, MELEE };

protected:
	int m_iState;
	int m_iTimeOut;
	bool m_bFinish;
	bool m_bMeleeing;		// 正在普攻
	bool m_bCasting;		// 正在施法
	bool m_bDelayContinue;	// 对于正在普攻时，延迟推进下一个技能
	bool m_bSelfSkillFlag;	// 标记当前技能为对自己施法
	bool m_bSending;		// 正在发送协议
	bool m_bLastMeleeStopped;	// 用于超出范围时的追踪，标识上一个Melee行为是否已经停止

public:
	ComboSkillAction(CECPlayerWrapper* pPlayer)
		: Action(pPlayer, CECPlayerWrapper::ACTION_COMBOSKILL), m_bFinish(false)
	{
		m_iState = TRACE;
		m_iTimeOut = 30000;
		m_bMeleeing = false;
		m_bDelayContinue = false;
		m_bCasting = false;
		m_bSelfSkillFlag = false;
		m_bSending = false;
		m_bLastMeleeStopped = true;
	}

	bool CanBreak() const
	{
		return !m_bSending && !m_bCasting && !m_bMeleeing && !host->GetHostPlayer()->IsPlayerMoving();
	}

	bool Tick(DWORD dwDeltaTime)
	{
		CECComboSkill* pCombo = host->GetHostPlayer()->GetComboSkill();
		if( !pCombo && !m_bMeleeing )
			return true;

		// 当目标发生改变时，终止
		if( pCombo && !pCombo->IsIgnoreAtkLoop() &&
			pCombo->GetTarget() != host->GetSelectedTarget() &&
			!m_bCasting && !m_bMeleeing )
			return true;

		if( !m_bMeleeing )
		{
			m_iTimeOut -= dwDeltaTime;
			if( m_iTimeOut <= 0 )
				return true;

			return m_bFinish;
		}

		// 必须等普攻结束了，Action才终止
		return false;
	}

	void OnEvent(int iEvent, int iParam)
	{
		if( iEvent == AP_EVENT_COMBOCONTINUE )
		{
			if( m_iState == MELEE )
			{
				// 因为当前正在普攻，所以TraceWork需要延迟处理
				m_bDelayContinue = true;
				m_bSelfSkillFlag = (iParam == 1);
			}
			else
			{
				if( iParam == 1 )
				{
					// 对自己施的法，无追踪过程
					m_iState = SPELL;
					m_iTimeOut = 2000;
					m_bSending = true;
				}
				else
				{
					m_iState = TRACE;
					m_iTimeOut = 30000;
				}
			}
		}
		else if( iEvent == AP_EVENT_TRACEOK )
		{
			if( m_iState == TRACE )
			{
				m_iState = (iParam == 0) ? MELEE : SPELL;
				m_iTimeOut = 2000;
				m_bSending = true;
			}
		}
		else if( iEvent == AP_EVENT_MOVEFINISHED )
		{
			// 追踪失败，停止
			if( m_iState == TRACE )
				m_bFinish = true;
		}
		else if( iEvent == AP_EVENT_STARTMELEE )
		{
			if( m_iState == MELEE )
			{
				m_iTimeOut = 10000;
				m_bMeleeing = true;
				m_bSending = false;
				host->ResetAttackError();
			}
		}
		else if( iEvent == AP_EVENT_STOPMELEE )
		{
			if( !m_bLastMeleeStopped )
			{
				m_bMeleeing = false;
				m_bLastMeleeStopped = true;
				return;
			}

			if( m_iState == MELEE && m_bMeleeing )
			{
				m_bMeleeing = false;
				host->ResetAttackError();

				// 技能成功结束
				CECComboSkill* pCombo = host->GetHostPlayer()->GetComboSkill();
				if( pCombo && pCombo->IsStop() )
					m_bFinish = true;
				else
				{
					// 如果10秒内，组合技都无法推进则停止
					m_iTimeOut = 10000;

					if( m_bDelayContinue )
					{
						m_bDelayContinue = false;
						
						if( m_bSelfSkillFlag )
						{
							m_iState = SPELL;
							m_iTimeOut = 2000;
							m_bSending = true;
						}
						else
						{
							m_iState = TRACE;
							m_iTimeOut = 30000;
						}
					}
				}
			}
		}
		else if( iEvent == AP_EVENT_MELEEOUTOFRANGE )
		{
			// 超出距离StopAttack时
			if( !m_bLastMeleeStopped )
			{
				m_bLastMeleeStopped = true;
				return;
			}

			if( m_iState == MELEE )
			{
				m_iState = TRACE;
				m_iTimeOut = 30000;
				m_bLastMeleeStopped = (iParam == 1);	// iParam == 1, 表示是通过StopAttack来驱动此事件
			}
		}
		else if( iEvent == AP_EVENT_STARTSKILL )
		{
			if( m_iState == SPELL )
			{
				// 设超时时间为技能执行时间的2倍
				m_iTimeOut = iParam * 2;

				// 开始吟唱，技能施放成功
				m_bCasting = true;
				m_bSending = false;
				host->ResetAttackError();
			}
		}
		else if( iEvent == AP_EVENT_STOPSKILL )
		{
			if( m_iState == SPELL && m_bCasting )
			{
				m_bCasting = false;
				host->ResetAttackError();

				// 技能成功结束
				CECComboSkill* pCombo = host->GetHostPlayer()->GetComboSkill();
				if( pCombo && pCombo->IsStop() )
					m_bFinish = true;
				else
				{
					// 如果10秒内，组合技都无法推进则停止
					m_iTimeOut = 10000;
				}
			}
		}
		else if( iEvent == AP_EVENT_COMBOFINISH )
		{
			// 组合技终止
			m_bFinish = true;
		}
	}
};


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class CECPlayerWrapper
//  
///////////////////////////////////////////////////////////////////////////

CECPlayerWrapper::CECPlayerWrapper(CECHostPlayer* pHost)
{
	m_pHost = pHost;
	m_pSession = g_pGame->GetGameSession();
	m_pCurAction = NULL;
	m_iAttackErrCnt = 0;
	m_iPickupErrCnt = 0;
	m_bForceAttack = false;
	m_vOrigPos = m_pHost->GetPos();
}

CECPlayerWrapper::~CECPlayerWrapper()
{
	ClearAction();
}

void CECPlayerWrapper::Tick(DWORD dwDeltaTime)
{
	if( m_pCurAction )
	{
		if( m_pCurAction->Tick(dwDeltaTime) )
		{
			// 开始下一个行为
			EndCurAction();
			StartAction();
		}
	}

	// 处理延迟行为
	if( m_DelayTask.iType != ACTION_IDLE )
		ProcessDelayTask();

	abase::hash_map<int, int>::iterator it = m_InvalidObj.begin();
	while( it != m_InvalidObj.end() )
	{
		it->second -= dwDeltaTime;
		if( it->second <= 0 )
			it = m_InvalidObj.erase(it);
		else
			++it;
	}
}

void CECPlayerWrapper::StopPolicy()
{
	CECAutoPolicy::GetInstance().StopPolicy();
}

void CECPlayerWrapper::OnStopPolicy()
{
	ClearAction();

	m_bForceAttack = false;
	m_iAttackErrCnt = m_iPickupErrCnt = 0;
	m_InvalidObj.clear();
	m_MonsterAttackMe.clear();
}

bool CECPlayerWrapper::HaveAction() const
{
	if( m_pCurAction || m_Actions.size() > 0 )
		return true;
	else
		return false;
}

bool CECPlayerWrapper::AddAction(Action* pAction)
{
	if( !pAction ) return false;
	if( m_pCurAction && m_pCurAction->GetType() == pAction->GetType() )
		return false;

	std::list<Action*>::iterator it;
	for( it=m_Actions.begin();it!=m_Actions.end();++it )
	{
		if( (*it)->GetType() == pAction->GetType() )
			return false;
	}

	m_Actions.push_back(pAction);
	return true;
}

bool CECPlayerWrapper::StartAction()
{
	if( m_pCurAction )
		return true;

	bool ret = false;
	while( m_Actions.size() )
	{
		m_pCurAction = m_Actions.front();
		m_Actions.pop_front();
		
		if( (ret = m_pCurAction->StartAction()) )
		{
#ifdef _SHOW_AUTOPOLICY_DEBUG
			a_LogOutput(1, "CECPlayerWrapper::StartAction, Type: %s!", AP_GetActionName(m_pCurAction->GetType()));
#endif
			break;
		}
		else
			EndCurAction();
	}

	return ret;
}

bool CECPlayerWrapper::EndCurAction()
{
	if( !m_pCurAction )
		return false;

#ifdef _SHOW_AUTOPOLICY_DEBUG
	a_LogOutput(1, "CECPlayerWrapper::EndCurAction, Type:%s!", AP_GetActionName(m_pCurAction->GetType()));
#endif

	m_pCurAction->EndAction();
	delete m_pCurAction;
	m_pCurAction = NULL;
	return true;
}

void CECPlayerWrapper::ClearAction()
{
	if( m_pCurAction )
		EndCurAction();

	std::list<Action*>::iterator it;
	for( it=m_Actions.begin();it!=m_Actions.end();++it )
		delete *it;
	m_Actions.clear();
}

bool CECPlayerWrapper::AddOneAction(int iActionType, int iParam)
{
	switch(iActionType)
	{
	case ACTION_MOVE: AddAction(new MoveAction(this)); break;
	case ACTION_CASTSKILL: AddAction(new CastSkillAction(this)); break;
	case ACTION_USEITEM: AddAction(new UseItemAction(this)); break;
	case ACTION_PICKUP: AddAction(new PickupAction(this, iParam)); break;
	case ACTION_COMBOSKILL: AddAction(new ComboSkillAction(this)); break;
	case ACTION_MELEE: AddAction(new MeleeAction(this)); break;
	default: ASSERT(0); return false;
	}

	return StartAction();
}

bool CECPlayerWrapper::AddIdleAction(int iTime)
{
	AddAction(new IdleAction(this, iTime));
	return StartAction();
}

int CECPlayerWrapper::GetItemIndex(int iPack, int tid)
{
	if( iPack >= IVTRTYPE_PACK && iPack <= IVTRTYPE_TASKPACK )
	{
		CECInventory* pPack = m_pHost->GetPack(iPack);
		return pPack->FindItem(tid);
	}

	return -1;
}

int CECPlayerWrapper::GetItemCount(int iPack, int tid)
{
	if( iPack >= IVTRTYPE_PACK && iPack <= IVTRTYPE_TASKPACK )
	{
		CECInventory* pPack = m_pHost->GetPack(iPack);
		return pPack->GetItemTotalNum(tid);
	}

	return 0;
}

A3DVECTOR3 CECPlayerWrapper::GetPos() const
{
	return m_pHost->GetPos();
}

void CECPlayerWrapper::MoveTo(float x, float z)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if( pGameUI ) pGameUI->AutoMoveStart((int)x, (int)z, false);
	AddOneAction(ACTION_MOVE);
}

void CECPlayerWrapper::CancelAction()
{
	g_pGame->GetGameRun()->PostMessage(MSG_HST_PRESSCANCEL, MAN_PLAYER, 0);
}

bool CECPlayerWrapper::SearchTarget(int& id, int& tid)
{
	int iTarget = 0;
	int iTargetTempl = 0;
	CECNPCMan* pNPCMan = g_pGame->GetGameRun()->GetWorld()->GetNPCMan();
	CECMatterMan* pMatterMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();

	m_iAttackErrCnt = 0;

	// 先选取攻击自己的怪物
	if( m_MonsterAttackMe.size() > 0 )
	{
		float fMinDist = 999999.0f;
		std::set<int>::iterator itD;

		// 选取离自己最近的
		for( std::set<int>::iterator it=m_MonsterAttackMe.begin();it!=m_MonsterAttackMe.end(); )
		{
			CECNPC* pNPC = pNPCMan->GetNPC(*it);
			if( !pNPC )
			{
				m_MonsterAttackMe.erase(it++);
				continue;
			}

			float fDist = a3d_Magnitude(m_pHost->GetPos() - pNPC->GetPos());
			if( fDist < fMinDist )
			{
				fMinDist = fDist;
				iTarget = *it;
				iTargetTempl = pNPC->GetTemplateID();
				itD = it;
			}

			++it;
		}

		if( iTarget )
		{
			m_MonsterAttackMe.erase(itD);
			id = iTarget;
			tid = iTargetTempl;
			return true;
		}
	}

	// 选取最近的目标
	abase::vector<CECObject*> targets;

	APtrArray<CECNPC*> monsters;
	pNPCMan->TabSelectCandidates(0, monsters);
	for(int i=0; i<monsters.GetSize(); i++)
		targets.push_back(monsters[i]);

	abase::vector<CECMatter*> matters;
	abase::vector<CECMatter*>::iterator matIt;
	pMatterMan->FindMattersInRange(MATTER_SEARCH_RANGE, true, matters);
	for(matIt=matters.begin(); matIt!=matters.end(); ++matIt)
		targets.push_back(*matIt);

	if( targets.size() > 0 )
	{
		std::sort(targets.begin(), targets.end(), ObjectSorter());

		abase::vector<CECObject*>::iterator it;
		for( it=targets.begin();it!=targets.end();++it )
		{
			if( (*it)->IsMonsterNPC() )
			{
				CECNPC* pNPC = dynamic_cast<CECNPC*>(*it);
				if( pNPC && NpcCanAttack(pNPC->GetNPCID()) )
				{
					iTarget = pNPC->GetNPCID();
					iTargetTempl = pNPC->GetTemplateID();
					break;
				}
			}
			else if( (*it)->IsMatter() )
			{
				CECMatter* pMatter = dynamic_cast<CECMatter*>(*it);
				if( pMatter && MatterCanPickup(pMatter->GetMatterID(), pMatter->GetTemplateID()) )
				{
					iTarget = pMatter->GetMatterID();
					iTargetTempl = pMatter->GetTemplateID();
					break;
				}
			}
		}

		if( iTarget && iTargetTempl )
		{
			id = iTarget;
			tid = iTargetTempl;
			return true;
		}
	}

	id = tid = 0;
	return false;
}

int CECPlayerWrapper::GetSelectedTarget() const
{
	return m_pHost->GetSelectedTarget();
}

void CECPlayerWrapper::SelectTarget(int iTarget)
{
	m_iAttackErrCnt = 0;
	m_pHost->SelectTarget(iTarget);
}

void CECPlayerWrapper::Unselect()
{
	m_iAttackErrCnt = 0;
	m_pHost->SelectTarget(0);
}

bool CECPlayerWrapper::NpcCanAttack(int nid)
{
	if( m_pHost->AttackableJudge(nid, false) != 1 )
		return false;

	// 连续3次没有攻击成功
	if( m_InvalidObj.find(nid) != m_InvalidObj.end() )
		return false;

	return true;
}

bool CECPlayerWrapper::NormalAttack()
{
	bool bRet = m_pHost->CmdNormalAttack(false, false, 0, m_bForceAttack ? 1 : -1);
	if( bRet )
	{
		AddAttackError();
		AddOneAction(ACTION_MELEE);
	}

	return bRet;
}

bool CECPlayerWrapper::CastComboSkill(int group_id, bool bIgnoreAtkLoop, bool bDelay)
{
	if( bDelay )
	{
		DoDelayTask(ACTION_COMBOSKILL, group_id, bIgnoreAtkLoop ? 1 : 0);
		return true;
	}

	AddOneAction(ACTION_COMBOSKILL, group_id);

	bool bRet = m_pHost->ApplyComboSkill(group_id, bIgnoreAtkLoop, m_bForceAttack ? 1 : -1);
	if( !bRet && m_iAttackErrCnt++ >= 3 )
	{
		m_iAttackErrCnt = 0;
		m_pHost->ClearComboSkill();
	}

	return true;
}

bool CECPlayerWrapper::CastSkill(int skill_id, bool bDelay)
{
	if( bDelay )
	{
		DoDelayTask(ACTION_CASTSKILL, skill_id);
		return true;
	}

	bool bRet = m_pHost->ApplySkillShortcut(skill_id, false, 0, m_bForceAttack ? 1 : -1);
	if( bRet )
	{
		AddAttackError();
		AddOneAction(ACTION_CASTSKILL);

		// 给自己施法不用追踪
		if(	m_pHost->GetPrepSkill() )
			AP_ActionEvent(AP_EVENT_TRACEOK);
	}

	return bRet;
}

void CECPlayerWrapper::UseItem(int iSlot)
{
	bool bRet = m_pHost->UseItemInPack(IVTRTYPE_PACK, iSlot);
	if (bRet) AddOneAction(ACTION_USEITEM);
}

bool CECPlayerWrapper::MatterCanPickup(int mid, int tid)
{
	int iPickMode = CECAutoPolicy::GetInstance().GetConfigData().iAutoPickMode;
	if( iPickMode == 0 || (iPickMode == 2 && !ISMONEYTID(tid)) )
		return false;

	if( !m_pHost->CanTakeItem(tid, 1) )
		return false;

	// 判断是否连续拣了3次
	if( m_InvalidObj.find(mid) != m_InvalidObj.end() )
		return false;

	return true;
}

void CECPlayerWrapper::Pickup(int mid)
{
	bool bRet = m_pHost->PickupObject(mid, false);

	if( m_iPickupErrCnt++ >= 3 )
	{
		m_iPickupErrCnt = 0;
		m_InvalidObj[mid] = INVALIDOBJ_TIMEOUT;
	}

	if( bRet )
		AddOneAction(ACTION_PICKUP, mid);
}

bool CECPlayerWrapper::IsPlayerInSlice(int idPlayer)
{
	CECPlayerMan* pPlayerMan = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	CECPlayer* pPlayer = pPlayerMan->GetPlayer(idPlayer);
	if( pPlayer && !pPlayer->IsDead() )
		return true;
	else
		return false;
}

int CECPlayerWrapper::GetWeaponEndurance() const
{
	CECIvtrWeapon* pWeapon = dynamic_cast<CECIvtrWeapon*>(m_pHost->GetEquipment()->GetItem(EQUIPIVTR_WEAPON));
	if( pWeapon )
	{
		if( pWeapon->IsRangeWeapon() )
		{
			CECIvtrArrow* pArrow = dynamic_cast<CECIvtrArrow*>(m_pHost->GetEquipment()->GetItem(EQUIPIVTR_PROJECTILE));
			if( !pArrow || !pArrow->GetCount() )
				return 0;
		}

		return pWeapon->GetCurEndurance();
	}
	else
		return 0;
}

bool CECPlayerWrapper::IsDead() const
{
	return m_pHost->IsDead();
}

bool CECPlayerWrapper::IsRevivedByOther() const
{
	return m_pHost->GetReviveLostExp() >= 0.0f;
}

void CECPlayerWrapper::AcceptRevive()
{
	m_pSession->c2s_CmdRevivalAgree();
}

bool CECPlayerWrapper::ReviveByItem()
{
	if( !m_pHost->GetCoolTime(GP_CT_SOUL_STONE) )
	{
		m_pSession->c2s_CmdReviveItem();
		return true;
	}

	return false;
}

void CECPlayerWrapper::ReviveInTown()
{
	m_pSession->c2s_CmdReviveVillage();
}

bool CECPlayerWrapper::IsInSanctuary() const
{
	return m_pHost->IsInSanctuary();
}

bool CECPlayerWrapper::IsMonsterAttackMe() const
{
	return m_MonsterAttackMe.size() > 0;
}

void CECPlayerWrapper::OnObjectDisappear(int object_id)
{
	abase::hash_map<int, int>::iterator itO;
	if( (itO = m_InvalidObj.find(object_id)) != m_InvalidObj.end() )
		m_InvalidObj.erase(itO);

	std::set<int>::iterator it;
	if( (it = m_MonsterAttackMe.find(object_id)) != m_MonsterAttackMe.end() )
		m_MonsterAttackMe.erase(it);
}

void CECPlayerWrapper::OnMonsterAttackMe(int monster_id)
{
	if( GetSelectedTarget() != monster_id )
	{
		m_MonsterAttackMe.insert(monster_id);

		abase::hash_map<int, int>::iterator it = m_InvalidObj.find(monster_id);
		if( it != m_InvalidObj.end() )
			m_InvalidObj.erase(it);
	}
}

A3DVECTOR3 CECPlayerWrapper::GetObjectPos(int object_id) const
{
	if( ISPLAYERID(object_id) )
	{
		CECPlayerMan* pMan = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
		CECElsePlayer* pPlayer = pMan->GetElsePlayer(object_id);
		return pPlayer ? pPlayer->GetPos() : A3DVECTOR3(0.0f, 0.0f, 0.0f);
	}
	else if( ISNPCID(object_id) )
	{
		CECNPCMan* pMan = g_pGame->GetGameRun()->GetWorld()->GetNPCMan();
		CECNPC* pNPC = pMan->GetNPC(object_id);
		return pNPC ? pNPC->GetPos() : A3DVECTOR3(0.0f, 0.0f, 0.0f);
	}
	else if( ISMATTERID(object_id) )
	{
		CECMatterMan* pMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
		CECMatter* pMatter = pMan->GetMatter(object_id);
		return pMatter ? pMatter->GetPos() : A3DVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return A3DVECTOR3(0.0f, 0.0f, 0.0f);
}

void CECPlayerWrapper::AddAttackError()
{
	if( m_iAttackErrCnt++ >= MAX_ATTACK_ERROR )
	{
		m_iAttackErrCnt = 0;
		m_InvalidObj[GetSelectedTarget()] = INVALIDOBJ_TIMEOUT;
	}
}

void CECPlayerWrapper::SetInvalidObject(int object_id)
{
	m_InvalidObj[object_id] = INVALIDOBJ_TIMEOUT;
}

void CECPlayerWrapper::OnActionEvent(int iEvent, int iParam)
{
	if( m_pCurAction )
		m_pCurAction->OnEvent(iEvent, iParam);

// 	std::list<Action*>::iterator it;
// 	for( it=m_Actions.begin();it!=m_Actions.end();++it )
// 		(*it)->OnEvent(it);
}

void CECPlayerWrapper::DoDelayTask(int iType, int iParam1 /* = 0 */, int iParam2 /* = 0 */)
{
	m_DelayTask.iType = iType;
	m_DelayTask.iParam1 = iParam1;
	m_DelayTask.iParam2 = iParam2;

	// 如果正在普攻、移动，则停止
	if( m_pHost->IsMeleeing() || m_pHost->IsPlayerMoving() )
		CancelAction();
}

void CECPlayerWrapper::ProcessDelayTask()
{
	if( m_pCurAction && !m_pCurAction->CanBreak() )
		return;

	// 强制清除当前行为
	ClearAction();
	m_iAttackErrCnt = m_iPickupErrCnt = 0;

	switch(m_DelayTask.iType)
	{
	case ACTION_CASTSKILL:
		CastSkill(m_DelayTask.iParam1);
		break;
	case ACTION_COMBOSKILL:
		CastComboSkill(m_DelayTask.iParam1, m_DelayTask.iParam2 == 1);
		break;
	default: ASSERT(0);
	}

	m_DelayTask.iType = ACTION_IDLE;
	m_DelayTask.iParam1 = 0;
	m_DelayTask.iParam2 = 0;
}
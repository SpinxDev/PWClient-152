/*
* FILE: EC_PlayerWrapper.h
*
* DESCRIPTION: 
*
* CREATED BY: Shizhenhua, 2013/8/21
*
* HISTORY: 
*
* Copyright (c) 2011 ACE Studio, All Rights Reserved.
*/


#pragma once

#include <ABaseDef.h>
#include <A3DVector.h>
#include <vector.h>
#include <hashmap.h>

#include <set>
#include <list>


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define MAX_PLAYER_ACTION 10


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECPlayer;
class CECHostPlayer;
class CECGameSession;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

// 自动策略下的一些事件
enum
{
	AP_EVENT_CANNOTMOVE,		// 不能移动
	AP_EVENT_MOVEFINISHED,		// 移动结束
	AP_EVENT_TRACEOK,			// 追踪成功
	AP_EVENT_STARTSKILL,		// 技能开始
	AP_EVENT_STOPSKILL,			// 技能停止
	AP_EVENT_STARTUSEITEM,		// 使用物品开始
	AP_EVENT_STOPUSEITEM,		// 使用物品结束
	AP_EVENT_PICKUPOK,			// 成功捡取物品
	AP_EVENT_CANNOTPICKUP,		// 他人物品，不能捡取
	AP_EVENT_STARTMELEE,		// 开始普攻
	AP_EVENT_STOPMELEE,			// 结束普攻
	AP_EVENT_COMBOCONTINUE,		// 组合技推进
	AP_EVENT_COMBOFINISH,		// 组合技终止，应对最后一个技能没有放出的情况
	AP_EVENT_MELEEOUTOFRANGE,	// 普攻超出距离
};

void AP_ActionEvent(int iEvent, int iParam = 0);

const char* AP_GetActionName(int iAction);
const char* AP_GetEventName(int iEvent);


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECPlayerWrapper
//	
///////////////////////////////////////////////////////////////////////////

class CECPlayerWrapper
{
public:

	enum
	{
		ACTION_IDLE,			// 空闲
		ACTION_MOVE,			// 移动
		ACTION_CASTSKILL,		// 发技能
		ACTION_USEITEM,			// 使用物品
		ACTION_PICKUP,			// 捡拾
		ACTION_COMBOSKILL,		// 组合技
		ACTION_MELEE,			// 普攻
	};

	struct DelayTask
	{
		int iType;
		int iParam1;
		int iParam2;

		DelayTask() : iType(ACTION_IDLE), iParam1(0), iParam2(0) {}
	};

	// 玩家当前执行的行为基类
	// 优先处理行为，行为完成后执行AI逻辑
	class Action
	{
	protected:
		int type;
		CECPlayerWrapper* host;

	public:
		Action(CECPlayerWrapper* pHost, int t) : host(pHost), type(t) {}

		virtual bool StartAction() { return true; }
		virtual void EndAction() {}

		// 返回true, 说明本行为执行完毕
		virtual bool Tick(DWORD dwDeltaTime) { return false; }

		// 响应事件
		virtual void OnEvent(int iEvent, int iParam) {}

		// 是否可中断
		virtual bool CanBreak() const { return true; }

		// 是否还有后续行为
		bool HaveNextAction() const { return host->m_Actions.size() > 0; }

		// 行为类型
		int GetType() const { return type; }
	};

	typedef std::list<Action*> ActionQueue;

	friend class Action;
	friend class CECAutoPolicy;

public:
	CECPlayerWrapper(CECHostPlayer* pHost);
	virtual ~CECPlayerWrapper();

	// 更新逻辑
	void Tick(DWORD dwDeltaTime);

	// 停止策略
	void StopPolicy();

	// 清除所有行为
	void ClearAction();
	// 响应玩家行为的事件
	void OnActionEvent(int iEvent, int iParam);

	// 玩家行为是否已经处理完
	bool HaveAction() const;
	// 添加一个空闲行为
	bool AddIdleAction(int iTime);

	// 获取玩家物品
	int GetItemIndex(int iPack, int tid);
	// 获取指定物品的个数
	int GetItemCount(int iPack, int tid);

	// 获取当前位置
	A3DVECTOR3 GetPos() const;
	// 移动到指定位置
	void MoveTo(float x, float z);
	// 玩家取消当前行为
	void CancelAction();
	// 获取玩家初始位置
	A3DVECTOR3 GetOrigPos() const { return m_vOrigPos; }

	// 搜寻一个目标
	bool SearchTarget(int& id, int& tid);

	// 获取当前选中的目标
	int GetSelectedTarget() const;
	// 选取指定目标
	void SelectTarget(int iTarget);
	// 取消选中
	void Unselect();
	// 判断选取对象能否被攻击
	bool NpcCanAttack(int nid);
	// 发起普攻
	bool NormalAttack();
	// 施放组合技能
	bool CastComboSkill(int group_id, bool bIgnoreAtkLoop, bool bQueue = false);
	// 施放技能
	bool CastSkill(int skill_id, bool bQueue = false);
	// 使用物品
	void UseItem(int iSlot);

	// 判断物品是否可以捡取
	bool MatterCanPickup(int mid, int tid);
	// 捡取物品
	void Pickup(int mid);

	// 判断玩家是否已离开视野
	bool IsPlayerInSlice(int idPlayer);
	// 获取武器持久
	int GetWeaponEndurance() const;
	// 玩家是否死亡
	bool IsDead() const;
	// 是否被其他玩家施法相救
	bool IsRevivedByOther() const;
	// 接受他人复活
	void AcceptRevive();
	// 使用复活卷轴复活
	bool ReviveByItem();
	// 回城复活
	void ReviveInTown();
	// 设置强行攻击标记
	void SetForceAttack(bool bFlag) { m_bForceAttack = bFlag; }
	// 是否在安全区内
	bool IsInSanctuary() const;
	// 有正在攻击自己的怪
	bool IsMonsterAttackMe() const;

	// 获取物体的位置
	A3DVECTOR3 GetObjectPos(int object_id) const;

	// 设置玩家的初始位置
	void SetOrigPos(const A3DVECTOR3& vPos) { m_vOrigPos = vPos; }

	// 设置指定的目标无效
	void SetInvalidObject(int object_id);

	///////////////////////////////////////////////////////////////////////////

	// 响应NPC或Matter消失
	void OnObjectDisappear(int object_id);
	// 响应怪物攻击自己
	void OnMonsterAttackMe(int monster_id);

	void AddAttackError();
	void ResetAttackError() { m_iAttackErrCnt = 0; }
	void ResetPickupError() { m_iPickupErrCnt = 0; }
	int GetAttackError() const { return m_iAttackErrCnt; }
	int GetPickupError() const { return m_iPickupErrCnt; }

	void OnStopPolicy();

	CECHostPlayer* GetHostPlayer() { return m_pHost; }
	CECGameSession* GetGameSession() { return m_pSession; }

protected:
	CECHostPlayer* m_pHost;
	CECGameSession* m_pSession;
	ActionQueue m_Actions;
	Action* m_pCurAction;
	A3DVECTOR3 m_vOrigPos;				// 玩家进入世界的位置

	bool m_bForceAttack;				// 强行攻击的标记
	int m_iAttackErrCnt;
	int m_iPickupErrCnt;
	abase::hash_map<int, int> m_InvalidObj;	// 不可攻击的怪、或不可捡取的物品
	std::set<int> m_MonsterAttackMe;	// 攻击过我的怪

	DelayTask m_DelayTask;				// 延迟任务，用于处理辅助技能的施放

private:
	bool AddAction(Action* pAction);
	bool StartAction();
	bool EndCurAction();
	bool AddOneAction(int iActionType, int iParam = 0);

	// 延迟任务
	void DoDelayTask(int iType, int iParam1 = 0, int iParam2 = 0);
	void ProcessDelayTask();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

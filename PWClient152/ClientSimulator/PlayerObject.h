/*
 * FILE: PlayerObject.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once


#include <ABaseDef.h>
#include <vector.h>
#include <hashmap.h>
#include <AAssist.h>
#include <A3DVector.h>
#include "EC_GPDataType.h"
#include <list>
#include "LuaFunc.h"

///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define PLAYER_STATE_COUNT 4


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

class AIObject;
class PlayerObject;
class GameObject;


///////////////////////////////////////////////////////////////////////////
//  
//  玩家行为的定义
//  
///////////////////////////////////////////////////////////////////////////

#define MAX_PLAYER_TASK 10

enum TASK_TYPE
{
	TASK_TYPE_IDLE = 0,
	TASK_TYPE_MOVE,
	TASK_TYPE_NORMAL_ATTACK,
	TASK_TYPE_CAST_SKILL,
};

///////////////////////////////////////////////////////////////////////////

class task_base
{
protected:
	PlayerObject* _imp;
	int _counter;

	task_base() : _imp(NULL), _counter(-1) {}

public:
	task_base(PlayerObject* imp, int counter=-1) : _imp(imp), _counter(counter) {}

	virtual bool StartTask() = 0;
	virtual void EndTask() {}
	virtual bool Tick(int tick)
	{
		if( _counter > 0 )
		{
			_counter -= tick;
			return _counter > 0;
		}

		return true;
	}

	virtual int GetTaskType() = 0;
};

///////////////////////////////////////////////////////////////////////////

class task_idle : public task_base
{
protected:
	task_idle() {}

public:
	task_idle(PlayerObject* imp, int counter) : task_base(imp, counter) {}

	virtual bool StartTask() { return true; };
	virtual bool Tick(int tick);
	virtual int GetTaskType() { return TASK_TYPE_IDLE; }
};

///////////////////////////////////////////////////////////////////////////

class task_move : public task_base
{
protected:
	A3DVECTOR3 _dest;
	int	_time;
	float _speed;
	char _mode;

	task_move() {}

public:
	task_move(PlayerObject* imp, const A3DVECTOR3& dest, int time, float speed, char mode)
		: task_base(imp, time), _dest(dest), _time(time), _speed(speed), _mode(mode)
	{ ASSERT(time >= 100); }

	virtual bool StartTask();
	virtual void EndTask() {}
	virtual int GetTaskType() { return TASK_TYPE_MOVE; }
};

///////////////////////////////////////////////////////////////////////////

class task_stop_move : public task_move
{
protected:
	task_stop_move() {}

public:
	task_stop_move(PlayerObject* imp, const A3DVECTOR3&dest, int time, float speed, char mode)
		: task_move(imp, dest, time, speed, mode)
	{ ASSERT(time >= 100); }

	virtual bool StartTask();
	virtual void EndTask() {}
	virtual int GetTaskType() { return TASK_TYPE_MOVE; }
};

///////////////////////////////////////////////////////////////////////////

class task_normal_attack : public task_base
{
protected:
	task_normal_attack() {}

public:
	task_normal_attack(PlayerObject* imp) : task_base(imp,-1) {}

	virtual bool StartTask() { return true; }
	virtual void EndTask() {}
	virtual int GetTaskType() { return TASK_TYPE_NORMAL_ATTACK; }
};

///////////////////////////////////////////////////////////////////////////

class task_cast_skill : public task_base
{
protected:
	task_cast_skill() {}

public:
	task_cast_skill(PlayerObject* imp) : task_base(imp,-1) {}

	virtual bool StartTask() { return true; }
	virtual void EndTask() {}
	virtual int GetTaskType() { return TASK_TYPE_CAST_SKILL; }
};


///////////////////////////////////////////////////////////////////////////
//  
//  class PlayerObject
//  
///////////////////////////////////////////////////////////////////////////

class PlayerObject
{
public:

	enum PackType
	{
		PACK_INVENTORY,
		PACK_EQUIPMENTS,
		PACK_TASK_INVENTORY,
	};

	struct ITEM
	{
		int tid;
		size_t count;

		ITEM() : tid(0), count(0) {}
	};

	struct SKILL
	{
		int id;
		int level;
		int mp_cost;	// 消耗MP
		float range;	// 范围

		SKILL() : id(0), level(0), mp_cost(0), range(0.0f) {}
	};

	struct PET_DATA
	{
		char name[16];		// 宠物的名字
		int pet_class;		// 宠物的类型
		short level;		// 宠物的等级
	};

	typedef abase::vector<ITEM> UserPackage;	// 玩家包裹
	typedef abase::vector<SKILL> SkillList;		// 技能列表
	typedef abase::vector<PET_DATA> PetList;	// 宠物列表

public:
	PlayerObject( GameObject* pGame );
	virtual ~PlayerObject();

	bool Init(const S2C::cmd_self_info_1* info);
	void Tick();

	///////////////////////////////////////////////////////////////////////////
	// 获取玩家基本信息

	int GetRoleID() { return m_iRoleID; }
	int GetProfession();
	int GetLevel() { return m_iLevel; }
	int GetLevel2() { return m_iLevel2; }
	bool IsDead() { return m_bIsDead; }
	const A3DVECTOR3& GetCurPos() { return m_vPos; }
	BYTE GetDirection() { return m_byDir; }
	void SetDirection(BYTE byDir) { m_byDir = byDir; }
	float GetRunSpeed() { return m_fRunSpeed; }
	int GetSelectedTarget() { return m_iSelectTarget; }
	char GetForceAttack() { return m_cForceAttack; }
	void SetForceAttack(bool bForce);
	bool IsMale() const;
	const wchar_t* GetRoleName();

	int GetHP() { return m_iHP; }
	int GetMaxHP() { return m_iMaxHP; }
	int GetMP() { return m_iMP; }
	int GetMaxMP() { return m_iMaxMP; }
	
	// 获取玩家金币数
	size_t GetMoney() { return m_nMoney; }

	// 当前经验
	int GetExp() { return m_iExp; }
	// 技能点
	int GetSP() { return m_iSP; }

	// 选取待捡的物品
	void SelectMatter(int id) { ASSERT(ISMATTERID(id)); m_iSelectTarget = id; }

	// 是否正在变形中
	bool IsChangingFace() const { return m_bChangingFace; }
	// 是否是时装模式
	bool IsInFashion() const { return m_bInFashion; }

	///////////////////////////////////////////////////////////////////////////
	// 技能相关

	// 随机选取一个可用的攻击技能
	int SelectAttackSkill(float& sqr_range);

	int GetSkillNum() { return m_Skills.size(); }
	SKILL* GetSkill(size_t idx) { return idx >= m_Skills.size() ? NULL : &m_Skills[idx]; }
	SKILL* GetSkillByID(int id);

	///////////////////////////////////////////////////////////////////////////
	// 玩家行为相关

	// 更改玩家AI策略
	void SetAIPolicy( const char* name );

	// 执行AI命令
	void AICommand( StringVector& cmd );

	bool AddIdleTask(int iTime);
	bool AddMoveTask(const A3DVECTOR3& vDest, int time, float speed, char mode);
	bool AddStopMoveTask(const A3DVECTOR3& vDest, int time, float speed, char mode);
	bool AddAttackTask();
	bool AddCastSkillTask();

	bool HasNextTask() { return !m_Tasks.empty(); }
	bool IsCurTaskEmpty() { return m_pCurTask == NULL; }
	void ClearTask();

	void MoveAgentLearn(const A3DVECTOR3& vPos);
	bool GetNextMovePos(const A3DVECTOR3& vPos, const A3DVECTOR3& vDest, float fRange, A3DVECTOR3& vNext);
	bool GetNextMovePos(const A3DVECTOR3& vPos, BYTE dir, float fRange, A3DVECTOR3& vNext);

	// 是否正在移动
	bool IsMoving() { return m_bIsMoving; }

	///////////////////////////////////////////////////////////////////////////
	// 包裹相关

	int GetItemCount(PackType pack, int tid);
	int GetItemIndex(PackType pack, int tid);
	bool IsEquipExist(size_t index);
	bool IsEquipExist(int tid, size_t index);
	bool IsItemExist(int tid);
	int GetEmptySlot();

	///////////////////////////////////////////////////////////////////////////
	// 目标对象

	bool MatterCanPickup(int tid, size_t& pile_limit);
	bool MatterCanGather(int tid);
	bool NpcCanAttack(int tid);
	bool PlayerCanAttack(int id);

	///////////////////////////////////////////////////////////////////////////
	// 宠物相关

	size_t GetPetCount() const { return m_PetData.size(); }
	const PET_DATA* GetPetData(size_t idx) const { return idx < m_PetData.size() ? &m_PetData[idx] : NULL; }

	///////////////////////////////////////////////////////////////////////////
	// C2S Command

	void SendGameData(void* buf, size_t size);
	void SendDebugCmd(short cmd);
	void SendDebugCmd(short cmd, int param);
	void SendDebugCmd(short cmd, int param1, int param2);
	void SendDebugCmd(short cmd, void* buf, size_t sz);
	void Goto(float x, float z);
	void RandomMove();			// 在规定区域内随机瞬移
	void PlayerMove(A3DVECTOR3& pos, int time, float speed, char mode);
	void PlayerStopMove(A3DVECTOR3& pos, int time, float speed, char mode);
	void SelectTarget(int id);
	void Unselect();
	void NormalAttack(char force_attack);
	void CastSkill(int skill_id, char force_attack, size_t target_count, int* targets);
	void Pickup(int id, int tid);
	void EquipItem(int inv_index, int eq_index);
	void GetInventoryDetail(int pack);
	void ResurrectByItem();		// 使用复活卷轴
	void ResurrectInTown();		// 回城复活
	void CheckSecurityPassword();
	void CancelAction();
	void Logout();
	void Speak(const wchar_t* strText);
	void Sitdown(bool bSitdown);	// 打坐
	void TalkToNPC(int nid);		// 和NPC对话
	void LearnSkill(int id);		// 学技能
	void Booth(bool bOpen, const wchar_t* szName=NULL);	// 摆摊
	void PlayAction(int iPose);		// 表演动作
	void UseItem(int iSlot);		// 使用包裹内的物品
	void SwitchFashion(bool bFashion);	// 切换到时装模式/普通模式
	void SummonPet(int iPetIdx);	// 召唤宠物
	void BanishPet(int iPetIdx);	// 召回宠物
	void AttachBuddy(int iPlayerID);	// 拥抱指定角色
	void DetachBuddy();				// 解除相依相偎
	void TeamInvite(int idPlayer);	// 邀请组队
	void DoAutoTeam(int type, int goal_id, int op);	// 自动组队 0 unknown, 1 task, 2 activity

	///////////////////////////////////////////////////////////////////////////
	// S2C Command

	void OnS2CCommand(WORD cmd, void* buf, size_t size);

	///////////////////////////////////////////////////////////////////////////
	// 内部函数，不应该释放给脚本
	
	void ResetPos(const A3DVECTOR3& vPos) { m_vPos = vPos; }
	void MoveTo(const A3DVECTOR3& vDest);
	void IncMoveStamp() { m_wMoveStamp++; }
	void ResetMoveStamp(WORD wStamp) { m_wMoveStamp = wStamp; }
	WORD GetMoveStamp() { return m_wMoveStamp; }
	void SetMoving(bool bMoving) { m_bIsMoving = bMoving; }
	void SetDead(bool bDead) { m_bIsDead = bDead; }

	// 获取游戏对象
	GameObject* GetGameObject() { return m_pGame; }

protected:
	GameObject* m_pGame;
	bool m_bInitOK;

	int m_iRoleID;				// 角色ID
	bool m_bIsDead;				// 是否死亡
	bool m_bChangingFace;		// 是否正在变形中

	int m_iHP;
	int m_iMaxHP;
	int m_iMP;
	int m_iMaxMP;
	int m_iLevel;
	int m_iLevel2;				// 修真级别
	int m_iExp;
	int m_iSP;					// 技能点
	float m_fAttackRange;
	float m_fRunSpeed;
	size_t m_nMoney;
	size_t m_nMoneyCapacity;
	bool m_bInFashion;			// 当前处于时装模式

	UserPackage m_Equips;		// 装备栏
	UserPackage m_Inventory;	// 背包
	SkillList m_Skills;			// 技能列表
	PetList m_PetData;			// 宠物列表

	A3DVECTOR3 m_vPos;
	WORD m_wMoveStamp;
	bool m_bIsMoving;
	BYTE m_byDir;

	int m_iSelectTarget;		// 当前选择的对象
	char m_cForceAttack;		// 强制PK标志

	task_base* m_pCurTask;
	std::list<task_base*> m_Tasks;	// 玩家当前行为列表
	DWORD m_dwLastTime;				// 计算每帧所花费的时间
	int m_iPosError;				// 被矫正位置的次数

	AString m_sScriptAI;		// LUA脚本提供的AI
	AIObject* m_pAI;			// 缺省的AI

protected:

	bool IsPositiveSkill(int id, int& mp_cost, float& range);
	bool PushItem(int tid, int count, int& slot, int& slot_count);

	bool AddTask(task_base* pTask);
	bool StartTask();
	bool EndCurTask();
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////

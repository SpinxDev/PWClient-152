/*
 * FILE: PlayerObject.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "PlayerObject.h"
#include "GameObject.h"
#include "EC_GPDataType.h"
#include "GameSession.h"
#include "SessionManager.h"
#include "EC_SendC2SCmds.h"
#include "EC_Skill.h"
#include "AIObject.h"
#include "MoveAgent.h"
#include <publicchat.hpp>
#include "luaFunc.h"
#include "elementdataman.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define CHAT_ACHAR_LENGTH 120


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  玩家行为相关
//  
///////////////////////////////////////////////////////////////////////////

bool task_idle::Tick(int tick)
{
	if( _imp->HasNextTask() ) return false;	
	return task_base::Tick(tick);
}

bool task_move::StartTask()
{
	// 直接就移动
	_imp->MoveTo(_dest);
	_imp->IncMoveStamp();
	_imp->PlayerMove(_dest,_time,_speed,_mode);
	_imp->SetMoving(true);
	return true;
}

bool task_stop_move::StartTask()
{
	// 直接就移动
	_imp->MoveTo(_dest);
	_imp->IncMoveStamp();
	_imp->PlayerStopMove(_dest,_time,_speed,_mode);
	_imp->SetMoving(false);
	return true;
}


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class PlayerObject
//  
///////////////////////////////////////////////////////////////////////////

// 构造函数
PlayerObject::PlayerObject( GameObject* pGame ) : m_pGame(pGame), m_bIsDead(false), m_bChangingFace(false),
	m_iLevel(0), m_iLevel2(0), m_iExp(0), m_iSP(0), m_iHP(0), m_iMaxHP(0), m_iMP(0), m_iMaxMP(0), m_iRoleID(0),
	m_fAttackRange(0.0f), m_fRunSpeed(0.0f), m_nMoney(0), m_nMoneyCapacity(200000000),
	m_vPos(0,0,0), m_wMoveStamp(0), m_bIsMoving(false), m_byDir(0), m_cForceAttack(0),
	m_iSelectTarget(-1), m_pCurTask(NULL), m_iPosError(0), m_bInFashion(false)
{
	m_bInitOK = false;
	m_dwLastTime = GetTickCount();
	m_sScriptAI = "";	// 使用缺省AI

	if( !(m_pAI = new AIObject(this)) )
	{
		ASSERT(m_pAI);
		a_LogOutput(1, "PlayerObject::PlayerObject(), Create AI object failed!");
	}
}

// 析构函数
PlayerObject::~PlayerObject()
{
	// 清理任务列表
	ClearTask();

	if( m_pAI )
	{
		delete m_pAI;
		m_pAI = NULL;
	}

	// 删除脚本AI实例
	if( !m_sScriptAI.IsEmpty() )
	{
		CScriptValue val;
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> ret;
		val.SetVal(m_pGame->GetGameSession()->GetUserName());
		args.push_back(val);
		LuaFuncFactory::GetSingleton().Call("AIManager", "OnExit", args, ret);
	}
}

// 初始化玩家信息
bool PlayerObject::Init(const S2C::cmd_self_info_1* info)
{
	m_iRoleID = info->cid;
	m_iExp = info->iExp;
	m_vPos = info->pos;
	m_iLevel2 = info->level2;

	// 提取附加数据
	BYTE* pData = (BYTE*)info + sizeof(S2C::cmd_self_info_1);
	if( info->state & GP_STATE_ADV_MODE )
		pData += sizeof(int) * 2;

	// 形状数据
	if( info->state & GP_STATE_SHAPE )
		pData += sizeof(BYTE);

	// 表情数据
	if( info->state & GP_STATE_EMOTE )
		pData += sizeof(BYTE);

	// 玩家状态数据
	if( info->state & GP_STATE_EXTEND_PROPERTY )
		pData += sizeof(DWORD) * PLAYER_STATE_COUNT;

	// 帮派ID
	if( info->state & GP_STATE_FACTION )
		pData += sizeof(int) + sizeof(BYTE);

	// 摊位CRC
	if( info->state & GP_STATE_BOOTH )
		pData += sizeof(BYTE);

	// Effect
	if( info->state & GP_STATE_EFFECT )
	{
		BYTE byNum = *pData;
		pData += sizeof(BYTE);
		short* pEffects = (short*)pData;
		pData += sizeof(short) * byNum;

		elementdataman* pDataMan = SessionManager::GetSingleton().GetDataMan();
		for( BYTE n=0;n<byNum;n++ )
		{
			DATA_TYPE DataType = pDataMan->get_data_type(pEffects[n], ID_SPACE_ESSENCE);
			if( DataType == DT_FACEPILL_ESSENCE )
			{
				m_bChangingFace = true;
				break;
			}
		}
	}

	// 红名等级
	if( info->state & GP_STATE_PARIAH )
		pData += sizeof(BYTE);

	// 骑乘
	if( info->state & GP_STATE_IN_MOUNT )
		pData += sizeof(unsigned short) + sizeof(int);

	// 相依相偎
	if( info->state & GP_STATE_IN_BIND )
		pData += sizeof(char) + sizeof(int);

	// 配偶ID
	if( info->state & GP_STATE_SPOUSE )
		pData += sizeof(int);

	// 装备禁用
	if( info->state & GP_STATE_EQUIPDISABLED )
		pData += sizeof(int);

	// 玩家势力
	if( info->state & GP_STATE_PLAYERFORCE )
		pData += sizeof(int);

	// 与另一个对象所连结的效果
	if( info->state & GP_STATE_MULTIOBJ_EFFECT )
	{
		int iCount = *(int*)pData;
		pData += sizeof(int);

		for( int n=0;n<iCount;n++ )
			pData += sizeof(int) + sizeof(char);
	}

	// 国家
	if( info->state & GP_STATE_COUNTRY )
		pData += sizeof(int);

	// 时装模式
	if( info->state & GP_STATE_FASHION )
		m_bInFashion = true;

	// 以我断定，此人已死
	if( info->state & GP_STATE_CORPSE )
		m_bIsDead = true;

	return true;
}

// 更改玩家AI策略
void PlayerObject::SetAIPolicy( const char* name )
{
	CScriptValue val;
	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> ret;

	if( !m_bInitOK )
	{
		a_LogOutput(1, "PlayerObject::SetAIPolicy, change AI policy failed before player is initialized OK!");
		return;
	}

	if( name )
	{
		if( m_sScriptAI == name )
			return;

		if( m_sScriptAI.IsEmpty() )
		{
			// 离开缺省AI
			m_pAI->Leave();
		}
		else
		{
			// 离开当前AI
			val.SetVal(m_pGame->GetGameSession()->GetUserName());
			args.push_back(val);
			LuaFuncFactory::GetSingleton().Call("AIManager", "OnLeave", args, ret);
		}

		// 进入新的AI
		args.clear(); ret.clear();
		val.SetVal(m_pGame->GetGameSession()->GetUserName());
		args.push_back(val);
		val.SetVal(name);
		args.push_back(val);
		LuaFuncFactory::GetSingleton().Call("AIManager", "OnEnter", args, ret);
		if( !ret[0].GetBool() )
		{
			SetAIPolicy(NULL);
			a_LogOutput(1, "Can't find the specified policy, we will use the default one! User:%s", m_pGame->GetGameSession()->GetUserName());
			return;
		}

		m_sScriptAI = name;
	}
	else
	{
		if( !m_sScriptAI.IsEmpty() )
		{
			// 使玩家离开当前策略
			val.SetVal(m_pGame->GetGameSession()->GetUserName());
			args.push_back(val);
			LuaFuncFactory::GetSingleton().Call("AIManager", "OnLeave", args, ret);

			// 进入缺省AI策略
			m_pAI->Enter();
			m_sScriptAI.Empty();
		}
	}
}

// 执行AI命令
void PlayerObject::AICommand( StringVector& cmd )
{
	if( m_sScriptAI.IsEmpty() )
	{
		m_pAI->Command(cmd);
	}
	else
	{
		CScriptValue val;
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> ret;

		// 用户名
		val.SetVal(m_pGame->GetGameSession()->GetUserName());
		args.push_back(val);

		// 压入命令参数
		CScriptValue cmdLine;
		abase::vector<CScriptValue> cmds;
		for( StringVector::iterator it=cmd.begin();it!=cmd.end();++it )
		{
			val.SetVal(*it);
			cmds.push_back(val);
		}

		cmdLine.SetArray(cmds);
		args.push_back(cmdLine);

		LuaFuncFactory::GetSingleton().Call("AIManager", "OnCommand", args, ret);
	}
}

// 更新玩家逻辑
void PlayerObject::Tick()
{
	DWORD dwCurTime = GetTickCount();
	DWORD dwDeltaTime = dwCurTime - m_dwLastTime;
	m_dwLastTime = dwCurTime;

	// 处理玩家行为列表
	if( m_pCurTask )
	{
		if( !m_pCurTask->Tick(dwDeltaTime) )
		{
			EndCurTask();
			StartTask();
		}

		return;
	}

	if( !m_sScriptAI.IsEmpty() )
	{
		// 调用LUA脚本AI
		CScriptValue val;
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> ret;
		val.SetVal(m_pGame->GetGameSession()->GetUserName());
		args.push_back(val);
		LuaFuncFactory::GetSingleton().Call("AIManager", "OnTick", args, ret);
	}
	else
	{
		if( m_pAI ) m_pAI->Tick();
	}
}

///////////////////////////////////////////////////////////////////////////
//  
//  玩家行为相关
//  
///////////////////////////////////////////////////////////////////////////

// 添加一个 空闲等待
bool PlayerObject::AddIdleTask(int iTime)
{
	AddTask(new task_idle(this, iTime));
	return StartTask();
}

// 移动
bool PlayerObject::AddMoveTask(const A3DVECTOR3& vDest, int time, float speed, char mode)
{
	AddTask(new task_move(this, vDest, time, speed, mode));
	return StartTask();
}

// 停止移动
bool PlayerObject::AddStopMoveTask(const A3DVECTOR3& vDest, int time, float speed, char mode)
{
	AddTask(new task_stop_move(this, vDest, time, speed, mode));
	return StartTask();
}

bool PlayerObject::AddAttackTask()
{
	AddTask(new task_normal_attack(this));
	return StartTask();
}

bool PlayerObject::AddCastSkillTask()
{
	AddTask(new task_cast_skill(this));
	return StartTask();
}

// 添加一个行为
bool PlayerObject::AddTask(task_base* pTask)
{
	if( m_Tasks.size() >= MAX_PLAYER_TASK )
	{
		std::list<task_base*>::iterator it;
		for( it=m_Tasks.begin();it!=m_Tasks.end();++it )
		{
			if( *it ) delete *it;
		}
		m_Tasks.clear();
	}

	m_Tasks.push_back(pTask);
	return m_pCurTask != NULL;
}

// 开始一个玩家行为
bool PlayerObject::StartTask()
{
	if( m_pCurTask ) return false;

	bool ret = false;
	while( m_Tasks.size() )
	{
		m_pCurTask = m_Tasks.front();
		m_Tasks.pop_front();

		if( (ret = m_pCurTask->StartTask()) )
			break;
		else
			EndCurTask();
	}

	return ret;
}

// 结束一个玩家行为
bool PlayerObject::EndCurTask()
{
	if( !m_pCurTask )
		return false;

	m_pCurTask->EndTask();
	delete m_pCurTask;
	m_pCurTask = NULL;
	return true;
}

// 清除玩家行为
void PlayerObject::ClearTask()
{
	if( m_pCurTask )
		EndCurTask();

	std::list<task_base*>::iterator it;
	for( it=m_Tasks.begin();it!=m_Tasks.end();++it )
	{
		if( *it ) delete *it;
	}

	m_Tasks.clear();
}

// 移动到目标位置
void PlayerObject::MoveTo(const A3DVECTOR3& vDest)
{
	A3DVECTOR3 direction = vDest;
	direction -= m_vPos;
	if( direction.SquaredMagnitude() > 1e-6 ) m_byDir = DirConvert(direction);
	m_vPos = vDest;
}

void PlayerObject::MoveAgentLearn(const A3DVECTOR3& vPos)
{
	MoveAgent::GetSingleton().Learn(vPos);
}

bool PlayerObject::GetNextMovePos(const A3DVECTOR3& vPos, const A3DVECTOR3& vDest, float fRange, A3DVECTOR3& vNext)
{
	return MoveAgent::GetSingleton().GetMovePos(vPos, vDest, fRange, vNext);
}

bool PlayerObject::GetNextMovePos(const A3DVECTOR3& vPos, BYTE dir, float fRange, A3DVECTOR3& vNext)
{
	return MoveAgent::GetSingleton().GetMovePos(vPos, dir, fRange, vNext);
}

// 强制攻击
void PlayerObject::SetForceAttack(bool bForce)
{
	if( bForce )
		m_cForceAttack |= GP_PVPMASK_FORCE;
	else
		m_cForceAttack = 0;
}

// 获取玩家的职业
int PlayerObject::GetProfession()
{
	return m_pGame->GetGameSession()->GetProfession();
}

// 玩家是否为男性
bool PlayerObject::IsMale() const
{
	return m_pGame->GetGameSession()->GetGender() == GENDER_MALE;
}

// 获取玩家的技能信息
PlayerObject::SKILL* PlayerObject::GetSkillByID(int id)
{
	for( size_t i=0;i<m_Skills.size();i++ )
	{
		if( m_Skills[i].id == id )
			return &m_Skills[i];
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////
//  
//  玩家包裹相关
//  
///////////////////////////////////////////////////////////////////////////

// 检测装备插槽是否存在任意装备
bool PlayerObject::IsEquipExist(size_t index)
{
	return index < m_Equips.size() && m_Equips[index].tid != 0;
}

// 检测装备插槽是否存在指定装备
bool PlayerObject::IsEquipExist(int tid, size_t index)
{
	return index < m_Equips.size() && m_Equips[index].tid == tid;
}

// 检测物品是否存在
bool PlayerObject::IsItemExist(int tid)
{
	UserPackage::iterator it;
	for( it=m_Inventory.begin();it!=m_Inventory.end();++it )
	{
		if( it->tid == tid && it->count > 0 )
			return true;
	}

	return false;
}

// 获取背包内空的插槽
int PlayerObject::GetEmptySlot()
{
	for( size_t i=0;i<m_Inventory.size();i++ )
	{
		if( m_Inventory[i].tid == 0 )
			return i;
	}

	return -1;
}

// 获取指定物品的个数
int PlayerObject::GetItemCount(PackType pack, int tid)
{
	int count = 0;
	UserPackage* pk = (pack == PACK_INVENTORY) ? &m_Inventory : &m_Equips;
	for( size_t i=0;i<pk->size();i++ )
	{
		if( (*pk)[i].tid == tid )
			count += (*pk)[i].count;
	}
	return count;
}

// 获取指定物品的索引
int PlayerObject::GetItemIndex(PackType pack, int tid)
{
	UserPackage* pk = (pack == PACK_EQUIPMENTS) ? &m_Equips : &m_Inventory;
	for( size_t i=0;i<pk->size();i++ )
	{
		if( (*pk)[i].tid == tid )
			return i;
	}

	return -1;
}

// 添加物品到背包
bool PlayerObject::PushItem(int tid, int count, int& slot, int& slot_count)
{
	int iFirstEmpty = -1;
	int iCurCount = count;
	int pile_num = GetItemPileLimit(tid);

	for( size_t i=0;i<m_Inventory.size();i++ )
	{
		if( m_Inventory[i].tid )
		{
			if( m_Inventory[i].tid != tid || m_Inventory[i].count >= (size_t)pile_num )
				continue;

			if( m_Inventory[i].count + iCurCount > (size_t)pile_num  )
			{
				int iAddNum = pile_num - m_Inventory[i].count;
				m_Inventory[i].count += iAddNum;
				iCurCount -= iAddNum;
			}
			else
			{
				m_Inventory[i].count += iCurCount;
				iCurCount = 0;
			}

			if( !iCurCount )
			{
				slot = i;
				slot_count = m_Inventory[i].count;
				return true;
			}
		}
		else if( iFirstEmpty < 0 )
			iFirstEmpty = i;
	}

	if( iFirstEmpty < 0 || !iCurCount )
		return false;

	// 使用一个新的格子
	ASSERT(iCurCount <= pile_num);
	m_Inventory[iFirstEmpty].tid = tid;
	m_Inventory[iFirstEmpty].count = iCurCount;
	slot = iFirstEmpty;
	slot_count = iCurCount;
	return true;
}

///////////////////////////////////////////////////////////////////////////
//  
//  目标对象相关
//  
///////////////////////////////////////////////////////////////////////////

// 东西是否能被捡起
bool PlayerObject::MatterCanPickup(int tid, size_t& pile_limit)
{
	if( tid == 3044 )
	{
		// 金币超过包裹最大值
		if( m_nMoney >= m_nMoneyCapacity )
			return false;
	}
	else
	{
		int empty_index = -1;
		for( size_t i=0;i<m_Inventory.size();i++ )
		{
			if( m_Inventory[i].tid == 0 )
			{
				empty_index = i;
				break;
			}
		}

		if( empty_index == -1 ) return false;
	}

	return true;
}

// 东西是否能被采集
bool PlayerObject::MatterCanGather(int tid)
{
	return false;
}

// NPC能否被攻击
bool PlayerObject::NpcCanAttack(int tid)
{
	return true;
}

// 玩家能否被攻击
bool PlayerObject::PlayerCanAttack(int id)
{
	GameObject::ObjectInfo* pObj = m_pGame->GetPlayer(id);
	if( !pObj ) return false;
	return true;
}

// 是否为主动技能
bool PlayerObject::IsPositiveSkill(int id, int& mp_cost, float& range)
{
	CECSkill skill(id, 1);
	mp_cost = skill.GetRequiredMP();
	range = skill.GetCastRange(0.0f, 0.0f);
	return skill.IsPositiveSkill();
}

// 选取一个可用的攻击技能
int PlayerObject::SelectAttackSkill(float& sqr_range)
{
	if( a_Random(0, 2) != 0 )
	{
		// 66%几率使用技能
		if( m_Skills.size() )
		{
			SKILL& sk = m_Skills[a_Random(0, m_Skills.size()-1)];

			if( m_iMP >= sk.mp_cost )
			{
				sqr_range = sk.range * sk.range;
				return sk.id;
			}
		}
	}

	//使用普通攻击
	sqr_range = m_fAttackRange * m_fAttackRange;
	return 0;
}


///////////////////////////////////////////////////////////////////////////
//  
//  C2S Command Functions
//  
///////////////////////////////////////////////////////////////////////////

void PlayerObject::SendGameData(void* buf, size_t size)
{
	GameSession* pSession = m_pGame->GetGameSession();
	pSession->SendGameData(buf, size);
}

///////////////////////////////////////////////////////////////////////////
// 发送调试命令

void PlayerObject::SendDebugCmd(short cmd)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendDebugCmd(cmd, 0);
}

void PlayerObject::SendDebugCmd(short cmd, int param)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendDebugCmd(cmd, 1, param);
}

void PlayerObject::SendDebugCmd(short cmd, int param1, int param2)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendDebugCmd(cmd, 2, param1, param2);
}

void PlayerObject::SendDebugCmd(short cmd, void* buf, size_t sz)
{
#pragma pack(1)
	struct DEBUG_CMD
	{
		short cmd;
		int param[5];
	};
#pragma pack()

	if( sz <= sizeof(int)*5 )
	{
		DEBUG_CMD data;
		data.cmd = cmd;
		memcpy(data.param, buf, sz);
		SendGameData(&data, sizeof(short)+sz);
	}
}

///////////////////////////////////////////////////////////////////////////

// 玩家飞到指定位置
void PlayerObject::Goto(float x, float z)
{
	A3DVECTOR3 pos(x, 0.0f, z);
	SendDebugCmd(C2S::GOTO, &pos, sizeof(pos));
}

// 玩家随机瞬移
void PlayerObject::RandomMove()
{
	A3DVECTOR3 pos;
	A3DVECTOR3 vRegionMin, vRegionMax;
	MoveAgent::GetSingleton().GetRegion(vRegionMin, vRegionMax);
	Goto(a_Random(vRegionMin.x, vRegionMax.x), a_Random(vRegionMin.z, vRegionMax.z));
}

// 玩家移动
void PlayerObject::PlayerMove(A3DVECTOR3& pos, int time, float speed, char mode)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdPlayerMove(pos, pos, time, speed*256.0f, mode, m_wMoveStamp);
}

// 玩家停止移动
void PlayerObject::PlayerStopMove(A3DVECTOR3& pos, int time, float speed, char mode)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdStopMove(pos, speed*256.0f, mode, m_byDir, m_wMoveStamp, time);
}

// 选择目标
void PlayerObject::SelectTarget(int id)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdSelectTarget(id);
}

// 取消选择
void PlayerObject::Unselect()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdUnselect();
}

// 普通攻击
void PlayerObject::NormalAttack(char force_attack)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdNormalAttack(force_attack);
}

// 发技能
void PlayerObject::CastSkill(int skill_id, char force_attack, size_t target_count, int* targets)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdCastSkill(skill_id, force_attack, target_count, targets);
}

// 捡东西
void PlayerObject::Pickup(int id, int tid)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdPickup(id, tid);
}

// 装备物品
void PlayerObject::EquipItem(int inv_index, int eq_index)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdEquipItem(inv_index, eq_index);
}

// 获取物品详细信息
void PlayerObject::GetInventoryDetail(int pack)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdGetIvtrDetailData(pack);
}

// 使用复活卷轴复活
void PlayerObject::ResurrectByItem()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdReviveItem();
}

// 回城复活
void PlayerObject::ResurrectInTown()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdReviveVillage();
}

// 检测仓库密码
void PlayerObject::CheckSecurityPassword()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdOpenFashionTrash("");
}

// 取消上一动作
void PlayerObject::CancelAction()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdCancelAction();
}

// 登出玩家
void PlayerObject::Logout()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdLogout(0);
}

// 玩家讲话
void PlayerObject::Speak(const wchar_t* strText)
{
	GNET::PublicChat p;
	p.channel = GP_CHAT_LOCAL;
	p.roleid = m_iRoleID;

	AWString strMsg = strText;
	int iMsgLen = strMsg.GetLength();
	a_ClampRoof(iMsgLen, CHAT_ACHAR_LENGTH);

	// 禁用机器人发送指令#
	if( strMsg[0] == _AL('#') )
		strMsg[0] = _AL(' ');

	int iSize = iMsgLen * sizeof(wchar_t);
	p.msg.replace(strMsg, iSize);

	m_pGame->GetGameSession()->SendNetData(p);
}

// 玩家打坐
void PlayerObject::Sitdown(bool bSitdown)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	if( bSitdown )
		c2s_SendCmdSitDown();
	else
		c2s_SendCmdStandUp();
}

// 和NPC对话，开始NPC服务
void PlayerObject::TalkToNPC(int nid)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdNPCSevHello(nid);
}

// 玩家学技能
void PlayerObject::LearnSkill(int id)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdNPCSevLearnSkill(id);
}

// 玩家摆摊
void PlayerObject::Booth(bool bOpen, const wchar_t* szName/* =NULL */)
{
	AWString sBoothName;
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());

	if( bOpen )
	{
		if( szName )
			sBoothName = szName;
		else
			sBoothName.Format(L"asdf", GetRoleName());

		// 收购一个幻仙石
		C2S::cmd_open_booth::entry_t* aEntry = new C2S::cmd_open_booth::entry_t[12];
		aEntry[0].type = 3043;
		aEntry[0].index = 0;
		aEntry[0].count = 1;
		aEntry[0].price = 1000;

  		aEntry[1].type = 3043;
  		aEntry[1].index = 0xffff;
  		aEntry[1].count = 5;
  		aEntry[1].price = 50000;
 
  		aEntry[2].type = 3043;
  		aEntry[2].index = 0xffff;
  		aEntry[2].count = 1;
  		aEntry[2].price = 8000;

		aEntry[3].type = 15761;
		aEntry[3].index = 0xffff;
		aEntry[3].count = 1;
		aEntry[3].price = 5000;

		for(int i=4; i<12; i++)
		{
			aEntry[i].type = 11208;
			aEntry[i].index = 0xffff;
			aEntry[i].count = i;
			aEntry[i].price = 1000 * i;
		}

		c2s_SendCmdOpenBooth(12, (const wchar_t*)sBoothName, aEntry);
		delete[] aEntry;
	}
	else
	{
		c2s_SendCmdCloseBooth();
	}
}

// 玩家动作
void PlayerObject::PlayAction(int iPose)
{
	if( m_bIsDead || m_bIsMoving )
		return;

	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdEmoteAction(iPose);
}

// 使用包裹内的物品
void PlayerObject::UseItem(int iSlot)
{
	if( m_bIsDead || m_bIsMoving )
		return;

	if( iSlot < 0 || iSlot >= (int)m_Inventory.size() )
		return;

	if( !m_Inventory[iSlot].tid || !m_Inventory[iSlot].count )
		return;

	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdUseItem(PACK_INVENTORY, (BYTE)iSlot, m_Inventory[iSlot].tid, 1);
}

// 使玩家切换到时装模式
void PlayerObject::SwitchFashion(bool bFashion)
{
	if( m_bInFashion != bFashion )
	{
		SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
		c2s_SendCmdFashionSwitch();
	}
}

// 召唤宠物
void PlayerObject::SummonPet(int iPetIdx)
{
	if( m_bIsDead || m_bIsMoving )
		return;

	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdPetSummon(iPetIdx);
}

// 召回宠物
void PlayerObject::BanishPet(int iPetIdx)
{
	if( m_bIsDead || m_bIsMoving )
		return;

	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdPetBanish(iPetIdx);
}

// 拥抱指定角色
void PlayerObject::AttachBuddy(int iPlayerID)
{
	GameObject::ObjectInfo* pObj = m_pGame->GetPlayer(iPlayerID);
	if( pObj )
	{
		SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
		if( IsMale() )
			c2s_SendCmdBindPlayerInvite(iPlayerID);
		else
			c2s_SendCmdBindPlayerRequest(iPlayerID);
	}
}

// 解除相依相偎
void PlayerObject::DetachBuddy()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdCancelBindPlayer();
}

// 邀请组队
void PlayerObject::TeamInvite(int idPlayer)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdTeamInvite(idPlayer);
}

// 自动组队
// 0 unknown, 1 task, 2 activity
void PlayerObject::DoAutoTeam(int type, int goal_id, int op)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdAutoTeamSetGoal(type, goal_id, op);
}

///////////////////////////////////////////////////////////////////////////
// 响应玩家收到S2C协议

void PlayerObject::OnS2CCommand(WORD cmd, void* buf, size_t size)
{
	using namespace S2C;

	elementdataman* pDataMan = SessionManager::GetSingleton().GetDataMan();

	switch(cmd)
	{
	case TASK_DATA:
		{
			// 标志着玩家的数据获取完
			// 开启角色内服无冷却
			SendDebugCmd(8903, 73125);
			m_pAI->Enter();
			m_bInitOK = true;
		}
		break;
	case OWN_EXT_PROP:
		{
			cmd_own_ext_prop* pCmd = (cmd_own_ext_prop*)buf;
			m_fRunSpeed = pCmd->prop.mv.run_speed;
			m_fAttackRange = pCmd->prop.ak.attack_range;
		}
		break;
	case SELF_INFO_00:
		{
			cmd_self_info_00* pCmd = (cmd_self_info_00*)buf;
			m_iLevel = pCmd->sLevel;
			m_iLevel2 = pCmd->Level2;
			m_iHP = pCmd->iHP;
			m_iMaxHP = pCmd->iMaxHP;
			m_iMP = pCmd->iMP;
			m_iMaxMP = pCmd->iMaxMP;
			m_iExp = pCmd->iExp;
			m_iSP = pCmd->iSP;
		}
		break;
	case PLAYER_EXT_PROP_MOVE:
		{
			cmd_pep_move* pCmd = (cmd_pep_move*)buf;
			if( pCmd->idPlayer == m_iRoleID )
				m_fRunSpeed = pCmd->ep_move.run_speed;
		}
		break;
	case OWN_IVTR_DETAIL_DATA:
		{
			// 获取玩家包裹数据
			UserPackage* pack;
			cmd_own_ivtr_detail_info* pCmd = (cmd_own_ivtr_detail_info*)buf;
			if( pCmd->byPackage == PACK_INVENTORY )
				pack = &m_Inventory;
			else if( pCmd->byPackage == PACK_EQUIPMENTS )
				pack = &m_Equips;
			else break;

			pack->clear();
			pack->insert(pack->end(), pCmd->ivtr_size, ITEM());

			ITEM it;
			char* pEnd = (char*)buf + size;
			char* p = (char*)pCmd->content;
			size_t count = *(size_t*)p; p += sizeof(size_t);
			bool err = false;
			size_t index;
			unsigned short crc, len;
			for( size_t i=0;i<count;i++ )
			{
				index = *(size_t*)p;	p += sizeof(size_t);
				it.tid = *(int*)p;		p += sizeof(int);
				p += sizeof(int);
				p += sizeof(int);
				it.count = *(size_t*)p;		p += sizeof(size_t);
				crc = *(unsigned short*)p;	p += sizeof(unsigned short);
				len = *(unsigned short*)p;	p += sizeof(unsigned short);
				if( len ) p += len;
				if( p > pEnd )
				{
					err = true;
					break;
				}
				if( index >= pCmd->ivtr_size )
				{
					err = true;
					break;
				}

				(*pack)[index] = it;
			}

			if( err )
			{
				a_LogOutput(1, "OWN_IVTR_DETAIL_DATA, Invalid data size when build the inventory content!");
				pack->clear();
			}
		}
		break;
	case GET_OWN_MONEY:
		{
			cmd_get_own_money* pCmd = (cmd_get_own_money*)buf;
			m_nMoney = pCmd->amount;
			m_nMoneyCapacity = pCmd->max_amount;
		}
		break;
	case PICKUP_MONEY:
		{
			cmd_pickup_money* pCmd = (cmd_pickup_money*)buf;
			m_nMoney += pCmd->iAmount;
		}
		break;
	case PICKUP_ITEM:
		{
			cmd_pickup_item* pCmd = (cmd_pickup_item*)buf;
			if( pCmd->byPackage != PACK_INVENTORY ) break;

			int slot, slot_count;
			if( !PushItem(pCmd->tid, pCmd->iAmount, slot, slot_count) ||
				slot != pCmd->bySlot || slot_count != (int)pCmd->iSlotAmount )
			{
				// 与服务器数据不一致
				a_LogOutput(1, "S2C::PICKUP_ITEM, PushItem error, 与服务器数据不一致！");
				GetInventoryDetail(PACK_INVENTORY);
			}
		}
		break;
	case EQUIP_ITEM:
		{
			cmd_equip_item* pCmd = (cmd_equip_item*)buf;
			if( pCmd->index_inv >= m_Inventory.size() || pCmd->index_equip >= m_Inventory.size() )
				break;
			abase::swap(m_Inventory[pCmd->index_inv], m_Equips[pCmd->index_equip]);
			if( m_Inventory[pCmd->index_inv].count != pCmd->count_inv || m_Equips[pCmd->index_equip].count != pCmd->count_equip )
			{
				// 与服务器数据不一致
				a_LogOutput(1, "S2C::EQUIP_ITEM, 与服务器数据不一致！");
				GetInventoryDetail(PACK_INVENTORY);
				GetInventoryDetail(PACK_EQUIPMENTS);
			}
		}
		break;
	case PLAYER_DROP_ITEM:
		{
			UserPackage* pack;
			cmd_player_drop_item* pCmd = (cmd_player_drop_item*)buf;
			if( pCmd->byPackage == PACK_INVENTORY )
				pack = &m_Inventory;
			else if( pCmd->byPackage == PACK_EQUIPMENTS )
				pack = &m_Equips;
			else break;

			if( pCmd->bySlot >= pack->size() )
				break;
			ITEM& it = (*pack)[pCmd->bySlot];
			if( it.tid != pCmd->tid ) break;
			if( it.count > pCmd->count )
				it.count -= pCmd->count;
			else
			{
				it.tid = 0;
				it.count = 0;
			}
		}
		break;
	case TRASHBOX_PWD_STATE:
		{
			cmd_trashbox_pwd_state* pCmd = (cmd_trashbox_pwd_state*)buf;
			if( !pCmd->has_passwd ) CheckSecurityPassword();
		}
		break;
	case SKILL_DATA:
		{
			// 读取技能数据
			SKILL skill;
			cmd_skill_data* pCmd = (cmd_skill_data*)buf;
			for( size_t i=0;i<pCmd->skill_count;i++ )
			{
				skill.id = pCmd->skill_list[i].id_skill;
				skill.level = pCmd->skill_list[i].level;

				// 禁用回城术
				if( skill.id == 167 ) continue;

				if( IsPositiveSkill(skill.id,skill.mp_cost,skill.range) )
				{
					if( skill.range < 1e-6 ) skill.range = m_fAttackRange;
					m_Skills.push_back(skill);
				}
			}
		}
		break;
	case HOST_CORRECT_POS:
		{
			cmd_host_correct_pos* pCmd = (cmd_host_correct_pos*)buf;
			m_vPos = pCmd->pos;
			ResetMoveStamp(pCmd->stamp);
			ClearTask();

			// 连着三次以上被矫正位置，重新飞一个
			if( m_iPosError++ >= 3 )
			{
				A3DVECTOR3 vMin, vMax;
				MoveAgent::GetSingleton().GetRegion(vMin, vMax);
				float fRnd = a_Random(-10.0f, 10.0f);
				A3DVECTOR3 vDest = m_vPos + A3DVECTOR3(fRnd, 0.0f, fRnd);
				a_Clamp(vDest.x, vMin.x, vMax.x);
				a_Clamp(vDest.z, vMin.z, vMax.z);
				SendDebugCmd(C2S::GOTO, &vDest, sizeof(vDest));
				m_iPosError = 0;
			}
		}
		break;
	case SET_MOVE_STAMP:
		{
			cmd_set_move_stamp* pCmd = (cmd_set_move_stamp*)buf;
			ResetMoveStamp(pCmd->move_stamp);
		}
		break;
	case NOTIFY_HOSTPOS:
		{
			cmd_notify_hostpos* pCmd = (cmd_notify_hostpos*)buf;
			m_vPos = pCmd->vPos;
			ClearTask();
		}
		break;

	case SELECT_TARGET:
		{
			cmd_select_target* pCmd = (cmd_select_target*)buf;
			m_iSelectTarget = pCmd->idTarget;
		}
		break;
	case UNSELECT:
		{
			m_iSelectTarget = -1;
		}
		break;
	case HOST_START_ATTACK:
		{
			cmd_host_start_attack* pCmd = (cmd_host_start_attack*)buf;
			AddAttackTask();

			if( !pCmd->ammo_remain )
			{
				m_Equips[11].tid = 0;
				m_Equips[11].count = 0;
			}
			else
			{
				m_Equips[11].count = pCmd->ammo_remain;
			}
		}
		break;
	case ATTACK_ONCE:
		{
			cmd_attack_once* pCmd = (cmd_attack_once*)buf;
			if( pCmd->ammo_num && m_Equips[11].tid )
			{
				if( m_Equips[11].count <= pCmd->ammo_num )
				{
					m_Equips[11].tid = 0;
					m_Equips[11].count = 0;
				}
				else
				{
					m_Equips[11].count -= pCmd->ammo_num;
				}
			}
		}
		break;
	case HOST_STOPATTACK:
		{
			if( m_pCurTask && m_pCurTask->GetTaskType() == TASK_TYPE_NORMAL_ATTACK )
			{
				EndCurTask();
				StartTask();
			}
		}
		break;
	case OBJECT_CAST_SKILL:
		{
			cmd_object_cast_skill* pCmd = (cmd_object_cast_skill*)buf;
			if( pCmd->caster == m_iRoleID )
			{
				AddCastSkillTask();
			}
		}
		break;
	case HOST_STOP_SKILL:
		{
			if( m_pCurTask && m_pCurTask->GetTaskType() == TASK_TYPE_CAST_SKILL )
			{
				EndCurTask();
				StartTask();
			}
		}
		break;
	case HOST_DIED:
		{
			cmd_host_died* pCmd = (cmd_host_died*)buf;
			m_bIsDead = true;
			m_vPos = pCmd->pos;
			ClearTask();
		}
		break;

	case BE_HURT:
		{
			cmd_be_hurt* pCmd = (cmd_be_hurt*)buf;
			if( ISPLAYERID(pCmd->attacker_id) )
				SetForceAttack(true);
		}
		break;

	case LEARN_SKILL:
		{
			bool bFound = false;
			cmd_learn_skill* pCmd = (cmd_learn_skill*)buf;
			for( size_t i=0;i<m_Skills.size();i++ )
			{
				if( m_Skills[i].id == pCmd->skill_id )
				{
					if( !pCmd->skill_level )
						m_Skills.erase(m_Skills.begin() + i);
					else
						m_Skills[i].level = pCmd->skill_level;
					bFound = true;
					break;
				}
			}

			if( !bFound )
			{
				SKILL skill;
				skill.id = pCmd->skill_id;
				skill.level = pCmd->skill_level;

				CECSkill skData(skill.id, 1);
				skill.mp_cost = skData.GetRequiredMP();
				skill.range = skData.GetCastRange(0.0f, 0.0f);
				m_Skills.push_back(skill);
			}
		}
		break;

	case COST_SKILL_POINT:
		{
			cmd_cost_skill_point* pCmd = (cmd_cost_skill_point*)buf;
			m_iSP -= pCmd->skill_point;
			a_ClampFloor(m_iSP, 0);
		}
		break;

	case PLAYER_ENABLE_EFFECT:
		{
			cmd_player_enable_effect* pCmd = (cmd_player_enable_effect*)buf;
			DATA_TYPE DataType = pDataMan->get_data_type(pCmd->effect, ID_SPACE_ESSENCE);
			if( DataType == DT_FACEPILL_ESSENCE )
				m_bChangingFace = true;
		}
		break;
	case PLAYER_DISABLE_EFFECT:
		{
			cmd_player_disable_effect* pCmd = (cmd_player_disable_effect*)buf;
			DATA_TYPE DataType = pDataMan->get_data_type(pCmd->effect, ID_SPACE_ESSENCE);
			if( DataType == DT_FACEPILL_ESSENCE )
				m_bChangingFace = false;
		}
		break;

	case PLAYER_ENABLE_FASHION:
		{
			cmd_player_enable_fashion* pCmd = (cmd_player_enable_fashion*)buf;
			if( pCmd->idPlayer == m_iRoleID )
				m_bInFashion = pCmd->is_enabble ? true : false;
		}
		break;
	case PET_ROOM:
		{
			cmd_pet_room* pCmd = (cmd_pet_room*)buf;
			const BYTE* pData = (BYTE*)buf + sizeof(cmd_pet_room);

			PET_DATA pet_data;
			m_PetData.clear();
			for( int i=0;i<pCmd->count;i++ )
			{
				int iSlot = *(int*)pData;
				pData += sizeof(int);
				const info_pet* pPet = (const info_pet*)pData;
				pData += sizeof(info_pet);

				strncpy(pet_data.name, pPet->name, 16);
				pet_data.pet_class = pPet->pet_class;
				pet_data.level = pPet->level;
				m_PetData.push_back(pet_data);
			}
		}
		break;
	case PLAYER_BIND_REQUEST:
		{
			cmd_player_bind_request* pCmd = (cmd_player_bind_request*)buf;
			SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
			c2s_SendCmdBindPlayerRequestRe(pCmd->who, 0);
		}
		break;
	case PLAYER_BIND_INVITE:
		{
			cmd_player_bind_invite* pCmd = (cmd_player_bind_invite*)buf;
			SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
			c2s_SendCmdBindPlayerInviteRe(pCmd->who, 0);
		}
		break;

	case COOLTIME_DATA:
		{
		}
		break;
	case ENCHANT_RESULT:
		{
		}
		break;

	default: break;
	}
}

// 获取角色的名字
const wchar_t* PlayerObject::GetRoleName()
{
	return m_pGame->GetGameSession()->GetRoleName();
}

/*
 * FILE: AIObject.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/9/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "AIObject.h"
#include "PlayerObject.h"
#include "GameObject.h"
#include "MoveAgent.h"
#include <ALog.h>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class AIObject
//  
///////////////////////////////////////////////////////////////////////////

AIObject::AIObject( PlayerObject* pPlayer ) : m_pHost(pPlayer), m_iState(AI_STATE_IDLE)
{
	m_iExtData = 0;
	m_iFollowID = 0;

	// 该AI对象的命令
	m_AICommand["follow"] = &AIObject::OnCmd_Follow;
}

AIObject::~AIObject()
{
}

// 改变当前AI状态
void AIObject::SetCurState(int iState)
{
	m_iState = iState;
	m_iExtData = 0;
}

// 玩家进入该策略
void AIObject::Enter()
{
	m_pHost->RandomMove();
	SetCurState(AI_STATE_BORN);
}

// 玩家离开该策略
void AIObject::Leave()
{
	// 如果正在攻击或放技能，则停止
	m_pHost->CancelAction();
	m_pHost->ClearTask();
}

// 接受命令
void AIObject::Command( StringVector& cmd )
{
	if( cmd.size() )
	{
		AICommandMap::iterator it;
		it = m_AICommand.find(cmd[0]);
		if( it != m_AICommand.end() )
			(this->*it->second)(cmd);
		else
		{
			a_LogOutput(1, "Can't find the cmd (%s) in AIObject!", cmd[0]);
		}
	}
}

// 更新玩家AI
void AIObject::Tick()
{
	typedef void (AIObject::*FN_TICK_FUNC) ();
	static FN_TICK_FUNC ticks[] =
	{
		&AIObject::Tick_Born,
		&AIObject::Tick_Idle,
		&AIObject::Tick_Patrol,
		&AIObject::Tick_Follow,
		&AIObject::Tick_Combat,
		&AIObject::Tick_Dead,
	};

	if( m_iState != AI_STATE_FOLLOW )
	{
		if( m_iFollowID )
			SetCurState(AI_STATE_FOLLOW);
	}
	else
	{
		if( !m_iFollowID )
			SetCurState(AI_STATE_IDLE);
	}

	// 触发死亡
	if( m_pHost->IsDead() )
		SetCurState(AI_STATE_DEAD);

	ASSERT(m_iState >= 0 && m_iState < AI_STATE_NUM);
	(this->*ticks[m_iState])();
}

// 玩家进入世界，首先提高自身属性
void AIObject::Tick_Born()
{
	m_iExtData = 0;

	// 升级
	if( m_pHost->GetLevel() < 10 )
	{
		m_pHost->SendDebugCmd(10889, 10000000);
	}
	else m_iExtData++;

	// 确保血气值不过低
	if( m_pHost->GetHP() < m_pHost->GetMaxHP() / 2 )
	{
		int index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 27070);

		if( index == -1 )
			m_pHost->SendDebugCmd(10800, 27070);
		else
			m_pHost->EquipItem(index, 20);
	}
	else m_iExtData++;

	// 保证真气值不过低
	if( m_pHost->GetMP() < m_pHost->GetMaxMP() / 2 )
	{
		int index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 27071);

		if( index == -1 )
			m_pHost->SendDebugCmd(10800, 27071);	
		else
			m_pHost->EquipItem(index, 21);
	}
	else m_iExtData++;

	// 穿上gm披风
	if( !m_pHost->IsEquipExist(11215, 3) )
	{
		int index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 11215);
		if( index == -1 )
		{
			m_pHost->SendDebugCmd(10800, 11215);
		}
		else
		{
			m_pHost->EquipItem(index, 3);
		}
	}
	else m_iExtData++;

	// 如果是羽芒职业，我们使用弓箭(GM飞天少女弓 + 狼牙箭)
	if( m_pHost->GetProfession() == PROF_ARCHOR && !(m_pHost->IsEquipExist(11214, 0) &&
		m_pHost->IsEquipExist(410, 11)) )
	{
		int index;

		// 装备飞天少女弓
		if( !m_pHost->IsEquipExist(11214, 0) )
		{
			index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 11214);
			if( index == -1 )
				m_pHost->SendDebugCmd(10800, 11214);
			else
				m_pHost->EquipItem(index, 0);
		}

		// 装备骨矢
		if( !m_pHost->IsEquipExist(410, 11) )
		{
			index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 410);
			if( index == -1 )
				m_pHost->SendDebugCmd(10800, 410, 999);
			else
			{
				if( m_pHost->IsEquipExist(11214,0) )
					m_pHost->EquipItem(index, 11);
			}
		}
	}
	// 如果是剑灵职业，我们使用GM☆联翩千羽尽
	else if( m_pHost->GetProfession() == PROF_JIANLING && !m_pHost->IsEquipExist(29719, 0) )
	{
		int index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 29719);
		if( index == -1 )
			m_pHost->SendDebugCmd(10800, 29719);
		else
			m_pHost->EquipItem(index, 0);
	}
	// 其他职业，禁止使用弓箭
	else if( m_pHost->GetProfession() != PROF_ARCHOR && m_pHost->IsEquipExist(11) )
	{
		// 把弓拿下来
		int index = m_pHost->GetEmptySlot();
		if( index >= 0 ) m_pHost->EquipItem(index, 0);
	}
	else m_iExtData++;

	// 保证有足够的复活卷轴
	if( m_pHost->GetItemCount(PlayerObject::PACK_INVENTORY, 3043) < 10 )
	{
		m_pHost->SendDebugCmd(10800, 3043, 10);
	}
	else m_iExtData++;

	// 一切就绪，GO GO GO
	if( m_iExtData >= 6 )
	{
		SetCurState(AI_STATE_COMBAT);
	}

	m_pHost->AddIdleTask(3000);
}

// 玩家空闲状态
void AIObject::Tick_Idle()
{
	if( m_pHost->GetHP() < m_pHost->GetMaxHP() / 2 )
	{
		// 打坐
		if( !m_iExtData )
		{
			m_pHost->Sitdown(true);
			m_iExtData = 1;
		}
	}
	else
	{
		if( m_iExtData )
		{
			m_pHost->Sitdown(false);
			m_iExtData = 0;
		}
	}

	if( !m_iExtData )
	{
		// 50米内有目标，则进入战斗状态，否则进入巡逻状态
		int target = m_pHost->GetGameObject()->GatherTarget(50.0f, m_pHost->GetForceAttack());
		if( target != -1 )
			SetCurState(AI_STATE_COMBAT);
		else
			SetCurState(AI_STATE_PATROL);
	}
	else
	{
		m_pHost->AddIdleTask(1000);
	}
}

// 玩家死亡状态
void AIObject::Tick_Dead()
{
	if( m_pHost->IsDead() )
	{
		// 如果包裹内有复活卷轴，直接复活，否则回城复活
		if( m_pHost->GetItemCount(PlayerObject::PACK_INVENTORY,3043) > 0 )
			m_pHost->ResurrectByItem();
		else
			m_pHost->ResurrectInTown();

		m_pHost->AddIdleTask(3000);
	}
	else
	{
		// 50米内有目标则进入战斗状态，否则进入巡逻状态
		GameObject* pGame = m_pHost->GetGameObject();
		if( pGame->GatherTarget(50.0f, m_pHost->GetForceAttack()) != -1 )
			SetCurState(AI_STATE_COMBAT);
		else
			SetCurState(AI_STATE_PATROL);
	}
}

// 玩家巡逻状态
void AIObject::Tick_Patrol()
{
	A3DVECTOR3 vNextPos;
	A3DVECTOR3 vCurPos = m_pHost->GetCurPos();
	BYTE byDir = m_pHost->GetDirection();
	float fRunSpeed = m_pHost->GetRunSpeed();

	// 如果50米内有目标进入战斗状态
	GameObject* pGame = m_pHost->GetGameObject();
	int target = pGame->GatherTarget(50.0f, m_pHost->GetForceAttack());
	if( target != -1 )
	{
		SetCurState(AI_STATE_COMBAT);
		return;
	}

	// 随机移动
	if( m_pHost->GetNextMovePos(vCurPos,byDir,fRunSpeed,vNextPos) )
	{
		float fDist = (vNextPos - vCurPos).Magnitude();
		int use_time = (int)(fDist / fRunSpeed * 1000);
		if( use_time < 100 ) use_time = 100;
		if( use_time > 1000 ) use_time = 1000;
		m_pHost->AddMoveTask(vNextPos, use_time, fRunSpeed, 0x21);
	}
	else
	{
		if( m_pHost->IsMoving() )
		{
			m_pHost->AddStopMoveTask(vCurPos, 100, fRunSpeed, 0x21);
		}

		if( a_Random(0, 2) == 0 )
			m_pHost->SetDirection(a_Random(0, 255));
	}

	if( m_pHost->IsCurTaskEmpty() )
		m_pHost->AddIdleTask(1000);
}

// 玩家跟随某一目标
void AIObject::Tick_Follow()
{
	GameObject* pGame = m_pHost->GetGameObject();
	GameObject::ObjectInfo* obj = pGame->GetEntity(m_iFollowID);
	if( !obj )
	{
		SetCurState(AI_STATE_IDLE);
		return;
	}

	A3DVECTOR3 next_pos;
	A3DVECTOR3 vCurPos = m_pHost->GetCurPos();
	float fRunSpeed = m_pHost->GetRunSpeed();

	float min_dis = 2.0f * 2.0f;
	if( (obj->pos - vCurPos).SquaredMagnitude() <= min_dis )
		return;

	if( m_pHost->GetNextMovePos(vCurPos,obj->pos,fRunSpeed,next_pos) )
	{
		float distance = (next_pos - vCurPos).Magnitude();
		int use_time = (int)(distance / fRunSpeed * 1000);
		if( use_time < 100 ) use_time = 100;
		if( use_time > 1000 ) use_time = 1000;

		bool stop = (next_pos - obj->pos).SquaredMagnitude() < min_dis;
		if( stop )
		{
			m_pHost->AddStopMoveTask(next_pos, use_time, fRunSpeed, 0x21);
		}
		else
		{
			m_pHost->AddMoveTask(next_pos, use_time, fRunSpeed, 0x21);
		}
	}
	else
	{
		if( m_pHost->IsMoving() )
		{
			m_pHost->AddStopMoveTask(next_pos, 100, fRunSpeed, 0x21);
		}
	}
	
	if( m_pHost->IsCurTaskEmpty() )
	{
		m_pHost->AddIdleTask(1000);
	}
}

// 玩家随机进行战斗
void AIObject::Tick_Combat()
{
	GameObject* pGame = m_pHost->GetGameObject();
	char force_attack = m_pHost->GetForceAttack();
	int sel_target = m_pHost->GetSelectedTarget();

	A3DVECTOR3 vCurPos = m_pHost->GetCurPos();
	float fRunSpeed = m_pHost->GetRunSpeed();

	if( sel_target == -1 )
	{
		int iTarget = pGame->GatherTarget(50.0f, force_attack);
		if( iTarget != -1 )
		{
			m_iExtData = 0;

			if( ISPLAYERID(iTarget) || ISNPCID(iTarget) )
				m_pHost->SelectTarget(iTarget);
			else if( ISMATTERID(iTarget) )
				m_pHost->SelectMatter(iTarget);
		}
		else
		{
			// 50米范围内没找到目标，进入巡逻状态
			SetCurState(AI_STATE_PATROL);
			return;
		}

		if( m_pHost->IsMoving() )
		{
			m_pHost->AddStopMoveTask(vCurPos, 100, fRunSpeed, 0x21);
		}
	}
	else
	{
		GameObject::ObjectInfo* obj = pGame->GetEntity(sel_target);
		if( obj )
		{
			float min_dis = 0.0f;
			int use_skill = 0;
			if( ISPLAYERID(obj->id) || ISNPCID(obj->id) )
				use_skill = m_pHost->SelectAttackSkill(min_dis);
			else if( ISMATTERID(obj->id) )
				min_dis = 1.5f * 1.5f;

			if( (vCurPos - obj->pos).SquaredMagnitude() < min_dis )
			{
				if( ISPLAYERID(obj->id) || ISNPCID(obj->id) )
				{
					if( use_skill )
						m_pHost->CastSkill(use_skill, force_attack, 1, &obj->id);
					else
						m_pHost->NormalAttack(force_attack);
				}
				else if( ISMATTERID(obj->id) )
				{
					m_pHost->Pickup(obj->id, obj->tid);

					// 如果连拣三次都失败，则放弃
					// 并加入无效列表，防止再次捡取
					if( m_pHost->IsCurTaskEmpty() && ++m_iExtData == 3 )
					{
						m_pHost->Unselect();
						m_pHost->GetGameObject()->AddInvalidMatter(obj->id);
					}
				}
			}
			else
			{
				// 距离过远
				A3DVECTOR3 next_pos;
				if( m_pHost->GetNextMovePos(vCurPos,obj->pos,fRunSpeed,next_pos) )
				{
					float distance = (next_pos - vCurPos).Magnitude();
					int use_time = (int)(distance / fRunSpeed * 1000);
					if( use_time < 100 ) use_time = 100;
					if( use_time > 1000 ) use_time = 1000;

					bool stop = (next_pos - obj->pos).SquaredMagnitude() < min_dis;
					if( stop )
					{
						m_pHost->AddStopMoveTask(next_pos, use_time, fRunSpeed, 0x21);
					}
					else
					{
						m_pHost->AddMoveTask(next_pos, use_time, fRunSpeed, 0x21);
					}
				}
				else
				{
					if( m_pHost->IsMoving() )
					{
						m_pHost->AddStopMoveTask(vCurPos, 100, fRunSpeed, 0x21);
					}
				}
			}
		}
		else
		{
			m_pHost->Unselect();

			if( m_pHost->IsMoving() )
			{
				m_pHost->AddStopMoveTask(vCurPos, 100, fRunSpeed, 0x21);
			}
		}
	}

	// 检查羽芒弹药，不足则补充
	if( m_pHost->GetProfession() == PROF_ARCHOR && !m_pHost->IsEquipExist(410, 11) )
	{
		SetCurState(AI_STATE_BORN);
	}

	if( m_pHost->IsCurTaskEmpty() )
	{
		m_pHost->AddIdleTask(1000);
	}
}

// AI接受跟随命令
void AIObject::OnCmd_Follow( StringVector& cmd )
{
	int roleid = 0;
	if( cmd.size() >= 2 ) roleid = atoi(cmd[1]);
	m_iFollowID = roleid;
}

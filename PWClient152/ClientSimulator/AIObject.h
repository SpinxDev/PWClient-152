/*
 * FILE: AIObject.h
 *
 * DESCRIPTION: 本文件定义了一个默认的玩家AI策略
 *
 * CREATED BY: Shizhenhua, 2012/9/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once


#include <ABaseDef.h>
#include <vector.h>
#include <hashmap.h>
#include <A3DVector.h>
#include "Common.h"


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

class PlayerObject;


///////////////////////////////////////////////////////////////////////////
//  
//  class AIObject
//  
///////////////////////////////////////////////////////////////////////////

class AIObject
{
	enum
	{
		AI_STATE_BORN,		// 进入游戏，提高自己能力
		AI_STATE_IDLE,		// 空闲状态
		AI_STATE_PATROL,	// 四处游荡
		AI_STATE_FOLLOW,	// 跟踪目标
		AI_STATE_COMBAT,	// 进入战斗
		AI_STATE_DEAD,		// 死亡

		AI_STATE_NUM,
	};

	typedef void (AIObject::*FN_AICMD_PROC) ( StringVector& cmd );
	typedef abase::hash_map<AString,FN_AICMD_PROC> AICommandMap;

public:
	AIObject( PlayerObject* pPlayer );
	virtual ~AIObject();

	void Enter();	// 玩家进入本策略
	void Tick();
	void Leave();	// 玩家离开本策略

	// 接受命令
	void Command( StringVector& cmd );

	int GetCurState() { return m_iState; }
	void SetCurState(int iState);

	PlayerObject* GetHostPlayer() { return m_pHost; }

protected:
	PlayerObject* m_pHost;
	int m_iState;
	int m_iExtData;
	int m_iFollowID;		// 跟随者ID

	AICommandMap m_AICommand;

protected:

	// State handlers
	void Tick_Idle();
	void Tick_Dead();
	void Tick_Patrol();
	void Tick_Follow();
	void Tick_Combat();
	void Tick_Born();

	// AI command handlers
	void OnCmd_Follow( StringVector& cmd );
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////

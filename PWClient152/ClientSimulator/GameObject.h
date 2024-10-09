/*
 * FILE: GameClient.h
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
#include <A3DVector.h>
#include "PlayerObject.h"
#include <set>


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

class GameSession;


///////////////////////////////////////////////////////////////////////////
//  
//  class GameObject
//  
///////////////////////////////////////////////////////////////////////////

class GameObject
{
public:
	struct ObjectInfo
	{
		int id;
		A3DVECTOR3 pos;
		int hp;
		int tid;	// NPC, 和Matter
		bool dead;	// 只针对玩家
		
		ObjectInfo() : id(-1), pos(0,0,0), hp(0), tid(0), dead(false) {}
	};

	typedef abase::hash_map<int,ObjectInfo> ObjectTable;

	enum
	{
		TargetElsePlayer	= 0x1,
		TargetNPC			= 0x2,
		TargetMatter		= 0x4,

		TargetAll			= 0xff,
	};

public:
	GameObject( GameSession* pSession );
	virtual ~GameObject();

	void Tick();

	// 在指定范围内寻找一个目标(ElsePlayer, NPC, Matter)
	int GatherTarget( int target_mask, float fRange, char bForceAttack );
	int GatherTarget( float fRange, char bForceAttack ) { return GatherTarget(TargetAll, fRange, bForceAttack); }

	// 物品不可用，以后不能再被选择到
	void AddInvalidMatter(int id);

	// 响应S2C协议
	void OnS2CCommand(WORD cmd, void* pBuf, size_t size);

	// 根据TID查找指定的NPC
	int SearchNPC(int tid);

	ObjectInfo* GetEntity(int id);
	ObjectInfo* GetPlayer(int id);
	ObjectInfo* GetNPC(int id);
	ObjectInfo* GetMatter(int id);

	PlayerObject* GetHostPlayer() { return m_pHostPlayer; }
	GameSession* GetGameSession() { return m_pSession; }

protected:
	GameSession* m_pSession;		// 隶属于哪个Session
	PlayerObject* m_pHostPlayer;	// 主玩家对象
	ObjectTable m_Objects;			// 玩家周围的对象（玩家、怪物、物品）
	std::set<int> m_BadMatters;		// 无效的物体列表（不会被选取）

protected:

};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////

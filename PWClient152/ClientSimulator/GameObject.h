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
		int tid;	// NPC, ��Matter
		bool dead;	// ֻ������
		
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

	// ��ָ����Χ��Ѱ��һ��Ŀ��(ElsePlayer, NPC, Matter)
	int GatherTarget( int target_mask, float fRange, char bForceAttack );
	int GatherTarget( float fRange, char bForceAttack ) { return GatherTarget(TargetAll, fRange, bForceAttack); }

	// ��Ʒ�����ã��Ժ����ٱ�ѡ��
	void AddInvalidMatter(int id);

	// ��ӦS2CЭ��
	void OnS2CCommand(WORD cmd, void* pBuf, size_t size);

	// ����TID����ָ����NPC
	int SearchNPC(int tid);

	ObjectInfo* GetEntity(int id);
	ObjectInfo* GetPlayer(int id);
	ObjectInfo* GetNPC(int id);
	ObjectInfo* GetMatter(int id);

	PlayerObject* GetHostPlayer() { return m_pHostPlayer; }
	GameSession* GetGameSession() { return m_pSession; }

protected:
	GameSession* m_pSession;		// �������ĸ�Session
	PlayerObject* m_pHostPlayer;	// ����Ҷ���
	ObjectTable m_Objects;			// �����Χ�Ķ�����ҡ������Ʒ��
	std::set<int> m_BadMatters;		// ��Ч�������б����ᱻѡȡ��

protected:

};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////

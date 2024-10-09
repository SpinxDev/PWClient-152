/*
 * FILE: EC_Monster.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_NPC.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

struct MONSTER_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECMonster
//	
///////////////////////////////////////////////////////////////////////////

class CECMonster : public CECNPC
{
public:		//	Types

public:		//	Constructor and Destructor

	CECMonster(CECNPCMan* pNPCMan);
	virtual ~CECMonster();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	virtual bool Init(int tid, const S2C::info_npc& Info);

	//	Get NPC name color
	virtual DWORD GetNameColor();
	//	Is monster in invader camp in battle ?
	virtual bool IsInBattleInvaderCamp();
	//	Is monster in defender camp in battle ?
	virtual bool IsInBattleDefenderCamp();
	//	Get role in battle
	virtual int GetRoleInBattle();

	virtual const MONSTER_ESSENCE * GetMonsterEssence()const{ return m_pDBEssence; }

	//  Is immune text disable ?
	bool IsImmuneDisable();
	float GetTransparentLimit();

	const MONSTER_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	MONSTER_ESSENCE*	m_pDBEssence;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////




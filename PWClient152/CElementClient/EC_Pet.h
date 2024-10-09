/*
 * FILE: EC_Pet.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/12/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
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

struct PET_ESSENCE;

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

class CECPet : public CECNPC
{
public:		//	Types

public:		//	Constructor and Destructor

	CECPet(CECNPCMan* pNPCMan);
	virtual ~CECPet();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	virtual bool Init(int tid, const S2C::info_npc& Info);

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport, int iRenderFlag=0);

	//	Get NPC name color
	virtual DWORD GetNameColor();	
	
	virtual bool IsImmuneDisable();
	virtual float GetTransparentLimit();
	virtual bool ShouldHideName()const;

	//	Is this host player's pet
	bool IsHostPet();
	//	Is this pet attackable;
	bool CanBeAttacked();
	//	Is a follow pet ?
	bool IsFollowPet()const;
	//	Is a combat pet ?
	bool IsCombatPet()const;
	//	是否为召唤宠
	bool IsSummonPet()const;
	//	是否为植物宠
	bool IsPlantPet()const;
	// 是否是进化宠
	bool IsEvolutionPet() const;

	const PET_ESSENCE* GetDBEssence() const{ return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	PET_ESSENCE*	m_pDBEssence;
	ACString		m_strCompName;		//	Complete name with master's name

protected:	//	Operations

	//	Build complete name
	void BuildCompleteName();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////




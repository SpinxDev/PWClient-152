/*
 * FILE: EC_ComboSkill.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/12/26
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_Configs.h"

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

class CECHostPlayer;
struct SkillArrayWrapper;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECComboSkill
//	
///////////////////////////////////////////////////////////////////////////

class CECComboSkill
{
public:		//	Types

	//	Special skill id
	enum
	{
		SID_ATTACK		= -1,	//	Normal attack
		SID_LOOPSTART	= -2,	//	Loop start flag
	};

public:		//	Constructor and Destructor

	CECComboSkill();
	virtual ~CECComboSkill() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(CECHostPlayer* pHost, int iGroup, int idTarget, bool bForceAttack, bool bIgnoreAtkLoop);
	//	Continue combo skill
	bool Continue(bool bMeleeing);
	//	Get id of skill next Continue() will trigger
	int GetNextSkill();
	//	Get stop flag
	bool IsStop() { return m_bStop; }
	
	//	Replace skill id
	void ReplaceSkillID(int idOld, int idNew);
	void ReplaceSkillID(const SkillArrayWrapper &idOlds, int idNew);

	bool FindSkillID(int idSkill)const;

	//  Get the target
	int GetTarget() const { return m_idTarget; }
	//  Do we ignore the attack and loop flag ?
	bool IsIgnoreAtkLoop() const { return m_bIgnoreAtkLoop; }

	//  Get group index
	int GetGroupIndex() const { return m_iGroup; }

protected:	//	Attributes

	CECHostPlayer*	m_pHost;	//	Host player object
	EC_COMBOSKILL	m_cs;		//	Combo skill data

	int		m_iGroup;		//	Group index
	int		m_iCursor;
	bool	m_bStop;		//	Stop flag
	int		m_iLoopStart;	//	Loop start index
	int		m_idTarget;		//	Attack target
	bool	m_bForceAtk;	//	Force attack flag
	bool	m_bIgnoreAtkLoop;	//  Ignore attack and loop flag

protected:	//	Operations

	//	Step cursor
	void StepCursor(bool bFirst);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////




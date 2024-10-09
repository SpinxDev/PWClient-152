/*
 * FILE: EC_HPWorkSpell.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_HPWork.h"
#include "EC_Counter.h"

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

class CECSkill;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkSpell
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkSpell : public CECHPWork
{
public:		//	Types

	//	Spell magic state
	enum
	{
		ST_INCANT = 0,
		ST_SPELL,
	};

public:		//	Constructor and Destructor

	CECHPWorkSpell(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkSpell();

public:		//	Attributes

public:		//	Operations

	//	Prepare cast
	void PrepareCast(int idTarget, CECSkill* pSkill, int iIncantTime);
	//	Change state
	void ChangeState(int iState);
	//	Get state
	int GetState() { return m_iState; }
	CECSkill * GetSkill() const { return m_pSkill; }

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	//  Cancel work
	virtual void Cancel();

protected:	//	Attributes

	CECSkill*	m_pSkill;		//	Skill object
	CECCounter	m_OverTimeCnt;	//	Over time counter
	int			m_iState;
	int			m_idTarget;		//	Target id

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
};

//	根据技能 ID 匹配 CECHPWorkSpell
class CECHPWorkSpellMatcher : public CECHPWorkMatcher{
	CECSkill * m_pSkill;
public:
	CECHPWorkSpellMatcher(CECSkill * pSkill) : m_pSkill(pSkill){
	}
	virtual bool operator()(CECHPWork *pWork, int priority, bool isDelayWork)const{
		bool bMatching(false);
		if (pWork && pWork->GetWorkID() == CECHPWork::WORK_SPELLOBJECT){
			CECHPWorkSpell *pWorkSpell = dynamic_cast<CECHPWorkSpell *>(pWork);
			if (m_pSkill == pWorkSpell->GetSkill()){
				bMatching = true;
			}
		}
		return bMatching;
	}
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkSkillStateAction
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkSkillStateAction : public CECHPWork
{
public:		//	Types
	
public:		//	Constructor and Destructor
	
	CECHPWorkSkillStateAction(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkSkillStateAction();
	
public:		//	Attributes
	
public:		//	Operations
	
	void SetSkill(int skill) { m_skill = skill;}
	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	//  Cancel work
	virtual void Cancel();
	
protected:	//	Attributes
	
	int			m_skill;
	CECCounter	m_OverTimeCnt;	//	Over time counter
	
protected:	//	Operations
	
	//	On first tick
	virtual void OnFirstTick();
};
///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


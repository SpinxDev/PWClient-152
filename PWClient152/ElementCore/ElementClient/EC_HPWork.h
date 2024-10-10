/*
 * FILE: EC_HPWork.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/2/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_ObjectWork.h"
#include "AArray.h"
#include <vector.h>

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
class CECHPWorkMan;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWork
//	
///////////////////////////////////////////////////////////////////////////
class CECHPWork : public CECObjectWork
{
public:		//	Types

	//	Host work ID
	enum
	{
		WORK_INVALID = -1,
		WORK_STAND = 0,		//	Stand and do nothing
		WORK_MOVETOPOS,		//	Move to a destination terrain position
		WORK_TRACEOBJECT,	//	Trace specified object (NPC, player etc.)
		WORK_HACKOBJECT,	//	Hack specified object (NPC, player etc.)
		WORK_SPELLOBJECT,	//	Spell magic on specified object (NPC, player etc.)
		WORK_USEITEM,		//	Host use item
		WORK_DEAD,			//	Host is dead
		WORK_FOLLOW,		//	Follow some one else
		WORK_FLYOFF,		//	Fly off
		WORK_FREEFALL,		//	Free fall
		WORK_SIT,			//	Sit
		WORK_PICKUP,		//	Pickup item
		WORK_CONCENTRATE,	//	All concentrate work
		WORK_REVIVE,		//	Revive
		WORK_FLASHMOVE,		//	Flash move
		WORK_BEBOUND,		//	Be bound by buddy
		WORK_PASSIVEMOVE,	//	Passive movement (knock back / pull)
		WORK_CONGREGATE,	//	Congregate reply
		WORK_SKILLSTATEACT,	//	skill buff action
		WORK_FORCENAVIGATEMOVE,		//	force navigate move
		NUM_WORK,
	};

protected:
	//	Work mask
	enum
	{
		MASK_STAND			= 0x0001,
		MASK_MOVETOPOS		= 0x0002,
		MASK_TRACEOBJECT	= 0x0004,
		MASK_HACKOBJECT		= 0x0008,
		MASK_SPELLOBJECT	= 0x0010,
		MASK_USEITEM		= 0x0020,
		MASK_DEAD			= 0x0040,
		MASK_FOLLOW			= 0x0080,
		MASK_FLYOFF			= 0x0100,
		MASK_FREEFALL		= 0x0200,
		MASK_SIT			= 0x0400,
		MASK_PICKUP			= 0x0800,
		MASK_CONCENTRATE	= 0x1000,
		MASK_REVIVE			= 0x2000,
		MASK_FLASHMOVE		= 0x4000,
		MASK_BEBOUND		= 0x8000,
		MASK_CONGREGATE		= 0x10000,
		MASK_SKILLSTATEACT	= 0x20000,
		MASK_FORCENAVIGATE	= 0x40000,
		MASK_ALLWORK		= 0xffffffff,
	};

public:		//	Constructor and Destructor

	CECHPWork(int iWorkID, CECHPWorkMan* pWorkMan);
	virtual ~CECHPWork() {}

public:		//	Attributes

public:		//	Operations

	//	ÅÉÉú×Ô CECObjectWork
	virtual void Cancel();

	//	This work is do player moving ?
	virtual bool IsMoving() { return false; }
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);

	const char *GetWorkName()const;
	static const char * GetWorkName(int iWorkID);
	static unsigned int GetWorkMask(int iWorkID);
	static bool Validate(int iWorkID);

protected:	//	Attributes

	CECHPWorkMan*	m_pWorkMan;	//	Work manager
	CECHostPlayer*	m_pHost;	//	Host player object

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkMatcher
//	
///////////////////////////////////////////////////////////////////////////
class CECHPWorkMatcher
{
public:
	virtual ~CECHPWorkMatcher(){}
	virtual bool operator()(CECHPWork *pWork, int priority, bool isDelayWork)const=0;
};

class CECHPWorkIDMatcher : public CECHPWorkMatcher{
	int		m_workID;
public:
	explicit CECHPWorkIDMatcher(int workID)
		: m_workID(workID)
	{
	}
	virtual bool operator()(CECHPWork *pWork, int priority, bool isDelayWork)const{
		return pWork != NULL && pWork->GetWorkID() == m_workID;
	}
};

class CECHPWorkPriorityMatcher : public CECHPWorkMatcher{
	int		m_priority;
public:
	explicit CECHPWorkPriorityMatcher(int priority) 
		: m_priority(priority)
	{
	}
	virtual bool operator()(CECHPWork *pWork, int priority, bool isDelayWork)const{
		return pWork != NULL
			&& m_priority == priority;
	}
};

class CECHPWorkDelayMatcher : public CECHPWorkMatcher{
	bool	m_matchDelayedWork;
public:
	explicit CECHPWorkDelayMatcher(bool bMatchDelayedWork)
		: m_matchDelayedWork(bMatchDelayedWork)
	{
	}
	virtual bool operator()(CECHPWork *pWork, int priority, bool isDelayWork)const{
		return pWork != NULL && m_matchDelayedWork == isDelayWork;
	}
};

class CECHPWorkGeneralMatcher : public CECHPWorkMatcher{
	int		m_workID;
	int		m_priority;
	bool	m_matchDelayedWork;
public:
	CECHPWorkGeneralMatcher(int workID, int priority, bool bIncludeDelayedWork)
		: m_workID(workID)
		, m_priority(priority)
		, m_matchDelayedWork(bIncludeDelayedWork)
	{
	}
	virtual bool operator()(CECHPWork *pWork, int priority, bool isDelayWork)const{
		return pWork != NULL
			&& pWork->GetWorkID() == m_workID
			&& priority == m_priority
			&& isDelayWork == m_matchDelayedWork;
	}
};

class CECWorkCompositeMatcher : public CECHPWorkMatcher{
	CECHPWorkMatcher	&	m_matcher1;
	CECHPWorkMatcher	&	m_matcher2;
public:
	CECWorkCompositeMatcher(CECHPWorkMatcher &matcher1, CECHPWorkMatcher &matcher2)
		: m_matcher1(matcher1)
		, m_matcher2(matcher2)
	{
	}
	virtual bool operator()(CECHPWork *pWork, int priority, bool isDelayWork)const{
		return m_matcher1(pWork, priority, isDelayWork)
			&& m_matcher2(pWork, priority, isDelayWork);
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkMan
//	
///////////////////////////////////////////////////////////////////////////
class CECHPWorkSpell;
class CECHPWorkTrace;
class CECHPWorkPostTickCommand;
class CECNPC;
class CECHPWorkMan
{
public:		//	Types

	friend class CECHostPlayer;

	//	Work priority
	enum
	{
		PRIORITY_0 = 0,
		PRIORITY_1,
		PRIORITY_2,
		NUM_PRIORITY,
	};

	//	Delayed work info.
	struct DELAYWORK
	{
		int			iPriority;
		CECHPWork*	pWork;
	};

public:
	CECHPWorkMan(CECHostPlayer* pHost);
	virtual ~CECHPWorkMan();

public:
	bool ValidatePriority(int iPriority)const;

	bool StartWork(int iPriority, CECHPWork* pWork, bool bNoDelay=false);
	bool StartWork_p0(CECHPWork* pWork, bool bNoDelay=false) { return StartWork(PRIORITY_0, pWork, bNoDelay); }
	bool StartWork_p1(CECHPWork* pWork, bool bNoDelay=false) { return StartWork(PRIORITY_1, pWork, bNoDelay); }
	bool StartWork_p2(CECHPWork* pWork, bool bNoDelay=false) { return StartWork(PRIORITY_2, pWork, bNoDelay); }

	void FinishWork(const CECHPWorkMatcher &matcher);
	void FinishRunningWork(int idWork);
	void FinishAllWork(bool bGotoStand);
	
	CECHPWork* GetRunningWork(int iWorkID)const;
	CECHPWork* GetWork(int iWorkID, int iPriority=PRIORITY_1, bool bIncludeDelayedWork=false)const;
	CECHPWork* GetWork(const CECHPWorkMatcher &matcher)const;
	
	bool IsWorkRunning(int iWorkID)const;
	bool IsAnyWorkRunning()const;

	bool IsSpellingMagic()const;
	bool IsPickingUp()const;
	bool IsSitting()const;
	bool IsStanding()const;
	bool IsUsingItem()const;
	bool IsFlashMoving()const;
	bool IsPassiveMoving()const;
	bool IsFlyingOff()const;
	bool IsReviving()const;
	bool IsFollowing()const;
	bool IsTracing()const;
	bool IsOperatingPet()const;
	bool IsCongregating()const;
	bool IsPlayingSkillAction()const;
	bool IsMovingToPosition()const;
	bool IsFreeFalling()const;
	bool IsDead()const;
	bool IsBeingBound()const;

	bool IsMoving()const;

	bool HasDelayedWork()const;
	bool HasWorkOnPriority(int iPriority)const;
	bool HasWorkRunningOnPriority(int iPriority)const;

	bool CanStartWork(int iWorkID, int iPriority = PRIORITY_1)const;
	bool CanCastSkillImmediately(int idSkill)const;

	CECHPWork* CreateWork(int idWork);
	CECHPWorkTrace *CreateNPCTraceWork(CECNPC *pNPC, int relatedTaskID);

	CECHostPlayer* GetHostPlayer() { return m_pHost; }

	void Tick(DWORD dwDeltaTime);
	void SetPostTickCommand(CECHPWorkPostTickCommand *command);

private:
	CECHostPlayer*	m_pHost;

	typedef abase::vector<CECHPWork *> WorkList;
	WorkList	m_WorkStack[NUM_PRIORITY];
	int			m_iCurPriority;
	DELAYWORK	m_Delayed;

	CECHPWorkPostTickCommand *	m_pPostTickCommand;

private:
	bool InternallyStartWork(int iPriority, CECHPWork *pWork);
	bool CanRunSimultaneouslyWithCurrentWork(int iPriority, const CECHPWork *pWork)const;
	bool CanRunSimultaneously(const CECHPWork *pWork1, const CECHPWork *pWork2)const;
	bool CanSpellWhileMoving(const CECHPWorkSpell *pWorkSpell)const;
	void StartAwaitingWorks();
	
	void FinishWorkAtPriority(int iPriority);
	void KillWork(int iPriority, int index);
	void KillWork(CECHPWork *pWork);

	void CancelWork(CECHPWork *pWork);
	void CancelWorkAtPriority(int iPriority);

	bool DelayWork(int iPriority, CECHPWork* pWork);
	void StartDelayedWork();
	void ClearDelayedWork();
	CECHPWork* GetDelayedWork()const{ return m_Delayed.pWork; }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkPostTickCommand
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkPostTickCommand{
public:
	virtual ~CECHPWorkPostTickCommand(){}
	virtual bool Run(CECHPWorkMan *pWorkMan)=0;
};

class CECHPWork;
class CECHPWorkMan;
class CECHPWorkPostTickRunWorkCommand : public CECHPWorkPostTickCommand{
	CECHPWork *		m_pWork;
	int				m_iPriority;
	bool			m_bNoDelay;
	bool			m_bShouldTick;
	DWORD			m_dwTickTime;
	
	CECHPWorkPostTickRunWorkCommand(const CECHPWorkPostTickRunWorkCommand &);
	CECHPWorkPostTickRunWorkCommand & operator=(const CECHPWorkPostTickRunWorkCommand &);
	
public:
	CECHPWorkPostTickRunWorkCommand(CECHPWork *pWork, bool bNoDelay=false, int iPriority=CECHPWorkMan::PRIORITY_1, bool bShouldTick=false, DWORD dwTickTime=0);
	virtual ~CECHPWorkPostTickRunWorkCommand();
	virtual bool Run(CECHPWorkMan *pWorkMan);
};
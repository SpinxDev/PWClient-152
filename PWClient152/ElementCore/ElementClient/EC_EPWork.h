// Filename	: EC_EPWork.h
// Creator	: Xu Wenbin
// Date		: 2014/8/6

#ifndef _ELEMENTCLIENT_EC_EPWORK_H_
#define _ELEMENTCLIENT_EC_EPWORK_H_

#include <ABaseDef.h>
#include <A3DVector.h>
#include <vector.h>

#include "EC_Counter.h"

class CECElsePlayer;
class CECEPWorkMan;
class CECEPWork{	
public:	
	enum
	{
		WORK_INVALID = -1,
		WORK_STAND = 0,		//	Stand and do nothing
		WORK_MOVE,			//	Move
		WORK_HACKOBJECT,	//	Hack specified object (NPC, player etc.)
		WORK_SPELL,			//	Spell magic
		WORK_PICKUP,		//	Pickup matter
		WORK_DEAD,			//	Dead
		WORK_USEITEM,		//	Use item
		WORK_IDLE,			//	Idle works
		WORK_FLASHMOVE,		//	Flash move
		WORK_PASSIVEMOVE,	//	Move controlled by server
		WORK_CONGREGATE,	//	Congregate reply
		WORK_SKILLSTATEACT, //	skill buff action
		NUM_WORK,
	};	
	
	//	Idle work type
	enum
	{
		IDLE_SITDOWN = 0,
		IDLE_REVIVE,
		IDLE_BOOTH,
	};

public:	
	CECEPWork(int iWorkID, CECEPWorkMan* pWorkMan);
	virtual ~CECEPWork(){}
	
	const char *GetWorkName()const;
	int GetWorkID()const;
	bool IsFinished()const;

	virtual void Start();
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Cancel();
	virtual void Finish();
	
	static const char * GetWorkName(int iWork);
	static bool Validate(int iWork);

protected:
	CECElsePlayer * GetPlayer()const;

protected:
	int				m_iWorkID;
	CECEPWorkMan *	m_pWorkMan;
	bool			m_bFinished;

private:
	CECEPWork(const CECEPWork &);
	CECEPWork & operator = (const CECEPWork &);
};

class CECEPWorkMatcher
{
public:
	virtual ~CECEPWorkMatcher(){}
	virtual bool operator()(CECEPWork *pWork, int iWorkType)const=0;
};

class CECEPWorkIDMatcher : public CECEPWorkMatcher{
	int		m_workID;
public:
	explicit CECEPWorkIDMatcher(int workID)
		: m_workID(workID)
	{
	}
	virtual bool operator()(CECEPWork *pWork, int iWorkType)const{
		return pWork != NULL && pWork->GetWorkID() == m_workID;
	}
};

class CECEPWorkTypeMatcher : public CECEPWorkMatcher{
	int		m_iWorkType;
public:
	explicit CECEPWorkTypeMatcher(int iWorkType) 
		: m_iWorkType(iWorkType)
	{
	}
	virtual bool operator()(CECEPWork *pWork, int iWorkType)const{
		return pWork != NULL
			&& m_iWorkType == iWorkType;
	}
};

class CECEPWorkGeneralMatcher : public CECEPWorkMatcher{
	int		m_workID;
	int		m_iWorkType;
public:
	CECEPWorkGeneralMatcher(int workID, int iWorkType)
		: m_workID(workID)
		, m_iWorkType(iWorkType)
	{
	}
	virtual bool operator()(CECEPWork *pWork, int iWorkType)const{
		return pWork != NULL
			&& pWork->GetWorkID() == m_workID
			&& iWorkType == m_iWorkType;
	}
};

class CECWorkCompositeMatcher : public CECEPWorkMatcher{
	CECEPWorkMatcher	&	m_matcher1;
	CECEPWorkMatcher	&	m_matcher2;
public:
	CECWorkCompositeMatcher(CECEPWorkMatcher &matcher1, CECEPWorkMatcher &matcher2)
		: m_matcher1(matcher1)
		, m_matcher2(matcher2)
	{
	}
	virtual bool operator()(CECEPWork *pWork, int iWorkType)const{
		return m_matcher1(pWork, iWorkType)
			&& m_matcher2(pWork, iWorkType);
	}
};

//	class CECEPWorkMan
class CECEPWorkSpell;
class CECEPWorkMan{
public:
	//	Work type
	enum
	{
		WT_NOTHING = 0,		//	Do thing
		WT_NORMAL,			//	Normal type work
		WT_INTERRUPT,		//	Interrupt type work
		NUM_WORKTYPE,
	};
public:
	CECEPWorkMan(CECElsePlayer *pElsePlayer);
	~CECEPWorkMan();
	bool ValidateWorkType(int iWorkType)const;
	static const char * GetWorkTypeName(int iWorkType);
	
	CECElsePlayer * GetPlayer()const;
	int  GetCurrentWorkType()const;

	void Tick(DWORD dwDeltaTime);
	
	void StartNormalWork(CECEPWork *pWork);
	void StartWork(int iWorkType, CECEPWork *pWork);
	bool FindWork(int iWorkType, int iWorkID)const;
	
	bool IsWorkRunning(int iWork)const;
	CECEPWork* GetRunningWork(int iWorkID)const;
	CECEPWork* GetWork(int iWorkID)const;
	
	void FinishWork(const CECEPWorkMatcher &matcher);
	void FinishWork(int idWork);
	void FinishRunningWork(int idWork);
	bool FinishIdleWork(int iType);
	bool FinishCongregateWork(int iType);

private:
	void FinishWorkAtWorkType(int iWorkType);
	
	bool IsAnyWorkRunning()const;
	void KillWork(int iWorkType, int index);
	void StartAwaitingWorks();
	
	void CancelWork(CECEPWork *pWork);
	void CancelWorkAtWorkType(int iWorkType);
	
	bool CanMergeWithCurrentWork(int iWorkType, const CECEPWork *pWork);
	bool CanRunSimultaneously(const CECEPWork *pWork1, const CECEPWork *pWork2);
	bool CanReplace(const CECEPWork *pNewWork, const CECEPWork *pExistWork);
	bool CanSpellWhileMoving(const CECEPWorkSpell *pWorkSpell);

	void AppendWork(int iWorkType, CECEPWork *pWork);
	void MergeWork(int iWorkType, CECEPWork *pWork);

private:
	CECElsePlayer *	m_pElsePlayer;

	typedef abase::vector<CECEPWork*>	WorkList;
	WorkList	m_WorkStack[NUM_WORKTYPE];
	int			m_iCurWorkType;
};

//	class CECEPWorkStand
class CECEPWorkStand : public CECEPWork{
	int	GetStandAction()const;
public:
	CECEPWorkStand(CECEPWorkMan* pWorkMan);
	virtual void Start();
	virtual void Tick(DWORD dwDeltaTime);
};

//	class CECEPWorkMove
class CECEPWorkMove : public CECEPWork{
public:
	CECEPWorkMove(CECEPWorkMan* pWorkMan);
	virtual void Start();
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Cancel();
};

//	class CECEPWorkMelee
class CECEPWorkMelee : public CECEPWork{
	int		m_iAttackTarget;
public:
	CECEPWorkMelee(CECEPWorkMan* pWorkMan, int attackTarget);
	virtual void Start();
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Cancel();
};

//	class CECEPWorkSpell
class CECSkill;
class CECEPWorkSpell : public CECEPWork{
	CECCounter	m_SkillCnt;	
	CECSkill*	m_pCurSkill;
	int			m_idCurSkillTarget;
public:
	CECEPWorkSpell(CECEPWorkMan* pWorkMan, DWORD dwPeriod, CECSkill *pSkill, int target);
	virtual ~CECEPWorkSpell();
	virtual void Start();
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Cancel();
	CECSkill * GetSkill()const;
};

//	class CECEPWorkPickUp
class CECEPWorkPickUp : public CECEPWork{
public:
	enum Type{
		PICKUP_ITEM,
		GATHER_ITEM,
		GATHER_MONSTER_SPIRIT,
	};
private:
	CECCounter	m_PickupCnt;
	Type		m_type;
	int			m_gatherItemID;
public:
	CECEPWorkPickUp(CECEPWorkMan* pWorkMan, DWORD dwPeriod, Type type, int gatherItemID=0);
	virtual void Start();
	virtual void Tick(DWORD dwDeltaTime);
};

//	class CECEPWorkDead
class CECEPWorkDead : public CECEPWork{
	DWORD	m_dwParam;
public:
	CECEPWorkDead(CECEPWorkMan* pWorkMan, DWORD dwParam);
	virtual void Start();
};

//	class CECEPWorkUseItem
class CECEPWorkUseItem : public CECEPWork{
	int		m_itemID;
public:
	CECEPWorkUseItem(CECEPWorkMan* pWorkMan, int itemID);
	virtual void Start();
};

//	class CECEPWorkIdle
class CECEPWorkIdle : public CECEPWork{
	int			m_iType;
	bool		m_bOTCheck;
	CECCounter	m_OTCnt;
	DWORD		m_dwParam;
public:
	CECEPWorkIdle(CECEPWorkMan* pWorkMan, int iType, int iOTTime, DWORD dwParam);
	virtual void Tick(DWORD dwDeltaTime);
	int GetType()const;
};

//	class CECEPWorkFlashMove
class CECEPWorkFlashMove : public CECEPWork{
	A3DVECTOR3	m_vServerPos;
	float		m_fMoveSpeed;
public:
	CECEPWorkFlashMove(CECEPWorkMan* pWorkMan, const A3DVECTOR3 &vServerPos, float fMoveSpeed);
	virtual void Start();
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Cancel();
};

//	class CECEPWorkPassiveMove
class CECEPWorkPassiveMove : public CECEPWork{
	A3DVECTOR3	m_vServerPos;
	float		m_fMoveSpeed;
public:
	CECEPWorkPassiveMove(CECEPWorkMan* pWorkMan, const A3DVECTOR3 &vServerPos, float fMoveSpeed);
	virtual void Start();
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Cancel();
};

//	class CECEPWorkCongregate
class CECEPWorkCongregate : public CECEPWork{
	int		m_iType;
public:
	CECEPWorkCongregate(CECEPWorkMan* pWorkMan, int iType);
	virtual void Start();
	int GetType()const;
};

//	class CECEPWorkSkillStateAct
class CECEPWorkSkillStateAct : public CECEPWork{
	int		m_skillID;
public:
	CECEPWorkSkillStateAct(CECEPWorkMan* pWorkMan, int idSkill);
	virtual void Start();
};

#endif	//	_ELEMENTCLIENT_EC_EPWORK_H_
/*
 * FILE: EC_ActionSwitcher.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD, 2013/11/07
 *
 * HISTORY: 
 *
 * Copyright (c) 
 */

#ifndef _ECACTIONSWITCHER_H_
#define _ECACTIONSWITCHER_H_

#include <AList2.h>
#include <AArray.h>
///////////////////////////////////////////////////////////////////////////
//	
//	Class CECActionSwitcher
//	
///////////////////////////////////////////////////////////////////////////
class CECHostPlayer;
class CECActionSwitcherBase;
class CECActionBase;
class CECActionCancelRide;
class CECActionRide;
class CECActionLanding;
class CECActionFly;
class CECActionSkill;
class CECActionUseSkillItem;

class CECActionSequencedRelationFilter;
class CECCancelRideActionFilter;
class CECRideActionFilter;
class CECLandingActionFilter;
class CECFlyActionFilter;
class CECSkillActionFilter;
class CECUseSkillItemActionFilter;
class CECActionContext;
//////////////////////////////////////////////////////////////////////////
//					CECActionSequencedRelationFilter
// Filter 类用来控制新的action在加入到 swicher里时 的控制逻辑。
// CECActionSequencedRelationFilter 类实现当前 Ride-》fly， fly-》ride，ride-》skill, 通过前后顺序关系判断。
// 如果有新的action需要不同的控制，基于 CECFitlerBase 派生新的类
//////////////////////////////////////////////////////////////////////////

class CECFitlerBase
{
public:
	virtual ~CECFitlerBase(){};
	virtual bool CanAddAction(CECActionContext* pContext,CECActionBase* pAct) = 0;
};

class CECActionSequencedRelationFilter: public CECFitlerBase
{
public:
	virtual bool CanAddAction(CECActionContext* pContext,CECActionBase* pAct);
	virtual bool IsPredecessorTo(CECActionSequencedRelationFilter* ) = 0;

public:
	virtual bool IsSuccessorTo(CECCancelRideActionFilter* )		{return false;}
	virtual bool IsSuccessorTo(CECRideActionFilter* )			{return false;}
	virtual bool IsSuccessorTo(CECLandingActionFilter* )		{return false;}
	virtual bool IsSuccessorTo(CECFlyActionFilter* )			{return false;}
	virtual bool IsSuccessorTo(CECSkillActionFilter* )			{return false;}
	virtual bool IsSuccessorTo(CECUseSkillItemActionFilter* )	{return false;}
	
};

class CECCancelRideActionFilter: public CECActionSequencedRelationFilter
{
public:
	bool IsPredecessorTo(CECActionSequencedRelationFilter* pFilter)		{return pFilter->IsSuccessorTo(this);};
	bool IsSuccessorTo(CECRideActionFilter* )					{return true;}	
};

class CECFlyActionFilter : public CECActionSequencedRelationFilter
{
public:
	virtual bool IsSuccessorTo(CECCancelRideActionFilter* )		{return true;}
	virtual bool IsSuccessorTo(CECLandingActionFilter* )		{return true;}	
	virtual bool IsPredecessorTo(CECActionSequencedRelationFilter* pFilter) { return pFilter->IsSuccessorTo(this);}

private:
};

class CECLandingActionFilter: public CECActionSequencedRelationFilter
{
public:
	virtual bool IsSuccessorTo(CECFlyActionFilter* )			{return true;}
	bool IsPredecessorTo(CECActionSequencedRelationFilter* pFilter)		{return pFilter->IsSuccessorTo(this);}
};

class CECRideActionFilter: public CECActionSequencedRelationFilter
{
public:
	virtual bool IsSuccessorTo(CECCancelRideActionFilter* )		{return true;}	
	virtual bool IsSuccessorTo(CECLandingActionFilter* )		{return true;}
	bool IsPredecessorTo(CECActionSequencedRelationFilter* pFilter)		{return pFilter->IsSuccessorTo(this);}
};

class CECSkillActionFilter: public CECActionSequencedRelationFilter
{
public:
	virtual bool IsSuccessorTo(CECCancelRideActionFilter* )		{return true;}
	virtual bool IsSuccessorTo(CECLandingActionFilter* )		{return true;}
	virtual bool IsSuccessorTo(CECFlyActionFilter* )			{return true;}
	bool IsPredecessorTo(CECActionSequencedRelationFilter* pFilter)		{return pFilter->IsSuccessorTo(this);}
};

class CECUseSkillItemActionFilter: public CECActionSequencedRelationFilter
{
public:
	virtual bool IsSuccessorTo(CECCancelRideActionFilter* )		{return true;}
	virtual bool IsSuccessorTo(CECLandingActionFilter* )		{return true;}
	virtual bool IsSuccessorTo(CECFlyActionFilter* )			{return true;}
	bool IsPredecessorTo(CECActionSequencedRelationFilter* pFilter)		{return pFilter->IsSuccessorTo(this);}	
};

//////////////////////////////////////////////////////////////////////////
//					Action类
// 描述：具体的动作类

class CECActionBase
{
public:
	
	enum ACTION_RESULT
	{
		FD_NONE = 0,
		FD_SUCC,
		FD_FAIL,
	};
	
public:
	CECActionBase(CECHostPlayer* pHost);
	virtual ~CECActionBase(){ delete m_ActionFilter;};

	bool NeedBeRemoved();
	CECActionSequencedRelationFilter* GetActionFilter() { return m_ActionFilter;}
	
	virtual bool CanDo() = 0;
	virtual bool DoOnce() = 0;	

	virtual bool Update(DWORD dt);
	virtual bool CanAddTo(CECActionContext* pContext);

	
protected:
	CECHostPlayer* m_pHostPlayer;
	ACTION_RESULT m_iFirstDoRes;	
	CECActionSequencedRelationFilter* m_ActionFilter;
};

// 下马
class CECActionCancelRide : public CECActionBase
{
public:
	CECActionCancelRide(CECHostPlayer* pHost):CECActionBase(pHost){ m_ActionFilter = new CECCancelRideActionFilter();}
	bool CanDo();
	bool DoOnce();

};

// 起飞
class CECActionFly : public CECActionBase
{
public:
	CECActionFly(CECHostPlayer* pHost):CECActionBase(pHost){ m_ActionFilter = new CECFlyActionFilter();}
	bool CanDo();
	bool DoOnce();
};

// 降落
class CECActionLanding: public CECActionBase
{
public:
	CECActionLanding(CECHostPlayer* pHost):CECActionBase(pHost){ m_ActionFilter = new CECLandingActionFilter();}
	bool CanDo();
	bool DoOnce();
};

// 上马
class CECActionRide: public CECActionBase
{
public:
	CECActionRide(CECHostPlayer* pHost,int index):CECActionBase(pHost),m_index(index){ m_ActionFilter = new CECRideActionFilter();}
	bool CanDo();
	bool DoOnce();
protected:
	int m_index;
private:
};

// 放技能
class CECActionSkill: public CECActionBase
{
public:
	CECActionSkill(CECHostPlayer* pHost,int skill,bool bCom,int iSel,int iForceAtk);
	bool CanDo();
	bool DoOnce();
protected:
	int m_iSkill;
	bool m_bCommbo;
	int m_idSelect;
	int m_iForceAtk;
	bool m_bCastSkill;
private:
};

// 使用目标物品
class CECActionUseSkillItem: public CECActionBase
{
public:
	CECActionUseSkillItem(CECHostPlayer* pHost,int iPack,int iSlot,int tid);
	bool CanDo();
	bool DoOnce();

protected:
	int m_iPack;
	int m_iSlot;
	int m_tid;
};

class CECContextValidChecker;
//////////////////////////////////////////////////////////////////////////
//						CECActionContext
// 描述： context类用过管理逻辑上多个action。比如飞行切换到坐骑，需要两个action：CECActionLanding和CECActionRide。
class CECActionContext
{
public:
	enum CONTEXT
	{
		AC_NONE = 0,
		AC_RIDETOFLY,
		AC_RIDETOSKILL,
		AC_FLYTORIDE,
		AC_RIDETOUSETARGETITEM,
	};
public:
	CECActionContext(CECHostPlayer* pHost, CONTEXT c):m_pHostPlayer(pHost),m_context(c),m_bForceRemove(false),m_ErrorCtr(NULL){}
	virtual ~CECActionContext();
public:
	virtual bool Init(){ return false;}
	bool NeedBeRemoved();	
	virtual bool Update(DWORD dt); 
	virtual bool CanBeAddedTo(CECActionSwitcherBase* pSwitcher);

	CECActionBase* GetFirstAction();
	CECActionBase* GetLastAction();
	CECActionBase* GetAction(int index);
	int GetActionCount() { return m_actions.GetSize();}
	
	bool IsContext(CONTEXT c) { return m_context == c;}
	void SetForceRemove(bool v) { m_bForceRemove = v;}
	CECHostPlayer* GetHostPlayer() const { return m_pHostPlayer;}
	virtual bool NeedCheckValid() { return false;}

protected:
	virtual bool AddAction(CECActionBase* pAct);
protected:
	AArray<CECActionBase*> m_actions;
	CECHostPlayer* m_pHostPlayer;
	CONTEXT m_context;
	bool m_bForceRemove;
	CECContextValidChecker* m_ErrorCtr;
}; 

class CECContextValidChecker
{
public:
	CECContextValidChecker(CECActionContext* context):m_pActionContext(context),error_correct(0){}
	virtual void Update(DWORD dt) = 0;
protected:
	int error_correct;
	CECActionContext* m_pActionContext;
};

class CECContextValidCheckerForRide : public CECContextValidChecker
{
	enum
	{
		MAX_ERRORTIME = 3000,
	};
public:
	CECContextValidCheckerForRide(CECActionContext* context):CECContextValidChecker(context){}
	void Update(DWORD dt);
protected:
};

// ride -> fly
class CECRideToFlyContext: public CECActionContext
{
public:
	CECRideToFlyContext(CECHostPlayer* pHost):CECActionContext(pHost,AC_RIDETOFLY){}
	bool Init();
	bool NeedCheckValid();
protected:
};
// ride to cast skill
class CECRideToSkillContext: public CECActionContext
{
public:
	CECRideToSkillContext(CECHostPlayer* pHost,int skill,bool bCom,int iSel,int iForceAtk):CECActionContext(pHost,AC_RIDETOSKILL)
		,m_skill(skill)
		,m_iSel(iSel)
		,m_iForceAtk(iForceAtk){}
	bool Init();
	bool NeedCheckValid();
protected:
	int m_skill;
	bool m_bCom;
	int m_iSel;
	int m_iForceAtk;
};
// fly -> ride
class CECFlyToRideContext: public CECActionContext
{
public:
	CECFlyToRideContext(CECHostPlayer* pHost,int petIdx):CECActionContext(pHost,AC_FLYTORIDE),m_iPetIndex(petIdx){}
	bool Init();
protected:
	int m_iPetIndex;
};
// ride to use target item
class CECRideToUseTargetItemContext: public CECActionContext
{
public:
	CECRideToUseTargetItemContext(CECHostPlayer* pHost,int pack,int index,int tid):CECActionContext(pHost,AC_RIDETOUSETARGETITEM)
		,m_iPack(pack)
		,m_iSlot(index)
		,m_tid(tid){}
	bool Init();
	bool NeedCheckValid();
protected:
	int m_iPack;
	int m_iSlot;
	int m_tid;	
};

//////////////////////////////////////////////////////////////////////////
//					 CECActionSwitcher
// 描述：	CECActionSwitcherBase 类用来产生空的switcher，如果客户端禁止swicher功能，则产生一个CECActionSwitcherBase对象，
//			防止漏掉对CECHostplayer::m_pActionSwitcherd的非空测试

class CECActionSwitcherBase
{
public:
	enum 
	{
		MSG_FLY = 0,
		MSG_MOUNTPET,
		MSG_CASTSKILL,
	};

public:
	CECActionSwitcherBase(CECHostPlayer* pHost):m_pHostPlayer(pHost){}
	virtual ~CECActionSwitcherBase(){};

	void Clear();	

	int GetContextCount() const { return m_actionContexts.GetCount();}
	CECActionContext* GetContext(int index);
	CECActionContext* GetLastContext();
	bool CanAddMessage() const {return m_bCanAddMsg;}
	void SetPostMessageFlag(bool bCan){m_bCanAddMsg = bCan;}
	void PostMessge(int msg);

public:
	virtual void Tick(DWORD dt){};
	virtual bool OnRideToFlyAction()											{return false;} // ride -> fly
	virtual bool OnRideToSkillAction(int skill,bool bCom,int iSel,int iForceAtk){return false;} // ride -> skill
	virtual bool OnFlyToRideAction(int petIndex)								{return false;} // fly -> ride
	virtual bool OnRideToUseTargetItem(int iPack,int iSlot, int tid)			{return false;} // ride -> use target item
	virtual void ProcessMessage(){}

protected:
	virtual bool AddActionContext(CECActionContext* pContext)					{return false;}

protected:
	APtrList<CECActionContext*> m_actionContexts;
	CECHostPlayer* m_pHostPlayer;

	AArray<int,int> m_msgs;
	bool m_bCanAddMsg;

private:
};

// 具体的 switcher 功能实现
class CECActionSwitcher : public CECActionSwitcherBase
{
public:
	CECActionSwitcher(CECHostPlayer* pHost):CECActionSwitcherBase(pHost){}
	virtual ~CECActionSwitcher(){ Clear();}

	void Tick(DWORD dt);	
	bool OnRideToFlyAction(); // ride -> fly
	bool OnRideToSkillAction(int skill,bool bCom,int iSel,int iForceAtk); // ride -> skill
	bool OnFlyToRideAction(int petIndex); // fly -> ride
	bool OnRideToUseTargetItem(int iPack,int iSlot, int tid); // ride -> use target item

protected:
	void ProcessMessage();
	void OnFly();
	void OnRide();
	VOID OnCastSkill();

	bool AddActionContext(CECActionContext* pContext);

	void RemoveRideFlyRelatedContext();

protected:

};



///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_ECACTIONSWITCHER_H_

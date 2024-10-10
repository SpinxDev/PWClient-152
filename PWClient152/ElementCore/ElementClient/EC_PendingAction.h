// Filename	: EC_PendingAction.h
// Creator	: Xu Wenbin
// Date		: 2011/11/09
#pragma once

#include "EC_Counter.h"
#include <vector>

class CECGame;
class CECGameRun;
class CECGameSession;
class CECHostPlayer;

class CECPendingAction
{
public:
	CECPendingAction(DWORD dwTimerPeriod);
	virtual ~CECPendingAction();

	void		Update(DWORD dwElapsedTime);
	void		Trigger();

	bool		GetNeedExecute()const{ return m_bNeedExecute; }
	bool		GetTriggered()const{ return m_bTriggered; }

	virtual const char *GetName()const=0;

protected:

	virtual bool Execute()=0;

	bool IsInGame()const;

	CECGame				*	GetGame()const;
	CECGameRun		*	GetGameRun()const;
	CECGameSession*	GetGameSession()const;
	CECHostPlayer		*	GetHostPlayer()const;

private:
	CECCounter	m_Counter;					//	执行的时限
	bool					m_bNeedExecute;		//	是否还需要执行
	bool					m_bTriggered;				//	是否收到外界执行指示
	bool					m_bExecuteResult;		//	执行结果：是否成功执行
};

//	游戏小退
class CECPendingLogoutHalf : public CECPendingAction
{
public:
	CECPendingLogoutHalf(DWORD dwTimerPeriod=5000) : CECPendingAction(dwTimerPeriod) {}

protected:
	virtual bool Execute();
	virtual const char *GetName()const{ return "CECPendingLogoutHalf"; }
};

//	游戏大退
class CECPendingLogoutFull : public CECPendingAction
{
public:
	CECPendingLogoutFull(DWORD dwTimerPeriod=5000) : CECPendingAction(dwTimerPeriod) {}

protected:
	virtual bool Execute();
	virtual const char *GetName()const{ return "CECPendingLogoutFull"; }
};

//	寄售角色
class CECPendingSellingRole : public CECPendingAction
{
public:
	CECPendingSellingRole(DWORD dwTimerPeriod=5000) : CECPendingAction(dwTimerPeriod) {}

protected:
	virtual bool Execute();
	virtual const char *GetName()const{ return "CECPendingSellingRole"; }
};

//	跨服小退
class CECPendingLogoutCrossServer : public CECPendingAction
{
public:
	CECPendingLogoutCrossServer(DWORD dwTimerPeriod=5000) : CECPendingAction(dwTimerPeriod) {}

protected:
	virtual bool Execute();
	virtual const char *GetName()const{ return "CECPendingLogoutCrossServer"; }
};

//	进出跨服
class CECPendingCrossServerGetInOut : public CECPendingAction
{
public:
	CECPendingCrossServerGetInOut(bool bGetIn, DWORD dwTimerPeriod=5000)
		: CECPendingAction(dwTimerPeriod)
		, m_bGetIn(bGetIn) {}
	
protected:
	virtual bool Execute();
	virtual const char *GetName()const{ return "CECPendingCrossServerGetInOut"; }

private:
	bool	m_bGetIn;		//	进入跨服时为 true；否则为 false
};

class CECPendingActionArray
{
public:

	CECPendingActionArray(bool bAllowMultiActions);
	~CECPendingActionArray();

	void		Append(CECPendingAction *pAction);
	void		AppendForSaveConfig(CECPendingAction *pAction);
	void		Update(DWORD dwElapsedTime);

	void		TriggerAll();

	void		Clear();

	void		SetAllowMultiAction(bool bAllow);
	bool		GetAllowMultiAction()const{ return m_bAllowMultiActions; }

private:
	CECPendingActionArray(const CECPendingActionArray&);
	CECPendingActionArray & operator=(const CECPendingActionArray&);

	typedef	std::vector<CECPendingAction *>		PendingActionList;
	PendingActionList		m_actions;

	bool		m_bAllowMultiActions;
};
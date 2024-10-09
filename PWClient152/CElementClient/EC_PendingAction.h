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
	CECCounter	m_Counter;					//	ִ�е�ʱ��
	bool					m_bNeedExecute;		//	�Ƿ���Ҫִ��
	bool					m_bTriggered;				//	�Ƿ��յ����ִ��ָʾ
	bool					m_bExecuteResult;		//	ִ�н�����Ƿ�ɹ�ִ��
};

//	��ϷС��
class CECPendingLogoutHalf : public CECPendingAction
{
public:
	CECPendingLogoutHalf(DWORD dwTimerPeriod=5000) : CECPendingAction(dwTimerPeriod) {}

protected:
	virtual bool Execute();
	virtual const char *GetName()const{ return "CECPendingLogoutHalf"; }
};

//	��Ϸ����
class CECPendingLogoutFull : public CECPendingAction
{
public:
	CECPendingLogoutFull(DWORD dwTimerPeriod=5000) : CECPendingAction(dwTimerPeriod) {}

protected:
	virtual bool Execute();
	virtual const char *GetName()const{ return "CECPendingLogoutFull"; }
};

//	���۽�ɫ
class CECPendingSellingRole : public CECPendingAction
{
public:
	CECPendingSellingRole(DWORD dwTimerPeriod=5000) : CECPendingAction(dwTimerPeriod) {}

protected:
	virtual bool Execute();
	virtual const char *GetName()const{ return "CECPendingSellingRole"; }
};

//	���С��
class CECPendingLogoutCrossServer : public CECPendingAction
{
public:
	CECPendingLogoutCrossServer(DWORD dwTimerPeriod=5000) : CECPendingAction(dwTimerPeriod) {}

protected:
	virtual bool Execute();
	virtual const char *GetName()const{ return "CECPendingLogoutCrossServer"; }
};

//	�������
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
	bool	m_bGetIn;		//	������ʱΪ true������Ϊ false
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
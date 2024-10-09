/********************************************************************
	created:	2005/08/31
	created:	31:8:2005   11:01
	file name:	ECScriptUnitExecutor.h
	author:		yaojun
	
	purpose:	visitors to execute on CECScriptUnit tree.
				
*********************************************************************/

#pragma once

#include "ECScriptUnit.h"

class CECScriptUnit;
class CECScriptUnitFunction;
class CECScriptUnitComposite;
class CECScriptUnitIfBranch;
class CECScriptUnitWait;
class CECScriptUnitReturn;
class CECScriptUnitExecutorState;
class CECScriptContext;
class IECScriptActiveState;
class IECScriptCheckState;
class CECScript;

//////////////////////////////////////////////////////////////////////////
// CECScriptUnitExecutor
//////////////////////////////////////////////////////////////////////////

class CECScriptUnitExecutor : public IECScriptUnitVisitor
{
private:
	CECScriptUnitExecutorState * m_pState;
	CECScriptUnitExecutorState * m_pNextState;
	
	CECScriptContext * m_pContext;
	CECScript * m_pScript;
protected:
	CECScriptUnitExecutorState * GetState();
public:
	CECScriptUnitExecutor(CECScriptContext *pContext);
	virtual ~CECScriptUnitExecutor();

	virtual void VisitFunction(CECScriptUnitFunction *pUnit);
	virtual void VisitIfCondition(CECScriptUnitIfCondition *pUnit);
	virtual void VisitWait(CECScriptUnitWait *pUnit);
	virtual void VisitReturn(CECScriptUnitReturn *pUnit);
	virtual void VisitFinish(CECScriptUnitFinish *pUnit);
	virtual void VisitStart(CECScriptUnitStart *pUnit);
	virtual CECScriptContext * GetContext();
	virtual bool IsMatchOnlyOneBranch();

	
	CECScript * GetRunningScript();
	void SetRunningScript(CECScript *pScript);
	void ChangeState(CECScriptUnitExecutorState *pState);
	void SetNextState(CECScriptUnitExecutorState *pState);
	void ChangeToNextState();
};

//////////////////////////////////////////////////////////////////////////
// CECScriptUnitExecutor States
//////////////////////////////////////////////////////////////////////////

// interface
class CECScriptUnitExecutorState
{
private:
	CECScriptUnitExecutor * m_pExecutor;
protected:
	CECScriptUnitExecutor * GetExecutor();
public:
	CECScriptUnitExecutorState(CECScriptUnitExecutor *pExecutor) : m_pExecutor(pExecutor) {}
	virtual ~CECScriptUnitExecutorState() {}

	virtual void ExecuteFunction(CECScriptUnitFunction *pUnit) = 0;
	virtual void ExecuteIfCondition(CECScriptUnitIfCondition *pUnit) = 0;
	virtual void ExecuteWait(CECScriptUnitWait *pUnit) = 0;
	virtual void ExecuteReturn(CECScriptUnitReturn *pUnit) = 0;
	virtual void ExecuteFinish(CECScriptUnitFinish *pUnit) = 0;
	virtual void ExecuteStart(CECScriptUnitStart *pUnit) = 0;
	virtual bool IsMatchOnlyOneBranch() = 0;
};

// normal execution flow
class CECScriptUnitExecutorStateNormal : public CECScriptUnitExecutorState
{
public:
	CECScriptUnitExecutorStateNormal(CECScriptUnitExecutor *pExecutor)
	  : CECScriptUnitExecutorState(pExecutor)
	{
	}
	virtual void ExecuteFunction(CECScriptUnitFunction *pUnit);
	virtual void ExecuteIfCondition(CECScriptUnitIfCondition *pUnit);
	virtual void ExecuteWait(CECScriptUnitWait *pUnit);
	virtual void ExecuteReturn(CECScriptUnitReturn *pUnit);
	virtual void ExecuteFinish(CECScriptUnitFinish *pUnit);
	virtual void ExecuteStart(CECScriptUnitStart *pUnit);
	virtual bool IsMatchOnlyOneBranch();
};

// just skip any script
class CECScriptUnitExecutorStateNull : public CECScriptUnitExecutorState
{
public:
	CECScriptUnitExecutorStateNull(CECScriptUnitExecutor *pExecutor)
		: CECScriptUnitExecutorState(pExecutor)
	{
	}
	virtual void ExecuteFunction(CECScriptUnitFunction *pUnit) {}
	virtual void ExecuteIfCondition(CECScriptUnitIfCondition *pUnit);
	virtual void ExecuteWait(CECScriptUnitWait *pUnit) {}
	virtual void ExecuteReturn(CECScriptUnitReturn *pUnit) {}
	virtual void ExecuteFinish(CECScriptUnitFinish *pUnit) {}
	virtual void ExecuteStart(CECScriptUnitStart *pUnit) {}
	virtual bool IsMatchOnlyOneBranch();
};

// blocked during last execution
// find the block point and check condition again
class CECScriptUnitExecutorStateBlock : public CECScriptUnitExecutorState
{
private:
	CECScriptUnitWait * m_pUnitWait;
protected:
	CECScriptUnitWait * GetUnitWait() { return m_pUnitWait; }
public:
	CECScriptUnitExecutorStateBlock(CECScriptUnitExecutor *pExecutor, CECScriptUnitWait *pUnitWait)
		: CECScriptUnitExecutorState(pExecutor), m_pUnitWait(pUnitWait)
	{
	}
	virtual void ExecuteFunction(CECScriptUnitFunction *pUnit);
	virtual void ExecuteIfCondition(CECScriptUnitIfCondition *pUnit);
	virtual void ExecuteWait(CECScriptUnitWait *pUnit);
	virtual void ExecuteReturn(CECScriptUnitReturn *pUnit);
	virtual void ExecuteFinish(CECScriptUnitFinish *pUnit);
	virtual void ExecuteStart(CECScriptUnitStart *pUnit);
	virtual bool IsMatchOnlyOneBranch();
};

// wait outter program to pop me
class CECScriptUnitExecutorStateWaitPop : public CECScriptUnitExecutorState
{
public:
	CECScriptUnitExecutorStateWaitPop(CECScriptUnitExecutor *pExecutor)
		: CECScriptUnitExecutorState(pExecutor)
	{
	}
	virtual void ExecuteFunction(CECScriptUnitFunction *pUnit) {}
	virtual void ExecuteIfCondition(CECScriptUnitIfCondition *pUnit) {}
	virtual void ExecuteWait(CECScriptUnitWait *pUnit) {}
	virtual void ExecuteReturn(CECScriptUnitReturn *pUnit) {}
	virtual void ExecuteFinish(CECScriptUnitFinish *pUnit) {}
	virtual void ExecuteStart(CECScriptUnitStart *pUnit);
	virtual bool IsMatchOnlyOneBranch() { return true; }
};

// afer pop
class CECScriptUnitExecutorStateAfterPop : public CECScriptUnitExecutorState
{
public:
	CECScriptUnitExecutorStateAfterPop(CECScriptUnitExecutor *pExecutor)
		: CECScriptUnitExecutorState(pExecutor)
	{
	}
	virtual void ExecuteFunction(CECScriptUnitFunction *pUnit) {}
	virtual void ExecuteIfCondition(CECScriptUnitIfCondition *pUnit) {}
	virtual void ExecuteWait(CECScriptUnitWait *pUnit) {}
	virtual void ExecuteReturn(CECScriptUnitReturn *pUnit) {}
	virtual void ExecuteFinish(CECScriptUnitFinish *pUnit) {}
	virtual void ExecuteStart(CECScriptUnitStart *pUnit);
	virtual bool IsMatchOnlyOneBranch() { return true; }
};


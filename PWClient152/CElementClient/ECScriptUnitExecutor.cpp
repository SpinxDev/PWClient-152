#include <ABaseDef.h>
#include "ECScriptUnitExecutor.h"
#include "ECScriptUnit.h"
#include "CodeTemplate.h"
#include "ECScriptContext.h"
#include "ECScriptController.h"
#include "ECScript.h"
#include "ECScriptUI.h"
#include "ECScriptOption.h"


//////////////////////////////////////////////////////////////////////////
// CECScriptUnitExecutor
//////////////////////////////////////////////////////////////////////////

CECScriptUnitExecutor::CECScriptUnitExecutor(CECScriptContext *pContext) :
	m_pContext(pContext)
{
	m_pState = new CECScriptUnitExecutorStateNormal(this);
	m_pNextState = new CECScriptUnitExecutorStateNormal(this);
}

CECScriptUnitExecutor::~CECScriptUnitExecutor()
{
	SAFE_DELETE(m_pState);
	SAFE_DELETE(m_pNextState);
}

CECScriptUnitExecutorState * CECScriptUnitExecutor::GetState()
{
	ASSERT(m_pState);
	return m_pState;
}

void CECScriptUnitExecutor::VisitFunction(CECScriptUnitFunction *pUnit)
{
	GetState()->ExecuteFunction(pUnit);
}

void CECScriptUnitExecutor::VisitIfCondition(CECScriptUnitIfCondition *pUnit)
{
	GetState()->ExecuteIfCondition(pUnit);
}

void CECScriptUnitExecutor::VisitWait(CECScriptUnitWait *pUnit)
{
	GetState()->ExecuteWait(pUnit);
}

void CECScriptUnitExecutor::VisitReturn(CECScriptUnitReturn *pUnit)
{
	GetState()->ExecuteReturn(pUnit);
}

void CECScriptUnitExecutor::VisitFinish(CECScriptUnitFinish *pUnit)
{
	GetState()->ExecuteFinish(pUnit);
}

bool CECScriptUnitExecutor::IsMatchOnlyOneBranch()
{
	return GetState()->IsMatchOnlyOneBranch();
}

void CECScriptUnitExecutor::VisitStart(CECScriptUnitStart *pUnit)
{
	GetState()->ExecuteStart(pUnit);
}


void CECScriptUnitExecutor::ChangeState(CECScriptUnitExecutorState *pState)
{
	ASSERT(pState);
	
	SAFE_DELETE(m_pState);
	m_pState = pState;
}

void CECScriptUnitExecutor::SetNextState(CECScriptUnitExecutorState *pState)
{
	ASSERT(pState);

	SAFE_DELETE(m_pNextState);
	m_pNextState = pState;
}

void CECScriptUnitExecutor::ChangeToNextState()
{
	ASSERT(m_pNextState);

	ChangeState(m_pNextState);

	m_pNextState = new CECScriptUnitExecutorStateNormal(this);
}

CECScriptContext * CECScriptUnitExecutor::GetContext()
{
	return m_pContext;
}

void CECScriptUnitExecutor::SetRunningScript(CECScript *pScript)
{
	m_pScript = pScript;
}

CECScript * CECScriptUnitExecutor::GetRunningScript()
{
	return m_pScript;
}



//////////////////////////////////////////////////////////////////////////
// CECScriptUnitExecutorState
//////////////////////////////////////////////////////////////////////////

CECScriptUnitExecutor * CECScriptUnitExecutorState::GetExecutor()
{ 
	ASSERT(m_pExecutor); 
	return m_pExecutor; 
}

//////////////////////////////////////////////////////////////////////////
// CECScriptUnitExecutorStateNull
//////////////////////////////////////////////////////////////////////////

void CECScriptUnitExecutorStateNull::ExecuteIfCondition(CECScriptUnitIfCondition *pUnit)
{
	pUnit->SetResult(false);
}

bool CECScriptUnitExecutorStateNull::IsMatchOnlyOneBranch()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CECScriptUnitExecutorStateNormal
//////////////////////////////////////////////////////////////////////////

void CECScriptUnitExecutorStateNormal::ExecuteFunction(CECScriptUnitFunction *pUnit)
{
	pUnit->ExecFunction(GetExecutor());
}

void CECScriptUnitExecutorStateNormal::ExecuteIfCondition(CECScriptUnitIfCondition *pUnit)
{
	pUnit->SetResult(
		pUnit->EvaluateCondition(
			GetExecutor()->GetContext()->GetCheckState()));
}

bool CECScriptUnitExecutorStateNormal::IsMatchOnlyOneBranch()
{
	return true;
}


void CECScriptUnitExecutorStateNormal::ExecuteWait(CECScriptUnitWait *pUnit)
{
	if (!pUnit->EvaluateCondition(GetExecutor()->GetContext()->GetCheckState()))
	{
		// set state to block when next tick
		CECScriptUnitExecutorState *pNextState = 
			new CECScriptUnitExecutorStateBlock(
				GetExecutor(), pUnit);
		GetExecutor()->SetNextState(pNextState);
		
		// change to null state to skip any following script
		GetExecutor()->ChangeState(
			new CECScriptUnitExecutorStateNull(
				GetExecutor()));
	}
}

void CECScriptUnitExecutorStateNormal::ExecuteReturn(CECScriptUnitReturn *pUnit)
{
	GetExecutor()->ChangeState(
		new CECScriptUnitExecutorStateNull(
			GetExecutor()));
}

void CECScriptUnitExecutorStateNormal::ExecuteStart(CECScriptUnitStart *pUnit)
{
	CECScript *pScript = GetExecutor()->GetRunningScript();
	if (pScript->IsForcePop())
	{
		GetExecutor()->GetContext()->GetController()->FinishCurrentScript(pScript);
	}
	else
	{
		GetExecutor()->GetContext()->GetUI()->AddToSpirit(pScript->GetID(), pScript->GetName());

		CECScriptUnitExecutorStateWaitPop *pNextState = 
			new CECScriptUnitExecutorStateWaitPop(
			GetExecutor());
		GetExecutor()->SetNextState(pNextState);
		
		GetExecutor()->ChangeState(
			new CECScriptUnitExecutorStateWaitPop(
				GetExecutor()));
	}
}

void CECScriptUnitExecutorStateNormal::ExecuteFinish(CECScriptUnitFinish *pUnit)
{
	GetExecutor()->GetContext()->GetController()->FinishCurrentScript(NULL);

	GetExecutor()->ChangeState(
		new CECScriptUnitExecutorStateNull(
			GetExecutor()));
}

//////////////////////////////////////////////////////////////////////////
// CECScriptUnitExecutorStateBlock
//////////////////////////////////////////////////////////////////////////

void CECScriptUnitExecutorStateBlock::ExecuteFunction(CECScriptUnitFunction *pUnit)
{
}

void CECScriptUnitExecutorStateBlock::ExecuteIfCondition(CECScriptUnitIfCondition *pUnit)
{
	pUnit->SetResult(true);
}

bool CECScriptUnitExecutorStateBlock::IsMatchOnlyOneBranch()
{
	return false;
}

void CECScriptUnitExecutorStateBlock::ExecuteWait(CECScriptUnitWait *pUnit)
{
	if (pUnit == GetUnitWait()) // found last block point
	{
		if (pUnit->EvaluateCondition(GetExecutor()->GetContext()->GetCheckState()))
		{
			// check condition passed
			// switch to normal state to make execution continue
			GetExecutor()->ChangeState(
				new CECScriptUnitExecutorStateNormal(
					GetExecutor()));
		}
		else
		{
			// fail check again

			// set state to block when next tick
			CECScriptUnitExecutorState *pNextState = 
				new CECScriptUnitExecutorStateBlock(
				GetExecutor(), pUnit);
			GetExecutor()->SetNextState(pNextState);

			// swith to null state to skip any following script
			GetExecutor()->ChangeState(
				new CECScriptUnitExecutorStateNull(
						GetExecutor()));
		}
	}
}

void CECScriptUnitExecutorStateBlock::ExecuteReturn(CECScriptUnitReturn *pUnit)
{
}

void CECScriptUnitExecutorStateBlock::ExecuteFinish(CECScriptUnitFinish *pUnit)
{
}

void CECScriptUnitExecutorStateBlock::ExecuteStart(CECScriptUnitStart *pUnit)
{

}

//////////////////////////////////////////////////////////////////////////
// CECScriptUnitExecutorStateWaitPop
//////////////////////////////////////////////////////////////////////////

void CECScriptUnitExecutorStateWaitPop::ExecuteStart(CECScriptUnitStart *pUnit)
{
	// always remain in this state
	// until the outter program change it
	CECScriptUnitExecutorStateWaitPop *pNextState = 
		new CECScriptUnitExecutorStateWaitPop(
			GetExecutor());
	GetExecutor()->SetNextState(pNextState);
}

//////////////////////////////////////////////////////////////////////////
// CECScriptUnitExecutorStateAfterStart
//////////////////////////////////////////////////////////////////////////

void CECScriptUnitExecutorStateAfterPop::ExecuteStart(CECScriptUnitStart *pUnit)
{
	GetExecutor()->GetContext()->GetController()->FinishCurrentScript(
		GetExecutor()->GetRunningScript());
	GetExecutor()->ChangeState(
		new CECScriptUnitExecutorStateNormal(
			GetExecutor()));
}

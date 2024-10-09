#include "ECScriptUnit.h"
#include "ECScriptBoolExp.h"
#include "ECScriptFunctionBase.h"


void CECScriptUnitWait::Visit(IECScriptUnitVisitor *pVisitor)
{
	pVisitor->VisitWait(this);
}

void CECScriptUnitCommonBlock::Visit(IECScriptUnitVisitor *pVisitor)
{
	CECScriptUnitArray::iterator iter = m_vecChildren.begin();
	for ( ; iter != m_vecChildren.end(); ++iter) 
	{
		CECScriptUnit *pUnit = *iter;
		pUnit->Visit(pVisitor);
	}
}

void CECScriptUnitFunction::Visit(IECScriptUnitVisitor *pVisitor)
{
	pVisitor->VisitFunction(this);
}
	
void CECScriptUnitReturn::Visit(IECScriptUnitVisitor *pVisitor)
{
	pVisitor->VisitReturn(this);
}


void CECScriptUnitStart::Visit(IECScriptUnitVisitor *pVisitor)
{
	pVisitor->VisitStart(this);
}

CECScriptUnitIfBlock::CECScriptUnitIfBranch::~CECScriptUnitIfBranch()
{
	delete m_pCondition;
	delete m_pBlock;
}

CECScriptUnitWait::~CECScriptUnitWait() 
{ 
	delete m_pCondition; 
}


int CECScriptUnitCommonBlock::GetChildrenCount() const
{
	return static_cast<int>(m_vecChildren.size());
}

void CECScriptUnitCommonBlock::PushBackChild(CECScriptUnit *pChildUnit)
{
	m_vecChildren.push_back(pChildUnit);
}

CECScriptUnitCommonBlock::~CECScriptUnitCommonBlock()
{
	CECScriptUnitArray::iterator iter = m_vecChildren.begin();
	for (; iter != m_vecChildren.end(); ++iter)
	{
		CECScriptUnit *p = *iter;
		delete p;
	}
}

CECScriptUnitFunction::~CECScriptUnitFunction()
{
	delete m_pFunction;
}

bool CECScriptUnitIfBlock::CECScriptUnitIfBranch::GetConditionResult()
{
	return m_pCondition->GetResult();
}

void CECScriptUnitIfBlock::CECScriptUnitIfBranch::VisitCondition(IECScriptUnitVisitor *pVisitor) 
{ 
	m_pCondition->Visit(pVisitor);
}

void CECScriptUnitIfBlock::CECScriptUnitIfBranch::VisitBlock(IECScriptUnitVisitor *pVisitor) 
{ 
	m_pBlock->Visit(pVisitor); 
}

bool CECScriptUnitWait::EvaluateCondition(IECScriptCheckState *pState) 
{ 
	return m_pCondition->Evaluate(pState); 
}

void CECScriptUnitFunction::ExecFunction(IECScriptUnitVisitor *pVisitor)
{
	m_pFunction->Execute(pVisitor->GetContext());
}


CECScriptUnitIfCondition::~CECScriptUnitIfCondition()
{
	delete m_pCondition;
}

bool CECScriptUnitIfCondition::EvaluateCondition(IECScriptCheckState* pState)
{
	return m_pCondition->Evaluate(pState);
}

void CECScriptUnitIfCondition::Visit(IECScriptUnitVisitor *pVisitor)
{
	pVisitor->VisitIfCondition(this);
}

void CECScriptUnitIfBlock::Visit(IECScriptUnitVisitor *pVisitor)
{
	for (int i = 0; i < GetBranchCount(); i++)
	{
		CECScriptUnitIfBranch *pBranch = m_vecBranches[i];
		pBranch->VisitCondition(pVisitor);
		if (pBranch->GetConditionResult())
		{
			pBranch->VisitBlock(pVisitor);
			if (pVisitor->IsMatchOnlyOneBranch())
				break;
		}
	}
}

int CECScriptUnitIfBlock::GetBranchCount()
{
	return static_cast<int>(m_vecBranches.size());
}

void CECScriptUnitIfBlock::PushBackBranch(CECScriptUnitIfCondition *pCondition, CECScriptUnitCommonBlock *pBlock)
{
	CECScriptUnitIfBranch *pBranch = new CECScriptUnitIfBranch(pCondition, pBlock);
	m_vecBranches.push_back(pBranch);
}

CECScriptUnitIfBlock::~CECScriptUnitIfBlock()
{
	int nCount = static_cast<int>(m_vecBranches.size());
	for (int i = 0; i < nCount; i++)
	{
		CECScriptUnitIfBranch *p = m_vecBranches[i];
		delete p;
	}
}

void CECScriptUnitFinish::Visit(IECScriptUnitVisitor *pVisitor)
{
	pVisitor->VisitFinish(this);
}


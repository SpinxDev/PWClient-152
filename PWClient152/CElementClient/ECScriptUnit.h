/********************************************************************
	created:	2005/08/30
	created:	30:8:2005   17:31
	file name:	ECScriptUnit.h
	author:		yaojun
	
	purpose:	this file defines class CECScriptUnit and its derived
				class which are used to construct a tree from a 
				script file.
*********************************************************************/


#pragma once

#include <vector.h>
#include <ABaseDef.h>

class CECScriptUnit;
class CECScriptBoolExp;
class CECScriptFunctionBase;
class IECScriptCheckState;
class CECScriptContext;

//////////////////////////////////////////////////////////////////////////
// Script Unit
//////////////////////////////////////////////////////////////////////////
class IECScriptUnitVisitor;

class CECScriptUnit
{
public:
	virtual ~CECScriptUnit() {}
	
	virtual void Visit(IECScriptUnitVisitor *pVisitor) = 0;
};

class CECScriptUnitWait : public CECScriptUnit
{
private:
	CECScriptBoolExp * m_pCondition;
public:
	CECScriptUnitWait(CECScriptBoolExp * pCondition) : m_pCondition(pCondition) {ASSERT(m_pCondition);}
	virtual ~CECScriptUnitWait();

	virtual void Visit(IECScriptUnitVisitor *pVisitor);
	bool EvaluateCondition(IECScriptCheckState *pState);
};

class CECScriptUnitCommonBlock : public CECScriptUnit
{
private:
	typedef abase::vector<CECScriptUnit *> CECScriptUnitArray;
	CECScriptUnitArray m_vecChildren; 
public:
	virtual ~CECScriptUnitCommonBlock();
	
	virtual void Visit(IECScriptUnitVisitor *pVisitor);
	
	int GetChildrenCount() const;
	void PushBackChild(CECScriptUnit *pChildUnit);
};

class CECScriptUnitFunction : public CECScriptUnit
{
private:
	CECScriptFunctionBase * m_pFunction;
public:
	CECScriptUnitFunction(CECScriptFunctionBase * pFunction) : m_pFunction(pFunction) { ASSERT(m_pFunction); }
	virtual ~CECScriptUnitFunction();
	
	virtual void Visit(IECScriptUnitVisitor *pVisitor);

	void ExecFunction(IECScriptUnitVisitor *pVisitor);
};

class CECScriptUnitReturn : public CECScriptUnit
{
public:
	virtual void Visit(IECScriptUnitVisitor *pVisitor);
};

class CECScriptUnitFinish : public CECScriptUnit
{
public:
	virtual void Visit(IECScriptUnitVisitor *pVisitor);
};

class CECScriptUnitStart : public CECScriptUnit
{
public:
	virtual void Visit(IECScriptUnitVisitor *pVisitor);
};

class CECScriptUnitIfCondition : public CECScriptUnit
{
private:
	CECScriptBoolExp * m_pCondition;
	bool m_bResult;
public:
	CECScriptUnitIfCondition(CECScriptBoolExp * pCondition)
		: m_pCondition(pCondition), m_bResult(false) 
	{
		ASSERT(m_pCondition);
	}
	virtual ~CECScriptUnitIfCondition();
	virtual void Visit(IECScriptUnitVisitor *pVisitor);
	bool EvaluateCondition(IECScriptCheckState* pState);
	bool GetResult() { return m_bResult; }
	void SetResult(bool bResult) { m_bResult = bResult; }
};



class CECScriptUnitIfBlock : public CECScriptUnit
{
private:
	class CECScriptUnitIfBranch : public CECScriptUnit
	{
	private:
		CECScriptUnitIfCondition * m_pCondition;
		CECScriptUnitCommonBlock * m_pBlock;
	public:
		CECScriptUnitIfBranch(CECScriptUnitIfCondition *pCondition, CECScriptUnitCommonBlock *pBlock)
			: m_pCondition(pCondition), m_pBlock(pBlock)
		{
			ASSERT(m_pCondition);
			ASSERT(m_pBlock);
		}
		virtual ~CECScriptUnitIfBranch();
		virtual void Visit(IECScriptUnitVisitor *pVisitor) {}
	public:
		void VisitCondition(IECScriptUnitVisitor *pVisitor);
		void VisitBlock(IECScriptUnitVisitor *pVisitor);
		bool GetConditionResult();
	};
private:
	typedef abase::vector<CECScriptUnitIfBranch *> CBranchArray;
	CBranchArray m_vecBranches;
	bool m_bMatchOnlyOneBranch;
public:
	CECScriptUnitIfBlock() : m_bMatchOnlyOneBranch(false) {}
	virtual ~CECScriptUnitIfBlock();
	virtual void Visit(IECScriptUnitVisitor *pVisitor);

	int GetBranchCount();
	void PushBackBranch(CECScriptUnitIfCondition *pCondition, CECScriptUnitCommonBlock *pBlock);
	void SetMatchOnlyOneBranch(bool bMatchOnlyOneBranch) { m_bMatchOnlyOneBranch = bMatchOnlyOneBranch; }
};

//////////////////////////////////////////////////////////////////////////
// Script Unit Visitor interface
//////////////////////////////////////////////////////////////////////////


class IECScriptUnitVisitor
{
public:
	virtual void VisitFunction(CECScriptUnitFunction *pUnit) = 0;
	virtual void VisitIfCondition(CECScriptUnitIfCondition *pUnit) = 0;
	virtual void VisitWait(CECScriptUnitWait *pUnit) = 0;
	virtual void VisitReturn(CECScriptUnitReturn *pUnit) = 0;
	virtual void VisitFinish(CECScriptUnitFinish *pUnit) = 0;
	virtual void VisitStart(CECScriptUnitStart *pUnit) = 0;
	virtual CECScriptContext * GetContext() = 0;
	virtual bool IsMatchOnlyOneBranch() = 0;
};
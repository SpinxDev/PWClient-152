/********************************************************************
	created:	2005/08/30
	created:	30:8:2005   17:33
	file name:	ECScriptBoolExp.h
	author:		yaojun
	
	purpose:	this file defines CECScriptBoolExp and its derived 
				class which are used to construct a tree from a
				boolean expression
*********************************************************************/

#pragma once

#include <AString.h>
#include <ABaseDef.h>

class IECScriptCheckState;

class CECScriptBoolExp
{
public:
	virtual ~CECScriptBoolExp() {}

	virtual bool Evaluate(IECScriptCheckState * pState) const = 0;
	virtual CECScriptBoolExp * Clone() const;
};

class CECScriptBoolExpAnd : public CECScriptBoolExp
{
private:
	CECScriptBoolExp * m_pOP1;
	CECScriptBoolExp * m_pOP2;
public:
	CECScriptBoolExpAnd(CECScriptBoolExp *pOP1, CECScriptBoolExp * pOP2)
		: m_pOP1(pOP1), m_pOP2(pOP2)
	{ ASSERT(pOP1); ASSERT(pOP2); }
	virtual ~CECScriptBoolExpAnd()
	{
		delete m_pOP1;
		delete m_pOP2;
	}
	virtual CECScriptBoolExp * Clone() const
	{
		return new CECScriptBoolExpAnd(
			m_pOP1->Clone(), m_pOP2->Clone());
	}

	virtual bool Evaluate(IECScriptCheckState * pState) const 
	{
		return m_pOP1->Evaluate(pState) && m_pOP2->Evaluate(pState);
	}
};

class CECScriptBoolExpOr : public CECScriptBoolExp
{
private:
	CECScriptBoolExp * m_pOP1;
	CECScriptBoolExp * m_pOP2;
public:
	CECScriptBoolExpOr(CECScriptBoolExp *pOP1, CECScriptBoolExp * pOP2) 
		: m_pOP1(pOP1), m_pOP2(pOP2) 
	{ ASSERT(pOP1); ASSERT(pOP2); }
	virtual ~CECScriptBoolExpOr()
	{
	  delete m_pOP1;
	  delete m_pOP2;
	}
	virtual CECScriptBoolExp * Clone() const
	{
		return new CECScriptBoolExpOr(
			m_pOP1->Clone(), m_pOP2->Clone());
	}

	virtual bool Evaluate(IECScriptCheckState * pState) const 
	{
	  return m_pOP1->Evaluate(pState) || m_pOP2->Evaluate(pState);
	}
};

class CECScriptBoolExpNot : public CECScriptBoolExp
{
private:
	CECScriptBoolExp * m_pOP;
public:
	CECScriptBoolExpNot(CECScriptBoolExp *pOP) 
		: m_pOP(pOP)
	{ ASSERT(pOP); }
	virtual ~CECScriptBoolExpNot()
	{
	  delete m_pOP;
	}
	virtual CECScriptBoolExp * Clone() const
	{
		return new CECScriptBoolExpNot(
			m_pOP->Clone());
	}

	virtual bool Evaluate(IECScriptCheckState * pState) const 
	{
	  return !m_pOP->Evaluate(pState);
	}
};

class CECScriptBoolExpConstant : public CECScriptBoolExp
{
private:
	bool m_bVal;
public:
	CECScriptBoolExpConstant(bool bVal) : m_bVal(bVal) {}
	virtual CECScriptBoolExp * Clone() const { return new CECScriptBoolExpConstant(m_bVal); }
	virtual bool Evaluate(IECScriptCheckState * pState) const { return m_bVal; }
};

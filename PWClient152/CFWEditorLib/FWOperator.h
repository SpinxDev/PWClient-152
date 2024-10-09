#ifndef _FWOPERATOR_H_
#define _FWOPERATOR_H_

#include "FWFormulaElement.h"
#include "FWConfig.h"

class FWOperand;

class FWOperator : public FWFormulaElement
{
public:
	// note : if the FWFormulaElement is an operator
	// the m_strRaw have and only have one char
	char GetOP() const { return m_strRaw[0]; }

	FWOperator() {}
	FWOperator(const FWOperator & src) { *this = src; }
	FWOperator(const AString & raw) : FWFormulaElement(raw, FE_TYPE_OPERATOR) {}
	virtual ~FWOperator();
	FWOperator & operator = (const FWOperator & src)
	{
		FWFormulaElement::operator = (src);
		return *this;	
	}
	IMPLEMENT_CLONE_INLINE(FWOperator, FWFormulaElement);

	// compute two operand
	FWOperand Operate(const FWOperand& left, const FWOperand& right);

	// get the priority of two operator
	friend int Compare(const FWOperator& left, const FWOperator& right);
	friend bool operator > (const FWOperator& left, const FWOperator& right);
	friend bool operator < (const FWOperator& left, const FWOperator& right);
	friend bool operator == (const FWOperator& left, const FWOperator& right);

protected:
	// get priority level of the operator
	// higher the level is higher its priority
	int GetPRI() const;
};

#endif 
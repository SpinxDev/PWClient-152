#ifndef _FWOPERAND_H_
#define _FWOPERAND_H_

#include "A3DGFXEditorInterface.h"
#include "FWFormulaElement.h"
#include "CodeTemplate.h"
#include "FWConfig.h"

class A3DGFXEx;
typedef A3DGFXEx FWAssembly;

class FWOutterPropertyList;

class FWOperand : public FWFormulaElement
{
protected:
	// value of the oprand, only valid when m_bIsVal is true
	GFX_PROPERTY m_Val;
	// whether the operand is a value or just a raw string
	bool m_bIsVal;
public:
	GFX_PROPERTY GetVal() const { return m_Val; }
	bool IsVal() const { return m_bIsVal; }

	FWOperand();
	FWOperand(const FWOperand & src) { *this = src; }
	FWOperand(const AString & raw) :	FWFormulaElement(raw, FE_TYPE_OPERAND), m_bIsVal(false) {}
	FWOperand(const GFX_PROPERTY& val) :
		FWFormulaElement("", FE_TYPE_OPERAND)
	{
		m_Val = val;
		m_bIsVal = true;
	}
	virtual ~FWOperand();
	FWOperand & operator = (const FWOperand & src);
	IMPLEMENT_CLONE_INLINE(FWOperand, FWFormulaElement);

	// this function is responsible of convert the raw
	// string to value
	operator GFX_PROPERTY() const;

	friend FWOperand operator + (const FWOperand &left, const FWOperand &right);
	friend FWOperand operator - (const FWOperand &left, const FWOperand &right);
	friend FWOperand operator * (const FWOperand &left, const FWOperand &right);
	friend FWOperand operator / (const FWOperand &left, const FWOperand &right);
};

#endif 
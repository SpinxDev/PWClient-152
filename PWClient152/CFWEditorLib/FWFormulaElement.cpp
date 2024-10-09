// FormulaElement.cpp: implementation of the FWFormulaElement class.
//
//////////////////////////////////////////////////////////////////////

#include "FWFormulaElement.h"
#include <A3DMacros.h>

#define new A_DEBUG_NEW




FWFormulaElement::FWFormulaElement()
{

}

FWFormulaElement::~FWFormulaElement()
{

}

FWFormulaElement::FWFormulaElement(const AString & raw)
{
	m_strRaw = raw;
	m_nType = DetermineType(raw);
}

FWFormulaElement::FWFormulaElement(const AString & raw, int nType)
{
	m_strRaw = raw;
	m_nType = nType;
}

FWFormulaElement& FWFormulaElement::operator = (const FWFormulaElement & src)
{
	m_strRaw = src.GetRawString();
	m_nType = src.GetType();
	return *this;
}


int FWFormulaElement::DetermineType(const AString & raw)
{
	ASSERT(raw.GetLength() > 0);
	
	if (raw.GetLength() == 1)
	{
		static const AString opSet = "+-*/";
		if (opSet.Find(raw[0]) != -1)
			return FE_TYPE_OPERATOR;
		else if (raw[0] == '[')
			return FE_TYPE_LEFT_BRACKET;
		else if (raw[0] == ']')
		{
			return FE_TYPE_RIGHT_BRACKET;
		}
	}
	return FE_TYPE_OPERAND;
}

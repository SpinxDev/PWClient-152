#ifndef _FWFORMULAELEMENT_H_
#define _FWFORMULAELEMENT_H_

#include "CodeTemplate.h"
#include "FWConfig.h"
#include <AString.h>

#define FE_TYPE_OPERATOR		0
#define FE_TYPE_LEFT_BRACKET	1
#define FE_TYPE_RIGHT_BRACKET	2
#define FE_TYPE_OPERAND			3

class FWFormulaElement  
{
protected:
	AString m_strRaw;	// element in string form
	int m_nType;		// see FE_TYPE_xxx
public:
	int GetType() const { return m_nType; }
	AString GetRawString() const { return m_strRaw; }

	FWFormulaElement();
	FWFormulaElement(const AString & raw);
	FWFormulaElement(const AString & raw, int nType);
	virtual ~FWFormulaElement();
	FWFormulaElement& operator = (const FWFormulaElement & src);
	IMPLEMENT_CLONE_INLINE(FWFormulaElement, FWFormulaElement);

	static int DetermineType(const AString & raw);
};

#endif 
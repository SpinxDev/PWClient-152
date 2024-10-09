#ifndef _FWFORMULA_H_
#define _FWFORMULA_H_

#include <AStack.h>
#include <AList2.h>
#include <AString.h>
#include "FWConfig.h"

class FWFormulaElement;

typedef AStack<FWFormulaElement *, FWFormulaElement*> AFEStack;
typedef APtrList<FWFormulaElement *> AFEList;
class AScriptFile;

class FWFormula  
{
	// expression at left of "="
	AString m_strLValue;

	// token of expression in postfix form
	AFEList m_lstPostfix;	
public:
	FWFormula();
	virtual ~FWFormula();
	
	// load from ini file
	bool Load(AScriptFile *pFile);
	
	bool Compute() const;
protected:
	// called by Load(), build expression as postfix form
	void ReadAndBuildPostfixExpression(AScriptFile *pFile);
};

#endif 
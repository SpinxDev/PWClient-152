// FormulaList.cpp: implementation of the FWFormulaList class.
//
//////////////////////////////////////////////////////////////////////

#include "FWFormulaList.h"
#include "FWFormula.h"
#include "FWOperand.h"
#include "CodeTemplate.h"
#include <AScriptFile.h>
#include <AScriptFile.h>

#define new A_DEBUG_NEW




FWFormulaList::FWFormulaList()
{

}

FWFormulaList::~FWFormulaList()
{
	DeleteAll();
}

bool FWFormulaList::Load(AScriptFile *pFile)
{
	DeleteAll();

	BEGIN_FAKE_WHILE;

	// find "InnerPropertyCount" section
	bool bReadSucceed = false;
	while ((bReadSucceed = pFile->GetNextToken(true)))
	{
		if (AString(pFile->m_szToken) == "InnerPropertyCount")
			break;
	}
	CHECK_BREAK(bReadSucceed);

	// read count
	CHECK_BREAK(pFile->GetNextToken(false));
	CHECK_BREAK(AString(pFile->m_szToken) == "=");
	int nCount = pFile->GetNextTokenAsInt(false);

	// read no more if count is zero
	if (0 == nCount) return true; 

	// let FWFormula do the loading work
	int i(0);
	for (i = 0; i < nCount; i++)
	{
		FWFormula *p = new FWFormula;
		if (!p->Load(pFile))
		{
			SAFE_DELETE(p);
			break;
		}
		else
		{
			AddTail(p);
		}
	}
	CHECK_BREAK(i == nCount);

	END_FAKE_WHILE;

	BEGIN_ON_FAIL_FAKE_WHILE;
	
	DeleteAll();
	
	END_ON_FAIL_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

bool FWFormulaList::Compute(FWAssembly *pAssembly, FWOutterPropertyList * pOutterPropertyList) const
{
	// set evn variables
	// this will be used by fomulas
	s_pAssembly = pAssembly;
	s_pOutterPropertyList = pOutterPropertyList;

	// compute each fomula
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWFormula *p = GetNext(pos);
		if (!p->Compute())
			return false;
	}
	return true;
}

void FWFormulaList::DeleteAll()
{
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWFormula *p = GetNext(pos);
		SAFE_DELETE(p);
	}
	RemoveAll();
}

FWAssembly * FWFormulaList::s_pAssembly = NULL;
FWOutterPropertyList * FWFormulaList::s_pOutterPropertyList = NULL;

FWAssembly * FWFormulaList::GetEnvAssembly()
{
	return s_pAssembly;
}

FWOutterPropertyList * FWFormulaList::GetEnvOutterPropertyList()
{
	return s_pOutterPropertyList;
}

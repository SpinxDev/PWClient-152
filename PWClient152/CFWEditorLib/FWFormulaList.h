#ifndef _FWFORMULALIST_H_
#define _FWFORMULALIST_H_

#include <AList2.h>
#include "FWConfig.h"

class FWFormula;
class A3DGFXEx;
typedef A3DGFXEx FWAssembly;
class FWOutterPropertyList;
class AScriptFile;

class FWFormulaList : public APtrList<FWFormula *>
{
	// the env that indicate the object that the formula will apply to
	static FWAssembly * s_pAssembly;
	static FWOutterPropertyList * s_pOutterPropertyList;
public:
	FWFormulaList();
	virtual ~FWFormulaList();

	bool Load(AScriptFile *pFile);
	bool Compute(FWAssembly *pAssembly, FWOutterPropertyList * pOutterPropertyList) const;

	static FWAssembly * GetEnvAssembly();
	static FWOutterPropertyList * GetEnvOutterPropertyList();
protected:
	void DeleteAll();
};

#endif 
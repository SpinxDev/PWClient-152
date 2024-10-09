#ifndef _FWFIREWORKSIMPLE_H_
#define _FWFIREWORKSIMPLE_H_

#include "FWFireworkLeaf.h"

class FWFireworkSimple : public FWFireworkLeaf  
{
	FW_DECLARE_SERIAL(FWFireworkSimple)
public:
	FWFireworkSimple();
	virtual ~FWFireworkSimple();
protected:
#ifdef _FW_EDITOR
	virtual const char * GetDlgToShow();
#endif
};

#endif 
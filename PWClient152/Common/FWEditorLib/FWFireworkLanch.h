#ifndef _FWFIREWORKLANCH_H_
#define _FWFIREWORKLANCH_H_

#include "FWFireworkLeaf.h"

class FWFireworkLanch : public FWFireworkLeaf  
{
	FW_DECLARE_SERIAL(FWFireworkLanch)
public:
	FWFireworkLanch();
	virtual ~FWFireworkLanch();
#ifdef _FW_EDITOR
	virtual const char * GetDlgToShow();
#endif
};

#endif 
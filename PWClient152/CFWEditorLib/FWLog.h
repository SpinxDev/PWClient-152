#ifndef _FWLOG_H_
#define _FWLOG_H_

#include <ALog.h>
#include "CodeTemplate.h"

class FWLog : public ALog  
{
	DECLARE_SINGLETON(FWLog)
public:
	FWLog();
	virtual ~FWLog();

};

#endif 
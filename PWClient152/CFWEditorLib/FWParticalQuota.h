#ifndef _FWPARTICALQUOTA_H_
#define _FWPARTICALQUOTA_H_

#include "CodeTemplate.h"

class FWParticalQuota  
{
	PROPERTY_INT(ParticalCount);
	PROPERTY_FLOAT(TimeStart);
	PROPERTY_FLOAT(TimeEnd);
public:
	FWParticalQuota();
	virtual ~FWParticalQuota();
};

#endif 
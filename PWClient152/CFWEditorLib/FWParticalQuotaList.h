#ifndef _FWPARTICALQUOTALIST_H_
#define _FWPARTICALQUOTALIST_H_

#include "FWParticalQuota.h"
#include <AList2.h>

class FWParticalQuotaList : public AList2<FWParticalQuota, FWParticalQuota&>
{
	int m_nSumOfQuota;
public:
	FWParticalQuotaList();
	virtual ~FWParticalQuotaList();

	void AddTailList(const FWParticalQuotaList *pSrc);
	
	void UpdateSumOfQuota();
	int GetSumOfQuota() const;
};

#endif 
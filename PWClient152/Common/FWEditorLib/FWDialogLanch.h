#ifndef _FWDIALOGLANCH_H_
#define _FWDIALOGLANCH_H_

#include "FWDialogPropBase.h"

class FWDialogLanch : public FWDialogPropBase  
{
	FW_DECLARE_DYNCREATE(FWDialogLanch)
	DECLARE_EVENT_MAP_FW()
protected:
	int GetProfileType();
public:
	FWDialogLanch();
	virtual ~FWDialogLanch();
	
};

#endif 
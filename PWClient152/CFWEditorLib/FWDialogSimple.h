#ifndef _FWDIALOGSIMPLE_H_
#define _FWDIALOGSIMPLE_H_

#include "FWDialogPropBase.h"

class FWDialogSimple : public FWDialogPropBase
{
	FW_DECLARE_DYNCREATE(FWDialogSimple)
	DECLARE_EVENT_MAP_FW()
protected:
	virtual int GetProfileType();
public:
	FWDialogSimple();
	virtual ~FWDialogSimple();
};

#endif 
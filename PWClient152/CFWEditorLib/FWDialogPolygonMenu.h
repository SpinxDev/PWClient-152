#ifndef _FWDIALOGPOLYGONMENU_H_
#define _FWDIALOGPOLYGONMENU_H_

#include "FWDialogBase.h"

class FWDialogPolygonMenu : public FWDialogBase  
{
	FW_DECLARE_DYNCREATE(FWDialogPolygonMenu)
	DECLARE_COMMAND_MAP_FW()
public:
	FWDialogPolygonMenu();
	virtual ~FWDialogPolygonMenu();

	void OnCommandDuplicateHandle();
	void OnCommandRemoveHandle();

	virtual void Show (bool bShow, bool bModal = false);
};

#endif 
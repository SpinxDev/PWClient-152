#ifndef _FWDGRAPHMENU_H_
#define _FWDGRAPHMENU_H_

#include "FWDialogBase.h"

class FWDialogGraphMenu : public FWDialogBase  
{
	FW_DECLARE_DYNCREATE(FWDialogGraphMenu)
	DECLARE_EVENT_MAP_FW()
public:
	FWDialogGraphMenu() {};
	virtual ~FWDialogGraphMenu() {};

	void OnClickBtnBezier(WPARAM wParam, LPARAM lParam);
	void OnClickBtnPlygon(WPARAM wParam, LPARAM lParam);
	void OnClickBtnRectangle(WPARAM wParam, LPARAM lParam);
	void OnClickBtnEllipse(WPARAM wParam, LPARAM lParam);
};

#endif 
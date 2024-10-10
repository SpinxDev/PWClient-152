#ifndef _FWDIALOGPLAY_H_
#define _FWDIALOGPLAY_H_

#include "FWDialogBase.h"

class FWDialogPlay : public FWDialogBase  
{
	FW_DECLARE_DYNCREATE(FWDialogPlay)
	DECLARE_EVENT_MAP_FW()
public:
	FWDialogPlay();
	virtual ~FWDialogPlay();

	void OnClickBtnReturn(WPARAM wParam, LPARAM lParam);
	void OnClickBtnStop(WPARAM wParam, LPARAM lParam);
	void OnClickBtnPause(WPARAM wParam, LPARAM lParam);
	void OnClickBtnPlay(WPARAM wParam, LPARAM lParam);
};

#endif 
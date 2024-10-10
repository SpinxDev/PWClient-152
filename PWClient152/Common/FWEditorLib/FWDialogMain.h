#ifndef _FWDMAIN_H_
#define _FWDMAIN_H_

#include "FWDialogBase.h"


class FWDialogMain : public FWDialogBase
{
	FW_DECLARE_DYNCREATE(FWDialogMain)
	DECLARE_EVENT_MAP_FW()
public:
	FWDialogMain(){};
	virtual ~FWDialogMain(){};

	void OnClickBtnLanch(WPARAM wParam, LPARAM lParam);
	void OnClickBtnPreview(WPARAM wParam, LPARAM lParam);
	void OnClickBtnCustom(WPARAM wParam, LPARAM lParam);
	void OnClickBtnText(WPARAM wParam, LPARAM lParam);
	void OnClickBtnSimple(WPARAM wParam, LPARAM lParam);
	void OnClickBtnGraph(WPARAM wParam, LPARAM lParam);

	virtual void OnOK();
	virtual void OnCancel();
};

#endif 
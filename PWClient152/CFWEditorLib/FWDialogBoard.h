#ifndef _FWDIALOGBOARD_H_
#define _FWDIALOGBOARD_H_

#include "FWDialogBase.h"

class FWDoc;
class FWState;

class FWDialogBoard : public FWDialogBase  
{
	FW_DECLARE_DYNCREATE(FWDialogBoard)
	DECLARE_EVENT_MAP_FW()
public:
	FWDialogBoard();
	virtual ~FWDialogBoard();

	void OnRButtonUp(WPARAM wParam, LPARAM lParam);
	void OnRButtonDown(WPARAM wParam, LPARAM lParam);
	void OnKeyUp(WPARAM wParam, LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	void OnTimer(WPARAM wParam, LPARAM lParam);

	ARectI GetClientRect(void);

	// base class interface
	virtual bool Render(void);
};

#endif 
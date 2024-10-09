#ifndef _FWSTATESELECT_H_
#define _FWSTATESELECT_H_

#include "FWState.h"

class FWStateNetSelect;
class FWMoveHandleState;
class FWStateMove;

class FWStateSelect : public FWState  
{
	FW_DECLARE_DYNAMIC(FWStateSelect);
public:
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnRClick(UINT nFlags, APointI point);
	virtual void OnMouseMove(UINT nFlags, APointI point);
	virtual void OnLClick(UINT nFlags, APointI point);
	virtual void OnDraw();
	virtual void OnLBeginDrag(UINT nFlags, APointI point);
	virtual void OnLButtonDblClk(UINT nFlags, APointI point);

	FWStateSelect(FWView *pView);
	virtual ~FWStateSelect();

private:
	// called by OnKeyDown() to move glyph
	bool ProcessDirKey(UINT nChar, UINT nRepCnt);
};

#endif 

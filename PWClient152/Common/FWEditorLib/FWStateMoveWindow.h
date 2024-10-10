#ifndef _FWSTATEMOVEWINDOW_H_
#define _FWSTATEMOVEWINDOW_H_

#include "FWState.h"

class FWGlyph;

class FWStateMoveWindow : public FWState  
{
public:
	FWStateMoveWindow(FWView *pView);
	virtual ~FWStateMoveWindow();

	virtual void OnLButtonUp(UINT nFlags, APointI point);
};

#endif 
#ifndef _FWSTATE_H
#define _FWSTATE_H

#include "FWConfig.h"
#include "FWObject.h"
#include <APoint.h>

class FWDoc;
class FWView;
typedef unsigned int UINT;

// FWState is the common interface for classes 
// representing current state.
// The application is switching from state to 
// state. For example, when select tool is 
// selected, the app goes into 
// select state. And when rectangle tool is 
// selected, the app goes into create rectangle 
// state. Each concrete class derived from FWState 
// represents a state. Call 
// FWView::ChangeState to change the current state.
// FWState has virtual functions such as 
// OnLMouseDown, OnKeyUp. Overrides them to 
// define the system's behavior. It also has a 
// member m_pBoard, which makes interface and 
// document control possible.
// all member stored in FWStatexxx are in logic coordinates
class FWState : public FWObject  
{
	friend class FWView;
	FW_DECLARE_DYNAMIC(FWState);

protected:
	FWView * m_pView;
	FWView * GetView();
	FWDoc * GetDocument();
public:

	virtual void OnTimer(UINT nIDEvent){}
	virtual void OnLButtonDblClk(UINT nFlags, APointI point){}
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){}
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){}
	virtual void OnLButtonUp(UINT nFlags, APointI point){}
	virtual void OnLButtonDown(UINT nFlags, APointI point){}
	virtual void OnRButtonUp(UINT nFlags, APointI point){}
	virtual void OnRButtonDown(UINT nFlags, APointI point){}
	virtual void OnMouseWheel(UINT nFlags, short zDelta, APointI pt){}
	virtual void OnMouseMove(UINT nFlags, APointI point){}

	virtual void OnDraw();

	// Many operations have to distinguish clicking 
	// from dragging. If user hold mouse button 
	// longer than MAXCLICKTIME, the FWState calls 
	// OnL/RBeginDrag(). Or, OnL/RClick() will be called in the 
	// default implementations of OnL/RButtonUp.
	virtual void OnLClick(UINT nFlags, APointI point){}
	virtual void OnRClick(UINT nFlags, APointI point){}
	virtual void OnLBeginDrag(UINT nFlags, APointI point){}
	virtual void OnRBeginDrag(UINT nFlags, APointI point){}

	FWState(FWView *pView);
	virtual ~FWState();
private:
	// Many operations have to distinguish clicking from
	// dragging. This value is boundary between clicking 
	// and dragging. If user hold mouse button longer 
	// than this value, it is a dragging. If shorter or 
	// equal to this time, it is a clicking.
	// This value is in milliseconds.
	static const int MAXCLICKTIME;

	// the timerid for left button checking
	static const int LTIMERID;
	
	// the timerid for right button checking
	static const int RTIMERID;
	
	// The state when left button is pressed
	APointI m_leftPoint;
	UINT m_leftFlags;
	
	// The state when right button is pressed
	APointI m_rightPoint;
	UINT m_rightFlags;

	// Store whether the destructor has been
	// called or not
	bool m_bDeleted;

	// This four function are used to
	// implement the distinguishing 
	// dragging from clicking
	void OnLButtonUpImpl(UINT nFlags, APointI point);
	void OnLButtonDownImpl(UINT nFlags, APointI point);
	void OnRButtonUpImpl(UINT nFlags, APointI point);
	void OnRButtonDownImpl(UINT nFlags, APointI point);
	void OnTimerImpl(UINT nIDEvent);
};

#endif 

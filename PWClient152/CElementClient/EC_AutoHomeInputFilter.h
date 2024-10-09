#pragma once
#include "APoint.h"

#include "EC_InputFilter.h"
class CECInputCtrl;

class CECAutoHomeInputFilter : public CECInputFilter
{
public:
	CECAutoHomeInputFilter(CECInputCtrl* pInputCtrl);
	virtual ~CECAutoHomeInputFilter();

	//	Mouse press information
	struct PRESS
	{
		bool	bPressed;		//	Pressed flag
		DWORD	dwPressTime;	//	Start pressing time stamp
		APointI	ptPress;		//	Press position
		bool	bTurnCamera;	//	Turn camera flag
	};
	
protected:

	PRESS		m_LBPress;			//	Left button press information
	PRESS		m_RBPress;			//	Right button press information

protected:	//	Operations

	//	Translate keyboard input
	bool TranslateKeyboard();
	//	Translate mouse input
	bool TranslateMouse();	

	//	Key message handlers
	void OnKeyDown(int iKey, DWORD dwFlags);
	void OnKeyUp(int iKey, DWORD dwFlags);
	//	Mouse message handlers
	void OnLButtonDown(int x, int y, DWORD dwFlags);
	void OnLButtonUp(int x, int y, DWORD dwFlags);
	void OnLButtonDblClk(int x, int y, DWORD dwFlags);
	void OnRButtonDown(int x, int y, DWORD dwFlags);
	void OnRButtonUp(int x, int y, DWORD dwFlags);
	void OnRButtonDblClk(int x, int y, DWORD dwFlags);
	void OnMouseMove(int x, int y, DWORD dwFlags);
	void OnMouseWheel(int x, int y, int iDelta, DWORD dwFlags);

public:
	virtual bool TranslateInput(int iInput);
};

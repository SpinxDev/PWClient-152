 /*
 * FILE: EC_InputCtrl.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AArray.h"
#include "AStack.h"
#include "APoint.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Flags of SHIFT, CONTROL, ALT key states
#define EC_KSF_SHIFT	0x0001
#define EC_KSF_CONTROL	0x0002
#define EC_KSF_ALT		0x0004
#define EC_KSF_CTRLKEYS	0x0007
#define EC_KSF_FIRSTPRESS 0x0100

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECInputFilter;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECInputCtrl
//	
///////////////////////////////////////////////////////////////////////////

class CECInputCtrl
{
public:		//	Types

	//	Some constants
	enum
	{
		NUM_VIRTUALKEY	= 256,	//	Number of virtual key
	};
	
	//	Windows message
	struct WINMSG
	{
		UINT	uMsg;
		WPARAM	wParam;
		LPARAM	lParam;
		DWORD	dwCtrlkeys;		//	Control keys (SHIFT, CTRL, ALT) states
	};

public:		//	Constructor and Destructor

	CECInputCtrl();
	virtual ~CECInputCtrl();

public:		//	Attributes

public:		//	Operations

	//	Release object
	void Release();

	//	Tick routine
	bool Tick();
	//	Process windows key message
	bool DealKeyMessage(UINT message, WPARAM wParam, LPARAM lParam);
	//	Process windows mouse message
	bool DealMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);

	//	Activate keyboard filter
	bool ActivateKBFilter(CECInputFilter* pFilter);
	//	Disactivate keyboard filter
	void DisactivateKBFilter(CECInputFilter* pFilter);
	//	Activate mouse filter
	bool ActivateMouseFilter(CECInputFilter* pFilter);
	//	Disactivate mouse filter
	void DisactivateMouseFilter(CECInputFilter* pFilter);

	//	Set common keyboard filter
	void SetComKeyboardFilter(CECInputFilter* pFilter);
	//	Set common mouse filter
	void SetComMouseFilter(CECInputFilter* pFilter);

	//	Clear filters in keyboard filter stack
	void ClearKBFilterStack() { m_KBFilterStack.RemoveAll(); }
	//	Clear filters in mouse filter stack
	void ClearMouFilterStack() { m_MouFilterStack.RemoveAll(); }

	//	Capture mouse
	void CaptureMouse(bool bCapture);
	//	Get mouse current position
	void GetMousePos(int* pix, int* piy);
	//	Set mosue cursor position
	void SetMousePos(int x, int y);
	//	Is mouse captured by us ?
	bool IsMouseCaptured();

	//	Key is being pressed ?
	bool KeyIsBeingPressed(int iKey);
	//	SHIFT is pressed ?
	bool IsShiftPressed(DWORD dwFlags) { return (dwFlags & EC_KSF_SHIFT) ? true : false; }
	//	CONTROL is pressed ?
	bool IsCtrlPressed(DWORD dwFlags) { return (dwFlags & EC_KSF_CONTROL) ? true : false; }
	//	ALT is pressed ?
	bool IsAltPressed(DWORD dwFlags) { return (dwFlags & EC_KSF_ALT) ? true : false; }
	// First pressed ?
	bool IsFirstPressed(DWORD dwFlags) { return (dwFlags & EC_KSF_FIRSTPRESS) != 0; }

	//	Get mouse message number
	int GetWinMouseMsgNum() { return m_aWinMouMsgs.GetSize(); }
	//	Get mouse message by index
	const WINMSG& GetWinMouseMsg(int n) { return m_aWinMouMsgs[n]; }
	//	Get key message number
	int GetWinKeyMsgNum() { return m_aWinKeyMsgs.GetSize(); }
	//	Get key message by index
	const WINMSG& GetWinKeyMsg(int n) { return m_aWinKeyMsgs[n]; }

protected:	//	Attributes

	AArray<WINMSG, WINMSG&>		m_aWinMouMsgs;	//	Windows mouse messages
	AArray<WINMSG, WINMSG&>		m_aWinKeyMsgs;	//	Windows key messages

	bool	m_aKeyStates[NUM_VIRTUALKEY];	//	Key pressing states
	int		m_iMouCapCnt;		//	Mouse capture counter
	DWORD	m_dwCtrlFlags;		//	Control key flags

	CECInputFilter*	m_pComKBFilter;		//	Common keyboard filter
	CECInputFilter* m_pComMouFilter;	//	Common mouse filter

	APtrStack<CECInputFilter*>	m_KBFilterStack;	//	Keyboard filter stack
	APtrStack<CECInputFilter*>	m_MouFilterStack;	//	Mouse filter stack

protected:	//	Operations

	//	Get keyboard filter but don't pop it
	CECInputFilter* PeekKBFilter()
	{
		if (m_KBFilterStack.GetElementNum())
			return m_KBFilterStack.Peek();
		else
			return NULL;
	}

	//	Get keyboard filter but don't pop it
	CECInputFilter* PeekMouFilter()
	{
		if (m_MouFilterStack.GetElementNum())
			return m_MouFilterStack.Peek();
		else
			return NULL;
	}

	//	Check key press state
	void CheckKeyPressStates();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



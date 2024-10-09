/*
 * FILE: EC_InputCtrl.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_InputCtrl.h"
#include "EC_InputFilter.h"
#include "EC_HostInputFilter.h"
#include "EC_GhostInputFilter.h"
#include "EC_Game.h"

#include "AC.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define KEYISPRESSED(k)	((GetAsyncKeyState(k) & 0x8000) ? true : false)

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECInputCtrl
//	
///////////////////////////////////////////////////////////////////////////

CECInputCtrl::CECInputCtrl() : m_aWinMouMsgs(0, 64), m_aWinKeyMsgs(0, 64)
{
	m_pComKBFilter	= NULL;
	m_pComMouFilter	= NULL;
	m_iMouCapCnt	= 0;
	m_dwCtrlFlags	= 0;

	memset(m_aKeyStates, 0, sizeof (m_aKeyStates));
}

CECInputCtrl::~CECInputCtrl()
{
	//	Release filters
}

//	Release object
void CECInputCtrl::Release()
{
}

//	Tick routine
bool CECInputCtrl::Tick()
{
	//	Synchronize SHIFT, CTRL and control key states
	CheckKeyPressStates();

	if (m_pComKBFilter)
		m_pComKBFilter->TranslateInput(CECInputFilter::IT_KEYBOARD);

	if (m_pComMouFilter)
		m_pComMouFilter->TranslateInput(CECInputFilter::IT_MOUSE);

	CECInputFilter* pFilter = PeekKBFilter();
	if (pFilter)
		pFilter->TranslateInput(CECInputFilter::IT_KEYBOARD);

	pFilter = PeekMouFilter();
	if (pFilter)
		pFilter->TranslateInput(CECInputFilter::IT_MOUSE);

	//	Clear messages
	m_aWinMouMsgs.RemoveAll(false);
	m_aWinKeyMsgs.RemoveAll(false);

	return true;
}

//	Check key press state. Because we set key states depending on window's
//	message (WM_KEYDOWN and WM_KEYUP), we may get wrong key states when these
//	message missed (usually caused by application switching). So we check key
//	states every tick.
void CECInputCtrl::CheckKeyPressStates()
{
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		m_dwCtrlFlags |= EC_KSF_SHIFT;
	else
		m_dwCtrlFlags &= ~EC_KSF_SHIFT;

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		m_dwCtrlFlags |= EC_KSF_CONTROL;
	else
		m_dwCtrlFlags &= ~EC_KSF_CONTROL;

	if (GetAsyncKeyState(VK_MENU) & 0x8000)
		m_dwCtrlFlags |= EC_KSF_ALT;
	else
		m_dwCtrlFlags &= ~EC_KSF_ALT;

	for (int i=0; i < NUM_VIRTUALKEY; i++)
	{
		if (!(GetAsyncKeyState(i) & 0x8000))
			m_aKeyStates[i] = false;
	}
}

//	Key is being pressed ?
bool CECInputCtrl::KeyIsBeingPressed(int iKey)
{
//	return (GetAsyncKeyState(iKey) & 0x8000) ? true : false;
	return m_aKeyStates[iKey];
}

//	Process windows key message
bool CECInputCtrl::DealKeyMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	bool bGenMsg = false;
	bool bFirstPress = false;

	switch (message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:

		if (wParam == VK_SHIFT)
			m_dwCtrlFlags |= EC_KSF_SHIFT;
		else if (wParam == VK_CONTROL)
			m_dwCtrlFlags |= EC_KSF_CONTROL;
		else if (wParam == VK_MENU)
			m_dwCtrlFlags |= EC_KSF_ALT;
		else
			bGenMsg = true;

		if (!m_aKeyStates[wParam])
			bFirstPress = true;

		m_aKeyStates[wParam] = true;
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:

		if (wParam == VK_SHIFT)
			m_dwCtrlFlags &= ~EC_KSF_SHIFT;
		else if (wParam == VK_CONTROL)
			m_dwCtrlFlags &= ~EC_KSF_CONTROL;
		else if (wParam == VK_MENU)
			m_dwCtrlFlags &= ~EC_KSF_ALT;
		else
			bGenMsg = true;

		m_aKeyStates[wParam] = false;
		break;
	}

	if (bGenMsg)
	{
		WINMSG Msg;

		Msg.uMsg		= message;
		Msg.wParam		= wParam;
		Msg.lParam		= lParam;

		Msg.dwCtrlkeys	= m_dwCtrlFlags;
		if (bFirstPress)
			Msg.dwCtrlkeys |= EC_KSF_FIRSTPRESS;

		m_aWinKeyMsgs.Add(Msg);
	}

	return true;
}

//	Process windows mouse message
bool CECInputCtrl::DealMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	WINMSG Msg;
	Msg.uMsg		= message;
	Msg.wParam		= wParam;
	Msg.lParam		= lParam;
	Msg.dwCtrlkeys	= 0;

	if (wParam & MK_CONTROL)
		Msg.dwCtrlkeys |= EC_KSF_CONTROL;

	if (wParam & MK_SHIFT)
		Msg.dwCtrlkeys |= EC_KSF_SHIFT;

	if (m_dwCtrlFlags & EC_KSF_ALT)
		Msg.dwCtrlkeys |= EC_KSF_ALT;

	m_aWinMouMsgs.Add(Msg);

	return true;
}

//	Capture mouse
void CECInputCtrl::CaptureMouse(bool bCapture)
{
	HWND hWnd = g_pGame->GetGameInit().hWnd;

	if (bCapture)
	{
		if (::GetCapture() != hWnd)
		{
			::SetCapture(hWnd);
			m_iMouCapCnt = 1;
		}
		else
			m_iMouCapCnt++;
	}
	else
	{
		if (::GetCapture() == hWnd)
		{
			if (!(--m_iMouCapCnt))
				::ReleaseCapture();
		}
	}
}

//	Mouse is captured by us ?
bool CECInputCtrl::IsMouseCaptured()
{
	HWND hWnd = g_pGame->GetGameInit().hWnd;
	return ::GetCapture() == hWnd;
}

//	Activate keyboard filter
bool CECInputCtrl::ActivateKBFilter(CECInputFilter* pFilter)
{
	ASSERT(pFilter);
	m_KBFilterStack.Push(pFilter);
	return true;
}

//	Disactivate keyboard filter
void CECInputCtrl::DisactivateKBFilter(CECInputFilter* pFilter)
{
	if (m_KBFilterStack.GetElementNum())
	{
		CECInputFilter* pTopFilter = m_KBFilterStack.Peek();
		if (pFilter == pTopFilter)
			m_KBFilterStack.Pop();
		else
		{
			ASSERT(pFilter == pTopFilter);
		}
	}
}

//	Activate mouse filter
bool CECInputCtrl::ActivateMouseFilter(CECInputFilter* pFilter)
{
	ASSERT(pFilter);
	m_MouFilterStack.Push(pFilter);
	return true;
}

//	Disactivate mouse filter
void CECInputCtrl::DisactivateMouseFilter(CECInputFilter* pFilter)
{
	if (m_MouFilterStack.GetElementNum())
	{
		CECInputFilter* pTopFilter = m_MouFilterStack.Peek();
		if (pFilter == pTopFilter)
			m_MouFilterStack.Pop();
		else
		{
			ASSERT(pFilter == pTopFilter);
		}
	}
}

//	Set common keyboard filter
void CECInputCtrl::SetComKeyboardFilter(CECInputFilter* pFilter)
{
	m_pComKBFilter = pFilter;
}

//	Set common mouse filter
void CECInputCtrl::SetComMouseFilter(CECInputFilter* pFilter)
{
	m_pComMouFilter = pFilter;
}

//	Get mouse current position
void CECInputCtrl::GetMousePos(int* pix, int* piy)
{
	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(g_pGame->GetGameInit().hWnd, &pt);

	*pix = pt.x;
	*piy = pt.y;
}

//	Set mosue cursor position
void CECInputCtrl::SetMousePos(int x, int y)
{
	POINT pt = {x, y};
	::ClientToScreen(g_pGame->GetGameInit().hWnd, &pt);
	::SetCursorPos(pt.x, pt.y);
}
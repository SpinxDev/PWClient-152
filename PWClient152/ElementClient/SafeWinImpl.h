/////////////////////////////////////////////////////////////////////////////
//
// SafeWinImpl.h : Defines a class fixing the crash in CWindowImpl when
//                 OnFinalMessage destroys the instance
//
/////////////////////////////////////////////////////////////////////////////
//
// Written by Alexander Nickolov (agnickolov@mvps.org)
//
/////////////////////////////////////////////////////////////////////////////

#include <atlwin.h>

#if !defined(_ATL_VER)
#error ERROR: This header requires ATL!
#endif


#ifndef __SAFEWINIMPL_H_
#define __SAFEWINIMPL_H_


/////////////////////////////////////////////////////////////////////////////
// CWindowImpl fix

template<typename T, typename Base = CWindow, typename Traits =
CControlWinTraits>
class CWindowImplSafe :
	public CWindowImpl<T, Base, Traits>
{
public:
	CWindowImplSafe() :
	  m_hwndSave(NULL)
	  {
	  }

public:
	virtual WNDPROC GetWindowProc()
	{
		return WindowProcSafe;
	}
	static LRESULT CALLBACK WindowProcSafe(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Implementation helpers
protected:
	HWND m_hwndSave;
};

#if (_ATL_VER >= 0x0700)

template <typename T, typename Base, typename Traits>
LRESULT CALLBACK CWindowImplSafe<T, Base, Traits>::WindowProcSafe(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWindowImplSafe<T, Base, Traits>* pThis = (CWindowImplSafe<T, Base, Traits>*)hWnd;
	// set a ptr to this message and save the old value
	_ATL_MSG msg(pThis->m_hWnd, uMsg, wParam, lParam);
	const _ATL_MSG* pOldMsg = pThis->m_pCurrentMsg;
	pThis->m_pCurrentMsg = &msg;
	// pass to the message map to process
	LRESULT lRes;
	BOOL bRet = pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0);
	// restore saved value for the current message
	ATLASSERT(pThis->m_pCurrentMsg == &msg);
	pThis->m_pCurrentMsg = pOldMsg;
	// do the default processing if message was not handled
	if(!bRet)
	{
		if(uMsg != WM_NCDESTROY)
			lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
		else
		{
			// unsubclass, if needed
			LONG_PTR pfnWndProc = ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC);
			lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
			if(pThis->m_pfnSuperWindowProc != ::DefWindowProc && ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC) == pfnWndProc)
				::SetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC, (LONG_PTR)pThis->m_pfnSuperWindowProc);
			// mark window as destryed
			pThis->m_dwState |= WINSTATE_DESTROYED;
			pThis->m_hwndSave = pThis->m_hWnd;
			pThis->m_hWnd     = NULL;
		}
	}
	if((pThis->m_dwState & WINSTATE_DESTROYED) && (pThis->m_pCurrentMsg == NULL))
	{
		// clear out the saved handle
		HWND hWnd = pThis->m_hwndSave;
		pThis->m_hwndSave = NULL;
		pThis->m_dwState &= ~WINSTATE_DESTROYED;
		// clean up after window is destroyed
		pThis->OnFinalMessage(hWnd);
	}
	return lRes;
}

#else

template <typename T, typename Base, typename Traits>
LRESULT CALLBACK CWindowImplSafe<T, Base, Traits>::WindowProcSafe(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWindowImplSafe<T, Base, Traits>* pThis = (CWindowImplSafe<T, Base, Traits>*)hWnd;
	// set a ptr to this message and save the old value
	MSG msg = { pThis->m_hWnd, uMsg, wParam, lParam, 0, { 0, 0 } };
	const MSG* pOldMsg = pThis->m_pCurrentMsg;
	pThis->m_pCurrentMsg = &msg;
	// pass to the message map to process
	LRESULT lRes;
	BOOL bRet = pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0);
	// restore saved value for the current message
	ATLASSERT(pThis->m_pCurrentMsg == &msg);
	pThis->m_pCurrentMsg = pOldMsg;
	// do the default processing if message was not handled
	if(!bRet)
	{
		if(uMsg != WM_NCDESTROY)
			lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
		else
		{
			// unsubclass, if needed
			LONG pfnWndProc = ::GetWindowLong(pThis->m_hWnd, GWL_WNDPROC);
			lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
			if(pThis->m_pfnSuperWindowProc != ::DefWindowProc && ::GetWindowLong(pThis->m_hWnd, GWL_WNDPROC) == pfnWndProc)
				::SetWindowLong(pThis->m_hWnd, GWL_WNDPROC, (LONG)pThis->m_pfnSuperWindowProc);
			// clear out window handle
			pThis->m_hwndSave = pThis->m_hWnd;
			pThis->m_hWnd     = NULL;
		}
	}
	if ((pThis->m_hwndSave != NULL) && (pThis->m_pCurrentMsg == NULL)) {
		// clear out the saved handle
		HWND hWnd = pThis->m_hwndSave;
		pThis->m_hwndSave = NULL;
		// clean up after window is destroyed
		pThis->OnFinalMessage(hWnd);
	}
	return lRes;
}

#endif


#endif /* !__SAFEWINIMPL_H_ */


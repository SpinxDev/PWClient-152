// WndInput.cpp : implementation file
//

#include "Global.h"
#include "WndInput.h"
#include "AMemory.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CWndInput

CWndInput::CWndInput()
{
	m_iDataType		= DATA_INTEGER;
	m_bEditing		= false;
	m_dwUserData	= 0;
	m_pNotifyWnd	= NULL;
}

CWndInput::~CWndInput()
{
}


BEGIN_MESSAGE_MAP(CWndInput, CWnd)
	//{{AFX_MSG_MAP(CWndInput)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndInput message handlers

BOOL CWndInput::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
					   const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (dwStyle & WS_CHILD)
		dwStyle |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	else
		dwStyle |= WS_CLIPCHILDREN;

	if (!CWnd::CreateEx(0, AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW))), 
						lpszWindowName,	dwStyle, rect, pParentWnd, nID, pContext))
		return FALSE;

	//	Create edit control
	CRect rcEdit = rect;
	rcEdit.DeflateRect(0, 0);
	if (!m_EditWnd.Create(WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, rcEdit, this,	ID_EDITWINDOW))
		return FALSE;

	//	Add a frame to edit box
	DWORD dwEditStyle = ::GetWindowLong(m_EditWnd.m_hWnd, GWL_STYLE);
	::SetWindowLong(m_EditWnd.m_hWnd, GWL_STYLE, dwEditStyle | WS_BORDER);

	return TRUE;
}

void CWndInput::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if (IsWindow(m_EditWnd.GetSafeHwnd()))
		m_EditWnd.MoveWindow(0, 0, cx-0, cy-0);
}

//	Use parent window's font as edit box font
void CWndInput::InheritFont()
{
	CWnd* pParent = GetParent();
	if (!pParent)
		return;

	//	Get parent window's font
	CFont* pFont = pParent->GetFont();
	if (!pFont)
		return;

	LOGFONT Info;
	pFont->GetLogFont(&Info);

	m_Font.CreateFontIndirect(&Info);
	m_EditWnd.SetFont(&m_Font);
}

//	Popup window, begin to input data
bool CWndInput::Popup(CWnd* pNotifyWnd, DWORD dwUserData, int iDataType, void* pIniData, const RECT& rc)
{
	if (iDataType < DATA_INTEGER || iDataType > DATA_STRING)
		return false;

	CString str = "";

	if (pIniData)
	{
		switch (iDataType)
		{
		case DATA_INTEGER:	str.Format("%d", *(int*)pIniData);		break;
		case DATA_FLOAT:	str.Format("%f", *(float*)pIniData);	break;
		case DATA_STRING:	str = (char*)pIniData;					break;
		}
	}

	m_iDataType		= iDataType;
	m_dwUserData	= dwUserData;
	m_pNotifyWnd	= pNotifyWnd;

	//	Adjust window's position and size
	SetWindowPos(GetParent(), rc.left, rc.top, rc.right-rc.left+1, rc.bottom-rc.top+1, 0);

	m_EditWnd.SetFocus();
	m_EditWnd.SetWindowText(str);
	m_EditWnd.SetSel(0, -1);

	ShowWindow(SW_SHOW);
	UpdateWindow();

	m_bEditing = true;

	return true;
}

//	Close window, end inputing data
void CWndInput::Close()
{
	//	Get editing result
	m_EditWnd.GetWindowText(m_strResult);
	ShowWindow(SW_HIDE);
}

/*	End editing and close input window.

	bCancel: true, user cancel editing.
	bClickEnd: user click outside of input window to close it.
	pPoint: if bClickEnd is true, this is click point.
*/
void CWndInput::EndEditing(bool bCancel, bool bClickEnd, const POINT* pPoint)
{
	if (!m_bEditing)
		return;

	if (bClickEnd && pPoint)
	{
		RECT rcWnd;
		GetWindowRect(&rcWnd);

		POINT pt = *pPoint;
		ClientToScreen(&pt);

		if (PtInRect(&rcWnd, pt))
			return;
	}

	m_bEditing = false;

	Close();

	//	Send message to specified window
	if (m_pNotifyWnd)
		m_pNotifyWnd->SendMessage(WM_INPUTWNDEND, bCancel ? 1 : 0, m_dwUserData);
}

void CWndInput::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_bEditing)
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	EndEditing(false, true, &point);

	CWnd::OnLButtonDown(nFlags, point);
}

void CWndInput::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_bEditing)
	{
		CWnd::OnRButtonDown(nFlags, point);
		return;
	}

	EndEditing(false, true, &point);
	
	CWnd::OnRButtonDown(nFlags, point);
}

BOOL CWndInput::PreTranslateMessage(MSG* pMsg) 
{
	if (!m_bEditing)
		return CWnd::PreTranslateMessage(pMsg);

	if (pMsg->hwnd == m_EditWnd.GetSafeHwnd() && pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
			EndEditing(false, false, NULL);
		else if (pMsg->wParam == VK_ESCAPE)
			EndEditing(true, false, NULL);
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CWndInput::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	
	if (m_bEditing && nState == WA_INACTIVE)
		EndEditing(false, false, NULL);
}



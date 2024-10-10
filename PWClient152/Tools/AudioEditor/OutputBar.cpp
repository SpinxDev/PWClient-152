// outputbar.cpp : implementation of the COutputBar class
//

#include "stdafx.h"
#include "OutputBar.h"
#include "Global.h"
#include "FLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 1;
extern CString		g_csLog;
extern HWND			g_hOutputBar;

using AudioEngine::LogLevel;
using AudioEngine::LOG_ERROR;
using AudioEngine::LOG_WARNING;

/////////////////////////////////////////////////////////////////////////////
// COutputBar

BEGIN_MESSAGE_MAP(COutputBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(COutputBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_MESSAGE(WM_LOG, &COutputBar::OnSetOutputWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputBar construction/destruction

COutputBar::COutputBar()
{
	// TODO: add one-time construction code here
}

COutputBar::~COutputBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// COutputBar message handlers

int COutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create list window.
	// TODO: create your own window here:
	const DWORD dwViewStyle =	
		LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY
		| ES_WANTRETURN | ES_AUTOVSCROLL | WS_VSCROLL | ES_NOHIDESEL;
	
	if (!m_wndEdit.Create (dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create output view\n");
		return -1;      // fail to create
	}
	
	g_hOutputBar = m_hWnd;

	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	lf.lfHeight = -MulDiv(9, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);//9ºÅ×Ö
	strcpy(lf.lfFaceName, "ËÎÌå");
	VERIFY(m_font.CreateFontIndirect(&lf));
	m_wndEdit.SetFont(&m_font);
	return 0;
}

void COutputBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	m_wndEdit.SetWindowPos (NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectList;
	m_wndEdit.GetWindowRect (rectList);
	ScreenToClient (rectList);

	rectList.InflateRect (1, 1);
	dc.Draw3dRect (rectList,	::GetSysColor (COLOR_3DSHADOW), 
								::GetSysColor (COLOR_3DSHADOW));
}

LRESULT COutputBar::OnSetOutputWindow(WPARAM wParam, LPARAM lParam )
{
	static int iLineCnt = 0;

	if(iLineCnt % 500 == 0 && iLineCnt > 0)
		m_wndEdit.SetWindowText("");

	CHARFORMAT cf={0};
	cf.cbSize =   sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;   
	cf.dwEffects &= ~CFE_AUTOCOLOR;

	bool bSetColor = false;
	int nStart;
	int nEnd;
	nStart = m_wndEdit.GetTextLengthEx(GTL_DEFAULT);
	m_wndEdit.SetSel(-1,-1);
	m_wndEdit.ReplaceSel(g_csLog);
	
	if((LogLevel)lParam == LOG_ERROR)
	{
		cf.crTextColor = RGB(255,0,0);
	}
	else
		if ((LogLevel)lParam == LOG_WARNING)
	{
		cf.crTextColor = RGB(200,100,0);
	}
	else
	{
		cf.crTextColor = RGB(0,0,0);
	}
	nEnd = m_wndEdit.GetTextLengthEx(GTL_DEFAULT);
	m_wndEdit.SetSel(nStart, nEnd);
	m_wndEdit.SetSelectionCharFormat(cf);
	m_wndEdit.HideSelection(TRUE,   FALSE);
	m_wndEdit.LineScroll(1);	
	m_wndEdit.SetSel(-1,-1);

	++iLineCnt;
	return 0;
}
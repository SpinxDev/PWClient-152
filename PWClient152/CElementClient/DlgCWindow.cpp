// Filename	: DlgCWindow.cpp
// Creator	: Xiao Zhou
// Date		: 2008/11/27

#include "AFI.h"
#include "DlgCWindow.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "AUIListBox.h"
#include "WndThreadManager.h"
#include "CSplit.h"
#include "EC_Resource.h"
#include "A2DSprite.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgCWindow, CDlgBase)

AUI_ON_COMMAND("select",	OnCommandSelect)
AUI_ON_COMMAND("refresh",	OnCommandRefresh)
AUI_ON_COMMAND("clear",		OnCommandClear)
AUI_ON_COMMAND("IDCANCEL",	OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgCWindow, CDlgBase)

AUI_ON_EVENT("Img_ZoomIn",			WM_LBUTTONDOWN,		OnEventLButtonDown_Img_ZoomIn)
AUI_ON_EVENT("Img_ZoomIn",			WM_LBUTTONUP,		OnEventLButtonUp_Img_ZoomIn)
AUI_ON_EVENT("Img_ZoomIn",			WM_MOUSEMOVE,		OnEventMouseMove_Img_ZoomIn)

AUI_END_EVENT_MAP()

int CDlgCWindow::m_nCWindowNum = 0;
#define PIXEL_PER_STEP 5
#define CWINDOW_MIN_WIDTH 300
#define CWINDOW_MIN_HEIGHT 200

CDlgCWindow::CDlgCWindow()
{
}

CDlgCWindow::~CDlgCWindow()
{
}

bool CDlgCWindow::OnInitDialog()
{
	m_pWPic = (PAUIWINDOWPICTURE)GetDlgItem("WPic");
	if( GetDlgItem("Img_ZoomIn") )
		GetDlgItem("Img_ZoomIn")->SetAcceptMouseMessage(true);
	m_pImg_ZoomIn = GetDlgItem("Img_ZoomIn");
	m_pImg_Title = GetDlgItem("Img_Title");
//	m_pChk_IsWindow = (PAUICHECKBOX)GetDlgItem("Chk_IsWindow");
	m_pScl_Horizon = (PAUISCROLL)GetDlgItem("Scl_Horizon");
	m_pScl_Vertical = (PAUISCROLL)GetDlgItem("Scl_Vertical");
	return true;
}

void CDlgCWindow::OnShowDialog()
{
	if( stricmp(m_szName, "Win_CWindowMain") == 0 )
		OnCommandRefresh("");
	else
	{
		m_pScl_Horizon->SetBarLength(5);
		m_pScl_Horizon->SetScrollStep(2);
		m_pScl_Horizon->SetBarLevel(0);
		m_pScl_Vertical->SetBarLength(5);
		m_pScl_Vertical->SetScrollStep(2);
		m_pScl_Vertical->SetBarLevel(0);
		if (IsIconic(m_pWPic->GetHWnd()))
		{
			ShowWindow(m_pWPic->GetHWnd(), SW_SHOWNOACTIVATE);
		}
	}
}

void CDlgCWindow::OnTick()
{
	if( stricmp(m_szName, "Win_CWindowMain") == 0 )
	{
		CWndThreadManager* pWndThreadMan = GetAUIManager()->GetWndThreadManager();
		DWORD dwNum = pWndThreadMan->GetWindowNums();
		HWND* pHWnd = (HWND*)a_malloctemp(sizeof(HWND) * dwNum);
		pWndThreadMan->GetWindowList(pHWnd, dwNum, &dwNum);
		for(int i = 0; i < 10; i++)
		{
			char szName[50];
			sprintf(szName, "Win_CWindow%d", i);
			CDlgCWindow* pDlg = (CDlgCWindow*)GetAUIManager()->GetDialog(szName);
			if( pDlg )
			{
				bool b = false;
				for(DWORD j = 0; j < dwNum; j++)
					if( pHWnd[j] != NULL && pDlg->m_pWPic->GetHWnd() == pHWnd[j] )
					{
						pHWnd[j] = NULL;
						b = true;
						break;
					}

				if( !b )
				{
					pDlg->m_pWPic->SetHWnd(NULL);
					pDlg->Show(false);
				}
			}
		}
		if(dwNum>0)
		{				
			pWndThreadMan->SetTopMostWindow(this->GetGame()->GetGameInit().hWnd);
		}
		else
		{
			pWndThreadMan->SetTopMostWindow(NULL);
			//::SetWindowPos(this->GetGame()->GetGameInit().hWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);	
		}
		for(DWORD j = 0; j < dwNum; j++)
			if( pHWnd[j] != NULL )
			{
				for(int i = 0; i < 10; i++)
				{
					char szName[50];
					sprintf(szName, "Win_CWindow%d", i);
					CDlgCWindow* pDlg = (CDlgCWindow*)GetAUIManager()->GetDialog(szName);
					if( !pDlg )
					{
						GetAUIManager()->CreateDlg(m_pA3DEngine, m_pA3DDevice, "CWindow.xml");
						pDlg = (CDlgCWindow*)GetAUIManager()->GetDialog("Win_CWindow");
						pDlg->SetName(szName);
					}
					if( !pDlg->m_pWPic->GetHWnd() )
					{
						pDlg->m_pWPic->SetHWnd(pHWnd[j]);
						pDlg->Show(true);
						break;
					}
				}
			}
		a_freetemp(pHWnd);
	}
	else
	{
		if( GetAUIManager()->GetActiveDialog() == this )
		{
			if (!GetFocus() || !strstr(GetFocus()->GetName(), "Scl_"))
			{
				ChangeFocus(m_pWPic);
			}
		}
		if( !m_pWPic->GetHWnd() || !GetAUIManager()->GetDialog("Win_CWindowMain")->IsShow() )
			Show(false);
	}
}

bool CDlgCWindow::Render()
{
	if( stricmp(m_szName, "Win_CWindowMain") == 0 )
	{

	}
	else
	{
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		int WidthMax = p->Width - 3 - 27;
		int HeightMax = p->Height - 15 - 20;
		int x = GetPos().x;
		int y = GetPos().y;
		int Width = m_pWPic->GetSize().cx;
		int Height = m_pWPic->GetSize().cy;
		int OriginalW = m_pWPic->GetImage()->GetWidth();
		int OriginalH = m_pWPic->GetImage()->GetHeight();
		A3DRECT rcDisplay;
		int newX = GetPos().x;
		int newY = GetPos().y;
		int NewWidth = OriginalW;
		int NewHeight = OriginalH;
		
		if (m_pWPic->GetPos().x + OriginalW > WidthMax + 3)
		{
			newX = 0;
		}
		if (m_pWPic->GetPos().y + OriginalH > HeightMax + 15)
		{
			newY = 0;
		}
		
		if (OriginalW >= WidthMax)
		{
			NewWidth = WidthMax;
		}
		if (OriginalH >= HeightMax)
		{
			NewHeight = HeightMax;
		}
		
		if (Width >= OriginalW)
		{
			m_pScl_Horizon->Show(false);
			NewWidth = OriginalW;
		}
		else
		{
			m_pScl_Horizon->Show(true);
		}
		
		if (Height >= OriginalH)
		{
			m_pScl_Vertical->Show(false);
			NewHeight = OriginalH;
		}
		else
		{	
			m_pScl_Vertical->Show(true);
		}
		
		if (m_pScl_Vertical->IsShow())
		{
			int MaxLevel = (OriginalH - NewHeight)/PIXEL_PER_STEP;
			m_pScl_Vertical->SetScrollRange(0, MaxLevel);
			int level = m_pScl_Vertical->GetBarLevel();
			m_pScl_Vertical->SetBarLength(NewHeight/PIXEL_PER_STEP);
			rcDisplay.top = level*PIXEL_PER_STEP;
			rcDisplay.bottom = NewHeight + level * PIXEL_PER_STEP;
		}
		else
		{
			rcDisplay.top = 0;
			rcDisplay.bottom = NewHeight;
		}
		if (m_pScl_Horizon->IsShow())
		{
			int MaxLevel = (OriginalW - NewWidth)/PIXEL_PER_STEP;
			m_pScl_Horizon->SetScrollRange(0, MaxLevel);
			int level = m_pScl_Horizon->GetBarLevel();
			m_pScl_Horizon->SetBarLength(NewWidth/PIXEL_PER_STEP);
			rcDisplay.left = level*PIXEL_PER_STEP;
			rcDisplay.right = NewWidth + level * PIXEL_PER_STEP;
		}
		else
		{
			rcDisplay.left = 0;
			rcDisplay.right= NewWidth;
		}
		SetPosEx(newX, newY);
		m_pWPic->SetDisplayRect(false, rcDisplay);
		WindowResize(NewWidth + 30, NewHeight + 35);
	}
	return CDlgBase::Render();
}


BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	if( !IsWindowVisible(hWnd) || !IsWindowEnabled(hWnd) )
		return TRUE;

	LONG gwl_style = GetWindowLong(hWnd,GWL_STYLE);
    if ((gwl_style & WS_POPUP) && !(gwl_style & WS_CAPTION))
        return TRUE;

    HWND hParent = (HWND)GetWindowLong(hWnd, GWL_HWNDPARENT);
    if (IsWindowEnabled(hParent))
        return TRUE;

    if (IsWindowVisible(hParent))
        return TRUE;

	ACHAR szWindowName[1024];
    int nLen = GetClassName(hWnd, szWindowName, 1024);
    if (!wcscmp(szWindowName, _AL("Shell_TrayWnd")))
        return TRUE;
    if (!wcscmp(szWindowName, _AL("ElementClient Window")))
        return TRUE;

	szWindowName[nLen] = '\t';
    GetWindowText(hWnd, szWindowName + nLen + 1, 1024);

	PAUILISTBOX pList = (PAUILISTBOX)lParam;
	pList->AddString(szWindowName);

	return TRUE;
}

void CDlgCWindow::OnCommandRefresh(const char *szCommand)
{
	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("Lst_Window");
	pList->ResetContent();
	EnumWindows(EnumWindowsProc, (LPARAM)pList);
}

void CDlgCWindow::OnCommandClear(const char *szCommand)
{
	GetAUIManager()->ScoutWindow(NULL);
}

void CDlgCWindow::OnCommandSelect(const char *szCommand)
{
	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("Lst_Window");
	int nSel = pList->GetCurSel();
	if( nSel >= 0 && nSel < pList->GetCount() )
	{
		CSplit s(pList->GetText(nSel));
		CSplit::VectorAWString vec = s.Split(_AL("\t"));
		HWND hWnd = FindWindow(vec[0], vec[1]);
		if( hWnd )
		{
			GetDlgItem("Txt_Class")->SetText(vec[0]);
			GetDlgItem("Txt_Name")->SetText(vec[1]);
			GetAUIManager()->ScoutWindow(hWnd);
		}
	}
}

void CDlgCWindow::OnCommandCancel(const char *szCommand)
{
	if( stricmp(m_szName, "Win_CWindowMain") == 0 )
		Show(false);
	else
		PostMessage(m_pWPic->GetHWnd(), WM_QUIT, 0, 0);
}

void CDlgCWindow::OnEventLButtonDown_Img_ZoomIn(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	GetGame()->ChangeCursor(RES_CUR_HAND);
	SetCaptureObject(pObj);
	m_nMouseLastX = GET_X_LPARAM(lParam);
	m_nMouseLastY = GET_Y_LPARAM(lParam);
	m_nOldSize = GetSize();
}

void CDlgCWindow::OnEventLButtonUp_Img_ZoomIn(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	GetGame()->ChangeCursor(RES_CUR_NORMAL);
	SetCaptureObject(NULL);
}

void CDlgCWindow::OnEventMouseMove_Img_ZoomIn(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	GetGame()->ChangeCursor(RES_CUR_HAND);
	if( GetCaptureObject() == pObj )
	{
		SIZE size = m_nOldSize;
		size.cx -= m_nMouseLastX - GET_X_LPARAM(lParam);
		size.cy -= m_nMouseLastY - GET_Y_LPARAM(lParam);
		WindowResize(size.cx, size.cy);		
	}
}

void CDlgCWindow::WindowResize(int width, int height)
{
	int x = width;
	int y = height;
	int nMaxX = m_pWPic->GetImage()->GetWidth() + m_pImg_ZoomIn->GetSize().cx+ 5;
	int nMaxY = m_pWPic->GetImage()->GetHeight() + m_pImg_ZoomIn->GetSize().cy + m_pImg_Title->GetSize().cy + 10;
	a_Clamp(x, CWINDOW_MIN_WIDTH, 3000);
	a_Clamp(y, CWINDOW_MIN_HEIGHT, 2000);

	SetSize(x, y);
	
	SIZE size = GetSize();
	m_pWPic->SetSize(size.cx - 30, size.cy - 35);
	m_pImg_ZoomIn->SetPos(GetSize().cx - m_pImg_ZoomIn->GetSize().cx-3, GetSize().cy - m_pImg_ZoomIn->GetSize().cy-10);
	m_pImg_Title->SetSize(size.cx, m_pImg_Title->GetSize().cy);
	
	m_pScl_Horizon->SetPos(m_pScl_Horizon->GetPos(true).x, GetSize().cy - m_pScl_Horizon->GetSize().cy-10);
	m_pScl_Horizon->SetSize(m_pImg_ZoomIn->GetPos(true).x - 10 - m_pScl_Horizon->GetPos(true).x, m_pScl_Horizon->GetSize().cy);
	m_pScl_Vertical->SetPos(GetSize().cx - m_pScl_Vertical->GetSize().cx - 10, m_pScl_Vertical->GetPos(true).y);
	m_pScl_Vertical->SetSize(m_pScl_Vertical->GetSize().cx, m_pImg_ZoomIn->GetPos(true).y - 10 - m_pScl_Vertical->GetPos(true).y);
	if (IsZoomed(m_pWPic->GetHWnd()))
	{
		//	PostMessage(m_pWPic->GetHWnd(), WM_SYSCOMMAND, SW_RESTORE, 0);
		ShowWindow(m_pWPic->GetHWnd(), SW_SHOWNOACTIVATE);
	}
	SetWindowPos(m_pWPic->GetHWnd(), HWND_NOTOPMOST, 0, 0, m_pWPic->GetSize().cx, m_pWPic->GetSize().cy, SWP_NOMOVE | SWP_NOACTIVATE);
}
 // ViewFrame.cpp : implementation of the CViewFrame class
//

#include "Global.h"
#include "ElementMap.h"
#include "ViewFrame.h"
#include "Render.h"

#include "A3D.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CViewFrame

CViewFrame::CViewFrame()
{
	m_strTitle	= "";
	m_pMap		= NULL;
	m_pOperation = NULL;
}

CViewFrame::~CViewFrame()
{
}


BEGIN_MESSAGE_MAP(CViewFrame, CWnd)
	//{{AFX_MSG_MAP(CViewFrame)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_VIEW_RESETCAMERA, OnViewResetcamera)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_KEYDOWN, OnKeyDown)
	ON_MESSAGE(WM_KEYUP, OnKeyUp)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CViewFrame message handlers

BOOL CViewFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_BTNFACE+1), NULL);

	return TRUE;
}

BOOL CViewFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
		return FALSE;

	//	Create render window.
	//	Note: when CViewFrame is created rect is (0, 0, 0, 0), m_RenderWnd's real
	//	position and size will be adjusted in OnSize()
	if (!m_RenderWnd.Create(rect, this, 100))
		return FALSE;

	return TRUE;
}

//	Initlaize device objects
bool CViewFrame::InitDeviceObjects()
{
	//	Create a A3DTerrain2 object and release it, this ensure all
	//	global objects like A3DTerrain2Env to be initialized.
	A3DTerrain2 Terrain;
	Terrain.Init(g_Render.GetA3DEngine(), 100.0f, 100.0f);
	Terrain.Release();

	if (!m_RenderWnd.InitDeviceObjects())
		return false;

	//	Activate default viewport
	ActivateViewport(VIEW_PERSPECTIVE);

	return true;
}

void CViewFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	//	Draw title
	DrawTitle(&dc);
	Render();
	// Do not call CWnd::OnPaint() for painting messages
}

void CViewFrame::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if (!m_RenderWnd.m_hWnd)
		return;

	RECT rcWnd;
	GetClientRect(&rcWnd);
	rcWnd.top += SIZE_TITLEHEIGHT;

	m_RenderWnd.MoveWindow(&rcWnd);
}

//	Tick routine
bool CViewFrame::FrameMove(DWORD dwDeltaTime)
{
	m_RenderWnd.FrameMove(dwDeltaTime);
	return true;
}

//	render routine
bool CViewFrame::Render()
{
	m_RenderWnd.Render();
	return true;
}

//	Get current active viewport
CViewport* CViewFrame::GetActiveViewport()
{
	int iView = m_RenderWnd.GetActiveViewport();
	if (iView < 0)
		return NULL;

	return m_RenderWnd.GetViewport(iView);
}

//	Change currently active viewport
bool CViewFrame::ActivateViewport(int iNewView)
{
	m_RenderWnd.ActivateViewport(iNewView);

	//	Change title bar
	switch (m_RenderWnd.GetActiveViewport())
	{
	case VIEW_XY:			m_strTitle = "Front";		break;
	case VIEW_XZ:			m_strTitle = "Top";			break;
	case VIEW_YZ:			m_strTitle = "Left";		break;
	case VIEW_PERSPECTIVE:	m_strTitle = "Perspective";	break;
	default:
		ASSERT(0);
		return false;
	}

	//	Force to update title
	InvalidateRect(NULL);

	return true;
}

//	Draw title
void CViewFrame::DrawTitle(CDC* pDC)
{
	COLORREF colFont = RGB(0, 0, 0);
	COLORREF colBack = ::GetSysColor(COLOR_BTNFACE);
	COLORREF colOldFont = pDC->SetTextColor(colFont);
	COLORREF colOldBack = pDC->SetBkColor(colBack);

	RECT rc;
	GetClientRect(&rc);
	rc.bottom = SIZE_TITLEHEIGHT;

	pDC->DrawText(m_strTitle, &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

	pDC->SetBkColor(colOldBack);
	pDC->SetTextColor(colOldFont);
}

void CViewFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CMenu Menu;
	Menu.LoadMenu(IDR_VIEWFRAME);
	CMenu* pPopupMenu = Menu.GetSubMenu(0);
	ASSERT(pPopupMenu);
	
	if (g_Configs.bWireFrame)
		pPopupMenu->CheckMenuItem(ID_VF_WIREFRAME, MF_BYCOMMAND | MF_CHECKED);
	else
		pPopupMenu->CheckMenuItem(ID_VF_WIREFRAME, MF_BYCOMMAND | MF_UNCHECKED);

	POINT pt = {point.x, point.y};
	ClientToScreen(&pt);
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);

	CWnd::OnRButtonDown(nFlags, point);
}

BOOL CViewFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (!(wParam & 0xffff0000))
	{
		//	Is from menu item
		WORD wID = wParam & 0x0000ffff;

		if (wID == ID_VF_WIREFRAME)
		{
			g_Configs.bWireFrame = !g_Configs.bWireFrame;
		}
		else
		{
			int iView = VIEW_PERSPECTIVE;

			switch (wID)
			{
			case ID_VF_VIEWXY:	iView = VIEW_XY;	break;
			case ID_VF_VIEWXZ:	iView = VIEW_XZ;	break;
			case ID_VF_VIEWYZ:	iView = VIEW_YZ;	break;
			case ID_VF_VIEWPERSPECTIVE:	iView = VIEW_PERSPECTIVE;	break;
			}

			ActivateViewport(iView);
		}
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

LRESULT CViewFrame::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	m_RenderWnd.SendMessage(WM_KEYDOWN, wParam, lParam);
	return TRUE;
}

LRESULT CViewFrame::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	m_RenderWnd.SendMessage(WM_KEYUP, wParam, lParam);
	return TRUE;
}

//	Terrain data has been loaded ?
bool CViewFrame::HasTerrainData()
{
	if (m_pMap && m_pMap->GetTerrain()->GetRender())
		return true;
	
	return false;
}

//	Reset camera position
void CViewFrame::OnViewResetcamera()
{
	float y = 10.0f;

	if (HasTerrainData())
		y = m_pMap->GetTerrain()->GetPosHeight(g_vOrigin) + 5.0f;

	m_RenderWnd.ResetCameraPos(0.0f, y, 0.0f);
}

// Set current operation object, message used by operation object first\xqf
void CViewFrame::SetCurrentOperation(COperation *pOperation)
{ 
	m_pOperation =  pOperation;
}

//	View frame has focus ?
bool CViewFrame::HasFocus()
{
	CWnd* pFocusWnd = CWnd::GetFocus();
	if (pFocusWnd == this || IsChild(pFocusWnd))
		return true;

	return false;
}



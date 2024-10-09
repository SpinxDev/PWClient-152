// DlgHeightMap.cpp : implementation file
//

#include "Global.h"
#include <afxpriv.h>
#include "Terrain.h"
#include "ElementMap.h"
#include "DlgHeightMap.h"
#include "DlgNewHeiMap.h"
#include "DlgNewMap.h"
#include "MainFrm.h"
#include "SceneObject.h"
#include "SceneObjectManager.h"
#include "SceneBoxArea.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgHeightMap dialog


CDlgHeightMap::CDlgHeightMap(CTerrain* pTerrain, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHeightMap::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgHeightMap)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pTerrain	= pTerrain;
	m_bModified	= false;
	m_bHMLoaded	= false;
}


void CDlgHeightMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHeightMap)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgHeightMap, CDialog)
	//{{AFX_MSG_MAP(CDlgHeightMap)
	ON_WM_INITMENUPOPUP()
	ON_WM_CREATE()
	ON_COMMAND(ID_HM_HM_LOAD, OnHmHmLoad)
	ON_COMMAND(ID_HM_VIEW_VIEWALL, OnHmViewViewall)
	ON_UPDATE_COMMAND_UI(ID_HM_VIEW_VIEWALL, OnUpdateHmViewViewall)
	ON_COMMAND(ID_HM_EDIT_MAPPROP, OnHmEditMapprop)
	ON_COMMAND(ID_HM_HM_SAVE, OnHmHmSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHeightMap message handlers

int CDlgHeightMap::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT(m_pTerrain);

	//	Create tool bar
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER) ||
		!m_ToolBar.LoadToolBar(IDR_HEIGHTMAP))
	{
		g_Log.Log("CDlgHeightMap::OnCreate, Failed to create toolbar\n");
		return -1;
	}

	//	Create height view window. It's a little strange that when I move
	//	below two lines into OnInitDialog(), m_MapView cannot be created
	RECT rcWnd = {0, 0, 100, 100};
	if (!m_MapView.Create(m_pTerrain, rcWnd, this, 50))
	{
		g_Log.Log("CDlgHeightMap::OnCreate, Failed to height map view window\n");
		return -1;
	}

	return 0;
}

//	Adjust dialog box size
bool CDlgHeightMap::AdjustDialogSize()
{
	//	We need to resize the dialog to make room for control bars.
	//	First, figure out how big the control bars are.
	RECT rcClientStart, rcClientNow;
	GetClientRect(&rcClientStart);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 
				   0, reposQuery, &rcClientNow);
	
	//	Now move all the controls so they are in the same relative
	//	position within the remaining client area as they would be
	//	with no control bars.
	POINT ptOffset;
	ptOffset.x = rcClientNow.left - rcClientStart.left;
	ptOffset.y = rcClientNow.top - rcClientStart.top;

	RECT rcWnd;
	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild)
	{
		pChild->GetWindowRect(&rcWnd);
		ScreenToClient(&rcWnd);
		OffsetRect(&rcWnd, ptOffset.x, ptOffset.y);
		pChild->MoveWindow(&rcWnd, FALSE);

		pChild = pChild->GetNextWindow();
	}

	//	Adjust the dialog window dimensions
	GetWindowRect(&rcWnd);
	rcWnd.right += (rcClientStart.right-rcClientStart.left) - (rcClientNow.right-rcClientNow.left);
	rcWnd.bottom += (rcClientStart.bottom-rcClientStart.top) - (rcClientNow.bottom-rcClientNow.top);
	MoveWindow(&rcWnd, FALSE);
	
	//	And position the control bars
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	return true;
}

BOOL CDlgHeightMap::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//	Adjust dialog box size
	if (!AdjustDialogSize())
	{
		g_Log.Log("CDlgHeightMap::OnCreate, Failed to adjust dialog box size");
		return -1;
	}

	//	Force to update tool bar's initial states. lParam of WM_IDLEUPDATECMDUI
	//	isn't used by MFC, but we use it as force update flag here
	m_ToolBar.SendMessage(WM_IDLEUPDATECMDUI, TRUE, 1);

	//	Resize height view window
	RECT rcWnd;
	GetDlgItem(IDC_STATIC_HEIGHTMAPVIEW)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	m_MapView.Resize(rcWnd.left, rcWnd.top, 512, 512);

	//	Create height map bitmap
	m_MapView.CreateHeightMapBmp();

	//	Set height map view frame's title
	SetMapViewTitle();

	m_bModified	= false;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	Set height map view frame's title
void CDlgHeightMap::SetMapViewTitle()
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();
	if (pHeightMap->pHeightData)
	{
		float fScale = m_pTerrain->GetTerrainSize() / pHeightMap->iWidth;
		CString str;
		str.Format("高度图（%d x %d），1 像素 = %.4f 米", pHeightMap->iWidth, pHeightMap->iHeight, fScale);
		GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(str);
	}
	else
		GetDlgItem(IDC_STATIC_TITLE)->SetWindowText("高度图");
}

/*	OnInitMenuPopup updates the state of items on a popup menu.  
	This code is based on CFrameWnd::OnInitMenuPopup.  We assume the
	application does not support context sensitive help.
*/
void CDlgHeightMap::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	if (bSysMenu)
		return;

	ASSERT(pPopupMenu);

	//	Check the enabled state of various menu items
	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	
	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue;	//	Menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			//	Possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;	//	First item of popup can't be routed to
		    }

			state.DoUpdate(this, FALSE);	//	Popups are never auto disabled
		}
		else
		{
			//	Normal menu item
			//	Auto enable/disable if command is _not_ a system command
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, state.m_nID < 0xF000);
		}
	}
}

//	Import a height map
void CDlgHeightMap::OnHmHmLoad() 
{
	CDlgNewHeiMap MapDlg(true, m_pTerrain->GetTerrainSize());
	if (MapDlg.DoModal() == IDCANCEL)
		return;

	int w = MapDlg.GetWidth();
	int h = MapDlg.GetHeight();
	int iType = MapDlg.GetType();

	if (!m_MapView.LoadHeightMap(w, h, iType, MapDlg.GetHeightMapFile()))
	{
		MessageBox("加载高度图失败");
		return;
	}

	m_bModified = true;
	m_bHMLoaded	= true;
	
	//	Change height map view frame's title
	SetMapViewTitle();

	//对整个地图创建一个默认区域
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager)
		{
			PSCENEBOXAREA pDefaultArea;
			pDefaultArea = pSManager->GetDefaultBoxArea();
			BOXAREA_PROPERTY data;
			Init_BoxArea_Data(data);
			if(pDefaultArea)
			{
				pDefaultArea->GetProperty(data);
			}else
			{
				pDefaultArea = pSManager->CreateBoxArea(AString("BoxArea_default"),true);
				pDefaultArea->SetObjectID(pSManager->GenerateObjectID());
				pDefaultArea->InitNatureObjects();
			}
			data.m_fHeight = (float)(2*(h-1));
			data.m_fLength = (float)(2*(w-1));
			data.m_fWidth = (float)(2*(w-1));
			pDefaultArea->SetProperty(data);
		}
	}
}

void CDlgHeightMap::OnHmHmSave() 
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();
	if (!pHeightMap->pHeightData)
		return;

	char szFilter[] = "Raw Files (*.raw)|*.raw||";
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(FALSE, "*.raw", NULL, dwFlags, szFilter, NULL);
		
	if (FileDia.DoModal() == IDCANCEL)
		return;

	FILE* fp = fopen(FileDia.GetPathName(), "wb+");
	if (!fp)
	{
		MessageBox("创建文件失败!");
		return;
	}

	WORD* pLineBuf = new WORD [pHeightMap->iWidth];
	if (!pLineBuf)
	{
		fclose(fp);
		g_Log.Log("CDlgHeightMap::OnHmHmSave, Not enough memory !");
		return;
	}

	float* pSrc = pHeightMap->pHeightData;

	for (int i=0; i < pHeightMap->iHeight; i++)
	{
		WORD* pDst = pLineBuf;

		for (int j=0; j < pHeightMap->iWidth; j++, pDst++, pSrc++)
			*pDst = (WORD)(*pSrc * 65535);

		fwrite(pLineBuf, 1, pHeightMap->iWidth * sizeof (WORD), fp);
	}

	delete [] pLineBuf;
	fclose(fp);
}

void CDlgHeightMap::OnHmViewViewall() 
{
	if (m_MapView.GetViewWholeFlag())
		m_MapView.ViewWhole(false);
	else
		m_MapView.ViewWhole(true);
}

void CDlgHeightMap::OnUpdateHmViewViewall(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_MapView.GetViewWholeFlag() ? 1 : 0);
}

void CDlgHeightMap::OnHmEditMapprop() 
{
	CDlgNewMap MapDlg;

	CDlgNewMap::DATA DlgData;
	DlgData.bNewMap		= false;
	DlgData.fTileSize	= m_pTerrain->GetTileSize();
	DlgData.fMinHeight	= m_pTerrain->GetYOffset();
	DlgData.fMaxHeight	= m_pTerrain->GetYOffset() + m_pTerrain->GetMaxHeight();
	DlgData.strProjName	= m_pTerrain->GetMap()->GetMapName();

	MapDlg.SetData(DlgData);

	if (MapDlg.DoModal() == IDCANCEL)
		return;

	MapDlg.GetData(DlgData);

	m_pTerrain->SetTileSize(DlgData.fTileSize);
	m_pTerrain->SetYOffset(DlgData.fMinHeight);
	m_pTerrain->SetMaxHeight(DlgData.fMaxHeight - DlgData.fMinHeight);

	//	Change height map view frame's title
	SetMapViewTitle();

	m_bModified = true;
}

void CDlgHeightMap::OnCancel() 
{
	CElementMap* pMap = m_pTerrain->GetMap();
	ASSERT(pMap);

	if (m_bHMLoaded)
	{
		//	Reload terrain render
		SetCursor(LoadCursor(NULL, IDC_WAIT));
		pMap->RecreateTerrainRender(true, false);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	else if (m_bModified)
	{
		ARectF rcArea = m_pTerrain->GetTerrainArea();

		//	Update terrain height
		SetCursor(LoadCursor(NULL, IDC_WAIT));
		pMap->UpdateTerrainHeight(rcArea);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	
	CDialog::OnCancel();
}




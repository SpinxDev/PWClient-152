// ToolTabWnd.cpp : implementation file
//

#include "Global.h"
#include "ToolTabWnd.h"
#include "TerrainModifyLayMaskPanel.h"
#include "TerrainModifyLayMaskSmoothPanel.h"
#include "ObjectPropertyPanel.h"
#include "RollUpPanel.h"
#include "BrushSetPanel.h"
#include "ObjectPropertyPanel.h"
#include "SceneModelListPanel.h"
#include "OptVernierDlg.h"


//#define new A_DEBUG_NEW

//	Window IDs
#define WNDID_TABCONTROL	100
#define WNDID_TERRAINPAGE	101
#define WNDID_OPTIONSPAGE	102
#define WNDID_OBJECTSPAGE   103

/////////////////////////////////////////////////////////////////////////////
// CToolTabWnd

CToolTabWnd::CToolTabWnd()
{
	m_pCurPage	= NULL;
}

CToolTabWnd::~CToolTabWnd()
{
}


BEGIN_MESSAGE_MAP(CToolTabWnd, CWnd)
	//{{AFX_MSG_MAP(CToolTabWnd)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CToolTabWnd message handlers

BOOL CToolTabWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CWnd::CreateEx(0, AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
						LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)), (HBRUSH)COLOR_WINDOW),
						lpszWindowName, 
						dwStyle,
						rect, 
						pParentWnd, 
						nID, 
						pContext))
		return FALSE;

	RECT rcWnd;
	GetClientRect(&rcWnd);

	//	Create tab control
	m_TabCtrl.Create(WS_CHILD | WS_VISIBLE | TCS_FOCUSNEVER, rcWnd, this, WNDID_TABCONTROL);

	//	Change tab control font
    //	m_TabCtrl.SetFont(GetFont());

	//	Add pages lag
	m_TabCtrl.InsertItem(PAGE_TERRAIN, "地形");
	m_TabCtrl.InsertItem(PAGE_OBJECTS, "对象");
	m_TabCtrl.InsertItem(PAGE_OPTIONS, "设置");
	

	//	Create pages...
	m_TerrainPage.Create("TerrainPage", "TerrainPage", WS_CHILD, rcWnd, &m_TabCtrl, WNDID_TERRAINPAGE);
	m_OptionsPage.Create("OptionsPage", "OptionsPage", WS_CHILD, rcWnd, &m_TabCtrl, WNDID_OPTIONSPAGE);
	m_ObjectsPage.Create("ObjectsPage", "ObjectsPage", WS_CHILD, rcWnd, &m_TabCtrl, WNDID_OBJECTSPAGE);
	//	Set default page
	m_TabCtrl.SetCurSel(PAGE_TERRAIN);
	ChangePage();

	return TRUE;
}

void CToolTabWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if(nType = SIZE_MINIMIZED) return;
	
	if (!m_TabCtrl.m_hWnd)
		return;

	RECT rcWnd;
	GetClientRect(&rcWnd);

	//	Move the tab control to the new position and size.
	m_TabCtrl.MoveWindow(&rcWnd, TRUE);

	//	Resize current page
	ResizeCurPage();
}

//	Release resources
void CToolTabWnd::Release()
{
	m_TerrainPage.Release();
	m_OptionsPage.Release();
	m_ObjectsPage.Release();
}

BOOL CToolTabWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR* pnmhdr = (NMHDR*)lParam;

	if (wParam == WNDID_TABCONTROL)	
	{
		//	Is tab control
		if (pnmhdr->code == TCN_SELCHANGE)
			ChangePage();
	}
	
	return CWnd::OnNotify(wParam, lParam, pResult);
}

//	Change page
void CToolTabWnd::ChangePage()
{
	int iSel = m_TabCtrl.GetCurSel();
	CWnd* pWnd = NULL;

	switch (iSel)
	{
	case PAGE_TERRAIN:	pWnd = &m_TerrainPage;	break;
	case PAGE_OPTIONS:	pWnd = &m_OptionsPage;	break;
	case PAGE_OBJECTS:  pWnd = &m_ObjectsPage;  break;
	default:
		return;
	}

	if (m_pCurPage)
	{
		m_pCurPage->SendMessage(WM_ACTIVATETOOLTABPAGE, 0, 0);
		m_pCurPage->ShowWindow(SW_HIDE);
	}

	pWnd->ShowWindow(SW_SHOW);
	pWnd->SendMessage(WM_ACTIVATETOOLTABPAGE, 1, 0);

	m_pCurPage = pWnd;

	ResizeCurPage();
}

//	Resize current page
void CToolTabWnd::ResizeCurPage()
{
	if (!m_pCurPage)
		return;

	//	Adjust the rectangle to fit the tab control into the dialog's client rectangle.
	RECT rcClient;
	m_TabCtrl.GetClientRect(&rcClient);
	m_TabCtrl.AdjustRect(FALSE, &rcClient);

	m_pCurPage->MoveWindow(&rcClient);
	m_pCurPage->ShowWindow(SW_SHOW);
}

//	Initlaize device objects
bool CToolTabWnd::InitDeviceObjects()
{
	m_OptionsPage.InitDeviceObjects();
	return true;
}

//	On terrain layer has been added or deleted
void CToolTabWnd::OnTrnLayerAddedOrDeleted()
{
	CTerrainModifyLayMaskPanel* pPanel1 = (CTerrainModifyLayMaskPanel*)m_TerrainPage.GetPanel(CTerrainPage::PANEL_MODIFYLAYMASK)->GetContentWnd();
	pPanel1->UpdateLayList();
	CTerrainModifyLayMaskSmoothPanel* pPanel2 = (CTerrainModifyLayMaskSmoothPanel*)m_TerrainPage.GetPanel(CTerrainPage::PANEL_MODIFYLAYSMOOTH)->GetContentWnd();
	pPanel2->UpdateLayList();
}

void CToolTabWnd::OnPlayerPassMapCurLayerChange()
{
	CTerrainModifyPanel* pPanel = (CTerrainModifyPanel*)m_TerrainPage.GetPanel(CTerrainPage::PANEL_MODIFY)->GetContentWnd();
	pPanel->OnPlayerPassMapCurLayerChange();
}

void CToolTabWnd::OnBrushOptionPanel()
{
	CBrushSetPanel* pPanel1 = (CBrushSetPanel*)m_OptionsPage.GetPanel(COptionsPage::PANEL_BRUSH)->GetContentWnd();
	pPanel1->UpdateUI();
}

void CToolTabWnd::UpdateBrushCenterHeight( float fHeight)
{
	CTerrainModifyStretchPanel* pPanel1 = (CTerrainModifyStretchPanel*)m_TerrainPage.GetPanel(CTerrainPage::PANEL_MODIFYSTRETCH)->GetContentWnd();
	pPanel1->UpdateCurrentBrushHeight(fHeight);
}

void CToolTabWnd::UpdateBrushHeightRect()
{
	CTerrainModifyStretchPanel* pPanel1 = (CTerrainModifyStretchPanel*)m_TerrainPage.GetPanel(CTerrainPage::PANEL_MODIFYSTRETCH)->GetContentWnd();
	pPanel1->UpdateBrushHeightRect();
}

void CToolTabWnd::ShowPropertyPannel(ADynPropertyObject *pPropertyObject)
{
	CObjectPropertyPanel* pPanel1 = (CObjectPropertyPanel*)m_ObjectsPage.GetPanel(CObjectsPage::PANEL_PROPERTY_OBJECT)->GetContentWnd();
	pPanel1->AttachDataObject(pPropertyObject);
}

void CToolTabWnd::RiseUpAllSceneObjectButton()
{
	CSceneObjectPanel* pPanel1 = (CSceneObjectPanel*)m_ObjectsPage.GetPanel(CObjectsPage::PANEL_SCENE_OBJECT)->GetContentWnd();
	pPanel1->RiseUpAllButton();
   HideObjectProperyPanel();
	
}

void CToolTabWnd::RiseUpAllTerrainOperationButton()
{
	CTerrainModifyPanel* pPanel2 = (CTerrainModifyPanel*)m_TerrainPage.GetPanel(CTerrainPage::PANEL_MODIFY)->GetContentWnd();
	pPanel2->RiseUpAllButton();
	HideTerrainProperyPanel();
}

void CToolTabWnd::SetMaskLay( char *szMaskLay)
{
	CTerrainModifyLayMaskSmoothPanel* pSmooth = (CTerrainModifyLayMaskSmoothPanel*)m_TerrainPage.GetPanel(CTerrainPage::PANEL_MODIFYLAYSMOOTH)->GetContentWnd();
	pSmooth->SetMaskLay(szMaskLay);

	CTerrainModifyLayMaskPanel* pMask = (CTerrainModifyLayMaskPanel*)m_TerrainPage.GetPanel(CTerrainPage::PANEL_MODIFYLAYMASK)->GetContentWnd();
	pMask->SetMaskLay(szMaskLay);
}

void CToolTabWnd::UpdatePropertyData(bool bGet)
{
	CObjectPropertyPanel* pPanel1 = (CObjectPropertyPanel*)m_ObjectsPage.GetPanel(CObjectsPage::PANEL_PROPERTY_OBJECT)->GetContentWnd();
	pPanel1->UpdatePropertyData(bGet);
}

void CToolTabWnd::UpdateSceneObjectList()
{
	CSceneObjectsList* pPanel1 = (CSceneObjectsList*)m_ObjectsPage.GetPanel(CObjectsPage::PANEL_SCENE_LIST_OBJECT)->GetContentWnd();
	pPanel1->FreshObjectList();
}

void CToolTabWnd::UpdateModelList()
{
	CSceneModelListPanel* pPanel1 = (CSceneModelListPanel*)m_ObjectsPage.GetPanel(CObjectsPage::PANEL_MODEL_LIST_OBJECT)->GetContentWnd();
	pPanel1->FreshObjectList();
}

void CToolTabWnd::UpdateSkyBoxPanel()
{
	CSkySetDialog* pPanel1 = (CSkySetDialog*)m_ObjectsPage.GetPanel(CObjectsPage::PANEL_SKY_SET)->GetContentWnd();
	pPanel1->Update();
}

void CToolTabWnd::UpdateVernierDlg()
{
	COptVernierDlg* pPanel1 = (COptVernierDlg*)m_ObjectsPage.GetPanel(CObjectsPage::PANEL_VERNIER_OPT)->GetContentWnd();
	pPanel1->ShowData();
}

void CToolTabWnd::RiseUpVernierButton()
{
	COptVernierDlg* pPanel1 = (COptVernierDlg*)m_ObjectsPage.GetPanel(CObjectsPage::PANEL_VERNIER_OPT)->GetContentWnd();
	pPanel1->RaiseButton();
}

void CToolTabWnd::UpdatePlantsPanel()
{
	CTerrainModifyPlantsPanel* pPanel1 = (CTerrainModifyPlantsPanel*)m_TerrainPage.GetPanel(CTerrainPage::PANEL_MODIFYPLANTS)->GetContentWnd();
	pPanel1->FreshList();
}

void CToolTabWnd::HideObjectProperyPanel(bool bHide)
{

	if(bHide)
	{
		CRollUpPanel *pRoll =  m_ObjectsPage.GetPanel(CObjectsPage::PANEL_SCENE_LIST_OBJECT);
		if(pRoll->IsOpen())
			m_ObjectsPage.ShowPanel(CObjectsPage::PANEL_SCENE_LIST_OBJECT,true);
		else m_ObjectsPage.ShowPanel(CObjectsPage::PANEL_SCENE_LIST_OBJECT,false);
		m_ObjectsPage.HidePanel(CObjectsPage::PANEL_PROPERTY_OBJECT);
		m_ObjectsPage.HidePanel(CObjectsPage::PANEL_CLOUD_SET);
	}else
	{
		CRollUpPanel *pRoll =  m_ObjectsPage.GetPanel(CObjectsPage::PANEL_PROPERTY_OBJECT);
		if(pRoll->IsOpen())
			m_ObjectsPage.ShowPanel(CObjectsPage::PANEL_PROPERTY_OBJECT,true);
		else m_ObjectsPage.ShowPanel(CObjectsPage::PANEL_PROPERTY_OBJECT,false);
	}
	
}

void CToolTabWnd::HideTerrainProperyPanel()
{
	m_TerrainPage.HideAllPanels();

	int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
	bool aOpenFlags[1] = {true};
	m_TerrainPage.ShowPanels(aIndices, aOpenFlags, 1);
}



void CToolTabWnd::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	return;
	CWnd::OnClose();
}

// MainFrm.cpp : implementation of the CMainFrame class
//

//#include "Global.h"
#include "MainFrm.h"
#include "ElementMap.h"
#include "Terrain.h"
#include "MaskModifier.h"
#include "ViewFrame.h"
#include "Render.h"
#include "UndoMan.h"

#include "DlgNewMap.h"
#include "DlgHeightMap.h"
#include "DlgSurfaces.h"
#include "DlgSaveAs.h"
#include "DlgSelectFile.h"
#include "ExpGameDataDlg.h"
#include "DlgExpGameData.h"
#include "DlgLighting.h"
#include "DlgAutoPartHM.h"
#include "OperationManager.h"
#include "UndoLightDeleteAction.h"
#include "SceneObjectManager.h"
#include "BezierUndoAction.h"
#include "ModifiedProfileDlg.h"
#include "TerrainConnectDlg.h"
#include "TerrainBlockViewDlg.h"
#include "SceneBoxArea.h"
#include "BezierAddOperation.h"
#include "SceneObjectRotateOperation.h"
#include "LightMapTransition.h"
#include "ResManDlg.h"
#include "VssOptionDlg.h"
#include "VssOperation.h"
#include "TerrainResExpDlg.h"
#include "AScriptFile.h"
#include "MoveAllObjHeightDlg.h"
#include "Bitmap.h"
#include "DelObjectDlg.h"
#include "WorkSpaceSetDlg.h"
#include "TerrainRender.h"
#include "ScenePrecinctObject.h"
#include "SceneAIGenerator.h"
#include "MousterStatDlg.h"
#include "ModelChangedStatusDlg.h"
#include "ExportLitDlg.h"
#include "DynamicObjectManDlg.h"
#include "NpcControllerManDlg.h"
#include "MousterStatFilterDlg.h"
#include "ElementEditor.h"
#include "SceneGatherObject.h"
#include "ProjList.h"
#include "DlgExportBHT.h"
#include "DlgExportPathInfo.h"
#include "DlgRandomMapInfoSetting.h"
#include "DlgSceneObjectFinder.h"

#include "AC.h"
#include "A3D.h"

#include <vector>
#include <shlwapi.h>


//#define new A_DEBUG_NEW

//	Minimal size of frame window
enum
{
	MIN_WIDTH		= 300,
	MIN_HEIGHT		= 300,
	TOOLBAR_WIDTH	= 284,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZING()
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_TERRAIN_HEIGHTMAP, OnTerrainHeightmap)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_HEIGHTMAP, OnUpdateTerrainHeightmap)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_ENTERIDLE()
	ON_COMMAND(ID_TERRAIN_SURACES, OnTerrainSuraces)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_SURACES, OnUpdateTerrainSuraces)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS, OnUpdateFileSaveas)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_COMMAND(ID_FILE_EXPORTGAMEDATA, OnFileExportgamedata)
	ON_COMMAND(ID_FILE_EXPORT_SERVER_BHT_BY_PROJECT, OnFileExportServerBHTByProject)
	ON_COMMAND(ID_FILE_EXPORT_PATH_INFO, OnFileExportPathInfo)
	ON_COMMAND(ID_FILE_RANDOM_MAP_INFO,OnFileRandomMapInfo)
	ON_COMMAND(ID_TERRAIN_LIGHTING, OnTerrainLighting)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_LIGHTING, OnUpdateTerrainLighting)
	ON_COMMAND(ID_TERRAIN_CREATESURFS, OnTerrainCreatesurfs)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_CREATESURFS, OnUpdateTerrainCreatesurfs)
	ON_COMMAND(ID_BRUSH_CIRCLE, OnBrushCircle)
	ON_COMMAND(ID_BRUSH_RADIUS_1, OnBrushRadius1)
	ON_COMMAND(ID_BRUSH_RADIUS_2, OnBrushRadius2)
	ON_COMMAND(ID_BRUSH_RADIUS_3, OnBrushRadius3)
	ON_COMMAND(ID_BRUSH_RADIUS_ADD, OnBrushRadiusAdd)
	ON_COMMAND(ID_BRUSH_RADIUS_SUB, OnBrushRadiusSub)
	ON_COMMAND(ID_BRUSH_RECT, OnBrushRect)
	ON_COMMAND(ID_OPERATION_REDO, OnOperationRedo)
	ON_COMMAND(ID_OPERATION_UNDO, OnOperationUndo)
	ON_UPDATE_COMMAND_UI(ID_OPERATION_UNDO, OnUpdateOperationUndo)
	ON_UPDATE_COMMAND_UI(ID_OPERATION_REDO, OnUpdateOperationRedo)
	ON_COMMAND(ID_OPERATION_SELECT, OnOperationSelect)
	ON_COMMAND(ID_OPERATION_MOVE, OnOperationMove)
	ON_UPDATE_COMMAND_UI(ID_OPERATION_MOVE, OnUpdateOperationMove)
	ON_COMMAND(ID_OPERATION_ROTATE, OnOperationRotate)
	ON_UPDATE_COMMAND_UI(ID_OPERATION_ROTATE, OnUpdateOperationRotate)
	ON_UPDATE_COMMAND_UI(ID_OPERATION_SELECT, OnUpdateOperationSelect)
	ON_COMMAND(ID_OPERATION_SELECTMULTI, OnOperationSelectmulti)
	ON_UPDATE_COMMAND_UI(ID_OPERATION_SELECTMULTI, OnUpdateOperationSelectmulti)
	ON_COMMAND(ID_OPERATION_SCALE, OnOperationScale)
	ON_UPDATE_COMMAND_UI(ID_OPERATION_SCALE, OnUpdateOperationScale)
	ON_COMMAND(ID_OPERATION_DELETE, OnOperationDelete)
	ON_COMMAND(ID_BEZIER_ADDPOINT, OnBezierAddpoint)
	ON_COMMAND(ID_BEZIER_CUT, OnBezierCut)
	ON_COMMAND(ID_BEZIER_SMOOTH, OnBezierSmooth)
	ON_COMMAND(ID_BEZIER_SUBPOINT, OnBezierSubpoint)
	ON_UPDATE_COMMAND_UI(ID_BEZIER_SUBPOINT, OnUpdateBezierSubpoint)
	ON_UPDATE_COMMAND_UI(ID_BEZIER_CUT, OnUpdateBezierCut)
	ON_UPDATE_COMMAND_UI(ID_BEZIER_ADDPOINT, OnUpdateBezierAddpoint)
	ON_COMMAND(ID_BEZIER_DRAG, OnBezierDrag)
	ON_UPDATE_COMMAND_UI(ID_BEZIER_DRAG, OnUpdateBezierDrag)
	ON_UPDATE_COMMAND_UI(ID_BEZIER_SMOOTH, OnUpdateBezierSmooth)
	ON_COMMAND(ID_BUTTON_NOSPEAK, OnButtonNospeak)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_NOSPEAK, OnUpdateButtonNospeak)
	ON_COMMAND(ID_BUTTON_SHOW_VERNIER, OnButtonShowVernier)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SHOW_VERNIER, OnUpdateButtonShowVernier)
	ON_COMMAND(ID_BEZIER_TEST, OnBezierTest)
	ON_UPDATE_COMMAND_UI(ID_BEZIER_TEST, OnUpdateBezierTest)
	ON_COMMAND(ID_EDIT_OBJECT_COPY, OnEditObjectCopy)
	ON_COMMAND(ID_FILE_AUTOPARTHM, OnFileAutoparthm)
	ON_COMMAND(ID_FILE_SORT_MAP, OnFileSortMap)
	ON_COMMAND(ID_TERRAIN_MODIFIED_PROFILE, OnTerrainModifiedProfile)
	ON_COMMAND(ID_MAP_AUTO_MERGE, OnMapAutoMerge)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_MODIFIED_PROFILE, OnUpdateTerrainModifiedProfile)
	ON_UPDATE_COMMAND_UI(ID_MAP_AUTO_MERGE, OnUpdateMapAutoMerge)
	ON_COMMAND(ID_OPERATION_OBSTRUCT, OnOperationObstruct)
	ON_COMMAND(ID_BEZIER_MERGE, OnBezierMerge)
	ON_UPDATE_COMMAND_UI(ID_BEZIER_MERGE, OnUpdateBezierMerge)
	ON_UPDATE_COMMAND_UI(ID_OPERATION_OBSTRUCT, OnUpdateOperationObstruct)
	ON_COMMAND(ID_EXP_AI_DATA, OnExpAiData)
	ON_COMMAND(ID_IMP_AI_DATA, OnImpAiData)
	ON_UPDATE_COMMAND_UI(ID_IMP_AI_DATA, OnUpdateImpAiData)
	ON_UPDATE_COMMAND_UI(ID_EXP_AI_DATA, OnUpdateExpAiData)
	ON_COMMAND(ID_TERRAIN_BLOCK_VIEW, OnTerrainBlockView)
	ON_COMMAND(ID_DAY_NIGHT, OnDayNight)
	ON_UPDATE_COMMAND_UI(ID_DAY_NIGHT, OnUpdateDayNight)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_BLOCK_VIEW, OnUpdateTerrainBlockView)
	ON_COMMAND(ID_BUTTON_RAIN, OnButtonRain)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_RAIN, OnUpdateButtonRain)
	ON_COMMAND(ID_BUTTON_SNOW, OnButtonSnow)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SNOW, OnUpdateButtonSnow)
	ON_COMMAND(ID_MENU_RES_MAN, OnMenuResMan)
	ON_COMMAND(ID_SET_VSS, OnSetVss)
	ON_COMMAND(ID_GET_VSS_ALL, OnGetVssAll)
	ON_COMMAND(VSS_TEST_MAP_CHECKIN, OnTestMapCheckin)
	ON_COMMAND(ID_EXPORT_GAME_RES, OnExportGameRes)
	ON_UPDATE_COMMAND_UI(VSS_TEST_MAP_CHECKIN, OnUpdateTestMapCheckin)
	ON_UPDATE_COMMAND_UI(ID_GET_VSS_ALL, OnUpdateGetVssAll)
	ON_UPDATE_COMMAND_UI(ID_MENU_RES_MAN, OnUpdateMenuResMan)
	ON_COMMAND(ID_MOVE_ALL_SCENE_OBJECT, OnMoveAllSceneObject)
	ON_COMMAND(ID_GET_ALL_MAP_DATA, OnGetAllMapData)
	ON_COMMAND(ID_EXP_TOPVIEW_MAP, OnExpTopviewMap)
	ON_COMMAND(ID_OPERATION_TEST_BEIZER_INTERSECT, OnOperationTestBeizerIntersect)
	ON_COMMAND(ID_TEST_PRECINCT, OnTestPrecinct)
	ON_COMMAND(ID_TEST_PRECINCT_ERROR, OnTestPrecinctError)
	ON_COMMAND(ID_SHOW_MODEL_PLANE, OnShowModelPlane)
	ON_UPDATE_COMMAND_UI(ID_SHOW_MODEL_PLANE, OnUpdateShowModelPlane)
	ON_COMMAND(ID_MERGE_TREE_TYPE, OnMergeTreeType)
	ON_COMMAND(ID_ROTATE_X, OnRotateX)
	ON_COMMAND(ID_ROTATE_Y, OnRotateY)
	ON_COMMAND(ID_ROTATE_Z, OnRotateZ)
	ON_UPDATE_COMMAND_UI(ID_ROTATE_X, OnUpdateRotateX)
	ON_UPDATE_COMMAND_UI(ID_ROTATE_Y, OnUpdateRotateY)
	ON_UPDATE_COMMAND_UI(ID_ROTATE_Z, OnUpdateRotateZ)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(ID_INDICATOR_COMBO, OnSelchangeCombo)
	ON_COMMAND(ID_SHOW_SEL_ONLY, OnShowSelOnly)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SEL_ONLY, OnUpdateShowSelOnly)
	ON_COMMAND(ID_SET_WORKSPACE, OnSetWorkspace)
	ON_UPDATE_COMMAND_UI(ID_SET_WORKSPACE, OnUpdateSetWorkspace)
	ON_COMMAND(ID_SHOW_TERRAIN_LIGHT, OnShowTerrainLight)
	ON_UPDATE_COMMAND_UI(ID_SHOW_TERRAIN_LIGHT, OnUpdateShowTerrainLight)
	ON_COMMAND(ID_SHOW_DISTANCE, OnShowDistance)
	ON_UPDATE_COMMAND_UI(ID_SHOW_DISTANCE, OnUpdateShowDistance)
	ON_COMMAND(ID_INSERT_MOUNSTER_TO_MAP, OnInsertMounsterToMap)
	ON_COMMAND(ID_GET_CURRENT_PROJ_WORKSPACE_MAPS, OnGetCurrentProjWorkspaceMaps)
	ON_COMMAND(ID_MOUNSTER_STAT, OnMounsterStat)
	ON_COMMAND(ID_MODEL_CHANGED, OnModelChanged)
	ON_COMMAND(ID_EXPORT_SKY_INFOR, OnExportSkyInfor)
	ON_COMMAND(ID_EXP_LIGHT_INFOR_LIST, OnExpLightInforList)
	ON_COMMAND(ID_DYNAMIC_OBJECT_EDITOR, OnDynamicObjectEditor)
	ON_COMMAND(ID_MONSTER_CONTROLLER_EDITOR, OnMonsterControllerEditor)
	ON_COMMAND(ID_RELOAD_ALL_TEXTURES, OnReloadAllTextures)
	ON_COMMAND(ID_EXPORT_STAT_NPCS, OnExportStatNpcs)
	ON_COMMAND(ID_OPERATION_ROTATE2, OnOperationRotate2)
	ON_UPDATE_COMMAND_UI(ID_OPERATION_ROTATE2, OnUpdateOperationRotate2)
	ON_COMMAND(ID_STAT_ALL_TRIANGLE, OnStatAllTriangle)
	ON_COMMAND(ID_STATUS_TRIANGLE_SEL, OnStatusTriangleSel)
	ON_COMMAND(ID_OBJECT_DROP, OnObjectDrop)
	ON_COMMAND(ID_TRANLATE_MAP_TO_PW, OnTranlateMapToPw)
	ON_COMMAND(ID_EXPORT_COORD_DATA, OnExportCoordData)
	ON_COMMAND(ID_SELECT_BY_ID, OnSelectByID)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,  
	ID_INDICATOR_BRUSH_RADIUS,// status line indicator
	ID_INDICATOR_STATIC_X,
	ID_INDICATOR_EDIT_X,
	ID_INDICATOR_STATIC_Y,
	ID_INDICATOR_EDIT_Y,
	ID_INDICATOR_STATIC_Z,
	ID_INDICATOR_EDIT_Z,
	ID_INDICATOR_COMBO,
	ID_INDICATOR_PROGRESS,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bActive		= false;
	m_pMap			= NULL;
	m_dwLastFrame	= 0;
	m_dwTickTime	= 0;
	m_nSceneObjectOperation = 2;
	m_bOtherOperation = false;
	m_bShowMergeDlg = false;
	m_bEnableBezierOperation = false;
	m_bEnableBezierMerge = false;
	m_bEnableBezierSmooth = false;
	m_bEnableBezierTest = false;
}

CMainFrame::~CMainFrame()
{

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a view to occupy the client area of the frame
	if (!m_wndViewframe.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if(!m_wndVssDlgBar.Create(this,IDD_VSS_LOG,CBRS_BOTTOM|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC,IDD_VSS_LOG))
	{
		TRACE0("Failed to create vss bar\n");
		return -1;      // fail to create
	}

	// Create brush bar
	if (!m_wndBrushToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_LEFT  
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndBrushToolBar.LoadToolBar(IDR_BRUSH))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	//	TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndBrushToolBar.EnableDocking(CBRS_ALIGN_ANY);
	CRect toolBarRect;
	RecalcLayout(TRUE);
	m_wndToolBar.GetWindowRect(&toolBarRect);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar,CBRS_ALIGN_ANY,0);
	//toolBarRect.OffsetRect(1,0);
	DockControlBar(&m_wndBrushToolBar,CBRS_ALIGN_ANY,0);
	

	//	Initialize tool tab window
	if (!InitToolTabWnd())
	{
		g_Log.Log("CMainFrame::OnCreate, Failed to create tool tab window");
		return -1;
	}

	g_BezierMergeDlg.Create(IDD_BEZIER_MERGE,this);
	
	m_dwLastFrame = a_GetTime();
	
	m_StatusCombo.Create(&m_wndStatusBar, ID_INDICATOR_COMBO,
			 WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST);
	m_StatusCombo.AddString("不显示相邻地图");
	m_StatusCombo.AddString("显示相邻地图(上)");
	m_StatusCombo.AddString("显示相邻地图(下)");
	m_StatusCombo.AddString("显示相邻地图(左)");
	m_StatusCombo.AddString("显示相邻地图(右)");
	m_StatusCombo.SetCurSel(0);


	m_StatusProgress.Create(&m_wndStatusBar, ID_INDICATOR_PROGRESS, WS_VISIBLE | PBS_SMOOTH);
	
	g_BlockViewDlg.Create(IDD_TERRAIN_BLOCK_VIEW_DLG,this);
	g_BlockViewDlg.ShowWindow(false);
	//把当前的操作设置为选择操作
	SetOtherOperation(false);
	m_DayAndNightDlg.Create(IDD_DIALOG_DAY_NIGHT,&m_wndStatusBar);
	m_DayAndNightDlg.ShowWindow(SW_SHOW);
	return 0;
}

void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	ReleaseMap();	
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}

//	Initialize tool tab window
bool CMainFrame::InitToolTabWnd()
{
	int iScrWid = GetSystemMetrics(SM_CXFULLSCREEN);
	int iScrHei = GetSystemMetrics(SM_CYFULLSCREEN);
	CRect Rect(iScrWid-TOOLBAR_WIDTH, 0, iScrWid, iScrHei);

	if (!m_ToolTabWnd.Create(NULL, "工具栏", WS_POPUP | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, Rect, this, NULL))
	{
		g_Log.Log("CMainForm::InitRenderWnd, Failed to create tool tab window.\n");
		return false;
	}

	//	This trigger WM_SIZE message for m_ToolTabWnd
	m_ToolTabWnd.MoveWindow(&Rect, FALSE);
	//m_ToolTabWnd.ShowWindow(SW_SHOWMINIMIZED);
	m_ToolTabWnd.ShowWindow(SW_SHOW);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndViewframe.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndViewframe.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect) 
{
	//	Prevent window's size to be too small
	if (MIN_WIDTH > pRect->right - pRect->left)
	{
		if (fwSide == WMSZ_BOTTOMLEFT || fwSide == WMSZ_LEFT || fwSide == WMSZ_TOPLEFT)
			pRect->left = pRect->right - MIN_WIDTH;
		else
			pRect->right = pRect->left + MIN_WIDTH;
	}

	if (MIN_HEIGHT > pRect->bottom - pRect->top)
	{
		if (fwSide == WMSZ_TOP || fwSide == WMSZ_TOPLEFT || fwSide == WMSZ_TOPRIGHT)
			pRect->top = pRect->bottom - MIN_HEIGHT;
		else
			pRect->bottom = pRect->top + MIN_HEIGHT;
	}

	CFrameWnd::OnSizing(fwSide, pRect);
}

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	
	m_bActive = bActive ? true : false;
}

//	Create new map
bool CMainFrame::CreateMap()
{
	//	Release old map
	ReleaseMap();

	if (!(m_pMap = new CElementMap))
		return false;

	return true;
}

//	Release map
void CMainFrame::ReleaseMap()
{
	if (!m_pMap)
		return;

	//	Save edit record data to file
	m_pMap->SaveEditRecord();

	//	Reset undo manager
	g_UndoMan.Reset();

	m_pMap->Release();
	delete m_pMap;
	m_pMap = NULL;

	//	Unbind map and view frame
	m_wndViewframe.SetMap(NULL);
}

//	Create a new map
void CMainFrame::OnFileNew() 
{
	if (TellToSave() == IDCANCEL)
		return;

	CDlgNewMap NewMapDlg;

	CDlgNewMap::DATA DlgData;
	DlgData.bNewMap		= true;
	DlgData.fTileSize	= g_Configs.fTileSize;
	DlgData.fMinHeight	= g_Configs.fMinHeight;
	DlgData.fMaxHeight	= g_Configs.fMaxHeight;
	DlgData.strProjName	= "";

	NewMapDlg.SetData(DlgData);

	if (NewMapDlg.DoModal() == IDCANCEL)
		return;

	NewMapDlg.GetData(DlgData);

	g_Configs.fTileSize		= DlgData.fTileSize;
	g_Configs.fMinHeight	= DlgData.fMinHeight;
	g_Configs.fMaxHeight	= DlgData.fMaxHeight;
	
	//	Create new map object
	if (!CreateMap())
		return;

	CTerrain::INITDATA TrnInit;
	TrnInit.fTileSize	= DlgData.fTileSize;
	TrnInit.fMinHeight	= DlgData.fMinHeight;
	TrnInit.fMaxHeight	= DlgData.fMaxHeight;

	//	Initalize empty map
	if (!m_pMap->CreateEmpty(NewMapDlg.m_strProjName, TrnInit))
	{
		g_Log.Log("CMainFrame::OnFileNew, Failed to create empty map");
		return;
	}

	m_pMap->SetModifiedFlag(true);

	//	Bind map with view frame
	m_wndViewframe.SetMap(m_pMap);

	//	Update lay paint list\xqf
	m_ToolTabWnd.OnTrnLayerAddedOrDeleted();
	m_ToolTabWnd.HideObjectProperyPanel();
	m_ToolTabWnd.OnPlayerPassMapCurLayerChange();
	//	Update main frame title
	UpdateMainFrameTitle();
	SetOtherOperation(false);
	m_ToolTabWnd.UpdatePlantsPanel();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	m_ToolTabWnd.RiseUpVernierButton();
	g_bReadOnly =           false;
}

//	Save current map
void CMainFrame::OnFileSave() 
{
	if(g_bReadOnly) return;
	if (!SaveMap())
	{
		MessageBox("文件保存失败!");
		return;
	}
}

//	Save map as...
void CMainFrame::OnFileSaveas() 
{
	CElementMap* pMap = m_wndViewframe.GetMap();
	if (!pMap)
		return;

	CDlgSaveAs SaveDlg(pMap);
	if (SaveDlg.DoModal() == IDCANCEL)
		return;

	//	Get old project name
	CString strOldName = pMap->GetMapName();

	//	Change map name and save data
	m_pMap->ChangeMapName(SaveDlg.GetProjectName());
	OnFileSave();
	CString strOldFile;
	CString strNewFile;
	
	strOldFile.Format("%s%s\\%s\\%sRender.t2bk", g_szWorkDir, g_szEditMapDir, strOldName,strOldName);
	strNewFile.Format("%s%s\\%s\\%sRender.t2bk", g_szWorkDir, g_szEditMapDir, SaveDlg.GetProjectName(),SaveDlg.GetProjectName());
	CopyFile(strOldFile, strNewFile, FALSE);

	strOldFile.Format("%s%s\\%s\\%sRender.t2lm", g_szWorkDir, g_szEditMapDir, strOldName,strOldName);
	strNewFile.Format("%s%s\\%s\\%sRender.t2lm", g_szWorkDir, g_szEditMapDir, SaveDlg.GetProjectName(),SaveDlg.GetProjectName());
	CopyFile(strOldFile, strNewFile, FALSE);

	strOldFile.Format("%s%s\\%s\\%sRender.t2lm1", g_szWorkDir, g_szEditMapDir, strOldName,strOldName);
	strNewFile.Format("%s%s\\%s\\%sRender.t2lm1", g_szWorkDir, g_szEditMapDir, SaveDlg.GetProjectName(),SaveDlg.GetProjectName());
	CopyFile(strOldFile, strNewFile, FALSE);

	strOldFile.Format("%s%s\\%s\\%sRender.t2mk", g_szWorkDir, g_szEditMapDir, strOldName,strOldName);
	strNewFile.Format("%s%s\\%s\\%sRender.t2mk", g_szWorkDir, g_szEditMapDir, SaveDlg.GetProjectName(),SaveDlg.GetProjectName());
	CopyFile(strOldFile, strNewFile, FALSE);

	strOldFile.Format("%s%s\\%s\\%sRender.trn2", g_szWorkDir, g_szEditMapDir, strOldName,strOldName);
	strNewFile.Format("%s%s\\%s\\%sRender.trn2", g_szWorkDir, g_szEditMapDir, SaveDlg.GetProjectName(),SaveDlg.GetProjectName());
	CopyFile(strOldFile, strNewFile, FALSE);

	if (strOldName.CompareNoCase(SaveDlg.GetProjectName()))
	{
		//	Copy old light map file to new directory
		
		strOldFile.Format("%s%s\\%s\\LightMap.bmp", g_szWorkDir, g_szEditMapDir, strOldName);
		if (AUX_FileExist(strOldFile))
		{
			strNewFile.Format("%s%s\\%s\\LightMap.bmp", g_szWorkDir, g_szEditMapDir, SaveDlg.GetProjectName());
			CopyFile(strOldName, strNewFile, FALSE);
		}
	}

	//	Update main frame title
	UpdateMainFrameTitle();
}

//	Save map data to file
bool CMainFrame::SaveMap()
{
	CElementMap* pMap = m_wndViewframe.GetMap();
	if (!pMap){
		return true;
	}
	if (g_bReadOnly){
		return false;
	}
	AString strFile = g_szWorkDir;
	strFile += pMap->GetMapFile();
	if (pMap->Save(strFile)){
		pMap->SetModifiedFlag(false);
		return true;
	}
	return false;
}

//	Open a map
void CMainFrame::OnFileOpen()
{
	SetOtherOperation(false);
	m_ToolTabWnd.UpdatePlantsPanel();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	m_ToolTabWnd.RiseUpVernierButton();
	int n = m_StatusCombo.GetCount();
	if(n>0) m_StatusCombo.SetCurSel(0);
	m_strLastNeighborMap = "";
	//	Let user select a project file
	CString strPath = g_szWorkDir;
	strPath += g_szEditMapDir;

	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= false;
	Params.szFileFilter	= "*.elproj";
	Params.szPrompt		= "请选择一个项目文件";
	Params.szRootDir	= strPath;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;

	if (TellToSave() == IDCANCEL)
		return;

	CString strFile = FileDlg.GetFullPathName();

	//	Create new map object
	if (!CreateMap())
		return;

	SetCursor(LoadCursor(NULL, IDC_WAIT));

	//	Load empty map
	if (!m_pMap->Load(strFile))
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox("加载项目数据失败！");
		//	Reset undo manager
		g_UndoMan.Reset();
		
		m_pMap->Release();
		delete m_pMap;
		m_pMap = NULL;
		
		//	Unbind map and view frame
		m_wndViewframe.SetMap(NULL);
		return;
	}
	g_LogDlg.DoModal();
	SetCursor(LoadCursor(NULL, IDC_ARROW));

	//	Need to receate terrain render
	if (m_pMap->GetTerrain()->GetUpdateRenderFlag())
		MessageBox("地形渲染数据需要更新。");
	
	m_pMap->SetModifiedFlag(false);
	m_pMap->GetTerrain()->GetMaskModifier()->UpdateMaskMapToRender(ARectI(0,0,10000,10000));

	//	Bind map with view frame
	m_wndViewframe.SetMap(m_pMap);

	//	Set global directional light basing on map setting
	const LIGHTINGPARAMS& LtParams = m_pMap->GetLightingParams();
	g_Render.ChangeLight(LtParams.vSunDir, LtParams.dwSunCol, LtParams.dwAmbient);

	//Update lay paint list\xqf
	m_ToolTabWnd.OnTrnLayerAddedOrDeleted();
	m_ToolTabWnd.UpdateBrushHeightRect();
	m_ToolTabWnd.OnPlayerPassMapCurLayerChange();
	
	//	Update main frame title
	UpdateMainFrameTitle();
	
	m_ToolTabWnd.HideObjectProperyPanel();

	// Release last g_PassMapGenerator object
}

//	Close current project
void CMainFrame::OnFileClose() 
{
	SetOtherOperation(false);
	m_ToolTabWnd.UpdatePlantsPanel();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	m_ToolTabWnd.RiseUpVernierButton();

	if (TellToSave() == IDCANCEL)
		return;

	//	Release map object
	ReleaseMap();

	//	Update main frame title
	UpdateMainFrameTitle();

	//  Hide all Panels
	m_ToolTabWnd.HideObjectProperyPanel();
	
	//  Reset scene objects list
	m_ToolTabWnd.UpdateSceneObjectList();
}

//	Edit terrain height map
void CMainFrame::OnTerrainHeightmap() 
{
	if (!m_wndViewframe.GetMap())
		return;
	
	CDlgHeightMap HMDlg(m_wndViewframe.GetMap()->GetTerrain());
	HMDlg.DoModal();

	if (HMDlg.GetModifiedFlag())
	{
		m_pMap->SetModifiedFlag(true);
		GetToolTabWnd()->UpdateBrushHeightRect();
	}
		
}

void CMainFrame::OnTerrainSuraces() 
{
	CElementMap* pMap = m_wndViewframe.GetMap();
	if (!pMap)
		return;

	if (pMap->GetTerrain()->GetMaskModifier()->HasModify())
	{
		//	Reload terrain render
		SetCursor(LoadCursor(NULL, IDC_WAIT));
		m_pMap->RecreateTerrainRender(false, false);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}

	CDlgSurfaces SurfDlg(pMap->GetTerrain());
	SurfDlg.DoModal();

	if (SurfDlg.GetModifiedFlag())
	{
		pMap->SetModifiedFlag(true);

		if (SurfDlg.GetLayerAddDelFlag())
			m_ToolTabWnd.OnTrnLayerAddedOrDeleted();

		pMap->GetTerrain()->GetMaskModifier()->UpdateMaskMapToRender(ARectI(0,0,10000,10000));
	}
}

void CMainFrame::OnTerrainLighting() 
{
	CElementMap* pMap = m_wndViewframe.GetMap();
	if (!pMap)
		return;

	const LIGHTINGPARAMS& Params = pMap->GetLightingParams();
	const LIGHTINGPARAMS& NightParams = pMap->GetNightLightingParams();

	CDlgLighting::INITDATA Init;
	Init.bVecDir	= true;
	Init.dwAmbient	= Params.dwAmbient;
	Init.dwSunCol	= Params.dwSunCol;
	Init.iMapSize	= Params.iMapSize;
	Init.vSunDir	= Params.vSunDir;
	Init.fSunPower  = Params.fSunPower;

	Init.dwNightAmbient	= NightParams.dwAmbient;
	Init.dwNightSunCol	= NightParams.dwSunCol;
	Init.iNightMapSize	= NightParams.iMapSize;
	Init.vNightSunDir	= NightParams.vSunDir;
	Init.fNightSunPower = NightParams.fSunPower;

	CDlgLighting LtDlg(Init);
	if (LtDlg.DoModal() == IDOK)
	{
		const LIGHTINGPARAMS& Result = LtDlg.GetParams();
		const LIGHTINGPARAMS& NightResult = LtDlg.GetNightParams();
		if(Result.iMapSize != NightResult.iMapSize)
		{
			MessageBox("白天夜晚的灯光图的尺寸必须一样！设置无效！");
			return;
		}
		m_pMap->SetLightingParams(Result);
		m_pMap->SetNightLightingParams(NightResult);
		m_pMap->SetModifiedFlag(true);
		if(g_bIsNight) g_Render.ChangeLight(NightResult.vSunDir, NightResult.dwSunCol, NightResult.dwAmbient);
		else g_Render.ChangeLight(Result.vSunDir, Result.dwSunCol, Result.dwAmbient);
	}
}

void CMainFrame::OnTerrainCreatesurfs() 
{
	bool bLighting = MessageBox("更新渲染数据的同时重新生成光照图吗？这可能需要比较长的时间。", "提示", MB_YESNO) == IDYES ? true : false;
	
	//  Create EditorTempMap folder
	char szPath[128];
	sprintf(szPath,"%sEditMaps\\%s\\EditorTempMap",g_szWorkDir,g_Configs.szCurProjectName);
	if(!PathFileExists(szPath))
	{
		if(!CreateDirectory(szPath, NULL))
		{
			MessageBox("创建临时文件夹失败，无法进行地形光照计算和地形更新!");
			return;
		}
	}
	//	Reload terrain render
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	m_pMap->RecreateTerrainRender(false, bLighting);
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE)
	{
		if (!OnAppExit())
			return;
	}

	CFrameWnd::OnSysCommand(nID, lParam);
}

//	Called when application is being closed
//	Return true to really exit application, return false to canel
bool CMainFrame::OnAppExit()
{
	if (TellToSave() == IDCANCEL)
		return false;

	//	Save all edit configs
	AUX_SaveConfigs();

	return true;
}

//	Initialize device objects
bool CMainFrame::InitDeviceObjects()
{
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();

	//	Apply fog parameters
	pA3DDevice->SetFogEnable(g_Configs.bEnableFog);
//	pA3DDevice->SetFogTableEnable(false);
	pA3DDevice->SetFogStart(50.0f);
	pA3DDevice->SetFogEnd(500.0f);
	pA3DDevice->SetFogDensity(0.5f);
	pA3DDevice->SetFogColor(A3DCOLORRGBA(255,255,255,255));
	
	//	Decide maximum texture size
	g_Configs.iMaxTexSize = a_Min(pA3DDevice->GetMaxTextureWidth(), pA3DDevice->GetMaxTextureHeight());

	if (!m_wndViewframe.InitDeviceObjects())
	{
		g_Log.Log("CMainFrame::InitDeviceObjects, Failed to call CViewFrame::InitDeviceObjects");
		return false;
	}

	if (!m_ToolTabWnd.InitDeviceObjects())
	{
		g_Log.Log("CMainFrame::InitDeviceObjects, Failed to call CToolTabWnd::InitDeviceObjects");
		return false;
	}

	return true;
}

//	When a model dialog or menu is active, OnEnterIdle will be called
void CMainFrame::OnEnterIdle(UINT nWhy, CWnd* pWho) 
{
	CFrameWnd::OnEnterIdle(nWhy, pWho);
	
	Tick();	
}

//	Tick routine
bool CMainFrame::Tick()
{
	if (!AppIsActive())
		return true;

	m_dwTickTime = m_dwLastFrame ? a_GetTime() - m_dwLastFrame : 0;

	if (m_dwTickTime)
	{
		m_dwLastFrame = a_GetTime();

		a_ClampRoof(m_dwTickTime, (DWORD)80);

		//	Do frame move
		if (m_pMap)
			m_pMap->FrameMove(m_dwTickTime);

		//	Do render
		m_wndViewframe.FrameMove(m_dwTickTime);
		m_wndViewframe.Render();
	}

	return true;
}

//	Tell user to save map file
int CMainFrame::TellToSave()
{
	if (m_pMap && m_pMap->GetModifiedFlag())
	{
		//	Notify user to save map data
		int iRet = MessageBox("地图文件已被修改，是否保存？", "提示", MB_YESNOCANCEL);
		if (iRet == IDYES)
		{
			//	Save map data
			OnFileSave();
		}

		return iRet;
	}

	return IDNO;
}

//	Export game data
void CMainFrame::OnFileExportgamedata() 
{
	CExpGameDataDlg ExpDlg;
	ExpDlg.DoModal();
}

static CString GetFileDirectory(const CString & strFilePath)
{
	// 从文件全称获取文件目录，不带"\\"或"/"
	int p = strFilePath.ReverseFind(L'\\');
	if (p<0){
		p = strFilePath.ReverseFind(L'/');
	}
	if (p<0){
		return strFilePath;
	}
	return strFilePath.Left(p);
}
static CString s_LastFolder;
static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		// 将目录设置为初始化目录
		SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)s_LastFolder);
	}
	return 0;
}
static bool BrowseChangeFolder(HWND hWnd, CString &strNewFolder)
{
	// 设定上次访问目录	
	if (s_LastFolder.IsEmpty())
	{
		// 当前目录为空，则从程序所在目录出发		
		TCHAR szFilePath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		s_LastFolder = GetFileDirectory(szFilePath);
	}
	
	// 初始化访问结构
	BROWSEINFO bi = {0};
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_VALIDATE;
	bi.lpszTitle = _T("请选择文件夹");
	bi.lpfn = BrowseCallbackProc;
	bi.hwndOwner = hWnd;
	
	// 访问并返回新目录
	ITEMIDLIST *pIDL = SHBrowseForFolder(&bi);
	TCHAR szNewFolder[MAX_PATH] = {0};
	if (SHGetPathFromIDList(pIDL, szNewFolder) == TRUE)
	{
		s_LastFolder = strNewFolder = szNewFolder;
		return true;
	}	
	return false;
}

void CMainFrame::OnFileExportServerBHTByProject() 
{
	//	分块导出各分块地图到单独的 bht 文件，用于随机地形

	//	获取保存文件夹
	CString strNewFolder;
	if (!BrowseChangeFolder(GetSafeHwnd(), strNewFolder)){
		return;
	}

	CDlgExportBHT dlgExport(AUX_GetMainFrame());
	dlgExport.SetExportDir(strNewFolder);
	dlgExport.DoModal();
}

void CMainFrame::OnFileExportPathInfo()
{
	CDlgExportPathInfo dlgExport(AUX_GetMainFrame());
	dlgExport.DoModal();
}
void CMainFrame::OnFileRandomMapInfo()
{
	if (AUX_IsRandomMapWorkSpace())
	{
		CDlgRandomMapInfoSetting dlgRandMap;
		dlgRandMap.DoModal();
	}
	else
		MessageBox("非随机地图工作区无法打开此界面");
}
void CMainFrame::OnFileAutoparthm() 
{
	CDlgAutoPartHM Dlg;
	Dlg.DoModal();
}

//Only set brush command
void CMainFrame::OnBrushCircle()
{
	g_Configs.bCircleBrush = true;
	m_ToolTabWnd.OnBrushOptionPanel();
}

//Only set brush command
void CMainFrame::OnBrushRadius1()
{
	g_Configs.iBrushRadius = 5;
	m_ToolTabWnd.OnBrushOptionPanel();
	UpdateBrushWidth();
}

//Only set brush command
void CMainFrame::OnBrushRadius2()
{
	g_Configs.iBrushRadius = 10;
	m_ToolTabWnd.OnBrushOptionPanel();
	UpdateBrushWidth();
}

//Only set brush command
void CMainFrame::OnBrushRadius3()
{
	g_Configs.iBrushRadius = 15;
	m_ToolTabWnd.OnBrushOptionPanel();
	UpdateBrushWidth();
}

//Only set brush command
void CMainFrame::OnBrushRadiusAdd()
{
	g_Configs.iBrushRadius++;
	if(g_Configs.iBrushRadius>100) g_Configs.iBrushRadius = 100;
	m_ToolTabWnd.OnBrushOptionPanel();
	UpdateBrushWidth();
}

//Only set brush command
void CMainFrame::OnBrushRadiusSub()
{
	g_Configs.iBrushRadius--;
	if(g_Configs.iBrushRadius<1) g_Configs.iBrushRadius = 1;
	m_ToolTabWnd.OnBrushOptionPanel();
	UpdateBrushWidth();
}

//Only set brush command
void CMainFrame::OnBrushRect()
{
	g_Configs.bCircleBrush = false;
	m_ToolTabWnd.OnBrushOptionPanel();
}

void CMainFrame::UpdateBrushWidth()
{
	CString str;
	str.Format("Brush radius = %dm", g_Configs.iBrushRadius);
	m_wndStatusBar.SetPaneText(1, str, TRUE);
}

void CMainFrame::UpdateBrushHeight(float fHeight)
{
	m_ToolTabWnd.UpdateBrushCenterHeight(fHeight);
}

void CMainFrame::OnOperationRedo() 
{
	// TODO: Add your command handler code here
	g_UndoMan.Redo();
}

void CMainFrame::OnOperationUndo() 
{
	// TODO: Add your command handler code here
	g_UndoMan.Undo();
}

void CMainFrame::OnUpdateOperationUndo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(g_UndoMan.GetUndoListActionNum()<=0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CMainFrame::OnUpdateOperationRedo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(g_UndoMan.GetRedoListActionNum()<=0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

//	Update main frame title
void CMainFrame::UpdateMainFrameTitle()
{
	CString str;
	if (!m_pMap)
	{
		str.Format("Element Editor-[%s]",g_szProject);
		SetWindowText(str);
	}
	else
	{
		
		str.Format("Element Editor-[%s] - %s%s\\%s",g_szProject, g_szWorkDir, g_szEditMapDir, m_pMap->GetMapName());
		SetWindowText(str);
	}
}


void CMainFrame::OnOperationSelect() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();

	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_SELECT);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 1;
	
	
}

void CMainFrame::OnOperationMove() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_MOVE);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 2;
	
}

void CMainFrame::OnUpdateOperationMove(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_nSceneObjectOperation==2) 
	{
		if(!m_bOtherOperation)
			pCmdUI->SetCheck(true);
		else pCmdUI->SetCheck(false);
	}
	else pCmdUI->SetCheck(false);
}

void CMainFrame::OnOperationRotate() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_ROTATE);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 3;
	
}

void CMainFrame::OnUpdateOperationRotate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_nSceneObjectOperation==3) 
	{
		if(!m_bOtherOperation)
			pCmdUI->SetCheck(true);
		else pCmdUI->SetCheck(false);
	}
	else pCmdUI->SetCheck(false);
}

void CMainFrame::OnOperationRotate2() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_ROTATE2);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 12;	
}

void CMainFrame::OnStatAllTriangle() 
{
	// TODO: Add your command handler code here
	if(m_pMap) 
	{
		UINT nTriangleNum = 0;
		CSceneObjectManager *pMan = m_pMap->GetSceneObjectMan();
		ALISTPOSITION pos = pMan->m_listSceneObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pMan->m_listSceneObject.GetPrev(pos);
			nTriangleNum += ptemp->GetTriangleNum();
		}

		CTerrain *pTerrain = m_pMap->GetTerrain();
		int nGrid =  (int)(pTerrain->GetTerrainSize()/pTerrain->GetTileSize());
		UINT nTerrain = nGrid*nGrid*4;
		CString strMsg;
		strMsg.Format("地型三角形总数: %d\n场景对象三角形总数: %d\n地图三角形总数:%d",nTerrain,nTriangleNum,nTerrain + nTriangleNum);
		MessageBox(strMsg);
	}
}

void CMainFrame::OnUpdateOperationRotate2(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_nSceneObjectOperation==12) 
	{
		if(!m_bOtherOperation)
			pCmdUI->SetCheck(true);
		else pCmdUI->SetCheck(false);
	}
	else pCmdUI->SetCheck(false);
}

void CMainFrame::OnUpdateOperationSelect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_nSceneObjectOperation==1) 
	{
		if(!m_bOtherOperation)
			pCmdUI->SetCheck(true);
		else pCmdUI->SetCheck(false);
	}
	else pCmdUI->SetCheck(false);
}

void CMainFrame::OnOperationSelectmulti() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_MULTISELECT);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 4;
	
}

void CMainFrame::OnUpdateOperationSelectmulti(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_nSceneObjectOperation==4) 
	{
		if(!m_bOtherOperation)
			pCmdUI->SetCheck(true);
		else pCmdUI->SetCheck(false);
	}
	else pCmdUI->SetCheck(false);
}

void CMainFrame::SetOtherOperation(bool bUseOther)
{ 
	m_bOtherOperation = bUseOther; 
	if(m_bOtherOperation)
	{
		if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
		if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
		if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	}
	COperation *pOperation = NULL;
	if(m_nSceneObjectOperation == 1)
	{//selected
		pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_SELECT);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else 
	if(m_nSceneObjectOperation == 2)
	{//MOVE
		pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_MOVE);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else
	if(m_nSceneObjectOperation == 3)
	{//ROTATE
		pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_ROTATE);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else
	if(m_nSceneObjectOperation == 4)
	{//Multi selected
		pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_MULTISELECT);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else
	if(m_nSceneObjectOperation == 5)
	{//Scale
		pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_SCALE);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else
	if(m_nSceneObjectOperation == 6)
	{// bezier delete a point
		pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_DELETE_POINT);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else
	if(m_nSceneObjectOperation == 7)
	{// bezier cut segment
		pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_CUT_SEGMENT);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else
	if(m_nSceneObjectOperation == 8)
	{// bezier add point
		pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_ADD_POINT);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else
	if(m_nSceneObjectOperation == 9)
	{//drag bezier
		pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_DRAG);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else
	if(m_nSceneObjectOperation == 10)
	{//Test bezier
		pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_TEST);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else
	if(m_nSceneObjectOperation == 11)
	{// Bezier merge
		pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_MERGE);
		m_wndViewframe.SetCurrentOperation(pOperation);
	}else
	{
		m_wndViewframe.SetCurrentOperation(0);
	}
};

void CMainFrame::EnableBezierOperation(bool bEnable)
{
	m_bEnableBezierOperation = bEnable;
}

/*
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==WM_KEYDOWN)
	{
		if(lParam == VK_DELETE)
		{
			if(IDOK==MessageBox("你要删除当前选择的对象吗？",NULL,MB_OKCANCEL))
			{

			}
		}
	}
	return CFrameWnd::WindowProc(message, wParam, lParam);
}
*/


void CMainFrame::OnOperationScale() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_SCALE);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 5;
}

void CMainFrame::OnUpdateOperationScale(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_nSceneObjectOperation==5) 
	{
		if(!m_bOtherOperation)
			pCmdUI->SetCheck(true);
		else pCmdUI->SetCheck(false);
	}
	else pCmdUI->SetCheck(false);
}

void CMainFrame::OnOperationDelete() 
{
	// TODO: Add your command handler code here
    if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount()>0)
		{
			CDelObjectDlg dlg;
			if(IDOK==dlg.DoModal())
			{
				CUndoSceneObjectDeleteAction *pUndo = new CUndoSceneObjectDeleteAction(m_pMap->GetSceneObjectMan());
				if(pUndo->GetSeletedListData())
				{
					g_UndoMan.AddUndoAction(pUndo);
					m_ToolTabWnd.UpdateSceneObjectList();
					m_ToolTabWnd.HideObjectProperyPanel();
				}else 
				{
					pUndo->Release();
					delete pUndo;
				}
			}
		}
	}
	
}


void CMainFrame::OnBezierAddpoint() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_ADD_POINT);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 8;
}

void CMainFrame::OnBezierCut() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_CUT_SEGMENT);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 7;
}

void CMainFrame::OnBezierSmooth() 
{
	// TODO: Add your command handler code here
	if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount() == 1)
		{
			CSceneObject * pObj = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			if(pObj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
			{
				if(!((CEditerBezier*)pObj)->IsBezier()) return;
				//记录Undo
				CBezierUndoAction *pUndo = new CBezierUndoAction();
				pUndo->StartRecord();
				g_UndoMan.AddUndoAction(pUndo);
				((CEditerBezier *)pObj)->SmoothBezier();
			}
		}
	}
		
}

void CMainFrame::OnBezierSubpoint() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_DELETE_POINT);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 6;
}

void CMainFrame::OnUpdateBezierSubpoint(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
		if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount() == 1)
		{
			CSceneObject * pObj = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			if(pObj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
			{
				pCmdUI->Enable(true);
				
				if(m_nSceneObjectOperation==6) 
				{
					if(!m_bOtherOperation)
						pCmdUI->SetCheck(true);
					else pCmdUI->SetCheck(false);
				}
				else pCmdUI->SetCheck(false);

				return;
			}
		}
	}
	pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateBezierCut(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	/*
	if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount() == 1)
		{
			CSceneObject * pObj = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			if(pObj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
			{
				pCmdUI->Enable(true);
				
				if(m_nSceneObjectOperation==7) 
				{
					if(!m_bOtherOperation)
						pCmdUI->SetCheck(true);
					else pCmdUI->SetCheck(false);
				}
				else pCmdUI->SetCheck(false);

				return;
			}
		}
	}*/
	pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateBezierAddpoint(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount() == 1)
		{
			CSceneObject * pObj = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			if(pObj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
			{
				pCmdUI->Enable(true);
				
				if(m_nSceneObjectOperation==8) 
				{
					if(!m_bOtherOperation)
						pCmdUI->SetCheck(true);
					else pCmdUI->SetCheck(false);
				}
				else pCmdUI->SetCheck(false);

				return;
			}
		}
	}
	pCmdUI->Enable(false);
}

void CMainFrame::OnBezierDrag() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_DRAG);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 9;
	
}

void CMainFrame::OnUpdateBezierDrag(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code her
	if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount() == 1)
		{
			CSceneObject * pObj = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			if(pObj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
			{
				pCmdUI->Enable(true);
				
				if(m_nSceneObjectOperation==9) 
				{
					if(!m_bOtherOperation)
						pCmdUI->SetCheck(true);
					else pCmdUI->SetCheck(false);
				}
				else pCmdUI->SetCheck(false);

				return;
			}
		}
	}
	pCmdUI->Enable(false);
}

void CMainFrame::OnBezierTest() 
{
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_TEST);
	m_wndViewframe.SetCurrentOperation(pOperation);
	if(m_nSceneObjectOperation != 10)
	{
		m_nSceneObjectOperation = 10;
		((CBezierTestOperation*)pOperation)->Play();
	}else 
	{
		m_nSceneObjectOperation = 2;
		((CBezierTestOperation*)pOperation)->Stop();
		SetOtherOperation(false);
	}
	
}

void CMainFrame::OnUpdateBezierTest(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount() == 1)
		{
			CSceneObject * pObj = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			if(pObj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
			{
				if(((CEditerBezier*)pObj)->IsBezier())
				{
					pCmdUI->Enable(true);
					
					if(m_nSceneObjectOperation==10) 
					{
						if(!m_bOtherOperation)
							pCmdUI->SetCheck(true);
						else pCmdUI->SetCheck(false);
					}
					else pCmdUI->SetCheck(false);
					
					return;
				}
			}
		}
	}
	pCmdUI->Enable(false);
}


void CMainFrame::OnUpdateBezierSmooth(CCmdUI* pCmdUI) 
{
	if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount() == 1)
		{
			CSceneObject * pObj = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			if(pObj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
			{
				if(((CEditerBezier*)pObj)->IsBezier())
				{
					pCmdUI->Enable(true);
					return;
				}
			}
		}
	}
	pCmdUI->Enable(false);
}

void CMainFrame::OnButtonNospeak() 
{
	// TODO: Add your command handler code here
	if(m_pMap)
	{
		g_Configs.bEnableSpeak = !g_Configs.bEnableSpeak;
		if(!g_Configs.bEnableSpeak)
		{
			g_BackMusic.StopMusic(true,true);
			g_BackMusic.StopBackSFX();
			CScenePrecinctObject *pObj = m_pMap->GetSceneObjectMan()->GetCurrentPrecinct();
			if(pObj) pObj->StopBackMusic();
		}
	}
}

void CMainFrame::OnUpdateButtonNospeak(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(g_Configs.bEnableSpeak)
		pCmdUI->SetCheck(0);
	else pCmdUI->SetCheck(1);
}

void CMainFrame::OnButtonShowVernier() 
{
	// TODO: Add your command handler code here
	g_Configs.bShowVernier = !g_Configs.bShowVernier;
}

void CMainFrame::OnUpdateButtonShowVernier(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_Configs.bShowVernier);
}


void CMainFrame::OnEditObjectCopy() 
{
	// TODO: Add your command handler code here
	if(m_pMap)
	{
		CSceneObjectManager *pMan = m_pMap->GetSceneObjectMan();
		if(IDYES==MessageBox("你确信要复制选择的所有对象吗？","",MB_YESNO))
		{
			
			std::vector<CSceneObject *> selObject;
			ALISTPOSITION pos = pMan->m_listSelectedObject.GetTailPosition();
			while(pos)
			{
				CSceneObject* ptemp = pMan->m_listSelectedObject.GetPrev(pos);
				CSceneObject* pNew = ptemp->CopyObject();
				if(pNew) 
				{
					pNew->SetSelected(false);
					selObject.push_back(pNew);
				}
			}
			
			pMan->ClearSelectedList();
			int n = selObject.size();
			for( int i = 0; i < n ; i++)
			{
				CSceneObject *ptemp = selObject.at(i);
				pMan->AddObjectPtrToSelected(ptemp);
			}
			selObject.clear();
		}
	}
}

void CMainFrame::OnFileSortMap() 
{
	// TODO: Add your command handler code here
	CDlgExpGameData ExpDlg;
	ExpDlg.DoModal();
}

void CMainFrame::OnTerrainModifiedProfile() 
{
	// TODO: Add your command handler code here
	if(g_bReadOnly) return;
	if(m_pMap)
	{
		CModifiedProfileDlg dlg;
		dlg.SetTerrain(m_pMap->GetTerrain());
		dlg.DoModal();	
	}
}

void CMainFrame::OnMapAutoMerge() 
{
	// TODO: Add your command handler code here
	if(g_bReadOnly) return;
	CTerrainConnectDlg dlg;
	dlg.DoModal();	
}

void CMainFrame::OnUpdateTerrainModifiedProfile(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_pMap)
		pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateMapAutoMerge(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_pMap)
		pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
}

void CMainFrame::OnOperationObstruct() 
{
	// TODO: Add your command handler code here
	if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount() == 1)
		{
			CSceneObject * pObj = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			if(pObj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
			{
				if(((CEditerBezier*)pObj)->IsBezier()) 
				{
					((CEditerBezier*)pObj)->ConvertToObstruct();
				}
			}
		}
	}
}

void CMainFrame::OnBezierMerge() 
{
	// TODO: Add your command handler code here
	//m_bShowMergeDlg = !m_bShowMergeDlg;
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	g_BezierMergeDlg.UpdateUI();
	g_BezierMergeDlg.ShowWindow(true);
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_BEZIER_MERGE);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 11;
}

void CMainFrame::OnUpdateBezierMerge(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount() == 2)
		{
			CSceneObject * pObj1 = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			CSceneObject * pObj2 = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetHead();
			if(pObj1->GetObjectType() == CSceneObject::SO_TYPE_BEZIER && pObj2->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
			{
				if(((CEditerBezier*)pObj1)->IsBezier() == ((CEditerBezier*)pObj2)->IsBezier()) 
				{
					pCmdUI->Enable(true);
					if(m_nSceneObjectOperation==11) 
					{
						if(!m_bOtherOperation)
							pCmdUI->SetCheck(true);
						else pCmdUI->SetCheck(false);
					}
					return;
				}
			}
		}
	}
	pCmdUI->Enable(false);
	pCmdUI->SetCheck(false);
}



void CMainFrame::OnUpdateOperationObstruct(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bEnableBezierOperation);
	if(!m_bEnableBezierOperation) return;
}

void CMainFrame::OnExpAiData() 
{
	if(!m_pMap) return;
	
	char szFilters[]= "AI Generator (*.TAI)|*.TAI|";
 
	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg (false, "TAI", "*.TAI",
      OFN_HIDEREADONLY|OFN_NOCHANGEDIR, szFilters, this);
   
   // Display the file dialog. When user clicks OK, fileDlg.DoModal() 
   // returns IDOK.
   if( fileDlg.DoModal ()==IDOK )
   {
	   CString pathName = fileDlg.GetPathName();
	   AFile File;
	   if (!File.Open(pathName, AFILE_CREATENEW))
	   {
		   g_Log.Log("CMainFrame::OnExpAiData, Failed to open file %s.", pathName);
		   return;
	   }
	   m_pMap->GetSceneObjectMan()->ExpAiData(&File);
	   File.Close();
	   
   }
}

void CMainFrame::OnImpAiData() 
{
	if(!m_pMap) return;
	
	// TODO: Add your command handler code here
	char szFilters[]= "AI Generator (*.TAI)|*.TAI|";
 
	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg (true, "TAI", "*.TAI",
      OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_NOCHANGEDIR, szFilters, this);
   
   // Display the file dialog. When user clicks OK, fileDlg.DoModal() 
   // returns IDOK.
   if( fileDlg.DoModal ()==IDOK )
   {
	   CString pathName = fileDlg.GetPathName();
	   AFile File;
	   if (!File.Open(pathName, AFILE_OPENEXIST))
	   {
		   g_Log.Log("CMainFrame::OnImpAiData, Failed to open file %s.", pathName);
		   return;
	   }
	   m_pMap->GetSceneObjectMan()->ImpAiData(&File);
	   File.Close();
	   
   }
}

void CMainFrame::OnUpdateImpAiData(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_pMap)
		pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateExpAiData(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_pMap)
		pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
}

void CMainFrame::OnTerrainBlockView() 
{
	// TODO: Add your command handler code here
	if(m_pMap)
	{
		g_BlockViewDlg.ShowWindow(true);
		if(m_pMap->GetTerrain()->GetMaskModifier()->HasModify())
			m_pMap->RecreateTerrainRender(false,false);
		g_BlockViewDlg.UpdateDlg();
	}
}

void CMainFrame::OnDayNight() 
{
	// TODO: Add your command handler code here
	g_bIsNight = !g_bIsNight;
	if(m_pMap)
	{
		CSceneBoxArea *pCurArea = m_pMap->GetSceneObjectMan()->GetCurrentArea();
		if(pCurArea) pCurArea->OnCamEntry();
	}
}

void CMainFrame::OnUpdateDayNight(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_bIsNight);
}

void CMainFrame::OnUpdateTerrainBlockView(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_pMap)
		pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
	
}

void CMainFrame::OnButtonRain() 
{
	// TODO: Add your command handler code here
	g_Configs.bShowRain = !g_Configs.bShowRain;
}

void CMainFrame::OnUpdateButtonRain(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_Configs.bShowRain);
}

void CMainFrame::OnButtonSnow() 
{
	// TODO: Add your command handler code here
	g_Configs.bShowSnow = !g_Configs.bShowSnow;
}

void CMainFrame::OnUpdateButtonSnow(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_Configs.bShowSnow);
}


void CMainFrame::OnMenuResMan() 
{
	// TODO: Add your command handler code here
	CResManDlg dlg;
	dlg.DoModal();
	
}

// static void		_processAllMaps()
// {
// 	/////////测试记录文件/////////
// 	AFile fileLog;
// 	if(!fileLog.Open("E:\\MapsLog.txt",AFILE_TEXT|AFILE_CREATENEW))
// 	{
// 		AfxMessageBox("Error to create fileLog!");
// 		return;
// 	}
// 	
// 	AfxGetApp()->BeginWaitCursor();
// 	//取所有地图项目
// 	CString str,str1;
// 	std::vector<CString> listWorkspace;
// 	AScriptFile File;
// 	if (File.Open("EditMaps\\workspace.ws"))
// 	{
// 		while (File.GetNextToken(true))
// 		{
// 			listWorkspace.push_back(CString(File.m_szToken));
// 		}
// 		File.Close();
// 	}
// 	else
// 	{
// 		AfxMessageBox("Error!");
// 		fileLog.Close();
// 		return;
// 	}
// 
// 	//listWorkspace.push_back(CString("a01"));
// 
// 	//转换项目
// 	for(int i = 0; i < (int)listWorkspace.size(); i++)
// 	{
// 		str.Format(".....开始处理地图工程 (%d): < %s > .....!",i,listWorkspace[i]);
// 		fileLog.WriteLine(str);
// 		
// 		sprintf(g_szEditMapDir, "EditMaps\\%s",listWorkspace[i]);
// 		str.Format("%sEditMaps\\%s",g_szWorkDir,listWorkspace[i]);
// 			
// 		//加载项目文件
// 		DWORD dwCol;
// 		char szProj[256];
// 		sprintf(szProj,"EditMaps\\%s\\ProjList.dat",listWorkspace[i]);
// 		if (!File.Open(szProj))
// 		{
// 			str.Format("XXX处理地图工程: < %s > 失败XXX!",listWorkspace[i]);
// 			fileLog.WriteLine(str);
// 			AfxMessageBox(str);
// 			continue;	//继续
// 		}
// 		
// 		if(File.GetNextToken(true))
// 		{
// 			CString strCol(File.m_szToken);
// 			if(strCol.Find("col=")!=-1)
// 			{
// 				strCol.Replace("col=","");
// 				dwCol = (int)atof(strCol);
// 			}
// 			else
// 			{
// 				File.ResetScriptFile();
// 				dwCol = 1;
// 			}
// 		}
// 		
// 		while (File.GetNextToken(true))
// 		{
// 			//开始处理工程中地图
// 			
// 			//加载地图
// 			CElementMap elementMap;
// 			str.Format("%sEditMaps\\%s\\%s\\%s.elproj",g_szWorkDir,listWorkspace[i],File.m_szToken,File.m_szToken);
// 			if(!elementMap.Load(str))
// 			{
// 				str.Format("XXX加载地图工程: < %s > 中地图: < %s > 失败XXX!",listWorkspace[i],File.m_szToken);
// 				fileLog.WriteLine(str);
// 				AfxMessageBox(str);
// 				elementMap.Release();
// 				continue;	//继续
// 			}
// 		
// 			//保存地图
// 			if(!elementMap.Save(str))
// 			{
// 				str.Format("XXX保存地图工程: < %s > 中地图: < %s > 失败XXX!",listWorkspace[i],File.m_szToken);
// 				fileLog.WriteLine(str);
// 				AfxMessageBox(str);
// 				elementMap.Release();
// 				continue;	//继续
// 			}
// 
// 
// 			//Checkout .scene文件
// // 			str1.Format("%s\\%s",g_szEditMapDir,File.m_szToken);
// // 			str.Format("%s\\%s",g_szWorkDir,str1);
// // 			g_VSS.SetWorkFolder(str);
// // 
// // 			str.Format("%s\\%s.scene",str1,File.m_szToken);
// // 			g_VSS.CheckOutFile(str);
// // 			if(0==g_VSS.IsCheckOutStatus(str)) 
// // 			{
// // 				str.Format("Checkout file: %s 失败!",str);
// // 				fileLog.WriteLine(str);
// // 				AfxMessageBox(str);
// // 			}
// 
// 			//Checkin .scene文件
// 			str1.Format("%s\\%s",g_szEditMapDir,File.m_szToken);
// 			g_VSS.SetProjectPath(str1);
// 					
// 			str.Format("%s.scene",File.m_szToken);
// 			g_VSS.CheckInFile(str);
// 			str = str1 + "\\" + str;
// 			if(1==g_VSS.IsCheckOutStatus(str)) 
// 			{
// 				str.Format("Checkin file: %s 失败!",str);
// 				fileLog.WriteLine(str);
// 				AfxMessageBox(str);
// 			}
// 
// 	
// 			//Checkin地图
// // 			if(!AUX_CheckInMap(File.m_szToken))
// // 			{
// // 				str.Format("XXXCheckin地图工程: < %s > 中地图: < %s > 失败XXX!",listWorkspace[i],File.m_szToken);
// // 				fileLog.WriteLine(str);
// // 				AfxMessageBox(str);
// // 				elementMap.Release();
// // 				continue;	//继续
// // 			}
// // 
// // 			//保存Render文件到服务器
// // 			CString src,dst;
// // 			src.Format("%s%s\\%s",g_szWorkDir,g_szEditMapDir,File.m_szToken);
// // 			dst.Format("%s\\MapsRenderData\\%s\\%s",g_Configs.szServerPath,listWorkspace[i],File.m_szToken);
// // 		
// // 			///////////////项目文件夹///////////////
// // 			CString strFolder;
// // 			strFolder.Format("%s\\MapsRenderData\\%s",g_Configs.szServerPath,listWorkspace[i],File.m_szToken);
// // 			if(!PathIsDirectory(strFolder))
// // 			{
// // 				if(!CreateDirectory(strFolder,NULL))
// // 				{
// // 					str.Format("创建文件夹: %s 失败!",strFolder);
// // 					fileLog.WriteLine(str);
// // 					AfxMessageBox(str);
// // 					elementMap.Release();
// // 					continue;	//继续
// // 				}
// // 			}
// // 			///////////////文件夹///////////////
// // 
// // 			if(!AUX_CopyRenderFiles(dst,src)) 
// // 			{
// // 				str.Format("XXX保存地图工程: < %s > 中地图: < %s > Render数据到服务器失败XXX!",listWorkspace[i],File.m_szToken);
// // 				fileLog.WriteLine(str);
// // 				AfxMessageBox(str);
// // 				elementMap.Release();
// // 				continue;	//继续
// // 			}
// 			
// 			elementMap.Release();
// 
// 			str.Format("处理地图工程: < %s > 中地图: < %s >  成功!",listWorkspace[i],File.m_szToken);
//  			fileLog.WriteLine(str);
// 		}
// 		File.Close();
// 
// 		//换一行
// 		fileLog.WriteLine("\n");
// 	}
// 	AfxGetApp()->EndWaitCursor();
// 
// 	fileLog.Close();
// 	AfxMessageBox("地图转换完成!");
// }	

void CMainFrame::OnSetVss() 
{
	// TODO: Add your command handler code here
	//_processAllMaps();
	//	return;

	CVssOptionDlg dlg;
	if(IDOK==dlg.DoModal())
	{
		((CElementEditorApp*)AfxGetApp())->InitVss();
	}
}

void CMainFrame::OnGetVssAll() 
{
	BeginWaitCursor();
	// TODO: Add your command handler code here
	CString str = CString(g_szWorkDir);
	g_VSS.SetWorkFolder(str);
	
	g_VSS.SetProjectPath("Building");
	g_VSS.GetAllFiles("Building");
	
	g_VSS.SetProjectPath("ResTable");
	g_VSS.GetAllFiles("ResTable");
	
	g_VSS.SetProjectPath("");
	MessageBox("模型更新完成!");
	EndWaitCursor();
}

void CMainFrame::OnTestMapCheckin() 
{
	// TODO: Add your command handler code here
	if(m_pMap && !g_bReadOnly)
	{
		if(IDNO==MessageBox("你真的要把当前地图所有改动保存到服务器?","注意",MB_YESNO))
			return;
		if (!SaveMap()){
			MessageBox("保存失败、无法Checkin! 详细情况请看ElementEditor.log。");
			return;
		}
		CString strMap = m_pMap->GetMapName();
		
		if(!AUX_ModifiedResTable())
			return;
		
		if(!AUX_IncreaseVersion(m_pMap->GetMapName()))
		{
			MessageBox("不能更新跟踪数据!请检查地图是否已经Checkout!");
			return;
		}

		CString dst,src;
		if(!AUX_CheckInMap(strMap.GetBuffer(0)))
		{
			//new a vss project
			if(IDYES == MessageBox("如果这是一个全新地图，你需要把它加入服务器请选是，否则选否？",NULL,MB_YESNO))
			{
				CString strTemp,str,str1;
				strTemp.Format("%s",g_szEditMapDir);
				g_VSS.SetProjectPath(strTemp);
				strTemp.Format("%s\\%s",g_szEditMapDir,m_pMap->GetMapName());
				g_VSS.CreateProject(m_pMap->GetMapName());
				g_VSS.SetProjectPath(strTemp);
	
				int n = AUX_GetMapExStringSize();
				for( int i = 0; i < n; ++i)
				{
					str1.Format("%s\\%s",g_szEditMapDir,m_pMap->GetMapName());
					str = AUX_GetMapExString(i, m_pMap->GetMapName());
					str = str1 + "\\" + str;

					if(AUX_FileIsExist(str))
						g_VSS.AddFile(str);
				}
			}
			else
				return;
		}
		src.Format("%s%s\\%s",g_szWorkDir,g_szEditMapDir,strMap);
		dst.Format("%s\\MapsRenderData\\%s\\%s",g_Configs.szServerPath,g_Configs.szCurProjectName,strMap);
		if(!AUX_CopyRenderFiles(dst,src)) 
		{
			AUX_CheckOutMap(strMap.GetBuffer(0));
			return;
		}
		strMap.ReleaseBuffer();
		//注意：这儿把地图下多出的文件加入到VSS，比如MASK BITMAP,但是如果是减少就没发了哟。
		//这儿以后可以改动，这样就需要一个文件列表什么的保存到服务器。就总体来看，这些数据
		//只是为了编辑，而导出后数据不会有多余，因此也可以不改动。
		OnFileClose();
	}
}

void CMainFrame::OnExportGameRes() 
{
	// TODO: Add your command handler code here
	CTerrainResExpDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnUpdateTestMapCheckin(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_VSS.IsInited());
}

void CMainFrame::OnUpdateGetVssAll(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_VSS.IsInited());
}

void CMainFrame::OnUpdateMenuResMan(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_VSS.IsInited());
}

void CMainFrame::OnMoveAllSceneObject() 
{
	// TODO: Add your command handler code here
	CMoveAllObjHeightDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnGetAllMapData() 
{
	BeginWaitCursor();
	CString str;
	str.Format("%sEditMaps",g_szWorkDir);
	g_VSS.SetWorkFolder(str);
	g_VSS.SetProjectPath("EditMaps");
	g_VSS.GetFile("EditMaps\\workspace.ws");
	
	std::vector<CString> listWorkspace;
	// Create folder
	AScriptFile File;
	if (File.Open("EditMaps\\workspace.ws"))
	{//Clear current project list
		CString path;
		while (File.GetNextToken(true))
		{
			path.Format("%sEditMaps\\%s",g_szWorkDir,File.m_szToken);
			if(!PathFileExists(path))
			{
				if(!CreateDirectory(path, NULL))
				{
					MessageBox("CMainFrame::OnGetAllMapData() ,创建取出路径失败！");
					listWorkspace.clear();
					return;
				}
			}
			path.Format("%sEditMaps\\%s",g_szWorkDir,File.m_szToken);
			g_VSS.SetWorkFolder(path);
			path.Format("EditMaps\\%s",File.m_szToken);
			g_VSS.SetProjectPath(path);
			path.Format("EditMaps\\%s\\projlist.dat",File.m_szToken);
			g_VSS.GetFile(path);
			listWorkspace.push_back(CString(File.m_szToken));

			path.Format("EditMaps\\%s\\randommap.dat",File.m_szToken);
			g_VSS.GetFile(path);
		}
		File.Close();
	}
	// TODO: Add your command handler code here
	for( int i = 0; i < listWorkspace.size(); i++)
	{
		str.Format("%sEditMaps\\%s",g_szWorkDir,listWorkspace[i]);
		g_VSS.SetWorkFolder(str);
		str.Format("EditMaps\\%s",listWorkspace[i]);
		g_VSS.SetProjectPath(str);
		g_VSS.GetAllFiles(str);
		g_VSS.SetProjectPath("");
		// Load project file list
		DWORD dwCol;
		char szProj[256];
		sprintf(szProj,"EditMaps\\%s\\ProjList.dat",listWorkspace[i]);
		if (!File.Open(szProj))
		{
			g_Log.Log("Open file %s failed!",szProj);
			continue;
		}
		
		if(File.GetNextToken(true))
		{
			CString strCol(File.m_szToken);
			if(strCol.Find("col=")!=-1)
			{
				strCol.Replace("col=","");
				dwCol = (int)atof(strCol);
			}else
			{
				File.ResetScriptFile();
				dwCol = 1;
			}
		}
		
		while (File.GetNextToken(true))
		{
			CString dstPath,srcPath,strFilePathName;
			strFilePathName.Format("%sEditMaps\\%s\\%s\\%s.elproj",g_szWorkDir,listWorkspace[i],File.m_szToken,File.m_szToken);
			if(AUX_FileIsReadOnly(strFilePathName))
			{
				//如果文件已经被本地取出，本地就是最新的，就不能覆盖本地的地图文件
				dstPath.Format("%sEditMaps\\%s\\%s",g_szWorkDir,listWorkspace[i],File.m_szToken);
				srcPath.Format("%s\\MapsRenderData\\%s\\%s",g_Configs.szServerPath,listWorkspace[i],File.m_szToken);
				AUX_CopyRenderFiles(dstPath,srcPath);
			}
		}
		File.Close();
	}
	MessageBox("地图更新完成!");
	EndWaitCursor();
}


void CMainFrame::OnExpTopviewMap() 
{
	// TODO: Add your command handler code here
	CTerrainResExpDlg dlg;
	dlg.nExpType=exp_topview;
	g_Configs.bShowAIArea = false;
	g_Configs.bShowAudio = false;
	g_Configs.bShowBoxArea = false;
	g_Configs.bShowGrass = false;
	g_Configs.bShowCritterGroup = false;
	g_Configs.bShowDummy = false;
	g_Configs.bShowLight = false;
	g_Configs.bShowGather = false;
	g_Configs.bShowNPCPath = false;
	g_Configs.bShowNonNPCPath = false;
	g_Configs.bShowFixedNpc = false;
	g_Configs.bShowObjectName = false;
	g_Configs.bShowSkinModel = false;
	g_Configs.bShowSpecially = false;
	g_Configs.bShowRange = false;
	g_Configs.bShowCloudBox = false;
	g_Configs.bShowCloudLine = false;
	g_Configs.bShowInstanceBox = false;
	g_Configs.bShowDynamic = false;
	g_bShowPosDot = false;
	dlg.DoModal();
	g_bShowPosDot = true;
}

void CMainFrame::OnOperationTestBeizerIntersect() 
{
	// TODO: Add your command handler code here
	if(m_pMap)
	{
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount() == 1)
		{
			CSceneObject * pObj1 = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			if(pObj1->GetObjectType() == CSceneObject::SO_TYPE_BEZIER )
			{
				if(((CEditerBezier*)pObj1)->IntersectWithSelf())
					AfxMessageBox("该曲线自相交,请修改头点与尾点的位置！");
				else AfxMessageBox("该曲线正常!");
			}
		}
	}
}

void CMainFrame::OnTestPrecinct() 
{
	// TODO: Add your command handler code here
	if(m_pMap)
	{
		bool error = false;
		CELBitmap *pPrecinctMap = m_pMap->ExpPrecinctMap();
		if(pPrecinctMap==NULL) 
		{
			MessageBox("检测失败!不能生成区域位图.");
			return;
		}
		BYTE *pData = pPrecinctMap->GetBitmapData();
		for( int w  = 0; w < pPrecinctMap->GetWidth(); w++)
			for( int h = 0; h < pPrecinctMap->GetHeight(); h++)
				if(pData[h*pPrecinctMap->GetWidth() + w]!=0)
				{
					error = true;
					break;
				}
		if(error)
		{
			pPrecinctMap->SaveToFile("precinct.bmp");
			MessageBox("区域未完全覆盖!请查看编辑器路径下的Precinct.bmp");
		}else MessageBox("区域覆盖所有范围!");
		pPrecinctMap->Release();
		delete pPrecinctMap;
	}
}
 

void CMainFrame::OnTestPrecinctError() 
{
	// TODO: Add your command handler code here
	CTerrainResExpDlg dlg;
	dlg.nExpType=exp_test_precinct;
	dlg.DoModal();
}

void CMainFrame::OnShowModelPlane() 
{
	// TODO: Add your command handler code here
	g_bShowModelPlane = !g_bShowModelPlane;
}

void CMainFrame::OnUpdateShowModelPlane(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_bShowModelPlane);
}

void CMainFrame::OnMergeTreeType() 
{
	// TODO: Add your command handler code here
	if(m_pMap)
	{
		BeginWaitCursor();
		m_pMap->MergeTreeType();
		m_pMap->SetModifiedFlag(true);
		GetToolTabWnd()->UpdatePlantsPanel();
		EndWaitCursor();
		MessageBox("树优化处理完成!");
	}
}

void CMainFrame::OnRotateX() 
{
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_ROTATE_EX);
	((CSceneObjectRotateOperationEx*)pOperation)->SetRotateFlag(CSceneObjectRotateOperationEx::ROTATE_X);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 12;
	
	// TODO: Add your command handler code here
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	if(m_StatusEditX.m_hWnd == NULL)
    { /* create it */
     m_StatusEditX.Create(&m_wndStatusBar, ID_INDICATOR_EDIT_X, WS_VISIBLE | WS_BORDER);
    } /* create it */
}

void CMainFrame::OnRotateY() 
{
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_ROTATE_EX);
	((CSceneObjectRotateOperationEx*)pOperation)->SetRotateFlag(CSceneObjectRotateOperationEx::ROTATE_Y);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 12;
	// TODO: Add your command handler code here
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.DestroyWindow();

	if(m_StatusEditY.m_hWnd == NULL)
    { /* create it */
     m_StatusEditY.Create(&m_wndStatusBar, ID_INDICATOR_EDIT_Y, WS_VISIBLE | WS_BORDER);
    } /* create it */
}

void CMainFrame::OnRotateZ() 
{
	m_ToolTabWnd.RiseUpAllSceneObjectButton();
	m_ToolTabWnd.RiseUpAllTerrainOperationButton();
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_OBJECT_ROTATE_EX);
	((CSceneObjectRotateOperationEx*)pOperation)->SetRotateFlag(CSceneObjectRotateOperationEx::ROTATE_Z);
	m_wndViewframe.SetCurrentOperation(pOperation);
	m_nSceneObjectOperation = 12;
	// TODO: Add your command handler code here
	
	if(m_StatusEditX.m_hWnd) m_StatusEditX.DestroyWindow();
	if(m_StatusEditY.m_hWnd) m_StatusEditY.DestroyWindow();
	
	if(m_StatusEditZ.m_hWnd == NULL)
    { /* create it */
     m_StatusEditZ.Create(&m_wndStatusBar, ID_INDICATOR_EDIT_Z, WS_VISIBLE | WS_BORDER);
    } /* create it */
}

void CMainFrame::OnUpdateRotateX(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_StatusEditX.m_hWnd == NULL) pCmdUI->SetCheck(0);
	else pCmdUI->SetCheck(1);
}

void CMainFrame::OnUpdateRotateY(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_StatusEditY.m_hWnd == NULL) pCmdUI->SetCheck(0);
	else pCmdUI->SetCheck(1);
}

void CMainFrame::OnUpdateRotateZ(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_StatusEditZ.m_hWnd == NULL) pCmdUI->SetCheck(0);
	else pCmdUI->SetCheck(1);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	//RECT rc;
	//GetClientRect(&rc);
	//m_wndVssDlgBar.SetWindowPos(NULL,0,0,rc.right,50,SW_SHOW);
	// TODO: Add your message handler code here
	m_StatusEditX.Reposition();
	m_StatusEditY.Reposition();
	m_StatusEditZ.Reposition();
	m_StatusCombo.Reposition();
	m_StatusProgress.Reposition();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			CString txt;
			
			if(m_pMap && (m_StatusEditX.m_hWnd || m_StatusEditY.m_hWnd || m_StatusEditZ.m_hWnd))
			{
				if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount()==1)
				{
					CSceneObject *test = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetHead();
					A3DMATRIX4 matr = test->GetRotateMatrix();
					float anglex, angley,anglez;
					float data[11] = { matr._11,matr._12,matr._13,matr._14,matr._21,matr._22,matr._23,matr._24,matr._31,matr._32,matr._33}; 
					AUX_ToEulerAnglesXYZ(data,anglex,angley,anglez);
					CString str;
					if(m_StatusEditX.m_hWnd) 
					{
						m_StatusEditX.GetWindowText(str);
						anglex = (float)atof(str);
					}
					
					if(m_StatusEditY.m_hWnd) 
					{
						m_StatusEditY.GetWindowText(str);
						angley = (float)atof(str);
					}
					
					if(m_StatusEditZ.m_hWnd) 
					{
						m_StatusEditZ.GetWindowText(str);
						anglez = (float)atof(str);
					}
					
					test->ResetRotateMat();
					test->RotateX(anglex * A3D_PI / 180);
					test->RotateY(angley * A3D_PI / 180);
					test->RotateZ(anglez * A3D_PI / 180);
					SetFocus();
				}
			}
		}if(pMsg->wParam == VK_HOME)
		{
			g_Configs.fCameraMove +=1.0f;
		}if(pMsg->wParam == VK_END)
		{
			g_Configs.fCameraMove -=1.0f;
			if(g_Configs.fCameraMove < 1.0f) g_Configs.fCameraMove = 1.0f;
		}
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::SetXValue(float x)
{
	CString str;
	str.Format("%4.2f", x);
	if(m_StatusEditX.m_hWnd) m_StatusEditX.SetWindowText(str);
	else m_wndStatusBar.SetPaneText(3, str, TRUE);
}

void CMainFrame::SetYValue(float y)
{
	CString str;
	str.Format("%4.2f", y);
	if(m_StatusEditY.m_hWnd) m_StatusEditY.SetWindowText(str);
	else m_wndStatusBar.SetPaneText(5, str, TRUE);
}

void CMainFrame::SetZValue(float z)
{
	CString str;
	str.Format("%4.2f", z);
	
	if(m_StatusEditZ.m_hWnd) m_StatusEditZ.SetWindowText(str);
	else m_wndStatusBar.SetPaneText(7, str, TRUE);
}

void CMainFrame::OnSelchangeCombo()
{
	if(m_pMap==NULL) return;
	int n = m_StatusCombo.GetCurSel();
	if(n == CB_ERR) return;
	CString s;
	BeginWaitCursor();
	m_StatusCombo.GetLBText(n, s);
	if(m_strLastNeighborMap == s) return;
	if(s=="不显示相邻地图")
	{
		if(m_pMap->LoadNeighborTerrain(4))
			m_strLastNeighborMap = s;
		m_pMap->LoadNeighborModels(4);
	}
	if(s=="显示相邻地图(上)")
	{
		if(m_pMap->LoadNeighborTerrain(1))
			m_strLastNeighborMap = s;
		m_pMap->LoadNeighborModels(1);
	}
	
	if(s=="显示相邻地图(下)")
	{
		if(m_pMap->LoadNeighborTerrain(7))
			m_strLastNeighborMap = s;
			m_pMap->LoadNeighborModels(7);
	}
		
	if(s=="显示相邻地图(左)")
	{
		if(m_pMap->LoadNeighborTerrain(3))
			m_strLastNeighborMap = s;
		m_pMap->LoadNeighborModels(3);
	}
	
	if(s=="显示相邻地图(右)")
	{
		if(m_pMap->LoadNeighborTerrain(5))
			m_strLastNeighborMap = s;
		m_pMap->LoadNeighborModels(5);
	}
		
	EndWaitCursor();
	
	SetFocus();
}



void CMainFrame::OnShowSelOnly() 
{
	// TODO: Add your command handler code here
	g_bShowSelectedOnly = !g_bShowSelectedOnly;
}

void CMainFrame::OnUpdateShowSelOnly(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_bShowSelectedOnly);
}

void CMainFrame::OnSetWorkspace() 
{
	// TODO: Add your command handler code here
	CWorkSpaceSetDlg dlg;
	if(IDOK==dlg.DoModal())
	{
	}
}

void CMainFrame::OnUpdateSetWorkspace(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_pMap) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CMainFrame::OnShowTerrainLight() 
{
	// TODO: Add your command handler code here
	g_bShowTerrainLight = !g_bShowTerrainLight;
	
	if(m_pMap) 
	{
		if(m_pMap->GetTerrain())
		{
			CTerrainRender *pRender = m_pMap->GetTerrain()->GetRender();
			if(pRender) pRender->EnableLight(g_bShowTerrainLight);
		}
		if(m_pMap->GetNeighborTerrain())
		{
			CTerrainRender *pRender = m_pMap->GetNeighborTerrain()->GetRender();
			if(pRender) pRender->EnableLight(g_bShowTerrainLight);
		}
	}
}

void CMainFrame::OnUpdateShowTerrainLight(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(!g_bShowTerrainLight);
}

void CMainFrame::OnShowDistance() 
{
	// TODO: Add your command handler code here
	if(m_pMap) 
	{
		CString msg;
		if(m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount()==2)
		{
			CSceneObject *pObj1 = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetHead();
			CSceneObject *pObj2 = m_pMap->GetSceneObjectMan()->m_listSelectedObject.GetTail();
			A3DVECTOR3 v1 = pObj1->GetPos();
			A3DVECTOR3 v2 = pObj2->GetPos();
			v1 = v2 - v1;
			float dis = v1.Magnitude();
			msg.Format("%4.2fm",dis);
			MessageBox(msg);
		}
	}
}

void CMainFrame::OnUpdateShowDistance(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
}

void CMainFrame::OnInsertMounsterToMap() 
{
	const float delta = 20;
	// TODO: Add your command handler code here
	if(m_pMap==NULL) return;
	CTerrain *pTerrain = m_pMap->GetTerrain();
	if(pTerrain==NULL) return;
	ARectF rc = pTerrain->GetTerrainArea();
	float lx = rc.left + delta;
	float lz = rc.bottom + delta;

	char szFilter[] = "Resorce Files (*.txt)|*.txt||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.txt", NULL, dwFlags, szFilter, NULL);
	if(FileDia.DoModal()==IDOK)
	{
		CString strPathName = FileDia.GetPathName();
		AScriptFile File;
		if (File.Open(strPathName))
		{
			AString objName;
			int num = 0;
			while (File.GetNextToken(true))
			{
				while(1)
				{
					objName.Format("TestMounster_%d",num++);
					if(m_pMap->GetSceneObjectMan()->FindSceneObject(objName)==NULL)
						break;
				}
				
				CSceneFixedNpcGenerator *pNew = m_pMap->GetSceneObjectMan()->CreateFixedNpc(objName);
				if(pNew)
				{
					
					if(lx > rc.right - delta)
					{	
						lx = rc.left + delta;
						lz += delta;
					}else lx += delta;
					A3DVECTOR3 vPos(lx,0,lz);
					float fh = pTerrain->GetPosHeight(vPos);
					vPos.Set(vPos.x,fh,vPos.z);
					pNew->SetPos(vPos);
					pNew->SetObjectID(m_pMap->GetSceneObjectMan()->GenerateObjectID());
					
					FIXED_NPC_DATA data = pNew->GetProperty();
					data.bIsMounster = true;
					data.dwID = atof(File.m_szToken);
					pNew->SetProperty(data);
					data.strNpcName = pNew->GetNPCName(data.dwID);
					pNew->SetProperty(data);
					pNew->UpdateProperty(false);
					pNew->ReloadModel();
				}
				
			}
			File.Close();
			m_ToolTabWnd.UpdateSceneObjectList();
		}
	}

}

void CMainFrame::OnGetCurrentProjWorkspaceMaps() 
{
	// TODO: Add your command handler code here
	BeginWaitCursor();
	CString str;
	str.Format("%sEditMaps",g_szWorkDir);
	g_VSS.SetWorkFolder(str);
	g_VSS.SetProjectPath("EditMaps");
	g_VSS.GetFile("EditMaps\\workspace.ws");
	
	std::vector<CString> listWorkspace;
	// Create folder
	AScriptFile File;
	if (File.Open("EditMaps\\workspace.ws"))
	{//Clear current project list
		CString path;
		while (File.GetNextToken(true))
		{
			path.Format("%sEditMaps\\%s",g_szWorkDir,File.m_szToken);
			if(!PathFileExists(path))
			{
				if(!CreateDirectory(path, NULL))
				{
					MessageBox("CMainFrame::OnGetAllMapData() ,创建取出路径失败！");
					listWorkspace.clear();
					return;
				}
			}
			path.Format("%sEditMaps\\%s",g_szWorkDir,File.m_szToken);
			g_VSS.SetWorkFolder(path);
			path.Format("EditMaps\\%s",File.m_szToken);
			g_VSS.SetProjectPath(path);
			path.Format("EditMaps\\%s\\projlist.dat",File.m_szToken);
			g_VSS.GetFile(path);
			listWorkspace.push_back(CString(File.m_szToken));

			path.Format("EditMaps\\%s\\randommap.dat",File.m_szToken);
			g_VSS.GetFile(path);
		}
		File.Close();
	}
	listWorkspace.clear();
	listWorkspace.push_back(CString(g_Configs.szCurProjectName));
	// TODO: Add your command handler code here
	for( int i = 0; i < listWorkspace.size(); i++)
	{
		str.Format("%sEditMaps\\%s",g_szWorkDir,listWorkspace[i]);
		g_VSS.SetWorkFolder(str);
		str.Format("EditMaps\\%s",listWorkspace[i]);
		g_VSS.SetProjectPath(str);
		g_VSS.GetAllFiles(str);
		g_VSS.SetProjectPath("");
		// Load project file list
		DWORD dwCol;
		char szProj[256];
		sprintf(szProj,"EditMaps\\%s\\ProjList.dat",listWorkspace[i]);
		if (!File.Open(szProj))
		{
			g_Log.Log("Open file %s failed!",szProj);
			listWorkspace.clear();
			EndWaitCursor();
			return;
		}
		
		if(File.GetNextToken(true))
		{
			CString strCol(File.m_szToken);
			if(strCol.Find("col=")!=-1)
			{
				strCol.Replace("col=","");
				dwCol = (int)atof(strCol);
			}else
			{
				File.ResetScriptFile();
				dwCol = 1;
			}
		}
		
		while (File.GetNextToken(true))
		{
			CString dstPath,srcPath,strFilePathName;
			strFilePathName.Format("%sEditMaps\\%s\\%s\\%s.elproj",g_szWorkDir,listWorkspace[i],File.m_szToken,File.m_szToken);
			if(AUX_FileIsReadOnly(strFilePathName))
			{
				//如果文件已经被本地取出，本地就是最新的，就不能覆盖本地的地图文件
				dstPath.Format("%sEditMaps\\%s\\%s",g_szWorkDir,listWorkspace[i],File.m_szToken);
				srcPath.Format("%s\\MapsRenderData\\%s\\%s",g_Configs.szServerPath,listWorkspace[i],File.m_szToken);
				AUX_CopyRenderFiles(dstPath,srcPath);
			}
		}
		File.Close();
	}
	MessageBox("地图更新完成!");
	EndWaitCursor();
}

void CMainFrame::OnMounsterStat() 
{
	// TODO: Add your command handler code here
	CMousterStatFilterDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnModelChanged() 
{
	// TODO: Add your command handler code here
	CModelChangedStatusDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnExpLightData() 
{
	// TODO: Add your command handler code here
}

void CMainFrame::OnExportSkyInfor() 
{
	// TODO: Add your command handler code here
	if(IDYES != AfxMessageBox("你确定要输出当前工作空间中所用到的天空纹理路径吗？",MB_YESNO|MB_ICONQUESTION)) return;
	abase::vector<CString> listSky;

	
	ALog  m_Log;
	char szLogPath[MAX_PATH];
	sprintf(szLogPath,"%s_sky.log",g_Configs.szCurProjectName);
	m_Log.Init(szLogPath, "下面是项目中用到的天空纹理路径...");

	//逐一检测地图
	CString mapName;
	CElementMap emap;
	
	// Load project file list
	AScriptFile File;
	DWORD dwCol;
	char szProj[256];
	sprintf(szProj,"%s\\ProjList.dat",g_szEditMapDir);
	if (!File.Open(szProj))
	{
		g_Log.Log("Open file %s failed!",szProj);
		return;
	}
	
	if(File.GetNextToken(true))
	{
		CString strCol(File.m_szToken);
		if(strCol.Find("col=")!=-1)
		{
			strCol.Replace("col=","");
			dwCol = (int)atof(strCol);
		}else
		{
			File.ResetScriptFile();
			dwCol = 1;
		}
		
	}
	abase::vector<CString> projectList;
	while (File.GetNextToken(true))
	{
		CString strNew(File.m_szToken);
		projectList.push_back(strNew);
	}
	File.Close();
	
	// Proessing map
	BeginWaitCursor();
	abase::vector<CString>::iterator theIterator;
	for(theIterator = projectList.begin(); theIterator != projectList.end(); theIterator ++)
	{
		
		mapName.Format("%s%s\\%s\\%s.elproj",g_szWorkDir,g_szEditMapDir,(*theIterator),(*theIterator));
		if(!emap.Load(mapName))
		{
			CString error_msg;
			error_msg.Format("不能打开地图文件 :%s",mapName);
			MessageBox(error_msg);
			EndWaitCursor();
			return;
		}
		CSceneObjectManager *pMan =emap.GetSceneObjectMan();
		ALISTPOSITION pos = pMan->m_listSceneObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pMan->m_listSceneObject.GetPrev(pos);
			int type = ptemp->GetObjectType();
			if(type==CSceneObject::SO_TYPE_AREA)
			{
				CSceneBoxArea *pArea = (CSceneBoxArea *)ptemp;
				BOX_AREA_PROPERTY pro;
				pArea->GetProperty(pro);
				
				CString pathArray[6];
				pathArray[0] = pro.m_strSky0;
				pathArray[1] = pro.m_strSky1;
				pathArray[2] = pro.m_strSky2;
				pathArray[3] = pro.m_strSkyNight0;
				pathArray[4] = pro.m_strSkyNight1;
				pathArray[5] = pro.m_strSkyNight2;
				
				for( int j = 0; j < 6; ++j)
				{
					CString temp = pathArray[j];
					int npos = temp.ReverseFind('\\');
					temp = temp.Left(npos);
					m_Log.Log("%s------%s",emap.GetMapName(),temp);
					bool bInsert = true;
					for( int i = 0; i < listSky.size(); ++i)
					{
						if(listSky[i] == temp)
						{
							bInsert = false;
							break;
						}
					}
					if(bInsert) listSky.push_back(temp);
				}
			}
		}
		emap.Release();
	}
	
	m_Log.Log("下面是用到过的天空纹理列表:");
	for( int k = 0; k < listSky.size(); ++k)
		m_Log.Log(listSky[k]);
	
	EndWaitCursor();
	m_Log.Release();
	MessageBox("处理完成!");
}

void CMainFrame::OnExpLightInforList() 
{
	// TODO: Add your command handler code here
	CExportLitDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnDynamicObjectEditor() 
{
	// TODO: Add your command handler code here
	CDynamicObjectManDlg dlg;
	dlg.DoModal();
}


void CMainFrame::OnMonsterControllerEditor() 
{
	// TODO: Add your command handler code here
	CNpcControllerManDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnReloadAllTextures() 
{
	// TODO: Add your command handler code here
	BeginWaitCursor();
	g_Render.GetA3DEngine()->GetA3DTextureMan()->ReloadAllTextures(false);
	EndWaitCursor();
}

void CMainFrame::OnExportStatNpcs() 
{
	
	// TODO: Add your command handler code here
	CElementMap *pMap = GetMap();
	if(pMap==NULL) return; 
	
	char szFilter[] = "Text Files (*.txt)|*.txt||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(FALSE, "*.txt", NULL, dwFlags, szFilter, NULL);
	CString text;
	if(FileDia.DoModal()==IDOK)
	{
		CString strPathName = FileDia.GetPathName();
		AFile file;
		if(!file.Open(strPathName,AFILE_CREATENEW | AFILE_TEXT | AFILE_NOHEAD)) return;
		
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		BeginWaitCursor();
		
		text.Format("AI AREA :");
		file.WriteLine(text);

		APtrList<CSceneObject *>* al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_AIAREA);
		ALISTPOSITION pos = al->GetTailPosition();
		while( pos )
		{
			CSceneAIGenerator* ptemp = (CSceneAIGenerator*)al->GetPrev(pos);
			if( !ptemp->IsDeleted())
			{
				int n = ptemp->GetNPCNum();
				for(int i = 0; i < n; i++)
				{
					NPC_PROPERTY* np = ptemp->GetNPCProperty(i);
					text.Format("Npc id = %d; Pos = (%4.2f,%4.2f,%4.2f); Box = (L %4.2f, W %4.2f, H %4.2f); count = %d; PathID = %d",np->dwID,ptemp->GetPos().x,ptemp->GetPos().y,ptemp->GetPos().z,ptemp->GetProperty().fEdgeLen, ptemp->GetProperty().fEdgeWth, ptemp->GetProperty().fEdgeHei,np->dwNum,np->nPathID);
					file.WriteLine(text);
				}
			}
		}
		
		text.Format("\r\n\r\n\r\n FIXED NPCS :");
		file.WriteLine(text);
		
	
		al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_FIXED_NPC);
		pos = al->GetTailPosition();
		while( pos )
		{
			CSceneFixedNpcGenerator* ptemp = (CSceneFixedNpcGenerator*)al->GetPrev(pos);
			if( !ptemp->IsDeleted())
			{
				FIXED_NPC_DATA dat = ptemp->GetProperty();
				text.Format("Npc id = %d; Pos = (%4.2f,%4.2f,%4.2f); Radius = %4.2f; PathID = %d",dat.dwID,ptemp->GetPos().x,ptemp->GetPos().y,ptemp->GetPos().z,ptemp->GetProperty().fRadius, ptemp->GetProperty().nPathID);
				file.WriteLine(text);
			}
		}
		
		al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_GATHER);
		pos = al->GetTailPosition();
		while( pos )
		{
			CSceneGatherObject* ptemp = (CSceneGatherObject*)al->GetPrev(pos);
			if( !ptemp->IsDeleted())
			{
				int n = ptemp->GetGatherNum();
				for(int i = 0; i < n; i++)
				{
					GATHER* np = ptemp->GetGather(i);
					text.Format("Mine id = %d; Pos = (%4.2f,%4.2f,%4.2f); Box = (L %4.2f, W %4.2f)",np->dwId,ptemp->GetPos().x,ptemp->GetPos().y,ptemp->GetPos().z,ptemp->GetProperty().fEdgeLen, ptemp->GetProperty().fEdgeWth);
					file.WriteLine(text);
				}
			}
		}

		file.Close();
		EndWaitCursor();
	}
	
}

void CMainFrame::OnStatusTriangleSel() 
{
	// TODO: Add your command handler code here
	if(m_pMap) 
	{
		UINT nTriangleNum = 0;
		CSceneObjectManager *pMan = m_pMap->GetSceneObjectMan();
		ALISTPOSITION pos = pMan->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pMan->m_listSelectedObject.GetPrev(pos);
			nTriangleNum += ptemp->GetTriangleNum();
		}
		CString strMsg;
		strMsg.Format("被选中对象的三角形总数: %d",nTriangleNum);
		MessageBox(strMsg);
	}
}

void CMainFrame::OnObjectDrop() 
{
	// TODO: Add your command handler code here
	
	if(m_pMap) 
	{
		if(IDNO == AfxMessageBox("确定要让选择的所有对象落地么？",MB_YESNO)) return;
		CSceneObjectManager *pMan = m_pMap->GetSceneObjectMan();
		ALISTPOSITION pos = pMan->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pMan->m_listSelectedObject.GetPrev(pos);
			A3DVECTOR3 vPos = ptemp->GetPos();
			RAYTRACERT rt;
			A3DVECTOR3 vHitPos,vNormal;
			CConvexHullData *pTraceCHData = NULL;
			float fFraction = 1.0f;
			vHitPos = vPos;
			bool bTrace = pMan->RayTraceStaticModel(vPos,vPos + A3DVECTOR3(0,-99999.0f,0),vHitPos,&fFraction,vNormal,pTraceCHData);
			if(m_pMap->RayTrace(vPos,A3DVECTOR3(0,-99999.0f,0),1,&rt))
			{
				if(bTrace)
				{
					A3DVECTOR3 vTemp = rt.vHitPos - vPos;
					if(vTemp.MagnitudeH() < (vHitPos - vPos).MagnitudeH())
						vHitPos = rt.vHitPos;
				}else vHitPos = rt.vHitPos;
			}
			ptemp->SetPos(vHitPos);	
		}
	}

	/*
	if(m_pMap) 
	{
		CSceneObjectManager *pMan = m_pMap->GetSceneObjectMan();
		pMan->ExportBuilding();
	}*/
}

void CMainFrame::OnTranlateMapToPw() 
{
	// TODO: Add your command handler code here
	
	if(m_pMap) 
	{
		if(IDYES == AfxMessageBox("你确定要把当前地图转换到完美世界项目中去么？",MB_YESNO))
		{
			CSceneObjectManager *pMan = m_pMap->GetSceneObjectMan();
			pMan->ExportBuilding();
		}
	}
}

void CMainFrame::OnExportCoordData() 
{	
	// TODO: Add your command handler code here
	CElementMap *pMap = GetMap();
	if(pMap==NULL) return;
	
	ProjList	projList;
	if (!projList.Init()){
		return;
	}
	int iRow(-1), iCol(-1);
	if (!projList.GetRowCol(pMap->GetMapName(), iRow, iCol)){
		return;
	}
	float fMapSize = 1024.0f;
	
	if(pMap->GetTerrain())
		fMapSize = pMap->GetTerrain()->GetTerrainSize();

	float fOffsetX = (iCol - (projList.GetNumCol()-1)*0.5f)*fMapSize;
	float fOffsetZ = ((projList.GetNumRow()-1)*0.5f - iRow)*fMapSize;
	
	char szFilter[] = "Text Files (*.txt)|*.txt||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(FALSE, "*.txt", "Coord_data.txt", dwFlags, szFilter, NULL);
	CString text;
	if(FileDia.DoModal()!=IDOK){
		return;
	}
	CString strPathName = FileDia.GetPathName();
	AFile file;
	if(!file.Open(strPathName,AFILE_CREATENEW | AFILE_TEXT | AFILE_NOHEAD)){
		return;
	}
	const char *szMapName = g_Configs.szCurProjectName;
	const char *szFormat = "%d		%6s		%10.2f	%10.2f	%10.2f";
	
	CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
	BeginWaitCursor();
	
	APtrList<CSceneObject *>* al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_AIAREA);
	ALISTPOSITION pos = al->GetTailPosition();
	while( pos )
	{
		CSceneAIGenerator* ptemp = (CSceneAIGenerator*)al->GetPrev(pos);
		if( !ptemp->IsDeleted())
		{
			int n = ptemp->GetNPCNum();
			for(int i = 0; i < n; i++)
			{
				NPC_PROPERTY* np = ptemp->GetNPCProperty(i);
				A3DVECTOR3 pos = ptemp->GetPos();
				text.Format(szFormat,np->dwID, szMapName, pos.x+fOffsetX, pos.y, pos.z+fOffsetZ);
				file.WriteLine(text);
			}
		}
	}	
	
	al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_FIXED_NPC);
	pos = al->GetTailPosition();
	while( pos )
	{
		CSceneFixedNpcGenerator* ptemp = (CSceneFixedNpcGenerator*)al->GetPrev(pos);
		if( !ptemp->IsDeleted())
		{
			FIXED_NPC_DATA dat = ptemp->GetProperty();			
			A3DVECTOR3 pos = ptemp->GetPos();
			text.Format(szFormat,dat.dwID, szMapName, pos.x+fOffsetX, pos.y, pos.z+fOffsetZ);
			file.WriteLine(text);
		}
	}
	
	al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_GATHER);
	pos = al->GetTailPosition();
	while( pos )
	{
		CSceneGatherObject* ptemp = (CSceneGatherObject*)al->GetPrev(pos);
		if( !ptemp->IsDeleted())
		{
			int n = ptemp->GetGatherNum();
			for(int i = 0; i < n; i++)
			{
				GATHER* np = ptemp->GetGather(i);
				A3DVECTOR3 pos = ptemp->GetPos();
				text.Format(szFormat,np->dwId, szMapName, pos.x+fOffsetX, pos.y, pos.z+fOffsetZ);
				file.WriteLine(text);
			}
		}
	}
	
	file.Close();
	EndWaitCursor();
}

void CMainFrame::OnSelectByID() 
{	
	if(!m_pMap){
		return;
	}
	CDlgSceneObjectFinder dlgFinder;
	if (dlgFinder.DoModal() != IDOK){
		return;
	}
	CSceneObjectManager *pMan = m_pMap->GetSceneObjectMan();

	//	添加选中结果
	pMan->ClearSelectedList();	
	ALISTPOSITION pos = dlgFinder.m_listSelectedObject.GetTailPosition();
	while (pos){
		CSceneObject* pTemp = dlgFinder.m_listSelectedObject.GetPrev(pos);
		pMan->AddObjectPtrToSelected(pTemp);
	}

	//	更新属性栏
	if (pMan->m_listSelectedObject.GetCount()==1){
		PSCENEOBJECT pObj = pMan->m_listSelectedObject.GetHead();
		pObj->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pObj->m_pProperty);
		AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(false);
		AUX_GetMainFrame()->SetFocus();
	}else{
		AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel();
		AUX_GetMainFrame()->SetFocus();
	}
}
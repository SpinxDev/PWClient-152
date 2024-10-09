// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__7A7C173F_02F5_4560_ADE2_92E1CA63A34A__INCLUDED_)
#define AFX_MAINFRM_H__7A7C173F_02F5_4560_ADE2_92E1CA63A34A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ViewFrame.h"
#include "ToolTabWnd.h"
#include "StatusEdit.h"
#include "StatusProgress.h"
#include "StatusStatic.h"
#include "StatusCombo.h"
#include "DayNightSetDlg.h"
#include "VssDlgBar.h"


class CElementMap;

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

	//	Get view frame
	CViewFrame* GetViewFrame() { return &m_wndViewframe; }
	//	Get map object
	CElementMap* GetMap() { return m_pMap; }
	//	Get tool tab window
	CToolTabWnd* GetToolTabWnd() { return &m_ToolTabWnd; }
	CVssDlgBar * GetVssBar(){ return &m_wndVssDlgBar; }

	//	Initialize device objects
	bool InitDeviceObjects();

	//	Application is active ?
	bool AppIsActive() { return m_bActive; }
	//	Called when application is being closed
	bool OnAppExit();
	//	Get tick time
	DWORD GetTickTime() { return m_dwTickTime; }

	//	Tick routine
	bool Tick();

	// Update brush width show
	void UpdateBrushWidth();
	// Updeate brush height
	void UpdateBrushHeight( float fHeight);
	//这儿是为了控制其工具条上的菜单命令状态
	void SetOtherOperation(bool bUseOther);
	void EnableBezierOperation(bool bEnable);
	void EnableBezierTest(bool bEnable){ m_bEnableBezierTest = bEnable; };
	void EnableBezierSmooth(bool bEnable){ m_bEnableBezierSmooth = bEnable; };
	void EnableBezierMerge(bool bEnable){ m_bEnableBezierMerge = bEnable; };
	void SetShowMergeDlg(bool bshow){ m_bShowMergeDlg = bshow; };
	void SetXValue(float x);
	void SetYValue(float y);
	void SetZValue(float z);
	//	Update main frame title
	void UpdateMainFrameTitle();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  //	control bar embedded members

	CStatusBar     m_wndStatusBar;
	CVssDlgBar     m_wndVssDlgBar;
	CToolBar		 m_wndToolBar;
	CToolBar         m_wndBrushToolBar;
	CViewFrame		 m_wndViewframe;
	CToolTabWnd		 m_ToolTabWnd;
	CStatic          m_StaticBrushWidth;	//	Only for show brush width

	CElementMap*	m_pMap;		//	Map object

	bool		m_bActive;		//	App active flag
	bool        m_bShowMergeDlg;
	DWORD		m_dwLastFrame;	//	Time of last frame
	DWORD		m_dwTickTime;	//	Tick time

	int         m_nSceneObjectOperation;//0,表示其他，1，选择。2，移动。3旋转
	bool        m_bOtherOperation; //决定操作菜单命令是否显示压下状态
	bool        m_bEnableBezierOperation;
	bool        m_bEnableBezierSmooth;
	bool        m_bEnableBezierTest;
	bool        m_bEnableBezierMerge;
	CString     m_strLastNeighborMap;


protected:	//	Operations
	CStatusProgress m_StatusProgress;
	CStatusEdit  m_StatusEditX;
	CStatusEdit  m_StatusEditY;
	CStatusEdit  m_StatusEditZ;
	CStatusCombo m_StatusCombo;
	//CStatusStatic m_StatusIcon;
	CDayNightSetDlg m_DayAndNightDlg;

	//	Initialize tool tab window
	bool InitToolTabWnd();

	//	Create new map
	bool CreateMap();
	//	Release map
	void ReleaseMap();
	//	Save map data to file
	bool SaveMap();
	//	Tell user to save map file
	int TellToSave();
	

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg void OnFileNew();
	afx_msg void OnTerrainHeightmap();
	afx_msg void OnUpdateTerrainHeightmap(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam); 
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnTerrainSuraces();
	afx_msg void OnUpdateTerrainSuraces(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveas();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveas(CCmdUI* pCmdUI);
	afx_msg void OnFileClose();
	afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
	afx_msg void OnFileExportgamedata();
	afx_msg void OnFileExportServerBHTByProject();
	afx_msg void OnFileExportPathInfo();
	afx_msg void OnFileRandomMapInfo();
	afx_msg void OnTerrainLighting();
	afx_msg void OnUpdateTerrainLighting(CCmdUI* pCmdUI);
	afx_msg void OnTerrainCreatesurfs();
	afx_msg void OnUpdateTerrainCreatesurfs(CCmdUI* pCmdUI);
	afx_msg void OnBrushCircle();
	afx_msg void OnBrushRadius1();
	afx_msg void OnBrushRadius2();
	afx_msg void OnBrushRadius3();
	afx_msg void OnBrushRadiusAdd();
	afx_msg void OnBrushRadiusSub();
	afx_msg void OnBrushRect();
	afx_msg void OnOperationRedo();
	afx_msg void OnOperationUndo();
	afx_msg void OnUpdateOperationUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOperationRedo(CCmdUI* pCmdUI);
	afx_msg void OnOperationSelect();
	afx_msg void OnOperationMove();
	afx_msg void OnUpdateOperationMove(CCmdUI* pCmdUI);
	afx_msg void OnOperationRotate();
	afx_msg void OnUpdateOperationRotate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOperationSelect(CCmdUI* pCmdUI);
	afx_msg void OnOperationSelectmulti();
	afx_msg void OnUpdateOperationSelectmulti(CCmdUI* pCmdUI);
	afx_msg void OnOperationScale();
	afx_msg void OnUpdateOperationScale(CCmdUI* pCmdUI);
	afx_msg void OnOperationDelete();
	afx_msg void OnBezierAddpoint();
	afx_msg void OnBezierCut();
	afx_msg void OnBezierSmooth();
	afx_msg void OnBezierSubpoint();
	afx_msg void OnUpdateBezierSubpoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBezierCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBezierAddpoint(CCmdUI* pCmdUI);
	afx_msg void OnBezierDrag();
	afx_msg void OnUpdateBezierDrag(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBezierSmooth(CCmdUI* pCmdUI);
	afx_msg void OnButtonNospeak();
	afx_msg void OnUpdateButtonNospeak(CCmdUI* pCmdUI);
	afx_msg void OnButtonShowVernier();
	afx_msg void OnUpdateButtonShowVernier(CCmdUI* pCmdUI);
	afx_msg void OnBezierTest();
	afx_msg void OnUpdateBezierTest(CCmdUI* pCmdUI);
	afx_msg void OnEditObjectCopy();
	afx_msg void OnFileAutoparthm();
	afx_msg void OnFileSortMap();
	afx_msg void OnTerrainModifiedProfile();
	afx_msg void OnMapAutoMerge();
	afx_msg void OnUpdateTerrainModifiedProfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMapAutoMerge(CCmdUI* pCmdUI);
	afx_msg void OnOperationObstruct();
	afx_msg void OnBezierMerge();
	afx_msg void OnUpdateBezierMerge(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOperationObstruct(CCmdUI* pCmdUI);
	afx_msg void OnExpAiData();
	afx_msg void OnImpAiData();
	afx_msg void OnUpdateImpAiData(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExpAiData(CCmdUI* pCmdUI);
	afx_msg void OnTerrainBlockView();
	afx_msg void OnDayNight();
	afx_msg void OnUpdateDayNight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTerrainBlockView(CCmdUI* pCmdUI);
	afx_msg void OnButtonRain();
	afx_msg void OnUpdateButtonRain(CCmdUI* pCmdUI);
	afx_msg void OnButtonSnow();
	afx_msg void OnUpdateButtonSnow(CCmdUI* pCmdUI);
	afx_msg void OnMenuResMan();
	afx_msg void OnSetVss();
	afx_msg void OnGetVssAll();
	afx_msg void OnTestMapCheckin();
	afx_msg void OnExportGameRes();
	afx_msg void OnUpdateTestMapCheckin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGetVssAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMenuResMan(CCmdUI* pCmdUI);
	afx_msg void OnMoveAllSceneObject();
	afx_msg void OnGetAllMapData();
	afx_msg void OnExpTopviewMap();
	afx_msg void OnOperationTestBeizerIntersect();
	afx_msg void OnTestPrecinct();
	afx_msg void OnTestPrecinctError();
	afx_msg void OnShowModelPlane();
	afx_msg void OnUpdateShowModelPlane(CCmdUI* pCmdUI);
	afx_msg void OnMergeTreeType();
	afx_msg void OnRotateX();
	afx_msg void OnRotateY();
	afx_msg void OnRotateZ();
	afx_msg void OnUpdateRotateX(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRotateY(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRotateZ(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeCombo();
	afx_msg void OnShowSelOnly();
	afx_msg void OnUpdateShowSelOnly(CCmdUI* pCmdUI);
	afx_msg void OnSetWorkspace();
	afx_msg void OnUpdateSetWorkspace(CCmdUI* pCmdUI);
	afx_msg void OnShowTerrainLight();
	afx_msg void OnUpdateShowTerrainLight(CCmdUI* pCmdUI);
	afx_msg void OnShowDistance();
	afx_msg void OnUpdateShowDistance(CCmdUI* pCmdUI);
	afx_msg void OnInsertMounsterToMap();
	afx_msg void OnGetCurrentProjWorkspaceMaps();
	afx_msg void OnMounsterStat();
	afx_msg void OnModelChanged();
	afx_msg void OnExpLightData();
	afx_msg void OnExportSkyInfor();
	afx_msg void OnExpLightInforList();
	afx_msg void OnDynamicObjectEditor();
	afx_msg void OnMonsterControllerEditor();
	afx_msg void OnReloadAllTextures();
	afx_msg void OnExportStatNpcs();
	afx_msg void OnOperationRotate2();
	afx_msg void OnUpdateOperationRotate2(CCmdUI* pCmdUI);
	afx_msg void OnStatAllTriangle();
	afx_msg void OnStatusTriangleSel();
	afx_msg void OnObjectDrop();
	afx_msg void OnTranlateMapToPw();
	afx_msg void OnExportCoordData();
	afx_msg void OnSelectByID();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__7A7C173F_02F5_4560_ADE2_92E1CA63A34A__INCLUDED_)

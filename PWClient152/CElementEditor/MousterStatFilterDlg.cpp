// MousterStatFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "MousterStatFilterDlg.h"
#include "NpcControllerManDlg.h"
#include "MousterStatDlg.h"
#include "ElementMap.h"
#include "MainFrm.h"
#include "SceneObjectManager.h"
#include "SceneAIGenerator.h"
#include "SceneGatherObject.h"
#include "SceneDynamicObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMousterStatFilterDlg dialog


CMousterStatFilterDlg::CMousterStatFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMousterStatFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMousterStatFilterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMousterStatFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMousterStatFilterDlg)
	DDX_Control(pDX, IDC_COMBO_SET, m_comboSet);
	DDX_Control(pDX, IDC_TREE_FILTER, m_treeFilter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMousterStatFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CMousterStatFilterDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_SET, OnSelchangeComboSet)
	ON_BN_CLICKED(IDSELECTALL, OnSelectall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMousterStatFilterDlg message handlers

BOOL CMousterStatFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	HTREEITEM hItem = m_treeFilter.InsertItem("没有关联控制器");
	m_treeFilter.SetItemData(hItem,0);
	abase::vector<int> listController;
	GetMapController(listController);
	
	for( int i = 0; i < g_ControllerObjectMan.GetIdNum(); ++i)
	{
		CONTROLLER_OBJECT obj;
		g_ControllerObjectMan.GetObjByIndex(i,obj);
		if(FindContoller(listController,obj.id))
		{
			hItem = m_treeFilter.InsertItem(obj.szName);
			m_treeFilter.SetItemData(hItem,obj.id);
		}
	}
	m_comboSet.InsertString(0,"-1");
	m_comboSet.InsertString(1,"0");
	m_comboSet.InsertString(2,"1");
	m_comboSet.InsertString(3,"2");
	m_comboSet.InsertString(4,"3");
	m_comboSet.InsertString(5,"4");
	m_comboSet.InsertString(6,"5");

	m_comboSet.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMousterStatFilterDlg::OnOK()
{
	CMousterStatDlg dlg;
	HTREEITEM hRoot = m_treeFilter.GetRootItem();
	while(hRoot)
	{
		INT id = m_treeFilter.GetItemData(hRoot);
		bool bCheck = m_treeFilter.GetCheck(hRoot) ? true : false;
		hRoot = m_treeFilter.GetNextItem(hRoot,TVGN_NEXT);
		dlg.mapFilter[id] = bCheck;
	}
	dlg.m_nSet = m_comboSet.GetCurSel() - 1;
	dlg.DoModal();
}

void CMousterStatFilterDlg::GetMapController(abase::vector<int> &listController)
{
	CMainFrame *pFrame = AUX_GetMainFrame();
	if(pFrame)
	{
		CElementMap *pMap = pFrame->GetMap();
		if(pMap==NULL) return; 
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		abase::vector<STAT_MOUNSETER> listMounster;
		
		BeginWaitCursor();
		
		APtrList<CSceneObject *>* al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_FIXED_NPC);
		ALISTPOSITION pos = al->GetTailPosition();
		while( pos )
		{
			CSceneFixedNpcGenerator* ptemp = (CSceneFixedNpcGenerator*)al->GetPrev(pos);
			if( !ptemp->IsDeleted() )
			{
				FIXED_NPC_DATA dat = ptemp->GetProperty();
				AddMapController(listController,dat.nControllerId);
			}
		}

		al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_AIAREA);
		pos = al->GetTailPosition();
		while( pos )
		{
			CSceneAIGenerator* ptemp = (CSceneAIGenerator*)al->GetPrev(pos);
			if( !ptemp->IsDeleted() )
			{
				AI_DATA dat = ptemp->GetProperty();
				AddMapController(listController,dat.nControllerId);
			}
		}

		al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_GATHER);
		pos = al->GetTailPosition();
		while( pos )
		{
			CSceneGatherObject* ptemp = (CSceneGatherObject*)al->GetPrev(pos);
			if( !ptemp->IsDeleted() )
			{
				GATHER_DATA dat = ptemp->GetProperty();
				AddMapController(listController,dat.nControllerId);
			}
		}
		
		al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_DYNAMIC);
		pos = al->GetTailPosition();
		while( pos )
		{
			CSceneDynamicObject* ptemp = (CSceneDynamicObject*)al->GetPrev(pos);
			if( !ptemp->IsDeleted() )
			{
				int did = ptemp->GetControllerID();
				AddMapController(listController,did);
			}
		}
	}
}

void CMousterStatFilterDlg::AddMapController(abase::vector<int> &listController,int id)
{
	for( int i = 0; i < listController.size(); ++i)
	{
		if(listController[i] == id) return;
	}
	listController.push_back(id);
}

bool CMousterStatFilterDlg::FindContoller(abase::vector<int> &listController,int id)
{
	for( int i = 0;i < listController.size(); ++i)
		if(id==listController[i]) return true;
	return false;
}

void CMousterStatFilterDlg::OnSelchangeComboSet() 
{
	// TODO: Add your control notification handler code here
	
}

void CMousterStatFilterDlg::OnSelectall() 
{
	HTREEITEM hItem = m_treeFilter.GetRootItem();
	if (!hItem){
		return;
	}
	BOOL fNewCheck = m_treeFilter.GetCheck(hItem) ? FALSE : TRUE;
	while(hItem)
	{
		m_treeFilter.SetCheck(hItem, fNewCheck);
		hItem = m_treeFilter.GetNextItem(hItem,TVGN_NEXT);
	}
}

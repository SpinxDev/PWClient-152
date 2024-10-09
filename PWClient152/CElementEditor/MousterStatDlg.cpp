// MousterStatDlg.cpp : implementation file
//

#include "Global.h"
#include "elementeditor.h"
#include "MousterStatDlg.h"
#include "ElementMap.h"
#include "MainFrm.h"
#include "SceneObjectManager.h"
#include "SceneAIGenerator.h"
#include "SceneGatherObject.h"
#include "SceneDynamicObject.h"


//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CMousterStatDlg dialog


CMousterStatDlg::CMousterStatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMousterStatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMousterStatDlg)
	//}}AFX_DATA_INIT

	m_nSet = 0;
}


void CMousterStatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMousterStatDlg)
	DDX_Control(pDX, IDC_LIST_REPORT, m_listReport);
	DDX_Control(pDX, IDC_TAB_SORT, m_TabSort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMousterStatDlg, CDialog)
	//{{AFX_MSG_MAP(CMousterStatDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SORT, OnSelchangeTabSort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMousterStatDlg message handlers
BOOL CMousterStatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_TabSort.InsertItem(0,"怪物统计");
	m_TabSort.InsertItem(1,"NPC统计");
	m_TabSort.InsertItem(2,"矿点统计");
	m_TabSort.InsertItem(3,"动态物品");

	m_listReport.SetExtendedStyle(m_listReport.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	BuildMounsterTable();
	StatMounster();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMousterStatDlg::BuildMounsterTable()
{
	m_listReport.DeleteAllItems();
	for( int i = 0; i < 4; ++i) m_listReport.DeleteColumn(0);
	
	m_listReport.InsertColumn(0, "怪物ID", LVCFMT_LEFT, 100);
	m_listReport.InsertColumn(1, "区域怪物数量", LVCFMT_LEFT, 100);
	m_listReport.InsertColumn(2, "定点怪物数量", LVCFMT_LEFT, 100);	
	m_listReport.InsertColumn(3, "怪物总数", LVCFMT_LEFT, 100);	
}

void CMousterStatDlg::BuildNpcTable()
{
	m_listReport.DeleteAllItems();
	for( int i = 0; i < 4; ++i) m_listReport.DeleteColumn(0);
	

	m_listReport.InsertColumn(0, "NpcID", LVCFMT_LEFT, 100);
	m_listReport.InsertColumn(1, "数量", LVCFMT_LEFT, 100);

}

void CMousterStatDlg::BuildGatherTable()
{
	m_listReport.DeleteAllItems();
	for( int i = 0; i < 4; ++i) m_listReport.DeleteColumn(0);
	
	m_listReport.InsertColumn(0, "矿ID", LVCFMT_LEFT, 100);
	m_listReport.InsertColumn(1, "数量", LVCFMT_LEFT, 100);
}

void CMousterStatDlg::BuildDynamicTable()
{
	m_listReport.DeleteAllItems();
	for( int i = 0; i < 4; ++i) m_listReport.DeleteColumn(0);
	
	m_listReport.InsertColumn(0, "动态物品ID", LVCFMT_LEFT, 100);
	m_listReport.InsertColumn(1, "数量", LVCFMT_LEFT, 100);	
}

void CMousterStatDlg::AddIDMounsterTable(abase::vector<STAT_MOUNSETER>& listTable, int id, int num, bool bFixed)
{
	for( int i = 0; i < listTable.size(); ++i)
	{
		if(listTable[i].nId == id) 
		{
			if(bFixed) listTable[i].nFixed +=num;
			else listTable[i].nArea +=num;
			return;
		}
	}
	STAT_MOUNSETER sm;
	sm.nId = id;
	if(bFixed) sm.nFixed += num;
	else sm.nArea += num;
	listTable.push_back(sm);
}

bool CMousterStatDlg::IsStat(int id)
{
	abase::hash_map<int,bool>::iterator it = mapFilter.find(id);
	if(it==mapFilter.end()) return false;
	bool r = it->second;
	return r;
}


void CMousterStatDlg::StatMounster()
{
	CMainFrame *pFrame = AUX_GetMainFrame();
	if(pFrame)
	{
		CElementMap *pMap = pFrame->GetMap();
		if(pMap==NULL) return; 
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		abase::vector<STAT_MOUNSETER> listMounster;
		
		BeginWaitCursor();
		
		APtrList<CSceneObject *>* al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_AIAREA);
		ALISTPOSITION pos = al->GetTailPosition();
		while( pos )
		{
			CSceneAIGenerator* ptemp = (CSceneAIGenerator*)al->GetPrev(pos);
			if( !ptemp->IsDeleted() && ptemp->GetCopyFlags()==m_nSet)
			{
				AI_DATA dat = ptemp->GetProperty();
				if(!IsStat(dat.nControllerId)) continue;
				int n = ptemp->GetNPCNum();
				for(int i = 0; i < n; i++)
				{
					NPC_PROPERTY* np = ptemp->GetNPCProperty(i);
					AddIDMounsterTable(listMounster,np->dwID,np->dwNum,false);
				}
			}
		}


		//for fixed mounster
		al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_FIXED_NPC);
		pos = al->GetTailPosition();
		while( pos )
		{
			CSceneFixedNpcGenerator* ptemp = (CSceneFixedNpcGenerator*)al->GetPrev(pos);
			if( !ptemp->IsDeleted()  && ptemp->GetCopyFlags()==m_nSet)
			{
				FIXED_NPC_DATA dat = ptemp->GetProperty();
				if(!IsStat(dat.nControllerId)) continue;
				if(dat.bIsMounster)
				{
					AddIDMounsterTable(listMounster,dat.dwID,1,true);
				}
			}
		}

		for( int i = 0; i < listMounster.size(); ++i)
		{
			CString strMsg;
			strMsg.Format("%d",listMounster[i].nId);
			int ps = m_listReport.InsertItem(0,strMsg);
			strMsg.Format("%d",listMounster[i].nArea);
			m_listReport.SetItemText(ps,1,strMsg);
			strMsg.Format("%d",listMounster[i].nFixed);
			m_listReport.SetItemText(ps,2,strMsg);
			strMsg.Format("%d",listMounster[i].nFixed + listMounster[i].nArea);
			m_listReport.SetItemText(ps,3,strMsg);
		}

		EndWaitCursor();
	}
}

void CMousterStatDlg::StatNpc()
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
			if( !ptemp->IsDeleted()  && ptemp->GetCopyFlags()==m_nSet)
			{
				FIXED_NPC_DATA dat = ptemp->GetProperty();
				if(!IsStat(dat.nControllerId)) continue;
				if(!dat.bIsMounster)
				{
					AddIDMounsterTable(listMounster,dat.dwID,1,true);
				}
			}
		}

		for( int i = 0; i < listMounster.size(); ++i)
		{
			CString strMsg;
			strMsg.Format("%d",listMounster[i].nId);
			int ps = m_listReport.InsertItem(0,strMsg);
			strMsg.Format("%d",listMounster[i].nFixed);
			m_listReport.SetItemText(ps,1,strMsg);
		}

		EndWaitCursor();
	}
}

void CMousterStatDlg::StatGather()
{
	CMainFrame *pFrame = AUX_GetMainFrame();
	if(pFrame)
	{
		CElementMap *pMap = pFrame->GetMap();
		if(pMap==NULL) return; 
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		abase::vector<STAT_MOUNSETER> listMounster;
		
		BeginWaitCursor();
		
		APtrList<CSceneObject *>* al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_GATHER);
		ALISTPOSITION pos = al->GetTailPosition();
		while( pos )
		{
			CSceneGatherObject* ptemp = (CSceneGatherObject*)al->GetPrev(pos);
			if( !ptemp->IsDeleted()  && ptemp->GetCopyFlags()==m_nSet)
			{
				GATHER_DATA dat = ptemp->GetProperty();
				if(!IsStat(dat.nControllerId)) continue;
				int n = ptemp->GetGatherNum();
				for(int i = 0; i < n; i++)
				{
					GATHER* np = ptemp->GetGather(i);
					AddIDMounsterTable(listMounster,np->dwId,np->dwNum,false);
				}
			}
		}

		for( int i = 0; i < listMounster.size(); ++i)
		{
			CString strMsg;
			strMsg.Format("%d",listMounster[i].nId);
			int ps = m_listReport.InsertItem(0,strMsg);
			strMsg.Format("%d",listMounster[i].nArea);
			m_listReport.SetItemText(ps,1,strMsg);
		}

		EndWaitCursor();
	}
}

void CMousterStatDlg::StatDynamic()
{
	CMainFrame *pFrame = AUX_GetMainFrame();
	if(pFrame)
	{
		CElementMap *pMap = pFrame->GetMap();
		if(pMap==NULL) return; 
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		abase::vector<STAT_MOUNSETER> listMounster;
		
		BeginWaitCursor();
		
		APtrList<CSceneObject *>* al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_DYNAMIC);
		ALISTPOSITION pos = al->GetTailPosition();
		while( pos )
		{
			CSceneDynamicObject* ptemp = (CSceneDynamicObject*)al->GetPrev(pos);
			if( !ptemp->IsDeleted() )
			{
				if(!IsStat(ptemp->GetControllerID())) continue;
				AddIDMounsterTable(listMounster,ptemp->GetDynamicObjectID(),1,true);
			}
		}
		
		for( int i = 0; i < listMounster.size(); ++i)
		{
			CString strMsg;
			strMsg.Format("%d",listMounster[i].nId);
			int ps = m_listReport.InsertItem(0,strMsg);
			strMsg.Format("%d",listMounster[i].nFixed);
			m_listReport.SetItemText(ps,1,strMsg);
		}

		EndWaitCursor();
	}
}

void CMousterStatDlg::OnSelchangeTabSort(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int n = m_TabSort.GetCurSel();
	switch(n)
	{
	case 0: BuildMounsterTable();StatMounster(); break;
	case 1: BuildNpcTable();StatNpc(); break;
	case 2: BuildGatherTable();StatGather(); break;
	case 3: BuildDynamicTable();StatDynamic(); break;
	}
	*pResult = 0;
}

// PrecinctNpcSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "PrecinctNpcSelDlg.h"
#include "MainFrm.h"
#include "SceneObjectManager.h"
#include "precinctImpDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CPrecinctNpcSelDlg dialog


CPrecinctNpcSelDlg::CPrecinctNpcSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrecinctNpcSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrecinctNpcSelDlg)
	//}}AFX_DATA_INIT
	m_pPrecinct = NULL;
}


void CPrecinctNpcSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrecinctNpcSelDlg)
	DDX_Control(pDX, IDC_LIST_PRECINCT_NPC, m_listPrecinctNpc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrecinctNpcSelDlg, CDialog)
	//{{AFX_MSG_MAP(CPrecinctNpcSelDlg)
	ON_BN_CLICKED(ID_DEL_POS, OnDelPos)
	ON_BN_CLICKED(ID_DEL_POS2, OnDelPos2)
	ON_BN_CLICKED(IDC_PRECINCT_ADD_POS, OnPrecinctAddPos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrecinctNpcSelDlg message handlers

BOOL CPrecinctNpcSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FreshPrecinctList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CPrecinctNpcSelDlg::FreshPrecinctList()
{
	int num = m_listPrecinctNpc.GetCount();
	for(int k = 0; k < num; k++)
		m_listPrecinctNpc.DeleteString(0);
	
	if(m_pPrecinct)
	{
		int n = m_pPrecinct->GetNpcNum();
		for( int i = 0; i <  n; i++)
		{
			PRECINCT_NPC *pNpc = m_pPrecinct->GetNpc(i);
			m_listPrecinctNpc.AddString(pNpc->strName);
		}
	}
}

void CPrecinctNpcSelDlg::OnDelPos() 
{
	// TODO: Add your control notification handler code here
	if(m_pPrecinct)
	{
		int index = m_listPrecinctNpc.GetCurSel();
		if(index != -1)
		{
			m_pPrecinct->DeleteNpc(index);
			FreshPrecinctList();
		}
	}
}

void CPrecinctNpcSelDlg::OnDelPos2() 
{
	// TODO: Add your control notification handler code here
	
	int index = m_listPrecinctNpc.GetCurSel();
	if(m_pPrecinct==NULL || index == -1) return;
	
	PRECINCT_NPC* pNpc = m_pPrecinct->GetNpc(index);
	if(pNpc==NULL) return;
	
	CPrecinctImpDlg dlg;
	dlg.m_strName = pNpc->strName;
	dlg.m_fX = pNpc->vPos.x;
	dlg.m_fY = pNpc->vPos.y;
	dlg.m_fZ = pNpc->vPos.z;
	if(IDOK==dlg.DoModal())
	{
		if(dlg.m_strName.IsEmpty()) 
		{
			MessageBox("名字不能为空!");
			return;
		}
		pNpc->strName = dlg.m_strName;
		pNpc->vPos = A3DVECTOR3(dlg.m_fX,dlg.m_fY,dlg.m_fZ);
		FreshPrecinctList();
	}
}
void CPrecinctNpcSelDlg::OnPrecinctAddPos() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CPrecinctImpDlg dlg;
	if(IDOK==dlg.DoModal())
	{
		if(dlg.m_strName.IsEmpty()) 
		{
			MessageBox("名字不能为空!");
			return;
		}
		PRECINCT_NPC* pNpc = new PRECINCT_NPC;
		pNpc->strName = dlg.m_strName;
		pNpc->vPos = A3DVECTOR3(dlg.m_fX,dlg.m_fY,dlg.m_fZ);
		m_pPrecinct->AddNpc(pNpc);
		FreshPrecinctList();
	}
}

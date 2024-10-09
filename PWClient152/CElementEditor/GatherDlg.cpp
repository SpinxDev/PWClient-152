// GatherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "GatherDlg.h"
#include "GatherPropertyDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGatherDlg dialog


CGatherDlg::CGatherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGatherDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGatherDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pGather = NULL;
}


void CGatherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGatherDlg)
	DDX_Control(pDX, IDC_GATHER_LIST, m_listGather);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGatherDlg, CDialog)
	//{{AFX_MSG_MAP(CGatherDlg)
	ON_BN_CLICKED(ID_GATHER_ADD, OnGatherAdd)
	ON_BN_CLICKED(ID_GATHER_MODIFY, OnGatherModify)
	ON_BN_CLICKED(ID_GATHER_DEL, OnGatherDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGatherDlg message handlers

BOOL CGatherDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_pGather)
	{
		int num = m_pGather->GetGatherNum();
		for(int i = 0; i < num; i++)
		{
			GATHER *pProperty = m_pGather->GetGather(i);
			m_listGather.AddString(pProperty->strName);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGatherDlg::OnGatherAdd() 
{
	// TODO: Add your control notification handler code here
	CGatherPropertyDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		GATHER *pnew = m_pGather->GetGather(dlg.m_data.strName);
		if(pnew)
		{
			MessageBox("该类型的采集对象已经存在于采集区域中，加入失败!");
			return;
		}
		GATHER* g = new GATHER;
		*g = dlg.m_data;
		m_pGather->AddGather(g);
		FreshList();
	}
}

void CGatherDlg::OnGatherModify() 
{
	// TODO: Add your control notification handler code here
	CGatherPropertyDlg dlg;
	int index = m_listGather.GetCurSel();
	if(index==-1) return;
	CString name;
	m_listGather.GetText(index,name);
	GATHER* pro = m_pGather->GetGather(AString(name));
	
	CString str;
	dlg.m_bModified = true;
	dlg.m_data = *pro;
	if(dlg.DoModal()==IDOK)
	{
		GATHER *pnew = m_pGather->GetGather(dlg.m_data.strName);
		if(pnew)
		{
			if(pnew!=pro)
			{
				MessageBox("该类型的怪物已经加入到怪物产生器中，修改无效!");
				return;
			}
		}
		*pro = dlg.m_data;
		FreshList();
	}
}

void CGatherDlg::OnGatherDel() 
{
	// TODO: Add your control notification handler code here
	int index = m_listGather.GetCurSel();
	if(index==-1) return;
	m_pGather->DeleteGather(index);
	FreshList();
}

void CGatherDlg::FreshList()
{
	int num = m_listGather.GetCount();
	 for(int i = 0; i < num; i++)
		m_listGather.DeleteString(0);

	if(m_pGather)
	{
		int num = m_pGather->GetGatherNum();
		for(int i = 0; i < num; i++)
		{
			GATHER *pProperty = m_pGather->GetGather(i);
			m_listGather.AddString(pProperty->strName);
		}
	}
}

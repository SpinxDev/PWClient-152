// TaskRegionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "TaskRegionDlg.h"
#include "RegionPointDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskRegionDlg dialog


CTaskRegionDlg::CTaskRegionDlg(Task_Region* pRegion,unsigned long& ulCount,CWnd* pParent /*=NULL*/)
	: CDialog(CTaskRegionDlg::IDD, pParent)
	,m_pRegion(pRegion)
	,m_ulCount(ulCount)
{
	//{{AFX_DATA_INIT(CTaskRegionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTaskRegionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskRegionDlg)
	DDX_Control(pDX, IDC_LST_REGION, m_lstRegion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskRegionDlg, CDialog)
	//{{AFX_MSG_MAP(CTaskRegionDlg)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskRegionDlg message handlers

BOOL CTaskRegionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_lstRegion.InsertColumn(0, "序号", LVCFMT_LEFT, 50);
	m_lstRegion.InsertColumn(1, "最小点", LVCFMT_LEFT, 165);
	m_lstRegion.InsertColumn(2, "最大点", LVCFMT_LEFT, 165);
	
	m_lstRegion.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	UpdateList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CTaskRegionDlg::UpdateList()
{
	m_lstRegion.DeleteAllItems();
	unsigned long i;
	
	for (i=0;i<m_ulCount;i++)
	{
		CString str;
		Task_Region& t = m_pRegion[i];
		
		str.Format("%u",i);
		m_lstRegion.InsertItem(i,str);
		str.Format("%.1f,%.1f,%.1f",t.zvMin.x,t.zvMin.y,t.zvMin.z);
		m_lstRegion.SetItemText(i,1,str);

		str.Format("%.1f,%.1f,%.1f",t.zvMax.x,t.zvMax.y,t.zvMax.z);
		m_lstRegion.SetItemText(i,2,str);
	}	
}

void CTaskRegionDlg::OnBtnAdd() 
{
	if(m_ulCount>= MAX_TASKREGION)
	{
		AfxMessageBox("超出区域个数!",MB_OK);
		return;
	}
	
	int iCount = m_lstRegion.GetItemCount();
	
	ASSERT(iCount == m_ulCount);	
	
	Task_Region& t = m_pRegion[m_ulCount];
	
	CRegionPointDlg dlg;
	if (dlg.DoModal()==IDOK)
	{
		t.zvMin.x = dlg.m_fX;
		t.zvMin.y = dlg.m_fY;
		t.zvMin.z = dlg.m_fZ;
		
		t.zvMax.x = dlg.m_fX2;
		t.zvMax.y = dlg.m_fY2;
		t.zvMax.z = dlg.m_fZ2;
		
		m_ulCount++;
	}
	
	UpdateList();
	
}

void CTaskRegionDlg::OnBtnDel() 
{
	if (AfxMessageBox("要删除吗？",MB_OKCANCEL)==IDOK)
	{
		POSITION pos = m_lstRegion.GetFirstSelectedItemPosition();
		if (!pos) return;
		
		int nIndex = m_lstRegion.GetNextSelectedItem(pos);
		
		unsigned long i = (unsigned long)nIndex + 1;
		
		for (; i < m_ulCount; i++)
		{
			m_pRegion[i-1].zvMin = m_pRegion[i].zvMin;
			m_pRegion[i-1].zvMax = m_pRegion[i].zvMax;
		}
		
		m_ulCount--;

		UpdateList();
	}
	
}

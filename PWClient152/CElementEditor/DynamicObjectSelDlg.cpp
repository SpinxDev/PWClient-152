// DynamicObjectSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "DynamicObjectSelDlg.h"
#include "DynamicObjectManDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynamicObjectSelDlg dialog


CDynamicObjectSelDlg::CDynamicObjectSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDynamicObjectSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDynamicObjectSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nDyObjID = 0;
}


void CDynamicObjectSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDynamicObjectSelDlg)
	DDX_Control(pDX, IDC_LIST_DY_OBJ, m_listObj);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDynamicObjectSelDlg, CDialog)
	//{{AFX_MSG_MAP(CDynamicObjectSelDlg)
	ON_BN_CLICKED(ID_SET_NULL, OnSetNull)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicObjectSelDlg message handlers


BOOL CDynamicObjectSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int n = g_DynamicObjectMan.GetIdNum();
	int index = -1;
	int idx = 0;
	for( int i = 0; i < n; ++i)
	{
		DYNAMIC_OBJECT obj;
		InitDynamicObj(&obj);
		g_DynamicObjectMan.GetObjByIndex(i,obj);
		if(obj.id!=0)
		{
			char name[256];
			sprintf(name,"[%d]%s(%s)",obj.id,obj.szName,obj.szObjectPath);
			int sel = m_listObj.InsertString(idx++,name);
			m_listObj.SetItemData(sel,obj.id);
			if(obj.id == m_nDyObjID) index = sel;
		}
	}
	if(index!=-1) m_listObj.SetCurSel(index);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDynamicObjectSelDlg::OnOK()
{
	int sel = m_listObj.GetCurSel();
	if(sel==-1) CDialog::OnCancel();
	m_nDyObjID = m_listObj.GetItemData(sel);

	CDialog::OnOK();
}

void CDynamicObjectSelDlg::OnSetNull() 
{
	// TODO: Add your control notification handler code here
	m_nDyObjID = 0;
	CDialog::OnOK();
}

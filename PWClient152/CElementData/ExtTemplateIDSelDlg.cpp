// ExtTemplateIDSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "ExtTemplateIDSelDlg.h"
#include "BaseDataIDMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ExtTemplateIDSelDlg dialog


ExtTemplateIDSelDlg::ExtTemplateIDSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ExtTemplateIDSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ExtTemplateIDSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nExtID = 0;
}


void ExtTemplateIDSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ExtTemplateIDSelDlg)
	DDX_Control(pDX, IDC_LIST_TEMPLATE_EXT, m_listExt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ExtTemplateIDSelDlg, CDialog)
	//{{AFX_MSG_MAP(ExtTemplateIDSelDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ExtTemplateIDSelDlg message handlers

BOOL ExtTemplateIDSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int sel = 0;

	abase::vector<AString> pathArray;
	g_ExtBaseIDMan.GeneratePathArray(pathArray);
	m_listExt.InsertString(0,"(ÎÞ)");
	for( size_t i = 1; i < pathArray.size() + 1; ++i)
	{
		CString path = pathArray[i-1];
		int id = g_ExtBaseIDMan.GetIDByPath(AString(path));
		if(m_nExtID !=0 )
		{
			if(id==m_nExtID) sel = i;
		}
		
		path.Format("[%d]%s",id,pathArray[i-1]);
		m_listExt.InsertString(i,path);
	}
	
	if(sel!=-1) m_listExt.SetCurSel(sel);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ExtTemplateIDSelDlg::OnOK()
{
	int sel = m_listExt.GetCurSel();
	if( sel != -1)
	{
		if(sel==0) 
		{
			m_nExtID = 0;
		}else
		{
			CString path;
			m_listExt.GetText(sel,path);
			int pos = path.Find(']');
			int len = path.GetLength();
			if(pos!=-1)
			{
				path = path.Right(len - pos - 1);
				int id = g_ExtBaseIDMan.GetIDByPath(AString(path));
				if(id!=0) m_nExtID = id;
			}
		}
		CDialog::OnOK();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK SEL_EXT_FUNCS::OnActivate(void)
{
	ExtTemplateIDSelDlg dlg;
	dlg.m_nExtID = m_var;
	if(dlg.DoModal()==IDOK)
	{
		m_var = dlg.m_nExtID;
		CString path = g_ExtBaseIDMan.GetPathByID(dlg.m_nExtID);
		if(dlg.m_nExtID==0) m_strText = "(ÎÞ)";
		else m_strText.Format("%d(%s)",dlg.m_nExtID,path);
	}
	return TRUE;
}

LPCTSTR CALLBACK SEL_EXT_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK SEL_EXT_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK SEL_EXT_FUNCS::OnSetValue(const AVariant& var)
{
	m_var = var;
	int id = int(m_var);
	if(id==-1) m_strText = "(´íÎó)";
	else if(id==0) m_strText = "(ÎÞ)";
	else 
	{
		CString path = g_ExtBaseIDMan.GetPathByID(id);
		m_strText.Format("%d(%s)",id,path);
	}
}

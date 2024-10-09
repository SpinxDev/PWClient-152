// PolicyTreeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "PolicyTreeDlg.h"
#include "BaseDataIDMan.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPolicyTreeDlg dialog
#define		IDC_TREE_ITEM_POLICY	1		


CPolicyTreeDlg::CPolicyTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPolicyTreeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPolicyTreeDlg)
		// NOTE: the ClassWizard will add member initialization here
	m_strSelPolicyPath = "";
	//}}AFX_DATA_INIT
}


void CPolicyTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPolicyTreeDlg)
	DDX_Control(pDX, IDC_TREE_POLICY, m_Tree);
	//}}AFX_DATA_MAP
}

BOOL CPolicyTreeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	HTREEITEM hRoot = m_Tree.InsertItem("¹ÖÎï²ßÂÔ",NULL,TVI_LAST);
	m_Tree.SetItemData(hRoot,0);
	EnumPolicy(hRoot);

	return TRUE;
}

void CPolicyTreeDlg::OnOK()
{
	HTREEITEM hSel = m_Tree.GetSelectedItem();
	if(hSel)
	{
		int nType = (int)m_Tree.GetItemData(hSel);
		if(nType == IDC_TREE_ITEM_POLICY)
		{
			m_strSelPolicyPath = m_Tree.GetItemText(hSel);
			CString strPath(g_szWorkDir);
			m_strSelPolicyPath = strPath + m_strSelPolicyPath;
		}
	}

	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CPolicyTreeDlg, CDialog)
	//{{AFX_MSG_MAP(CPolicyTreeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPolicyTreeDlg message handlers
void	CPolicyTreeDlg::EnumPolicy(HTREEITEM hRoot)
{
	abase::vector<AString> aPathArray;
	g_PolicyIDMan.GeneratePathArray(aPathArray);
	int nSize = (int)aPathArray.size();
	for (int i=0;i<nSize;i++)
	{
		AString& strName = aPathArray[i];
		HTREEITEM hNewItem = m_Tree.InsertItem(strName,hRoot,TVI_LAST);
		m_Tree.SetItemData(hNewItem,IDC_TREE_ITEM_POLICY);
	}

	m_Tree.Expand(hRoot,TVE_EXPAND);
}
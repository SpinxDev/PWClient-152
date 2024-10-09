// TemplateExtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "TemplateExtDlg.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTemplateExtDlg dialog


CTemplateExtDlg::CTemplateExtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateExtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateExtDlg)
	m_strName = _T("");
	m_strItemName1 = _T("");
	m_strItemName2 = _T("");
	m_strItemName3 = _T("");
	m_strItemName4 = _T("");
	//}}AFX_DATA_INIT
	m_nItemNum = 1;
	m_bModified  = false;
}


void CTemplateExtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateExtDlg)
	DDX_Control(pDX, IDC_EDIT_NAME, m_EditName);
	DDX_Control(pDX, IDC_COMBO_EXT_ITEM5, m_cExtItem1);
	DDX_Control(pDX, IDC_COMBO_EXT_NUM, m_cExtItemNum);
	DDX_Control(pDX, IDC_COMBO_EXT_ITEM4, m_cExtItem4);
	DDX_Control(pDX, IDC_COMBO_EXT_ITEM3, m_cExtItem3);
	DDX_Control(pDX, IDC_COMBO_EXT_ITEM2, m_cExtItem2);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_NAME1, m_strItemName1);
	DDX_Text(pDX, IDC_EDIT_NAME2, m_strItemName2);
	DDX_Text(pDX, IDC_EDIT_NAME3, m_strItemName3);
	DDX_Text(pDX, IDC_EDIT_NAME4, m_strItemName4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateExtDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplateExtDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_TEMPLATE_LIST, OnSelchangeComboTemplateList)
	ON_CBN_SELCHANGE(IDC_COMBO_EXT_NUM, OnSelchangeComboExtNum)
	ON_CBN_SELCHANGE(IDC_COMBO_EXT_ITEM2, OnSelchangeComboExtItem2)
	ON_CBN_SELCHANGE(IDC_COMBO_EXT_ITEM3, OnSelchangeComboExtItem3)
	ON_CBN_SELCHANGE(IDC_COMBO_EXT_ITEM4, OnSelchangeComboExtItem4)
	ON_CBN_SELCHANGE(IDC_COMBO_EXT_ITEM5, OnSelchangeComboExtItem5)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateExtDlg message handlers

void CTemplateExtDlg::OnSelchangeComboTemplateList() 
{
	// TODO: Add your control notification handler code here
	//int item = m_comboTemplate.GetCurSel();
	//m_comboTemplate.GetLBText(item,m_strTemplateName);
	//MessageBox(m_strTemplateName);
}

int TranslateType(const char *szType)
{
	CString type[3];
	type[0] = "int";
	type[1] = "float";
	type[2] = "uint";
	for(int i = 0; i< 3; i++)
	if(stricmp(szType,type[i])==0)
		return i;
	return 0;
}

BOOL CTemplateExtDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	CString type[3];
	type[0] = "int";
	type[1] = "float";
	type[2] = "uint";
	
	int i(0);
	for(i= 0; i< 4; i++)
		m_strItemType[i] = "int";

	for(i=1; i<4; i++)
	{
		CString str;
		str.Format("%d",i);
		m_cExtItemNum.AddString(str);
	}

	for(i=0; i<3;i++)
	{
		m_cExtItem1.AddString(type[i]);
	}
	for(i=0; i<3;i++)
	{
		m_cExtItem2.AddString(type[i]);
	}
	for(i=0; i<3;i++)
	{
		m_cExtItem3.AddString(type[i]);
	}
	for(i=0; i<3;i++)
	{
		m_cExtItem4.AddString(type[i]);
	}

	m_cExtItemNum.SetCurSel(0);
	m_cExtItem1.SetCurSel(0);
	m_cExtItem2.SetCurSel(0);
	m_cExtItem3.SetCurSel(0);
	m_cExtItem4.SetCurSel(0);
	m_nItemNum = 1;
	
	if(m_bModified)
	{
		LoadExt(m_strDir);
		m_strName = m_extTmp.GetName();
		m_nItemNum = m_extTmp.GetItemNum();
		if(m_nItemNum>4) m_nItemNum = 3;

		m_cExtItemNum.SetCurSel(m_nItemNum-1);
		
		CWnd *pWnd = GetDlgItem(IDC_EDIT_NAME);
		pWnd->EnableWindow(false);

		for(int k = 0; k < m_nItemNum; k++)
		{
			AString tempType = m_extTmp.GetItemType(k);
			if(k==0) 
			{
				m_cExtItem1.SetCurSel(TranslateType(tempType));
				m_strItemName1 = m_extTmp.GetItemName(k);
			}
			if(k==1) 
			{
				m_cExtItem2.SetCurSel(TranslateType(tempType));
				m_strItemName2 = m_extTmp.GetItemName(k);
			}
			if(k==2) 
			{
				m_cExtItem3.SetCurSel(TranslateType(tempType));
				m_strItemName3 = m_extTmp.GetItemName(k);
			}
			if(k==3) 
			{
				m_cExtItem4.SetCurSel(TranslateType(tempType));
				m_strItemName4 = m_extTmp.GetItemName(k);
			}
		}
	
		for(int i = m_extTmp.GetItemNum()-1; i>=0 ; i--)
		{
			m_extTmp.RemoveAt(i);
		}
		UpdateData(false);
	}
	DisableItem(m_nItemNum);

	m_EditName.SetFocus();
	return FALSE;
	//return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CTemplateExtDlg::EnumTemplates()
{
}

void CTemplateExtDlg::OnSelchangeComboExtNum() 
{
	// TODO: Add your control notification handler code here
	int item = m_cExtItemNum.GetCurSel();
	CString num;
	m_cExtItemNum.GetLBText(item,num);
	m_nItemNum = (int)atoi(num);
	DisableItem(m_nItemNum);
}

void CTemplateExtDlg::DisableItem(int num)
{
	CWnd *pWnd = NULL;
	CWnd *pWnd2 = NULL;
	DWORD id[4] = { IDC_COMBO_EXT_ITEM5,IDC_COMBO_EXT_ITEM2,IDC_COMBO_EXT_ITEM3,IDC_COMBO_EXT_ITEM4};
	DWORD id2[4] = { IDC_EDIT_NAME1,IDC_EDIT_NAME2,IDC_EDIT_NAME3,IDC_EDIT_NAME4};
	for(int i=0; i<4; i++)
	{
		pWnd = GetDlgItem(id[i]);
		pWnd2 = GetDlgItem(id2[i]);
		if(i<num) 
		{
			pWnd->EnableWindow(true);
			pWnd2->EnableWindow(true);
		}
		else 
		{
			pWnd->EnableWindow(false);
			pWnd2->EnableWindow(false);
		}
	}
}

void CTemplateExtDlg::OnSelchangeComboExtItem2() 
{
	// TODO: Add your control notification handler code here
	int item = m_cExtItem2.GetCurSel();
	m_cExtItem2.GetLBText(item,m_strItemType[1]);
}

void CTemplateExtDlg::OnSelchangeComboExtItem3() 
{
	// TODO: Add your control notification handler code here
	int item = m_cExtItem3.GetCurSel();
	m_cExtItem3.GetLBText(item,m_strItemType[2]);
}

void CTemplateExtDlg::OnSelchangeComboExtItem4() 
{
	// TODO: Add your control notification handler code here
	int item = m_cExtItem4.GetCurSel();
	m_cExtItem4.GetLBText(item,m_strItemType[3]);
}

void CTemplateExtDlg::OnSelchangeComboExtItem5() 
{
	// TODO: Add your control notification handler code here
	int item = m_cExtItem1.GetCurSel();
	m_cExtItem1.GetLBText(item,m_strItemType[0]);	
}

void CTemplateExtDlg::LoadExt(CString pathName)
{
	if(m_bModified)
	{
		if(0 != m_extTmp.Load(pathName,false))
		{
			MessageBox("¼ÓÔØÎÄ¼þÊ§°Ü");
		}
	}
}

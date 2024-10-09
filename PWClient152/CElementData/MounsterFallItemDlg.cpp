// MounsterFallItem.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "MounsterFallItemDlg.h"
#include "PropertyList.h"
#include "AObject.h"
#include "TemplIDSelDlg.h"
#include "TemplIDSelDlgEx.h"
#include "BaseDataIDMan.h"
#include "BaseDataTemplate.h"
#include "ExtendDataTempl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMounsterFallItemDlg dialog


CMounsterFallItemDlg::CMounsterFallItemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMounsterFallItemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMounsterFallItemDlg)
	m_strName = _T("");
	//}}AFX_DATA_INIT
	m_pItem = 0;
	m_pOldItem = 0;
	m_bModified = false;
	m_bChanged = false;
	m_bReadOnly = false;
}


void CMounsterFallItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMounsterFallItemDlg)
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cbType);
	DDX_Control(pDX, IDC_LIST_MOUNSTER, m_listMounster);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMounsterFallItemDlg, CDialog)
	//{{AFX_MSG_MAP(CMounsterFallItemDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_DEL_ALL, OnDelAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMounsterFallItemDlg message handlers

BOOL CMounsterFallItemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here

	if(m_pItem==0)
	{
		m_pItem = new CMounsterFallItem;
		m_pItem->SetItemType(1);
		ITEM_ELEMENT *pItem = m_pItem->GetElementList();
		float *pPro =  m_pItem->GetProbabilityList();
		int i(0);
		for( i = 0; i < 256; i++)
		{
			pItem[i].fProbability = 0.0f;
			pItem[i].id = 0;
		}
		for( i = 0; i < 8; i++)
			pPro[i] = 0.0f;
		m_pItem->SetItemName("test");
	}else 
	{
		m_bModified = true;
		m_pOldItem = m_pItem->Copy();
		FreshMounsterList();
	}

	m_pList = new CPropertyList();
	m_pProperty = new ADynPropertyObject;
	
	CWnd *pWnd = GetDlgItem(IDC_LIST_ITEM);
	CRect rc,rc1;
	pWnd->GetClientRect(&rc);

	int width = rc.Width();
	int length = rc.Height();

	pWnd->GetWindowRect(&rc);
	GetWindowRect(&rc1);
	rc.left = rc.left - rc1.left;
	rc.top = rc.top - rc1.top - 20;
	rc.right = rc.left + width;
	rc.bottom = rc.top + length;

	BOOL bResult = m_pList->Create("掉落表", WS_CHILD | WS_VISIBLE, rc, this, 111);
	
	char name[128];
	
	ITEM_ELEMENT *pItem = m_pItem->GetElementList();
	float *pPro = m_pItem->GetProbabilityList();
	DWORD flag = 0;
	if(m_bReadOnly) flag = WAY_READONLY;
	int i(0);
	for( i = 0; i < 8; i++)
	{
		sprintf(name,"掉落%d个物品概率",i);
		m_pProperty->DynAddProperty(AVariant(pPro[i]), name,NULL,NULL,flag);
	}
	
	for(i = 0; i < 256; i++)
	{
		CTemplIDSel* _IDSel = new CTemplIDSel;
		m_pListIDSel[i] = _IDSel;
		_IDSel->Init(
			"BaseData",
			".tmpl",
			AVariant(0),
			enumBaseID);
		sprintf(name,"掉落物品%d",i);
		m_pProperty->DynAddProperty(AVariant(pItem[i].id), name, (ASet*)_IDSel->GetFuncsPtr(), NULL, WAY_CUSTOM | flag);
		sprintf(name,"物品%d掉落概率",i);
		m_pProperty->DynAddProperty(AVariant(pItem[i].fProbability), name,NULL,NULL,flag);
	}
	UpdateItemData(false);
	
	m_cbType.AddString("完全替代");//1
	m_cbType.AddString("同时生效");//2
	if(m_pItem->GetItemType()==1) m_cbType.SetCurSel(0);
	else if(m_pItem->GetItemType()==2) m_cbType.SetCurSel(1);
	if(m_bReadOnly) 
	{
		CWnd *pEdit = GetDlgItem(IDC_EDIT_NAME);
		pEdit->EnableWindow(FALSE);
		m_cbType.EnableWindow(false);
	}
	m_strName = m_pItem->GetItemName();
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMounsterFallItemDlg::UpdateItemData(bool bGet)
{
	m_pList->UpdateData(true);
	int index = 8;
	ITEM_ELEMENT *pItem = m_pItem->GetElementList();
	float *pPro = m_pItem->GetProbabilityList();
	int i(0);
	if(bGet)
	{
		for( i = 0; i < 8; ++i)
			pPro[i] = m_pProperty->GetPropVal(i);
		
		for( i = 0; i < 256; ++i)
		{
			pItem[i].id = m_pProperty->GetPropVal(index++);//id
			pItem[i].fProbability = m_pProperty->GetPropVal(index++);//per
		}
	}else
	{
		for( i = 0; i < 8; ++i)
			m_pProperty->SetPropVal(i,pPro[i]);

		for( i = 0; i < 256; ++i)
		{
			m_pProperty->SetPropVal(index++,pItem[i].id);
			m_pListIDSel[i]->SetValue(AVariant(pItem[i].id));
			m_pProperty->SetPropVal(index++,pItem[i].fProbability);
		}

	}
	if(m_pList && m_pProperty) m_pList->AttachDataObject(m_pProperty);
}

void CMounsterFallItemDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
	if(m_pList)
	{
		m_pList->CleanItems();
		m_pList->DestroyWindow();
		delete m_pList;
	}
	if(m_pProperty) delete m_pProperty;

	for( int i = 0; i < 256; i++)
		delete m_pListIDSel[i];
}

void CMounsterFallItemDlg::FreshMounsterList()
{
	int n = m_listMounster.GetCount();
	int i(0);
	for( i = 0; i < n; ++i) m_listMounster.DeleteString(0);

	n = m_pItem->GetMounsterNum();
	for( i = 0; i < n; ++i)
	{
		m_listMounster.AddString(GetMounsterName(m_pItem->GetMounsterID(i)));
	}
}

CString CMounsterFallItemDlg::GetMounsterName(unsigned int id)
{
	unsigned long ulID = id;

	CString str;
	if (ulID != 0)
	{
		str = g_BaseIDMan.GetPathByID(ulID);
		if (!str.IsEmpty())
		{
			BaseDataTempl tmpl;
			if (tmpl.Load(str) == 0)
			{
				str = tmpl.GetName();
				tmpl.Release();
				return str;
			}
		}
		else
		{
			str.Format("（错误的ID = %d）", ulID);
			return str;
		}
	}
	else
		str = "（无）";
	return str;
}

void CMounsterFallItemDlg::OnAdd() 
{
	// TODO: Add your control notification handler code here
	if(m_pItem && !m_bReadOnly)
	{
		CTemplIDSelDlgEx dlg;
		dlg.m_strRoot = "BaseData\\怪物";
		dlg.m_strExt = ".tmpl";
		dlg.m_strInitPath = "BaseData\\怪物";
		if(IDOK==dlg.DoModal())
		{
			int n = dlg.listID.size();
			for( int i = 0; i < n; i++)
				m_pItem->AddMounster(dlg.listID[i]);
			FreshMounsterList();
			m_bChanged = true;
		}
	}
}

void CMounsterFallItemDlg::OnDel() 
{
	// TODO: Add your control notification handler code here
	int sel = m_listMounster.GetCurSel();
	if(sel!=-1 && !m_bReadOnly)
	{
		if(AfxMessageBox("你确定要删除指定元素吗？",MB_YESNO|MB_ICONQUESTION)!=IDYES) return;
		m_pItem->DeleteMounster(sel);
		m_bChanged = true;
		FreshMounsterList();
	}
}

void CMounsterFallItemDlg::OnModified() 
{
	// TODO: Add your control notification handler code here
	
}

void CMounsterFallItemDlg::OnOK()
{
	UpdateData(true);
	UpdateItemData(true);
	if(m_strName.IsEmpty()) 
	{
		MessageBox("名字不能为空!");
		return;
	}
	int type = m_cbType.GetCurSel();
	if(type==-1) type = 0;
	type +=1;
	m_pItem->SetItemType(type);
	m_pItem->SetItemName((LPCSTR)m_strName);
	if(m_bModified)
	{
		delete m_pOldItem;
	}else
	{
	}
	m_bChanged = true;
	CDialog::OnOK();
}

void CMounsterFallItemDlg::OnCancel()
{
	if(m_bModified)
	{
		delete m_pItem;
		m_pItem = m_pOldItem;
	}else
	{
		delete m_pItem;
	}
	m_bChanged = false;
	CDialog::OnCancel();
}

void CMounsterFallItemDlg::OnDelAll() 
{
	// TODO: Add your control notification handler code here
	if(m_bReadOnly) return;
	if(AfxMessageBox("你确定要删除所有列表中的怪物？",MB_YESNO|MB_ICONQUESTION)!=IDYES) return;
	
	int n = m_pItem->GetMounsterNum();
	for( int i = 0; i < n; i++)
		m_pItem->DeleteMounster(0);
	m_bChanged = true;
	FreshMounsterList();
}

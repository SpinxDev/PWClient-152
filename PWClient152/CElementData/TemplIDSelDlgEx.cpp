// TemplIDSelDlgEx.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "TemplIDSelDlgEx.h"
#include "BaseDataIDMan.h"
#include "BaseDataTemplate.h"
#include "ExtendDataTempl.h"
#include "AMemory.h"
#include "Global.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplIDSelDlgEx dialog


CTemplIDSelDlgEx::CTemplIDSelDlgEx(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplIDSelDlgEx::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplIDSelDlgEx)
		// NOTE: the ClassWizard will add member initialization here
	m_strTarget = _T("");
	//}}AFX_DATA_INIT
}


void CTemplIDSelDlgEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplIDSelDlgEx)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_TREE_TEMPL, m_IDTree);
	DDX_Text(pDX, IDC_TARGET, m_strTarget);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplIDSelDlgEx, CDialog)
	//{{AFX_MSG_MAP(CTemplIDSelDlgEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TEMPL, OnSelchangedTreeTempl)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplIDSelDlgEx message handlers
BOOL CTemplIDSelDlgEx::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ImageList.Create(IDB_LISTTAG, 16, 3, RGB(255, 0, 255));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDER));
    m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_OBJECT));
	m_IDTree.SetImageList(&m_ImageList,TVSIL_NORMAL);

	m_hNull = m_IDTree.InsertItem("£¨ÎÞ£©", TVI_ROOT);
	m_IDTree.SetItemData(m_hNull, TRUE);
	EnumTempl(m_strRoot, TVI_ROOT);
	m_IDTree.Expand(TVI_ROOT, TVE_EXPAND);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTemplIDSelDlgEx::EnumTempl(const CString& strPath, HTREEITEM hRoot)
{
	CFileFind finder;
	CString strFile = strPath + _T("\\*.*");

	BOOL bWorking = finder.FindFile(strFile);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots()) continue;
		else if (finder.IsDirectory())
		{
			CString strDir = strPath + _T("\\") + finder.GetFileName();
			for (ORG_PATHS::iterator it = _org_paths.begin(); it != _org_paths.end(); ++it)
			{
				if (strstr(it->first, strDir) != NULL)
				{
					HTREEITEM h = m_IDTree.InsertItem(finder.GetFileName(), hRoot);
					m_IDTree.SetItemImage(h, 0, 0);
					EnumTempl(strDir, h);
					break;
				}
				else if (strstr(strDir, it->first + "\\") != NULL)
				{
					CString strTmpl = strDir + "\\" + finder.GetFileName() + ".tmpl";
					if (GetFileAttributes(strTmpl) == (DWORD)-1) continue;
					CString strRemain = strDir.Mid(it->first.GetLength());
					int nLevel = 0;
					const char* szLevel = strchr(strRemain, '\\');
					while (szLevel)
					{
						nLevel++;
						szLevel = strchr(szLevel+1, '\\');
					}
					HTREEITEM h = m_IDTree.InsertItem(finder.GetFileName(), hRoot);
					m_IDTree.SetItemData(h, TRUE);
					if (nLevel == it->second)
						m_IDTree.SetItemImage(h, 4, 4);
					else
						m_IDTree.SetItemImage(h, 2, 2);
					if (m_strInitPath.CompareNoCase(strTmpl) == 0)
						m_IDTree.Select(h, TVGN_CARET);
					EnumTempl(strDir, h);
					break;
				}
			}
		}
	}
}

void CTemplIDSelDlgEx::OnSelchangedTreeTempl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	GetDlgItem(IDOK)->EnableWindow(m_IDTree.GetSelectedCount()>0);
	*pResult = 0;
}

void CTemplIDSelDlgEx::OnOK()
{
	listID.clear();
	HTREEITEM root = m_IDTree.GetRootItem();
	while(root)
	{
		GetAllSelTemplID(root,false);
		root = m_IDTree.GetNextItem(root, TVGN_NEXT);
	}
	CDialog::OnOK();
}

void CTemplIDSelDlgEx::OnSearch() 
{
	UpdateData();

	HTREEITEM hSearch = SearchItem(m_strTarget);
	if (hSearch) m_IDTree.SelectItem(hSearch);
}

HTREEITEM CTemplIDSelDlgEx::SearchItem(const char* szItem, HTREEITEM hRoot)
{
	HTREEITEM hChild = m_IDTree.GetChildItem(hRoot);
	while (hChild)
	{
		if (m_IDTree.GetItemText(hChild).CompareNoCase(szItem) == 0)
			return hChild;

		HTREEITEM hFind = SearchItem(szItem, hChild);
		if (hFind) return hFind;
		hChild = m_IDTree.GetNextItem(hChild, TVGN_NEXT);
	}

	return NULL;
}

unsigned long CTemplIDSelDlgEx::GetTemplID(HTREEITEM hLeaf)
{
	unsigned long id = 0;	
	CString strName = m_IDTree.GetItemText(hLeaf);
	CString strFile = strName;
	strName += ".tmpl";
	HTREEITEM hParent = m_IDTree.GetParentItem(hLeaf);
	while (hParent)
	{
		strFile = m_IDTree.GetItemText(hParent) + _T("\\") + strFile;
		hParent = m_IDTree.GetParentItem(hParent);
	}
	
	strFile = m_strRoot + _T("\\") + strFile + _T("\\") + strName;
	BaseDataTempl tmpl;
	
	if (tmpl.Load(strFile) == 0)
	{
		m_strFileName = strFile;
		m_strName = tmpl.GetName();
		id = tmpl.GetID();
		tmpl.Release();
	}	
	return id;
}

void CTemplIDSelDlgEx::GetAllSelTemplID(HTREEITEM hRoot, bool bAppend)
{
	HTREEITEM hChild = m_IDTree.GetChildItem(hRoot);
	if(hChild==0) 
	{
		if(m_IDTree.GetItemState(hRoot,TVIS_SELECTED) & TVIS_SELECTED || bAppend)
		{
			unsigned long id = GetTemplID(hRoot);
			if( id!=0 ) listID.push_back(id);
		}
	}
	while (hChild)
	{
		if(m_IDTree.GetItemState(hRoot,TVIS_SELECTED) & TVIS_SELECTED || bAppend)
			GetAllSelTemplID(hChild , true);
		else GetAllSelTemplID(hChild , false);
		hChild = m_IDTree.GetNextItem(hChild, TVGN_NEXT);
	}
}

void CTemplIDSelDlgEx::SetItemCheck( HTREEITEM hRoot ,bool bCheck)
{
	HTREEITEM hChild = m_IDTree.GetChildItem(hRoot);
	
	if(bCheck)
		m_IDTree.SetCheck(hRoot,bCheck);
	
	while (hChild)
	{
		if(m_IDTree.GetCheck(hRoot))
			SetItemCheck(hChild,true);
		else SetItemCheck(hChild, false);
		hChild = m_IDTree.GetNextItem(hChild, TVGN_NEXT);
	}	
}


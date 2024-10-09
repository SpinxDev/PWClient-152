// TemplIDSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "TemplIDSelDlg.h"
#include "BaseDataIDMan.h"
#include "BaseDataTemplate.h"
#include "ExtendDataTempl.h"
#include "AMemory.h"
#include "Global.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static	BaseDataIDManBase & GetBaseIDManByIDType(ID_TYPE type)
{
	switch(type)
	{
	case enumBaseID:			return	g_BaseIDMan;
	case enumExtID:				return g_ExtBaseIDMan;
	case enumRecipeID:		return g_RecipeIDMan;
	case enumFaceID:			return g_FaceIDMan;
	case enumConfigID:			return g_ConfigIDMan;
	default:
		assert(0);
	}

	return g_BaseIDMan;
}

BOOL CALLBACK SEL_ID_FUNCS::OnActivate(void)
{
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = m_strRootDir;
	dlg.m_strExt = m_strExt;
	dlg.m_ulID = m_var;

	BaseDataIDManBase &IDMan = GetBaseIDManByIDType(m_IDType);
	if (dlg.m_ulID)
	{
		dlg.m_strInitPath = IDMan.GetPathByID(dlg.m_ulID);
	}

	if (dlg.DoModal() != IDOK) return FALSE;

	if (dlg.m_ulID)
	{
		CString strValidFile = IDMan.GetPathByID(dlg.m_ulID);
		CString strCandidateFile = dlg.m_strFileName;
		if (strValidFile.CompareNoCase(strCandidateFile))
		{
			//	选中的不符合要求，是其它IDMan中的路径。清除为空
			dlg.m_ulID = 0;
		}
	}
	
	m_var = dlg.m_ulID;
	if (dlg.m_ulID == 0) m_strText = "（无）";
	else m_strText = dlg.m_strName;
	return TRUE;
}

LPCTSTR CALLBACK SEL_ID_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK SEL_ID_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK SEL_ID_FUNCS::OnSetValue(const AVariant& var)
{
	m_var = var;
	unsigned long ulID = unsigned long(m_var);

	AString str;
	if (ulID != 0)
	{
		BaseDataIDManBase &IDMan = GetBaseIDManByIDType(m_IDType);
		str = IDMan.GetPathByID(ulID);

		if (!str.IsEmpty())
		{
			BaseDataTempl tmpl;
			if (tmpl.Load(str) == 0)
			{
				m_strText = tmpl.GetName();
				tmpl.Release();
			}
		}
		else
		{
			m_strText.Empty();
			m_strText.Format("（错误的ID = %d）", ulID);
		}
	}
	else
		m_strText = "（无）";
}

/////////////////////////////////////////////////////////////////////////////
// CTemplIDSelDlg dialog

CTemplIDSelDlg::CTemplIDSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplIDSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplIDSelDlg)
	m_strTarget = _T("");
	//}}AFX_DATA_INIT
}

void CTemplIDSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplIDSelDlg)
	DDX_Control(pDX, IDC_TREE_TEMPL, m_IDTree);
	DDX_Text(pDX, IDC_TARGET, m_strTarget);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTemplIDSelDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplIDSelDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TEMPL, OnSelchangedTreeTempl)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplIDSelDlg message handlers

BOOL CTemplIDSelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ImageList.Create(IDB_LISTTAG, 16, 3, RGB(255, 0, 255));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDER));
    m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_OBJECT));
	m_IDTree.SetImageList(&m_ImageList,TVSIL_NORMAL);

	m_hNull = m_IDTree.InsertItem("（无）", TVI_ROOT);
	m_IDTree.SetItemData(m_hNull, TRUE);
	EnumTempl(m_strRoot, TVI_ROOT);
	m_IDTree.Expand(TVI_ROOT, TVE_EXPAND);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTemplIDSelDlg::EnumTempl(const CString& strPath, HTREEITEM hRoot)
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

void CTemplIDSelDlg::OnSelchangedTreeTempl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	GetDlgItem(IDOK)->EnableWindow(m_IDTree.GetItemData(pNMTreeView->itemNew.hItem));
	*pResult = 0;
}

void CTemplIDSelDlg::OnOK()
{
	HTREEITEM hItem = m_IDTree.GetSelectedItem();
	if (hItem == m_hNull)
	{
		m_ulID = 0;
	}
	else if (hItem)
	{
		CString strName = m_IDTree.GetItemText(hItem);
		CString strFile = strName;
		strName += ".tmpl";
		HTREEITEM hParent = m_IDTree.GetParentItem(hItem);
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
			m_ulID = tmpl.GetID();
			tmpl.Release();
		}
	}
	CDialog::OnOK();
}

void CTemplIDSelDlg::OnSearch() 
{
	UpdateData();

	HTREEITEM hSearch = SearchItem(m_strTarget);
	if (hSearch) m_IDTree.SelectItem(hSearch);
}

HTREEITEM CTemplIDSelDlg::SearchItem(const char* szItem, HTREEITEM hRoot)
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
// TemplIDSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TemplIDSelDlg.h"
#include "BaseDataIDMan.h"
#include "AFile.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ORG_PATHS _org_paths;
extern const char* _org_config_path;

extern BaseDataIDManBase g_TemplIDMan;

static const char* _format_version	= "Version: %u";
static const char* _format_id		= "ID: %u";
static const char* _format_tmpl_name= "TemplName: %s";

struct _tree_node
{
	_tree_node()
	{
		child	= 0;
		sibling = 0;
		img		= 0;
		flag	= 0;
	}

	~_tree_node()
	{
		_tree_node* c = child;

		while (c)
		{
			_tree_node* t = c->sibling;
			delete c;
			c = t;
		}
	}

	_tree_node*		child;
	_tree_node*		sibling;
	CString			name;
	CString			dir;
	int				img;
	int				flag;
};

_tree_node* _root = 0;

void _release_templ_tree()
{
	delete _root;
	_root = 0;
}

void add_child(_tree_node* parent, _tree_node* child)
{
	_tree_node* prev = parent->child;

	if (!prev)
		parent->child = child;
	else
	{
		while (prev->sibling)
			prev = prev->sibling;

		prev->sibling = child;
	}
}

_tree_node* find_root(_tree_node* parent, const char* szRoot)
{
	if (parent->dir.CompareNoCase(szRoot) == 0)
		return parent;

	_tree_node* child = parent->child;

	while (child)
	{
		_tree_node* r = find_root(child, szRoot);

		if (r)
			return r;

		child = child->sibling;
	}

	return 0;
}

BOOL LoadBaseDataHeader(const char* szPath, unsigned long& ulID, AString& strName)
{
	AFile file;

	if (!file.Open(szPath, AFILE_OPENEXIST | AFILE_TEXT))
		return FALSE;

	char	szLine[AFILE_LINEMAXLEN];
	char	szBuf[AFILE_LINEMAXLEN];
	DWORD	dwRead;
	DWORD	dwVersion = 0;

	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_version, &dwVersion);

	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_id, &ulID);

	szBuf[0] = '\0';
	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_tmpl_name, szBuf);
	strName = szBuf;

	file.Close();
	return TRUE;
}

CString GetBaseDataNameByID(unsigned long ulID)
{
	AString strName;
	AString strPath = g_TemplIDMan.GetPathByID(ulID);
	if (strPath.IsEmpty()) return CString();
	LoadBaseDataHeader(strPath, ulID, strName);
	return CString(strName);
}

void _enum_templ(const CString& strPath, _tree_node* parent)
{
	CFileFind finder;
	CString strFile = strPath + _T("\\*.*");
	BOOL bWorking = finder.FindFile(strFile);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;
		else if (finder.IsDirectory())
		{
			CString strDir = strPath + _T("\\") + finder.GetFileName();

			for (ORG_PATHS::iterator it = _org_paths.begin(); it != _org_paths.end(); ++it)
			{
				if (strstr(it->first, strDir) != NULL)
				{
					_tree_node* node = new _tree_node;
					node->name = finder.GetFileName();
					node->dir = strDir;
					add_child(parent, node);
					_enum_templ(strDir, node);
					break;
				}
				else if (strstr(strDir, it->first + "\\") != NULL)
				{
					CString strTmpl = strDir + "\\" + finder.GetFileName() + ".tmpl";

					if (GetFileAttributes(strTmpl) == (DWORD)-1)
						continue;

					CString strRemain = strDir.Mid(it->first.GetLength());
					int nLevel = 0;
					const char* szLevel = strchr(strRemain, '\\');

					while (szLevel)
					{
						nLevel++;
						szLevel = strchr(szLevel+1, '\\');
					}

					_tree_node* node = new _tree_node;
					node->name = finder.GetFileName();
					node->dir = strDir;
					node->flag = 1;
					add_child(parent, node);

					if (nLevel == it->second)
						node->img = 4;
					else
						node->img = 2;

					_enum_templ(strDir, node);
					break;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTemplIDSelDlg dialog


CTemplIDSelDlg::CTemplIDSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplIDSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplIDSelDlg)
	m_strToFind = _T("");
	//}}AFX_DATA_INIT
	m_ulID = 0;
}


void CTemplIDSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplIDSelDlg)
	DDX_Control(pDX, IDC_TREE_TEMPL, m_IDTree);
	DDX_Text(pDX, IDC_TO_FIND, m_strToFind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplIDSelDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplIDSelDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TEMPL, OnSelchangedTreeTempl)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplIDSelDlg message handlers

void CTemplIDSelDlg::OnSelchangedTreeTempl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	GetDlgItem(IDOK)->EnableWindow(m_IDTree.GetItemData(pNMTreeView->itemNew.hItem));
	*pResult = 0;
}

BOOL CTemplIDSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ImageList.Create(IDB_LISTTAG, 16, 3, RGB(255, 0, 255));
	m_IDTree.SetImageList(&m_ImageList, TVSIL_NORMAL);
	
	HTREEITEM hNone = m_IDTree.InsertItem("（无）", TVI_ROOT);
	m_IDTree.SetItemImage(hNone, 4, 4);
	m_IDTree.SetItemData(hNone, (DWORD)-1);
	InitTree();
	m_strInitPath = g_TemplIDMan.GetPathByID(m_ulID);
	_tree_node* r = find_root(_root, m_strRoot);
	
	if (!r)
		r = _root;
	
	_tree_node* child = r->child;
	
	while (child)
	{
		EnumTempl(child, TVI_ROOT);
		child = child->sibling;
	}
	m_IDTree.Expand(TVI_ROOT, TVE_EXPAND);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTemplIDSelDlg::EnumTempl(_tree_node* node, HTREEITEM hRoot)
{
	HTREEITEM h = m_IDTree.InsertItem(node->name, hRoot);
	m_IDTree.SetItemData(h, node->flag);
	m_IDTree.SetItemImage(h, node->img, node->img);

	if (node->flag)
	{
		CString strTmpl = node->dir + "\\" + node->name + ".tmpl";

		if (m_strInitPath.CompareNoCase(strTmpl) == 0)
			m_IDTree.Select(h, TVGN_CARET);
	}

	_tree_node* child = node->child;

	while (child)
	{
		EnumTempl(child, h);
		child = child->sibling;
	}
}

void CTemplIDSelDlg::OnOK()
{
	HTREEITEM hItem = m_IDTree.GetSelectedItem();

	if (hItem)
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
		m_ulID = 0;
		AString strTemplName;
		LoadBaseDataHeader(strFile, m_ulID, strTemplName);
		m_strSelPath = strFile;
		m_strName = strTemplName;
	}

	CDialog::OnOK();
}

void CTemplIDSelDlg::OnFind()
{
	UpdateData(TRUE);
	HTREEITEM hRoot = m_IDTree.GetSelectedItem();

	if (hRoot == NULL)
	{
		AfxMessageBox("请选择类别", MB_OK | MB_ICONSTOP);
		return;
	}

	if(!Find(hRoot, m_strToFind))
	{
		char strTemp[100];
		int iID = atoi(m_strToFind);
		itoa(iID,strTemp,10);
		CString strCtr(strTemp);
		bool bIsPureNumber = m_strToFind == strCtr;

		if (bIsPureNumber)
		{
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];
			_splitpath(g_TemplIDMan.GetPathByID(iID),NULL,NULL,fname,ext);
			
			CString strName(fname);
			
			Find2(TVI_ROOT,strName,iID);
		}
		else
		{
			Find(TVI_ROOT,m_strToFind);
		}
	}
}

BOOL CTemplIDSelDlg::Find2(HTREEITEM hItem, const CString& strToFind, int iID)
{
	if (hItem != TVI_ROOT)
	{
		if (m_IDTree.GetItemText(hItem) == strToFind)
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
			unsigned long tempID = 0;
			AString strTemplName;
			LoadBaseDataHeader(strFile, tempID, strTemplName);
			m_strSelPath = strFile;
			m_strName = strTemplName;
			if (tempID == iID)
			{
				m_IDTree.Select(hItem, TVGN_CARET);
				return TRUE;
			}
			
		}
	}
	
	HTREEITEM hChild = m_IDTree.GetChildItem(hItem);
	
	while (hChild)
	{
		if (Find2(hChild, strToFind,iID))
			return TRUE;
		
		hChild = m_IDTree.GetNextSiblingItem(hChild);
	}
	
	return FALSE;
}
BOOL CTemplIDSelDlg::Find(HTREEITEM hItem, const CString& strToFind)
{
	if (hItem != TVI_ROOT)
	{
		if (m_IDTree.GetItemText(hItem).Find(strToFind) >= 0)
		{
			m_IDTree.Select(hItem, TVGN_CARET);
			return true;
		}
	}

	HTREEITEM hChild = m_IDTree.GetChildItem(hItem);

	while (hChild)
	{
		if (Find(hChild, strToFind))
			return TRUE;

		hChild = m_IDTree.GetNextSiblingItem(hChild);
	}

	return FALSE;
}

void CTemplIDSelDlg::InitTree()
{
	if (_root == 0)
	{
		_root = new _tree_node;
		_root->dir = "BaseData";
		_enum_templ(_root->dir, _root);
	}
}

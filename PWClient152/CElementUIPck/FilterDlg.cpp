// FilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FilterDlg.h"
#include "Shellapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog


CFilterDlg::CFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_strInitPath = "c:\\";
	m_pfn = 0;
}


void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterDlg)
	DDX_Control(pDX, IDC_TREE_FILTER, m_PathTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CFilterDlg)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_FILTER, OnItemexpandedTreeFilter)
	ON_NOTIFY(NM_CLICK, IDC_TREE_FILTER, OnClickTreeFilter)
	ON_BN_CLICKED(ID_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(ID_SELECT_NONE, OnSelectNone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg message handlers

HICON CFilterDlg::GetShellIcon(int nindex) 
{
	HICON hicon=0; 
	HKEY hkeyshellicons; 
	
	
	
	//打开注册表，读相应的图标项目
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"software\\microsoft\\windows\\currentversion\\explorer\\shellicons", 0,KEY_READ,&hkeyshellicons)==ERROR_SUCCESS) 
	{ 
		char szbuffer[MAX_PATH]; 
		DWORD dwsize=MAX_PATH; 
		
		char szindex[4]; 
		sprintf(szindex,"%d",nindex); 
		if(RegQueryValueEx(hkeyshellicons,szindex,0,0,(LPBYTE)szbuffer,&dwsize)==ERROR_SUCCESS) 
		{
			CString strfile,strindex; 
			AfxExtractSubString(strfile,szbuffer,0); 
			AfxExtractSubString(strindex,szbuffer,1); 
			ExtractIconEx(strfile,atoi(strindex),0,&hicon,1); 
		} 
		RegCloseKey(hkeyshellicons); 
	}
	if(!hicon) 
		ExtractIconEx("shell32.dll",nindex,0,&hicon,1); 
	
	return hicon; 
}



BOOL CFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ImageList.Create(16,16, ILC_COLOR16, 36,36);
	m_ImageList.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_PathTree.SetImageList(&m_ImageList, TVSIL_NORMAL);


	//先添加“我的电脑”“文件夹”“打开文件夹”的图标
	CString mycomputername;
	LPITEMIDLIST ppid;
	SHGetSpecialFolderLocation(
		m_hWnd,
		CSIDL_DRIVES,
		&ppid);

	SHFILEINFO info;
	memset(&info,0,sizeof(info));
	SHGetFileInfo(
		(LPCTSTR)ppid,
		0,
		&info,
		sizeof(info),
		SHGFI_ICON | SHGFI_SMALLICON | SHGFI_PIDL | SHGFI_DISPLAYNAME);

	mycomputername = info.szDisplayName;
	m_ImageList.Add(info.hIcon);
	DestroyIcon(info.hIcon);

	TCHAR sys_path[MAX_PATH];
	GetSystemDirectory(sys_path, MAX_PATH);

	memset(&info,0,sizeof(info));
	SHGetFileInfo(
		sys_path,
		0,
		&info,
		sizeof(info),
		SHGFI_ICON | SHGFI_SMALLICON);
	m_ImageList.Add(info.hIcon);					//目录图标
	DestroyIcon(info.hIcon);

	SHGetFileInfo(
		sys_path,
		0,
		&info,
		sizeof(info),
		SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON);
	m_ImageList.Add(info.hIcon);
	DestroyIcon(info.hIcon);


	m_PathTree.SetCheck(m_PathTree.GetRootItem(),true);
	ReadPathTree(m_strInitPath,m_PathTree.GetRootItem(),2,true);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


HTREEITEM CFilterDlg::GetInsertPos(HTREEITEM hParent,CString name, bool bFolder)
{
	HTREEITEM hChild = m_PathTree.GetChildItem(hParent);
	HTREEITEM hLastFile = TVI_FIRST;
	HTREEITEM hLastReturn = 0; 
	name.MakeLower();
	while(hChild)
	{
		CString txt = m_PathTree.GetItemText(hChild);
		txt.MakeLower();
		DWORD dwData = m_PathTree.GetItemData(hChild);
		
		if(dwData==BFD_FOLDER)
		{	
			if(name > txt && bFolder)
			{
				hLastReturn = hChild;
			}
			hLastFile = hChild;
		}else if(dwData==BFD_FILE && !bFolder)
		{
			if(name > txt)
			{
				hLastReturn = hChild;
			}
		}
		
		hChild = m_PathTree.GetNextItem(hChild,TVGN_NEXT);	
	}

	if(hLastReturn!=0) return hLastReturn;
	
	if(bFolder) 
		return TVI_FIRST;
	else return hLastFile; 
}

void CFilterDlg::ReadPathTree(CString path, HTREEITEM hParent, int iCount, bool bInit, bool bOnlyFirst)
{
	if(iCount <= 0) return;
	if(!SetCurrentDirectory(path)) return;

	bool bSkip = false;
	CFileFind find;
	BOOL bResult = find.FindFile(_T("*.*"), 0);
	while(bResult && !bSkip)
	{
		HTREEITEM insertAfter = TVI_SORT;
		bResult = find.FindNextFile();
		if(find.IsDots() | find.IsSystem() | find.IsHidden() | find.IsTemporary()) continue;
		
		if(find.IsDirectory())
		{
			if(m_pfn)
			{
				CString temp = find.GetFilePath();
				if((*m_pfn)(temp,"",true)) continue;
			}
			insertAfter = GetInsertPos(hParent,find.GetFileName(),true);
			HTREEITEM hFolder = m_PathTree.InsertItem(
				find.GetFileName(),
				ICON_FOLDER,
				ICON_OPENFOLDER,
				hParent,insertAfter);
			if(bOnlyFirst) bSkip = true;
			m_PathTree.SetItemData(hFolder, BFD_FOLDER);
			if(hParent) m_PathTree.SetCheck(hFolder,m_PathTree.GetCheck(hParent));
			else if(bInit) m_PathTree.SetCheck(hFolder,true);
			ReadPathTree(find.GetFilePath(), hFolder, iCount - 1, false, true);
		}else
		{
			if(m_pfn)
			{
				CString temp = find.GetFilePath();
				if((*m_pfn)(temp,find.GetFileName(),false)) continue;
			}

			//文件的图标
			SHFILEINFO sfi; 
			ZeroMemory(&sfi,sizeof(sfi)); 
			CString strFileExt = find.GetFileName();
			int pos = strFileExt.ReverseFind('.');
			strFileExt = strFileExt.Right(pos);
			SHGetFileInfo(strFileExt, 
				FILE_ATTRIBUTE_NORMAL, 
				&sfi, 
				sizeof(sfi), 
				SHGFI_USEFILEATTRIBUTES|SHGFI_ICON|SHGFI_SMALLICON); 
			
			
			int nIcon = m_ImageList.Add(sfi.hIcon);
			DestroyIcon(sfi.hIcon);
#ifdef _DEBUG
			CString dMsg;
			dMsg.Format("%d\n",nIcon);
			TRACE(dMsg);
#endif
			insertAfter = GetInsertPos(hParent,find.GetFileName(),false);
			HTREEITEM hFile = m_PathTree.InsertItem(
				find.GetFileName(),
				nIcon,
				nIcon,
				hParent,insertAfter);
			if(bOnlyFirst) bSkip = true;
			m_PathTree.SetItemData(hFile, BFD_FILE);
			if(hParent) m_PathTree.SetCheck(hFile,m_PathTree.GetCheck(hParent));
			else if(bInit) m_PathTree.SetCheck(hFile,true);
		}
	}
}

CString CFilterDlg::GetItemPath(HTREEITEM hItem)
{
	if(hItem==0) return "";
	CString itemText = m_PathTree.GetItemText(hItem);
	CString path = itemText;
	while(hItem = m_PathTree.GetParentItem(hItem))
	{
		CString pathsub = m_PathTree.GetItemText(hItem);
		path = pathsub + "\\" + path;	
	}
	return path;
}

void CFilterDlg::OnItemexpandedTreeFilter(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pNMTreeView->action == TVE_EXPAND)
	{
		AfxGetApp()->BeginWaitCursor();

		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		DWORD dwData = m_PathTree.GetItemData(hItem);
		if(dwData >= BFD_HARDDISKBEGIN && dwData <= BFD_MAX/*BFD_MAX = BFD_FOLDER */)
		{
			//删除原来有的一条
			m_PathTree.DeleteItem(m_PathTree.GetChildItem(hItem));
			//枚举即将显示的ITEM
			CString path = GetItemPath(hItem);
			if(!path.IsEmpty()) ReadPathTree(m_strInitPath + path, hItem,1);
			m_PathTree.SetItemData(hItem,BFD_FOLDER_OPENED);
			
			HTREEITEM hChild = m_PathTree.GetChildItem(hItem);
			while(hChild)
			{
				dwData = m_PathTree.GetItemData(hChild);
				if(dwData != BFD_FILE)
				{
					CString path1 = GetItemPath(hChild);
					ReadPathTree(m_strInitPath + path1, hChild, 1, false, true);
				}
				hChild = m_PathTree.GetNextSiblingItem(hChild);
			}
		}
		
		AfxGetApp()->EndWaitCursor();
	}
	*pResult = 0;
}

void CFilterDlg::OnClickTreeFilter(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POINT pt;
	GetCursorPos(&pt);
	m_PathTree.ScreenToClient(&pt);
	UINT uFlags;
	
	HTREEITEM test = m_PathTree.HitTest(pt,&uFlags);
	if(test && uFlags & TVHT_ONITEMSTATEICON )
	{
		if(!m_PathTree.GetCheck(test))
		{
			SetChildItemCheck(test,true);
			m_PathTree.SetCheck(test,false);
		}else
		{
			SetParentItemCheck(test,false);
			SetChildItemCheck(test,false);
			m_PathTree.SetCheck(test,true);
		}
	}
	*pResult = 0;
}

void CFilterDlg::SetChildItemCheck( HTREEITEM hRoot ,bool bCheck)
{
	if(hRoot==0) return;
	m_PathTree.SetCheck(hRoot,bCheck);
	HTREEITEM hChild = m_PathTree.GetChildItem(hRoot);
	while (hChild)
	{
		SetChildItemCheck(hChild,bCheck);
		hChild = m_PathTree.GetNextItem(hChild, TVGN_NEXT);
	}	
}

void CFilterDlg::SetParentItemCheck( HTREEITEM hChild, bool bCheck)
{
	if(hChild==0) return;
	m_PathTree.SetCheck(hChild,bCheck);
	HTREEITEM hParent = m_PathTree.GetParentItem(hChild);
	if(hParent) SetParentItemCheck(hParent,bCheck);
}

void CFilterDlg::BuildTreeEx()
{
	CPathTreeNode *pRoot = m_PathTreeEx.InsertItem(NULL,"ROOT",false);
	HTREEITEM hItem = m_PathTree.GetRootItem();
	while(hItem)
	{
		EnumTree(hItem,pRoot);
		hItem = m_PathTree.GetNextItem(hItem,TVGN_NEXT);
	}
}

void CFilterDlg::EnumTree(HTREEITEM hRoot,CPathTreeNode *pParent)
{
	CPathTreeNode* pNewItem = m_PathTreeEx.InsertItem(pParent,m_PathTree.GetItemText(hRoot),m_PathTree.GetCheck(hRoot));
	if(pNewItem)
	{
		HTREEITEM hChild = m_PathTree.GetChildItem(hRoot);
		while(hChild)
		{
			EnumTree(hChild,pNewItem);
			hChild = m_PathTree.GetNextItem(hChild,TVGN_NEXT);
		}
	}
}

void CFilterDlg::OnOK()
{
	BuildTreeEx();
	CDialog::OnOK();
}

bool CFilterDlg::IsFiltered(CString strPath)
{
	CPathTreeNode*pParent = m_PathTreeEx.GetRoot();
	if(pParent==0) return false;

	CString tempPath = strPath;
	bool bBreak = false;
	while(!bBreak)
	{
		CString itemName;
		int pos = tempPath.FindOneOf("\\");	
		int len = tempPath.GetLength();
		if(pos!=-1)
		{
			itemName = tempPath.Left(pos);
			tempPath = tempPath.Mid(pos+1);
		}else 
		{
			itemName = tempPath;
			bBreak = true;
		}
		CPathTreeNode *pChild = 0;
		for(  int i = 0; i < pParent->children.size(); ++i)
		{
			pChild = pParent->children[i];
			CString txt = pChild->text;
			if(stricmp(txt,itemName)==0)
			{
				if(pChild->bCheck)
					return false;
				if(pChild->children.size()<1)
				{//叶子接点的状态决定未列举文件的状态
					if(!pChild->bCheck) return true;
					else return false;
				}
				pParent = pChild;
				break;
			}
		}
	}
	
	return true;
}

void CFilterDlg::OnSelectAll() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hRoot = m_PathTree.GetRootItem();
	while(hRoot)
	{
		SetChildItemCheck(hRoot,true);
		hRoot = m_PathTree.GetNextItem(hRoot,TVGN_NEXT);
	}
}

void CFilterDlg::OnSelectNone() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hRoot = m_PathTree.GetRootItem();
	while(hRoot)
	{
		SetChildItemCheck(hRoot,false);
		hRoot = m_PathTree.GetNextItem(hRoot,TVGN_NEXT);
	}
}

// DlgSelectFile.cpp : implementation file
//

#include "global.h"
#include <shlwapi.h>
#include "Resource.h"
#include "DlgSelectFile.h"
#include "AMemory.h"
#include "MainFrm.h"
#include "ElementMap.h"
#include "VssOperation.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectFile dialog


CDlgSelectFile::CDlgSelectFile(const PARAMS& Params, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectFile)
	m_strPrompt = _T("");
	//}}AFX_DATA_INIT

	ASSERT(Params.szRootDir);

	int iLen = strlen(Params.szRootDir);
	if (Params.szRootDir[iLen-1] == '\\')
		m_strRootDir = CString(Params.szRootDir, iLen-1);
	else
		m_strRootDir = Params.szRootDir;

	m_Params		= Params;
	m_strFilter		= Params.szFileFilter ? Params.szFileFilter : "*.*";
	m_strPrompt		= Params.szPrompt;

	m_strFileList	= "";
	m_iNumSelFile	= 0;
}

void CDlgSelectFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectFile)
	DDX_Control(pDX, IDC_TREE_DIRECTORY, m_DirTree);
	DDX_Control(pDX, IDC_LIST_FILE, m_FileList);
	DDX_Text(pDX, IDC_STATIC_PROMPT, m_strPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectFile, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectFile)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIRECTORY, OnSelchangedTreeDirectory)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, OnDblclkListFile)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_OUT, OnButtonCheckOut)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DIRECTORY, OnDblclkTreeDirectory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectFile message handlers

BOOL CDlgSelectFile::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//	Backup current directory
	GetCurrentDirectory(MAX_PATH, m_szCurPath);

	//	Initialize image list
	InitImageList();

	m_FileList.InsertColumn(0, "File name", LVCFMT_LEFT, 500);
	if (!m_Params.bMultiSelect)
	{
		DWORD dwStyle = ::GetWindowLong(m_FileList.m_hWnd, GWL_STYLE);
		::SetWindowLong(m_FileList.m_hWnd, GWL_STYLE, dwStyle | LVS_SINGLESEL);
	}

	//	Set root directory
	SetRootPath();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectFile::OnOK() 
{
	UpdateData(TRUE);
	
	HTREEITEM hItem = m_DirTree.GetSelectedItem();
	m_iNumSelFile = m_FileList.GetSelectedCount();

	if (!hItem || !m_iNumSelFile)
		return;

	//	Get full path and relative path
	GetItemPath(m_strFullPath, hItem, true);
	GetItemPath(m_strRelativePath, hItem, false);

	m_strFileList = "";

	bool bFirst = true;
	CString strPathName;
	POSITION pos = m_FileList.GetFirstSelectedItemPosition();
	while (pos)
	{
		int iItem = m_FileList.GetNextSelectedItem(pos);
		m_strFileList += m_FileList.GetItemText(iItem, 0) + "|";
		if(bFirst) 
		{
			strPathName = m_FileList.GetItemText(iItem, 0);
			bFirst = false;
		}
	}
	
	
	strPathName = m_strFullPath + "\\" + strPathName;
	if(AUX_FileIsReadOnly(strPathName))
	{
		if(IDOK!=AfxMessageBox("本地图还没有做Check out,地图是只读的,如果你想修改,请先Check out!",MB_OKCANCEL))
		{
			return;
		}
		g_bReadOnly = true;
	}else g_bReadOnly = false;

	CDialog::OnOK();
}

void CDlgSelectFile::OnCancel()
{
	CDialog::OnCancel();
}

void CDlgSelectFile::OnDestroy() 
{
	//	Restore current directory
	SetCurrentDirectory(m_szCurPath);
	CDialog::OnDestroy();
}

//	Initialize image list
void CDlgSelectFile::InitImageList()
{
	m_Images.Create(IDB_BITMAP_FLAGS, 16, 6, RGB(255, 0, 255));
	/*
	m_Images.Create(16, 16, ILC_COLOR24, 128, 128);
	m_Images.SetBkColor(GetSysColor(COLOR_WINDOW));

	char szPath[MAX_PATH];
	GetSystemDirectory(szPath, MAX_PATH);

	SHFILEINFO Info;
	memset(&Info, 0, sizeof (Info));

	//	Icon of closed folder
	SHGetFileInfo(szPath, 0, &Info, sizeof (Info), SHGFI_ICON | SHGFI_SMALLICON);
	m_Images.Add(Info.hIcon);
	DestroyIcon(Info.hIcon);

	//	Icon of opended folder
	SHGetFileInfo(szPath, 0, &Info, sizeof (Info), SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON);
	m_Images.Add(Info.hIcon);
	DestroyIcon(Info.hIcon);
	*/
	m_DirTree.SetImageList(&m_Images, TVSIL_NORMAL);
}

//	Set root path
void CDlgSelectFile::SetRootPath()
{
	if (!SetCurrentDirectory(m_strRootDir))
		return;
	
	//	Clear directory tree
	m_DirTree.DeleteAllItems();

	SHFILEINFO Info;
	memset(&Info, 0, sizeof (Info));

	//	Get normal icon
	SHGetFileInfo(m_strRootDir, 0, &Info, sizeof (Info), SHGFI_ICON | SHGFI_SMALLICON);
	int iImage = m_Images.Add(Info.hIcon);
	DestroyIcon(Info.hIcon);

	//	Get opened icon
	SHGetFileInfo(m_strRootDir, 0, &Info, sizeof (Info), SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON);
	int iSelImage = m_Images.Add(Info.hIcon);
	DestroyIcon(Info.hIcon);
	
	//	Create root node
	HTREEITEM hRoot = m_DirTree.InsertItem(m_strRootDir, iImage, iSelImage);
	m_DirTree.SetItemData(hRoot, PathIsRoot(m_strRootDir) ? PTIT_DRIVER : PTIT_FOLDER);
	
	//	Recursively travel all sub directoies
	RecursiveDirectories(m_strRootDir, hRoot);

	m_DirTree.Expand(hRoot, TVE_EXPAND);

	//	优先显示当前工程，方便选中附近工程
	bool bFindCurrentMap(false);
	while (m_DirTree.ItemHasChildren(hRoot))
	{
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if (!pMap){
			break;
		}
		HTREEITEM hItemCurProj = NULL;
		CString strMapName = pMap->GetMapName();
		HTREEITEM hChildItem = m_DirTree.GetChildItem(hRoot);
		while (hChildItem != NULL)
		{
			CString strItemText = m_DirTree.GetItemText(hChildItem);
			if (!strItemText.CompareNoCase(strMapName)){
				hItemCurProj = hChildItem;
				break;
			}
			hChildItem = m_DirTree.GetNextItem(hChildItem, TVGN_NEXT);
		}
		if (hItemCurProj){
			m_DirTree.EnsureVisible(hItemCurProj);
			m_DirTree.SelectItem(hItemCurProj);
			bFindCurrentMap = true;
		}
		break;
	}
	if (!bFindCurrentMap){
		m_DirTree.SelectItem(hRoot);
	}
}

//	Recursively travel all sub directoies
void CDlgSelectFile::RecursiveDirectories(const char* szPath, HTREEITEM hParent)
{
	if (!SetCurrentDirectory(szPath))
		return;

	CFileFind Find;
	BOOL bResult = Find.FindFile("*.*");

	while (bResult)
	{
		bResult = Find.FindNextFile();
		if (Find.IsDots() || !Find.IsDirectory())
			continue;
	
		if(stricmp(Find.GetFileName(),"EditorTempMap")==0) continue;
		HTREEITEM hItem = InsertPathItem(hParent, Find.GetFileName());

		//	Continue sub directories
		RecursiveDirectories(Find.GetFilePath(), hItem);
	}
}

//	Insert a path to directory tree
HTREEITEM CDlgSelectFile::InsertPathItem(HTREEITEM hParent, const char* szPath)
{
	int iImage=IMG_FOLDER, iSelImage=IMG_OPENEDFOLDER;

	HTREEITEM hItem = m_DirTree.InsertItem(szPath,hParent ? hParent : TVI_ROOT, TVI_LAST);
	CString str;
	str.Format("%s%s\\%s\\%s.elproj",g_szWorkDir,g_szEditMapDir,szPath,szPath);
	if(AUX_FileIsReadOnly(str))
		m_DirTree.SetItemImage(hItem, 2,2);
	else m_DirTree.SetItemImage(hItem, 3,3);
	
	return hItem;
}

//	When selected directory change
void CDlgSelectFile::OnSelchangedTreeDirectory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//	Get full path of this folder
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CString strPath;
	GetItemPath(strPath, hItem, true);

	if (!SetCurrentDirectory(strPath))
		return;

	//	Display files of this folder
	m_FileList.DeleteAllItems();

	CFileFind Find;
	BOOL bResult = Find.FindFile(m_strFilter);

	while (bResult)
	{
		bResult = Find.FindNextFile();
		if (Find.IsDots() || Find.IsSystem() || Find.IsDirectory())
			continue;

		m_FileList.InsertItem(0, Find.GetFileName());
	}

	*pResult = 0;
}

//	Get item path
void CDlgSelectFile::GetItemPath(CString& strPath, HTREEITEM hItem, bool bFull)
{
	ASSERT(hItem);

	if (hItem == m_DirTree.GetRootItem())
	{
		if (bFull)	//	Get full path
			strPath = m_DirTree.GetItemText(hItem);
		else
			strPath = "";

		return;
	}

	strPath = m_DirTree.GetItemText(hItem);

	HTREEITEM hParent = m_DirTree.GetParentItem(hItem);
	while (hParent && hParent != m_DirTree.GetRootItem())
	{
		strPath = m_DirTree.GetItemText(hParent) + "\\" + strPath;
		hParent = m_DirTree.GetParentItem(hParent);
	}

	if (bFull)	//	Get full path
		strPath = m_DirTree.GetItemText(m_DirTree.GetRootItem()) + "\\" + strPath;
}

//	Get selected file's name
CString CDlgSelectFile::GetFileName()
{
	if (!m_iNumSelFile)
		return CString("");

	int n = m_strFileList.Find('|');

	return m_strFileList.Left(n);
}

//	Get selected file's relative path name
CString CDlgSelectFile::GetRelativePathName()
{
	if (!m_iNumSelFile)
		return CString("");

	if (m_strRelativePath == "")
		return GetFileName();
	else
		return m_strRelativePath + "\\" + GetFileName();
}

//	Get selected file's full path name
CString CDlgSelectFile::GetFullPathName()
{
	if (!m_iNumSelFile)
		return CString("");

	return m_strFullPath + "\\" + GetFileName();
}

//	For multi-selected style...
//	Get start position of file list
DWORD CDlgSelectFile::GetStartPosition()
{
	return (DWORD)(-1);
}

//	Get next file name in file list
CString CDlgSelectFile::GetNextFileName(DWORD* pdwPos)
{
	if (!m_iNumSelFile)
	{
		*pdwPos = 0;
		return CString("");
	}

	int iFrom = (*pdwPos == (DWORD)(-1)) ? 0 : (int)(*pdwPos) + 1;

	int iTo = m_strFileList.Find('|', iFrom);
	if (iTo < 0)	//	This should not happen
	{
		*pdwPos = 0;
		return CString("");
	}

	if (iTo == m_strFileList.GetLength()-1)
		*pdwPos = 0;
	else
		*pdwPos = (DWORD)iTo;

	return m_strFileList.Mid(iFrom, iTo-iFrom);
}

//	Get next relative path and name in file list
CString CDlgSelectFile::GetNextRelativePathName(DWORD* pdwPos)
{
	if (!m_iNumSelFile)
		return CString("");

	if (m_strRelativePath == "")
		return GetNextFileName(pdwPos);
	else
		return m_strRelativePath + "\\" + GetNextFileName(pdwPos);
}

//	Get next full path name in file list
CString CDlgSelectFile::GetNextFullPathName(DWORD* pdwPos)
{
	if (!m_iNumSelFile)
		return CString("");

	return m_strFullPath + "\\" + GetNextFileName(pdwPos);
}

//	Double click file
void CDlgSelectFile::OnDblclkListFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!m_Params.bMultiSelect && m_FileList.GetSelectedCount())
		OnOK();
	
	*pResult = 0;
}



void CDlgSelectFile::OnButtonCheckOut() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	HTREEITEM hItem = m_DirTree.GetSelectedItem();
	
	if (!hItem)
		return;

	//	Get full path and relative path
	GetItemPath(m_strFullPath, hItem, true);
	GetItemPath(m_strRelativePath, hItem, false);

	CString strPathName;
	strPathName.Format("%s\\%s.elproj",m_strFullPath,m_strRelativePath);
	if(AUX_GetMainFrame()->GetMap()) 
	{
		if(stricmp(AUX_GetMainFrame()->GetMap()->GetMapName(),m_strRelativePath)==0)
		{
			MessageBox("请先关闭打开的地图！");
			return;
		}
	}
	if(AUX_FileIsReadOnly(strPathName))
	{
		CString dst,src;
		dst.Format("%s%s\\%s",g_szWorkDir,g_szEditMapDir,m_strRelativePath);
		src.Format("%s\\MapsRenderData\\%s\\%s",g_Configs.szServerPath,g_Configs.szCurProjectName,m_strRelativePath);
		if(!AUX_CopyRenderFiles(dst,src)) 
			return;
		
		if(AUX_CheckOutMap(m_strRelativePath.GetBuffer(0)))
		{
			m_DirTree.SetItemImage(hItem, 3,3);
		}else
		{
			AUX_UndoCheckOutMap(m_strRelativePath.GetBuffer(0));	
		}
		strPathName.ReleaseBuffer();
		m_strRelativePath.ReleaseBuffer();
	}
}

void CDlgSelectFile::OnButtonCheckIn() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	HTREEITEM hItem = m_DirTree.GetSelectedItem();
	m_iNumSelFile = m_FileList.GetSelectedCount();

	if (!hItem || !m_iNumSelFile)
		return;

	//	Get full path and relative path
	GetItemPath(m_strFullPath, hItem, true);
	GetItemPath(m_strRelativePath, hItem, false);

	m_strFileList = "";

	bool bFirst = true;
	CString strPathName;
	POSITION pos = m_FileList.GetFirstSelectedItemPosition();
	while (pos)
	{
		int iItem = m_FileList.GetNextSelectedItem(pos);
		m_strFileList += m_FileList.GetItemText(iItem, 0) + "|";
		if(bFirst) 
		{
			strPathName = m_FileList.GetItemText(iItem, 0);
			bFirst = false;
		}
	}
	
	if(!AUX_FileIsReadOnly(strPathName))
	{
		//判断地图是否已经打开
		CMainFrame *pFrame = AUX_GetMainFrame();
		if(pFrame)
		{
			CElementMap *pMap = pFrame->GetMap();
			if(strcmp(pMap->GetMapName(),strPathName)==0)
				return;
		}
		//如果地图不是打开的做Check in
		if(AUX_CheckInMap(strPathName.GetBuffer(0)))
		{
			m_DirTree.SetItemImage(hItem, 2,2);
		}
		strPathName.ReleaseBuffer();
	}
}

void CDlgSelectFile::OnDblclkTreeDirectory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_FileList.GetItemCount() > 0){
		if (m_FileList.GetSelectedCount() == 0){
			m_FileList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		}
		NMHDR thdr;
		thdr.code = NM_DBLCLK;
		thdr.hwndFrom = m_FileList.GetSafeHwnd();
		thdr.idFrom = IDC_LIST_FILE;
		::SendMessage(m_hWnd, WM_NOTIFY, IDC_LIST_FILE, (LPARAM)&thdr);
	}

	*pResult = 0;
}

// PathTree.cpp : implementation file
//

#include "stdafx.h"
#include "PathTree.h"
#include <shlwapi.h>
#include "AVariant.h"

/////////////////////////////////////////////////////////////////////////////
// CPathTree

CPathTree::CPathTree()
{
	m_filterproc = NULL;
	m_path   = "c:\\";
	m_param = 0;
}

CPathTree::~CPathTree()
{
	m_registerIcon.RemoveAll();
}

BEGIN_MESSAGE_MAP(CPathTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CPathTree)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPathTree message handlers

void CPathTree::PreSubclassWindow() 
{
	// TODO: 在对话框中创建时
	InitImageList();

	CTreeCtrl::PreSubclassWindow();
}

int CPathTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitImageList();

	return 0;
}

BOOL CPathTree::SetPath(CString path, FINDFILTER filterproc, DWORD param)
{
	char backup_path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, backup_path);
	if(!SetCurrentDirectory(path))
	{
		return FALSE;
	}

	//清理以前的视图
	DeleteAllItems();

	//读源路径的图标，因为可能是驱动器，所以独立设置
	SHFILEINFO info;
	memset(&info,0,sizeof(info));
	SHGetFileInfo(
		path,
		0,
		&info,
		sizeof(info),
		SHGFI_ICON | SHGFI_SMALLICON );
	int image = m_pImages.Add(info.hIcon);
	DestroyIcon(info.hIcon);

//test
/*
	memset(&info,0,sizeof(info));
	SHGetFileInfo(
		"a.bmp",
		FILE_ATTRIBUTE_NORMAL ,
		&info,
		sizeof(info),
		SHGFI_ICON | SHGFI_SMALLICON |SHGFI_USEFILEATTRIBUTES);
	image = m_pImages.Add(info.hIcon);
	DestroyIcon(info.hIcon);
	*/
//end test

	SHGetFileInfo(
		path,
		0,
		&info,
		sizeof(info),
		SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON);
	int selimage = m_pImages.Add(info.hIcon);
	DestroyIcon(info.hIcon);
	
	HTREEITEM hRoot = InsertItem(
		path,
		image,
		selimage);

	SetItemData(hRoot, PathIsRoot(path) ? PTIT_DRIVER : PTIT_FOLDER);		//目录
	m_param = param;
	ReadPathStruct(path,2,hRoot, filterproc);

	SetCurrentDirectoryA(backup_path);
	m_path = path;
	m_filterproc = filterproc;

	Expand(hRoot,TVE_EXPAND);
	SelectItem(hRoot);
	return TRUE;
}

CString CPathTree::GetPath()
{
	return m_path;
}

BOOL CPathTree::ReadPathStruct(CString path, int layer, HTREEITEM hParent, FINDFILTER filterproc)
{
	if(!SetCurrentDirectory(path))
	{
		return false;
	}

	CFileFind find;
	BOOL bResult = find.FindFile(_T("*.*"));
	while(bResult)
	{
		bResult = find.FindNextFile();
		if(find.IsDots() ||
		   find.IsSystem())
		{
			continue;
		}

		PATHTREEITEMTYPE type = find.IsDirectory() ? PTIT_FOLDER : PTIT_FILE;
		if(filterproc)
		{
			if(!filterproc(m_param, find.GetFilePath(), type))
			{
				continue;
			}
		}

		HTREEITEM hFileItem = InsertFileItem(
			hParent, 
			find.GetFilePath(),
			type,
			layer > 1);

		if(find.IsDirectory() && layer > 1 && hFileItem)
		{
			if(!ReadPathStruct(find.GetFilePath(), layer - 1, hFileItem, filterproc))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

HTREEITEM CPathTree::InsertFileItem(HTREEITEM hParent, CString filepath, PATHTREEITEMTYPE type, BOOL queryicon)
{
	int image = PATHTREE_FILEICON ,selimage = PATHTREE_FILEICON;
	if(queryicon) QueryFileIconIndex(filepath, type, image, selimage);

	int filenamepos = filepath.ReverseFind('\\');
	CString filename = filepath.Mid( filenamepos == -1 ? 0 : filenamepos + 1);
	HTREEITEM hRetVal = InsertItem(
		filename,
		image,
		selimage,
		hParent ? hParent : TVI_ROOT,
		TVI_LAST);

	SetItemData(hRetVal, type);
	return hRetVal;
}

void CPathTree::QueryFileIconIndex(CString filepath, PATHTREEITEMTYPE type, int & image, int & selimage)
{
	if(type == PTIT_FOLDER)
	{
		image	 = PATHTREE_FOLDERICON;
		selimage = PATHTREE_FOLDEROPENICON;
	}
	else if(type == PTIT_FILE)
	{
		int extpos = filepath.ReverseFind('.');
		CString fileext = filepath.Mid(extpos == -1 ? 0 : extpos + 1);

		POSITION pos = m_registerIcon.GetHeadPosition();
		while(pos)
		{
			FILEREGISTERICON & pReg = m_registerIcon.GetNext(pos);
			if(pReg.m_extendname == fileext)
			{
				image = pReg.m_imageindex;
				selimage = pReg.m_imageindex;
				return;
			}
		}

		SHFILEINFOA info;
		memset(&info,0,sizeof(info));
		SHGetFileInfoA(
			CSafeString(filepath),
			0,
			&info,
			sizeof(info),
			SHGFI_ICON | SHGFI_SMALLICON);
		int index = m_pImages.Add(info.hIcon);						//目录图标
		FILEREGISTERICON pReg;
		strcpy(pReg.m_extendname, CSafeString(fileext));
		pReg.m_imageindex = index;
		m_registerIcon.AddTail(pReg);

		DestroyIcon(info.hIcon);

		image = index;
		selimage = index;
	}
}

void CPathTree::InitImageList()
{
//	CoInitialize(NULL);

	m_pImages.Create(16,16,ILC_COLOR24,128,128);
	m_pImages.SetBkColor(GetSysColor(COLOR_WINDOW));

	m_pImages.Add(LoadIcon(NULL, IDI_WINLOGO));		//文件图标

	char sys_path[MAX_PATH];
	GetSystemDirectoryA(sys_path, MAX_PATH);

	SHFILEINFOA info;
	memset(&info,0,sizeof(info));
	SHGetFileInfoA(
		sys_path,
		0,
		&info,
		sizeof(info),
		SHGFI_ICON | SHGFI_SMALLICON);
	m_pImages.Add(info.hIcon);					//目录图标
	DestroyIcon(info.hIcon);

	SHGetFileInfoA(
		sys_path,
		0,
		&info,
		sizeof(info),
		SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON);
	m_pImages.Add(info.hIcon);
	DestroyIcon(info.hIcon);

	SetImageList(&m_pImages,TVSIL_NORMAL);			//目录展开图标
	
//	CoUninitialize();
}

CString CPathTree::GetSelectedPath(BOOL bFull, PATHTREEITEMTYPE * type)
{
	CString path;
	HTREEITEM item = GetSelectedItem();
	if(!item)
	{
		return path;
	}

	PATHTREEITEMTYPE param = (PATHTREEITEMTYPE)GetItemData(item);
	if(type) *type = param;
	return GetItemPath(item, bFull);
}

void CPathTree::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: 某个项目被展开
	if(pNMTreeView->action == TVE_EXPAND)
	{
		AfxGetApp()->BeginWaitCursor();

		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		if(hItem != GetRootItem())
		{
			HTREEITEM hChild = GetChildItem(hItem);
			while(hChild)
			{

				int image,selimage;
				GetItemImage(hChild, image, selimage);
				if(image == PATHTREE_FILEICON || selimage == PATHTREE_FILEICON)
				{
					CString path = GetItemPath(hChild);
					PATHTREEITEMTYPE type = (PATHTREEITEMTYPE)GetItemData(hChild);

					QueryFileIconIndex(path, type, image, selimage);
					SetItem(
						hChild,
						TVIF_IMAGE | TVIF_SELECTEDIMAGE,
						_T(""),
						image,
						selimage,
						0,
						0,
						0);

					if(type == PTIT_FOLDER)
					{
						//读入目录信息
						ReadPathStruct(path,1,hChild,m_filterproc);
					}
				}

				hChild = GetNextSiblingItem(hChild);
			}
		}
		
		AfxGetApp()->EndWaitCursor();
	}

	*pResult = 0;
}

CString CPathTree::GetItemPath(HTREEITEM hItem, BOOL bFull)
{
	CString path;
	if(!hItem)
	{
		return path;
	}

	PATHTREEITEMTYPE param = (PATHTREEITEMTYPE)GetItemData(hItem);
	path = GetItemText(hItem);
	HTREEITEM hParent = GetParentItem(hItem);
	while(hParent && hParent != GetRootItem())
	{
		path = GetItemText(hParent) + "\\" + path;
		hParent = GetParentItem(hParent);
	}

	if(bFull)
	{
		path = GetItemText(GetRootItem()) + "\\" + path;
	}

	if(param == PTIT_FOLDER || param == PTIT_DRIVER)
	{
		path += "\\";
	}
	path.Replace(_T("\\\\"),_T("\\"));
	return path;
}

void CPathTree::EnsureVisiblePath(CString path, BOOL bPathIsFull)
{
	CString root_path = m_path;
	root_path.MakeUpper();
	path.MakeUpper();

	CString directory = path;
	if(bPathIsFull)
	{
		// 提供的路径是绝对路径，转化成相对的先
		int pos = path.Find(root_path, 0);
		if(pos == -1) return;	// 没找到首路径，退出
		directory = path.Mid(pos + root_path.GetLength());
	}

	if(directory.IsEmpty())
	{
		return;
	}
	if(directory[0] == '\\') directory.Delete(0);

	// 开始查找
	HTREEITEM item = GetRootItem();
	int start = 0, end = 0;
	while(item)
	{
		end = directory.Find('\\', start);
		if(end == -1) end = directory.GetLength();
		CString name = directory.Mid(start, end - start);

		HTREEITEM child = GetChildItem(item);
		while(child)
		{
			CString childtext = GetItemText(child);
			childtext.MakeUpper();
			if(childtext == name)
			{
				item = child;
				SelectItem(item);
				//EnsureVisible(item);
				break;
			}

			child = GetNextSiblingItem(child);
		}

		// 查找一下级路径
		start ++;
		if(start >= directory.GetLength())
		{
			break;
		}
	}
}

void CPathTree::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CTreeCtrl::OnClose();
}

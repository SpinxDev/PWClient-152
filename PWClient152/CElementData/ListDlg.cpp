// ListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VssOperation.h"
#include "Global.h"
#include "ElementData.h"
#include "PropertyList.h"
#include "AObject.h"
#include "AVariant.h"
#include "ListDlg.h"
#include "DlgProgressNotify.h"
#include "BaseDataTemplate.h"
#include "TemplateDlg.h"
#include "TemplateCreateDlg.h"
#include "TemplateExtDlg.h"
#include "ExtendDataTempl.h"
#include "ElementDataDoc.h"
#include "TreeFlagMan.h"
#include "../CElementClient/AWStringWithWildcard.h"
#include <AFileImage.h>
#include <AFI.h>
#include <process.h>

#define new A_DEBUG_NEW
#define FD_OPENED 1923



/////////////////////////////////////////////////////////////////////////////
// CListDlg dialog


CListDlg::CListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	bRclick = false;
	m_bNeedReInitSearch = true;
}


void CListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListDlg)
	DDX_Control(pDX, IDC_TREE_SORT_LIST, m_Tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListDlg, CDialog)
	//{{AFX_MSG_MAP(CListDlg)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_SORT_LIST, OnRclickTreeSortList)
	ON_COMMAND(ID_CREATE_OBJECT, OnCreateObject)
	ON_NOTIFY(NM_CLICK, IDC_TREE_PROPERTY, OnClickTreeProperty)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_SORT_LIST, OnDblclkTreeSortList)
	ON_NOTIFY(NM_RDBLCLK, IDC_TREE_PROPERTY, OnRdblclkTreeProperty)
	ON_NOTIFY(NM_RDBLCLK, IDC_TREE_SORT_LIST, OnRdblclkTreeSortList)
	ON_COMMAND(ID_CREATE_OBJECT_EXT, OnCreateObjectExt)
	ON_UPDATE_COMMAND_UI(ID_CREATE_OBJECT_EXT, OnUpdateCreateObjectExt)
	ON_UPDATE_COMMAND_UI(ID_CREATE_OBJECT, OnUpdateCreateObject)
	ON_COMMAND(ID_VSS_CHECK_IN, OnVssCheckIn)
	ON_COMMAND(ID_VSS_CHECK_OUT, OnVssCheckOut)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SORT_LIST, OnSelchangedTreeSortList)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_SORT_LIST, OnSelchangingTreeSortList)
	ON_COMMAND(ID_EDIT_TEMPLATE_PROPERTY, OnEditTemplateProperty)
	ON_WM_DESTROY()
	ON_COMMAND(ID_MODIFIED_OBJECT_EXT, OnModifiedObjectExt)
	ON_WM_TIMER()
	ON_COMMAND(ID_VSS_CHECK_IN_ALL, OnVssCheckInAll)
	ON_COMMAND(ID_VSS_CHECK_OUT_ALL, OnVssCheckOutAll)
	ON_NOTIFY(NM_CLICK, IDC_TREE_SORT_LIST, OnClickTreeSortList)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_SORT_LIST, OnItemexpandedTreeSortList)
	ON_COMMAND(ID_VSS_UNDO_CHECK_OUT, OnVssUndoCheckOut)
	ON_UPDATE_COMMAND_UI(ID_VSS_UNDO_CHECK_OUT, OnUpdateVssUndoCheckOut)
	ON_COMMAND(ID_AUTO_RESAVE_TEMPLATE, OnAutoResaveTemplate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListDlg message handlers

void CListDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CTreeCtrl* pTree = (CTreeCtrl*)GetDlgItem(IDC_TREE_SORT_LIST);
	CRect rect;
	GetClientRect(&rect);
	if(pTree)
	{
		pTree->SetWindowPos(NULL,1,20,rect.Width() - 2,rect.Height()-21,0);	
	}
}

void CListDlg::DeleteAllItems()
{
		CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
		if(pCtrl) pCtrl->DeleteAllItems();
}

void CListDlg::EnumObjects( CString strPathName , HTREEITEM hTreeItemp, bool bProperty,int iCount,bool bOnlyFirst)
{
	if( iCount <=0 ) return;
	int nLevel = iCount - 1;
	CTreeCtrl* pCtrl = NULL; 
	if(bProperty) pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_PROPERTY);
	else pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	ASSERT(pCtrl != NULL);

	bool bSkip = false;
	CFileFind finder;
	CString OldPath;
	CString StrExt = strPathName + "\\*.*";
	BOOL bWorking = finder.FindFile(StrExt);
	
	CString extPath = strPathName;
	extPath.Replace(g_szWorkDir,"");
	
	while (bWorking && !bSkip)
	{
		bWorking = finder.FindNextFile();
		
		//是"."或是".." 跳过.
		if (finder.IsDots())
			continue;
		// 是文件夹，继续搜寻.
		if (finder.IsDirectory())
		{
			if (finder.GetFileName() == "TaskTemplate") continue;
			CString name = finder.GetFilePath();
			name.Replace(strPathName,"");
			name.Delete(0,1);
			
			HTREEITEM insertAfter = TVI_SORT;
			HTREEITEM hNewItem;
		
			hNewItem = pCtrl->InsertItem(name,hTreeItemp,insertAfter);		
			pCtrl->SetItemData(hNewItem,0);
			if(bOnlyFirst) bSkip = true;
			
			if(IsFixedItem(extPath + "\\" + name))
				pCtrl->SetItemImage(hNewItem,0,0);
			else
			{
				if(FileIsExist(AString(extPath + "\\" + name + "\\" + name + ".tmpl")))
				{
					if(!IsHideFiles(extPath + "\\" + name))
					{
						if(FileIsReadOnly(AString(extPath + "\\" + name + "\\" + name + ".tmpl")))
							pCtrl->SetItemImage(hNewItem,4,4);
						else pCtrl->SetItemImage(hNewItem,5,5);
					}else
					{
						if(FileIsReadOnly(AString(extPath + "\\" + name + "\\" + name + ".tmpl")))
							pCtrl->SetItemImage(hNewItem,2,2);
						else pCtrl->SetItemImage(hNewItem,3,3);
					}
				}
				else pCtrl->SetItemImage(hNewItem,1,1);
			}
			EnumObjects(finder.GetFilePath(),hNewItem,bProperty,nLevel);
		}
	}
	if(!bSkip) EnumFiles(strPathName,hTreeItemp,bProperty,bOnlyFirst);
	finder.Close();
}

void CListDlg::EnumFiles(CString strPathName, HTREEITEM hTreeItemp, bool bProperty, bool bOnlyFirst)
{
	CTreeCtrl* pCtrl = NULL;
	if(bProperty) pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_PROPERTY);
	else pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	ASSERT(pCtrl != NULL);
	
	bool bSkip = false;
	CFileFind finder;
	CString msg;
	CString strWildcard;
	strWildcard = strPathName + "\\*.*";
	
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking && !bSkip)
	{
		bWorking = finder.FindNextFile();
		
		//是"."或是".." 跳过.
		if (finder.IsDots())
			continue;
		
		// 是文件夹，继续搜寻.
		if (finder.IsDirectory())
		{
			continue;
		}
		// 是文件.
		msg = finder.GetFileName();
		CString ext = finder.GetFileName();
		CString extPath = strPathName;
		extPath.Replace(g_szWorkDir,"");
		if(ext.Find(".tmpl")==-1 && ext.Find(".ext")==-1) continue;
		CString parentName = pCtrl->GetItemText(hTreeItemp);
		if(stricmp(ext,parentName + ".tmpl")==0) continue;
		if(stricmp(ext,parentName + ".ext")==0) continue;

		HTREEITEM insertAfter = TVI_SORT;
		HTREEITEM hNewItem;
		hNewItem = pCtrl->InsertItem(ext,hTreeItemp,insertAfter);		
		if(bOnlyFirst) bSkip = true;
		HTREEITEM itemp = hNewItem;
		CString path = msg;
		while(itemp = pCtrl->GetParentItem(itemp))
		{
			CString pathsub = pCtrl->GetItemText(itemp);
			path = pathsub + "\\" + path;	
		}
		path = "BaseData\\" + path;
		
		if(FileIsReadOnly(AString(path)))
		{
			pCtrl->SetItemImage(hNewItem,4,4);
		}else
		{
			pCtrl->SetItemImage(hNewItem,5,5);
		}
		msg = finder.GetFilePath();
	}
	finder.Close();
}

BOOL CListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ImageList.Create(IDB_LISTTAG, 16, 3, RGB(255, 0, 255));
    
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	ASSERT(pCtrl != NULL);
	pCtrl->SetImageList(&m_ImageList,TVSIL_NORMAL);
	// Sort the tree control's items using my
	// callback procedure.
	CString strWildcard;
	strWildcard = g_szWorkDir;
	strWildcard += "BaseData";
	
#ifdef _DEBUG 
	DWORD lastTime = a_GetTime();
	CString strDebug;
#endif
	FreshObjectList(strWildcard,NULL,2);

#ifdef _DEBUG
	strDebug.Format("Times = %d", a_GetTime() - lastTime);
	TRACE(strDebug);
#endif
	
	ReInitSearch();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CListDlg::FreshObjectList(CString strPathName , HTREEITEM hTreeItemp, int iCount)
{
	if( iCount <=0 ) return;
	int nLevel = iCount;
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	ASSERT(pCtrl != NULL);

	bool bSkip = false;
	CFileFind finder;
	CString OldPath;
	CString StrExt = strPathName + "\\*.*";
	BOOL bWorking = finder.FindFile(StrExt);
	
	CString extPath = strPathName;
	extPath.Replace(g_szWorkDir,"");
	bool bExpand = false;
	
	if(g_Record.IsOpen(extPath))
	{
		bExpand = true;
		pCtrl->SetItemData(hTreeItemp,FD_OPENED);
	}else nLevel = nLevel-1;

  	while (bWorking && !bSkip)
	{
		bWorking = finder.FindNextFile();
		
		//是"."或是".." 跳过.
		if (finder.IsDots())
			continue;
		// 是文件夹，继续搜寻.
		if (finder.IsDirectory())
		{
			if (finder.GetFileName() == "TaskTemplate") continue;
			CString name = finder.GetFilePath();
			name.Replace(strPathName,"");
			name.Delete(0,1);
			
			HTREEITEM insertAfter = TVI_SORT;
			HTREEITEM hNewItem;
			
			hNewItem = pCtrl->InsertItem(name,hTreeItemp,insertAfter);		
			pCtrl->SetItemData(hNewItem,0);
			if(!bExpand && iCount < 2) bSkip = true;
			
			if(IsFixedItem(extPath + "\\" + name))
				pCtrl->SetItemImage(hNewItem,0,0);
			else
			{
				if(FileIsExist(AString(extPath + "\\" + name + "\\" + name + ".tmpl")))
				{
					if(!IsHideFiles(extPath + "\\" + name))
					{
						if(FileIsReadOnly(AString(extPath + "\\" + name + "\\" + name + ".tmpl")))
							pCtrl->SetItemImage(hNewItem,4,4);
						else pCtrl->SetItemImage(hNewItem,5,5);
					}else
					{
						if(FileIsReadOnly(AString(extPath + "\\" + name + "\\" + name + ".tmpl")))
							pCtrl->SetItemImage(hNewItem,2,2);
						else pCtrl->SetItemImage(hNewItem,3,3);
					}
				}
				else pCtrl->SetItemImage(hNewItem,1,1);
			}
			FreshObjectList(finder.GetFilePath(),hNewItem,nLevel);
		}
	}
	if(bExpand)
	{ 
		pCtrl->Expand(hTreeItemp,TVE_EXPAND);
		EnumFiles(strPathName,hTreeItemp,false,false);
	}else if( !bSkip ) EnumFiles(strPathName,hTreeItemp,false,true);
	
	finder.Close();
}

HTREEITEM CListDlg::InsertItem(CTreeCtrl *pTree, HTREEITEM hRoot, CString name)
{
	HTREEITEM hSibling = hRoot;
	CString str = pTree->GetItemText(hSibling);
	if(stricmp(str,name)==0) return hRoot;
	hSibling = pTree->GetChildItem(hRoot);
	while(hSibling)
	{	
		str = pTree->GetItemText(hSibling);
		if(stricmp(str,name)==0)
		{
			return hSibling;
		}
		hSibling = pTree->GetNextSiblingItem(hSibling);
	}
	HTREEITEM hItem = pTree->InsertItem(name, hRoot, TVI_SORT);
	return hItem;
}

static	BaseDataIDManBase & GetBaseIDManByFindType(CDlgFindTemplate::FIND_TYPE type)
{
	switch(type)
	{
	case CDlgFindTemplate::FT_TEMPL:			return	g_BaseIDMan;
	case CDlgFindTemplate::FT_EXTDATA:	return g_ExtBaseIDMan;
	case CDlgFindTemplate::FT_RECIPE:		return g_RecipeIDMan;
	case CDlgFindTemplate::FT_CONFIG:		return g_ConfigIDMan;
	case CDlgFindTemplate::FT_FACE:			return g_FaceIDMan;
	default:
		assert(0);
	}

	return g_BaseIDMan;
}

static CDlgFindTemplate::FIND_TYPE GetFindTypeByBaseIDMan(BaseDataIDManBase *pDataIDMan)
{
	CDlgFindTemplate::FIND_TYPE type = CDlgFindTemplate::FT_TEMPL;

	if (pDataIDMan == &g_BaseIDMan)
		type = CDlgFindTemplate::FT_TEMPL;
	else if (pDataIDMan == &g_ExtBaseIDMan)
		type = CDlgFindTemplate::FT_EXTDATA;
	else if (pDataIDMan == &g_RecipeIDMan)
		type = CDlgFindTemplate::FT_RECIPE;
	else if (pDataIDMan == &g_ConfigIDMan)
		type = CDlgFindTemplate::FT_CONFIG;
	else if (pDataIDMan == &g_FaceIDMan)
		type = CDlgFindTemplate::FT_FACE;

	return type;
}

static const char * GetExtByFindType(CDlgFindTemplate::FIND_TYPE type)
{
	return (type == CDlgFindTemplate::FT_EXTDATA) ? ".ext" : ".tmpl";
}

static bool GetDelExtByFindType(CDlgFindTemplate::FIND_TYPE type)
{
	//	判断扩展名是否显示在树形结构中（以便决定是否参与匹配）
	return (type != CDlgFindTemplate::FT_EXTDATA);
}

HTREEITEM CListDlg::Open(unsigned long ulID, CDlgFindTemplate::FIND_TYPE type)
{
	//	根据物品ID查找路径并打开、返回Tree的结点（为空时表失败）
	//
	HTREEITEM hItemRet = NULL;

	BaseDataIDManBase & BaseIDMan = GetBaseIDManByFindType(type);

	CString strBaseDataPath = BaseIDMan.GetPathByID(ulID);
	while (!strBaseDataPath.IsEmpty())
	{
		CString strFullPath = CString(g_szWorkDir) + strBaseDataPath;

		//	确定文件存在
		if (!FileIsExist(AString(strFullPath)))
			break;

		//	确定文件合法
		CString strExt = GetExtByFindType(type);
		CString strPrefix = "BaseData\\";
		if (strBaseDataPath.Right(strExt.GetLength()) != strExt ||
			strBaseDataPath.Left(strPrefix.GetLength()) != strPrefix)
			break;

		//	删除不需要处理的 BaseData\ 起始头和.tmpl结尾
		CString strToSplit = strBaseDataPath;
		strToSplit.Delete(0, strPrefix.GetLength());
		if (GetDelExtByFindType(type))
			strToSplit.Delete(strToSplit.GetLength() - strExt.GetLength(), strExt.GetLength());

		//	分解成多个路径
		AArray<CString> vNodeNames;
		do 
		{
			int index = strToSplit.Find('\\');
			if (index == -1)
			{
				vNodeNames.Add(strToSplit);
				strToSplit.Empty();
			}
			else
			{
				vNodeNames.Add(strToSplit.Left(index));
				strToSplit.Delete(0, index+1);
			}
		} while(!strToSplit.IsEmpty());

		if (vNodeNames.GetSize() > 2 &&
			vNodeNames.GetAt(vNodeNames.GetUpperBound()) == vNodeNames.GetAt(vNodeNames.GetUpperBound()-1))
		{
			//	最后两个结点名称应相同
			//	如对应"BaseData\装备\武器\长兵\长刀\朴刀\朴刀.tmpl"，获取的数组依次为"装备"、"武器"、"长兵"、"长刀"、"朴刀"、"朴刀"
			//	由于在Tree中以 "朴刀" 子结点代替 "朴刀\朴刀.tmpl" 结点，因此需要删除一个
			vNodeNames.RemoveTail();
		}

		//	逐个路径查找Item并打开
		CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
		ASSERT(pCtrl != NULL);
		if (!pCtrl)	break;

		HTREEITEM hItem = pCtrl->GetRootItem();
		while (hItem)
		{
			//	在当前层次找到当前匹配结点
			CString strCur = pCtrl->GetItemText(hItem);

			if (strCur == vNodeNames[0])
			{
				//	匹配到中间某个路径

				//	删除此路径
				vNodeNames.RemoveAt(0);

				//	是否需要继续匹配？
				if (!vNodeNames.GetSize())
				{
					//	没有其它内容需要匹配

					//	使结点可见
					pCtrl->EnsureVisible(hItem);

					//	也许不是叶子结点？
					if (!pCtrl->ItemHasChildren(hItem))
					{
						//	是要搜索的叶子结点，符合要求
						hItemRet = hItem;

						//	取消其它结点的选中状态
						HTREEITEM hItemSel = pCtrl->GetRootItem();
						while (hItemSel)
						{
							if (pCtrl->GetItemState(hItemSel, TVIS_SELECTED) == TVIS_SELECTED)
								pCtrl->SetItemState(hItemSel, 0, TVIS_SELECTED);
							hItemSel = pCtrl->GetNextVisibleItem(hItemSel);
						}

						//	选中此结点
						pCtrl->SelectItem(hItem);

						//	打开此结点
						CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp *)AfxGetApp())->OpenDocumentFile(strBaseDataPath);
					}
					//	else 不是叶子结点，找错了

					break;
				}

				//	以当前结点为父亲结点，在其子结点中继续查找

				//	先展开当前结点
				if (!(pCtrl->GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED))
				{
					pCtrl->SendMessage(TVM_EXPAND, TVE_COLLAPSE, (LPARAM)hItem);
					pCtrl->Expand(hItem, TVE_EXPAND);
				}

				//	然后获取其第一个遍历子结点
				hItem = pCtrl->GetChildItem(hItem);

				//	开始下一个层次的循环匹配
				continue;
			}
			
			//	继续本层次的其它兄弟结点中匹配
			hItem = pCtrl->GetNextSiblingItem(hItem);
		}

		break;
	}

	return hItemRet;
}

bool CListDlg::Find(CString name, CDlgFindTemplate::FIND_TYPE type, CDlgFindTemplResult::FindResult &result, CString &strColumnName)
{
	bool bRet(false);

	result.clear();

	if (!name.IsEmpty())
	{
		AWString strPattern = L"*";
		strPattern += AS2WC(AString(name));
		strPattern += L"*";

		BaseDataIDManBase & BaseIDMan = GetBaseIDManByFindType(type);
		AWStringWithWildcard matcher;
		CDlgFindTemplResult::FindItem item;

		//	先尝试按模板在游戏中的名称查找
		{
			//	先检查并重新初始化查找列表
			if (m_bNeedReInitSearch)
			{
				ReInitSearchImpl(false);
				m_bNeedReInitSearch = false;
			}

			//	再进行查找
			ID2NameMap &id2name = GetID2NameMapByFindType(type);
			unsigned long id(0);
			for (ID2NameMap::iterator it = id2name.begin(); it != id2name.end(); ++ it)
			{
				const AString &strPath = it->second;
				if (matcher.IsMatch(AS2WC(strPath), strPattern) &&	IsCanOpen(it->first, type))
				{
					item.uID = it->first;
					item.strName = strPath;
					item.strPath = BaseIDMan.GetPathByID(it->first);
					result.push_back(item);
				}
			}
			if (!result.empty())
			{
				strColumnName = "场景名";
				return	true;
			}
		}

		strColumnName = "文件路径名";

		//	再尝试按模板文件名查找
		AString strExt = GetExtByFindType(type);
		int nExt = strExt.GetLength();
		
		for (BaseDataIDPathMap::const_iterator it = BaseIDMan.begin_id(); it != BaseIDMan.end_id(); ++ it)
		{
			unsigned long id = it->first;
			AString strPath = it->second;

			//	除去开始路径
			int pos = strPath.ReverseFind('\\');
			if (pos < 0 || pos >= strPath.GetLength())
				continue;
			strPath.CutLeft(pos+1);

			//	除去.tmpl扩展名( .ext扩展名显示在树形结构中)
			if (strPath.GetLength() <= nExt)
				break;
			AString strRight = strPath.Right(nExt);
			if (strRight.CompareNoCase(strExt))
				continue;
			if (GetDelExtByFindType(type))
				strPath.CutRight(nExt);

			//	匹配
			if (matcher.IsMatch(AS2WC(strPath), strPattern) &&	IsCanOpen(it->first, type))
			{
				item.uID = id;
				item.strName = strPath;
				item.strPath = BaseIDMan.GetPathByID(id);
				result.push_back(item);
			}
		}
	}

	return !result.empty();
}

bool CListDlg::IsCanOpen(unsigned long uID, CDlgFindTemplate::FIND_TYPE type)
{
	//	判断给定 id 的模板是否是最底层的数据模板
	bool bRet(false);

	while (true)
	{
		BaseDataIDManBase & BaseIDMan = GetBaseIDManByFindType(type);
		CString strBaseDataPath = BaseIDMan.GetPathByID(uID);
		if (strBaseDataPath.IsEmpty())
			break;

		CString strFullPath = CString(g_szWorkDir) + strBaseDataPath;
		if (!af_IsFileExist(strFullPath))
			break;

		if (type == CDlgFindTemplate::FT_EXTDATA)
		{
			//	.ext 模板直接能打开
			bRet = true;
			break;
		}
		
		//	确定文件合法
		CString strExt = GetExtByFindType(type);
		CString strPrefix = "BaseData\\";
		if (strBaseDataPath.Right(strExt.GetLength()) != strExt ||
			strBaseDataPath.Left(strPrefix.GetLength()) != strPrefix)
			break;

		//	处理 .tmpl 等其它模板

		//	计算 .tmpl 文件距离 org 的目录层次
		int nTemplLevel(0);
		do 
		{
			//	计算上层目录
			int nPos = strBaseDataPath.ReverseFind('\\');
			if (nPos == -1)
			{
				ASSERT(false);
				return false;
			}
			strBaseDataPath = strBaseDataPath.Left(nPos);
			CString strDir = CString(g_szWorkDir) + strBaseDataPath;
			CString strPattern = strDir + "\\*.org";

			CFileFind finder;
			BOOL bNext = finder.FindFile(strPattern);
			if (!bNext)
			{
				//	本目录层次没有 org 文件，继续查找上层目录
				finder.Close();
				nTemplLevel ++;
				continue;
			}
			finder.Close();
			break;

		} while(true);

		if (nTemplLevel == 0)	break;

		//	计算 org 允许的层次
		int nMaxOrgLevel(0);

		CString strDir = CString(g_szWorkDir) + strBaseDataPath;
		CString strPattern = strDir + "\\*.org";
		CFileFind finder;
		BOOL bNext = finder.FindFile(strPattern);
		while (bNext)
		{
			bNext = finder.FindNextFile();
			if (finder.IsDirectory())
				continue;

			int nLevel(0);
			CString strTitle = finder.GetFileTitle();

			if (sscanf(strTitle, "%d", &nLevel) != 1)	continue;
			if (nLevel < 0)	continue;

			CString strTemp;
			strTemp.Format("%d", nLevel);
			if (strTemp.CompareNoCase(strTitle))	continue;
			if (nLevel > nMaxOrgLevel)
				nMaxOrgLevel = nLevel;
		}
		finder.Close();

		if (nMaxOrgLevel == 0)
			break;

		//	判定
		if (nTemplLevel != nMaxOrgLevel)
			break;

		bRet = true;
		break;
	}

	return bRet;
}

void CListDlg::ReInitSearch()
{
	m_bNeedReInitSearch = true;
}

static unsigned int __stdcall Thread_ReInitSearchImpl(void *pParam)
{
	CListDlg *pListDlg = (CListDlg *)pParam;
	
	for (int i = 0; i < CDlgFindTemplate::FT_NUM; ++ i)
	{
		CDlgFindTemplate::FIND_TYPE type = (CDlgFindTemplate::FIND_TYPE)i;
		CListDlg::ID2NameMap &id2name = pListDlg->GetID2NameMapByFindType(type);
		
		id2name.clear();
		
		//	逐个添加 id 到名称的映射
		BaseDataIDManBase & BaseIDMan = GetBaseIDManByFindType(type);		
		for (BaseDataIDPathMap::const_iterator it = BaseIDMan.begin_id(); it != BaseIDMan.end_id(); ++ it)
		{
			pListDlg->AddSearch(it->second, id2name);
			CDlgProgressNotify::m_nCurProgress ++;
		}
	}
	
	pListDlg->SaveSearchToFile();

	return 0;
}

void CListDlg::ReInitSearchImpl(bool bUpdateSearchFile)
{
	int i(0);
	static bool s_bFirst(true);

	if (s_bFirst)
	{
		//	启动后首次加载
		s_bFirst = false;

		//	根据参数、先尝试从记录文件中加载，以减少加载时间
		if (!bUpdateSearchFile && LoadSearchFromFile())
			return;

		//	需要更新文件，或者从文件加载失败

		//	不从记录文件中加载、或加载失败时，顺便同步记录文件
		bUpdateSearchFile = true;
	}

	if (!bUpdateSearchFile)
	{
		//	不需要更新记录文件时，忽略任何处理
		return;
	}

	//	使用另外线程和进度通知处理
	CDlgProgressNotify dlgProgress;
	
	int &nProgressMax = CDlgProgressNotify::m_nProgressMax;
	nProgressMax = 0;
	for (i = 0; i < CDlgFindTemplate::FT_NUM; ++ i)
	{
		CDlgFindTemplate::FIND_TYPE type = (CDlgFindTemplate::FIND_TYPE)i;
		BaseDataIDManBase & BaseIDMan = GetBaseIDManByFindType(type);
		nProgressMax += BaseIDMan.GetIdNum();
	}
	CDlgProgressNotify::m_nCurProgress = 0;
	CDlgProgressNotify::m_strWndText = "正在同步场景名，请稍等...";

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_ReInitSearchImpl, this, 0, NULL);
	dlgProgress.DoModal();
	::WaitForSingleObject((void *)hThread, INFINITE);
}

void CListDlg::AddSearch(const AString &strPathName, CDlgFindTemplate::FIND_TYPE type)
{
	ID2NameMap &id2name = GetID2NameMapByFindType(type);
	AddSearch(strPathName, id2name);
}

void CListDlg::AddSearch(const AString &strPathName, ID2NameMap &id2name)
{
	AFileImage file;

	if (!file.Open(strPathName, AFILE_OPENEXIST | AFILE_TEXT))
		return;	
	
	//static const char* _format_version	= "Version: %u";
	static const char* _format_id		= "ID: %u";
	static const char* _format_tmpl_name= "TemplName: %s";
	
	static char	szLine[AFILE_LINEMAXLEN];
	static char	szBuf[AFILE_LINEMAXLEN];
	DWORD	dwRead;

	//DWORD	dwVersion = 0;
	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	//sscanf(szLine, _format_version, &dwVersion);

	unsigned long id(0);
	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_id, &id);

	szBuf[0] = '\0';
	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_tmpl_name, szBuf);

	id2name[id] = szBuf;

	file.Close();
}

CListDlg::ID2NameMap & CListDlg::GetID2NameMapByFindType(CDlgFindTemplate::FIND_TYPE type)
{
	switch(type)
	{
	case CDlgFindTemplate::FT_TEMPL:			return	m_tmplID2Name;
	case CDlgFindTemplate::FT_EXTDATA:	return m_extID2Name;
	case CDlgFindTemplate::FT_RECIPE:		return m_recipeID2Name;
	case CDlgFindTemplate::FT_CONFIG:		return m_configID2Name;
	case CDlgFindTemplate::FT_FACE:			return m_faceID2Name;
	default:
		assert(0);
	}

	return m_tmplID2Name;
}

AString GetSearchFileNameByFindType(CDlgFindTemplate::FIND_TYPE type)
{
	static AString s_strExePath;
	if (s_strExePath.IsEmpty())
	{
		//	首次调用此函数
		char szPath[MAX_PATH];
		GetModuleFileNameA(NULL, szPath, sizeof(szPath)/sizeof(szPath[0]));
		s_strExePath = szPath;
		int nPos = s_strExePath.ReverseFind('\\');
		if (nPos < 0 || nPos >= s_strExePath.GetLength())
			nPos = s_strExePath.ReverseFind('/');
		if (nPos >= 0 && nPos < s_strExePath.GetLength())
			s_strExePath.CutRight(s_strExePath.GetLength() - nPos - 1);
	}

	AString strName;
	switch(type)
	{
	case CDlgFindTemplate::FT_TEMPL:			strName = "TemplID.log";	break;
	case CDlgFindTemplate::FT_EXTDATA:	strName =  "ExtDataID.log";	break;
	case CDlgFindTemplate::FT_RECIPE:		strName =  "RecipeID.log";	break;
	case CDlgFindTemplate::FT_CONFIG:		strName =  "ConfigID.log";	break;
	case CDlgFindTemplate::FT_FACE:			strName =  "FaceID.log";	break;
	default:
		assert(0);
		strName =  "TemplID.log";
		break;
	}

	return s_strExePath + strName;
}

void CListDlg::AddToSearchFile(const AString &strPathName, CDlgFindTemplate::FIND_TYPE type)
{
	//	获取 id 和 name

	AFile file;

	if (!file.Open(strPathName, AFILE_OPENEXIST | AFILE_TEXT))
		return;	
	
	//static const char* _format_version	= "Version: %u";
	static const char* _format_id		= "ID: %u";
	static const char* _format_tmpl_name= "TemplName: %s";
	
	static char	szLine[AFILE_LINEMAXLEN];
	static char	szBuf[AFILE_LINEMAXLEN];
	DWORD	dwRead;

	//DWORD	dwVersion = 0;
	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	//sscanf(szLine, _format_version, &dwVersion);

	unsigned long id(0);
	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_id, &id);

	szBuf[0] = '\0';
	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_tmpl_name, szBuf);

	file.Close();

	//	写入文件
	if (!file.Open(GetSearchFileNameByFindType(type), AFILE_NOHEAD | AFILE_OPENAPPEND | AFILE_TEXT))
	{
		ASSERT(false);
		return;
	}

	AString strNewLine;
	strNewLine.Format("%d, \"%s\"", id, szBuf);

	file.WriteLine(strNewLine);

	file.Close();
}

void CListDlg::SaveSearchToFile()
{
	for (int i = 0; i < CDlgFindTemplate::FT_NUM; ++ i)
	{
		CDlgFindTemplate::FIND_TYPE type = (CDlgFindTemplate::FIND_TYPE)i;
		ID2NameMap &id2name = GetID2NameMapByFindType(type);
		AString strFileName = GetSearchFileNameByFindType(type);

		AFile file;
		if (!file.Open(strFileName, AFILE_NOHEAD | AFILE_CREATENEW | AFILE_TEXT))
		{
			ASSERT(false);
			continue;
		}

		AString strNewLine;
		for (ID2NameMap::iterator it = id2name.begin(); it != id2name.end(); ++ it)
		{
			strNewLine.Format("%d, \"%s\"", it->first, it->second);
			file.WriteLine(strNewLine);
		}

		file.Close();
	}
}

bool CListDlg::LoadSearchFromFile()
{
	bool bOK(false);

	int i(0);
	
	//	清除已有内容
	for (i = 0; i < CDlgFindTemplate::FT_NUM; ++ i)
	{
		CDlgFindTemplate::FIND_TYPE type = (CDlgFindTemplate::FIND_TYPE)i;
		ID2NameMap &id2name = GetID2NameMapByFindType(type);
		id2name.clear();
	}
	
	//	从文件中加载
	for (i = 0; i < CDlgFindTemplate::FT_NUM; ++ i)
	{
		CDlgFindTemplate::FIND_TYPE type = (CDlgFindTemplate::FIND_TYPE)i;
		ID2NameMap &id2name = GetID2NameMapByFindType(type);
		AString strFileName = GetSearchFileNameByFindType(type);
		
		AFileImage file;
		if (!file.Open(strFileName, AFILE_NOHEAD | AFILE_OPENEXIST | AFILE_TEXT))
			break;		
		
		static char	szLine[AFILE_LINEMAXLEN];
		static char	szBuf[AFILE_LINEMAXLEN];
		DWORD	dwRead;
		unsigned long uID(0);
		AString strTemp;
		
		while (file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead))
		{
			if (dwRead > 0)
			{
				szBuf[0] = '\0';
				if (2 == sscanf(szLine, "%d, %s", &uID, szBuf))
				{
					strTemp = szBuf;
					if (strTemp.GetLength()>=2 && strTemp[0] == '\"' && strTemp[strTemp.GetLength()-1] == '\"')
					{
						strTemp.CutLeft(1);
						strTemp.CutRight(1);
					}
					if (!strTemp.IsEmpty())
						id2name[uID] = strTemp;
				}
			}
		}

		file.Close();
	}

	if (i == CDlgFindTemplate::FT_NUM)
	{
		bOK = true;
	}
	else
	{
		//	加载失败，清除已有内容
		for (i = 0; i < CDlgFindTemplate::FT_NUM; ++ i)
		{
			CDlgFindTemplate::FIND_TYPE type = (CDlgFindTemplate::FIND_TYPE)i;
			ID2NameMap &id2name = GetID2NameMapByFindType(type);
			id2name.clear();
		}
	}

	return bOK;
}

void CListDlg::OnRclickTreeSortList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint point;
	GetCursorPos(&point);
	POINT pt = {point.x, point.y};
	m_Tree.ScreenToClient(&pt);
	CPoint client(pt.x,pt.y);
	pt.x = point.x;
	pt.y = point.y;

	UINT uFlags;
	HTREEITEM itemp;
	
	if(m_Tree.GetSelectedCount() < 2)
	{
		itemp = m_Tree.HitTest(client,&uFlags);
		if(itemp == NULL || !(TVHT_ONITEM & uFlags)) return; 
		
		CString path = m_Tree.GetItemText(itemp);
		while(itemp = m_Tree.GetParentItem(itemp))
		{
			CString pathsub = m_Tree.GetItemText(itemp);
			path = pathsub + "\\" + path;	
		}
		path = "BaseData\\" + path;
		bool bPass = false;
		for (ORG_PATHS::iterator n = _org_paths.begin(); n != _org_paths.end(); ++n)
		{
			AString strOrgPath = n->first;
			
			if(path.Find(strOrgPath) != -1)
			{
				bPass = true;
			}
		}
		if(!bPass) return;
	}

	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_CREATE_OBJECT);
	
	CMenu* pPopupMenu = Menu.GetSubMenu(0);
	ASSERT(pPopupMenu);
	
	
	UpdateMenu(pPopupMenu);
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
	
	*pResult = 1; 
}

void CListDlg::OnCreateObject() 
{
	// TODO: Add your command handler code here
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	HTREEITEM oldItem = itemp;
	CString path = pCtrl->GetItemText(itemp);
	while(itemp = pCtrl->GetParentItem(itemp))
	{
		CString pathsub = pCtrl->GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}

	BaseDataIDManBase* pIDMan;
	if (path.Left(4) == "Face")
		pIDMan = &g_FaceIDMan;
	else if (path.Left(4) == "配方")
		pIDMan = &g_RecipeIDMan;
	else if (path.Left(4) == "镌刻")
		pIDMan = &g_RecipeIDMan;
	else if (path.Left(10) == "属性值随机")
		pIDMan = &g_RecipeIDMan;
	else if (path.Left(8) == "宝石转化")
		pIDMan = &g_RecipeIDMan;
	else if (path.Left(6) == "Config")
		pIDMan = &g_ConfigIDMan;
	else
		pIDMan = &g_BaseIDMan;

	path = "BaseData\\" + path;
	
	CTemplateCreateDlg dlg;
	dlg.m_strPath = path;
	if(dlg.DoModal()==IDOK)
	{
		dlg.m_strName.TrimLeft();
		dlg.m_strName.TrimRight();

		if(dlg.m_strName.Find("\\") != -1 ||
			dlg.m_strName.IsEmpty() || 
			dlg.m_strName.ReverseFind('.') >= 0)
		{
			MessageBox("模板创建失败,模板名字不能为空或含有\\.字符");
			return;
		}

		BaseDataTempl tmp;
		CString folder = path + "\\" +dlg.m_strName;
		tmp.SetName(LPCTSTR(dlg.m_strName));
		if(!tmp.CreateTempl(folder + "\\" + dlg.m_strName + ".tmpl","", pIDMan))
		{
			MessageBox("创建基本模板失败!");
			return;
		}
		if(0 != CreateDirectory(folder,NULL))
		{
			if (!dlg.m_strSrcTempl.IsEmpty())
			{
				BaseDataTempl src;
				if (src.Load(dlg.m_strSrcTempl) == 0)
				{
					tmp = src;
					src.Release();
				}
			}
			tmp.Save(folder + "\\" + dlg.m_strName + ".tmpl");
			tmp.Release();
#ifdef _MD5_CHECK
			WriteMd5CodeToTemplate(folder + "\\" + dlg.m_strName + ".tmpl");
#endif
		}else 
		{
			CString msg("创建文件夹失败--");
			msg += folder;
			MessageBox(msg);
			return;
		}
		//HTREEITEM hNewItem = pCtrl->InsertItem(dlg.m_strName,oldItem,TVI_SORT);
		//
		HTREEITEM insertAfter = TVI_FIRST;
		HTREEITEM hNewItem;
		HTREEITEM child = pCtrl->GetChildItem(oldItem);
		while(child)
		{
			CString ItemName = pCtrl->GetItemText(child);
			if( dlg.m_strName == "附加属性" )
			{
				insertAfter = TVI_FIRST;
				break;
			}
			else if(stricmp(ItemName, dlg.m_strName) < 0 || "附加属性" == ItemName)
			{
				insertAfter = child;
				child = pCtrl->GetNextItem(child,TVGN_NEXT);
				continue;
			}
			else
				break;
		}
		hNewItem = pCtrl->InsertItem(dlg.m_strName,oldItem,insertAfter);
		pCtrl->SelectItem(hNewItem);
		//
		if(!IsCreateFolder(path)) 
		{
			pCtrl->SetItemImage(hNewItem,5,5);
		}else
		{
			pCtrl->SetItemImage(hNewItem,3,3);
		}
		//把文件放到服务器上
		CString strPathName = path +"\\"+ dlg.m_strName + "\\" + dlg.m_strName + ".tmpl";
		g_VSS.SetProjectPath(path);
		g_VSS.CreateProject(dlg.m_strName);
		g_VSS.SetProjectPath(path + "\\" + dlg.m_strName);
		g_VSS.AddFile(strPathName);
		g_VSS.CheckOutFile(strPathName);
		CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp *)AfxGetApp())->OpenDocumentFile(strPathName);
		if (pDoc){
			//	利用打开重新设置模板的各项值 AVariant::m_iType 获取有意义的初始值并上传VSS
			pDoc->Save();
			g_VSS.CheckInFile(dlg.m_strName + ".tmpl");
			g_VSS.CheckOutFile(strPathName);
		}

		//	添加到可查找列表
		CDlgFindTemplate::FIND_TYPE type = GetFindTypeByBaseIDMan(pIDMan); 
		AddSearch(AString(strPathName), type);
		AddToSearchFile(AString(strPathName), type);
	}
}

void CListDlg::OnClickTreeProperty(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CListDlg::OnDblclkTreeSortList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0; 
}

void CListDlg::OnRdblclkTreeProperty(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CListDlg::OnRdblclkTreeSortList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CListDlg::OnCreateObjectExt() 
{
	// TODO: Add your command handler code here
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	HTREEITEM oldItem = itemp;
	CString path = pCtrl->GetItemText(itemp);
	while(itemp = pCtrl->GetParentItem(itemp))
	{
		CString pathsub = pCtrl->GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;
	
	CTemplateExtDlg dlg;
	dlg.m_strDir = path;
	if(dlg.DoModal()==IDOK)
	{
		if(dlg.m_strName.IsEmpty()) 
		{
			MessageBox("模板创建失败,模板名字不能为空");
			return;
		}
		ExtendDataTempl extTmp;
		extTmp.SetName(LPCTSTR(dlg.m_strName));
		path = path + "\\附加属性";
		CString newPath = path;
		path = path + "\\" + dlg.m_strName + ".ext";
		if(!extTmp.CreateTempl(path,dlg.m_strName, &g_ExtBaseIDMan))
		{
			MessageBox("创建附加属性失败!");
			return;	
		}
		AString itemName[4] = {dlg.m_strItemName1,dlg.m_strItemName2,dlg.m_strItemName3,dlg.m_strItemName4};
		for(int i = 0; i< dlg.m_nItemNum; i++)
		{
			AString itemType(dlg.m_strItemType[i]);
			extTmp.AddItem(itemName[i],itemType);
		}
		extTmp.Save(path);
#ifdef _MD5_CHECK
		WriteMd5CodeToTemplate(path);
#endif
		extTmp.Release();
		//刷新列表
		itemp = oldItem;
		itemp = pCtrl->GetChildItem(itemp);
		CString strName = pCtrl->GetItemText(itemp);
		if(stricmp(strName,"附加属性")!=0)
		{
			while(itemp)
			{
				itemp = pCtrl->GetNextItem(itemp,TVGN_NEXT);
				strName = pCtrl->GetItemText(itemp);
				if(stricmp(strName,"附加属性")==0)
					break;
			}
		}
		//HTREEITEM hNewItem = pCtrl->InsertItem(dlg.m_strName + ".ext",itemp,TVI_SORT);
		HTREEITEM insertAfter = TVI_FIRST;
		HTREEITEM hNewItem;
		HTREEITEM child = pCtrl->GetChildItem(itemp);
		CString strNewItemName = dlg.m_strName + ".ext";
		while(child)
		{
			CString ItemName = pCtrl->GetItemText(child);
			if( strNewItemName == "附加属性" )
			{
				insertAfter = TVI_FIRST;
				break;
			}
			else if(stricmp(ItemName, strNewItemName) < 0 || "附加属性" == ItemName)
			{
				insertAfter = child;
				child = pCtrl->GetNextItem(child,TVGN_NEXT);
				continue;
			}
			else
				break;
		}
		hNewItem = pCtrl->InsertItem(strNewItemName,itemp,insertAfter);
		pCtrl->SetItemImage(hNewItem,5,5);
		pCtrl->SelectItem(hNewItem);
		//把文件放到服务器上
		g_VSS.SetProjectPath("");
		g_VSS.SetProjectPath(newPath);
		g_VSS.AddFile(path);
		g_VSS.CheckOutFile(path);
		(CElementDataDoc *)((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path);

		//	添加到可查找列表		
		CDlgFindTemplate::FIND_TYPE type = CDlgFindTemplate::FT_EXTDATA;
		AddSearch(AString(path), type);
		AddToSearchFile(AString(path), type);
	}
}

void CListDlg::OnUpdateCreateObjectExt(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
}

void CListDlg::OnUpdateCreateObject(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
}

void CListDlg::OnVssCheckIn() 
{
	// TODO: Add your command handler code here
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	HTREEITEM oldItem = itemp;
	CString path;
	CString fileName = pCtrl->GetItemText(itemp);
	
	while(itemp = pCtrl->GetParentItem(itemp))
	{
		CString pathsub = pCtrl->GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;
	int num = path.GetLength();
	path = path.Left(num-1);
	
	bool bIsFolder = false;
	if(fileName.Find(".tmpl")==-1 && fileName.Find(".ext")==-1)
	{//不是文件
		path = path + "\\" + fileName;
		fileName = fileName + ".tmpl";
		if(!FileIsExist(AString(path + "\\" + fileName))) return;
		bIsFolder = true;
	}

	CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp*)AfxGetApp())->GetDocument(path + "\\" + fileName);
	if(pDoc) pDoc->OnSaveDocument(path + "\\" + fileName);
	
	g_VSS.SetProjectPath(path);
	g_VSS.CheckInFile(fileName);

	if(bIsFolder)
	{
		if(!IsHideFiles(path))
		{
			if(FileIsReadOnly(AString(path + "\\" + fileName)))
			{
				pCtrl->SetItemImage(oldItem,4,4);
			}else
			{
				pCtrl->SetItemImage(oldItem,5,5);
			}
		}else
		{
			if(FileIsReadOnly(AString(path + "\\" + fileName)))
			{
				pCtrl->SetItemImage(oldItem,2,2);
			}else
			{
				pCtrl->SetItemImage(oldItem,3,3);
			}	
		}
	}else
	{
		if(FileIsReadOnly(AString(path + "\\" + fileName)))
		{
			pCtrl->SetItemImage(oldItem,4,4);
		}else
		{
			pCtrl->SetItemImage(oldItem,5,5);
		}
	}
	
	if(pDoc)
	{
		if(pDoc) pDoc->OnCloseDocument();
		((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path + "\\" + fileName);
	}
}

void CListDlg::EnumCheck(HTREEITEM root, bool bCheckOut, bool bAppend)
{
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	bool pass = pCtrl->GetItemState(root,TVIS_SELECTED) & TVIS_SELECTED || bAppend;
	if(pass)
	{
		HTREEITEM itemp = root;
		HTREEITEM oldItem = root;
		CString path;
		CString fileName = pCtrl->GetItemText(itemp);
		
		while(itemp = pCtrl->GetParentItem(itemp))
		{
			CString pathsub = pCtrl->GetItemText(itemp);
			path = pathsub + "\\" + path;	
		}
		path = "BaseData\\" + path;
		int num = path.GetLength();
		path = path.Left(num-1);
		
		bool bIsFolder = false;
		if(fileName.Find(".tmpl")==-1 && fileName.Find(".ext")==-1)
		{//不是文件
			path = path + "\\" + fileName;
			fileName = fileName + ".tmpl";
			if(!FileIsExist(AString(path + "\\" + fileName))) goto sub_1;
			bIsFolder = true;
		}
		
		if(bCheckOut)
		{
			g_VSS.SetProjectPath(path);
			g_VSS.CheckOutFile(path + "\\" + fileName);
			
			if(bIsFolder)
			{
				if(!IsHideFiles(path))
				{
					if(FileIsReadOnly(AString(path + "\\" + fileName)))
					{
						pCtrl->SetItemImage(oldItem,4,4);
					}else
					{
						pCtrl->SetItemImage(oldItem,5,5);
					}
				}else
				{
					if(FileIsReadOnly(AString(path + "\\" + fileName)))
					{
						pCtrl->SetItemImage(oldItem,2,2);
					}else
					{
						pCtrl->SetItemImage(oldItem,3,3);
					}	
				}
			}else
			{
				if(FileIsReadOnly(AString(path + "\\" + fileName)))
				{
					pCtrl->SetItemImage(oldItem,4,4);
				}else
				{
					pCtrl->SetItemImage(oldItem,5,5);
				}
			}
			
			CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp*)AfxGetApp())->GetDocument(path + "\\" + fileName);
			if(pDoc)
			{
				if(pDoc) pDoc->OnCloseDocument();
				((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path + "\\" + fileName);
			}	
		}else
		{
			CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp*)AfxGetApp())->GetDocument(path + "\\" + fileName);
			if(pDoc)
			{
				//if(MessageBox("是否保存你对该文档所做的修改？","询问",MB_YESNO)==IDYES)
				//{
					pDoc->OnSaveDocument(path + "\\" + fileName);
				//}
				//pDoc->OnCloseDocument();
			}
			g_VSS.SetProjectPath(path);
			g_VSS.CheckInFile(fileName);
			
			if(bIsFolder)
			{
				if(!IsHideFiles(path))
				{
					if(FileIsReadOnly(AString(path + "\\" + fileName)))
					{
						pCtrl->SetItemImage(oldItem,4,4);
					}else
					{
						pCtrl->SetItemImage(oldItem,5,5);
					}
				}else
				{
					if(FileIsReadOnly(AString(path + "\\" + fileName)))
					{
						pCtrl->SetItemImage(oldItem,2,2);
					}else
					{
						pCtrl->SetItemImage(oldItem,3,3);
					}	
				}
			}else
			{
				if(FileIsReadOnly(AString(path + "\\" + fileName)))
				{
					pCtrl->SetItemImage(oldItem,4,4);
				}else
				{
					pCtrl->SetItemImage(oldItem,5,5);
				}
			}
			
			//if(pDoc) ((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path + "\\" + fileName);
		}
	}
	
	//Child item
sub_1:
	HTREEITEM hChild = m_Tree.GetChildItem(root);
	while (hChild)
	{
		EnumCheck(hChild,bCheckOut,pass);
		hChild = m_Tree.GetNextItem(hChild, TVGN_NEXT);
	}	
}

void CListDlg::UpdateMenu(CMenu *pMenu)
{
	if(m_Tree.GetSelectedCount() >1)
	{
		pMenu->EnableMenuItem(ID_CREATE_OBJECT_EXT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_CREATE_OBJECT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_GRAYED);
		pMenu->EnableMenuItem(ID_VSS_UNDO_CHECK_OUT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_VSS_CHECK_IN_ALL,MF_ENABLED);
		pMenu->EnableMenuItem(ID_VSS_CHECK_OUT_ALL,MF_ENABLED);
		pMenu->EnableMenuItem(ID_EDIT_TEMPLATE_PROPERTY,MF_GRAYED);
		pMenu->EnableMenuItem(ID_MODIFIED_OBJECT_EXT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_AUTO_RESAVE_TEMPLATE,MF_ENABLED);
		return;
	}else
	{
		pMenu->EnableMenuItem(ID_VSS_CHECK_IN_ALL,MF_GRAYED);
		pMenu->EnableMenuItem(ID_VSS_CHECK_OUT_ALL,MF_GRAYED);
		pMenu->EnableMenuItem(ID_AUTO_RESAVE_TEMPLATE,MF_GRAYED);
	}
	
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	HTREEITEM oldItem = itemp;
	CString itemText = pCtrl->GetItemText(itemp);
	CString path = itemText;

	HTREEITEM hChild = itemp;
	if(NULL!=pCtrl->GetChildItem(hChild))
	{
		pMenu->EnableMenuItem(ID_VSS_CHECK_IN_ALL,MF_ENABLED);
		pMenu->EnableMenuItem(ID_VSS_CHECK_OUT_ALL,MF_ENABLED);
		pMenu->EnableMenuItem(ID_AUTO_RESAVE_TEMPLATE,MF_ENABLED);
	}

	if(stricmp("附加属性",path)==0)
	{
		pMenu->EnableMenuItem(ID_CREATE_OBJECT_EXT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_CREATE_OBJECT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_GRAYED);
		pMenu->EnableMenuItem(ID_VSS_UNDO_CHECK_OUT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_EDIT_TEMPLATE_PROPERTY,MF_GRAYED);
		pMenu->EnableMenuItem(ID_MODIFIED_OBJECT_EXT,MF_GRAYED);
		return;
	}
	
	while(itemp = pCtrl->GetParentItem(itemp))
	{
		CString pathsub = pCtrl->GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;

	if(path.Find(".tmpl")!=-1 || path.Find(".ext")!=-1)
	{
		pMenu->EnableMenuItem(ID_CREATE_OBJECT,MF_GRAYED);
		
		if(FileIsReadOnly(AString(path)))
		{
			pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_ENABLED);
			pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_GRAYED);
			pMenu->EnableMenuItem(ID_VSS_UNDO_CHECK_OUT,MF_GRAYED);
		}else 
		{
			pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_GRAYED);
			pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_ENABLED);
			pMenu->EnableMenuItem(ID_VSS_UNDO_CHECK_OUT,MF_ENABLED);
		}
		pMenu->EnableMenuItem(ID_EDIT_TEMPLATE_PROPERTY,MF_ENABLED);
	}else 
	{
		if(FileIsExist(AString(path + "\\" + itemText + ".tmpl")))
		{
			if(FileIsReadOnly(AString(path + "\\" + itemText + ".tmpl")))
			{
				pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_ENABLED);
				pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_GRAYED);
				pMenu->EnableMenuItem(ID_VSS_UNDO_CHECK_OUT,MF_GRAYED);
			}else 
			{
				pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_GRAYED);
				pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_ENABLED);
				pMenu->EnableMenuItem(ID_VSS_UNDO_CHECK_OUT,MF_ENABLED);
			}
			pMenu->EnableMenuItem(ID_EDIT_TEMPLATE_PROPERTY,MF_ENABLED);
		}else
		{
			pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_GRAYED);
			pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_GRAYED);
			pMenu->EnableMenuItem(ID_VSS_UNDO_CHECK_OUT,MF_GRAYED);
			pMenu->EnableMenuItem(ID_EDIT_TEMPLATE_PROPERTY,MF_GRAYED);
		}
		
		if(!IsHideFiles(path))
		{
			pMenu->EnableMenuItem(ID_CREATE_OBJECT,MF_GRAYED);
		}else pMenu->EnableMenuItem(ID_CREATE_OBJECT,MF_ENABLED);
	}

	bool bPass = false;
	for (ORG_PATHS::iterator n = _org_paths.begin(); n != _org_paths.end(); ++n)
	{
		AString strOrgPath = n->first;
		
		if(stricmp(path,strOrgPath) == 0)
		{
			bPass = true;
		}
	}
	
	if(bPass)
	{
		bPass = false;
		itemp = oldItem;
		HTREEITEM child = pCtrl->GetChildItem(itemp);
		CString txt = pCtrl->GetItemText(child);
		if(stricmp(txt,"附加属性")==0) bPass = true;
		while(child && !bPass)
		{
			child = pCtrl->GetNextItem(child,TVGN_NEXT);
			if(child)
			{
				txt = pCtrl->GetItemText(child);
				if(stricmp(txt,"附加属性")==0)
				{
					bPass = true;
					break;
				}
			}
		}
	}
	if(!bPass) pMenu->EnableMenuItem(ID_CREATE_OBJECT_EXT,MF_GRAYED);
	else pMenu->EnableMenuItem(ID_CREATE_OBJECT_EXT,MF_ENABLED);

	if(itemText.Find(".ext")!=-1)
	{	if(!FileIsReadOnly(AString(path)))
			pMenu->EnableMenuItem(ID_MODIFIED_OBJECT_EXT,MF_ENABLED);
		else pMenu->EnableMenuItem(ID_MODIFIED_OBJECT_EXT,MF_GRAYED);
	}
	else pMenu->EnableMenuItem(ID_MODIFIED_OBJECT_EXT,MF_GRAYED);
}

void CListDlg::OnVssCheckOut() 
{
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	HTREEITEM oldItem = itemp;
	CString path;
	CString fileName = pCtrl->GetItemText(itemp);
	
	while(itemp = pCtrl->GetParentItem(itemp))
	{
		CString pathsub = pCtrl->GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;
	int num = path.GetLength();
	path = path.Left(num-1);
	
	bool bIsFolder = false;
	if(fileName.Find(".tmpl")==-1 && fileName.Find(".ext")==-1)
	{//不是文件
		path = path + "\\" + fileName;
		fileName = fileName + ".tmpl";
		if(!FileIsExist(AString(path + "\\" + fileName))) return;
		bIsFolder = true;
	}
	
	g_VSS.SetProjectPath(path);
	g_VSS.CheckOutFile(path + "\\" + fileName);

	if(bIsFolder)
	{
		if(!IsHideFiles(path))
		{
			if(FileIsReadOnly(AString(path + "\\" + fileName)))
			{
				pCtrl->SetItemImage(oldItem,4,4);
			}else
			{
				pCtrl->SetItemImage(oldItem,5,5);
			}
		}else
		{
			if(FileIsReadOnly(AString(path + "\\" + fileName)))
			{
				pCtrl->SetItemImage(oldItem,2,2);
			}else
			{
				pCtrl->SetItemImage(oldItem,3,3);
			}	
		}
	}else
	{
		if(FileIsReadOnly(AString(path + "\\" + fileName)))
		{
			pCtrl->SetItemImage(oldItem,4,4);
		}else
		{
			pCtrl->SetItemImage(oldItem,5,5);
		}
	}

	CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp*)AfxGetApp())->GetDocument(path + "\\" + fileName);
	if(pDoc)
	{
		if(pDoc) pDoc->OnCloseDocument();
		((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path + "\\" + fileName);
	}
}

bool CListDlg::IsCreateFolder(CString path)
{
	CString str;
	for (ORG_PATHS::iterator n = _org_paths.begin(); n != _org_paths.end(); ++n)
	{
		AString strOrgPath = n->first;
		
		if(path.Find(strOrgPath) != -1)
		{
			str = strOrgPath;
			break;
		}
	}
	if(str.IsEmpty()) return false;
	
	CString temp(path);
	temp.Replace(str,"");
	
	int num = 0;
	while(1)
	{
		int index = temp.Find("\\");
		if(index == -1)
			break;
		temp.Delete(0,index + 1);
		num++;
	}
	num = num + 2;

	CString org;
	org.Format("%d.org",num);
	org = str + "\\" + org;
	AFile file;
	if(file.Open(org,AFILE_OPENEXIST))
	{
		file.Close();
		return true;
	}

	return false;
}

bool CListDlg::IsHideFiles(CString path)
{
	CString str;
	for (ORG_PATHS::iterator n = _org_paths.begin(); n != _org_paths.end(); ++n)
	{
		AString strOrgPath = n->first;
		
		if(path.Find(strOrgPath) != -1)
		{
			str = strOrgPath;
			break;
		}
	}
	if(str.IsEmpty()) return false;
	
	CString temp(path);
	temp.Replace(str,"");
	
	int num = 0;
	while(1)
	{
		int index = temp.Find("\\");
		if(index == -1)
			break;
		temp.Delete(0,index + 1);
		num++;
	}
	num = num + 1;

	CString org;
	org.Format("%d.org",num);
	org = str + "\\" + org;
	AFile file;
	if(file.Open(org,AFILE_OPENEXIST))
	{
		file.Close();
		return true;
	}

	return false;
}

bool CListDlg::IsFixedItem(CString path)
{
	bool bPass = false;
	for (ORG_PATHS::iterator n = _org_paths.begin(); n != _org_paths.end(); ++n)
	{
		AString strOrgPath = n->first;
		
		if(strOrgPath.Find(path) != -1)
		{
			bPass = true;
			break;
		}
	}
	return bPass;
}

void CListDlg::OnSelchangedTreeSortList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CListDlg::OnSelchangingTreeSortList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CListDlg::OnEditTemplateProperty() 
{
	// TODO: Add your command handler code here
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	CString itemText = pCtrl->GetItemText(itemp);
	CString path = itemText;
	while(itemp = pCtrl->GetParentItem(itemp))
	{
		CString pathsub = pCtrl->GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;
	if(path.Find(".tmpl")!=-1 || path.Find(".ext")!=-1)
	{
		CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path);
	}else
	{
		path = path + "\\" + itemText + ".tmpl";
		if(FileIsExist(AString(path)))
			CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path);
	}
}

void CListDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	UpdateRecord();
}

void CListDlg::UpdateRecord()
{
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	g_Record.GetTreeList(pCtrl);
}



void CListDlg::OnModifiedObjectExt() 
{
	// TODO: Add your command handler code here
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	CString itemText = pCtrl->GetItemText(itemp);
	CString path = itemText;
	while(itemp = pCtrl->GetParentItem(itemp))
	{
		CString pathsub = pCtrl->GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;

	CTemplateExtDlg dlg;
	dlg.m_strDir = path;
	dlg.m_bModified = true;
	if(dlg.DoModal()==IDOK)
	{
		CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp*)AfxGetApp())->GetDocument(path);
		if(pDoc)
		{
			pDoc->OnCloseDocument();
		}
		AString itemName[4] = {dlg.m_strItemName1,dlg.m_strItemName2,dlg.m_strItemName3,dlg.m_strItemName4};
		for(int i = 0; i< dlg.m_nItemNum; i++)
		{
			AString itemType(dlg.m_strItemType[i]);
			dlg.m_extTmp.AddItem(itemName[i],itemType);
		}
		dlg.m_extTmp.Save(path);
#ifdef _MD5_CHECK		
		WriteMd5CodeToTemplate((LPCSTR)path);
#endif
		if(pDoc) ((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path);
	}
	dlg.m_extTmp.Release();
}


void CListDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnTimer(nIDEvent);
}

BOOL CListDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB)
		return false;
	return CDialog::PreTranslateMessage(pMsg);
}

void CListDlg::OnVssCheckInAll() 
{
	// TODO: Add your command handler code here
	CTreeItemList selList;
	m_Tree.GetSelectedList(selList);
	POSITION pos = selList.GetHeadPosition();
	while( pos )
	{
		HTREEITEM hitem = selList.GetNext(pos);
		EnumCheck(hitem,false,false);
	}
}

void CListDlg::OnVssCheckOutAll() 
{
	// TODO: Add your command handler code here
	CTreeItemList selList;
	m_Tree.GetSelectedList(selList);
	POSITION pos = selList.GetHeadPosition();
	while( pos )
	{
		HTREEITEM hitem = selList.GetNext(pos);
		EnumCheck(hitem,true,false);
	}
}

void CListDlg::OnClickTreeSortList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	if(m_Tree.GetSelectedCount() >1 || m_Tree.GetSelectedCount()==0) return;
	
#ifdef _DEBUG 
	DWORD lastTime = a_GetTime();
	CString strDebug;
#endif
	
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	POINT pt;
	GetCursorPos(&pt);
	m_Tree.ScreenToClient(&pt);
	HTREEITEM test = m_Tree.HitTest(pt);
	if(test!=itemp) return;

	CString itemText = pCtrl->GetItemText(itemp);
	CString path = itemText;
	while(itemp = pCtrl->GetParentItem(itemp))
	{
		CString pathsub = pCtrl->GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;
	
#ifdef _DEBUG
	strDebug.Format("Ready time: %d\n", a_GetTime()-lastTime);
	lastTime = a_GetTime();
	TRACE(strDebug);
#endif 
	
	if(path.Find(".tmpl")!=-1 || path.Find(".ext")!=-1)
	{
		CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path);
	}else
	{
		if(!IsFixedItem(path))
		{
			if(!IsHideFiles(path))
				if(FileIsExist(AString(path + "\\" + itemText + ".tmpl")))
				CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path + "\\" + itemText + ".tmpl");
		}
	}

#ifdef _DEBUG
	strDebug.Format("Open total time: %d\n", a_GetTime()-lastTime);
	lastTime = a_GetTime();
	TRACE(strDebug);
#endif 	
	*pResult = 0;
	*pResult = 0;
}

void CListDlg::OnItemexpandedTreeSortList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pNMTreeView->action == TVE_EXPAND)
	{
		AfxGetApp()->BeginWaitCursor();

		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		DWORD data = m_Tree.GetItemData(hItem);
		if(!(data & FD_OPENED))
		{
			//删除原来有的一条
			m_Tree.DeleteItem(m_Tree.GetChildItem(hItem));
			//枚举即将显示的ITEM
			CString path = GetItemPath(hItem);
			EnumObjects(path, hItem,false,1);
			HTREEITEM hChild = m_Tree.GetChildItem(hItem);
			m_Tree.SetItemData(hItem,FD_OPENED);
			while(hChild)
			{
				path = GetItemPath(hChild);
				EnumObjects(path, hChild,false,1,true);
				hChild = m_Tree.GetNextSiblingItem(hChild);
			}
		}
		AfxGetApp()->EndWaitCursor();
	}
	*pResult = 0;
}

CString CListDlg::GetItemPath(HTREEITEM hItem)
{
	if(hItem==0) return "";
	CString itemText = m_Tree.GetItemText(hItem);
	CString path = itemText;
	while(hItem = m_Tree.GetParentItem(hItem))
	{
		CString pathsub = m_Tree.GetItemText(hItem);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;
	path = g_szWorkDir + path;
	return path;
}

void CListDlg::OnVssUndoCheckOut() 
{
	// TODO: Add your command handler code here
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	HTREEITEM oldItem = itemp;
	CString path;
	CString fileName = pCtrl->GetItemText(itemp);
	
	while(itemp = pCtrl->GetParentItem(itemp))
	{
		CString pathsub = pCtrl->GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;
	int num = path.GetLength();
	path = path.Left(num-1);
	
	bool bIsFolder = false;
	if(fileName.Find(".tmpl")==-1 && fileName.Find(".ext")==-1)
	{//不是文件
		path = path + "\\" + fileName;
		fileName = fileName + ".tmpl";
		if(!FileIsExist(AString(path + "\\" + fileName))) return;
		bIsFolder = true;
	}
	
	g_VSS.SetProjectPath(path);
	g_VSS.UndoCheckOut(path + "\\" + fileName);

	if(bIsFolder)
	{
		if(!IsHideFiles(path))
		{
			if(FileIsReadOnly(AString(path + "\\" + fileName)))
			{
				pCtrl->SetItemImage(oldItem,4,4);
			}else
			{
				pCtrl->SetItemImage(oldItem,5,5);
			}
		}else
		{
			if(FileIsReadOnly(AString(path + "\\" + fileName)))
			{
				pCtrl->SetItemImage(oldItem,2,2);
			}else
			{
				pCtrl->SetItemImage(oldItem,3,3);
			}	
		}
	}else
	{
		if(FileIsReadOnly(AString(path + "\\" + fileName)))
		{
			pCtrl->SetItemImage(oldItem,4,4);
		}else
		{
			pCtrl->SetItemImage(oldItem,5,5);
		}
	}

	CElementDataDoc *pDoc = (CElementDataDoc *)((CElementDataApp*)AfxGetApp())->GetDocument(path + "\\" + fileName);
	if(pDoc)
	{
		if(pDoc) pDoc->OnCloseDocument();
		((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path + "\\" + fileName);
	}
}

void CListDlg::OnUpdateVssUndoCheckOut(CCmdUI* pCmdUI) 
{
	
}

void CListDlg::OnAutoResaveTemplate() 
{
// TODO: Add your command handler code here
	CTreeItemList selList;
	m_Tree.GetSelectedList(selList);
	BeginWaitCursor();
	POSITION pos = selList.GetHeadPosition();
	while( pos )
	{
		HTREEITEM hitem = selList.GetNext(pos);
		EnumSave(hitem);
	}
	EndWaitCursor();
}

void CListDlg::EnumSave(HTREEITEM root)
{
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	bool pass = true;
	if(pass)
	{
		HTREEITEM itemp = root;
		HTREEITEM oldItem = root;
		CString path;
		CString fileName = pCtrl->GetItemText(itemp);
		
		while(itemp = pCtrl->GetParentItem(itemp))
		{
			CString pathsub = pCtrl->GetItemText(itemp);
			path = pathsub + "\\" + path;	
		}
		path = "BaseData\\" + path;
		int num = path.GetLength();
		path = path.Left(num-1);
		
		bool bIsFolder = false;
		if(fileName.Find(".tmpl")==-1 && fileName.Find(".ext")==-1)
		{//不是文件
			path = path + "\\" + fileName;
			fileName = fileName + ".tmpl";
			if(!FileIsExist(AString(path + "\\" + fileName))) goto sub_1;
			bIsFolder = true;
		}
		
		CElementDataDoc *pDoc = (CElementDataDoc*)((CElementDataApp *)AfxGetApp())->OpenDocumentFile(path + "\\" + fileName);
		if(pDoc)
		{
			pDoc->OnSaveDocument(path + "\\" + fileName);
			pDoc->OnCloseDocument();
		}
		
	}
	
	//Child item
sub_1:
	HTREEITEM hChild = m_Tree.GetChildItem(root);
	while (hChild)
	{
		EnumSave(hChild);
		hChild = m_Tree.GetNextItem(hChild, TVGN_NEXT);
	}	
}
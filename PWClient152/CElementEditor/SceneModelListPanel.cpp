// SceneModelListPanel.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "SceneObjectManager.h"
#include "MainFrm.h"
#include "OperationManager.h"
#include "ModelContainer.h"
#include "SceneModelAddOperation.h"
#include "SceneModelListPanel.h"
//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CSceneModelListPanel dialog


CSceneModelListPanel::CSceneModelListPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CSceneModelListPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSceneModelListPanel)
	//}}AFX_DATA_INIT
}


void CSceneModelListPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSceneModelListPanel)
	DDX_Control(pDX, IDC_SCENE_MODEL_TREE, m_treeSceneModel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSceneModelListPanel, CDialog)
	//{{AFX_MSG_MAP(CSceneModelListPanel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SCENE_MODEL_TREE, OnSelchangedSceneObjectTree)
	ON_NOTIFY(NM_DBLCLK, IDC_SCENE_MODEL_TREE, OnDblclkSceneObjectTree)
	ON_NOTIFY(NM_RCLICK, IDC_SCENE_MODEL_TREE, OnRclickSceneModelTree)
	ON_COMMAND(ID_SET_TO_TREE, OnSetToTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSceneModelListPanel message handlers
void CSceneModelListPanel::FreshObjectList()
{
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_SCENE_MODEL_TREE);
	ASSERT(pCtrl != NULL);
	pCtrl->DeleteAllItems();
	
	if(!mResMan.Load()) return;
	int num = mResMan.GetResCount(ElementResMan::RES_TYPE_BUILDING);
	
	for(int i=0; i<num;i++)
	{
		Res_Item *pItem = mResMan.GetRes(ElementResMan::RES_TYPE_BUILDING,i);
		HTREEITEM hRoot = NULL;
		CString org = pItem->szOrgName;
		CString temp;
		while(1)
		{
			int pos = org.Find("\\");
			if(pos==-1) 
			{
				hRoot = pCtrl->InsertItem(org,hRoot,NULL);
				pCtrl->SetItemImage(hRoot,2,2);
				pCtrl->SetItemData(hRoot,(DWORD)pItem->szID);
				break;
			}
			temp = org.Left(pos);
			hRoot = InsertItem(pCtrl,hRoot,temp);
			if(hRoot==NULL) break;
			pCtrl->SetItemImage(hRoot,1,0);
			int len = org.GetLength();
			org = org.Right(len - pos - 1);
		}
	}

	
}

HTREEITEM CSceneModelListPanel::InsertItem(CTreeCtrl *pTree, HTREEITEM hRoot, CString name)
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

void CSceneModelListPanel::EnumObjects( CString strPathName , HTREEITEM hTreeItemp)
{
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_SCENE_MODEL_TREE);
	ASSERT(pCtrl != NULL);
	
	CFileFind finder;
	CString OldPath;
	CString StrExt = strPathName + "\\*.*";
	
	BOOL bWorking = finder.FindFile(StrExt);
	
  	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		
		//是"."或是".." 跳过.
		if (finder.IsDots())
			continue;
		// 是文件夹，继续搜寻.
		if (finder.IsDirectory())
		{
			CString name = finder.GetFilePath();
			name.Replace(strPathName,"");
			name.Delete(0,1);
			if( 0 !=stricmp(name,"Textures"))
			{
				HTREEITEM hNewItem = pCtrl->InsertItem(name,hTreeItemp,TVI_SORT);
				pCtrl->SetItemImage(hNewItem,1,0);
				EnumObjects(finder.GetFilePath(),hNewItem);
			}
		}
	}
	//文件
	EnumFiles(strPathName,hTreeItemp);
	
	finder.Close();
}

void CSceneModelListPanel::EnumFiles(CString strPathName, HTREEITEM hTreeItemp)
{
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_SCENE_MODEL_TREE);
	ASSERT(pCtrl != NULL);
	
	CFileFind finder;
	CString msg;
	CString strWildcard;
	strWildcard = strPathName + _T("\\*.mox");
	
	BOOL bWorking = finder.FindFile(strWildcard);
	int n = 0;
	while (bWorking)
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
		HTREEITEM hNewItem = pCtrl->InsertItem(msg,hTreeItemp,TVI_SORT);
		pCtrl->SetItemImage(hNewItem,2,2);
		msg = finder.GetFilePath();
		g_ModelContainer.AddElement(AString(msg));
	}
	finder.Close();
}

void CSceneModelListPanel::OnSelchangedSceneObjectTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CSceneModelListPanel::OnDblclkSceneObjectTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	/*Old Version
	HTREEITEM itemp = m_treeSceneModel.GetSelectedItem();
	if(!m_treeSceneModel.GetChildItem(itemp))
	{
		CString str = m_treeSceneModel.GetItemText(itemp);
		CString pre;
		while(true)
		{
			itemp = m_treeSceneModel.GetParentItem(itemp);
			if(itemp)
			{
				pre  = m_treeSceneModel.GetItemText(itemp);
				pre  += "\\";
				str = pre + str;
			}else break;
		}
		pre = "Models\\Static\\";
		str = pre + str;
		PESTATICMODEL pA3dM = g_ModelContainer.GetStaticModel((AString)str);
		if(pA3dM==NULL) return;
		
		//下面更新操作
		CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
		if(pParent)
		{
			//	Hide panels
			pParent->HideAllPanels();
			int aIndices[2] = {CObjectsPage::PANEL_SCENE_OBJECT,CObjectsPage::PANEL_PROPERTY_OBJECT};
			bool aOpenFlags[2] = {true,true};
			pParent->ShowPanels(aIndices, aOpenFlags,2);
			
			CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
			if(pMainFrame)
			{
				pMainFrame->SetOtherOperation(true);
				CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
				COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_MODEL_ADD);
				((SceneModelAddOperation*)pOperation)->SetStaticModel(AString(str));
				if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
			}
		}
	}*/

	HTREEITEM itemp = m_treeSceneModel.GetSelectedItem();
	CString strModelPath;
    DWORD pData = m_treeSceneModel.GetItemData(itemp);
	strModelPath.Format("%s",(char*)pData);
 	if(!strModelPath.IsEmpty() && !m_treeSceneModel.GetChildItem(itemp))
	{
		//PESTATICMODEL pA3dM = g_ModelContainer.GetStaticModel((AString)strModelPath);
		//if(pA3dM==NULL) return;
		
		//下面更新操作
		CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
		if(pParent)
		{
			CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
			if(pMainFrame)
			{
				pMainFrame->SetOtherOperation(true);
				CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
				COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SCENE_MODEL_ADD);
				((SceneModelAddOperation*)pOperation)->SetStaticModel(AString(strModelPath));
				if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
			}
		}
	}
	*pResult = 0;
}


BOOL CSceneModelListPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ImageList.Create(16, 16, ILC_COLOR8, 0 , 3);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON1));
    m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON3));
	
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_SCENE_MODEL_TREE);
	ASSERT(pCtrl != NULL);
	pCtrl->SetImageList(&m_ImageList,TVSIL_NORMAL);

	CString strWildcard;
	strWildcard = g_szWorkDir;
	strWildcard += "Models\\static";
	//EnumObjects(strWildcard,NULL);
	FreshObjectList();
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSceneModelListPanel::OnRclickSceneModelTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint point;
	GetCursorPos(&point);
	POINT pt = {point.x, point.y};
	//m_treeSceneModel.ScreenToClient(&pt);
	
	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_TREE_SET);
	
	CMenu* pPopupMenu = Menu.GetSubMenu(0);
	ASSERT(pPopupMenu);
	
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
	*pResult = 0;
}

void CSceneModelListPanel::OnSetToTree() 
{
	// TODO: Add your command handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString strFormat;
		HTREEITEM hSel = m_treeSceneModel.GetSelectedItem();
		HTREEITEM hParent = m_treeSceneModel.GetParentItem(hSel);

		if(hParent) hSel = hParent;
		if(hSel==NULL) return;
		
		strFormat.Format("你是否要将模型路径中带有\" %s \"字符串的模型都设置成树？",m_treeSceneModel.GetItemText(hSel));
		
		if(IDYES != AfxMessageBox(strFormat,MB_YESNO)) return;
		
		BeginWaitCursor();
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		APtrList< CSceneObject*>* pListObject = pSManager->GetSortObjectList(CSceneObject::SO_TYPE_STATIC_MODEL);
		
		ALISTPOSITION pos = pListObject->GetHeadPosition();
		while(pos)
		{
			CStaticModelObject *pObj = (CStaticModelObject*)pListObject->GetNext(pos);
			if(pObj->GetA3DModel())
			{
				AString strModelName = pObj->GetModelPath();
				Res_Item* pItem = mResMan.ResIsExistByPath(ElementResMan::RES_TYPE_BUILDING,strModelName);
				if(pItem)
				{
					strModelName = pItem->szOrgName;
					strFormat.Format("%s\\",m_treeSceneModel.GetItemText(hSel));
					if(strModelName.Find(strFormat)!=-1) 
						pObj->SetTreeFlag(true);
				}
			}
		}
		EndWaitCursor();
	}
}

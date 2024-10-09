// SceneObjectsList.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "SceneObjectsList.h"
#include "SceneObject.h"
#include "SceneObjectManager.h"
#include "MainFrm.h"
#include "EditerBezier.h"
#include "UndoLightDeleteAction.h"
#include "UndoMan.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CSceneObjectsList dialog


CSceneObjectsList::CSceneObjectsList(CWnd* pParent /*=NULL*/)
	: CDialog(CSceneObjectsList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSceneObjectsList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSceneObjectsList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSceneObjectsList)
	DDX_Control(pDX, IDC_SCENE_TREE, m_treeScene);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSceneObjectsList, CDialog)
	//{{AFX_MSG_MAP(CSceneObjectsList)
	ON_NOTIFY(NM_DBLCLK, IDC_SCENE_TREE, OnDblclkSceneTree)
	ON_NOTIFY(NM_CLICK, IDC_SCENE_TREE, OnClickSceneTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SCENE_TREE, OnSelchangedSceneTree)
	ON_BN_CLICKED(IDC_BUTTON_FRESH_SCENE_LIST, OnButtonFreshSceneList)
	ON_BN_CLICKED(IDC_BUTTON_FRESH_DEL_SCENE_OBJECT, OnButtonFreshDelSceneObject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSceneObjectsList message handlers
void CSceneObjectsList::FreshObjectList()
{

	CTreeCtrl* pCtrl = (CTreeCtrl*)GetDlgItem(IDC_SCENE_TREE);
	ASSERT(pCtrl != NULL);
	pCtrl->DeleteAllItems();
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		
		TVINSERTSTRUCT tvStaticModel;
		TVINSERTSTRUCT tvLight;
		TVINSERTSTRUCT tvWater;
		TVINSERTSTRUCT tvArea;
		TVINSERTSTRUCT tvPath;
		TVINSERTSTRUCT tvAIGenerator;
		TVINSERTSTRUCT tvSpecially;
		TVINSERTSTRUCT tvDummy;
		TVINSERTSTRUCT tvSkin;
		TVINSERTSTRUCT tvCritterGroup;
		TVINSERTSTRUCT tvNpc;
		TVINSERTSTRUCT tvPrecinct;
		TVINSERTSTRUCT tvGather;
		TVINSERTSTRUCT tvAudio;
		TVINSERTSTRUCT tvRange;
		TVINSERTSTRUCT tvCloud;
		TVINSERTSTRUCT tvInstanceBox;
		TVINSERTSTRUCT tvDynamic;
		
		//static model
		tvStaticModel.hParent = NULL;
		tvStaticModel.hInsertAfter = NULL;
		tvStaticModel.item.mask = TVIF_TEXT;
		tvStaticModel.item.pszText = _T("静态模型");
		HTREEITEM hStaticModel = pCtrl->InsertItem(&tvStaticModel);
		
		//light
		tvLight.hParent = NULL;
		tvLight.hInsertAfter = NULL;
		tvLight.item.mask = TVIF_TEXT;
		tvLight.item.pszText = _T("场景灯光");
		HTREEITEM hLight = pCtrl->InsertItem(&tvLight);
		
		//water
		tvWater.hParent = NULL;
		tvWater.hInsertAfter = NULL;
		tvWater.item.mask = TVIF_TEXT;
		tvWater.item.pszText = _T("水面");
		HTREEITEM hWater = pCtrl->InsertItem(&tvWater);

		//Area
		tvArea.hParent = NULL;
		tvArea.hInsertAfter = NULL;
		tvArea.item.mask = TVIF_TEXT;
		tvArea.item.pszText = _T("区域");
		HTREEITEM hArea = pCtrl->InsertItem(&tvArea);

		//Path
		tvPath.hParent = NULL;
		tvPath.hInsertAfter = NULL;
		tvPath.item.mask = TVIF_TEXT;
		tvPath.item.pszText = _T("路径");
		HTREEITEM hPath = pCtrl->InsertItem(&tvPath);
		
		//Ai generator
		tvAIGenerator.hParent = NULL;
		tvAIGenerator.hInsertAfter = NULL;
		tvAIGenerator.item.mask = TVIF_TEXT;
		tvAIGenerator.item.pszText = _T("AI产生器");
		HTREEITEM hAIGenerator = pCtrl->InsertItem(&tvAIGenerator);

		//Specially object
		tvSpecially.hParent = NULL;
		tvSpecially.hInsertAfter = NULL;
		tvSpecially.item.mask = TVIF_TEXT;
		tvSpecially.item.pszText = _T("特效");
		HTREEITEM hSpecially = pCtrl->InsertItem(&tvSpecially);

		//Dummy object
		tvDummy.hParent = NULL;
		tvDummy.hInsertAfter = NULL;
		tvDummy.item.mask = TVIF_TEXT;
		tvDummy.item.pszText = _T("哑元");
		HTREEITEM hDummy = pCtrl->InsertItem(&tvDummy);

		//SkinModel object
		tvSkin.hParent = NULL;
		tvSkin.hInsertAfter = NULL;
		tvSkin.item.mask = TVIF_TEXT;
		tvSkin.item.pszText = _T("动画模型");
		HTREEITEM hSkin = pCtrl->InsertItem(&tvSkin);

		//SkinModel object
		tvCritterGroup.hParent = NULL;
		tvCritterGroup.hInsertAfter = NULL;
		tvCritterGroup.item.mask = TVIF_TEXT;
		tvCritterGroup.item.pszText = _T("生物群");
		HTREEITEM hCritterGroup = pCtrl->InsertItem(&tvCritterGroup);

		//Fixed npc object
		tvNpc.hParent = NULL;
		tvNpc.hInsertAfter = NULL;
		tvNpc.item.mask = TVIF_TEXT;
		tvNpc.item.pszText = _T("NPC");
		HTREEITEM hNpc = pCtrl->InsertItem(&tvNpc);

		//Gather object
		tvGather.hParent = NULL;
		tvGather.hInsertAfter = NULL;
		tvGather.item.mask = TVIF_TEXT;
		tvGather.item.pszText = _T("可采集对象");
		HTREEITEM hGather = pCtrl->InsertItem(&tvGather);

		//Precinct object
		tvPrecinct.hParent = NULL;
		tvPrecinct.hInsertAfter = NULL;
		tvPrecinct.item.mask = TVIF_TEXT;
		tvPrecinct.item.pszText = _T("管辖区");
		HTREEITEM hPrecinct = pCtrl->InsertItem(&tvPrecinct);
		//Audio object 
		tvAudio.hParent = NULL;
		tvAudio.hInsertAfter = NULL;
		tvAudio.item.mask = TVIF_TEXT;
		tvAudio.item.pszText = _T("3d声音");
		HTREEITEM hAudio = pCtrl->InsertItem(&tvAudio);

		//Range object 
		tvRange.hParent = NULL;
		tvRange.hInsertAfter = NULL;
		tvRange.item.mask = TVIF_TEXT;
		tvRange.item.pszText = _T("范围对象");
		HTREEITEM hRange = pCtrl->InsertItem(&tvRange);

		//cloud 
		tvCloud.hParent = NULL;
		tvCloud.hInsertAfter = NULL;
		tvCloud.item.mask = TVIF_TEXT;
		tvCloud.item.pszText = _T("云层盒子");
		HTREEITEM hCloud = pCtrl->InsertItem(&tvCloud);

		tvInstanceBox.hParent = NULL;
		tvInstanceBox.hInsertAfter = NULL;
		tvInstanceBox.item.mask = TVIF_TEXT;
		tvInstanceBox.item.pszText = _T("副本盒子");
		HTREEITEM hInstanceBox = pCtrl->InsertItem(&tvInstanceBox);

		tvDynamic.hParent = NULL;
		tvDynamic.hInsertAfter = NULL;
		tvDynamic.item.mask = TVIF_TEXT;
		tvDynamic.item.pszText = _T("动态物品");
		HTREEITEM hDynamic = pCtrl->InsertItem(&tvDynamic);
		
		pCtrl->SetItemImage(hStaticModel,1,0);
		pCtrl->SetItemImage(hLight,1,0);
		pCtrl->SetItemImage(hWater,1,0);
		pCtrl->SetItemImage(hArea,1,0);
		pCtrl->SetItemImage(hPath,1,0);
		pCtrl->SetItemImage(hAIGenerator,1,0);
		pCtrl->SetItemImage(hSpecially,1,0);
		pCtrl->SetItemImage(hDummy,1,0);
		pCtrl->SetItemImage(hSkin,1,0);
		pCtrl->SetItemImage(hCritterGroup,1,0);
		pCtrl->SetItemImage(hPrecinct,1,0);
		pCtrl->SetItemImage(hGather,1,0);
		pCtrl->SetItemImage(hAudio,1,0);
		pCtrl->SetItemImage(hRange,1,0);
		pCtrl->SetItemImage(hCloud,1,0);
		pCtrl->SetItemImage(hInstanceBox,1,0);
		pCtrl->SetItemImage(hNpc,1,0);
		pCtrl->SetItemImage(hDynamic,1,0);

		ALISTPOSITION pos = pSManager->m_listSceneObject.GetTailPosition();
		while(pos)
		{
			PSCENEOBJECT ptemp = pSManager->m_listSceneObject.GetPrev(pos);;
			if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_LIGHT)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hLight, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}
			else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_STATIC_MODEL)
			{	
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hStaticModel, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}
			else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_WATER)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hWater, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}
			else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_AREA)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hArea, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}
			else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_BEZIER)
			{	
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hPath, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}
			else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_AIAREA)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hAIGenerator, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}
			else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_SPECIALLY)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hSpecially, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_DUMMY)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hDummy, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_ECMODEL)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hSkin, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_CRITTER_GROUP)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hCritterGroup, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_FIXED_NPC)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hNpc, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_PRECINCT)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hPrecinct, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_GATHER)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hGather, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_AUDIO)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hAudio, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_RANGE)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hRange, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_CLOUD_BOX)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hCloud, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_INSTANCE_BOX)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hInstanceBox, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}else if(ptemp->GetObjectType()==CSceneObject::SO_TYPE_DYNAMIC)
			{
				HTREEITEM hNew = pCtrl->InsertItem(ptemp->GetObjectName(), hDynamic, TVI_SORT);
				pCtrl->SetItemImage(hNew,2,2);
			}
		}
	}
}

BOOL CSceneObjectsList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ImageList.Create(16, 16, ILC_COLOR8, 0 , 3);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON1));
    m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON4));

	CTreeCtrl* pCtrl = (CTreeCtrl*)GetDlgItem(IDC_SCENE_TREE);
	ASSERT(pCtrl != NULL);
	pCtrl->SetImageList(&m_ImageList,TVSIL_NORMAL);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSceneObjectsList::OnDblclkSceneTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CSceneObjectsList::OnClickSceneTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM itemp = m_treeScene.GetSelectedItem();
	if(m_treeScene.GetParentItem(itemp))
	{
		CString str = m_treeScene.GetItemText(itemp);
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
			PSCENEOBJECT pobj = pSManager->FindSceneObject((AString)str);
			if(pobj)
			{
				pSManager->ClearSelectedList();
				pSManager->AddObjectPtrToSelected(pobj);
				CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
				if(pParent)
				{
					pobj->UpdateProperty(false);
					AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllTerrainOperationButton();
					AUX_GetMainFrame()->SetOtherOperation(false);
					AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pobj->m_pProperty);
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
					AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(false);
				}
			}
		}
	}
	*pResult = 0;
}

void CSceneObjectsList::OnSelchangedSceneTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	HTREEITEM itemp = m_treeScene.GetSelectedItem();
	if(m_treeScene.GetParentItem(itemp))
	{
		CString str = m_treeScene.GetItemText(itemp);
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
			PSCENEOBJECT pobj = pSManager->FindSceneObject((AString)str);
			if(pobj)
			{
				pSManager->ClearSelectedList();
				pSManager->AddObjectPtrToSelected(pobj);
				CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
				if(pParent)
				{
					pobj->UpdateProperty(false);
					AUX_GetMainFrame()->SetOtherOperation(false);
					AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pobj->m_pProperty);
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
					AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(false);
				}
			}
		}
	}
	
	*pResult = 0;
}

void CSceneObjectsList::OnButtonFreshSceneList() 
{
	// TODO: Add your control notification handler code here
	FreshObjectList();
}

void CSceneObjectsList::OnButtonFreshDelSceneObject() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		if(pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount()==1)
		{
			HTREEITEM itemp = m_treeScene.GetSelectedItem();
			if(m_treeScene.GetParentItem(itemp))
			{
				CSceneObject* pObj = pMap->GetSceneObjectMan()->m_listSelectedObject.GetHead();
				CString str = m_treeScene.GetItemText(itemp);
				if(str == pObj->GetObjectName())
				{
					CString msg;
					msg.Format("你确定要删除选中的对象(%s)吗？",str);
					if(IDYES!=AfxMessageBox(msg,MB_YESNO|MB_ICONQUESTION)) return;
					CUndoSceneObjectDeleteAction *pUndo = new CUndoSceneObjectDeleteAction(pMap->GetSceneObjectMan());
					if(pUndo->GetSeletedListData())
					{
						g_UndoMan.AddUndoAction(pUndo);
						m_treeScene.DeleteItem(itemp);
						AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel();
					}else 
					{
						pUndo->Release();
						delete pUndo;
					}
				}
			}
		}
	}
}

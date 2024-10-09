// TerrainModifyPlantsPanel.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "TerrainModifyPlantsPanel.h"
#include "InputPlantsNameDlg.h"
#include "TerrainPlants.h"
#include "SceneObjectManager.h"
#include "render.h"
#include "CommonFileDlg.h"
#include "OperationManager.h"
#include "TerrainPlantPaintOperation.h"
#include "EL_Forest.h"
#include "EL_Tree.h"
#include "EL_GrassType.h"
#include "EL_Grassland.h"
#include "TreeTypesAddDlg.h"
#include "TerrainRender.h"
#include "MainFrm.h"


//#define new A_DEBUG_NEW

#define MAX_VISIBLE_GRASS_NUM  30000

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyPlantsPanel dialog


CTerrainModifyPlantsPanel::CTerrainModifyPlantsPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainModifyPlantsPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainModifyPlantsPanel)
	m_strName = _T("");
	m_nGrassAlphaRef = 0;
	m_fGrassSightRange = 0.0f;
	m_fGassSize = 0.0f;
	m_fGrassSizeVar = 0.0f;
	m_fGrassSoft = 0.0f;
	m_strRes = _T("");
	//}}AFX_DATA_INIT
	m_pListTreeName = NULL;
}


void CTerrainModifyPlantsPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainModifyPlantsPanel)
	DDX_Control(pDX, IDC_EDIT_GRASS_MAX_NUM, m_editGrassMaxNum);
	DDX_Control(pDX, IDC_EDIT_GRASS_SOFT, m_ctrlSoft);
	DDX_Control(pDX, IDC_EDIT_GRASS_SIZEVAR, m_ctrlSizeVar);
	DDX_Control(pDX, IDC_EDIT_GRASS_SIZE, m_ctrlGrassSize);
	DDX_Control(pDX, IDC_EDIT_GRASS_SIGHTRANGE, m_ctrlSightRange);
	DDX_Control(pDX, IDC_EDIT_GRASS_ALPHAREF, m_ctrlAlphaRef);
	DDX_Control(pDX, IDC_EDIT_PLANTS_DENSITY, m_editDensity);
	DDX_Control(pDX, IDC_TREE_PLANT_LIST, m_treePlants);
	DDX_Text(pDX, IDC_EDIT_PLANT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_PLANT_RES, m_strRes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainModifyPlantsPanel, CDialog)
	//{{AFX_MSG_MAP(CTerrainModifyPlantsPanel)
	ON_BN_CLICKED(IDC_PLANT_ADD_SORT, OnPlantAddSort)
	ON_BN_CLICKED(IDC_PLANT_DELETE, OnPlantDelete)
	ON_BN_CLICKED(IDC_PLANT_DELETE_FROM_TERRAIN, OnPlantDeleteFromTerrain)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PLANT_LIST, OnDblclkTreePlantList)
	ON_NOTIFY(NM_CLICK, IDC_TREE_PLANT_LIST, OnClickTreePlantList)
	ON_EN_CHANGE(IDC_EDIT_PLANTS_DENSITY, OnChangeEditPlantsDensity)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLANTS_DENSITY, OnDeltaposSpinPlantsDensity)
	ON_BN_CLICKED(IDC_PLANT_ADD_TO_TERRAIN, OnPlantAddToTerrain)
	ON_BN_CLICKED(IDC_PLANT_MOVE_Y, OnPlantMoveY)
	ON_BN_CLICKED(IDC_PLANT_MOVE_XZ, OnPlantMoveXz)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRASS_ALPHAREF, OnDeltaposSpinGrassAlpharef)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRASS_SIGHTRANGE, OnDeltaposSpinGrassSightrange)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRASS_SIZE, OnDeltaposSpinGrassSize)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRASS_SIZEVAR, OnDeltaposSpinGrassSizevar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRASS_SOFT, OnDeltaposSpinGrassSoft)
	ON_BN_CLICKED(IDC_CHECK_GRASS_ENABLEALPHA, OnCheckGrassEnablealpha)
	ON_EN_CHANGE(IDC_EDIT_PLANT_NAME, OnChangeEditPlantName)
	ON_BN_CLICKED(IDC_TREE_TYPES_ADD, OnTreeTypesAdd)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PLANT_LIST, OnSelchangedTreePlantList)
	ON_EN_CHANGE(IDC_EDIT_GRASS_ALPHAREF, OnChangeEditGrassAlpharef)
	ON_EN_CHANGE(IDC_EDIT_GRASS_SIGHTRANGE, OnChangeEditGrassSightrange)
	ON_EN_CHANGE(IDC_EDIT_GRASS_SIZE, OnChangeEditGrassSize)
	ON_EN_CHANGE(IDC_EDIT_GRASS_SIZEVAR, OnChangeEditGrassSizevar)
	ON_EN_CHANGE(IDC_EDIT_GRASS_SOFT, OnChangeEditGrassSoft)
	ON_EN_KILLFOCUS(IDC_EDIT_GRASS_ALPHAREF, OnKillfocusEditGrassAlpharef)
	ON_EN_KILLFOCUS(IDC_EDIT_GRASS_SIGHTRANGE, OnKillfocusEditGrassSightrange)
	ON_EN_KILLFOCUS(IDC_EDIT_GRASS_SIZE, OnKillfocusEditGrassSize)
	ON_EN_KILLFOCUS(IDC_EDIT_GRASS_SIZEVAR, OnKillfocusEditGrassSizevar)
	ON_EN_KILLFOCUS(IDC_EDIT_GRASS_SOFT, OnKillfocusEditGrassSoft)
	ON_EN_KILLFOCUS(IDC_EDIT_PLANTS_DENSITY, OnKillfocusEditPlantsDensity)
	ON_EN_KILLFOCUS(IDC_EDIT_PLANT_NAME, OnKillfocusEditPlantName)
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SPIN_GRASS_MAXNUM, OnOutofmemorySpinGrassMaxnum)
	ON_EN_KILLFOCUS(IDC_EDIT_GRASS_MAX_NUM, OnKillfocusEditGrassMaxNum)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRASS_MAXNUM, OnDeltaposSpinGrassMaxnum)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_RES, OnButtonChangeRes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyPlantsPanel message handlers

BOOL CTerrainModifyPlantsPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AUX_FlatEditBox(&m_editDensity);
	AUX_FlatEditBox(&m_ctrlSoft);
	AUX_FlatEditBox(&m_ctrlSizeVar);
	AUX_FlatEditBox(&m_ctrlGrassSize);
	AUX_FlatEditBox(&m_ctrlAlphaRef);
	AUX_FlatEditBox(&m_ctrlSightRange);
	
	EnableGrassProperyCtrl(false);
	// TODO: Add extra initialization here
	m_ImageList.Create(16, 16, ILC_COLOR8, 0 , 4);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON5));
    m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON6));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON7));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON8));
	m_treePlants.SetImageList(&m_ImageList,TVSIL_NORMAL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrainModifyPlantsPanel::FreshList()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
 	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		CTreeCtrl* pCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_PLANT_LIST);
		ASSERT(pCtrl != NULL);
		pCtrl->DeleteAllItems();
		
		TVINSERTSTRUCT tvForest;
		tvForest.hParent = NULL;
		tvForest.hInsertAfter = NULL;
		tvForest.item.mask = TVIF_TEXT;
		tvForest.item.pszText = _T("所有树");
		
		TVINSERTSTRUCT tvGrass;
		tvGrass.hParent = NULL;
		tvGrass.hInsertAfter = NULL;
		tvGrass.item.mask = TVIF_TEXT;
		tvGrass.item.pszText = _T("所有草");
		
		HTREEITEM hTree = pCtrl->InsertItem(&tvForest);
		HTREEITEM hGrass = pCtrl->InsertItem(&tvGrass);

		pCtrl->SetItemImage(hTree,0,0);
		pCtrl->SetItemImage(hGrass,1,1);
		
		APtrList<PPLANT>& listPlant = pManager->GetPlants()->GetPlantList();
		ALISTPOSITION pos = listPlant.GetHeadPosition();
		while(pos)
		{
			PPLANT plant = listPlant.GetNext(pos);
			if(plant->nPlantType==PLANT_GRASS)
			{
				HTREEITEM hitem = m_treePlants.InsertItem(plant->strName, hGrass, TVI_SORT);
				pCtrl->SetItemImage(hitem,3,3);
			}else
			{
				HTREEITEM hitem = m_treePlants.InsertItem(plant->strName, hTree, TVI_SORT);	
				pCtrl->SetItemImage(hitem,2,2);
			}
		}
		CButton *pButton = (CButton *)GetDlgItem(IDC_PLANT_ADD_TO_TERRAIN);
		pButton->SetCheck(0);
		pButton = (CButton *)GetDlgItem(IDC_PLANT_DELETE_FROM_TERRAIN);
		pButton->SetCheck(0);
		
	}
}

int CTerrainModifyPlantsPanel::FindTypeByName(AString name)
{
	for(int i = 0; i<m_nNumTypes;i++)
	{
		if(m_pListTreeName[i] == name)
			return i;
	}
	return -1;
}


void CTerrainModifyPlantsPanel::OnPlantAddSort() 
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
		CELGrassLand *pGrassLand = pMan->GetElGrassLand();
		ASSERT(pGrassLand);	
		pGrassLand->SetA3DTerrain(pMap->GetTerrain()->GetRender());
		CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "Grasses","*.mox");
	
		if (FileDlg.DoModal() != IDOK)
			return; 
		AString strName = FileDlg.GetFullFileName();
		
		DWORD dwTypeID = 0;
		int   nTypes = pGrassLand->GetNumGrassType();
		for(int n = 0; n<nTypes; n++)
		{
			CELGrassType *pType = pGrassLand->GetGrassType(n);
			DWORD id = pType->GetTypeID();
			if(id>dwTypeID) dwTypeID = id;
		}
		
		//if(nVisibleNum > MAX_VISIBLE_GRASS_NUM)
		//{
		//	CString str;
		//	str.Format("所有草的最大可见数超过%d，请减少已加入类型草的最大可见数！",MAX_VISIBLE_GRASS_NUM);
		//	MessageBox(str);
		//	return ;
		//}

		CELGrassType* pGrassType = NULL;
		CELGrassType::GRASSDEFINE_DATA defData;
		defData.bAlphaBlendEnable = false;//alpha rong he
		defData.nAlphaRefValue = 64; //参考直 0 - 255 
		defData.vSightRange = 50.0f;//视野范围 20 - 100
		defData.vSize = 1.0f; //0-10
		defData.vSizeVar = 0.3f;//大小变动系数 0 - 1
		defData.vSoftness = 1.0f;//柔软度 0 - 2
		if(!pGrassLand->AddGrassType(++dwTypeID,defData,5000,strName,&pGrassType))
		{
			g_Log.Log("CTerrainModifyPlantsPanel::OnPlantAddSort()");
			return;
		}
		CTerrainPlants *pPlants = pMan->GetPlants();
		strName.Format("Grass_%d",dwTypeID);
		if(!pPlants->AddPlant(strName,dwTypeID,PLANT_GRASS))
		{
			g_Log.Log("CTerrainModifyPlantsPanel::OnPlantAddSort(),Add plant types to scene failed");
		}
		
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePlantsPanel();
	}
}

bool CTerrainModifyPlantsPanel::IsPlantMaxNum()
{
	/*
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
		CELGrassLand *pGrassLand = pMan->GetElGrassLand();
		ASSERT(pGrassLand);	

		int   nVisibleNum = 0;
		int nTypes = pGrassLand->GetNumGrassType();
		for(int n = 0; n<nTypes; n++)
		{
			CELGrassType *pType = pGrassLand->GetGrassType(n);
			nVisibleNum += pType->GetMaxVisibleCount();
		}
		
		if(nVisibleNum>MAX_VISIBLE_GRASS_NUM)
		{
			CString str;
			str.Format("所有草的最大可见数超过%d，请减少已加入类型草的最大可见数！",MAX_VISIBLE_GRASS_NUM);
			MessageBox(str);
			return true;
		}
	}*/
	return false;
}

void CTerrainModifyPlantsPanel::OnPlantDelete() 
{
	UpdateData(true);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		CTerrainPlants *pPlants  = pManager->GetPlants();
		HTREEITEM ti = m_treePlants.GetSelectedItem();
		CString str = m_treePlants.GetItemText(ti);
		if(m_treePlants.GetParentItem(ti))
		{
			PPLANT temp = pPlants->GetPlant(AString(str));
			if(temp)
			{
				CELGrassLand *pGrassLand = pManager->GetElGrassLand();
				CELForest *pForest = pManager->GetElForest();
				if(temp->GetPlantType() == PLANT_TREE)
				{
					CELTree *tree = pForest->GetTreeTypeByID(temp->nID);
					pForest->DeleteTreeType(tree);
				}else
				if(temp->GetPlantType() == PLANT_GRASS)
				{
					CELGrassType *grass = pGrassLand->GetGrassTypeByID(temp->nID);
					pGrassLand->DeleteGrassType(grass);
				}
				
				pPlants->DeletePlant(temp->strName);
				RaiseAllButton(NULL);
				AUX_GetMainFrame()->SetOtherOperation(false);
				//AUX_GetMainFrame()->GetToolTabWnd()->UpdatePlantsPanel();
				m_treePlants.DeleteItem(ti);
			}
		}
	}
}

void CTerrainModifyPlantsPanel::RaiseAllButton(CButton *pCur)
{
	CButton *pButton = NULL;
	pButton =(CButton*) GetDlgItem(IDC_PLANT_MOVE_Y);
	if(pButton!=pCur)
	pButton->SetCheck(0);
	pButton =(CButton*) GetDlgItem(IDC_PLANT_MOVE_XZ);
	if(pButton!=pCur)
	pButton->SetCheck(0);
	pButton =(CButton*) GetDlgItem(IDC_PLANT_DELETE_FROM_TERRAIN);
	if(pButton!=pCur)
	pButton->SetCheck(0);
	pButton =(CButton*) GetDlgItem(IDC_PLANT_ADD_TO_TERRAIN);
	if(pButton!=pCur)
	pButton->SetCheck(0);
}

void CTerrainModifyPlantsPanel::OnPlantDeleteFromTerrain() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM ti = m_treePlants.GetSelectedItem();
	CString str = m_treePlants.GetItemText(ti);
	CButton *pButton = (CButton *)GetDlgItem(IDC_PLANT_DELETE_FROM_TERRAIN);
	RaiseAllButton(pButton);
	BOOL bCheck = pButton->GetCheck();

	if(m_treePlants.GetParentItem(ti))
	{
		CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
		COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PLANTS);
		
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
			CTerrainPlants *pPlants  = pManager->GetPlants();
			PPLANT pPlant = pPlants->GetPlant((AString)str);
			if(pPlant)
			{
				((CTerrainPlantPaintOperation*)pOperation)->SetPlants(pPlant);
				if(pPlant->GetPlantType()==PLANT_TREE)
					((CTerrainPlantPaintOperation*)pOperation)->SetPaintType(CTerrainPlantPaintOperation::PLANT_TREE_DELETE);
				else ((CTerrainPlantPaintOperation*)pOperation)->SetPaintType(CTerrainPlantPaintOperation::PLANT_GRASS_DELETE);
				if(pViewFrame) 
				{
					if(bCheck) 
						pViewFrame->SetCurrentOperation(pOperation);
					else pMainFrame->SetOtherOperation(false);
				}
				UpdateProperty(false);
			}
		}
	}else
	{
		pButton->SetCheck(0);
	}
}

void CTerrainModifyPlantsPanel::OnDblclkTreePlantList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	/*
	HTREEITEM ti = m_treePlants.GetSelectedItem();
	CString str = m_treePlants.GetItemText(ti);
	if(m_treePlants.GetParentItem(ti))
	{
		CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
		COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PLANTS);
		
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
			CTerrainPlants *pPlants  = pManager->GetPlants();
			PPLANT pPlant = pPlants->GetPlant((AString)str);
			if(pPlant)
			{
				((CTerrainPlantPaintOperation*)pOperation)->SetPlants(pPlant);
				((CTerrainPlantPaintOperation*)pOperation)->SetPaintType(false);
				if(pViewFrame) 
				{
					pViewFrame->SetCurrentOperation(pOperation);
					CButton *pButton = (CButton *)GetDlgItem(IDC_PLANT_ADD_TO_TERRAIN);
					pButton->SetCheck(1);
				}
				UpdateProperty(false);
			}
		}
	}
	*/
	*pResult = 0;
}

void CTerrainModifyPlantsPanel::OnClickTreePlantList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
	if(pViewFrame) 
	{
		pViewFrame->SetCurrentOperation(NULL);
	}
	
	RaiseAllButton(NULL);
	*pResult = 0;
}

void CTerrainModifyPlantsPanel::UpdateProperty(bool bGet)
{
	HTREEITEM ti = m_treePlants.GetSelectedItem();
	if(ti==NULL) return;
	CString str = m_treePlants.GetItemText(ti);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	CString num;
	if(pMap && m_treePlants.GetParentItem(ti))
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		CTerrainPlants *pPlants  = pManager->GetPlants();
		PPLANT pPlant = pPlants->GetPlant((AString)str);
		if(pPlant) 
		{
			if(bGet)
			{//get data 
				UpdateData(true); 
				if( 0 !=strcmp(pPlant->strName,m_strName ))
					return;
				
				
				pPlant->density = m_nDensity;
				
				if(pPlant->GetPlantType() == PLANT_GRASS)
				{
					EnableGrassProperyCtrl(true);
					CELGrassLand *pGrassLand = pManager->GetElGrassLand();
			        CELGrassType *pGrassType = pGrassLand->GetGrassTypeByID(pPlant->nID);
					CELGrassType::GRASSDEFINE_DATA data;
					if(pGrassType==NULL)
					{
						g_Log.Log("草的类型不存在! CTerrainModifyPlantsPanel::UpdateProperty()");
						return;
					}
					data = pGrassType->GetDefineData();
					
					data.nAlphaRefValue = m_nGrassAlphaRef;
					data.vSightRange = m_fGrassSightRange;
					data.vSize = m_fGassSize;
					data.vSizeVar = m_fGrassSizeVar;
					data.vSoftness = m_fGrassSoft;
					
					CButton *pButton = (CButton*)GetDlgItem(IDC_CHECK_GRASS_ENABLEALPHA);
					if(pButton->GetCheck())
						data.bAlphaBlendEnable = true;
					else data.bAlphaBlendEnable = false;
					pGrassType->AdjustDefineData( g_Render.GetA3DEngine()->GetActiveViewport(),data);
					//pGrassType->AdjustMaxVisibleCount(g_Render.GetA3DEngine()->GetActiveViewport(),m_nGrassNum);
				}else
				if(pPlant->GetPlantType() == PLANT_TREE)
				{
					EnableGrassProperyCtrl(false);
				}
			}else
			{//set data
				m_strName = pPlant->strName;
				m_nDensity = pPlant->density;
				num.Format("%d",pPlant->density);
				m_editDensity.SetWindowText(num);
				UpdateData(false);
				if(pPlant->GetPlantType() == PLANT_GRASS)
				{
					EnableGrassProperyCtrl(true);
					CELGrassLand *pGrassLand = pManager->GetElGrassLand();
			        CELGrassType *pGrassType = pGrassLand->GetGrassTypeByID(pPlant->nID);
					CELGrassType::GRASSDEFINE_DATA data;
					if(pGrassType==NULL)
					{
						g_Log.Log("草的类型不存在! CTerrainModifyPlantsPanel::UpdateProperty()");
						return;
					}
					if(pGrassType->IsBadGrass())
					{
						CString err;
						err.Format("这种草(%s)不存在或者是文件损坏!",pGrassType->GetGrassFile());
						MessageBox(err);
						return;
					}
					data = pGrassType->GetDefineData();
					
					m_strRes  = pGrassType->GetGrassFile();
					m_nGrassAlphaRef = data.nAlphaRefValue;
					m_fGrassSightRange = data.vSightRange;
					m_fGassSize = data.vSize;
					m_fGrassSizeVar = data.vSizeVar;
					m_fGrassSoft = data.vSoftness;
					//m_nGrassNum = pGrassType->GetMaxVisibleCount();
					num.Format("%d",m_nGrassAlphaRef);
					m_ctrlAlphaRef.SetWindowText(num);
					
					num.Format("%4.2f",m_fGrassSightRange);
					m_ctrlSightRange.SetWindowText(num);

					num.Format("%4.2f",m_fGassSize);
					m_ctrlGrassSize.SetWindowText(num);

					num.Format("%4.2f",m_fGrassSizeVar);
					m_ctrlSizeVar.SetWindowText(num);

					num.Format("%4.2f",m_fGrassSoft);
					m_ctrlSoft.SetWindowText(num);

					num.Format("%d",m_nGrassNum);
					m_editGrassMaxNum.SetWindowText(num);
					
					CButton *pButton = (CButton*)GetDlgItem(IDC_CHECK_GRASS_ENABLEALPHA);
					pButton->SetCheck(data.bAlphaBlendEnable);
					
				}else
				if(pPlant->GetPlantType() == PLANT_TREE)
				{
					EnableGrassProperyCtrl(false);
					CELForest *pForest = pManager->GetElForest();
					CELTree *pTree = pForest->GetTreeTypeByID(pPlant->nID);
					if(pTree==NULL)
					{
						g_Log.Log("树的类型不存在! CTerrainModifyPlantsPanel::UpdateProperty()");
						return;
					}

					if(pTree->IsBadTree())
					{
						CString err;
						err.Format("这种树(%s)不存在或者是文件损坏,而且没有badtree.spt!",pTree->GetTreeFile());
						MessageBox(err);
						return;
					}

					m_strRes = pTree->GetTreeFile();
				}
				
			}
			UpdateData(false);
		}
	}else
	{
		EnableGrassProperyCtrl(false);
	}
}

void CTerrainModifyPlantsPanel::OnChangeEditPlantsDensity() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	//UpdateProperty(true);
}

void CTerrainModifyPlantsPanel::OnDeltaposSpinPlantsDensity(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM ti = m_treePlants.GetSelectedItem();
	if(ti==NULL) return;
	CString str = m_treePlants.GetItemText(ti);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	CString num;
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		CTerrainPlants *pPlants  = pManager->GetPlants();
		PPLANT pPlant = pPlants->GetPlant((AString)str);
		if(pPlant) 
		{
			m_editDensity.GetWindowText(num);
			pPlant->density = (int)atof(num) - pNMUpDown->iDelta;
			if(pPlant->density>100) pPlant->density = 100;
			else if(pPlant->density<1) pPlant->density = 1;
			num.Format("%d",pPlant->density);
			m_editDensity.SetWindowText(num);
		}
	}	
	*pResult = 0;
}

void CTerrainModifyPlantsPanel::OnPlantAddToTerrain() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM ti = m_treePlants.GetSelectedItem();
	CString str = m_treePlants.GetItemText(ti);
	
	CButton *pButton = (CButton *)GetDlgItem(IDC_PLANT_ADD_TO_TERRAIN);
	RaiseAllButton(pButton);
	BOOL bCheck = pButton->GetCheck();

	if(m_treePlants.GetParentItem(ti))
	{
		CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
		COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PLANTS);
		
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
			CTerrainPlants *pPlants  = pManager->GetPlants();
			PPLANT pPlant = pPlants->GetPlant((AString)str);
			if(pPlant)
			{
				((CTerrainPlantPaintOperation*)pOperation)->SetPlants(pPlant);
				if(pPlant->GetPlantType()==PLANT_TREE)
					((CTerrainPlantPaintOperation*)pOperation)->SetPaintType(CTerrainPlantPaintOperation::PLANT_TREE_ADD);
				else ((CTerrainPlantPaintOperation*)pOperation)->SetPaintType(CTerrainPlantPaintOperation::PLANT_GRASS_ADD);
				if(pViewFrame) 
				{
					if(bCheck) 
						pViewFrame->SetCurrentOperation(pOperation);
					else pMainFrame->SetOtherOperation(false);
				}
				UpdateProperty(false);
			}
		}
	}else
	{
		pButton->SetCheck(0);
	}
}

void CTerrainModifyPlantsPanel::OnPlantMoveY() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM ti = m_treePlants.GetSelectedItem();
	CString str = m_treePlants.GetItemText(ti);
	
	CButton *pButton = (CButton *)GetDlgItem(IDC_PLANT_MOVE_Y);
	RaiseAllButton(pButton);
	BOOL bCheck = pButton->GetCheck();

	if(m_treePlants.GetParentItem(ti))
	{
		CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
		COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PLANTS);
		
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
			CTerrainPlants *pPlants  = pManager->GetPlants();
			PPLANT pPlant = pPlants->GetPlant((AString)str);
			if(pPlant)
			{
				if(pPlant->GetPlantType()==PLANT_GRASS)
				{
					pViewFrame->SetCurrentOperation(NULL); 
					return;
				}
				((CTerrainPlantPaintOperation*)pOperation)->SetPlants(pPlant);
				((CTerrainPlantPaintOperation*)pOperation)->SetPaintType(CTerrainPlantPaintOperation::PLANT_TREE_MOVE_Y);
				if(pViewFrame) 
				{
					if(bCheck) 
						pViewFrame->SetCurrentOperation(pOperation);
					else pMainFrame->SetOtherOperation(false);
				}
				UpdateProperty(false);
			}
		}
	}else
	{
		pButton->SetCheck(0);
	}
}

void CTerrainModifyPlantsPanel::OnPlantMoveXz() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM ti = m_treePlants.GetSelectedItem();
	CString str = m_treePlants.GetItemText(ti);
	
	CButton *pButton = (CButton *)GetDlgItem(IDC_PLANT_MOVE_XZ);
	RaiseAllButton(pButton);
	BOOL bCheck = pButton->GetCheck();

	if(m_treePlants.GetParentItem(ti))
	{
		CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
		COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PLANTS);
		
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
			CTerrainPlants *pPlants  = pManager->GetPlants();
			PPLANT pPlant = pPlants->GetPlant((AString)str);
			if(pPlant)
			{
				if(pPlant->GetPlantType()==PLANT_GRASS) 
				{
					pViewFrame->SetCurrentOperation(NULL); 
					return;
				}
				((CTerrainPlantPaintOperation*)pOperation)->SetPlants(pPlant);
				((CTerrainPlantPaintOperation*)pOperation)->SetPaintType(CTerrainPlantPaintOperation::PLANT_TREE_MOVE_XZ);
				if(pViewFrame) 
				{
					if(bCheck) 
						pViewFrame->SetCurrentOperation(pOperation);
					else pMainFrame->SetOtherOperation(false);
				}
				UpdateProperty(false);
			}
		}
	}else
	{
		pButton->SetCheck(0);
	}
}

void CTerrainModifyPlantsPanel::OnDeltaposSpinGrassAlpharef(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CString num;
	m_ctrlAlphaRef.GetWindowText(num);
	m_nGrassAlphaRef = (int)atof(num) - pNMUpDown->iDelta;
	if(m_nGrassAlphaRef<0) m_nGrassAlphaRef = 0;
	if(m_nGrassAlphaRef>255) m_nGrassAlphaRef = 255;
	num.Format("%d",m_nGrassAlphaRef);
	m_ctrlAlphaRef.SetWindowText(num);

	UpdateProperty();
	*pResult = 0;
}

void CTerrainModifyPlantsPanel::OnDeltaposSpinGrassSightrange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	CString num;
	m_ctrlSightRange.GetWindowText(num);
	m_fGrassSightRange = (float)atof(num) - pNMUpDown->iDelta;
	if(m_fGrassSightRange<20.0f) m_fGrassSightRange = 20.0f;
	if(m_fGrassSightRange>100.0f) m_fGrassSightRange = 100.0f;
	
	num.Format("%4.2f",m_fGrassSightRange);
	m_ctrlSightRange.SetWindowText(num);
	
	UpdateProperty();
	*pResult = 0;
}

void CTerrainModifyPlantsPanel::OnDeltaposSpinGrassSize(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CString num;
	m_ctrlGrassSize.GetWindowText(num);
	m_fGassSize = (float)atof(num) - pNMUpDown->iDelta*0.01f;
	if(m_fGassSize<0.001f) m_fGassSize = 0.001f;
	if(m_fGassSize>10.0f) m_fGassSize = 10.0f;
	
	num.Format("%4.2f",m_fGassSize);
	m_ctrlGrassSize.SetWindowText(num);
	
	UpdateProperty();
	*pResult = 0;
}

void CTerrainModifyPlantsPanel::OnDeltaposSpinGrassSizevar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CString num;
	m_ctrlSizeVar.GetWindowText(num);
	m_fGrassSizeVar = (float)atof(num) - pNMUpDown->iDelta*0.01f;
	if(m_fGrassSizeVar<0.0001f) m_fGrassSizeVar = 0.0001f;
	if(m_fGrassSizeVar>1.0f) m_fGrassSizeVar = 1.0f;
	num.Format("%4.2f",m_fGrassSizeVar);
	m_ctrlSizeVar.SetWindowText(num);
	
	UpdateProperty();
	*pResult = 0;
}

void CTerrainModifyPlantsPanel::OnDeltaposSpinGrassSoft(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CString num;
	m_ctrlSoft.GetWindowText(num);
	
	m_fGrassSoft = (float)atof(num) - pNMUpDown->iDelta*0.1f;
	if(m_fGrassSoft<0.0001f) m_fGrassSoft = 0.0001f;
	if(m_fGrassSoft>2.0f) m_fGrassSoft = 2.0f;
	num.Format("%4.2f",m_fGrassSoft);
	m_ctrlSoft.SetWindowText(num);
	
	UpdateProperty();
	*pResult = 0;
}

void CTerrainModifyPlantsPanel::OnCheckGrassEnablealpha() 
{
	// TODO: Add your control notification handler code here
	UpdateProperty();
}

void CTerrainModifyPlantsPanel::OnChangeEditPlantName() 
{
}

void CTerrainModifyPlantsPanel::OnTreeTypesAdd() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTreeTypesAddDlg  dlg;
		dlg.DoModal();
	}
}

void CTerrainModifyPlantsPanel::OnSelchangedTreePlantList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateProperty(false);
	*pResult = 0;
}

void CTerrainModifyPlantsPanel::OnChangeEditGrassAlpharef() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

}

void CTerrainModifyPlantsPanel::OnChangeEditGrassSightrange() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

}

void CTerrainModifyPlantsPanel::OnChangeEditGrassSize() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CTerrainModifyPlantsPanel::OnChangeEditGrassSizevar() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

}

void CTerrainModifyPlantsPanel::OnChangeEditGrassSoft() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

}

void CTerrainModifyPlantsPanel::EnableGrassProperyCtrl(bool bEnable)
{
	CWnd *pWnd = NULL;
	pWnd = GetDlgItem(IDC_EDIT_GRASS_ALPHAREF);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_SPIN_GRASS_ALPHAREF);
	pWnd->EnableWindow(bEnable);
	
	pWnd = GetDlgItem(IDC_EDIT_GRASS_SIGHTRANGE);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_SPIN_GRASS_SIGHTRANGE);
	pWnd->EnableWindow(bEnable);

	pWnd = GetDlgItem(IDC_EDIT_GRASS_SIZE);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_SPIN_GRASS_SIZE);
	pWnd->EnableWindow(bEnable);

	pWnd = GetDlgItem(IDC_EDIT_GRASS_SIZEVAR);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_SPIN_GRASS_SIZEVAR);
	pWnd->EnableWindow(bEnable);

	pWnd = GetDlgItem(IDC_EDIT_GRASS_SOFT);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_SPIN_GRASS_SOFT);
	pWnd->EnableWindow(bEnable);

	pWnd = GetDlgItem(IDC_EDIT_GRASS_MAX_NUM);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_SPIN_GRASS_MAXNUM);
	pWnd->EnableWindow(bEnable);
	
	pWnd = GetDlgItem(IDC_CHECK_GRASS_ENABLEALPHA);
	pWnd->EnableWindow(bEnable);
}

void CTerrainModifyPlantsPanel::OnKillfocusEditGrassAlpharef() 
{
	// TODO: Add your control notification handler code here
		
	CString num;
	m_ctrlAlphaRef.GetWindowText(num);
	m_nGrassAlphaRef = (int)atof(num);
	if(m_nGrassAlphaRef<0) m_nGrassAlphaRef = 0;
	if(m_nGrassAlphaRef>255) m_nGrassAlphaRef = 255;
	num.Format("%d",m_nGrassAlphaRef);
	m_ctrlAlphaRef.SetWindowText(num);
	UpdateProperty();
}

void CTerrainModifyPlantsPanel::OnKillfocusEditGrassSightrange() 
{
	// TODO: Add your control notification handler code here
		
	CString num;
	m_ctrlSightRange.GetWindowText(num);
	m_fGrassSightRange = (float)atof(num);
	if(m_fGrassSightRange<20.0f) m_fGrassSightRange = 20.0f;
	if(m_fGrassSightRange>100.0f) m_fGrassSightRange = 100.0f;
	
	num.Format("%4.2f",m_fGrassSightRange);
	m_ctrlSightRange.SetWindowText(num);
	
	UpdateProperty();
}

void CTerrainModifyPlantsPanel::OnKillfocusEditGrassSize() 
{
	// TODO: Add your control notification handler code here
	
	CString num;
	m_ctrlGrassSize.GetWindowText(num);
	m_fGassSize = (float)atof(num);
	if(m_fGassSize<0.001f) m_fGassSize = 0.001f;
	if(m_fGassSize>10.0f) m_fGassSize = 10.0f;
	
	num.Format("%4.2f",m_fGassSize);
	m_ctrlGrassSize.SetWindowText(num);
	
	UpdateProperty();
}

void CTerrainModifyPlantsPanel::OnKillfocusEditGrassSizevar() 
{
	// TODO: Add your control notification handler code here
	
	CString num;
	m_ctrlSizeVar.GetWindowText(num);
	m_fGrassSizeVar = (float)atof(num);
	if(m_fGrassSizeVar<0.0001f) m_fGrassSizeVar = 0.0001f;
	if(m_fGrassSizeVar>1.0f) m_fGrassSizeVar = 1.0f;
	num.Format("%4.2f",m_fGrassSizeVar);
	m_ctrlSizeVar.SetWindowText(num);
	
	UpdateProperty();
}

void CTerrainModifyPlantsPanel::OnKillfocusEditGrassSoft() 
{
	// TODO: Add your control notification handler code here
		
	CString num;
	m_ctrlSoft.GetWindowText(num);
	
	m_fGrassSoft = (float)atof(num);
	if(m_fGrassSoft<0.0001f) m_fGrassSoft = 0.0001f;
	if(m_fGrassSoft>2.0f) m_fGrassSoft = 2.0f;
	num.Format("%4.2f",m_fGrassSoft);
	m_ctrlSoft.SetWindowText(num);
	
	UpdateProperty();
}

void CTerrainModifyPlantsPanel::OnKillfocusEditPlantsDensity() 
{
	// TODO: Add your control notification handler code here
	CString num;
	m_editDensity.GetWindowText(num);
	
	m_nDensity = (int)atof(num);
	if(m_nDensity<0) m_nDensity = 0;
	if(m_nDensity>100) m_nDensity = 100;
	num.Format("%d",m_nDensity);
	m_editDensity.SetWindowText(num);
	UpdateProperty();
}

void CTerrainModifyPlantsPanel::OnKillfocusEditPlantName() 
{
	// TODO: Add your control notification handler code here
	AString old = m_strName;
	UpdateData(true);
	if(m_strName.IsEmpty())
	{
		m_strName = old;
		MessageBox("名字不能为空，将恢复原名字");
		UpdateData(false);
		return;
	}
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		CTerrainPlants *pPlants  = pManager->GetPlants();
		HTREEITEM ti = m_treePlants.GetSelectedItem();
		CString str = m_treePlants.GetItemText(ti);
		if(m_treePlants.GetParentItem(ti))
		{
			PPLANT temp = pPlants->GetPlant(AString(str));
			if(!pPlants->GetPlant(AString(m_strName)))
			{
				temp->strName = AString(m_strName);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePlantsPanel();
			}
		}
	}
}
void CTerrainModifyPlantsPanel::OnOK()
{
	CString num;
	m_ctrlAlphaRef.GetWindowText(num);
	m_nGrassAlphaRef = (int)atof(num);
	if(m_nGrassAlphaRef<0) m_nGrassAlphaRef = 0;
	if(m_nGrassAlphaRef>255) m_nGrassAlphaRef = 255;
	num.Format("%d",m_nGrassAlphaRef);
	m_ctrlAlphaRef.SetWindowText(num);

	m_ctrlSightRange.GetWindowText(num);
	m_fGrassSightRange = (float)atof(num);
	if(m_fGrassSightRange<20.0f) m_fGrassSightRange = 20.0f;
	if(m_fGrassSightRange>100.0f) m_fGrassSightRange = 100.0f;
	
	num.Format("%4.2f",m_fGrassSightRange);
	m_ctrlSightRange.SetWindowText(num);

	m_ctrlGrassSize.GetWindowText(num);
	m_fGassSize = (float)atof(num);
	if(m_fGassSize<0.001f) m_fGassSize = 0.001f;
	if(m_fGassSize>10.0f) m_fGassSize = 10.0f;
	
	num.Format("%4.2f",m_fGassSize);
	m_ctrlGrassSize.SetWindowText(num);

	m_ctrlSizeVar.GetWindowText(num);
	m_fGrassSizeVar = (float)atof(num);
	if(m_fGrassSizeVar<0.0001f) m_fGrassSizeVar = 0.0001f;
	if(m_fGrassSizeVar>1.0f) m_fGrassSizeVar = 1.0f;
	num.Format("%4.2f",m_fGrassSizeVar);
	m_ctrlSizeVar.SetWindowText(num);

	m_ctrlSoft.GetWindowText(num);
	m_fGrassSoft = (float)atof(num);
	if(m_fGrassSoft<0.0001f) m_fGrassSoft = 0.0001f;
	if(m_fGrassSoft>2.0f) m_fGrassSoft = 2.0f;
	num.Format("%4.2f",m_fGrassSoft);
	m_ctrlSoft.SetWindowText(num);

	m_editGrassMaxNum.GetWindowText(num);
	m_nGrassNum = (int)atof(num);
	if(m_nGrassNum<1) m_nGrassNum = 1;
	if(m_nGrassNum>10000) m_nGrassNum = 10000;
	num.Format("%d",m_nGrassNum);
	m_editGrassMaxNum.SetWindowText(num);

	m_editDensity.GetWindowText(num);
	m_nDensity = (int)atof(num);
	if(m_nDensity<0) m_nDensity = 0;
	if(m_nDensity>100) m_nDensity = 100;
	num.Format("%d",m_nDensity);
	m_editDensity.SetWindowText(num);
	
	AString old = m_strName;
	UpdateData(true);
	if(m_strName.IsEmpty())
	{
		m_strName = old;
		MessageBox("名字不能为空，将恢复原名字");
		UpdateData(false);
		return;
	}
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		CTerrainPlants *pPlants  = pManager->GetPlants();
		HTREEITEM ti = m_treePlants.GetSelectedItem();
		CString str = m_treePlants.GetItemText(ti);
		if(m_treePlants.GetParentItem(ti))
		{
			PPLANT temp = pPlants->GetPlant(AString(str));
			if(!pPlants->GetPlant(AString(m_strName)))
			{
				temp->strName = AString(m_strName);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePlantsPanel();
			}
		}
	}
	UpdateProperty();
};

void CTerrainModifyPlantsPanel::OnOutofmemorySpinGrassMaxnum(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CTerrainModifyPlantsPanel::OnKillfocusEditGrassMaxNum() 
{
	// TODO: Add your control notification handler code here
	CString num;
	m_editGrassMaxNum.GetWindowText(num);
	m_nGrassNum = (int)atof(num);
	int oldNum = m_nGrassNum;
	if(m_nGrassNum<1) m_nGrassNum = 1;
	if(m_nGrassNum>10000) m_nGrassNum = 10000;
	
	if(IsPlantMaxNum())
	{
		m_nGrassNum = oldNum;
		num.Format("%d",m_nGrassNum);
		m_editGrassMaxNum.SetWindowText(num);
		return;
	}
	
	num.Format("%d",m_nGrassNum);
	m_editGrassMaxNum.SetWindowText(num);
	
	UpdateProperty();
}

void CTerrainModifyPlantsPanel::OnDeltaposSpinGrassMaxnum(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CString num;
	m_editGrassMaxNum.GetWindowText(num);
	m_nGrassNum = (int)atof(num) - pNMUpDown->iDelta;
	int oldNum = m_nGrassNum;
	if(m_nGrassNum<1) m_nGrassNum = 1;
	if(m_nGrassNum>10000) m_nGrassNum = 10000;
	
	
	if(IsPlantMaxNum())
	{
		m_nGrassNum = oldNum;
		num.Format("%d",m_nGrassNum);
		m_editGrassMaxNum.SetWindowText(num);
		return;
	}
	
	num.Format("%d",m_nGrassNum);
	m_editGrassMaxNum.SetWindowText(num);
	
	UpdateProperty();
}

void CTerrainModifyPlantsPanel::OnButtonChangeRes() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM ti = m_treePlants.GetSelectedItem();
	if(ti==NULL) return;
	CString str = m_treePlants.GetItemText(ti);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	CString num;
	if(pMap && m_treePlants.GetParentItem(ti))
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		CTerrainPlants *pPlants  = pManager->GetPlants();
		PPLANT pPlant = pPlants->GetPlant((AString)str);
		if(pPlant) 
		{
			UpdateData(true); 
			if(pPlant->GetPlantType() == PLANT_GRASS)
			{
				
				EnableGrassProperyCtrl(true);
				CELGrassLand *pGrassLand = pManager->GetElGrassLand();
				CELGrassType *pGrassType = pGrassLand->GetGrassTypeByID(pPlant->nID);
				CELGrassType::GRASSDEFINE_DATA data;
				if(pGrassType==NULL) return;
				data = pGrassType->GetDefineData();
				
				data.nAlphaRefValue = m_nGrassAlphaRef;
				data.vSightRange = m_fGrassSightRange;
				data.vSize = m_fGassSize;
				data.vSizeVar = m_fGrassSizeVar;
				data.vSoftness = m_fGrassSoft;
				CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "Grasses","*.mox");
				
				if (FileDlg.DoModal() != IDOK)
					return; 
				AString szGrassFile = FileDlg.GetFullFileName();
				szGrassFile.MakeLower();
				if(szGrassFile.IsEmpty()) return;
				if(szGrassFile.Find(".mox")==-1) return;
				//计算MaskDelta
				CTerrain *pTerrain = pMap->GetTerrain();
				if(pTerrain==NULL) return;
				float m_fTileSize = pTerrain->GetTileSize();
				int maskGrid = ((A3DTerrain2*)pTerrain->GetRender())->GetMaskGrid();
				m_fTileSize = maskGrid * m_fTileSize;
				int nGrassWidth = maskGrid * 4;
				float fMapDelta = m_fTileSize/nGrassWidth;
				
				//计算新数树类型的ID
				DWORD dwTypeID = 0;
				int   nTypes = pGrassLand->GetNumGrassType();
				for(int n = 0; n<nTypes; n++)
				{
					CELGrassType *pType = pGrassLand->GetGrassType(n);
					DWORD id = pType->GetTypeID();
					if(id>dwTypeID) dwTypeID = id;
				}
				
				//复制数据到新草类型中，同时修改可编辑的数据
				CELGrassType *pNewGrassType = NULL;
				pGrassLand->AddGrassType(++dwTypeID,data,5000,szGrassFile,&pNewGrassType);
				if(pNewGrassType==NULL) return;
				pPlant->nID = dwTypeID;
				for( int i = 0; i < ((GRASS_PLANT *)pPlant)->GetFlagNum(); i++)
				{
					DWORD dwChunkID;
					int flag = ((GRASS_PLANT *)pPlant)->GetMaskFlag(i);
					if(flag == -1) continue;
					CELGrassBitsMap *pBitsMap =  pGrassType->GetGrassBitsMap(flag);
					BYTE *pOldBit = pBitsMap->GetBitsMap();
					if(!pNewGrassType->AddGrassBitsMap(pOldBit,pBitsMap->GetCenter(),fMapDelta,pBitsMap->GetWidth(),pBitsMap->GetWidth(),dwChunkID))
					{
						g_Log.Log("CTerrainModifyPlantsPanel::OnButtonChangeRes(), Failed to AddGrassBitsMap");
						ASSERT(false);
						return;
					}
					((GRASS_PLANT *)pPlant)->m_listMaskFlags[i] = dwChunkID;
				}

				pGrassLand->DeleteGrassType(pGrassType);
			
			}else if(pPlant->GetPlantType() == PLANT_TREE)
			{
				CELForest *pForest = pManager->GetElForest();
				CELTree *pTree = pForest->GetTreeTypeByID(pPlant->nID);
				
				CTreeTypesAddDlg  dlg;
				dlg.m_bIsChange = true;
				if(IDOK!=dlg.DoModal())
					return;
				dlg.m_strSpeedTreeName.MakeLower();
				dlg.m_strDDSName.MakeLower();
				if(dlg.m_strSpeedTreeName.Find(".spt")==-1) 
				{
					MessageBox("没有指定spt文件！");
					return;
				}
				if(dlg.m_strDDSName.Find(".dds")==-1) 
				{
					MessageBox("没有指定DDS文件！");
					return;
				}

				if(pTree) pForest->DeleteTreeType(pTree);
				pTree = pForest->AddTreeType(pPlant->nID,dlg.m_strSpeedTreeName,dlg.m_strDDSName);
				if(pTree==NULL)
				{
					MessageBox("更改树类型失败，可能是不正确的文件类型或是文件不存在,请重新指定！");
					return;
				}
				pManager->UpdateLighting(g_bIsNight);
				m_strRes = pTree->GetTreeFile();

				ALISTPOSITION pos = pPlant->m_listPlantsPos.GetTailPosition();
				while(pos)
				{
					PPLANTPOS ptemp = pPlant->m_listPlantsPos.GetPrev(pos);
					
					DWORD dwID;
					pTree->AddTree(A3DVECTOR3(ptemp->x,ptemp->y,ptemp->z),dwID);
					ptemp->id = dwID;	
				}
			}
			UpdateData(false);
			pMap->SetModifiedFlag(true);
		}
	}
}

// TerrainModifyPanel.cpp : implementation file
//

#include "global.h"
#include "MainFrm.h"
#include "elementeditor.h"
#include "TerrainModifyPanel.h"
#include "OperationManager.h"
#include "TerrainPage.h"
#include "ObstructOperation.h"
#include "ObstructOperation2.h"
#include "pathmap\BitImage.h"
#include "pathmap\BlockImage.h"
#include "SceneObjectManager.h"
#include "PathMap\PlayerPassMapGenerator.h"
#include "DlgMergePlayerPassMap.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyPanel dialog


CTerrainModifyPanel::CTerrainModifyPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainModifyPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainModifyPanel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTerrainModifyPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainModifyPanel)
	DDX_Control(pDX, IDC_COMBO_SEL_PPASSMAP_LAYER, m_cbSelPPassMapLayer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainModifyPanel, CDialog)
	//{{AFX_MSG_MAP(CTerrainModifyPanel)
	ON_BN_CLICKED(IDC_TERRAIN_MODIFY_STRETCH, OnTerrainModifyStretch)
	ON_BN_CLICKED(IDC_TERRAIN_MODIFY_EMPTY, OnTerrainModifyEmpty)
	ON_BN_CLICKED(IDC_TERRAIN_MODIFY_SMOOTH, OnTerrainModifySmooth)
	ON_BN_CLICKED(IDC_TERRAIN_MODIFY_NOISE, OnTerrainModifyNoise)
	ON_BN_CLICKED(IDC_TERRAIN_MODIFY_LAYMASK, OnTerrainModifyLaymask)
	ON_BN_CLICKED(IDC_TERRAIN_MODIFY_LAYMASK_SOFT, OnTerrainModifyLaymaskSoft)
	ON_BN_CLICKED(IDC_TERRAIN_MODIFY_PLANT, OnTerrainModifyPlant)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT_RED, OnTerrainObstructRed)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT_GREEN, OnTerrainObstructGreen)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT_CONVEX, OnTerrainObstructConvex)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT_MAP_TRANS, OnTerrainObstructMapTrans)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT_MAP_SETSEED, OnTerrainObstructMapSetseed)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT_MAP_YELLOW, OnTerrainObstructYellow)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT_MAP_SPACE_CREATE, OnTerrainObstructMapSpaceCreate)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT_SAVE_ALL_MAP, OnTerrainObstructSaveAllMap)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT2_MAP_TRANS, OnTerrainObstruct2MapTrans)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT2_MAP_SETSEED, OnTerrainObstruct2MapSetseed)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT2_RED, OnTerrainObstruct2Red)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT2_GREEN, OnTerrainObstruct2Green)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT2_YELLOW, OnTerrainObstruct2Yellow)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT2_SAVE_ALL_MAP, OnTerrainObstruct2SaveAllMap)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_PPASSMAP_LAYER, OnSelchangeComboSelPpassmapLayer)
	ON_BN_CLICKED(IDC_TERRAIN_OBSTRUCT2_MERGE, OnTerrainObstruct2Merge)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyPanel message handlers


//该对话框所有的按纽都在这儿同时设置Check属性，以后加上去的按纽
//也最好在这儿加入，防止造成管理上的混乱。
void CTerrainModifyPanel::CheckOtherButtonCheck(CButton *pButton)
{
	int nIDs[] = {
		IDC_TERRAIN_MODIFY_STRETCH, IDC_TERRAIN_MODIFY_EMPTY, IDC_TERRAIN_MODIFY_SMOOTH,
			IDC_TERRAIN_MODIFY_NOISE, IDC_TERRAIN_MODIFY_LAYMASK, IDC_TERRAIN_MODIFY_LAYMASK_SOFT,
			IDC_TERRAIN_MODIFY_PLANT, IDC_TERRAIN_OBSTRUCT_RED, IDC_TERRAIN_OBSTRUCT_GREEN,
			IDC_TERRAIN_OBSTRUCT_CONVEX, IDC_TERRAIN_OBSTRUCT_MAP_TRANS, IDC_TERRAIN_OBSTRUCT_MAP_SETSEED,
			IDC_TERRAIN_OBSTRUCT_MAP_YELLOW, IDC_TERRAIN_OBSTRUCT2_MAP_TRANS,
			IDC_TERRAIN_OBSTRUCT2_MAP_TRANS, IDC_TERRAIN_OBSTRUCT2_MAP_SETSEED,
			IDC_TERRAIN_OBSTRUCT2_YELLOW, IDC_TERRAIN_OBSTRUCT2_RED, IDC_TERRAIN_OBSTRUCT2_GREEN,
	};
	CButton *pTemp;
	for (int i(0); i < sizeof(nIDs)/sizeof(nIDs[0]); ++ i)
	{
		pTemp = (CButton *)GetDlgItem(nIDs[i]);
		if(pButton != pTemp)
		{
			if(pTemp->GetCheck()!=0)
				pTemp->SetCheck(0);
		}
	}
}


void CTerrainModifyPanel::OnTerrainModifyStretch() 
{
	// TODO: Add your control notification handler code here
	
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_MODIFY_STRETCH);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[2] = {CTerrainPage::PANEL_MODIFY,CTerrainPage::PANEL_MODIFYSTRETCH};
				bool aOpenFlags[2] = {true,true};
				pParent->ShowPanels(aIndices, aOpenFlags, 2);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}
}

void CTerrainModifyPanel::OnTerrainModifyEmpty() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_MODIFY_EMPTY);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					if(pViewFrame) pViewFrame->SetCurrentOperation(0);//NULL Operation
				}
			}
		}
	}
}

void CTerrainModifyPanel::OnTerrainModifySmooth() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_MODIFY_SMOOTH);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);
				
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);//NULL Operation
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[2] = {CTerrainPage::PANEL_MODIFY,CTerrainPage::PANEL_MODIFYSMOOTH};
				bool aOpenFlags[2] = {true,true};
				pParent->ShowPanels(aIndices, aOpenFlags, 2);
				
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_SMOOTH);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}
}

void CTerrainModifyPanel::OnTerrainModifyNoise() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_MODIFY_NOISE);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[2] = {CTerrainPage::PANEL_MODIFY,CTerrainPage::PANEL_MODIFYNOISE};
				bool aOpenFlags[2] = {true,true};
				pParent->ShowPanels(aIndices, aOpenFlags, 2);
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_NOISE);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}
}

void CTerrainModifyPanel::OnTerrainModifyLaymask() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_MODIFY_LAYMASK);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[2] = {CTerrainPage::PANEL_MODIFY,CTerrainPage::PANEL_MODIFYLAYMASK};
				bool aOpenFlags[2] = {true,true};
				pParent->ShowPanels(aIndices, aOpenFlags, 2);
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
				pParent->m_TerrainModifyLayMaskPanel.UpdateLayList();
			}
		}
	}
}

void CTerrainModifyPanel::OnTerrainModifyLaymaskSoft() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_MODIFY_LAYMASK_SOFT);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[2] = {CTerrainPage::PANEL_MODIFY,CTerrainPage::PANEL_MODIFYLAYSMOOTH};
				bool aOpenFlags[2] = {true,true};
				pParent->ShowPanels(aIndices, aOpenFlags, 2);
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_MASK_SMOOTH);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
				pParent->m_TerrainModifyLayMaskSmoothPanel.UpdateLayList();
			}
		}
	}
}

void CTerrainModifyPanel::RiseUpAllButton()
{
	CheckOtherButtonCheck(0);
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame)
	{
		//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
		//if(pViewFrame) pViewFrame->SetCurrentOperation(0);//NULL Operation
		pMainFrame->SetOtherOperation(false);
	}
}

void CTerrainModifyPanel::OnTerrainModifyPlant() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_MODIFY_PLANT);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[2] = {CTerrainPage::PANEL_MODIFY,CTerrainPage::PANEL_MODIFYPLANTS};
				bool aOpenFlags[2] = {true,true};
				pParent->ShowPanels(aIndices, aOpenFlags, 2);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePlantsPanel();
					pMainFrame->SetOtherOperation(true);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PLANTS);
					//if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}
}
void CTerrainModifyPanel::OnTerrainObstructRed() 
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	// TODO: Add your control notification handler code here
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT_RED);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				if(!pMap->GetPassMapGen()->GetPassMap())
				{
					pButton->SetCheck(0);
					return;
				}
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_OBSTRUCT);
					((CObstructOperation*)pOperation)->Init(CObstructOperation::OBSTRUCT_RED);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}
}

void CTerrainModifyPanel::OnTerrainObstructGreen() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT_GREEN);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				if(!pMap->GetPassMapGen()->GetPassMap())
				{
					pButton->SetCheck(0);
					return;
				}
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_OBSTRUCT);
					((CObstructOperation*)pOperation)->Init(CObstructOperation::OBSTRUCT_GREEN);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}
}

void CTerrainModifyPanel::OnTerrainObstructConvex() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT_CONVEX);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_OBSTRUCT);
					((CObstructOperation*)pOperation)->Init(CObstructOperation::OBSTRUCT_CH_CANNOT_PASS);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}
}

// compute the reachable map
void CTerrainModifyPanel::OnTerrainObstructMapTrans() 
{
	// TODO: Add your control notification handler code here
	// Generate the pass map!
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT_MAP_TRANS);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				// 进入到NPC可达图显示状态
				CString strMsg=(pMap->GetPassMapGen()->GetPassMap())?"当前可达图已经存在，确认要重新计算吗？":"确认要计算可达图吗？";
				strMsg+= "这可能将花费很长时间！";
				if(MessageBox(strMsg, NULL, MB_OKCANCEL)==IDOK)
				{
					BeginWaitCursor();

					//  Initialize the global PassMap Generator object
					if(pMap->GetPassMapGen()->GetPassMap()) pMap->GetPassMapGen()->Release();
					float fTrnLen = pMap->GetTerrain() ? pMap->GetTerrain()->GetTerrainSize() : STANDARD_PASSMAP_SIZE;
					pMap->GetPassMapGen()->SetSampleInfo(STANDARD_PASSMAP_SAMPLE_STEP, fTrnLen,fTrnLen);
					pMap->GetPassMapGen()->SetPFSphereInfo();
					
					// Compute the pass map
					pMap->GetPassMapGen()->ComputePassMap();

					EndWaitCursor();
				}
				else
					if(!pMap->GetPassMapGen()->GetPassMap())
					{
						pButton->SetCheck(0);
						return;	// 如果尚未有任何可达图，则直接返回
					}

				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_OBSTRUCT);
					//((CObstructOperation*)pOperation)->Init(CObstructOperation::OBSTRUCT_SET_SEED);
					((CObstructOperation*)pOperation)->Init(CObstructOperation::OBSTRUCT_START);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}
	
	
}

void CTerrainModifyPanel::OnTerrainObstructMapSetseed() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT_MAP_SETSEED);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
				}
			}
			
		}else 
		{
			// now, we enter the mode of set pass map generator's seed !
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				if(!pMap->GetPassMapGen()->GetPassMap())
				{
					float fTrnLen = pMap->GetTerrain() ? pMap->GetTerrain()->GetTerrainSize() : STANDARD_PASSMAP_SIZE;
					pMap->GetPassMapGen()->SetSampleInfo(STANDARD_PASSMAP_SAMPLE_STEP,fTrnLen,fTrnLen);
				}
				pMap->GetPassMapGen()->SetPFSphereInfo();

				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_OBSTRUCT);
					((CObstructOperation*)pOperation)->Init(CObstructOperation::OBSTRUCT_SET_SEED);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}	
}

void CTerrainModifyPanel::OnTerrainObstructYellow() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT_MAP_YELLOW);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
					//CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					//if(pViewFrame) pViewFrame->SetCurrentOperation(0);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				if(!pMap->GetPassMapGen()->GetPassMap())
				{
					pButton->SetCheck(0);
					return;
				}
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);

				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_OBSTRUCT);
					((CObstructOperation*)pOperation)->Init(CObstructOperation::OBSTRUCT_YELLOW);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}	
}

void CTerrainModifyPanel::OnTerrainObstructMapSpaceCreate() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CButton *pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT_MAP_SPACE_CREATE);
		BeginWaitCursor();
		abase::vector<CConvexHullData *> CHs;
		SPOctree::CSpacePassableOctree* pass = pMap->GetSpacePass();
		pMap->GetSceneObjectMan()->GetAllConvexHulls(CHs);
		pass->Release();
		pass->Init(CHs);
		pass->BuildTree();
		pMap->SaveSpacePassMap();
		EndWaitCursor();
		if(pButton) pButton->SetCheck(0);
	}
}

void CTerrainModifyPanel::OnTerrainObstructSaveAllMap() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		if(!pMap->SaveAllPassMap())
			MessageBox("保存通过图失败! 详细情况请看ElementEditor.log。");
	}
	CButton *pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT_SAVE_ALL_MAP);
	pButton->SetCheck(0);
}


void CTerrainModifyPanel::OnTerrainObstruct2MapTrans() 
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT2_MAP_TRANS);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()!=0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				// 进入到NPC可达图显示状态
				CString strMsg=(pMap->GetPlayerPassMapGen()->GetPassMap())?"当前可达图已经存在，确认要重新计算吗？":"确认要计算可达图吗？";
				strMsg+= "这可能将花费很长时间！";
				if(MessageBox(strMsg, NULL, MB_OKCANCEL)==IDOK)
				{
					BeginWaitCursor();
					
					//  Initialize the global PassMap Generator object
					if(pMap->GetPlayerPassMapGen()->GetPassMap()) pMap->GetPlayerPassMapGen()->Release();
					pMap->GetPlayerPassMapGen()->SetSampleInfo();
					pMap->GetPlayerPassMapGen()->SetPFSphereInfo();
					
					// Compute the pass map
					pMap->GetPlayerPassMapGen()->ComputePassMap();
					
					EndWaitCursor();
				}
			}
			pButton->SetCheck(0);
		}
		CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
		if(pParent)
		{
			//	Hide panels
			pParent->HideAllPanels();
			int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
			bool aOpenFlags[1] = {true};
			pParent->ShowPanels(aIndices, aOpenFlags, 1);
			
			CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
			if(pMainFrame)
			{
				pMainFrame->SetOtherOperation(false);
			}
		}
	}
	
	
}

void CTerrainModifyPanel::OnTerrainObstruct2MapSetseed() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT2_MAP_SETSEED);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);
				
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
				}
			}
			
		}else 
		{
			// now, we enter the mode of set pass map generator's seed !
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				if(!pMap->GetPlayerPassMapGen()->GetPassMap())
				{
					pMap->GetPlayerPassMapGen()->SetSampleInfo();
				}
				pMap->GetPlayerPassMapGen()->SetPFSphereInfo();
				
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);
				
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_OBSTRUCT2);
					((CObstructOperation2*)pOperation)->Init(CObstructOperation2::OBSTRUCT_SET_SEED);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}	
}

void CTerrainModifyPanel::OnTerrainObstruct2RedGreenYellow(int nDlgItemID, int iOperation)
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	CButton *pButton;
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	pButton = (CButton *)GetDlgItem(nDlgItemID);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()==0)
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);
				
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(false);
				}
			}
			
		}else 
		{
			CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
			if(pParent)
			{
				if(!pMap->GetPlayerPassMapGen()->GetPassMap())
				{
					pButton->SetCheck(0);
					return;
				}
				//	Hide panels
				pParent->HideAllPanels();
				int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
				bool aOpenFlags[1] = {true};
				pParent->ShowPanels(aIndices, aOpenFlags, 1);
				
				CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if(pMainFrame)
				{
					pMainFrame->SetOtherOperation(true);
					CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
					COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_OBSTRUCT2);
					((CObstructOperation2*)pOperation)->Init(iOperation);
					if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
				}
			}
		}
	}
}

void CTerrainModifyPanel::OnTerrainObstruct2Green()
{
	OnTerrainObstruct2RedGreenYellow(IDC_TERRAIN_OBSTRUCT2_GREEN, CObstructOperation2::OBSTRUCT_GREEN);
}

void CTerrainModifyPanel::OnTerrainObstruct2Red()
{
	OnTerrainObstruct2RedGreenYellow(IDC_TERRAIN_OBSTRUCT2_RED, CObstructOperation2::OBSTRUCT_RED);
}

void CTerrainModifyPanel::OnTerrainObstruct2Yellow()
{
	OnTerrainObstruct2RedGreenYellow(IDC_TERRAIN_OBSTRUCT2_YELLOW, CObstructOperation2::OBSTRUCT_YELLOW);
}


void CTerrainModifyPanel::OnTerrainObstruct2SaveAllMap() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		if(!pMap->SaveCurPlayerPassMap())
			MessageBox("保存通过图失败! 详细情况请看ElementEditor.log。");
	}
	CButton *pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT2_SAVE_ALL_MAP);
	pButton->SetCheck(0);
}

BOOL CTerrainModifyPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int nMaxLayer = CElementMap::GetPlayerPassMapLayerCount();
	CString strTemp;
	for (int i(0); i < nMaxLayer; ++ i)
	{
		strTemp.Format("Layer%d", i);
		m_cbSelPPassMapLayer.AddString(strTemp);
	}

	return TRUE;
}

void CTerrainModifyPanel::OnSelchangeComboSelPpassmapLayer() 
{
	// TODO: Add your control notification handler code here
	int sel = m_cbSelPPassMapLayer.GetCurSel();
	if(sel >= 0)
	{
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap && pMap->GetPlayerPassMapCurLayer() != sel)
		{
			if (!pMap->CanChangePlayerPassMapLayer(sel)){
				m_cbSelPPassMapLayer.SetCurSel(pMap->GetPlayerPassMapCurLayer());
				AfxMessageBox("请优先使用低级层", MB_OK);
				return;
			}
			CString str;
			str.Format("需要保存第 %d 层的数据么?",pMap->GetPlayerPassMapCurLayer());
			if(IDYES==AfxMessageBox(str,MB_YESNO)) pMap->SaveCurPlayerPassMap();
			pMap->SetPlayerPassMapCurLayer(sel);
			pMap->LoadCurPlayerPassMap();
		}
		
	}
}

void CTerrainModifyPanel::OnPlayerPassMapCurLayerChange()
{
	int iCurPlayerLayer = -1;	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if (pMap){
		iCurPlayerLayer = pMap->GetPlayerPassMapCurLayer();
	}
	m_cbSelPPassMapLayer.SetCurSel(iCurPlayerLayer);
}

void CTerrainModifyPanel::OnTerrainObstruct2Merge() 
{
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	CButton *pButton = (CButton *)GetDlgItem(IDC_TERRAIN_OBSTRUCT2_MERGE);
	if(pButton)
	{
		CheckOtherButtonCheck(pButton);
		if(pButton->GetCheck()!=0)
		{
			pButton->SetCheck(0);
		}
		CTerrainPage *pParent = (CTerrainPage*)(GetParent()->GetParent());
		if(pParent)
		{
			//	Hide panels
			pParent->HideAllPanels();
			int aIndices[1] = {CTerrainPage::PANEL_MODIFY};
			bool aOpenFlags[1] = {true};
			pParent->ShowPanels(aIndices, aOpenFlags, 1);			
			CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
			if(pMainFrame)
			{
				pMainFrame->SetOtherOperation(false);
			}
		}
	}
	CDlgMergePlayerPassMap dlgMerge(AUX_GetMainFrame());
	dlgMerge.DoModal();
}

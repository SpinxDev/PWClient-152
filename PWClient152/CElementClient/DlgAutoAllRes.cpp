/*
 * FILE: DlgAutoAllRes.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/4/6
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoAllRes.h"
#include "DlgAutoCtrl.h"
#include "DlgAutoSwitchDlg.h"
#include "DlgAutoBaseMountain.h"
#include "DlgAutoBaseMountain2.h"
#include "DlgAutoBaseRoad.h"
#include "DlgAutoRoad.h"
#include "DlgAutoBaseLocalTex.h"
#include "DlgAutoOutdoorTree.h"
#include "DlgAutoOutdoorGrass.h"
#include "DlgAutoMountain.h"
#include "DlgAutoMountain2.h"
#include "DlgAutoLocalTexture.h"
#include "DlgAutoNote2.h"
#include "DlgAutoOutdoorBuilding.h"
#include "DlgAutoOutdoorOtherObject.h"
#include "DlgAutoIndoorFurniture.h"
#include "DlgAutoIndoorPlant.h"
#include "DlgAutoIndoorToy.h"
#include "DlgAutoIndoorVirtu.h"


#include "EC_HomeDlgsMgr.h"

#include "AutoBuilding.h"
#include "AutoSelectedArea.h"
#include "AutoTerrainAction.h"
#include "AutoBuildingOperation.h"
#include "AutoScene.h"

#include "Aui\\AUIStillImageButton.h"
#include "Aui\\AUITreeView.h"

#include <windowsx.h>


AUI_BEGIN_COMMAND_MAP(CDlgAutoAllRes, CDlgHomeBase)

AUI_ON_COMMAND("DelRes", OnCommandDelRes)
AUI_ON_COMMAND("ModifyRes", OnCommandModifyRes)
AUI_ON_COMMAND("CancelRes", OnCommandCancelRes)


AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoAllRes, CDlgHomeBase)

AUI_ON_EVENT("TreeView_AllRes", WM_LBUTTONUP, OnEventResTree)

AUI_END_EVENT_MAP()

CDlgAutoAllRes::CDlgAutoAllRes()
{
	Clear();
}


CDlgAutoAllRes::~CDlgAutoAllRes()
{
	Clear();
}

void CDlgAutoAllRes::Clear()
{
	m_pTreeAllRes = NULL;
	
	for( int i = 0; i < ARTI_NUM; i++)
	{
		m_pTreeParentItem[i] = NULL;
	}
	
	for( int j = 0; j < B_NUM; j++)
	{
		m_pBtn[j] = NULL;
	}

	m_pCurModifyingItem = NULL;
}
*/

//void CDlgAutoAllRes::InsertResTreeChildItem(AllResTreeItem_e i_Parent, ACHAR* i_szName, int i_nID /* = 0 */)
/*
{
// 	ACHAR* pszName[] = {
// 			_AL("基本地形"),
// 			_AL("山丘"),
// 			_AL("山脉"),
// 			_AL("路"),
// 			_AL("水"),
// 			_AL("特殊区域"),
// 			_AL("多边形树"),
// 			_AL("线形树"),
// 			_AL("草"),
// 			_AL("建筑"),
// 			_AL("其他"),
// 			_AL("家具"),
// 			_AL("植物"),
// 			_AL("玩具"),
// 			_AL("古董")
// 	};

	if(m_pTreeParentItem[i_Parent] == NULL)
	{
		m_pTreeParentItem[i_Parent] = m_pTreeAllRes->InsertItem(GetStringFromTable(1000 +i_Parent));
		m_pTreeAllRes->SetItemData(m_pTreeParentItem[i_Parent], i_Parent);
	}	

	P_AUITREEVIEW_ITEM pItem = m_pTreeAllRes->InsertItem(i_szName, m_pTreeParentItem[i_Parent]);
	m_pTreeAllRes->SetItemData(pItem, i_nID);

	m_pTreeAllRes->Expand(m_pTreeParentItem[i_Parent], AUITREEVIEW_EXPAND_EXPAND);
	m_pTreeAllRes->SelectItem(pItem);
	
	SetBtnsEnable(true, false, true);

	return;
}
																								


void CDlgAutoAllRes::OnShowDialog()
{
	this->SetCanMove(false);
	return;
}
	
bool CDlgAutoAllRes::OnInitContext()
{
	if( !InitResource())
		return false;

	InitVars();

	LoadDefault();
	UpdateData();
	return true;
}

#define GET_BTN_POINTER(a) DDX_Control( "BTN_" # a, m_pBtn[a]);
void CDlgAutoAllRes::DoDataExchange(bool bSave)
{
	DDX_Control("TreeView_AllRes", m_pTreeAllRes);
	
	GET_BTN_POINTER(B_DEL);
	GET_BTN_POINTER(B_CANCEL);
	GET_BTN_POINTER(B_MODIFY);
	

}

void CDlgAutoAllRes::LoadDefault()
{
	SetBtnsEnable(false, false, false);
	

	return;
}

bool CDlgAutoAllRes::InitResource()
{
	m_pTreeAllRes->DeleteAllItems();
	return true;
}


void CDlgAutoAllRes::OnCommandModifyRes(const char* szCommand)
{
	P_AUITREEVIEW_ITEM  pItem = m_pTreeAllRes->GetSelectedItem();
	if(pItem == NULL)
		return ;
	int i;
	for(  i = 0; i < ARTI_NUM; i++)
	{
		if(pItem == m_pTreeParentItem[i])
			return;
	}

	P_AUITREEVIEW_ITEM pParent = m_pTreeAllRes->GetParentItem(pItem);
	int nType = 0;
	for( i = 0; i < ARTI_NUM; i++)
	{
		if(pParent == m_pTreeParentItem[i])
		{
			nType = i;
			break;
		}
	}

	switch( nType)
	{		
		case ARTI_MOUNTAIN_POLY:
			TurnToMountainPolyMode();
			break;
		case ARTI_MOUNTAIN_LINE:
			TurnToMountainLineMode();
			break;
		case ARTI_ROAD:
			TurnToRoadMode();
			break;
		case ARTI_LOCALTEX:
			TurnToLocalTexMode();
			break;
		case ARTI_TREE_POLY:
			TurnToTreePolyMode();
			break;
		case ARTI_TREE_LINE:
			TurnToTreeLineMode();
			break;
		case ARTI_GRASS:
			TurnToGrassMode();
			break;

		case ARTI_TERRAIN:
		case ARTI_WATER:
			break;
			
		case ARTI_BUILDING:
			TurnToBuildingMode();
			SetModelOperation(pItem);
			
			break;

		case ARTI_OTHEROBJECT:
			TurnToOtherObjectMode();
			SetModelOperation(pItem);
			
			break;
		case ARTI_FURNITURE:
			TurnToFurnitureMode();
			SetModelOperation(pItem);
			
			break;

		case ARTI_PLANT:
			TurnToPlantMode();
			SetModelOperation(pItem);
			
			break;

		case ARTI_TOY:
			TurnToPlantMode();
			SetModelOperation(pItem);
			
			break;

		case ARTI_VIRTU:
			TurnToVirtuMode();
			SetModelOperation(pItem);
			
			break;
			
		default:
			break;
	}

	m_pCurModifyingItem = pItem;

	SetBtnsEnable(false, true, true);

	return;
}

void CDlgAutoAllRes::OnCommandDelRes(const char* szCommand)
{
	P_AUITREEVIEW_ITEM  pItem = m_pTreeAllRes->GetSelectedItem();
	if(pItem == NULL)
		return ;

	for( int i = 0; i < ARTI_NUM; i++)
	{
		if(pItem == m_pTreeParentItem[i])
			return;
	}

	if(!DelResAction(pItem))
	{
		ASSERT(0 && "Delete Res Action Error!");
	}

	P_AUITREEVIEW_ITEM pParentItem = m_pTreeAllRes->GetParentItem(pItem);

	m_pTreeAllRes->DeleteItem(pItem);
	if( !m_pTreeAllRes->ItemHasChildren(pParentItem))
	{
		for( int j = 0; j < ARTI_NUM; j++)
		{
			if( m_pTreeParentItem[j] == pParentItem)
			{
				m_pTreeParentItem[j] = NULL;
			}
		}
		
		m_pTreeAllRes->DeleteItem(pParentItem);
		
	}

	GetHomeDlgsMgr()->SetModifying(false);
	GetHomeDlgsMgr()->SetModelState(CECHomeDlgsMgr::MSE_CREATE);
	m_pTreeAllRes->SelectItem(NULL);
	SetBtnsEnable(false, false, false);
	
// 	pItem = m_pTreeAllRes->GetSelectedItem();
// 	if( pItem == NULL)
// 	{
// 		SetBtnsEnable(false, false, false);
// 	}
// 	else if( m_pTreeAllRes->ItemHasChildren(pItem))
// 	{	
// 		SetBtnsEnable(false, false, false);
// 
// 	}
// 	else
// 	{
// 		SetBtnsEnable(true, false, true);
// 	}

	//确认个对话框的纹理操作是否可用
	MakeSureTexOpEnable();
	
	return ;
}

bool CDlgAutoAllRes::DelResAction( const P_AUITREEVIEW_ITEM i_pItem) const
{
	ASSERT(i_pItem != NULL);

	P_AUITREEVIEW_ITEM pParentItem = m_pTreeAllRes->GetParentItem(i_pItem);
	
	int i = 0;
	for( ; i < ARTI_NUM; i++)
	{
		if( pParentItem == m_pTreeParentItem[i])
		{
			break;
		}
	}

	if( i == ARTI_NUM)
	{
		return false;
	}

	int nActType = -1;
	switch( i)
	{
		case ARTI_TERRAIN:
			break;
		case ARTI_MOUNTAIN_POLY:
			nActType = TAT_HILL_POLY;
			break;
		case ARTI_MOUNTAIN_LINE:
			nActType = TAT_HILL_LINE;
			break;
		case ARTI_ROAD:
			nActType = TAT_ROAD;
			break;
		case ARTI_WATER:
			break;
		case ARTI_LOCALTEX:
			nActType = TAT_TEXTURE_POLY;
			break;
		case ARTI_TREE_POLY:
			nActType = TAT_FOREST_POLY;
			break;
		case ARTI_TREE_LINE:
			nActType = TAT_FOREST_LINE;
			break;
		case ARTI_GRASS:
			nActType = TAT_GRASS_POLY;
			break;
		case ARTI_BUILDING:
			nActType = TAT_BUILDING;
			break;
		case ARTI_OTHEROBJECT:
			nActType = TAT_BUILDING;
			break;
		case ARTI_FURNITURE:
		case ARTI_PLANT:
		case ARTI_TOY:
		case ARTI_VIRTU:
			nActType = TAT_BUILDING;
			break;
		default:
			break;
	}
	
	if( nActType == -1)
		return false;


	int nIdx = 0;

	if( nActType != TAT_BUILDING)
	{
		nIdx = GetTreeViewItemIdx(i_pItem);
		if( nIdx == -1)
		{
			ASSERT(0);
		}
		GetAutoScene()->GetAutoSelectedArea()->DeleteAllViewPoints();
	}
	else 
	{
		int nID = m_pTreeAllRes->GetItemData(i_pItem);
		nIdx = GetAutoScene()->GetAutoBuilding()->GetIndexByID(nID);
	}

	GetAutoScene()->DeleteAction(nActType, nIdx, true);
	if (nActType == TAT_HILL_RECT 
		|| nActType == TAT_HILL_POLY 
		|| nActType == TAT_HILL_LINE
		|| nActType == TAT_ROAD
		|| nActType == TAT_TERRAIN_FLAT)
	{
		GetAutoScene()->RecreateTerrainRender(true);
	}
	return true;
}


int CDlgAutoAllRes::GetTreeViewItemIdx(const P_AUITREEVIEW_ITEM i_pItem) const
{
	if( i_pItem == NULL)
		return -1;

	for( int i = 0; i < ARTI_NUM; i++)
	{
		if( i_pItem == m_pTreeParentItem[i])
		{
			return -1;
		}
	}


	int nIdx = 0;

	P_AUITREEVIEW_ITEM pPreItem = m_pTreeAllRes->GetPrevSiblingItem(i_pItem);
	P_AUITREEVIEW_ITEM pCurItem = NULL;
	while(pPreItem != NULL)
	{
		nIdx ++;
		pCurItem = pPreItem;
		pPreItem = m_pTreeAllRes->GetPrevSiblingItem(pCurItem);
	}
	
	return nIdx;
}

void CDlgAutoAllRes::OnEventResTree(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if(GetHomeDlgsMgr()->GetModifying())
	{
		if( m_pCurModifyingItem != NULL)
		{
			m_pTreeAllRes->SelectItem(m_pCurModifyingItem);
		}

		return;
	}

	
	P_AUITREEVIEW_ITEM pItem = m_pTreeAllRes->GetSelectedItem();

	//没点到Item
	if( pItem == NULL)
	{
		

		GetHomeDlgsMgr()->SetModifying(false);
		SetBtnsEnable(false, false, false);
		return;	
	}

	//点到父Item
	for( int i = 0; i < ARTI_NUM; i++)
	{
		if( pItem == m_pTreeParentItem[i])
		{
			

			GetHomeDlgsMgr()->SetModifying(false);	
			SetBtnsEnable(false, false, false);
			return;
		}
	}


	//设置当前观察区域的点
	SetCurSelAreaViewPoints(pItem);

	GetHomeDlgsMgr()->SetModifying(false);
	SetBtnsEnable(true, false, true);

	return;
}

int CDlgAutoAllRes::GetParentItemID(P_AUITREEVIEW_ITEM i_pItem)
{
	ASSERT(i_pItem != NULL);

	P_AUITREEVIEW_ITEM pParentItem = m_pTreeAllRes->GetParentItem(i_pItem);
	
	int i = 0;
	for( ; i < ARTI_NUM; i++)
	{
		if( pParentItem == m_pTreeParentItem[i])
		{
			break;
		}
	}

	if( i == ARTI_NUM)
	{
		return -1; //没找到
	}

	return i;
}


void CDlgAutoAllRes::DelResTreeLastChildItem(AllResTreeItem_e i_Parent)
{
	if( m_pTreeParentItem[i_Parent] == NULL)
		return;

	P_AUITREEVIEW_ITEM pItem = m_pTreeAllRes->GetFirstChildItem(m_pTreeParentItem[i_Parent]);
	if(pItem == NULL)
		return;
	P_AUITREEVIEW_ITEM pNext = pItem;
	while(pNext != NULL)
	{
		pNext = m_pTreeAllRes->GetNextSiblingItem(pItem);
		if(pNext != NULL)
		{
			pItem = pNext;
		}
	}

	m_pTreeAllRes->DeleteItem(pItem);

	return;

}

void CDlgAutoAllRes::SetCurSelAreaViewPoints(P_AUITREEVIEW_ITEM i_pItem)
{
	if( i_pItem == NULL)
		return;
	
	//如果是父item,返回
	if( m_pTreeAllRes->GetFirstChildItem(i_pItem) != NULL)
		return;

	P_AUITREEVIEW_ITEM pParent = m_pTreeAllRes->GetParentItem(i_pItem);
	unsigned int uType = m_pTreeAllRes->GetItemData(pParent);

	CAutoSelectedArea* pSelArea = GetAutoScene()->GetAutoSelectedArea();
	int nNumPoints = 0;
	POINT_FLOAT* pPoints = NULL;
	int nIdx = GetTreeViewItemIdx(i_pItem);
	if( nIdx == -1)
	{
		ASSERT(0);
	}

	CAutoTerrainAction* pAction = GetAutoScene()->GetAutoTerrainAction();
	CAutoBuildingOperation* pOpera = GetAutoScene()->GetAutoBuildingOperation();
	switch(uType)
	{
	case ARTI_TERRAIN:
		break;
	case ARTI_WATER:
		break;

	case ARTI_ROAD:
		{	pAction->GetAreaPointsInAction(TAT_ROAD, nIdx, &nNumPoints, &pPoints);
			pSelArea->SetViewPoints(nNumPoints, pPoints);
			pSelArea->SetViewAreaClosed(false);

			pOpera->SetSelectedModelIndex(-1);
		}
		break;

	case ARTI_MOUNTAIN_LINE:
		{	
			pAction->GetAreaPointsInAction(TAT_HILL_LINE, nIdx, &nNumPoints, &pPoints);
			pSelArea->SetViewPoints(nNumPoints, pPoints);
			pSelArea->SetViewAreaClosed(false);

			pOpera->SetSelectedModelIndex(-1);
		}
		break;
	case ARTI_MOUNTAIN_POLY:
		{	
			pAction->GetAreaPointsInAction(TAT_HILL_POLY, nIdx, &nNumPoints, &pPoints);
			pSelArea->SetViewPoints(nNumPoints, pPoints);
			pSelArea->SetViewAreaClosed(true);

			pOpera->SetSelectedModelIndex(-1);
		}
		break;
	case ARTI_LOCALTEX:
		{	
			pAction->GetAreaPointsInAction(TAT_TEXTURE_POLY, nIdx, &nNumPoints, &pPoints);
			pSelArea->SetViewPoints(nNumPoints, pPoints);
			pSelArea->SetViewAreaClosed(true);

			pOpera->SetSelectedModelIndex(-1);
		}
		break;
	case ARTI_TREE_LINE:
		{	
			pAction->GetAreaPointsInAction(TAT_FOREST_LINE, nIdx, &nNumPoints, &pPoints);
			pSelArea->SetViewPoints(nNumPoints, pPoints);
			pSelArea->SetViewAreaClosed(false);

			pOpera->SetSelectedModelIndex(-1);
		}
		break;
	case ARTI_TREE_POLY:
		{	
			pAction->GetAreaPointsInAction(TAT_FOREST_POLY, nIdx, &nNumPoints, &pPoints);
			pSelArea->SetViewPoints(nNumPoints, pPoints);
			pSelArea->SetViewAreaClosed(true);

			pOpera->SetSelectedModelIndex(-1);
		}
		break;
	case ARTI_GRASS:
		{	
			pAction->GetAreaPointsInAction(TAT_GRASS_POLY, nIdx, &nNumPoints, &pPoints);
			pSelArea->SetViewPoints(nNumPoints, pPoints);
			pSelArea->SetViewAreaClosed(true);

			pOpera->SetSelectedModelIndex(-1);
		}
		break;

	case ARTI_BUILDING:
	case ARTI_OTHEROBJECT:
	case ARTI_FURNITURE:
	case ARTI_PLANT:
	case ARTI_TOY:
	case ARTI_VIRTU:
		{
			
			int nID = m_pTreeAllRes->GetItemData(i_pItem);
			int nIdx = GetAutoScene()->GetAutoBuilding()->GetIndexByID(nID);
			pOpera->SetSelectedModelIndex(nIdx);
			GetAutoScene()->GetAutoBuildingOperation()->SelectContainedModels();
			
			pSelArea->DeleteAllViewPoints();

			GetHomeDlgsMgr()->SetModelState(CECHomeDlgsMgr::MSE_HIT);
		}
		break;

	default:
		break;
	}
}

void CDlgAutoAllRes::TurnToMountainPolyMode()
{
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBScene("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseBaseMountain");
	GetHomeDlgsMgr()->SetModifying(true);
	GetHomeDlgsMgr()->m_pDlgAutoBaseMountain->SetBtnCreateToModify();
	GetHomeDlgsMgr()->m_pDlgAutoMountain->SetBtnCreateToModify();

	ViewAreaToEditArea(false);

	return;
}
void CDlgAutoAllRes::TurnToMountainLineMode()
{
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBScene("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseBaseMountain2");
	GetHomeDlgsMgr()->SetModifying(true);
	GetHomeDlgsMgr()->m_pDlgAutoBaseMountain2->SetBtnCreateToModify();
	GetHomeDlgsMgr()->m_pDlgAutoMountain2->SetBtnCreateToModify();

	ViewAreaToEditArea(true);

	return;

}
void CDlgAutoAllRes::TurnToRoadMode()
{
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBScene("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseBaseRoad");
	GetHomeDlgsMgr()->SetModifying(true);
	GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->SetBtnCreateToModify();
	GetHomeDlgsMgr()->m_pDlgAutoRoad->SetBtnCreateToModify();

	ViewAreaToEditArea(true);

	return;
}
void CDlgAutoAllRes::TurnToLocalTexMode()
{
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBScene("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseBaseLocalTex");
	GetHomeDlgsMgr()->SetModifying(true);
	GetHomeDlgsMgr()->m_pDlgAutoBaseLocalTex->SetBtnCreateToModify();
	GetHomeDlgsMgr()->m_pDlgAutoLocalTexture->SetBtnCreateToModify();

	ViewAreaToEditArea(false);

	return;
}
void CDlgAutoAllRes::TurnToTreePolyMode()
{
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBOutdoor("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseOutdoorTree");
	GetHomeDlgsMgr()->SetModifying(true);
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorTree->SetBtnCreateToModify();

	GetHomeDlgsMgr()->m_pDlgAutoOutdoorTree->SetPolyAndLineBtnPush(true, false);
	GetHomeDlgsMgr()->m_pDlgAutoNote2->Show(true);
	GetHomeDlgsMgr()->m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_POLY);
	GetAutoScene()->SetOperationType(OT_TREE_POLY);

	ViewAreaToEditArea(false);

	return;
}
void CDlgAutoAllRes::TurnToTreeLineMode()
{
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBOutdoor("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseOutdoorTree");
	SetOperationType(OT_TREE_LINE);
	GetHomeDlgsMgr()->SetModifying(true);
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorTree->SetBtnCreateToModify();

	GetHomeDlgsMgr()->m_pDlgAutoOutdoorTree->SetPolyAndLineBtnPush(false, true);
	GetHomeDlgsMgr()->m_pDlgAutoNote2->Show(true);
	GetHomeDlgsMgr()->m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_LINE);
	GetAutoScene()->SetOperationType(OT_TREE_LINE);

	ViewAreaToEditArea(true);

	return;
}
void CDlgAutoAllRes::TurnToGrassMode()
{
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBOutdoor("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseOutdoorGrass");
	GetHomeDlgsMgr()->SetModifying(true);
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorGrass->SetBtnCreateToModify();

	ViewAreaToEditArea(false);
	
	return;
}

int CDlgAutoAllRes::GetTreeViewSelItemIdx() const
{
	P_AUITREEVIEW_ITEM pItem = m_pTreeAllRes->GetSelectedItem();
	return GetTreeViewItemIdx(pItem);
}

void CDlgAutoAllRes::OnCommandCancelRes(const char* szCommand)
{
	GetHomeDlgsMgr()->SetModifying(false);

	if( GetHomeDlgsMgr()->GetModelState() == CECHomeDlgsMgr::MSE_MODIFY)
	{
		GetHomeDlgsMgr()->SetModelState(CECHomeDlgsMgr::MSE_HIT);
	}
	else
	{
		EditAreaToViewArea();
	}
	
	
	GetHomeDlgsMgr()->m_pDlgAutoBaseMountain->SetBtnModifyToCreate();
	GetHomeDlgsMgr()->m_pDlgAutoMountain->SetBtnModifyToCreate();
	GetHomeDlgsMgr()->m_pDlgAutoBaseMountain2->SetBtnModifyToCreate();
	GetHomeDlgsMgr()->m_pDlgAutoMountain2->SetBtnModifyToCreate();
	GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->SetBtnModifyToCreate();
	GetHomeDlgsMgr()->m_pDlgAutoRoad->SetBtnModifyToCreate();
	GetHomeDlgsMgr()->m_pDlgAutoBaseLocalTex->SetBtnModifyToCreate();
	GetHomeDlgsMgr()->m_pDlgAutoLocalTexture->SetBtnModifyToCreate();
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorTree->SetBtnModifyToCreate();
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorGrass->SetBtnModifyToCreate();

	SetBtnsEnable(true, false, true);

	//取消重新定位
	GetAutoScene()->GetAutoBuildingOperation()->SetBuildingOpera(CAutoBuildingOperation::BP_NONE);
}

void CDlgAutoAllRes::EditAreaToViewArea()
{
	CAutoSelectedArea* pSelArea = GetAutoScene()->GetAutoSelectedArea();
	if( pSelArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
	{
		bool bClosed = pSelArea->GetPolyEditAreaClosed();
		pSelArea->CopyPointsFromPolyEditToView();
		pSelArea->SetViewAreaClosed(bClosed);
		pSelArea->DeleteAllPolyEditPoints();
	}
	else if( pSelArea->GetAreatype() == CAutoSelectedArea::AT_LINE)
	{
		pSelArea->CopyPointsFromLineEditToView();
		pSelArea->SetViewAreaClosed(false);
		pSelArea->DeleteAllLineEditPoints();
	}
}

void CDlgAutoAllRes::ViewAreaToEditArea(bool i_bLine)
{
	CAutoSelectedArea* pSelArea = GetAutoScene()->GetAutoSelectedArea();
	if( i_bLine)
	{
		pSelArea->CopyPointsFromViewToLineEdit();
		pSelArea->DeleteAllViewPoints();
		pSelArea->SetAreaType(CAutoSelectedArea::AT_LINE);
	}
	else
	{
		bool bClosed = pSelArea->GetViewAreaClosed();
		pSelArea->CopyPointsFromViewToPolyEdit();
		pSelArea->SetPolyEditAreaClosed(bClosed);
		pSelArea->DeleteAllViewPoints();
		pSelArea->SetAreaType(CAutoSelectedArea::AT_POLY);
	}
}

void CDlgAutoAllRes::SetBtnsEnable(bool i_bModify, bool i_bCancel, bool i_bDel)
{
	m_pBtn[B_MODIFY]->Enable(i_bModify);
	m_pBtn[B_CANCEL]->Enable(i_bCancel);
	m_pBtn[B_DEL]->Enable(i_bDel);
}

void CDlgAutoAllRes::SetModelOperation(P_AUITREEVIEW_ITEM i_pItem)
{
	CAutoBuildingOperation* pOpera = GetAutoScene()->GetAutoBuildingOperation();
	int nID = m_pTreeAllRes->GetItemData(i_pItem);
	int nIdx = GetAutoScene()->GetAutoBuilding()->GetIndexByID(nID);
	pOpera->SetSelectedModelIndex(nIdx);
	
	
	GetHomeDlgsMgr()->SetModelState(CECHomeDlgsMgr::MSE_MODIFY);

	//设置重新定位
	GetAutoScene()->GetAutoBuildingOperation()->SetBuildingOpera(CAutoBuildingOperation::BP_REPOS);
}

void CDlgAutoAllRes::TurnToBuildingMode()
{
	
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBOutdoor("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseOutdoorBuilding");
	GetHomeDlgsMgr()->SetModifying(true);

	GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(true);
	
}

void CDlgAutoAllRes::TurnToOtherObjectMode()
{
	
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBOutdoor("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseOutdoorOtherObject");
		
	GetHomeDlgsMgr()->SetModifying(true);

	GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(true);
}

void CDlgAutoAllRes::TurnToFurnitureMode()
{
	
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBIndoor("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseIndoorFurniture");
	GetHomeDlgsMgr()->SetModifying(true);
	
	GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(true);

}

void CDlgAutoAllRes::TurnToPlantMode()
{
	
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBIndoor("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseIndoorPlant");

	GetHomeDlgsMgr()->SetModifying(true);
	GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(true);
}

void CDlgAutoAllRes::TurnToToyMode()
{
	
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBIndoor("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseIndoorToy");
	GetHomeDlgsMgr()->SetModifying(true);

	GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(true);
}

void CDlgAutoAllRes::TurnToVirtuMode()
{
	
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->OnCommandOnSBIndoor("");
	GetHomeDlgsMgr()->SwitchDialog("ChooseIndoorVirtu");
	GetHomeDlgsMgr()->SetModifying(true);
	
	GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(true);
}

void CDlgAutoAllRes::InitVars()
{
	for( int i = 0; i < ARTI_NUM; i++)
	{
		m_pTreeParentItem[i] = NULL;
	}
	m_pCurModifyingItem = NULL;
}

void CDlgAutoAllRes::MakeSureTexOpEnable(void)
{
	//路
	if( m_pTreeParentItem[ARTI_ROAD] != NULL)
	{
		GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->SetTexOpEnable(true);
	}
	else
	{
		GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->SetTexOpEnable(false);
	}

	//局部纹理
	if( m_pTreeParentItem[ARTI_LOCALTEX] != NULL)
	{
		GetHomeDlgsMgr()->m_pDlgAutoBaseLocalTex->SetTexOpEnable(true);

	}
	else
	{
		GetHomeDlgsMgr()->m_pDlgAutoBaseLocalTex->SetTexOpEnable(false);
	}

	//多边形山
	if( m_pTreeParentItem[ARTI_MOUNTAIN_POLY] != NULL)
	{
		GetHomeDlgsMgr()->m_pDlgAutoBaseMountain->SetTexOpEnable(true);

	}
	else
	{
		GetHomeDlgsMgr()->m_pDlgAutoBaseMountain->SetTexOpEnable(false);
	}

	//线性山
	if( m_pTreeParentItem[ARTI_MOUNTAIN_LINE] != NULL)
	{
			GetHomeDlgsMgr()->m_pDlgAutoBaseMountain2->SetTexOpEnable(true);
	}
	else
	{
		GetHomeDlgsMgr()->m_pDlgAutoBaseMountain2->SetTexOpEnable(false);
	}
}

void CDlgAutoAllRes::ReLoadSceneRes()
{
	Clear();
	DoDataExchange(true);
	OnInitContext();	

	ResCount_t resCount;

	//与建筑相关
	CAutoBuilding* pBuilding = GetAutoScene()->GetAutoBuilding();
	int nNumBuilding = pBuilding->GetNumBuildings();

	int i;
	for(  i = 0; i < nNumBuilding; i++)
	{
		CAutoBuilding::MODELSTATUS* pModelStatus =pBuilding->GetModelStatusByIndex(i);
		InsertModelItem(pModelStatus->dwUniqueID, pModelStatus->dwAttribute, resCount);
	}

	//与地形相关
	CAutoTerrainAction *pAction = GetAutoScene()->GetAutoTerrainAction();
	
	//多边形山
	for( i = 0; i < pAction->GetNumActionHillPoly(); i++)
	{
		InsertTerrainItem(ARTI_MOUNTAIN_POLY, resCount);
	}
	//线形山
	for( i = 0; i < pAction->GetNumActionHillLine(); i++)
	{
		InsertTerrainItem(ARTI_MOUNTAIN_LINE, resCount);

	}
	//局部纹理
	for( i = 0; i < pAction->GetNumActionPartTexture(); i++)
	{
		InsertTerrainItem(ARTI_LOCALTEX, resCount);

	}
	//路
	for( i = 0; i < pAction->GetNumActionRoad(); i++)
	{
		InsertTerrainItem(ARTI_ROAD, resCount);
	}

	//树草
	for( i = 0; i < pAction->GetNumActionForestPoly(); i++)
	{
		InsertTerrainItem(ARTI_TREE_POLY, resCount);

	}
	for( i = 0; i < pAction->GetNumActionForestLine(); i++)
	{
		InsertTerrainItem(ARTI_TREE_LINE, resCount);

	}
	
	//草
	for( i = 0; i < pAction->GetNumActionGrassPoly(); i++)
	{
		InsertTerrainItem(ARTI_GRASS, resCount);
	}

	//设置个对话框的操作计数
	SetOperationCount(resCount);
	return;
}

void CDlgAutoAllRes::InsertModelItem(unsigned int i_uModelID, unsigned int i_uType, ResCount_t& i_ResCount)
{
	ACHAR szName[MAX_PATH];

	switch(i_uType)
	{
	case CAutoBuilding::MA_HOUSE:
		{
			a_sprintf(szName, _AL("%s%d"), GetStringFromTable(1009), i_ResCount.nCntBuilding);
			InsertResTreeChildItem(ARTI_BUILDING, szName, i_uModelID);
			i_ResCount.nCntBuilding++;
		}
		break;

	case CAutoBuilding::MA_OUTDOOR:
		{
			a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1010), i_ResCount.nCntOthObj);
			InsertResTreeChildItem(ARTI_OTHEROBJECT, szName, i_uModelID);
			i_ResCount.nCntOthObj++;
		}
		break;

	case CAutoBuilding::MA_FURNITURE:
		{
 			a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1011), i_ResCount.nCntFurniture);
			InsertResTreeChildItem(ARTI_FURNITURE, szName, i_uModelID);
			i_ResCount.nCntFurniture++;
		}
		break;

	case CAutoBuilding::MA_PLANT:
		{
			a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1012), i_ResCount.nCntPlant);
			InsertResTreeChildItem(ARTI_PLANT, szName, i_uModelID);
			i_ResCount.nCntPlant++;
		}
		break;

	case CAutoBuilding::MA_TOY:
		{
			a_sprintf( szName, _AL("%s%d"),GetStringFromTable(1013), i_ResCount.nCntToy);
			InsertResTreeChildItem(ARTI_TOY, szName, i_uModelID);
			i_ResCount.nCntToy++;
		}
		break;

	case CAutoBuilding::MA_VIRTU:
		{
			a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1014), i_ResCount.nCntVirtu);
			InsertResTreeChildItem(ARTI_VIRTU, szName, i_uModelID);
			i_ResCount.nCntVirtu++;
		}
		break;

	default:
		break;
	}

	return;
					
}

void CDlgAutoAllRes::InsertTerrainItem(AllResTreeItem_e i_itemType, ResCount_t &i_ResCount)
{
	ACHAR szName[MAX_PATH];
	
	switch(i_itemType)
	{
	case ARTI_MOUNTAIN_POLY:
		{
			a_sprintf(szName, _AL("山丘%d"), i_ResCount.nCntMountainPoly);
			InsertResTreeChildItem(i_itemType, szName);
			i_ResCount.nCntMountainPoly++;
		}
		break;
	case ARTI_MOUNTAIN_LINE:
		{
			a_sprintf(szName, _AL("%s%d"), GetStringFromTable(1002), i_ResCount.nCntMountainLine);
			InsertResTreeChildItem(i_itemType, szName);
			i_ResCount.nCntMountainLine++;
		}
		break;
	case ARTI_ROAD:
		{
			a_sprintf(szName, _AL("%s%d"),GetStringFromTable(1003), i_ResCount.nCntRoad);
			InsertResTreeChildItem(i_itemType, szName);
			i_ResCount.nCntRoad++;
		}
		break;
	case ARTI_LOCALTEX:
		{
			a_sprintf(szName, _AL("%s%d"),GetStringFromTable(1005), i_ResCount.nCntLocalTex);
			InsertResTreeChildItem(i_itemType, szName);
			i_ResCount.nCntLocalTex++;
		}
		break;
	case ARTI_TREE_POLY:
		{
			a_sprintf(szName, _AL("%s%d"), GetStringFromTable(1006), i_ResCount.nCntTreePoly);
			InsertResTreeChildItem(i_itemType, szName);
			i_ResCount.nCntTreePoly++;
		}
		break;
	case ARTI_TREE_LINE:
		{
			a_sprintf(szName, _AL("%s%d"), GetStringFromTable(1007), i_ResCount.nCntTreeLine);
			InsertResTreeChildItem(i_itemType, szName);
			i_ResCount.nCntTreeLine++;
		}
		break;
	case ARTI_GRASS:
		{
			a_sprintf(szName, _AL("%s%d"), GetStringFromTable(1008), i_ResCount.nCntGrass);
			InsertResTreeChildItem(i_itemType, szName);
			i_ResCount.nCntGrass++;
		}
		break;
	default:
		break;
	}
}

void CDlgAutoAllRes::SetOperationCount(const ResCount_t &i_ResCount)
{
	//建筑相关
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorBuilding->SetActionCount(i_ResCount.nCntBuilding);
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorOtherObject->SetActionCount(i_ResCount.nCntOthObj);

	GetHomeDlgsMgr()->m_pDlgAutoIndoorFurniture->SetActionCount(i_ResCount.nCntFurniture);
	GetHomeDlgsMgr()->m_pDlgAutoIndoorPlant->SetActionCount(i_ResCount.nCntPlant);
	GetHomeDlgsMgr()->m_pDlgAutoIndoorToy->SetActionCount(i_ResCount.nCntToy);
	GetHomeDlgsMgr()->m_pDlgAutoIndoorVirtu->SetActionCount(i_ResCount.nCntVirtu);

	//地形相关
	GetHomeDlgsMgr()->m_pDlgAutoBaseMountain->SetActionCount(i_ResCount.nCntMountainPoly);
	GetHomeDlgsMgr()->m_pDlgAutoBaseMountain2->SetActionCount(i_ResCount.nCntMountainLine);
	GetHomeDlgsMgr()->m_pDlgAutoBaseLocalTex->SetActionCount(i_ResCount.nCntLocalTex);
	GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->SetActionCount(i_ResCount.nCntRoad);

	//树草
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorGrass->SetActionCount(i_ResCount.nCntGrass);
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorTree->SetActionCount(i_ResCount.nCntTreePoly);
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorTree->SetActionCount2(i_ResCount.nCntTreeLine);
}
*/
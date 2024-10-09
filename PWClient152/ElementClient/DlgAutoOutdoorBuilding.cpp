/*
 * FILE: DlgAutoOutdoorTree.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoOutdoorBuilding.h"
#include "DlgAutoAllRes.h"
#include "DlgAutoSwitchDlg.h"

#include "EC_HomeDlgsMgr.h"

#include "aui/AUIManager.h"
#include "aui/AUIListBox.h"

#include "A3DMacros.h"

#include "AutoBuildingOperation.h"
#include "AutoBuilding.h"
#include "AutoTerrainCommon.h"
#include "AutoTerrainAction.h"


AUI_BEGIN_COMMAND_MAP(CDlgAutoOutdoorBuilding, CDlgHomeBase)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoOutdoorBuilding, CDlgHomeBase)

AUI_ON_EVENT("ListBox_Buildings", WM_LBUTTONUP, OnEventLBtnUpBuildingList)

AUI_END_EVENT_MAP()

CDlgAutoOutdoorBuilding::CDlgAutoOutdoorBuilding()
{
	m_pListBoxBuildings = NULL; 
	m_nBuildingIdx = NULL;

	m_pModelInfos = NULL;
	m_nNumModelInfos = NULL;

}

CDlgAutoOutdoorBuilding::~CDlgAutoOutdoorBuilding()
{
}

AUIStillImageButton * CDlgAutoOutdoorBuilding::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnOutdoorBuilding;
}

void CDlgAutoOutdoorBuilding::OnShowDialog()
{
	this->SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);

}

bool CDlgAutoOutdoorBuilding::OnInitContext()
{
	if(!InitResource())
		return false;

	LoadDefault();
	UpdateData(false);

	return true;
}

void CDlgAutoOutdoorBuilding::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_Buildings", m_pListBoxBuildings);
	DDX_ListBox(bSave, "ListBox_Buildings", m_nBuildingIdx);
}

void CDlgAutoOutdoorBuilding::LoadDefault()
{
	m_nBuildingIdx = m_pListBoxBuildings->GetCount();
}

bool CDlgAutoOutdoorBuilding::InitResource()
{

	if( !FillBuildingListBox())
		return false;
	
	return true;
}

bool CDlgAutoOutdoorBuilding::FillBuildingListBox()
{
	CAutoScene* pScene = GetAutoScene();
	if(!pScene)
		return false;

	CAutoBuilding* pBuilding = pScene->GetAutoBuilding();

	if( !pBuilding)
		return false;

	m_pModelInfos = pBuilding->GetModelInfos();
	m_nNumModelInfos = pBuilding->GetNumModelInfos();

	m_pListBoxBuildings->ResetContent();

	int nIdx = 0;
	for( int i = 0; i < m_nNumModelInfos; i ++)
	{
		if(m_pModelInfos[i].dwAttribute == CAutoBuilding::MA_HOUSE)
		{
			m_pListBoxBuildings->InsertString(nIdx, AS2AC(m_pModelInfos[i].strName));
			m_pListBoxBuildings->SetItemData(nIdx, m_pModelInfos[i].dwID);
			
			nIdx++;
		}
	}
	return true;
}


void CDlgAutoOutdoorBuilding::AddAllResTreeViewItem(unsigned int uUiqueID)
{
	ACHAR szName[100];
	a_sprintf( szName, _AL("%s%d"),GetStringFromTable(1001), m_nActionCount);
	m_nActionCount++;
	
	GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_BUILDING, szName, uUiqueID);

	InValidateListIdx();
	
	return;
}

void CDlgAutoOutdoorBuilding::OnEventLBtnUpBuildingList(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( GetHomeDlgsMgr()->GetModelState() == CECHomeDlgsMgr::MSE_MODIFY)
	{
		InValidateListIdx();
		return;
	}
	
	UpdateData(true);

	if (GetOperationType() != OT_BUILDING)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

		return;
	}
	
	CAutoScene* pScene = GetAutoScene();

	
	CAutoBuilding* pBuilding = pScene->GetAutoBuilding();
	CAutoBuildingOperation* pBuildingOpera = pScene->GetAutoBuildingOperation();
	pBuildingOpera->SetSelectedModelIndex(-1);
	// Clear contained models
	pBuildingOpera->SelectContainedModels();

	DWORD dwID;

	//m_nBuildingIdx = Clamp(m_nBuildingIdx, 0, m_pListBoxBuildings->GetCount());
	dwID = m_pListBoxBuildings->GetItemData(m_nBuildingIdx);

	pBuildingOpera->SetBuildingOpera(CAutoBuildingOperation::BP_ADD);
	pBuildingOpera->SetCurModelID(dwID);
	
	GetHomeDlgsMgr()->SetModelState(CECHomeDlgsMgr::MSE_CREATE);

	return;
}

void CDlgAutoOutdoorBuilding::InValidateListIdx()
{
	m_nBuildingIdx = m_pListBoxBuildings->GetCount();
	UpdateData(false);
	return;
}
*/
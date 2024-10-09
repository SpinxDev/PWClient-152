/*
 * FILE: DlgAutoIndoorFurniture.cpp
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
#include "DlgAutoIndoorFurniture.h"
#include "DlgAutoAllRes.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoSwitchDlg.h"

#include "aui/AUIManager.h"
#include "aui/AUIListBox.h"

#include "A3DMacros.h"

#include "AutoBuildingOperation.h"
#include "AutoTerrainCommon.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoIndoorFurniture, CDlgHomeBase)


AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoIndoorFurniture, CDlgHomeBase)

AUI_ON_EVENT("ListBox_InDoorFurniture", WM_LBUTTONUP, OnEventLBtnUpListFuniture)

AUI_END_EVENT_MAP()

CDlgAutoIndoorFurniture::CDlgAutoIndoorFurniture()
{
	m_pListBoxFurniture = NULL;
	m_nFurnitureIdx = 0;

	m_pModelInfos = NULL;
	m_nNumModelInfos = 0;
}

CDlgAutoIndoorFurniture::~CDlgAutoIndoorFurniture()
{
}

AUIStillImageButton * CDlgAutoIndoorFurniture::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnIndoorFurniture;
}

void CDlgAutoIndoorFurniture::OnShowDialog()
{
	this->SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoIndoorFurniture::OnInitContext()
{
	if(!InitResource())
		return false;

	LoadDefault();
	UpdateData(false);

	return true;
}

void CDlgAutoIndoorFurniture::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_InDoorFurniture", m_pListBoxFurniture);
	DDX_ListBox(bSave, "ListBox_InDoorFurniture", m_nFurnitureIdx);
}

void CDlgAutoIndoorFurniture::LoadDefault()
{
	m_nFurnitureIdx = m_pListBoxFurniture->GetCount();
}

bool CDlgAutoIndoorFurniture::InitResource()
{

	if( !FillListBox())
		return false;
	
	return true;
}

bool CDlgAutoIndoorFurniture::FillListBox()
{
	CAutoScene* pScene = GetAutoScene();
	if(!pScene)
		return false;

	CAutoBuilding* pBuilding = pScene->GetAutoBuilding();

	if( !pBuilding)
		return false;

	m_pModelInfos = pBuilding->GetModelInfos();
	m_nNumModelInfos = pBuilding->GetNumModelInfos();

	m_pListBoxFurniture->ResetContent();

	int nIdx = 0;
	for( int i = 0; i < m_nNumModelInfos; i ++)
	{
		if(m_pModelInfos[i].dwAttribute == CAutoBuilding::MA_FURNITURE)
		{
			m_pListBoxFurniture->InsertString(nIdx, AS2AC(m_pModelInfos[i].strName));
			m_pListBoxFurniture->SetItemData(nIdx, m_pModelInfos[i].dwID);
			nIdx++;
		}
	}
	return true;
}


void CDlgAutoIndoorFurniture::AddAllResTreeViewItem(unsigned int uUiqueID)
{
	ACHAR szName[100];
	a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1001), m_nActionCount);
	m_nActionCount++;
	
	GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_FURNITURE, szName, uUiqueID);

	InValidateListIdx();
	return;
}

void CDlgAutoIndoorFurniture::OnEventLBtnUpListFuniture(WPARAM wParam, LPARAM lParam, AUIObject* pObject)
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
	dwID = m_pListBoxFurniture->GetItemData(m_nFurnitureIdx);

	pBuildingOpera->SetBuildingOpera(CAutoBuildingOperation::BP_ADD);
	pBuildingOpera->SetCurModelID(dwID);

	GetHomeDlgsMgr()->SetModelState(CECHomeDlgsMgr::MSE_CREATE);
}

void CDlgAutoIndoorFurniture::InValidateListIdx()
{
	m_nFurnitureIdx = m_pListBoxFurniture->GetCount();
	UpdateData(false);
	return;
}
*/
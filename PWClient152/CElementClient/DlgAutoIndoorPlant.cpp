/*
 * FILE: DlgAutoIndoorPlant.cpp
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
#include "DlgAutoIndoorPlant.h"
#include "DlgAutoAllRes.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoSwitchDlg.h"

#include "aui/AUIManager.h"
#include "aui/AUIListBox.h"

#include "A3DMacros.h"

#include "AutoBuildingOperation.h"
#include "AutoTerrainCommon.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoIndoorPlant, CDlgHomeBase)


AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoIndoorPlant, CDlgHomeBase)

AUI_ON_EVENT("ListBox_InDoorPlant", WM_LBUTTONUP, OnEventLBtnUpListPlant)

AUI_END_EVENT_MAP()

CDlgAutoIndoorPlant::CDlgAutoIndoorPlant()
{
	m_pListBoxPlant = NULL;
	m_nPlantIdx = 0 ;

	m_pModelInfos = NULL;
	m_nNumModelInfos = 0;
}

CDlgAutoIndoorPlant::~CDlgAutoIndoorPlant()
{
}

AUIStillImageButton * CDlgAutoIndoorPlant::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnIndoorPlant;
}

void CDlgAutoIndoorPlant::OnShowDialog()
{
	this->SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoIndoorPlant::OnInitContext()
{
	if(!InitResource())
		return false;

	LoadDefault();
	UpdateData(false);

	return true;
}

void CDlgAutoIndoorPlant::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_InDoorPlant", m_pListBoxPlant);
	DDX_ListBox(bSave, "ListBox_InDoorPlant", m_nPlantIdx);
}

void CDlgAutoIndoorPlant::LoadDefault()
{
	m_nPlantIdx = m_pListBoxPlant->GetCount();
}

bool CDlgAutoIndoorPlant::InitResource()
{

	if( !FillListBox())
		return false;
	
	return true;
}

bool CDlgAutoIndoorPlant::FillListBox()
{
	CAutoScene* pScene = GetAutoScene();
	if(!pScene)
		return false;

	CAutoBuilding* pBuilding = pScene->GetAutoBuilding();

	if( !pBuilding)
		return false;

	m_pModelInfos = pBuilding->GetModelInfos();
	m_nNumModelInfos = pBuilding->GetNumModelInfos();

	m_pListBoxPlant->ResetContent();

	int nIdx = 0;
	for( int i = 0; i < m_nNumModelInfos; i ++)
	{
// 		if(m_pModelInfos[i].dwAttribute == CAutoBuilding::MA_HOUSE)
// 		{
			m_pListBoxPlant->InsertString(nIdx, AS2AC(m_pModelInfos[i].strName));
			m_pListBoxPlant->SetItemData(nIdx, m_pModelInfos[i].dwID);
			nIdx++;
// 		}
	}
	return true;
}



void CDlgAutoIndoorPlant::AddAllResTreeViewItem(unsigned int uUiqueID)
{
	ACHAR szName[100];
	a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1001), m_nActionCount);
	m_nActionCount++;
	
	GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_PLANT, szName, uUiqueID);

	InValidateListIdx();
	return;
}

void CDlgAutoIndoorPlant::OnEventLBtnUpListPlant(WPARAM wParam, LPARAM lParam, AUIObject* pObject)
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
	dwID = m_pListBoxPlant->GetItemData(m_nPlantIdx);

	pBuildingOpera->SetBuildingOpera(CAutoBuildingOperation::BP_ADD);
	pBuildingOpera->SetCurModelID(dwID);

	GetHomeDlgsMgr()->SetModelState(CECHomeDlgsMgr::MSE_CREATE);
}

void CDlgAutoIndoorPlant::InValidateListIdx()
{
	m_nPlantIdx = m_pListBoxPlant->GetCount();
	UpdateData(false);
	return;
}
*/
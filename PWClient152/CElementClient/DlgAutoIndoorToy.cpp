/*
 * FILE: DlgAutoIndoorToy.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoIndoorToy.h"
#include "DlgAutoAllRes.h"
#include "DlgAutoSwitchDlg.h"

#include "EC_HomeDlgsMgr.h"

#include "aui/AUIManager.h"
#include "aui/AUIListBox.h"

#include "A3DMacros.h"

#include "AutoBuildingOperation.h"
#include "AutoTerrainCommon.h"


AUI_BEGIN_COMMAND_MAP(CDlgAutoIndoorToy, CDlgHomeBase)


AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoIndoorToy, CDlgHomeBase)

AUI_ON_EVENT("ListBox_InDoorToy", WM_LBUTTONUP, OnEventLBtnUpListToy)

AUI_END_EVENT_MAP()

CDlgAutoIndoorToy::CDlgAutoIndoorToy()
{
	m_pListBoxToy = NULL;
	m_nToyIdx = 0 ;

	m_pModelInfos = NULL;
	m_nNumModelInfos = 0;
}

CDlgAutoIndoorToy::~CDlgAutoIndoorToy()
{
}

AUIStillImageButton * CDlgAutoIndoorToy::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnIndoorToy;
}

void CDlgAutoIndoorToy::OnShowDialog()
{
	this->SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoIndoorToy::OnInitContext()
{
	if(!InitResource())
		return false;

	LoadDefault();
	UpdateData(false);

	return true;
}

void CDlgAutoIndoorToy::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_InDoorToy", m_pListBoxToy);
	DDX_ListBox(bSave, "ListBox_InDoorToy", m_nToyIdx);
}

void CDlgAutoIndoorToy::LoadDefault()
{
	m_nToyIdx = m_pListBoxToy->GetCount();
}

bool CDlgAutoIndoorToy::InitResource()
{

	if( !FillListBox())
		return false;
	
	return true;
}

bool CDlgAutoIndoorToy::FillListBox()
{
	CAutoScene* pScene = GetAutoScene();
	if(!pScene)
		return false;

	CAutoBuilding* pBuilding = pScene->GetAutoBuilding();

	if( !pBuilding)
		return false;

	m_pModelInfos = pBuilding->GetModelInfos();
	m_nNumModelInfos = pBuilding->GetNumModelInfos();

	m_pListBoxToy->ResetContent();

	int nIdx = 0;
	for( int i = 0; i < m_nNumModelInfos; i ++)
	{
// 		if(m_pModelInfos[i].dwAttribute == CAutoBuilding::MA_HOUSE)
// 		{
			m_pListBoxToy->InsertString(nIdx, AS2AC(m_pModelInfos[i].strName));
			m_pListBoxToy->SetItemData(nIdx, m_pModelInfos[i].dwID);
			nIdx++;
// 		}
	}
	return true;
}



void CDlgAutoIndoorToy::AddAllResTreeViewItem(unsigned int uUiqueID)
{
	ACHAR szName[100];
	a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1001), m_nActionCount);
	m_nActionCount++;
	
	GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_TOY, szName, uUiqueID);


	InValidateListIdx();
	return;
}

void CDlgAutoIndoorToy::OnEventLBtnUpListToy(WPARAM wParam, LPARAM lParam, AUIObject* pObject)
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
	dwID = m_pListBoxToy->GetItemData(m_nToyIdx);

	pBuildingOpera->SetBuildingOpera(CAutoBuildingOperation::BP_ADD);
	pBuildingOpera->SetCurModelID(dwID);
	
	GetHomeDlgsMgr()->SetModelState(CECHomeDlgsMgr::MSE_CREATE);
	
	return;
}
void CDlgAutoIndoorToy::InValidateListIdx()
{
	m_nToyIdx = m_pListBoxToy->GetCount();
	UpdateData(false);
	return;
}
*/
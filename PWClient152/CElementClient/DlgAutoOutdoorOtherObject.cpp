/*
 * FILE: DlgAutoOutdoorOtherObject.cpp
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
#include "DlgAutoOutdoorOtherObject.h"
#include "DlgAutoAllRes.h"
#include "DlgAutoSwitchDlg.h"

#include "EC_HomeDlgsMgr.h"

#include "aui/AUIManager.h"
#include "aui/AUIListBox.h"

#include "A3DMacros.h"

#include "AutoBuildingOperation.h"
#include "AutoBuildingOperation.h"
#include "AutoTerrainCommon.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoOutdoorOtherObject, CDlgHomeBase)


AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoOutdoorOtherObject, CDlgHomeBase)

AUI_ON_EVENT("ListBox_OutDoorOtherObject", WM_LBUTTONUP, OnEventLBtnUpListOtherObject)

AUI_END_EVENT_MAP()

CDlgAutoOutdoorOtherObject::CDlgAutoOutdoorOtherObject()
{
	m_pListBoxOtherObject = NULL; 
	m_nOtherObjectIdx = NULL;

	m_pModelInfos = NULL;
	m_nNumModelInfos = NULL;
}

CDlgAutoOutdoorOtherObject::~CDlgAutoOutdoorOtherObject()
{
}

AUIStillImageButton * CDlgAutoOutdoorOtherObject::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnOutdoorOtherObject;
}

void CDlgAutoOutdoorOtherObject::OnShowDialog()
{
	this->SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoOutdoorOtherObject::OnInitContext()
{
	if(!InitResource())
		return false;

	LoadDefault();
	UpdateData(false);

	return true;
}

void CDlgAutoOutdoorOtherObject::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_OutDoorOtherObject", m_pListBoxOtherObject);
	DDX_ListBox(bSave, "ListBox_OutDoorOtherObject", m_nOtherObjectIdx);
}

void CDlgAutoOutdoorOtherObject::LoadDefault()
{
	m_nOtherObjectIdx = m_pListBoxOtherObject->GetCount();
}

bool CDlgAutoOutdoorOtherObject::InitResource()
{

	if( !FillOtherObjectListBox())
		return false;
	
	return true;
}

bool CDlgAutoOutdoorOtherObject::FillOtherObjectListBox()
{
	CAutoScene* pScene = GetAutoScene();
	if(!pScene)
		return false;

	CAutoBuilding* pBuilding = pScene->GetAutoBuilding();

	if( !pBuilding)
		return false;

	m_pModelInfos = pBuilding->GetModelInfos();
	m_nNumModelInfos = pBuilding->GetNumModelInfos();

	m_pListBoxOtherObject->ResetContent();

	int nIdx = 0;
	for( int i = 0; i < m_nNumModelInfos; i ++)
	{
// 		if(m_pModelInfos[i].dwAttribute == CAutoBuilding::MA_HOUSE)
// 		{
			m_pListBoxOtherObject->InsertString(nIdx, AS2AC(m_pModelInfos[i].strName));
			m_pListBoxOtherObject->SetItemData(nIdx, m_pModelInfos[i].dwID);
			nIdx++;
// 		}
	}
	return true;
}

void CDlgAutoOutdoorOtherObject::OnEventLBtnUpListOtherObject(WPARAM wParam, LPARAM lParam, AUIObject* pObject)
{
	if( GetHomeDlgsMgr()->GetModelState() == CECHomeDlgsMgr::MSE_MODIFY)
	{
		InValidateListIdx();
		return;
	}

	UpdateData(true);

	if (GetOperationType() != OT_BUILDING)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

		return;
	}
	
	CAutoScene* pScene = GetAutoScene();

	
	CAutoBuilding* pBuilding = pScene->GetAutoBuilding();
	CAutoBuildingOperation* pBuildingOpera = pScene->GetAutoBuildingOperation();
	pBuildingOpera->SetSelectedModelIndex(-1);
	// Clear contained models
	pBuildingOpera->SelectContainedModels();

	DWORD dwID;
	dwID = m_pListBoxOtherObject->GetItemData(m_nOtherObjectIdx);

	pBuildingOpera->SetBuildingOpera(CAutoBuildingOperation::BP_ADD);
	pBuildingOpera->SetCurModelID(dwID);
	
	GetHomeDlgsMgr()->SetModelState(CECHomeDlgsMgr::MSE_CREATE);
	
	return;
}

void CDlgAutoOutdoorOtherObject::InValidateListIdx()
{
	m_nOtherObjectIdx = m_pListBoxOtherObject->GetCount();
	UpdateData(false);
	return;
}

void CDlgAutoOutdoorOtherObject::AddAllResTreeViewItem(unsigned int uUiqueID)
{
	ACHAR szName[100];
	a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1002), m_nActionCount);
	m_nActionCount++;
	GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_OTHEROBJECT, szName, uUiqueID);

	InValidateListIdx();

	return;
}
*/
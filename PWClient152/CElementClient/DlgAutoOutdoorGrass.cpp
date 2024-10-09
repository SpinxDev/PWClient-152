/*
 * FILE: DlgAutoOutdoorGrass.cpp
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
#include "DlgAutoOutdoorGrass.h"
#include "DlgAutoAllRes.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoSwitchDlg.h"

#include "aui\\AUILabel.h"
#include "aui\\AUISlider.h"
#include "aui\\AUIListBox.h"
#include "aui\\AUIManager.h"
#include "aui\\AUIStillImageButton.h"

#include "A3DMacros.h"
#include "AutoGrassPoly.h"
#include "AutoSelectedArea.h"
#include "AutoTerrainCommon.h"
#include "AutoTerrainAction.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoOutdoorGrass, CDlgHomeBase)

AUI_ON_COMMAND("AddNormalGrassInfo", OnCommandAddNormalGrassInfo)
AUI_ON_COMMAND("DelNormalGrassInfo", OnCommandDelNormalGrassInfo)
AUI_ON_COMMAND("ClearAllNormalGrassInfo", OnCommandClearAllNormalGrassInfo)

AUI_ON_COMMAND("AddWaterGrassInfo", OnCommandAddWaterGrassInfo)
AUI_ON_COMMAND("DelWaterGrassInfo", OnCommandDelWaterGrassInfo)
AUI_ON_COMMAND("ClearAllWaterGrassInfo", OnCommandClearAllWaterGrassInfo)

AUI_ON_COMMAND("CreateNormalGrass", OnCommandCreateNormalGrass)
AUI_ON_COMMAND("CreateWaterGrass", OnCommandCreateWaterGrass)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoOutdoorGrass, CDlgHomeBase)

AUI_ON_EVENT("ListBox_NormalGrassSet", WM_LBUTTONDBLCLK, OnEventDblClkNormalGrassSetList)
AUI_ON_EVENT("ListBox_WaterGrassSet", WM_LBUTTONDBLCLK, OnEventDblClkWaterGrassSetList)

AUI_ON_EVENT("ListBox_NormalGrassPlant", WM_LBUTTONDBLCLK, OnEventDblClkNormalGrassPlantList)
AUI_ON_EVENT("ListBox_WaterGrassPlant", WM_LBUTTONDBLCLK, OnEventDblClkWaterGrassPlantList)
AUI_END_EVENT_MAP()

CDlgAutoOutdoorGrass::CDlgAutoOutdoorGrass()
{
	m_pSliderGrassDensity = NULL;
	m_pSliderGrassSeed = NULL;

	m_pSliderNormalGrassWeight = NULL;
	m_pSliderWaterGrassWeight = NULL;
	
	m_pLabelGrassDensity = NULL;
	m_pLabelGrassSeed = NULL;

	m_pLabelNormalGrassWeight = NULL;
	m_pLabelWaterGrassWeight = NULL;

	m_nGrassDensity = 0;
	m_nGrassSeed = 0;
	m_nNormalGrassWeight = 0;
	m_nWaterGrassWeight = 0;

	m_fGrassDensity = 0.0f;
	m_fGrassSeed = 0.0f;
	m_fNormalGrassWeight = 0.0f;
	m_fWaterGrassWeight = 0.0f;

	m_pListBoxNormalGrassSet = NULL;
	m_pListBoxNormalGrassPlant = NULL;
	m_pListBoxWaterGrassSet = NULL;
	m_pListBoxWaterGrassPlant = NULL;

	m_pNormalGrassInfo = NULL;
	m_nNumNormalGrassInfos = NULL;

	m_pWaterGrassInfo = NULL;
	m_nNumWaterGrassInfos = NULL;

	m_pBtnCreateNormalGrass = NULL;
	m_pBtnCreateWaterGrass = NULL;
}

CDlgAutoOutdoorGrass::~CDlgAutoOutdoorGrass()
{
	
}

AUIStillImageButton * CDlgAutoOutdoorGrass::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnOutdoorGrass;
}

void CDlgAutoOutdoorGrass::OnShowDialog()
{
	CDlgHomeBase::OnShowDialog();
	this->SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoOutdoorGrass::OnInitContext()
{
	if(!InitResource())
		return false;

	SetSliderBound();

	LoadDefault();
	UpdateData(false);
	SetLabelsText();
	return true;
}

void CDlgAutoOutdoorGrass::DoDataExchange(bool bSave)
{
	DDX_Control("Slider_GrassDensity", m_pSliderGrassDensity);
	DDX_Control("Slider_GrassSeed", m_pSliderGrassSeed);
	DDX_Control("Slider_NormalGrassWeight", m_pSliderNormalGrassWeight);
	DDX_Control("Slider_WaterGrassWeight", m_pSliderWaterGrassWeight);
	
	DDX_Control("Label_GrassDensity", m_pLabelGrassDensity);
	DDX_Control("Label_GrassSeed", m_pLabelGrassSeed);
	DDX_Control("Label_NormalGrassWeight", m_pLabelNormalGrassWeight);
	DDX_Control("Label_WaterGrassWeight", m_pLabelWaterGrassWeight);

	DDX_Control("ListBox_NormalGrassSet", m_pListBoxNormalGrassSet);
	DDX_Control("ListBox_NormalGrassPlant", m_pListBoxNormalGrassPlant);
	DDX_Control("ListBox_WaterGrassSet", m_pListBoxWaterGrassSet);
	DDX_Control("ListBox_WaterGrassPlant", m_pListBoxWaterGrassPlant);

	DDX_Slider(bSave, "Slider_GrassDensity", m_nGrassDensity);
	DDX_Slider(bSave, "Slider_GrassSeed", m_nGrassSeed);
	DDX_Slider(bSave, "Slider_NormalGrassWeight", m_nNormalGrassWeight);
	DDX_Slider(bSave, "Slider_WaterGrassWeight", m_nWaterGrassWeight);

	DDX_Control( "Btn_CreateNormalGrass", m_pBtnCreateNormalGrass);
	DDX_Control( "Btn_CreateWaterGrass", m_pBtnCreateWaterGrass);

	m_fGrassDensity = float(m_nGrassDensity * 0.01f);
	m_fGrassSeed = float(m_nGrassSeed);

	m_fNormalGrassWeight = float(m_nNormalGrassWeight * 0.01f);
	m_fWaterGrassWeight = float(m_nWaterGrassWeight * 0.01f);



}

void CDlgAutoOutdoorGrass::LoadDefault()
{
	m_nGrassSeed = 1;
	m_nGrassDensity = 10;
	m_nNormalGrassWeight = 100;
	m_nWaterGrassWeight = 100;
}

bool CDlgAutoOutdoorGrass::InitResource()
{
	if( !FillNormalGrassSetListBox())
		return false;
	
	if( !FillWaterGrassSetListBox())
		return false;

	return true;
}

void CDlgAutoOutdoorGrass::SetLabelsText()
{
	ACHAR szName[100];

	swprintf(szName, _AL("%d"), m_nGrassSeed);
	m_pLabelGrassSeed->SetText(szName);

	swprintf(szName, _AL("%-4.2f"), m_fGrassDensity);
	m_pLabelGrassDensity->SetText(szName);

	swprintf(szName, _AL("%-4.2f"), m_fNormalGrassWeight);
	m_pLabelNormalGrassWeight->SetText(szName);

	swprintf(szName, _AL("%-4.2f"), m_fWaterGrassWeight);
	m_pLabelWaterGrassWeight->SetText(szName);

}

bool CDlgAutoOutdoorGrass::FillNormalGrassSetListBox()
{
	
	CAutoGrass* pGrass = GetAutoScene()->GetAutoGrass();
	if( !pGrass)
		return false;

	m_pNormalGrassInfo = pGrass->GetLandGrassInfos();
	m_nNumNormalGrassInfos = pGrass->GetNumLandGrassInfos();

	m_pListBoxNormalGrassSet->ResetContent();
	for( int i = 0; i < m_nNumNormalGrassInfos; i++)
	{
		m_pListBoxNormalGrassSet->InsertString(i, AS2AC(m_pNormalGrassInfo[i].strName));
	}

	return true;
}

bool CDlgAutoOutdoorGrass::FillWaterGrassSetListBox()
{
	CAutoGrass* pGrass = GetAutoScene()->GetAutoGrass();
	if( !pGrass)
		return false;


	m_pWaterGrassInfo = pGrass->GetWaterGrassInfos();
	m_nNumWaterGrassInfos = pGrass->GetNumWaterGrassInfos();
	
	m_pListBoxWaterGrassSet->ResetContent();

	for( int i = 0; i < m_nNumWaterGrassInfos; i++)
	{
		m_pListBoxWaterGrassSet->InsertString(i, AS2AC(m_pWaterGrassInfo[i].strName));
	}

	return true;
}

void CDlgAutoOutdoorGrass::SetSliderBound()
{
	m_pSliderGrassDensity->SetTotal(100, 0);
	m_pSliderGrassSeed->SetTotal(100, 0);
	m_pSliderNormalGrassWeight->SetTotal(100, 1);
	m_pSliderWaterGrassWeight->SetTotal(100, 1);
	
}

void CDlgAutoOutdoorGrass::OnCommandCreateNormalGrass(const char* szCommand)
{
	UpdateData(true);

	if (GetOperationType() != OT_GRASS_POLY)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CAutoScene* pScene = GetAutoScene();
	CAutoGrassPoly* pGrassPoly = pScene->GetAutoGrassPoly();
	if (NULL == pGrassPoly)
		return;

	//Fill points
	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumPolyEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetPolyEditPoints();

	pGrassPoly->SetAreaPoints(nNum, pPoints);

	bool bClosed = pSelArea->GetPolyEditAreaClosed();
	if (!bClosed)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	int nGrassTypeCount = m_NormalGrassInfoArray.GetSize();
	if (nGrassTypeCount <= 0)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	DWORD* pIDs = new DWORD[nGrassTypeCount];
	float* pWeights = new float[nGrassTypeCount];
	for (int i=0; i<nGrassTypeCount; i++)
	{
		pIDs[i] = m_NormalGrassInfoArray[i].dwID;
		pWeights[i] = m_NormalGrassInfoArray[i].fWeight;
	}

	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();


	if (!pGrassPoly->CreateGrassPoly(CAutoGrass::GK_LAND, (float)(m_nGrassDensity * 0.01f),
			nGrassTypeCount, pIDs, pWeights, m_nGrassSeed))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	// Store action
	CAutoGrass* pGrass = pScene->GetAutoGrass();
	CClosedArea* pArea = pGrassPoly->GetArea();

	if( GetHomeDlgsMgr()->GetModifying())
	{
		int nIdx = GetHomeDlgsMgr()->m_pDlgAutoAllRes->GetTreeViewSelItemIdx();
		if (!pAction->ChangeActionGrassPoly(
			CAutoGrass::GK_LAND,
			(float)(m_nGrassDensity * 0.01f),
			nGrassTypeCount,
			pIDs,
			pWeights,
			m_nGrassSeed,
			pArea->GetNumPoints(),
			pArea->GetFinalPoints(), 
			nIdx))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			delete[] pIDs;
			delete[] pWeights;

			return;
		}
	}
	else
	{
		if (!pAction->AddActionGrassPoly(
			CAutoGrass::GK_LAND,
			(float)(m_nGrassDensity * 0.01f),
			nGrassTypeCount,
			pIDs,
			pWeights,
			m_nGrassSeed,
			pArea->GetNumPoints(),
			pArea->GetFinalPoints()))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			delete[] pIDs;
			delete[] pWeights;

			return;
		}
	}
		
	delete[] pIDs;
	delete[] pWeights;

	//Clear points
	pSelArea->CopyPointsFromPolyEditToView();
	pSelArea->SetViewAreaClosed(true);
	pSelArea->DeleteAllPolyEditPoints();
	
	if( !GetHomeDlgsMgr()->GetModifying())
	{
		ACHAR szName[100];
		a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1006), m_nActionCount);
		m_nActionCount++;
		
		GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_GRASS, szName);
	}
	else
	{
		GetHomeDlgsMgr()->SetModifying(false);
		SetBtnCreateToModify();
		GetAutoScene()->RefreshAllGrassActions();
	}
	return;
}

void CDlgAutoOutdoorGrass::OnCommandCreateWaterGrass(const char* szCommand)
{
	UpdateData(true);

	if (GetOperationType() != OT_GRASS_POLY)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CAutoScene* pScene = GetAutoScene();
	CAutoGrassPoly* pGrassPoly = pScene->GetAutoGrassPoly();
	if (NULL == pGrassPoly)
		return;

	//Fill points
	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumPolyEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetPolyEditPoints();
	pGrassPoly->SetAreaPoints(nNum, pPoints);

	bool bClosed = pSelArea->GetPolyEditAreaClosed();

	if (!bClosed)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	int nGrassTypeCount = m_WaterGrassInfoArray.GetSize();
	if (nGrassTypeCount <= 0)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1007), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	DWORD* pIDs = new DWORD[nGrassTypeCount];
	float* pWeights = new float[nGrassTypeCount];
	for (int i=0; i<nGrassTypeCount; i++)
	{
		pIDs[i] = m_WaterGrassInfoArray[i].dwID;
		pWeights[i] = m_WaterGrassInfoArray[i].fWeight;
	}

	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();


	if (!pGrassPoly->CreateGrassPoly(CAutoGrass::GK_WATER, (float)(m_nGrassDensity * 0.01f),
			nGrassTypeCount, pIDs, pWeights, m_nGrassSeed))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	// Store action
	CAutoGrass* pGrass = pScene->GetAutoGrass();
	CClosedArea* pArea = pGrassPoly->GetArea();

	if( GetHomeDlgsMgr()->GetModifying())
	{
		int nIdx = GetHomeDlgsMgr()->m_pDlgAutoAllRes->GetTreeViewSelItemIdx();
		if (!pAction->ChangeActionGrassPoly(
				CAutoGrass::GK_WATER,
				(float)(m_nGrassDensity * 0.01f),
				nGrassTypeCount,
				pIDs,
				pWeights,
				m_nGrassSeed,
				pArea->GetNumPoints(),
				pArea->GetFinalPoints(), 
				nIdx))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		GetAutoScene()->RefreshAllGrassActions();
	}
	else
	{
		if (!pAction->AddActionGrassPoly(
				CAutoGrass::GK_WATER,
				(float)(m_nGrassDensity * 0.01f),
				nGrassTypeCount,
				pIDs,
				pWeights,
				m_nGrassSeed,
				pArea->GetNumPoints(),
				pArea->GetFinalPoints()))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
	}			
	delete[] pIDs;
	delete[] pWeights;

	
	//Clear points
	pSelArea->CopyPointsFromPolyEditToView();
	pSelArea->SetViewAreaClosed(true);
	pSelArea->DeleteAllPolyEditPoints();

	if( !GetHomeDlgsMgr()->GetModifying())
	{
		ACHAR szName[100];
		a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1008), m_nActionCount);
		m_nActionCount++;
		
		GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_GRASS, szName);
	}
	else
	{
		GetHomeDlgsMgr()->SetModifying(false);
		SetBtnModifyToCreate();
		
	}
	
	return;
}

void CDlgAutoOutdoorGrass::OnCommandAddNormalGrassInfo(const char* szCommand)
{
	UpdateData(true);

	PLANTIDWEIGHTINFO info;
	int nIdx = m_pListBoxNormalGrassSet->GetCurSel();

	if( nIdx < 0 || nIdx >= m_pListBoxNormalGrassSet->GetCount())
		return;

	info.dwID = m_pNormalGrassInfo[nIdx].dwID;
	info.fWeight = m_fNormalGrassWeight;

	if( MakeSureThisInfoNoExiting(m_NormalGrassInfoArray, info))
	{
		m_NormalGrassInfoArray.Add(info);
		ShowNormalGrassInfo();
	}
	return;
}

void CDlgAutoOutdoorGrass::OnCommandDelNormalGrassInfo(const char* szCommand)
{
	if( m_NormalGrassInfoArray.GetSize() == 0)
		return ;

	int nIdx = m_pListBoxNormalGrassPlant->GetCurSel();
	if( nIdx < 0 || nIdx >= m_pListBoxNormalGrassPlant->GetCount())
		return;
	m_NormalGrassInfoArray.RemoveAt(nIdx);
	ShowNormalGrassInfo();
}
	
void CDlgAutoOutdoorGrass::OnCommandClearAllNormalGrassInfo(const char* szCommand)
{

	m_NormalGrassInfoArray.RemoveAll();
	m_pListBoxNormalGrassPlant->ResetContent();
}

void CDlgAutoOutdoorGrass::OnCommandAddWaterGrassInfo(const char* szCommand)
{
	UpdateData(true);

	PLANTIDWEIGHTINFO info;
	int nIdx = m_pListBoxWaterGrassSet->GetCurSel();
	if ( nIdx < 0 || nIdx >= m_pListBoxWaterGrassSet->GetCount())
		return;

	info.dwID = m_pWaterGrassInfo[nIdx].dwID;
	info.fWeight = m_fWaterGrassWeight;

	if( MakeSureThisInfoNoExiting(m_WaterGrassInfoArray, info))
	{
		m_WaterGrassInfoArray.Add(info);
		ShowWaterGrassInfo();
	}
	return;
}

void CDlgAutoOutdoorGrass::OnCommandDelWaterGrassInfo(const char* szCommand)
{
	if(m_WaterGrassInfoArray.GetSize() == 0)
		return;

	int nIdx = m_pListBoxWaterGrassPlant->GetCurSel();
	if( nIdx < 0 || nIdx > m_pListBoxWaterGrassPlant->GetCount())
		return;

	m_WaterGrassInfoArray.RemoveAt(nIdx);
	ShowWaterGrassInfo();
}

void CDlgAutoOutdoorGrass::OnCommandClearAllWaterGrassInfo(const char* szCommand)
{
	m_WaterGrassInfoArray.RemoveAll();
	m_pListBoxWaterGrassPlant->ResetContent();
}

void CDlgAutoOutdoorGrass::ShowNormalGrassInfo()
{
	CAutoGrass* pGrass = GetAutoScene()->GetAutoGrass();
	CAutoGrass::GRASSINFO* pTypes = pGrass->GetLandGrassInfos();
	int nSize = m_NormalGrassInfoArray.GetSize();

	m_pListBoxNormalGrassPlant->ResetContent();
	
	for( int i = 0; i < nSize; i++)
	{
		int nIdx = pGrass->GetGlobalLandGrassIndexById(m_NormalGrassInfoArray[i].dwID);
		ASSERT(nIdx > -1);
		m_pListBoxNormalGrassPlant->AddString(AS2AC(pTypes[nIdx].strName));
	}
}

void CDlgAutoOutdoorGrass::ShowWaterGrassInfo()
{
	CAutoGrass* pGrass = GetAutoScene()->GetAutoGrass();
	CAutoGrass::GRASSINFO* pTypes = pGrass->GetWaterGrassInfos();
	int nSize = m_WaterGrassInfoArray.GetSize();
	int i;

	m_pListBoxWaterGrassPlant->ResetContent();
	
	for (i=0; i < nSize; i++)
	{
		int nIdx = pGrass->GetGlobalWaterGrassIndexById(m_WaterGrassInfoArray[i].dwID);
		ASSERT(nIdx > -1);
		m_pListBoxWaterGrassPlant->AddString(AS2AC(pTypes[nIdx].strName));
	}
}

void CDlgAutoOutdoorGrass::OnCommandMoveSlider(const char * szCommand)
{
	UpdateData(true);
	SetLabelsText();
}

void CDlgAutoOutdoorGrass::SetBtnCreateToModify()
{
	m_pBtnCreateNormalGrass->SetText( GetStringFromTable(1009));
	m_pBtnCreateWaterGrass->SetText( GetStringFromTable(1009) );
	return;
}

void CDlgAutoOutdoorGrass::SetBtnModifyToCreate()
{
	m_pBtnCreateNormalGrass->SetText( GetStringFromTable(1010) );
	m_pBtnCreateWaterGrass->SetText( GetStringFromTable(1011) );

	return;
}

void CDlgAutoOutdoorGrass::OnEventDblClkNormalGrassSetList(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	OnCommandAddNormalGrassInfo("");
	return;
}

void CDlgAutoOutdoorGrass::OnEventDblClkWaterGrassSetList(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	OnCommandAddWaterGrassInfo("");
	return;
}

void CDlgAutoOutdoorGrass::OnEventDblClkNormalGrassPlantList(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	OnCommandDelNormalGrassInfo("");
	return;
}

void CDlgAutoOutdoorGrass::OnEventDblClkWaterGrassPlantList(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	OnCommandDelWaterGrassInfo("");
	return;
}
	
bool CDlgAutoOutdoorGrass::MakeSureThisInfoNoExiting(const InfosArray_t &i_InfoArray, const PLANTIDWEIGHTINFO &i_Info)
{
	for( int i = 0; i < i_InfoArray.GetSize(); i++)
	{
		if(i_Info.dwID == i_InfoArray[i].dwID)
			return false;
	}
	return true;
}
*/
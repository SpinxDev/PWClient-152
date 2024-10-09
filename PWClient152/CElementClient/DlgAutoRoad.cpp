/*
 * FILE: DlgAutoRoad.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoRoad.h"
#include "DlgAutoBaseRoad.h"

#include "EC_HomeDlgsMgr.h"
#include "DlgAutoChooseTexture.h"
#include "DlgAutoAllRes.h"

#include "aui/AUILabel.h"
#include "aui/AUIManager.h"
#include "aui/AUIStillImageButton.h"

#include "AutoTerrainCommon.h"

#include "A3DEngine.h"
#include "A3DViewport.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "AutoScene.h"
#include "AutoTerrain.h"
#include "AutoRoad.h"					// Added by jdl, 2006.03.03
#include "AutoTerrainFlat.h"			// Added by jdl, 2006.03.03
#include "AutoBuildingOperation.h"		// Added by jdl, 2006.03.03
#include "AutoTerrainCommon.h"
#include "AutoTerrainAction.h"
#include "AutoSelectedArea.h"

#include "windowsx.h"

#include "A3DMacros.h"

#include "DlgAutoSliderBound.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoRoad, CDlgHomeBase)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)

AUI_ON_COMMAND("CreateRoad", OnCommandCreateRoad)
AUI_ON_COMMAND("TurnToBase", OnCommandTurnToBase)

AUI_ON_COMMAND("Save", OnCommandSave)
AUI_ON_COMMAND("SaveAs", OnCommandSaveAs)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoRoad, CDlgHomeBase)

AUI_END_EVENT_MAP()


CDlgAutoRoad::CDlgAutoRoad()
{	
	m_pSliderMaxRoadHeight = NULL;
	m_pSliderMainSmoothRoad = NULL;
	m_pSliderPartSmoothRoad = NULL;
	m_pSliderRoadWidth = NULL;
	m_pSliderSeedRoad = NULL;

	m_pLabelMaxRoadHeight = NULL;
	m_pLabelMainSmoothRoad = NULL;
	m_pLabelPartSmoothRoad = NULL;
	m_pLabelRoadWidth = NULL;
	m_pLabelSeedRoad = NULL;
	
	m_nMaxRoadHeight = 0;
	m_nMainSmoothRoad = 0;
	m_nPartSmoothRoad = 0;
	m_nRoadWidth = 0;
	m_nSeedRoad = 0;

	m_pBtnCreate = NULL;

	for( int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}

	memset(m_szFileName, 0, MAX_PATH);
}

CDlgAutoRoad::~CDlgAutoRoad()
{
}

bool CDlgAutoRoad::OnInitContext()
{
	SetSliderBound();
	LoadDefault();
	UpdateData(false);
	SetLabelsText();

	return true;
}

void CDlgAutoRoad::OnShowDialog()
{
	CDlgHomeBase::OnShowDialog();

	SetCanMove(false);
}


#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);
void CDlgAutoRoad::DoDataExchange(bool bSave)
{
	
	DDX_Control("Slider_MaxRoadHeight", m_pSliderMaxRoadHeight);
	DDX_Control("Slider_MainSmoothRoad", m_pSliderMainSmoothRoad);
	DDX_Control("Slider_PartSmoothRoad", m_pSliderPartSmoothRoad);
	DDX_Control("Slider_RoadWidth", m_pSliderRoadWidth);
	DDX_Control("Slider_SeedRoad", m_pSliderSeedRoad);

	DDX_Control("Label_MaxRoadHeight", m_pLabelMaxRoadHeight);
	DDX_Control("Label_MainSmoothRoad", m_pLabelMainSmoothRoad);
	DDX_Control("Label_PartSmoothRoad", m_pLabelPartSmoothRoad);
	DDX_Control("Label_RoadWidth", m_pLabelRoadWidth);
	DDX_Control("Label_SeedRoad", m_pLabelSeedRoad);

	DDX_Slider(bSave, "Slider_MaxRoadHeight", m_nMaxRoadHeight);
	DDX_Slider(bSave, "Slider_MainSmoothRoad", m_nMainSmoothRoad);
	DDX_Slider(bSave, "Slider_PartSmoothRoad", m_nPartSmoothRoad);
	DDX_Slider(bSave, "Slider_RoadWidth", m_nRoadWidth);
	DDX_Slider(bSave, "Slider_SeedRoad", m_nSeedRoad);

	DDX_CheckBox(bSave, "Check_FlatRoad", m_bCheckFlatRoad);

	DDX_Control("Btn_Create", m_pBtnCreate);

	GET_BTN_POINTER(BTN_SAVE);
	GET_BTN_POINTER(BTN_SAVE_AS);

}
void CDlgAutoRoad::SetSliderBound()
{	
	m_pSliderMaxRoadHeight->SetTotal(MAX_ROAD_HEIGHT, MIN_ROAD_HEIGHT);
	m_pSliderMainSmoothRoad->SetTotal(MAX_MAIN_SMOOTH_ROAD, MIN_MAIN_SMOOTH_ROAD);
	m_pSliderPartSmoothRoad->SetTotal(MAX_PART_SMOOTH_ROAD, MIN_PART_SMOOTH_ROAD);
	m_pSliderRoadWidth->SetTotal(MAX_ROAD_WIDTH, MIN_ROAD_WIDTH);
	m_pSliderSeedRoad->SetTotal(MAX_SEED_ROAD, MIN_SEED_ROAD);
}
void CDlgAutoRoad::LoadDefault()
{
	m_nMaxRoadHeight = 10;
	m_nMainSmoothRoad = 50;
	m_nPartSmoothRoad = 20;
	m_nRoadWidth = 20;
	m_nSeedRoad = 1;

	m_bCheckFlatRoad = false;
	GetHomeDlgsMgr()->LoadTextureParamInfo(m_texParamInfo, TT_ROADBED, TT_ROAD);

}
void CDlgAutoRoad::SetLabelsText()
{
	ACHAR szName[100];
	

	
	swprintf(szName, _AL("%d"), m_nMaxRoadHeight);
	m_pLabelMaxRoadHeight->SetText(szName);

	swprintf(szName, _AL("%d"), m_nMainSmoothRoad);
	m_pLabelMainSmoothRoad->SetText(szName);

	swprintf(szName, _AL("%-4.2f"), (float)(m_nPartSmoothRoad * 0.01f));
	m_pLabelPartSmoothRoad->SetText(szName);

	swprintf(szName, _AL("%d"), m_nRoadWidth);
	m_pLabelRoadWidth->SetText(szName);

	swprintf(szName, _AL("%d"), m_nSeedRoad);
	m_pLabelSeedRoad->SetText(szName);

}

void CDlgAutoRoad::OnCommandMoveSlider(const char * szCommand)
{
	UpdateData(true);
	SetLabelsText();
}

void CDlgAutoRoad::OnCommandCreateRoad(const char* szCommand)
{
	UpdateData(true);
	
	if (GetOperationType() != OT_ROAD)
	{
		return;
	}

	CAutoScene* pScene = GetAutoScene();
	CAutoRoad* pRoad = pScene->GetAutoRoad();
	if (NULL == pRoad)
		return;

	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumLineEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetLineEditPoints();
	
	
	pRoad->SetPoints(nNum, pPoints);

	if (!pRoad->IsPointsValid())
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();

	
	if (!pRoad->CreateRoad((float)m_nRoadWidth, (float)m_nMaxRoadHeight, (float)m_nMainSmoothRoad, (float) (m_nPartSmoothRoad * 0.01f), m_nSeedRoad, m_bCheckFlatRoad))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	// Store action
	if( GetHomeDlgsMgr()->GetModifying())
	{
		int nIdx = GetHomeDlgsMgr()->m_pDlgAutoAllRes->GetTreeViewSelItemIdx();
		
		if (!pAction->ChangeActionRoad(
				(float)m_nRoadWidth,
				(float)m_nMaxRoadHeight,
				(float)m_nMainSmoothRoad,
				(float)(m_nPartSmoothRoad * 0.01f),
				m_nSeedRoad,
				pRoad->GetNumPoints(),
				pRoad->GetPoints(),
				m_bCheckFlatRoad ? 1 : 0, 
				nIdx))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}

		GetAutoScene()->RefreshAllRoadActions();
	}
	else
	{
		if (!pAction->AddActionRoad(
				(float)m_nRoadWidth,
				(float)m_nMaxRoadHeight,
				(float)m_nMainSmoothRoad,
				(float)(m_nPartSmoothRoad * 0.01f),
				m_nSeedRoad,
				pRoad->GetNumPoints(),
				pRoad->GetPoints(),
				m_bCheckFlatRoad ? 1 : 0))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
	}
	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	if (!pAction->AddActionRoadTextureInfo(
			m_texParamInfo.fMainTextureU,
			m_texParamInfo.fMainTextureV,
			m_texParamInfo.fPatchTextureU,
			m_texParamInfo.fPatchTextureV,
			pTexture->GetTextureID(TT_ROADBED),
			pTexture->GetTextureID(TT_ROAD)))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	GetAutoScene()->RecreateTerrainRender(true);

	pSelArea->CopyPointsFromLineEditToView();
	pSelArea->SetViewAreaClosed(false);
	pSelArea->DeleteAllLineEditPoints();

	if( !GetHomeDlgsMgr()->GetModifying())
	{
		//Add Item to TerrainRes Tree View
		ACHAR szName[100];
		int nCount = GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->GetActionCount();
	
		a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1004), nCount);
		

		GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->SetActionCount(nCount + 1);
		
		GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_ROAD, szName);
	}
	else
	{
		GetHomeDlgsMgr()->SetModifying(false);
		SetBtnModifyToCreate();
		
	}

	//设置纹理操作可用
	GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->SetTexOpEnable(true);

	return;

}


void CDlgAutoRoad::OnCommandTurnToBase(const char* szCommand)
{
	this->Show(false);
	GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->Show(false);
	GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->Show(true);

	GetHomeDlgsMgr()->SetUnAdvDlgsEnable();
}

void CDlgAutoRoad::SetBtnCreateToModify()
{
	//设置"修改"
	m_pBtnCreate->SetText( GetStringFromTable(1005));
	return;
}

void CDlgAutoRoad::SetBtnModifyToCreate()
{
	m_pBtnCreate->SetText( GetStringFromTable(1006) );
	return;
}
*/
//void CDlgAutoRoad::InitSettings(ROADINFO &i_RoadInfo,
//									bool		i_bSaveEnable /* = false */,
//									const char* i_szFileName /* = NULL */)
/*{
	//确保数据可用
	m_nMaxRoadHeight = int(i_RoadInfo.fMaxRoadHeight);
	m_nMainSmoothRoad =int(i_RoadInfo.fMainSmoothRoad);
	m_nPartSmoothRoad = int(i_RoadInfo.fPartSmoothRoad * 100.0f);
	m_nRoadWidth = int(i_RoadInfo.fRoadWidth); 
	m_nSeedRoad = int(i_RoadInfo.dwSeedRoad);

	m_bCheckFlatRoad = i_RoadInfo.bFlatRoad;

	Clamp(m_nMaxRoadHeight, MAX_ROAD_HEIGHT, MIN_ROAD_HEIGHT);
	Clamp(m_nMainSmoothRoad, MAX_MAIN_SMOOTH_ROAD, MIN_MAIN_SMOOTH_ROAD);
	Clamp(m_nPartSmoothRoad, MAX_PART_SMOOTH_ROAD, MIN_PART_SMOOTH_ROAD);
	Clamp(m_nRoadWidth, MAX_ROAD_WIDTH, MIN_ROAD_WIDTH);
	Clamp(m_nSeedRoad, MAX_SEED_ROAD, MIN_SEED_ROAD);


	//设置保存和另存
	m_pBtn[BTN_SAVE]->Enable(i_bSaveEnable);
	m_pBtn[BTN_SAVE_AS]->Enable(!i_bSaveEnable);

	UpdateData(false);
	SetLabelsText();

	if( i_bSaveEnable) //如果可以保存，指定文件名
	{
		strcpy(m_szFileName, i_szFileName);
	}
	else				//如果可以另存，指定文件名
	{
		//获取时间
		WORD wYear,wMonth, wDay, wDate, wHour, wMinute, wSecond;
		a_GetSystemTime(&wYear, &wMonth, &wDay, &wDate, &wHour, &wMinute, &wSecond);

		sprintf(m_szFileName, "%d_%d_%d_%d_%d_%d", wYear, wMonth, wDay, wHour, wMinute, wSecond);
		
	}
}

void CDlgAutoRoad::OnCommandSave(const char* szCommand)
{
	UpdateData(true);

	CAutoParameters *pParameters = GetAutoScene()->GetAutoParameters();
	CAutoParameters::ROADINFO param;
	
	param.fMaxRoadHeight = float(m_nMaxRoadHeight);
	param.fMainSmoothRoad = float(m_nMainSmoothRoad);
	param.fPartSmoothRoad = float(m_nPartSmoothRoad * 0.01f);
	param.fRoadWidth = float(m_nRoadWidth);
	param.dwSeedRoad = m_nSeedRoad;


	//创建目录
	ACHAR szPath[MAX_PATH];
	a_strcpy(szPath, _AL("userdata\\"));
	_wmkdir(szPath);

	a_strcat(szPath, _AL("home\\"));
	_wmkdir(szPath);

	a_strcat(szPath,_AL("road\\"));
	_wmkdir(szPath);

	//设置实际文件名
	char szFileName[MAX_PATH];
	strcpy(szFileName, AC2AS(szPath));
	strcat(szFileName, m_szFileName);


	//保存
	if(!pParameters->SaveRoadInfo(szFileName, param))
		AutoHomeReport("CDlgAutoRoad::OnCommandSave, Failed to SaveRoadInfo!");

	this->Show(false);

	GetHomeDlgsMgr()->SetUnAdvDlgsEnable();
}

void CDlgAutoRoad::OnCommandSaveAs(const char* szCommand)
{
	UpdateData(true);
	
	GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->InsertRoadSettings(AS2AC(m_szFileName));
	OnCommandSave("");
	
}

*/
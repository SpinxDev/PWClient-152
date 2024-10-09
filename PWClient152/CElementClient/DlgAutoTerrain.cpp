/*
 * FILE: DlgAutoTerrain.cpp
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
#include "DlgAutoTerrain.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoChooseTexture.h"
#include "DlgAutoBaseTerrain.h"

#include "aui/AUILabel.h"
#include "aui/AUIStillImageButton.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "AutoScene.h"
#include "AutoTerrain.h"
#include "AutoBuildingOperation.h"		// Added by jdl, 2006.03.03
#include "AutoTerrainCommon.h"
#include "AutoTerrainAction.h"


#include "DlgAutoSliderBound.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoTerrain, CDlgHomeBase)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
AUI_ON_COMMAND("CreateTerrain", OnCommandCreateTerrain)
AUI_ON_COMMAND("TurnToBase", OnCommandTurnToBase)

AUI_ON_COMMAND("Save", OnCommandSave)
AUI_ON_COMMAND("SaveAs", OnCommandSaveAs)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoTerrain, CDlgHomeBase)

AUI_END_EVENT_MAP()





CDlgAutoTerrain::CDlgAutoTerrain()
{
	m_pSliderTextureRatio = NULL;
	m_pSliderMainTexture = NULL;
	m_pSliderPatchTexture = NULL;
	m_pSliderPatchDivision = NULL;

	m_nTextureRatio = 0;
	m_nMainTexture = 0;
	m_nPatchTexture = 0;
	m_nPatchDivision = 0;

	m_fTextureRatio = 0.0f;
	m_fMainTexture = 0.0f;
	m_fPatchTexture = 0.0f;
	m_fPatchDivision = 0.0f;

	m_pLabelTextureRatio = NULL;
	m_pLabelMainTexture = NULL;
	m_pLabelPatchTexture = NULL;
	m_pLabelPatchDivision = NULL;

	m_pSliderMaxHeight = NULL;
	m_pSliderMainSmooth = NULL;
	m_pSliderPartSmooth = NULL;
	m_pSliderTerrainSeed = NULL;

	m_pLabelMaxHeight = NULL;
	m_pLabelMainSmooth = NULL;
	m_pLabelPartSmooth = NULL;
	m_pLabelTerrainSeed = NULL;

	m_nMaxHeight = 0;
	m_nMainSmooth = 0;
	m_nPartSmooth = 0;
	m_nTerrainSeed = 0;

	for(int i = 0; i < BTN_NUM ; i++)
	{
		m_pBtn[i] = NULL;
	}
	memset(m_szFileName, 0, MAX_PATH);
}

CDlgAutoTerrain::~CDlgAutoTerrain()
{
}

bool CDlgAutoTerrain::OnInitContext()
{
	SetSliderBound();
	LoadDefault();
	UpdateData(false);
	SetLabelsText();
	
	return true;
}

void CDlgAutoTerrain::OnShowDialog()
{
	CDlgHomeBase::OnShowDialog();
	SetCanMove(false);
}


#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);

void CDlgAutoTerrain::DoDataExchange(bool bSave)
{
	DDX_Control("Slider_TextureRatio", m_pSliderTextureRatio);
	DDX_Control("Slider_MainTexture", m_pSliderMainTexture);
	DDX_Control("Slider_PatchTexture", m_pSliderPatchTexture);
	DDX_Control("Slider_PatchDivision", m_pSliderPatchDivision);

	DDX_Control("Label_TextureRatio", m_pLabelTextureRatio);
	DDX_Control("Label_MainTexture", m_pLabelMainTexture);
	DDX_Control("Label_PatchTexture", m_pLabelPatchTexture);
	DDX_Control("Label_PatchDivision", m_pLabelPatchDivision);

	DDX_Slider(bSave, "Slider_TextureRatio", m_nTextureRatio);
	DDX_Slider(bSave, "Slider_MainTexture", m_nMainTexture);
	DDX_Slider(bSave, "Slider_PatchTexture", m_nPatchTexture);
	DDX_Slider(bSave, "Slider_PatchDivision", m_nPatchDivision);

	m_fTextureRatio = float((m_nTextureRatio - 100) * 0.01f);
	m_fMainTexture = float(m_nMainTexture * 0.01f);
	m_fPatchTexture = float(m_nPatchTexture * 0.01f);
	m_fPatchDivision = float(m_nPatchDivision);

	DDX_Control("Slider_MaxHeight", m_pSliderMaxHeight);
	DDX_Control("Slider_MainSmooth", m_pSliderMainSmooth);
	DDX_Control("Slider_PartSmooth", m_pSliderPartSmooth);
	DDX_Control("Slider_TerrainSeed", m_pSliderTerrainSeed);

	DDX_Control("Label_MaxHeight", m_pLabelMaxHeight);
	DDX_Control("Label_MainSmooth", m_pLabelMainSmooth);
	DDX_Control("Label_PartSmooth", m_pLabelPartSmooth);
	DDX_Control("Label_TerrainSeed", m_pLabelTerrainSeed);

	DDX_Slider(bSave, "Slider_MaxHeight", m_nMaxHeight);
	DDX_Slider(bSave, "Slider_MainSmooth", m_nMainSmooth);
	DDX_Slider(bSave, "Slider_PartSmooth", m_nPartSmooth);
	DDX_Slider(bSave, "Slider_TerrainSeed", m_nTerrainSeed);

	GET_BTN_POINTER(BTN_SAVE);
	GET_BTN_POINTER(BTN_SAVE_AS);
}

void CDlgAutoTerrain::SetSliderBound()
{
	m_pSliderTextureRatio->SetTotal(MAX_TEXTURE_RATIO, MIN_TEXTURE_RATIO);
	m_pSliderMainTexture->SetTotal(MAX_MAIN_TEXTURE, MIN_MAIN_TEXTURE);
	m_pSliderPatchTexture->SetTotal(MAX_PATCH_TEXTURE, MIN_PATCH_TEXTURE);
	m_pSliderPatchDivision->SetTotal(MAX_PATCH_DIVISION, MIN_PATCH_DIVISION);
	
	m_pSliderMaxHeight->SetTotal(MAX_HEIGHT, MIN_HEIGHT);
	m_pSliderMainSmooth->SetTotal(MAX_MAINSMOOTH, MIN_MAINSMOOTH );
	m_pSliderPartSmooth->SetTotal(MAX_PARTSMOOTH, MIN_PARTSMOOTH);
	m_pSliderTerrainSeed->SetTotal(MAX_SEED, MIN_SEED);
}

void CDlgAutoTerrain::LoadDefault()
{
	m_nMaxHeight = 150;
	m_nMainSmooth = 3;
	m_nPartSmooth = 3;
	m_nTerrainSeed = 4;

	m_nTextureRatio = 120;
	m_nMainTexture = 40;
	m_nPatchTexture = 120;
	m_nPatchDivision = 30;

	GetHomeDlgsMgr()->LoadTextureParamInfo(m_texParamInfo, TT_MAIN, TT_MAIN_PATCH);
}

void CDlgAutoTerrain::OnCommandMoveSlider(const char * szCommand)
{
	UpdateData(true);
	SetLabelsText();
	
}

void CDlgAutoTerrain::SetLabelsText()
{
	ACHAR szName[100];
	swprintf(szName, _AL("%-4.2f"), m_fTextureRatio);
	m_pLabelTextureRatio->SetText(szName);

	swprintf(szName, _AL("%-4.2f"), m_fMainTexture);
	m_pLabelMainTexture->SetText(szName);

	swprintf(szName, _AL("%-4.2f"), m_fPatchTexture);
	m_pLabelPatchTexture->SetText(szName);

	swprintf(szName, _AL("%f"), m_fPatchDivision);
	m_pLabelPatchDivision->SetText(szName);

	

	swprintf(szName, _AL("%d"), m_nMaxHeight);
	m_pLabelMaxHeight->SetText(szName);

	swprintf(szName, _AL("%d"), m_nMainSmooth);
	m_pLabelMainSmooth->SetText(szName);
	
	swprintf(szName, _AL("%-4.2f"), m_nPartSmooth * 0.1f);
	m_pLabelPartSmooth->SetText(szName);

	swprintf(szName, _AL("%d"), m_nTerrainSeed);
	m_pLabelTerrainSeed->SetText(szName);

}

void CDlgAutoTerrain::OnCommandCreateTerrain(const char* szCommand)
{
	UpdateData(true);

	CAutoScene* pAutoScene = GetAutoScene();
	
	CAutoTerrain* pAutoTerrain = pAutoScene->GetAutoTerrain();

	pAutoTerrain->CreateTerrain((float)m_nMaxHeight, m_nMainSmooth, (float)(m_nPartSmooth * 0.1f), 
		m_fTextureRatio,
		m_fMainTexture, 
		m_fPatchTexture, 
		m_fPatchDivision,
		m_nTerrainSeed);

	pAutoScene->GetAutoTerrainAction()->AddActionBaseTerrain((float)m_nMaxHeight, m_nMainSmooth, (float)(m_nPartSmooth * 0.1f), 
		m_fTextureRatio, 
		m_fMainTexture, 
		m_fPatchTexture, 
		m_fPatchDivision,
		m_nTerrainSeed);

	pAutoScene->RecreateTerrainRender(true);

	pAutoTerrain->Tick(0);// Update terrain height
	CAutoBuildingOperation* pBuildingOperation = pAutoScene->GetAutoBuildingOperation();
	pBuildingOperation->AdjustModelsAfterTerrainChange();
		
}

void CDlgAutoTerrain::OnCommandTurnToBase(const char* szCommand)
{
	this->Show(false);
	GetHomeDlgsMgr()->SetUnAdvDlgsEnable();
	
	
}
*/
//void CDlgAutoTerrain::InitSettings(	float		i_fMaxHeight, 
//									int			i_iMainSmooth, 
//									float		i_fPartSmooth, 
//									int			i_iTerrainSeed, 
//									float		i_fTextureRatio,
//									float		i_fMainTexture,
//									float		i_fPatchTexture,
//									float		i_fPatchDivision,
//									bool i_bSaveEnable/* = false */,		//保存是否可用
//									const char* i_szFileName /* = NULL*/) //如果可以保存，指定文件名
																		//如果i_bSaveEnable 为true ,i_szFileName指定文件名
																		//否则,如果i_bSaveEnable 为 false, i_szFileName 为Null
/*{
	//确保数据可用
	int iMaxHeight = int(i_fMaxHeight);
	Clamp(iMaxHeight, MAX_HEIGHT, MIN_HEIGHT);
	Clamp(i_iMainSmooth, MAX_MAINSMOOTH, MIN_MAINSMOOTH);
	int iPartSmooth = int(i_fPartSmooth * 10.0f);
	Clamp(iPartSmooth, MAX_PARTSMOOTH, MIN_PARTSMOOTH);
	Clamp(i_iTerrainSeed, MAX_SEED, MIN_SEED);

	//纹理相关数据
	int iTextureRatio = int((i_fTextureRatio * 100.0f) + 100.0f);
	Clamp(iTextureRatio, MAX_TEXTURE_RATIO, MIN_TEXTURE_RATIO);
	int iMainTexture = int(i_fMainTexture * 100.0f);
	Clamp(iMainTexture, MAX_MAIN_TEXTURE, MIN_MAIN_TEXTURE);
	int iPatchTexture = int(i_fPatchTexture * 100.0f);
	Clamp(iPatchTexture, MAX_PATCH_TEXTURE, MIN_PATCH_TEXTURE);
	int iPatchDivision = int(i_fPatchDivision);
	Clamp(iPatchDivision, MAX_PATCH_DIVISION, MIN_PATCH_DIVISION);

	//赋值
	m_nMaxHeight = iMaxHeight;
	m_nMainSmooth = i_iMainSmooth;
	m_nPartSmooth = iPartSmooth;
	m_nTerrainSeed = i_iTerrainSeed;

	m_nTextureRatio = iTextureRatio;
	m_nMainTexture = iMainTexture;
	m_nPatchTexture = iPatchTexture;
	m_nPatchDivision = iPatchDivision;


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

void CDlgAutoTerrain::OnCommandSave(const char* szCommand)
{
	UpdateData(true);

	CAutoParameters *pParameters = GetAutoScene()->GetAutoParameters();
	CAutoParameters::MAINTERRAINHEIGHT param;
	
	param.fMaxHeight = float(m_nMaxHeight);
	param.nMainSmooth = m_nMainSmooth;
	param.fPartSmooth = float(m_nPartSmooth * 0.1f);
	param.dwSeed = m_nTerrainSeed;

	param.fTextureRatio = m_fTextureRatio;
	param.fMainTexture = m_fMainTexture;
	param.fPatchTexture = m_fPatchTexture;
	param.fPatchDivision = m_fPatchDivision;

	//创建目录
	ACHAR szPath[MAX_PATH];
	a_strcpy(szPath, _AL("userdata\\"));
	_wmkdir(szPath);

	a_strcat(szPath, _AL("home\\"));
	_wmkdir(szPath);

	a_strcat(szPath,_AL("terrain\\"));
	_wmkdir(szPath);

	//设置实际文件名
	char szFileName[MAX_PATH];
	strcpy(szFileName, AC2AS(szPath));
	strcat(szFileName, m_szFileName);


	//保存
	if(!pParameters->SaveMainTerrainHeight(szFileName, param))
		AutoHomeReport("CDlgAutoTerrain::OnCommandSave, Failed to SaveMainTerrainHeight!");

	this->Show(false);
	GetHomeDlgsMgr()->SetUnAdvDlgsEnable();
}

void CDlgAutoTerrain::OnCommandSaveAs(const char* szCommand)
{
	UpdateData(true);
	
	GetHomeDlgsMgr()->m_pDlgAutoBaseTerrain->InsertTerrainSettings(AS2AC(m_szFileName));
	OnCommandSave("");
	
}
*/
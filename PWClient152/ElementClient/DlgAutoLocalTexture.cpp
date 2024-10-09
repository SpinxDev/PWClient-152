/*
 * FILE: DlgAutoLocalTexture.cpp
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
#include "DlgAutoLocalTexture.h"
#include "DlgAutoBaseLocalTex.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoChooseTexture.h"
#include "DlgAutoAllRes.h"

#include "aui/AUILabel.h"
#include "aui/AUISlider.h"
#include "aui/AUIManager.h"
#include "aui/AUIStillImageButton.h"

#include "AutoTerrainCommon.h"
#include "AutoTerrainCommon.h"
#include "AutoTerrainAction.h"

#include "A3DEngine.h"
#include "A3DViewport.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "AutoScene.h"
#include "AutoTerrain.h"
#include "AutoPartTexture.h"		// Added by jdl, 2006.03.03
#include "AutoSelectedArea.h"

#include "windowsx.h"

#include "A3DMacros.h"

#include "DlgAutoSliderBound.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoLocalTexture, CDlgHomeBase)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)

AUI_ON_COMMAND("CreateLocalTex", OnCommandCreateLocalTex)

AUI_ON_COMMAND("TurnToBase", OnCommandTurnToBase)

AUI_ON_COMMAND("Save", OnCommandSave)
AUI_ON_COMMAND("SaveAs", OnCommandSaveAs)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoLocalTexture, CDlgHomeBase)

AUI_END_EVENT_MAP()

CDlgAutoLocalTexture::CDlgAutoLocalTexture()
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

	
	m_pSliderLocalTexBorderScope = NULL;
	m_pSliderSeedLocalTexture = NULL;

	m_pLabelLocalTexBorderScope = NULL;
	m_pLabelSeedLocalTexture = NULL;

	m_nLocalTexBorderScope = 0;
	m_fLocalTexBorderScope = 0.0f;
	m_nSeedLocalTexture = 0;

	m_nRadioTexture = 0;
	m_pBtnCreate = NULL;

	for(int i = 0; i < BTN_NUM ; i++)
	{
		m_pBtn[i] = NULL;
	}
	memset(m_szFileName, 0, MAX_PATH);
}

CDlgAutoLocalTexture::~CDlgAutoLocalTexture()
{
}

bool CDlgAutoLocalTexture::OnInitContext()
{
	SetSliderBound();
	LoadDefault();
	UpdateData(false);
	SetLabelsText();

	return true;
}

void CDlgAutoLocalTexture::OnShowDialog()
{
	CDlgHomeBase::OnShowDialog();

	SetCanMove(false);
}


#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);
void CDlgAutoLocalTexture::DoDataExchange(bool bSave)
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

	
	DDX_Control("Slider_LocalTexBorderScope", m_pSliderLocalTexBorderScope);
	DDX_Control("Slider_SeedLocalTexture", m_pSliderSeedLocalTexture);

	DDX_Control("Label_LocalTexBorderScope", m_pLabelLocalTexBorderScope);
	DDX_Control("Label_SeedLocalTexture", m_pLabelSeedLocalTexture);

	DDX_Slider(bSave, "Slider_LocalTexBorderScope", m_nLocalTexBorderScope);
	m_fLocalTexBorderScope = float(m_nLocalTexBorderScope * 0.01f);
	DDX_Slider(bSave, "Slider_SeedLocalTexture", m_nSeedLocalTexture);

	DDX_RadioButton(bSave, 1, m_nRadioTexture);

	DDX_Control("Btn_Create", m_pBtnCreate);
	
	GET_BTN_POINTER(BTN_SAVE);
	GET_BTN_POINTER(BTN_SAVE_AS);
}

void CDlgAutoLocalTexture::SetSliderBound()
{
	m_pSliderTextureRatio->SetTotal(MAX_TEXTURE_RATIO, MIN_TEXTURE_RATIO);
	m_pSliderMainTexture->SetTotal(MAX_MAIN_TEXTURE, MIN_MAIN_TEXTURE);
	m_pSliderPatchTexture->SetTotal(MAX_PATCH_TEXTURE, MIN_PATCH_TEXTURE);
	m_pSliderPatchDivision->SetTotal(MAX_PATCH_DIVISION, MIN_PATCH_DIVISION);

	m_pSliderLocalTexBorderScope->SetTotal(MAX_BORDER_SCOPE_LOCAL_TEX, MIN_BORDER_SCOPE_LOCAL_TEX);
	m_pSliderSeedLocalTexture->SetTotal(MAX_SEED_LOCAL_TEX, MIN_SEED_LOCAL_TEX);
}

void CDlgAutoLocalTexture::LoadDefault()
{
	m_nTextureRatio = 120;
	m_nMainTexture = 40;
	m_nPatchTexture = 120;
	m_nPatchDivision = 30;

	m_nLocalTexBorderScope = 50;
	m_nSeedLocalTexture = 1;
	m_nRadioTexture = 2;

	GetHomeDlgsMgr()->LoadTextureParamInfo(m_texParamInfo, TT_PART, TT_PART_PATCH);
}

void CDlgAutoLocalTexture::SetLabelsText()
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

	
	swprintf(szName, _AL("%-4.2f"), m_fLocalTexBorderScope);
	m_pLabelLocalTexBorderScope->SetText(szName);

	swprintf(szName, _AL("%d"), m_nSeedLocalTexture);
	m_pLabelSeedLocalTexture->SetText(szName);
}

void CDlgAutoLocalTexture::OnCommandMoveSlider(const char * szCommand)
{
	UpdateData(true);
	SetLabelsText();
}


void CDlgAutoLocalTexture::OnCommandCreateLocalTex(const char* szCommand)
{
	UpdateData(true);

	if (GetOperationType() != OT_TEXTURE_POLY)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CAutoScene* pScene = GetAutoScene();
	CAutoPartTexture* pPartTexture = pScene->GetAutoPartTexture();
	if (NULL == pPartTexture)
		return;

	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumPolyEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetPolyEditPoints();
	
	
	pPartTexture->SetAreaPoints(nNum, pPoints);

	bool bClosed = pSelArea->GetPolyEditAreaClosed();
	if (!bClosed)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	
	if (!pPartTexture->CreatePartTexture(
		m_fTextureRatio,
		m_fMainTexture,
		m_fPatchTexture,
		m_fPatchDivision,
		m_nSeedLocalTexture,
		m_fLocalTexBorderScope,
		m_nRadioTexture))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	// Store action
	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();
	CClosedArea* pArea = pScene->GetAutoPartTexture()->GetArea();

	if( GetHomeDlgsMgr()->GetModifying())
	{

		int nIdx = GetHomeDlgsMgr()->m_pDlgAutoAllRes->GetTreeViewSelItemIdx();
		if (!pAction->ChangeActionPartTexture(
						m_fTextureRatio,
						m_fMainTexture, 
						m_fPatchTexture,
						m_fPatchDivision,
						m_nSeedLocalTexture,
						(float)(m_nLocalTexBorderScope * 0.01f),
						pArea->GetNumPoints(),
						pArea->GetFinalPoints(),
						m_nRadioTexture, 
						nIdx))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		
		GetAutoScene()->RefreshAllPartTextureActions();
	}
	else
	{
		if (!pAction->AddActionPartTexture(
				m_fTextureRatio,
				m_fMainTexture, 
				m_fPatchTexture,
				m_fPatchDivision,
				m_nSeedLocalTexture,
				(float)(m_nLocalTexBorderScope * 0.01f),
				pArea->GetNumPoints(),
				pArea->GetFinalPoints(),
				m_nRadioTexture))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		GetAutoScene()->RecreateTerrainRender(true);
	}

	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	if (!pAction->AddActionPartTextureInfo(
			m_texParamInfo.fMainTextureU,
			m_texParamInfo.fMainTextureV,
			m_texParamInfo.fPatchTextureU,
			m_texParamInfo.fPatchTextureV,
			pTexture->GetTextureID(TT_PART),
			pTexture->GetTextureID(TT_PART_PATCH)))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

		GetAutoScene()->RecreateTerrainRender(true);

	pSelArea->CopyPointsFromPolyEditToView();
	pSelArea->SetViewAreaClosed(bClosed);
	pSelArea->DeleteAllPolyEditPoints();

	//Add Res item
	if( !GetHomeDlgsMgr()->GetModifying())
	{
		ACHAR szName[100];

		int nCount = GetHomeDlgsMgr()->m_pDlgAutoBaseLocalTex->GetActionCount();
		
		a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1007), nCount );

		GetHomeDlgsMgr()->m_pDlgAutoBaseLocalTex->SetActionCount(nCount + 1);
		
		GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_LOCALTEX, szName);
	}
	else
	{
		GetHomeDlgsMgr()->SetModifying(false);
		SetBtnModifyToCreate();
		
	}

	//设置纹理操作可用
	GetHomeDlgsMgr()->m_pDlgAutoBaseLocalTex->SetTexOpEnable(true);
	
}


void CDlgAutoLocalTexture::OnCommandTurnToBase(const char* szCommand)
{
	this->Show(false);
	GetHomeDlgsMgr()->SetUnAdvDlgsEnable();
}

void CDlgAutoLocalTexture::SetBtnCreateToModify()
{
	m_pBtnCreate->SetText( GetStringFromTable(1008) );
	return;
}

void CDlgAutoLocalTexture::SetBtnModifyToCreate()
{
	m_pBtnCreate->SetText( GetStringFromTable(1009) );
	return;
}
*/
//void CDlgAutoLocalTexture::InitSettings(float		i_fBorderScope,
//										int			i_iLocalTexSeed, 
//										int			i_iRadioTex, 
//										float		i_fTextureRatio,
//										float		i_fMainTexture,
//										float		i_fPatchTexture,
//										float		i_fPatchDivision,
//										bool		i_bSaveEnable /* = false */,
//										const char* i_szFileName /* = NULL */)
/*{
		//确保数据可用
	int iBorderscope = int(i_fBorderScope * 100.0f);
	Clamp(iBorderscope, MAX_BORDER_SCOPE_LOCAL_TEX, MIN_BORDER_SCOPE_LOCAL_TEX);
	Clamp(i_iLocalTexSeed, MAX_SEED_LOCAL_TEX, MIN_SEED_LOCAL_TEX);
	Clamp(i_iRadioTex, MAX_RADIO_LOCAL_TEX, MIN_RADIO_LOCAL_TEX);

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
	m_nLocalTexBorderScope = iBorderscope;
	m_nSeedLocalTexture = i_iLocalTexSeed;
	m_nRadioTexture = i_iRadioTex;

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

void CDlgAutoLocalTexture::OnCommandSave(const char* szCommand)
{
	UpdateData(true);

	CAutoParameters *pParameters = GetAutoScene()->GetAutoParameters();
	CAutoParameters::PARTTEXTUREINFO param;
	
	param.fBorderScope = m_fLocalTexBorderScope;
	param.nTextureType = m_nRadioTexture;
	param.dwSeed = m_nSeedLocalTexture;

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

	a_strcat(szPath,_AL("LocalTex\\"));
	_wmkdir(szPath);

	//设置实际文件名
	char szFileName[MAX_PATH];
	strcpy(szFileName, AC2AS(szPath));
	strcat(szFileName, m_szFileName);


	//保存
	if(!pParameters->SavePartTextureInfo(szFileName, param))
		AutoHomeReport("CDlgAutoTerrain::OnCommandSave, Failed to SaveMainTerrainHeight!");

	this->Show(false);

	GetHomeDlgsMgr()->SetUnAdvDlgsEnable();
}

void CDlgAutoLocalTexture::OnCommandSaveAs(const char* szCommand)
{
	UpdateData(true);
	
	GetHomeDlgsMgr()->m_pDlgAutoBaseLocalTex->InsertLocalTexSettings(AS2AC(m_szFileName));
	OnCommandSave("");
	
}
*/
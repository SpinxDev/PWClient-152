/*
 * FILE: DlgAutoBaseWater.cpp
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
#include "DlgAutoBaseWater.h"
#include "DlgAutoWater.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoSwitchDlg.h"
#include "DlgAutoChooseTexture.h"


#include "aui\\AUIListBox.h"
#include "aui\\AUISlider.h"
#include "aui\\AUILabel.h"
#include "aui\\AUIStillImageButton.h"
#include "aui\\AUIManager.h"
#include "A3DMacros.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "AutoScene.h"
#include "AutoTerrain.h"
#include "AutoWater.h"
#include "AutoParameters.h"
#include "AutoTerrainAction.h"
#include "A3DEngine.h"
#include "A3DViewport.h"

#include "windowsx.h"
#include "io.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoBaseWater, CDlgHomeBase)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
AUI_ON_COMMAND("CreateWater", OnCommandCreateWater)


AUI_ON_COMMAND("ApplyTex", OnCommandApplyTex)
AUI_ON_COMMAND("AdvTex", OnCommandAdvTex)
AUI_ON_COMMAND("DelTex", OnCommandDelTex)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoBaseWater, CDlgHomeBase)
AUI_ON_EVENT("ListBox_Texture", WM_LBUTTONUP, OnLButtonUpListBoxTex)

AUI_END_EVENT_MAP()

CDlgAutoBaseWater::CDlgAutoBaseWater()
{
	m_pListBoxTexture = NULL;
	m_nTextureIdx = 0;

	m_pSliderWaterHeight = NULL;
	m_pSliderSandWidth = NULL;

	m_pLabelWaterHeight = NULL;
	m_pLabelSandWidth = NULL;

	m_nWaterHeight = 0;
	m_nSandWidth = 0;

	m_iPreTexNum = 0;
	for(int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
}

CDlgAutoBaseWater::~CDlgAutoBaseWater()
{
}
	
AUIStillImageButton * CDlgAutoBaseWater::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnBaseWater;
}

void CDlgAutoBaseWater::OnShowDialog()
{
	SetCanMove(false);
	UpdateTerrainHeight();
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoBaseWater::OnInitContext()
{
	if( !InitResource())
		return false;
	SetSliderBound();
	
	LoadDefault();
	SetLabelsText();

	UpdateData(false);
	return true;
}


#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);
void CDlgAutoBaseWater::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_Texture", m_pListBoxTexture);
	DDX_ListBox(bSave, "ListBox_Texture", m_nTextureIdx);

	DDX_Control("Slider_WaterHeight", m_pSliderWaterHeight);
	DDX_Control("Slider_SandWidth", m_pSliderSandWidth);
	
	DDX_Control("Label_WaterHeight", m_pLabelWaterHeight);
	DDX_Control("Label_SandWidth", m_pLabelSandWidth);

	DDX_Slider(bSave, "Slider_WaterHeight", m_nWaterHeight);
	DDX_Slider(bSave, "Slider_SandWidth", m_nSandWidth);

	GET_BTN_POINTER(BTN_APPLY_TEX);
	GET_BTN_POINTER(BTN_ADV_TEX);
	GET_BTN_POINTER(BTN_DEL_TEX);
	
}	

void CDlgAutoBaseWater::LoadDefault()
{
	m_nTextureIdx = 0;

	m_pBtn[BTN_APPLY_TEX]->Enable(true);
	m_pBtn[BTN_ADV_TEX]->Enable(true);
	m_pBtn[BTN_DEL_TEX]->Enable(false);
}
bool CDlgAutoBaseWater::InitResource()
{
	FillTextureListBox();
	return true;

}
void CDlgAutoBaseWater::FillTextureListBox()
{
	m_pListBoxTexture->ResetContent();
	CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetTextureSet();

	if( pInfo)
	{
		m_iPreTexNum = GetAutoParam()->GetNumTextureSet();
		for( int i = 0; i < m_iPreTexNum ; i++)
		{
			m_pListBoxTexture->InsertString(i, AS2AC(pInfo[i].strName));
		}
	}

	//自定义数据
	_finddata_t fdIni;//文件属性
	long lFileHandle = _findfirst( "userdata\\home\\watertex\\*.*", &fdIni);//获得第一个文件的句柄
	if( lFileHandle == -1)
	{
		return ;
	}

	int nResult = 0;//保存findnext的返回值
	while( nResult != -1)
	{
		//如果是当前是目录文件,则跳过
		if( fdIni.attrib & 0x10)//16表示目录(0x10)
		{
			nResult = _findnext( lFileHandle, &fdIni);
			continue;
		}
	
		//在listbox 
		m_pListBoxTexture->InsertString(m_pListBoxTexture->GetCount(), AS2AC(fdIni.name));
		
		//获取下一个用户自定义数据文件的句柄
		nResult = _findnext( lFileHandle, &fdIni);
	}
	
	//关闭句柄
	_findclose( lFileHandle);
}

bool CDlgAutoBaseWater::GetTextureParameter(int a_nIdx, CAutoParameters::TEXTUREINFO* a_pTexInfo)
{
	UpdateData(true);
	
	if( !IsValidIdx(a_nIdx, m_pListBoxTexture))
		return false;

	if( a_nIdx < m_iPreTexNum)
	{
		CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetTextureSet();
		if( !GetAutoParam()->LoadTextureInfo(pInfo[a_nIdx].strPath, a_pTexInfo))
			return false;

	}	
	else //用户数据
	{
		char szFileName[MAX_PATH];
		strcpy(szFileName, "userdata\\home\\watertex\\");
		strcat(szFileName, AC2AS(m_pListBoxTexture->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadTextureInfo(szFileName, a_pTexInfo))
			return false;
	}

	return true;
}

void CDlgAutoBaseWater::UpdateTerrainHeight()
{
	CAutoScene* pScene = GetAutoScene();
	CAutoTerrain* pTerrain = pScene->GetAutoTerrain();
	CAutoTerrain::VERTEXINFO minVertex, maxVertex;
	pTerrain->GetExtremeVertices(minVertex, maxVertex);

	int nMaxHeight = (int)(maxVertex.fHeight + 0.5);
	int nMinHeight = (int)(minVertex.fHeight + 0.5);

	m_pSliderWaterHeight->SetTotal(nMaxHeight, nMinHeight);
	m_nWaterHeight = nMinHeight;
	UpdateData(false);
	SetLabelsText();

}

void CDlgAutoBaseWater::OnCommandCreateWater(const char* szCommand)
{
	PROFILE_BEGIN();

	CAutoScene* pAutoScene = GetAutoScene();
	CAutoTerrain* pAutoTerrain = pAutoScene->GetAutoTerrain();
	
	CAutoWater* pAutoWater = pAutoScene->GetAutoWater();
	
	
	if(!pAutoWater->CreateWater((float)m_nWaterHeight, (float)m_nSandWidth, 20.0f))
	{
		a_LogOutput(1, "CDlgAutoTerrain::OnCommandCreateWater, Failed to create water!");
		AutoHomeAssert();
		return;
	}
	
	// Store action
	CAutoTerrainAction* pAction = pAutoScene->GetAutoTerrainAction();
	if (!pAction->AddActionWater((float)m_nWaterHeight, (float)m_nSandWidth, 20.0f))
	{
		a_LogOutput(1, "CDlgAutoTerrain::OnCommandCreateWater, Failed to AddActionWater!");
		AutoHomeAssert();
		return;
	}
	
	pAutoScene->RecreateTerrainRender(true);

	PROFILE_END();
}

void CDlgAutoBaseWater::OnCommandMoveSlider(const char * szCommand)
{
	UpdateData(true);
	SetLabelsText();
}

void CDlgAutoBaseWater::SetLabelsText()
{
	ACHAR szName[100];
	swprintf(szName, _AL("%d"), m_nWaterHeight);
	m_pLabelWaterHeight->SetText(szName);

	swprintf(szName, _AL("%d"), m_nSandWidth);
	m_pLabelSandWidth->SetText(szName);

}

void CDlgAutoBaseWater::SetSliderBound()
{
	m_pSliderWaterHeight->SetTotal(0, 0);
	m_pSliderSandWidth->SetTotal(200, 0);
}



void CDlgAutoBaseWater::OnCommandApplyTex(const char* szCommand)
{
	UpdateData(true);

	CAutoScene* pScene = GetAutoScene();
	
	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	
	CAutoParameters::TEXTUREINFO texInfo;

	if(!GetTextureParameter(m_nTextureIdx, &texInfo))
		AutoHomeReport("CDlgAutoBaseTerrain::OnCommandCreateTexture, Can not Get Texture Parameter!");

	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();
	
	pTexture->ChangeTextureParam(
		TT_RIVERBED,
		TT_RIVERSAND,
		texInfo.fMainTextureU,
		texInfo.fMainTextureV,
		texInfo.fPatchTextureU,
		texInfo.fPatchTextureV,
		texInfo.dwMainTextureID,
		texInfo.dwPatchTextureID);


	pAction->AddActionWaterTextureInfo(
		texInfo.fMainTextureU,
		texInfo.fMainTextureV,
		texInfo.fPatchTextureU,
		texInfo.fPatchTextureV,
		texInfo.dwMainTextureID,
		texInfo.dwPatchTextureID);

	if (!pScene->RecreateTerrainRender(true))
	{
		AutoHomeReport( "DlgAutoTerrain::OnCommandCreateTexture, Failed to recreate terrain render!");
		return;
	}
}

void CDlgAutoBaseWater::OnCommandDelTex(const char* szCommand)
{
	UpdateData(true);

	if(m_nTextureIdx < m_iPreTexNum || m_nTextureIdx >= m_pListBoxTexture->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\watertex\\"));
	a_strcat(szFileName, m_pListBoxTexture->GetText(m_nTextureIdx));

	//删除当前文件
	DeleteFile(szFileName);
	

	//删除当前listbox选项
	m_pListBoxTexture->DeleteString(m_nTextureIdx);
	
	//设置当前listbox选中
	if(m_nTextureIdx < m_pListBoxTexture->GetCount())
		m_pListBoxTexture->SetCurSel(m_nTextureIdx);
	else
	{
		m_nTextureIdx = m_pListBoxTexture->GetCount() -1;

		m_pListBoxTexture->SetCurSel(m_nTextureIdx);
		if( m_nTextureIdx == m_iPreTexNum -1)
		{
			m_pBtn[BTN_DEL_TEX]->Enable(false);
		}
	}
	UpdateData(false);
}

void CDlgAutoBaseWater::InsertTexSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxTexture->GetCount();
	m_pListBoxTexture->InsertString(iIdx, szFileName);
	
}
void CDlgAutoBaseWater::OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//判断是否点中列表框中的选项
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxTexture->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxTexture->GetCurSel();

	if( nCurSel >= 0 && nCurSel < m_iPreTexNum) // 预设
	{
		m_pBtn[BTN_APPLY_TEX]->Enable(true);
		m_pBtn[BTN_ADV_TEX]->Enable(true);
		m_pBtn[BTN_DEL_TEX]->Enable(false);
		
	}
	else if( nCurSel < m_pListBoxTexture->GetCount()) //自定义
	{
		m_pBtn[BTN_APPLY_TEX]->Enable(true);
		m_pBtn[BTN_ADV_TEX]->Enable(true);
		m_pBtn[BTN_DEL_TEX]->Enable(true);
	}
	else //点在空位置											
	{
		m_pBtn[BTN_APPLY_TEX]->Enable(false);
		m_pBtn[BTN_ADV_TEX]->Enable(false);
		m_pBtn[BTN_DEL_TEX]->Enable(false);
	}

	return;
}

void CDlgAutoBaseWater::OnCommandAdvTex(const char* szCommand)
{
	UpdateData(true);
	
	
	CAutoParameters::TEXTUREINFO texInfo;
	if( !GetTextureParameter(m_nTextureIdx, &texInfo))
			AutoHomeReport("CDlgAutoBaseWater::OnCommandAdvTex, Can not Get TextureParameter!");

	TextureParamInfo_t texParam;
	GetHomeDlgsMgr()->TransformTexInfoToTexParam(texInfo, texParam);

		//预设数据
	if( m_nTextureIdx < m_iPreTexNum)
	{
	
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_WATER);
	}
	else //自定义数据
	{
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_WATER,
													true, 
													AC2AS(m_pListBoxTexture->GetText(m_nTextureIdx)));
	}

	GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->Show(true);
	GetHomeDlgsMgr()->SetUnAdvDlgsDisable();

}
*/
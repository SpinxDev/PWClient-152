/*
 * FILE: DlgAutoChooseTexture.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoChooseTexture.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoTerrain.h"
#include "DlgAutoMountain.h"
#include "DlgAutoWater.h"
#include "DlgAutoRoad.h"
#include "DlgAutoLocalTexture.h"

#include "DlgAutoBaseTerrain.h"
#include "DlgAutoBaseMountain.h"
#include "DlgAutoBaseMountain2.h"
#include "DlgAutoBaseWater.h"
#include "DlgAutoBaseRoad.h"
#include "DlgAutoBaseLocalTex.h"

#include "aui\\AUILabel.h"
#include "aui\\AUIListBox.h"
#include "aui\\AUIImagePicture.h"
#include "aui\\AUIStillImageButton.h"

#include "A2DSprite.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "A3DSurfaceMan.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "AutoScene.h"
#include "AutoTerrain.h"
#include "AutoTerrainAction.h"

#include "WindowsX.h"
#include "DlgAutoSliderBound.h"
#include "io.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoChooseTexture, CDlgHomeBase)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
//AUI_ON_COMMAND("OK", OnCommandOK)
AUI_ON_COMMAND("Cancel", OnCommandCancel)

AUI_ON_COMMAND("Save", OnCommandSave)
AUI_ON_COMMAND("SaveAs", OnCommandSaveAs)
AUI_ON_COMMAND("ApplyTex", OnCommandApply)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoChooseTexture, CDlgHomeBase)

AUI_ON_EVENT("ListBox_MainTexture", WM_LBUTTONUP, OnLButtonUpListBoxMainTexture)
AUI_ON_EVENT("ListBox_PatchTexture", WM_LBUTTONUP, OnLButtonUpListBoxPatchTexture)

AUI_END_EVENT_MAP()

CDlgAutoChooseTexture::CDlgAutoChooseTexture()
{
	m_pSliderMainTextureU = NULL;
	m_pSliderMainTextureV = NULL;
	m_pSliderPatchTextureU = NULL;
	m_pSliderPatchTextureV = NULL;
	
	
	
	m_pLabelMainTextureU = NULL;
	m_pLabelMainTextureV = NULL;
	m_pLabelPatchTextureU = NULL;
	m_pLabelPatchTextureV = NULL;

	
	
	m_nMainTextureU = 0;
	m_nMainTextureV = 0;
	m_nPatchTextureU = 0;
	m_nPatchTextureV = 0;

	m_pListBoxPatchTexture = NULL;
	m_pListBoxMainTexture = NULL;

	m_nMainTexIdx = 0;
	m_nPatchTexIdx = 0;

	m_pTexInfo = NULL;
	m_nNumTexInfos = 0;

	m_pImagePicMainTexture = NULL;
	m_pImagePicPatchTexture = NULL;
	
	m_pA2DSpriteMainTexture = new A2DSprite;
	m_pA2DSpritePatchTexture = new A2DSprite;

	m_nTexInitParam = CTIP_NULL;

	//数据同步，保存，另存
	for( int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
	memset(m_szFileName, 0, MAX_PATH);

}

CDlgAutoChooseTexture::~CDlgAutoChooseTexture()
{
	
	if( m_pA2DSpriteMainTexture != NULL)
	{
		m_pA2DSpriteMainTexture->Release();
		delete m_pA2DSpriteMainTexture;
		m_pA2DSpriteMainTexture = NULL;
	}
	
	if( m_pA2DSpritePatchTexture != NULL)
	{

		m_pA2DSpritePatchTexture->Release();
		delete m_pA2DSpritePatchTexture;
		m_pA2DSpritePatchTexture = NULL;
	}
}

bool CDlgAutoChooseTexture::OnInitContext()
{
	ClearListBox();

	if( !InitResource())
		return false;

	SetSliderBound();
	LoadDefault();
	UpdateData(false);
	SetLabelsText();

	FillTextureParam(0, 0);


	return true;
}

void CDlgAutoChooseTexture::OnShowDialog()
{
	CDlgHomeBase::OnShowDialog();
	SetCanMove(false);
}

#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);
void CDlgAutoChooseTexture::DoDataExchange(bool bSave)
{
	DDX_Control("Slider_MainTextureU", m_pSliderMainTextureU);
	DDX_Control("Slider_MainTextureV", m_pSliderMainTextureV);
	DDX_Control("Slider_PatchTextureU", m_pSliderPatchTextureU);
	DDX_Control("Slider_PatchTextureV", m_pSliderPatchTextureV);

	DDX_Control("Label_MainTextureU", m_pLabelMainTextureU);
	DDX_Control("Label_MainTextureV", m_pLabelMainTextureV);
	DDX_Control("Label_PatchTextureU", m_pLabelPatchTextureU);
	DDX_Control("Label_PatchTextureV", m_pLabelPatchTextureV);

	DDX_Control("ListBox_MainTexture", m_pListBoxMainTexture);
	DDX_Control("ListBox_PatchTexture", m_pListBoxPatchTexture);

	DDX_ListBox(bSave, "ListBox_MainTexture", m_nMainTexIdx);
	DDX_ListBox(bSave, "ListBox_PatchTexture", m_nPatchTexIdx);

	DDX_Control("ImagePic_MainTexture", m_pImagePicMainTexture);
	DDX_Control("ImagePic_PatchTexture", m_pImagePicPatchTexture);	

	DDX_Slider(bSave, "Slider_MainTextureU", m_nMainTextureU);
	DDX_Slider(bSave, "Slider_MainTextureV", m_nMainTextureV);
	DDX_Slider(bSave, "Slider_PatchTextureU", m_nPatchTextureU);
	DDX_Slider(bSave, "Slider_PatchTextureV", m_nPatchTextureV);

	GET_BTN_POINTER(BTN_APPLY);
	GET_BTN_POINTER(BTN_SAVE);
	GET_BTN_POINTER(BTN_SAVE_AS);

}

void CDlgAutoChooseTexture::SetSliderBound()
{
	m_pSliderMainTextureU->SetTotal(MAX_TEXTURE_U, MIN_TEXTURE_U);
	m_pSliderMainTextureV->SetTotal(MAX_TEXTURE_V, MIN_TEXTURE_V);
	m_pSliderPatchTextureU->SetTotal(MAX_TEXTURE_U, MIN_TEXTURE_U);
	m_pSliderPatchTextureV->SetTotal(MAX_TEXTURE_V, MIN_TEXTURE_V);
}

void CDlgAutoChooseTexture::LoadDefault()
{

	m_nMainTextureU = 10;
	m_nMainTextureV = 10;
	m_nPatchTextureU = 10;
	m_nPatchTextureV = 10;
}

void CDlgAutoChooseTexture::SetLabelsText()
{
	ACHAR szName[100];

	swprintf(szName, _AL("%d"), m_nMainTextureU);
	m_pLabelMainTextureU->SetText(szName);

	swprintf(szName, _AL("%d"), m_nMainTextureV);
	m_pLabelMainTextureV->SetText(szName);

	swprintf(szName, _AL("%d"), m_nPatchTextureU);
	m_pLabelPatchTextureU->SetText(szName);

	swprintf(szName, _AL("%d"), m_nPatchTextureV);
	m_pLabelPatchTextureV->SetText(szName);

}

void CDlgAutoChooseTexture::OnCommandMoveSlider(const char * szCommand)
{
	UpdateData(true);
	SetLabelsText();
}

void CDlgAutoChooseTexture::OnCommandOK(const char* szCommand)
{
	UpdateData(true);

	int nMainTexSel = m_pListBoxMainTexture->GetCurSel();
	if( nMainTexSel < 0 || nMainTexSel > m_pListBoxMainTexture->GetCount())
	{
		this->Show(false);
		return;
	}

	int nPatchTexSel = m_pListBoxPatchTexture->GetCurSel();
	if( nPatchTexSel < 0 || nPatchTexSel > m_pListBoxPatchTexture->GetCount())
	{
		this->Show(false);
		return;
	}

	FillTextureParam(nMainTexSel, nPatchTexSel);
	
	
	switch(m_nTexInitParam)
	{
	case CTIP_TERRAIN:
		{
			TextureParamInfo_t* pTexParamInfo = GetHomeDlgsMgr()->GetAutoTerrainDlg()->GetTextureParamInfo();
			*pTexParamInfo = m_texParamInfo;
			if(!GetHomeDlgsMgr()->SaveTextureParamInfo(m_texParamInfo, TT_MAIN, TT_MAIN_PATCH))
			{
				a_LogOutput(1, "CDlgAutoChooseTexture::OnCommandOK(), Failed to Save Terrain TextureParamInfo!");
				AutoHomeAssert();
			}
		}
		break;
	case CTIP_HILL:
	case CTIP_HILL_LINE:
		{
			TextureParamInfo_t* pTexParamInfo = GetHomeDlgsMgr()->GetAutoMountainDlg()->GetTextureParamInfo();
			*pTexParamInfo = m_texParamInfo;
			if(!GetHomeDlgsMgr()->SaveTextureParamInfo(m_texParamInfo, TT_BRIDGE, TT_BRIDGE_PATCH))
			{
				a_LogOutput(1, "CDlgAutoChooseTexture::OnCommandOK(), Failed to Save Mountain TextureParamInfo!");
				AutoHomeAssert();
			}
		}
		break;
	case CTIP_WATER:
		{
			TextureParamInfo_t* pTexParamInfo = GetHomeDlgsMgr()->GetAutoWaterDlg()->GetTextureParamInfo();
			*pTexParamInfo = m_texParamInfo;
			if(!GetHomeDlgsMgr()->SaveTextureParamInfo(m_texParamInfo,TT_RIVERBED, TT_RIVERSAND))
			{
				a_LogOutput(1, "CDlgAutoChooseTexture::OnCommandOK(), Failed to Save water TextureParamInfo!");
				AutoHomeAssert();
			}
		}
		break;
	case CTIP_ROAD:
		{
			TextureParamInfo_t* pTexParamInfo = GetHomeDlgsMgr()->GetAutoRoadDlg()->GetTextureParamInfo();
			*pTexParamInfo = m_texParamInfo;
			if(!GetHomeDlgsMgr()->SaveTextureParamInfo(m_texParamInfo,TT_ROADBED, TT_ROAD))
			{
				a_LogOutput(1, "CDlgAutoChooseTexture::OnCommandOK(), Failed to Save Road TextureParamInfo!");
				AutoHomeAssert();
			}
		}
		break;
	case CTIP_LOCALTEX:
		{
			TextureParamInfo_t* pTexParamInfo = GetHomeDlgsMgr()->GetAutoLocalTextureDlg()->GetTextureParamInfo();
			*pTexParamInfo = m_texParamInfo;
			if(!GetHomeDlgsMgr()->SaveTextureParamInfo(m_texParamInfo,TT_PART, TT_PART_PATCH))
			{
				a_LogOutput(1, "CDlgAutoChooseTexture::OnCommandOK(), Failed to Save LocalTexture TextureParamInfo!");
				AutoHomeAssert();
			}
		}
		break;
	default:
		break;
	};
	

	this->Show(false);
	
}

void CDlgAutoChooseTexture::OnCommandCancel(const char* szCommand)
{
	this->Show(false);
	GetHomeDlgsMgr()->SetUnAdvDlgsEnable();
}

bool CDlgAutoChooseTexture::InitResource()
{
	CAutoTexture* pAutoTexture = GetAutoScene()->GetAutoTerrain()->GetTexture();

	m_pTexInfo = pAutoTexture->GetTextureInfos();
	if( !m_pTexInfo)
	{
		a_LogOutput(1, "CDlgAutoChooseTexture::InitResource(), Can not get CAutoTexture::TEXTURETYPEINFO !");
		return false;
	}

	m_nNumTexInfos = pAutoTexture->GetNumTextureInfos();

	if( m_nNumTexInfos < 0)
	{
		a_LogOutput(1, "CDlgAutoChooseTexture::InitResource(), Number of TextureInfos Less than 0 !");
		return false;
	}

	
	
	for( int i = 0; i < m_nNumTexInfos; ++i)
	{
		m_pListBoxMainTexture->InsertString(i, AS2AC(m_pTexInfo[i].strName));
		m_pListBoxMainTexture->SetItemData(i, m_pTexInfo->dwID);
		
		m_pListBoxPatchTexture->InsertString(i, AS2AC(m_pTexInfo[i].strName));
		m_pListBoxPatchTexture->SetItemData(i, m_pTexInfo->dwID);
	
	}

	g_pGame->GetA3DEngine()->GetA3DSurfaceMan()->SetFolderName(".\\");

	A3DRECT rect; 
	rect.left = rect.top = 0;
	rect.right = rect.bottom = 64;

	if(!m_pA2DSpriteMainTexture->Init(g_pGame->GetA3DDevice(), m_pTexInfo[0].strTexture, 64, 64, 0, 1, &rect))
	{
		a_LogOutput(1, "CDlgAutoChooseTexture::InitResource(), Can not Init A2DSprite!");

		AutoHomeAssert();
		return false;
		
	}
	m_pA2DSpriteMainTexture->SetSrcBlend(A3DBLEND_ONE);
	m_pA2DSpriteMainTexture->SetDestBlend(A3DBLEND_ZERO);
	
	if(!m_pA2DSpritePatchTexture->Init(g_pGame->GetA3DDevice(), m_pTexInfo[0].strTexture, 64, 64, 0, 1, &rect))
	{
		a_LogOutput(1, "CDlgAutoChooseTexture::InitResource(), Can not Init A2DSprite!");

		AutoHomeAssert();
		return false;
		
	}
	m_pA2DSpritePatchTexture->SetSrcBlend(A3DBLEND_ONE);
	m_pA2DSpritePatchTexture->SetDestBlend(A3DBLEND_ZERO);
		


	g_pGame->GetA3DEngine()->GetA3DSurfaceMan()->SetFolderName("surfaces");

	m_pImagePicMainTexture->SetCover(m_pA2DSpriteMainTexture, 0);
	m_pImagePicPatchTexture->SetCover(m_pA2DSpritePatchTexture, 0);
	

	
	return true;
}

void CDlgAutoChooseTexture::ClearListBox()
{
	if(m_pListBoxMainTexture)
		m_pListBoxMainTexture->ResetContent();
	if( m_pListBoxPatchTexture)
		m_pListBoxPatchTexture->ResetContent();

}

void CDlgAutoChooseTexture::OnLButtonUpListBoxMainTexture(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//判断是否点中列表框中的选项
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxMainTexture->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxMainTexture->GetCurSel();
	if(nCurSel < 0 || nCurSel > m_pListBoxMainTexture->GetCount())
		return;

	g_pGame->GetA3DEngine()->GetA3DSurfaceMan()->SetFolderName(".\\");

	if(!m_pA2DSpriteMainTexture->Init(g_pGame->GetA3DDevice(), m_pTexInfo[nCurSel].strTexture, 0))
	{
		a_LogOutput(1, "CDlgAutoChooseTexture::InitResource(), Can not Init A2DSprite!");

		AutoHomeAssert();
		return ;
		
	}
	g_pGame->GetA3DEngine()->GetA3DSurfaceMan()->SetFolderName("surfaces");

	m_pImagePicMainTexture->SetCover(m_pA2DSpriteMainTexture, 0);
		return ;
}

void CDlgAutoChooseTexture::OnLButtonUpListBoxPatchTexture(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//判断是否点中列表框中的选项
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxPatchTexture->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxPatchTexture->GetCurSel();
	if(nCurSel < 0 || nCurSel > m_pListBoxPatchTexture->GetCount())
		return;

	g_pGame->GetA3DEngine()->GetA3DSurfaceMan()->SetFolderName(".\\");
	if(!m_pA2DSpritePatchTexture->Init(g_pGame->GetA3DDevice(), m_pTexInfo[nCurSel].strTexture, 0))
	{
		a_LogOutput(1, "CDlgAutoChooseTexture::InitResource(), Can not Init A2DSprite!");

		AutoHomeAssert();
		return ;
		
	}
	g_pGame->GetA3DEngine()->GetA3DSurfaceMan()->SetFolderName("surfaces");

	m_pImagePicPatchTexture->SetCover(m_pA2DSpritePatchTexture, 0);

	return ;
}

void CDlgAutoChooseTexture::FillTextureParam(int nMainTexSel, int nPatchTexSel)
{
	
	m_texParamInfo.fMainTextureU = (float)m_nMainTextureU;
	m_texParamInfo.fMainTextureV = (float)m_nMainTextureV;
	m_texParamInfo.fPatchTextureU = (float)m_nPatchTextureU;
	m_texParamInfo.fPatchTextureV = (float)m_nPatchTextureV;

	m_texParamInfo.strMainTextureFile = m_pTexInfo[nMainTexSel].strTexture;
	m_texParamInfo.strPatchTextureFile = m_pTexInfo[nPatchTexSel].strTexture;
	
	m_texParamInfo.nMainTexID = m_pTexInfo[nMainTexSel].dwID;
	m_texParamInfo.nPatchTexID = m_pTexInfo[nPatchTexSel].dwID;

	m_texParamInfo.nMainTexIdx = nMainTexSel;
	m_texParamInfo.nPatchTexIdx = nPatchTexSel;
	
}
*/
//bool CDlgAutoChooseTexture::InitTextureParamInfo(const TextureParamInfo_t& param, 
//												 ChooseTexInitParam_e texInitParam,
//												 bool		i_bSaveEnable /*= false*/,
//												 const char* i_szFileName /*= NULL*/)
/*{
	m_texParamInfo = param;
	
	m_nMainTextureU = (int)(m_texParamInfo.fMainTextureU + 0.5f);
	m_nMainTextureV = (int)(m_texParamInfo.fMainTextureV + 0.5f);
	m_nPatchTextureU = (int)(m_texParamInfo.fPatchTextureU + 0.5f);
	m_nPatchTextureV = (int)(m_texParamInfo.fPatchTextureV + 0.5f);

	

	if( m_texParamInfo.nMainTexIdx >= 0 && m_texParamInfo.nMainTexIdx < m_pListBoxMainTexture->GetCount())
	{
		//m_pListBoxMainTexture->SetSel(m_texParamInfo.nMainTexIdx, true);
		m_nMainTexIdx = m_texParamInfo.nMainTexIdx;
	}
	else
	{
		return false;
	}

	if( m_texParamInfo.nPatchTexIdx >= 0 && m_texParamInfo.nPatchTexIdx < m_pListBoxPatchTexture->GetCount())
	{
		//m_pListBoxPatchTexture->SetSel(m_texParamInfo.nPatchTexIdx, true);
		m_nPatchTexIdx = m_texParamInfo.nPatchTexIdx;
	}
	else
	{
		return false;
	}

	UpdateData(false);
	SetLabelsText();

	g_pGame->GetA3DEngine()->GetA3DSurfaceMan()->SetFolderName(".\\");

	if(!m_pA2DSpriteMainTexture->Init(g_pGame->GetA3DDevice(), m_pTexInfo[m_nMainTexIdx].strTexture, 0))
	{
		a_LogOutput(1, "CDlgAutoChooseTexture::InitTextureParamInfo(), Can not Init A2DSprite!");

		AutoHomeAssert();
		return false;
		
	}

	if(!m_pA2DSpritePatchTexture->Init(g_pGame->GetA3DDevice(), m_pTexInfo[m_nPatchTexIdx].strTexture, 0))
	{
		a_LogOutput(1, "CDlgAutoChooseTexture::InitTextureParamInfo(), Can not Init A2DSprite!");

		AutoHomeAssert();
		return false;
		
	}

	g_pGame->GetA3DEngine()->GetA3DSurfaceMan()->SetFolderName("surfaces");

	m_pImagePicMainTexture->SetCover(m_pA2DSpriteMainTexture, 0);
	m_pImagePicPatchTexture->SetCover(m_pA2DSpritePatchTexture, 0);
	
	if(texInitParam == CTIP_NULL)
	{
		return false;
	}
	m_nTexInitParam = texInitParam;

	
	//确定保存，另存，和文件名
	m_pBtn[BTN_SAVE]->Enable(i_bSaveEnable);
	m_pBtn[BTN_SAVE_AS]->Enable(!i_bSaveEnable);

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

	return true;
	
}

void CDlgAutoChooseTexture::OnCommandSave(const char* szCommand)
{
	UpdateData(true);
	FillTextureParam(m_nMainTexIdx, m_nPatchTexIdx);
	CAutoParameters *pParameters = GetAutoScene()->GetAutoParameters();
	CAutoParameters::TEXTUREINFO texInfo;

	//转换数据
	GetHomeDlgsMgr()->TransformTexParamToTexInfo(m_texParamInfo, texInfo);
	
	//创建目录
	ACHAR szPath[MAX_PATH];
	a_strcpy(szPath, _AL("userdata\\"));
	_wmkdir(szPath);

	a_strcat(szPath, _AL("home\\"));
	_wmkdir(szPath);

	switch(m_nTexInitParam)
	{
	case CTIP_TERRAIN:
		{
			a_strcat(szPath,_AL("terraintex\\"));
			_wmkdir(szPath);
		}
		break;

	case CTIP_WATER:
		{
			a_strcat(szPath,_AL("watertex\\"));
			_wmkdir(szPath);
		}
		break;

	case CTIP_HILL:
	case CTIP_HILL_LINE:
		{
			a_strcat(szPath,_AL("hilltex\\"));
			_wmkdir(szPath);
		}
		break;

	case CTIP_LOCALTEX:
		{
			a_strcat(szPath,_AL("localtextex\\"));
			_wmkdir(szPath);

		}
		break;

	case CTIP_ROAD:
		{
			a_strcat(szPath,_AL("roadtex\\"));
			_wmkdir(szPath);

		}
		break;

	case CTIP_NULL:
		return;

	default:
		return;
	}

	//设置实际文件名
	char szFileName[MAX_PATH];
	strcpy(szFileName, AC2AS(szPath));
	strcat(szFileName, m_szFileName);


	//保存
	if(!pParameters->SaveTextureInfo(szFileName, texInfo))
		AutoHomeReport("CDlgAutoChooseTexture::OnCommandSave, Failed to SaveTextureInfo!");

	this->Show(false);
	GetHomeDlgsMgr()->SetUnAdvDlgsEnable();
}

void CDlgAutoChooseTexture::OnCommandSaveAs(const char* szCommand)
{
	UpdateData(true);

	OnCommandSave("");
	
	switch(m_nTexInitParam)
	{
	case CTIP_TERRAIN:
		{
			GetHomeDlgsMgr()->m_pDlgAutoBaseTerrain->InsertTexSettings(AS2AC(m_szFileName));
		}
		break;

	case CTIP_WATER:
		{
			GetHomeDlgsMgr()->m_pDlgAutoBaseWater->InsertTexSettings(AS2AC(m_szFileName));
		}
		break;

	case CTIP_HILL:
		{
			GetHomeDlgsMgr()->m_pDlgAutoBaseMountain->InsertTexSettings(AS2AC(m_szFileName));
		}
		break;
	case CTIP_HILL_LINE:
		{
			GetHomeDlgsMgr()->m_pDlgAutoBaseMountain2->InsertTexSettings(AS2AC(m_szFileName));
		}
		break;
	case CTIP_LOCALTEX:
		{
			GetHomeDlgsMgr()->m_pDlgAutoBaseLocalTex->InsertTexSettings(AS2AC(m_szFileName));
		}
		break;

	case CTIP_ROAD:
		{
			GetHomeDlgsMgr()->m_pDlgAutoBaseRoad->InsertTexSettings(AS2AC(m_szFileName));
		}
		break;

	case CTIP_NULL:
		return;

	default:
		return;
	}

}



void CDlgAutoChooseTexture::OnCommandApply(const char* szCommand)
{
	UpdateData(true);

	CAutoScene* pScene = GetAutoScene();
	
	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	
	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();

	FillTextureParam(m_nMainTexIdx, m_nPatchTexIdx);
	


	
	switch( m_nTexInitParam)
	{
		case CTIP_TERRAIN:
		{
			pTexture->ChangeTextureParam(
				TT_MAIN,
				TT_MAIN_PATCH,
				m_texParamInfo.fMainTextureU,
				m_texParamInfo.fMainTextureV,
				m_texParamInfo.fPatchTextureU,
				m_texParamInfo.fPatchTextureV,
				m_texParamInfo.nMainTexID,
				m_texParamInfo.nPatchTexID);
			pAction->AddActionBaseTextureInfo(
				m_texParamInfo.fMainTextureU,
				m_texParamInfo.fMainTextureV,
				m_texParamInfo.fPatchTextureU,
				m_texParamInfo.fPatchTextureV,
				m_texParamInfo.nMainTexID,
				m_texParamInfo.nPatchTexID);
		}
		break;

	case CTIP_WATER:
		{
			pTexture->ChangeTextureParam(
				TT_RIVERBED,
				TT_RIVERSAND,
				m_texParamInfo.fMainTextureU,
				m_texParamInfo.fMainTextureV,
				m_texParamInfo.fPatchTextureU,
				m_texParamInfo.fPatchTextureV,
				m_texParamInfo.nMainTexID,
				m_texParamInfo.nPatchTexID);

			pAction->AddActionWaterTextureInfo(
				m_texParamInfo.fMainTextureU,
				m_texParamInfo.fMainTextureV,
				m_texParamInfo.fPatchTextureU,
				m_texParamInfo.fPatchTextureV,
				m_texParamInfo.nMainTexID,
				m_texParamInfo.nPatchTexID);
			
		}
		break;
		
	case CTIP_HILL:
	case CTIP_HILL_LINE:
		{
			pTexture->ChangeTextureParam(
				TT_BRIDGE,
				TT_BRIDGE_PATCH,
				m_texParamInfo.fMainTextureU,
				m_texParamInfo.fMainTextureV,
				m_texParamInfo.fPatchTextureU,
				m_texParamInfo.fPatchTextureV,
				m_texParamInfo.nMainTexID,
				m_texParamInfo.nPatchTexID);

			pAction->AddActionHillTextureInfo(
				m_texParamInfo.fMainTextureU,
				m_texParamInfo.fMainTextureV,
				m_texParamInfo.fPatchTextureU,
				m_texParamInfo.fPatchTextureV,
				m_texParamInfo.nMainTexID,
				m_texParamInfo.nPatchTexID);
		
		}
		break;


	case CTIP_LOCALTEX:
		{
			pTexture->ChangeTextureParam(
				TT_PART,
				TT_PART_PATCH,
				m_texParamInfo.fMainTextureU,
				m_texParamInfo.fMainTextureV,
				m_texParamInfo.fPatchTextureU,
				m_texParamInfo.fPatchTextureV,
				m_texParamInfo.nMainTexID,
				m_texParamInfo.nPatchTexID);

			pAction->AddActionPartTextureInfo(
				m_texParamInfo.fMainTextureU,
				m_texParamInfo.fMainTextureV,
				m_texParamInfo.fPatchTextureU,
				m_texParamInfo.fPatchTextureV,
				m_texParamInfo.nMainTexID,
				m_texParamInfo.nPatchTexID);
		
		}
		break;

	case CTIP_ROAD:
		{
			pTexture->ChangeTextureParam(
				TT_ROADBED,
				TT_ROAD,
				m_texParamInfo.fMainTextureU,
				m_texParamInfo.fMainTextureV,
				m_texParamInfo.fPatchTextureU,
				m_texParamInfo.fPatchTextureV,
				m_texParamInfo.nMainTexID,
				m_texParamInfo.nPatchTexID);

			pAction->AddActionRoadTextureInfo(
				m_texParamInfo.fMainTextureU,
				m_texParamInfo.fMainTextureV,
				m_texParamInfo.fPatchTextureU,
				m_texParamInfo.fPatchTextureV,
				m_texParamInfo.nMainTexID,
				m_texParamInfo.nPatchTexID);
		}
		break;

	case CTIP_NULL:
		return;

	default:
		return;
	}
	

	if (!pScene->RecreateTerrainRender(true))
	{
		AutoHomeReport( "CDlgAutoChooseTexture::OnCommandApply, Failed to recreate terrain render!");
		return;
	}

	return;
}
*/
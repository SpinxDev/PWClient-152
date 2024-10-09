/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   17:50
	file name:	DlgSettingSystem.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "A3DDevice.h"
#include "EC_ShadowRender.h"
#include "DlgSettingSystem.h"
#include "AUIComboBox.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_GameUIMan.h"
#include "AUIStillImageButton.h"
#include "AUICheckBox.h"
#include <A3DTerrainWater.h>
#include <EC_FullGlowRender.h>
#include "EL_BackMusic.h"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgSettingSystem, CDlgSetting)

AUI_ON_COMMAND("total", OnCommandTotal)
AUI_ON_COMMAND("bright", OnCommandBright)
AUI_ON_COMMAND("water", OnCommandWater)
AUI_ON_COMMAND("music", OnCommandMusic)
AUI_ON_COMMAND("ambience", OnCommandSFX)
AUI_ON_COMMAND("res", OnCommandRes)
AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)


AUI_END_COMMAND_MAP()


CDlgSettingSystem::CDlgSettingSystem()
{
	m_pComboRes = NULL;
	m_pComboFS = NULL;
	m_pLabelAdvanceWater = NULL;
	m_pSldBright = NULL;
	m_pSldWater = NULL;
	m_pChkWideScreen = NULL;
	m_pLabelWideScreen = NULL;
	memset(&m_setting, 0, sizeof(m_setting));
}

CDlgSettingSystem::~CDlgSettingSystem()
{

}

bool CDlgSettingSystem::OnInitDialog()
{
	if (!CDlgSetting::OnInitDialog())
		return false;

	// init control
	RefillResCombo();
	FillFSCombo();

	// init diaplay
	UpdateView();

	return true;
}


void CDlgSettingSystem::SetToDefault()
{
	GetGame()->GetConfigs()->DefaultUserSettings(&m_setting, NULL, NULL);
	
	UpdateData(false);
	UpdateControlState();
	MakeChangeNow();
	
	ACString strText;
	strText.Format(_AL("%d"), m_pA3DDevice->GetGammaLevel());
	m_pSldBright->SetHint(strText);
}

bool CDlgSettingSystem::Validate()
{
	UpdateData(true);
	
	// 窗口模式下不能选择比当前屏幕更大的分辨率。
	if (!m_setting.bFullScreen && 
		(m_setting.iRndWidth > m_pA3DDevice->GetDesktopDisplayMode().nWidth ||
		m_setting.iRndHeight > m_pA3DDevice->GetDesktopDisplayMode().nHeight))
	{
		AUIDialog *pMsgBox = NULL;
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(299), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
		return false;
	}
	
	// 自定义的非标准窗口尺寸下，不能切换到全屏模式。
	if (m_setting.bFullScreen && 
		m_bIncludeCustomSize && 
		m_pComboRes->GetCurSel() == m_pComboRes->GetCount() - 1)
	{
		AUIDialog *pMsgBox = NULL;
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(513), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
		return false;
	}

	return true;
}


void CDlgSettingSystem::Apply()
{
	UpdateData(true);

	// save setting
	GetGame()->GetConfigs()->SetSystemSettings(m_setting);
	// there's some setting can not be changed by SetSystemSetting()
	// we do it manually
	MakeChangeNow();

	UpdateView();
}

void CDlgSettingSystem::DoDataExchange(bool bSave)
{
	CDlgSetting::DoDataExchange(bSave);

	DDX_Control("Combo_Res", m_pComboRes);
	DDX_Control("Combo_Full", m_pComboFS);
//	DDX_Control("lb_advwater", m_pLabelAdvanceWater);
	DDX_Control("Slider_Bright", m_pSldBright);
	DDX_Control("Slider_Water", m_pSldWater);
	DDX_Control("Chk_WideScr", m_pChkWideScreen);
	DDX_Control("WideScr", m_pLabelWideScreen);

	DDX_Slider(bSave, "Slider_Total", m_setting.nLevel);
	DDX_CheckBox(bSave, "Chk_WideScr", m_setting.bWideScreen);
	DDX_CheckBox(bSave, "Chk_UI", m_setting.bScaleUI);
	DDX_Slider(bSave, "Slider_Bright", m_setting.iGamma);
	DDX_Slider(bSave, "Slider_Range", m_setting.nSight);
	DDX_Slider(bSave, "Slider_Water", m_setting.nWaterEffect);
	DDX_Slider(bSave, "Slider_Tree", m_setting.nTreeDetail);
	DDX_Slider(bSave, "Slider_Grass", m_setting.nGrassDetail);
	DDX_Slider(bSave, "Slider_Cloud", m_setting.nCloudDetail);
	DDX_CheckBox(bSave, "Chk_Shade", m_setting.bShadow);
	DDX_CheckBox(bSave, "Chk_Mipmap", m_setting.bMipMapBias);
	DDX_CheckBox(bSave, "Chk_Fullglow", m_setting.bFullGlow);
	DDX_CheckBox(bSave, "Chk_Spacewarp", m_setting.bSpaceWarp);
	DDX_CheckBox(bSave, "Chk_Sunflare", m_setting.bSunFlare);
	DDX_CheckBox(bSave, "Chk_VerticalSync", m_setting.bVSync);
	DDX_Slider(bSave, "Slider_Music", m_setting.nMusicVol);
	DDX_Slider(bSave, "Slider_Ambience", m_setting.nSoundVol);

	DDX_RadioButton(bSave, 1, m_setting.iTheme);

	if( bSave )
	{
		int nIndex;
		DDX_RadioButton(bSave, 0, nIndex);
		m_setting.bAdvancedWater = nIndex == 0 ? true : false;
	}
	else
	{
		int nIndex = m_setting.bAdvancedWater ? 0 : 1;
		DDX_RadioButton(bSave, 0, nIndex);
	}
	if (bSave)
	{
		int nIndex = 0;
		DDX_ComboBox(bSave, "Combo_Res", nIndex);
		ComboIndex2WidthHeight(nIndex, m_setting.iRndWidth, m_setting.iRndHeight);
	}
	else
	{
		int nIndex = 0;
		WidthHeight2ComboIndex(nIndex, m_setting.iRndWidth, m_setting.iRndHeight);
		DDX_ComboBox(bSave, "Combo_Res", nIndex);
	}
	if (bSave)
	{
		int nIndex = 0;
		DDX_ComboBox(bSave, "Combo_Full", nIndex);
		m_setting.bFullScreen = nIndex == 1 ? true : false;
	}
	else
	{
		int nIndex = m_setting.bFullScreen ? 1 : 0;
		DDX_ComboBox(bSave, "Combo_Full", nIndex);
	}
	if (bSave)
	{
		int nLevel = 0;
		DDX_Slider(bSave, "Slider_Texture", nLevel);
		m_setting.iTexDetail = 2 - nLevel;
	}
	else
	{
		int nLevel = 2 - m_setting.iTexDetail;
		DDX_Slider(bSave, "Slider_Texture", nLevel);
	}
	if (bSave)
	{
		int nLevel = 0;
		DDX_Slider(bSave, "Slider_Terrain", nLevel);
		m_setting.bSimpleTerrain = nLevel == 0 ? true : false;
	}
	else
	{
		int nLevel = m_setting.bSimpleTerrain ? 0 : 1;
		DDX_Slider(bSave, "Slider_Terrain", nLevel);
	}
	if (bSave)
	{
		int nLevel = 0;
		DDX_Slider(bSave, "Slider_Sample", nLevel);
		m_setting.iSoundQuality = 2 - nLevel;
	}
	else
	{
		int nLevel = 2 - m_setting.iSoundQuality;
		DDX_Slider(bSave, "Slider_Sample", nLevel);
	}
}

void CDlgSettingSystem::UpdateView()
{
	RefillResCombo();

	// read setting
	m_setting = GetGame()->GetConfigs()->GetSystemSettings();
	
	// show setting
	UpdateData(false);
	UpdateControlState();	
	MakeChangeNow();
}



void CDlgSettingSystem::OnShowDialog()
{
	CDlgSetting::OnShowDialog();

	m_pBtnSystem->SetPushed(true);
	m_pBtnGame->SetPushed(false);
	m_pBtnVideo->SetPushed(false);
	m_pBtnQuickKey->SetPushed(false);

	UpdateView();
}

void CDlgSettingSystem::ComboIndex2WidthHeight(int nIndex, int &nWidth, int &nHeight)
{
	nWidth = m_vecRes[nIndex].nWidth;
	nHeight = m_vecRes[nIndex].nHeight;
}

void CDlgSettingSystem::WidthHeight2ComboIndex(int &nIndex, int nWidth, int nHeight)
{
	for (int i = 0; i < (int)m_vecRes.size(); i++)
	{
		if (m_vecRes[i].nWidth == nWidth &&
			m_vecRes[i].nHeight == nHeight)
		{
			nIndex = i;
			return;
		}
	}
	nIndex = 0;
}



void CDlgSettingSystem::RefillResCombo()
{
	m_vecRes.clear();
	m_pComboRes->ResetContent();

	m_bIncludeCustomSize = true;
	A3DDEVFMT devFmt = m_pA3DEngine->GetA3DDevice()->GetDevFormat();
	for (int i = 0; i < m_pA3DDevice->GetNumDisplayModes(); i++ )
	{
		const D3DDISPLAYMODE &dm = m_pA3DDevice->GetDisplayMode(i);
		if( dm.Width > 400 && dm.Height > 400 )
		{
			Res res = {dm.Width, dm.Height};
			m_vecRes.push_back(res);
			
			ACString strItem;
			strItem.Format(_AL("%dx%d"), dm.Width, dm.Height);
			m_pComboRes->AddString(strItem);

			if (dm.Width == (UINT)devFmt.nWidth && dm.Height == (UINT)devFmt.nHeight)
				m_bIncludeCustomSize = false;
		}
	}

	if (m_bIncludeCustomSize)
	{
		Res res = {devFmt.nWidth, devFmt.nHeight};
		m_vecRes.push_back(res);

		// 自定义尺寸
		m_pComboRes->AddString(GetGameUIMan()->GetStringFromTable(512));
	}
}

void CDlgSettingSystem::FillFSCombo()
{
	// 窗口模式
	m_pComboFS->AddString(GetGameUIMan()->GetStringFromTable(297));
	// 全屏模式
	m_pComboFS->AddString(GetGameUIMan()->GetStringFromTable(298));
}

void CDlgSettingSystem::OnCommandTotal(const char *szCommand)
{
	UpdateData(true);

	switch (m_setting.nLevel) {
	case 0:
		{
			m_setting.nSight = 0;
			m_setting.nWaterEffect = 0;
			if (GetGame()->GetShadowRender()->CanDoShadow())
			{
				m_setting.bSimpleTerrain = true;
				m_setting.bShadow = false;
				m_setting.nCloudDetail = 0;
			}
			m_setting.nTreeDetail = 0;
			m_setting.nGrassDetail = 0;
			m_setting.bMipMapBias = false;
			m_setting.bFullGlow = false;
			m_setting.bSpaceWarp = false;
			m_setting.bSunFlare = false;
			m_setting.bAdvancedWater = false;
			m_setting.bSimpleTerrain = true;
		}
		break;
	case 1:
		{
			m_setting.nSight = 1;
			if (GetWorld()->GetTerrainWater()->CanRelfect())
				m_setting.nWaterEffect = 1;
			if (GetGame()->GetShadowRender()->CanDoShadow())
			{
				m_setting.bSimpleTerrain = false;
				m_setting.bShadow = true;
				m_setting.nCloudDetail = 1;
			}
			m_setting.nTreeDetail = 1;
			m_setting.nGrassDetail = 1;
			m_setting.bMipMapBias = true;
			m_setting.bFullGlow = false;
			m_setting.bSpaceWarp = false;
			m_setting.bSunFlare = false;
			m_setting.bAdvancedWater = false;
			m_setting.bSimpleTerrain = false;
		}
		break;
	case 2:
		{
			m_setting.nSight = 2;
			if (GetGame()->GetGameRun()->GetWorld()->GetTerrainWater()->CanRelfect())
				m_setting.nWaterEffect = 1;
			if (GetGame()->GetShadowRender()->CanDoShadow())
			{
				m_setting.bSimpleTerrain = false;
				m_setting.bShadow = true;
				m_setting.nCloudDetail = 2;
			}
			m_setting.nTreeDetail = 2;
			m_setting.nGrassDetail = 2;
			m_setting.bMipMapBias = true;
			if (GetGame()->GetGameRun()->GetFullGlowRender()->CanDoFullGlow())
			{
				m_setting.bFullGlow = true;
				m_setting.bSpaceWarp = true;
				m_setting.bSunFlare = true;
				m_setting.bAdvancedWater = true;
			}
			m_setting.bSimpleTerrain = false;
		}
		break;
	case 3:
		{
			m_setting.nSight = 3;
			if (GetGame()->GetGameRun()->GetWorld()->GetTerrainWater()->CanRelfect())
				m_setting.nWaterEffect = 1;
			if (GetGame()->GetShadowRender()->CanDoShadow())
			{
				m_setting.bSimpleTerrain = false;
				m_setting.bShadow = true;
				m_setting.nCloudDetail = 3;
			}
			m_setting.nTreeDetail = 3;
			m_setting.nGrassDetail = 3;
			m_setting.bMipMapBias = true;
			if (GetGame()->GetGameRun()->GetFullGlowRender()->CanDoFullGlow())
			{
				m_setting.bFullGlow = true;
				m_setting.bSpaceWarp = true;
				m_setting.bSunFlare = true;
				m_setting.bAdvancedWater = true;
			}
			m_setting.bSimpleTerrain = false;
		}
		break;
	case 4:
		{
			m_setting.nSight = 4;
			if (GetGame()->GetGameRun()->GetWorld()->GetTerrainWater()->CanRelfect())
				m_setting.nWaterEffect = 2;
			if (GetGame()->GetShadowRender()->CanDoShadow())
			{
				m_setting.bSimpleTerrain = false;
				m_setting.bShadow = true;
				m_setting.nCloudDetail = 4;
			}
			m_setting.nTreeDetail = 4;
			m_setting.nGrassDetail = 4;
			m_setting.bMipMapBias = true;
			if (GetGame()->GetGameRun()->GetFullGlowRender()->CanDoFullGlow())
			{
				m_setting.bFullGlow = true;
				m_setting.bSpaceWarp = true;
				m_setting.bSunFlare = true;
				m_setting.bAdvancedWater = true;
			}
			m_setting.bSimpleTerrain = false;
		}
		break;
	}

	UpdateData(false);
}

void CDlgSettingSystem::MakeChangeNow()
{
	m_pA3DDevice->SetGammaLevel(int(float(m_setting.iGamma) * 1.7f + 30.0f));
	GetGame()->SetAllVolume(m_setting);
}

void CDlgSettingSystem::OnCommandBright(const char *szCommand)
{
	UpdateData(true);

	MakeChangeNow();

	ACString strText;
	strText.Format(_AL("%d"), m_pA3DDevice->GetGammaLevel());
	m_pSldBright->SetHint(strText);
}

void CDlgSettingSystem::OnCommandWater(const char *szCommand)
{
}

void CDlgSettingSystem::OnCommandMusic(const char *szCommand)
{
	UpdateData(true);
	MakeChangeNow();
}

void CDlgSettingSystem::OnCommandSFX(const char *szCommand)
{
	UpdateData(true);
	MakeChangeNow();
}


void CDlgSettingSystem::OnCommandRes(const char *szCommand)
{
	UpdateData(true);
	if (m_bIncludeCustomSize && m_pComboRes->GetCurSel() == (int)m_vecRes.size() - 1 ||
		m_setting.iRndWidth >= 800 && m_setting.iRndHeight >= 600)
	{
		m_pChkWideScreen->Enable(true);
		m_pChkWideScreen->SetHint(_AL(""));
		m_pLabelWideScreen->SetHint(_AL(""));
	}
	else
	{
		m_pChkWideScreen->Enable(false);
		m_setting.bWideScreen = false;
		m_pChkWideScreen->SetHint(GetGameUIMan()->GetStringFromTable(529));
		m_pLabelWideScreen->SetHint(GetGameUIMan()->GetStringFromTable(529));
		UpdateData(false);
	}
}

void CDlgSettingSystem::UpdateControlState()
{
	UpdateData(true);

	if( CECConfigs::CanUseWaterReflect() && !CECConfigs::ShouldForceWaterReflect() )
	{
		GetDlgItem("Water_1")->SetColor(A3DCOLORRGB(255, 255, 255));
		GetDlgItem("Water_2")->SetColor(A3DCOLORRGB(255, 255, 255));
		GetDlgItem("Water_3")->SetColor(A3DCOLORRGB(255, 255, 255));
		GetDlgItem("Slider_Water")->Enable(true);
	}
	else
	{
		GetDlgItem("Water_1")->SetColor(A3DCOLORRGB(128, 128, 128));
		GetDlgItem("Water_2")->SetColor(A3DCOLORRGB(128, 128, 128));
		GetDlgItem("Water_3")->SetColor(A3DCOLORRGB(128, 128, 128));
		GetDlgItem("Slider_Water")->Enable(false);
	}

	if( GetGame()->GetShadowRender()->CanDoShadow() )
	{
		GetDlgItem("Chk_Shade")->Enable(true);
		GetDlgItem("Shade")->SetColor(A3DCOLORRGB(255, 255, 255));
		GetDlgItem("Slider_Terrain")->Enable(true);
	}
	else
	{
		GetDlgItem("Chk_Shade")->Enable(false);
		GetDlgItem("Shade")->SetColor(A3DCOLORRGB(128, 128, 128));
		CheckRadioButton(2, 1);
		GetDlgItem("Slider_Terrain")->Enable(false);
		((PAUISLIDER)GetDlgItem("Slider_Cloud"))->SetLevel(0);
		GetDlgItem("Slider_Cloud")->Enable(false);
	}

	if( GetGameRun()->GetFullGlowRender()->CanDoFullGlow() )
	{
		GetDlgItem("Chk_Fullglow")->Enable(true);
		GetDlgItem("Chk_Spacewarp")->Enable(true);
		GetDlgItem("Chk_Sunflare")->Enable(true);
		GetDlgItem("Rdo_AdvancedWaterOpen")->Enable(!CECConfigs::ShouldForceWaterRefract());
		GetDlgItem("Rdo_AdvancedWaterClose")->Enable(!CECConfigs::ShouldForceWaterRefract());
		GetDlgItem("Fullglow")->SetColor(A3DCOLORRGB(255, 255, 255));
		GetDlgItem("Spacewarp")->SetColor(A3DCOLORRGB(255, 255, 255));
		GetDlgItem("Sunflare")->SetColor(A3DCOLORRGB(255, 255, 255));
		GetDlgItem("AdvancedWater")->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	else
	{
		GetDlgItem("Chk_Fullglow")->Enable(false);
		GetDlgItem("Chk_Sunflare")->Enable(false);
		GetDlgItem("Chk_Spacewarp")->Enable(false);
		GetDlgItem("Rdo_AdvancedWaterOpen")->Enable(false);
		GetDlgItem("Rdo_AdvancedWaterClose")->Enable(false);
		GetDlgItem("Fullglow")->SetColor(A3DCOLORRGB(128, 128, 128));
		GetDlgItem("Spacewarp")->SetColor(A3DCOLORRGB(128, 128, 128));
		GetDlgItem("Sunflare")->SetColor(A3DCOLORRGB(128, 128, 128));
		GetDlgItem("AdvancedWater")->SetColor(A3DCOLORRGB(128, 128, 128));
	}

	ACString strText;
	strText.Format(_AL("%d"), m_pA3DDevice->GetGammaLevel());
	m_pSldBright->SetHint(strText);

	if (m_bIncludeCustomSize && m_pComboRes->GetCurSel() == (int)m_vecRes.size() - 1 ||
		m_setting.iRndWidth >= 800 && m_setting.iRndHeight >= 600)
	{
		m_pChkWideScreen->Enable(true);
		m_pChkWideScreen->SetHint(_AL(""));
		m_pLabelWideScreen->SetHint(_AL(""));
	}
	else
	{
		m_pChkWideScreen->Enable(false);
		m_pChkWideScreen->SetHint(GetGameUIMan()->GetStringFromTable(529));
		m_pLabelWideScreen->SetHint(GetGameUIMan()->GetStringFromTable(529));
	}
}

void CDlgSettingSystem::OnCommandCancel(const char *szCommand)
{
	CDlgSetting::OnCommandCancel(NULL);
	
	UpdateView();
	MakeChangeNow();
}
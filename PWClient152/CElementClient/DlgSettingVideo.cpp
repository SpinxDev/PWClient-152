/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   17:49
	file name:	DlgSettingVideo.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "DlgSettingVideo.h"
#include "DlgOptimizeMem.h"
#include "DlgMiniMap.h"
#include "AUICheckBox.h"
#include "AUIStillImageButton.h"
#include "AUISlider.h"
#include "AUICheckBox.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "DlgHost.h"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgSettingVideo, CDlgSetting)

AUI_ON_COMMAND("modellimit", OnCommandModeLimit)
AUI_ON_COMMAND("speffect", OnCommandSpEffect)

AUI_END_COMMAND_MAP()


CDlgSettingVideo::CDlgSettingVideo()
{
	m_pSldModeLimit = NULL;
	m_pSldSpEffect = NULL;
	m_pChk_Personize = NULL;
	memset(&m_setting, 0, sizeof(m_setting));
}

CDlgSettingVideo::~CDlgSettingVideo()
{

}

void CDlgSettingVideo::DoDataExchange(bool bSave)
{
	CDlgSetting::DoDataExchange(bSave);

	DDX_Control("Slider_ModelLimit", m_pSldModeLimit);
	DDX_Control("Slider_SpEffect", m_pSldSpEffect);
	DDX_Control("Chk_Personize", m_pChk_Personize);

	DDX_CheckBox(bSave, "Chk_OtherPlayer", m_setting.bPlayerHeadText);
	DDX_CheckBox(bSave, "Chk_Name", m_setting.bPlayerName);
	DDX_CheckBox(bSave, "Chk_Title", m_setting.bPlayerTitle);
	DDX_CheckBox(bSave, "Chk_Insignia", m_setting.bPlayerFaction);
	DDX_CheckBox(bSave, "Chk_Player", m_setting.bPlayerShop);
	DDX_CheckBox(bSave, "Chk_Talk", m_setting.bPlayerTalk);
	DDX_CheckBox(bSave, "Chk_Talkbubble", m_setting.bPlayerBubble);
	DDX_CheckBox(bSave, "Chk_Monster", m_setting.bMonsterName);
	DDX_CheckBox(bSave, "Chk_NPC", m_setting.bNPCName);
	DDX_CheckBox(bSave, "Chk_SelfName", m_setting.bPlayerSelfName);
	DDX_CheckBox(bSave, "Chk_Item", m_setting.bGoodsName);
	DDX_CheckBox(bSave, "Chk_Personize", m_setting.bShowCustomize);
	DDX_CheckBox(bSave, "Chk_ModelLimit", m_setting.bModelLimit);
	DDX_Slider(bSave, "Slider_ModelLimit", m_setting.nDistance);
	DDX_CheckBox(bSave, "Chk_Force", m_setting.bPlayerForce);
	if (bSave)
	{
		int nLevel = 0;
		DDX_Slider(bSave, "Slider_SpEffect", nLevel);
		m_setting.nEffect = nLevel + 1;
	}
	else
	{
		int nLevel = m_setting.nEffect - 1;
		DDX_Slider(bSave, "Slider_SpEffect", nLevel);
	}
	DDX_CheckBox(bSave, "Chk_MapNPC", m_setting.bMapNPC);
	DDX_CheckBox(bSave, "Chk_MapMonster", m_setting.bMapMonster);
	DDX_CheckBox(bSave, "Chk_MapPlayer", m_setting.bMapPlayer);
}

bool CDlgSettingVideo::OnInitDialog()
{
	if (!CDlgSetting::OnInitDialog())
		return false;

	// init diaplay
	UpdateView();

	return true;
}

void CDlgSettingVideo::OnShowDialog()
{
	CDlgSetting::OnShowDialog();

	// init control
	m_pBtnSystem->SetPushed(false);
	m_pBtnGame->SetPushed(false);
	m_pBtnVideo->SetPushed(true);
	m_pBtnQuickKey->SetPushed(false);

	// init diaplay
	UpdateView();
}

void CDlgSettingVideo::SetToDefault()
{
	GetGame()->GetConfigs()->DefaultUserSettings(NULL, &m_setting, NULL);
	
	UpdateData(false);
}

void CDlgSettingVideo::Apply()
{
	UpdateData(true);
	
	// save setting
	GetGame()->GetConfigs()->SetVideoSettings(m_setting);

	// todo : the following lines should be rewritten
	PAUICHECKBOX pChk = NULL;
	if (pChk = GetGameUIMan()->m_pDlgMiniMap->m_pChk_MapMonster)
		pChk->Check(m_setting.bMapMonster);
	
	if (pChk = GetGameUIMan()->m_pDlgHost->m_pChk_Player)
		pChk->Check(m_setting.bPlayerHeadText);
	if (pChk = GetGameUIMan()->m_pDlgHost->m_pChk_Surround)
		pChk->Check(m_setting.bModelLimit);
	if (pChk = GetGameUIMan()->m_pDlgHost->m_pChk_Sight)
		pChk->Check(false);
}

void CDlgSettingVideo::UpdateView()
{
	// read setting
	m_setting = GetGame()->GetConfigs()->GetVideoSettings();

	// show setting
	UpdateData(false);
}


void CDlgSettingVideo::OnCommandModeLimit(const char *szCommand)
{
	UpdateData(true);

	ACString strText;
	strText.Format(GetGameUIMan()->GetStringFromTable(528), 
		static_cast<int>(GetGame()->GetConfigs()->CalcPlayerVisRadius(m_setting.nDistance) + 0.5f));
	m_pSldModeLimit->SetHint(strText);
}

void CDlgSettingVideo::OnCommandSpEffect(const char *szCommand)
{
	UpdateData(true);

	ACString strHint = GetStringFromTable(523 + m_setting.nEffect - 1);
	m_pSldSpEffect->SetHint(strHint);
}
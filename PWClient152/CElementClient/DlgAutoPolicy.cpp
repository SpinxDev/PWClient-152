// Filename	: DlgAutoPolicy.cpp
// Creator	: Shizhenhua
// Date		: 2013/8/30

#include "DlgAutoPolicy.h"
//#include "DlgSkill.h"
#include "DlgSkillSubOther.h"
#include "DlgAutoHPMP.h"
#include "EC_AutoPolicy.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_ShortcutSet.h"
#include "EC_Configs.h"
#include "EC_HostPlayer.h"
#include "EC_Skill.h"
#include "EC_ComputerAid.h"
#include "EC_Shortcut.h"
#include "EC_UIConfigs.h"
#include "EC_PlayerWrapper.h"

#include <AFI.h>
#include <AUICTranslate.h>

#define new A_DEBUG_NEW

static const int MIN_RANGE = 20;

AUI_BEGIN_COMMAND_MAP(CDlgAutoPolicy, CDlgBase)
AUI_ON_COMMAND("Btn_AutoOpen", OnCommand_StartOrStop)
AUI_ON_COMMAND("Btn_Confirm", OnCommand_Confirm)
AUI_ON_COMMAND("Btn_Cancel", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_AutoHPMP", OnCommand_SetAutoHPMP)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoPolicy, CDlgBase)
AUI_ON_EVENT("Img_Skill*", WM_LBUTTONDOWN, OnEvent_LButtonDown)
AUI_END_EVENT_MAP()

CDlgAutoPolicy::CDlgAutoPolicy()
{
	m_pImg_Skill1 = NULL;
	m_pImg_Skill2 = NULL;
	m_pTxt_Interval = NULL;
	m_pTxt_Time = NULL;
	m_pTxt_Range = NULL;
	m_pCmb_AutoPick = NULL;
	m_pBtn_Switch = NULL;
	m_pLbl_RemainTime = NULL;
}

bool CDlgAutoPolicy::OnInitDialog()
{
	DDX_Control("Img_Skill01", m_pImg_Skill1);
	DDX_Control("Img_Skill02", m_pImg_Skill2);
	DDX_Control("Edt_Time", m_pTxt_Interval);
	DDX_Control("Edt_Time2", m_pTxt_Time);
	DDX_Control("Edt_Range", m_pTxt_Range);
	DDX_Control("Combo_Filter", m_pCmb_AutoPick);
	DDX_Control("Btn_AutoOpen", m_pBtn_Switch);
	DDX_Control("Txt_Time", m_pLbl_RemainTime);

	for( int i=0;i<3;i++ )
	{
		m_pCmb_AutoPick->AddString(GetStringFromTable(10928 + i));
	}

	return true;
}

void CDlgAutoPolicy::OnShowDialog()
{
	if( !CECUIConfig::Instance().GetGameUI().bEnableAutoPolicy )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10931), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		Show(false);
		return;
	}

	const CECAutoPolicy::CONFIG& cfgData = CECAutoPolicy::GetInstance().GetConfigData();

	SetSkillImage(m_pImg_Skill1, cfgData.attack_skill, cfgData.attack_iscombo);
	SetSkillImage(m_pImg_Skill2, cfgData.assist_skill, cfgData.assist_iscombo);

	ACString strText;
	strText.Format(_AL("%d"), cfgData.nAssistInterval / 1000);
	m_pTxt_Interval->SetText(strText);
	strText.Format(_AL("%d"), cfgData.nTime / 60000);
	m_pTxt_Time->SetText(strText);
	strText.Format(_AL("%d"), cfgData.nPetrolRadius);
	m_pTxt_Range->SetText(strText);
	m_pCmb_AutoPick->SetCurSel(cfgData.iAutoPickMode);

	if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
		m_pBtn_Switch->SetText(GetStringFromTable(10925));
	else
		m_pBtn_Switch->SetText(GetStringFromTable(10924));
}

void CDlgAutoPolicy::OnTick()
{
	if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
		m_pBtn_Switch->SetText(GetStringFromTable(10925));
	else
		m_pBtn_Switch->SetText(GetStringFromTable(10924));

	m_pLbl_RemainTime->SetText(GetRemainTimeTxt());
	GetDlgItem("Btn_AutoHPMP")->Enable(CEComputerAid::Instance().CanUse());
}

void CDlgAutoPolicy::OnCommand_Confirm(const char * szCommand)
{
	SaveConfigData();
	Show(false);

	CECAutoPolicy& autoPolicy = CECAutoPolicy::GetInstance();
	if( autoPolicy.IsAutoPolicyEnabled() )
		autoPolicy.SendEvent_ConfigChanged();
}

void CDlgAutoPolicy::OnCommand_StartOrStop(const char * szCommand)
{
	if( !CECUIConfig::Instance().GetGameUI().bEnableAutoPolicy )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10931), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		Show(false);
		return;
	}

	if( !szCommand )
		OnShowDialog();

	if( !m_pImg_Skill1->GetData() )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10923), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	if( CECAutoPolicy::GetInstance().GetPlayerWrapper()->GetWeaponEndurance() == 0 )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10932), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	int iTime = a_atoi(m_pTxt_Time->GetText());
	if( iTime <= 0 )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10926), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	if( IsShow() )
		SaveConfigData();

	CECAutoPolicy& policy = CECAutoPolicy::GetInstance();
	if( policy.IsAutoPolicyEnabled() )
		policy.StopPolicy();
	else
		policy.StartPolicy(CECAutoPolicy::POLICY_AUTOKILLMONSTER);
}

void CDlgAutoPolicy::OnCommand_SetAutoHPMP(const char * szCommand)
{
	GetGameUIMan()->m_pDlgAutoHPMP->SwitchShow();
}

void CDlgAutoPolicy::OnEvent_LButtonDown(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	if( pObj == m_pImg_Skill1 )
	{
		m_pImg_Skill1->ClearCover();
		m_pImg_Skill1->SetData(0);
		m_pImg_Skill1->SetData64(0);
		m_pImg_Skill1->SetHint(_AL(""));
	}
	else if( pObj == m_pImg_Skill2 )
	{
		m_pImg_Skill2->ClearCover();
		m_pImg_Skill2->SetData(0);
		m_pImg_Skill2->SetData64(0);
		m_pImg_Skill2->SetHint(_AL(""));
	}
}

void CDlgAutoPolicy::DragDropItem(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if(strstr(pDlgSrc->GetName(), "Win_SkillSubListSkillItem")		// 20140708 新的技能学习对话框
		|| !stricmp(pDlgSrc->GetName(), "Win_SkillSubOther")		// 20140708 新的连锁技能对话框
		|| !stricmp(pDlgSrc->GetName(), "Win_SkillSubPool"))	
	{
		if( strstr(pObjSrc->GetName(), "Item") 
			|| strstr(pObjSrc->GetName(), "Img_ConSkill") )		// 20140708 新的连锁技能对话框
		{
			int nCombo = pObjSrc->GetData();
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSkillImg ) SetSkillImage(pSkillImg, nCombo - 1, true);
		}
		else
		{
			CECSkill* pSkill = (CECSkill*)pObjSrc->GetDataPtr("ptr_CECSkill");
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSkill && pSkillImg ) SetSkillImage(pSkillImg, pSkill->GetSkillID(), false);
		}
	}
	else if( strstr(pDlgSrc->GetName(), "Win_Quickbar") )
	{
		CECShortcut* pSC = (CECShortcut*)pObjSrc->GetDataPtr("ptr_CECShortcut");
		if( !pSC ) return;

		if( pSC->GetType() == CECShortcut::SCT_SKILL )
		{
			CECSCSkill* pSCSkill = dynamic_cast<CECSCSkill*>(pSC);
			CECSkill* pSkill = (CECSkill*)pSCSkill->GetSkill();
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSkill && pSkillImg ) SetSkillImage(pSkillImg, pSkill->GetSkillID(), false);
		}
		else if( pSC->GetType() == CECShortcut::SCT_SKILLGRP )
		{
			CECSCSkillGrp* pSCSkillGrp = dynamic_cast<CECSCSkillGrp*>(pSC);
			int nCombo = pSCSkillGrp->GetGroupIndex();
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSkillImg ) SetSkillImage(pSkillImg, nCombo, true);
		}
		else if( pSC->GetType() == CECShortcut::SCT_COMMAND )
		{
			CECSCCommand* pSCCommand = dynamic_cast<CECSCCommand*>(pSC);
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSCCommand->GetCommandID() == CECSCCommand::CMD_NORMALATTACK && pSkillImg )
				SetSkillImage(pSkillImg, -1, false);
		}
	}
	else if(!stricmp(pDlgSrc->GetName(), "Win_SkillSubAction") )		// 20140708 新的动作对话框
	{
		if( !stricmp(pObjSrc->GetName(), "BscCmd_03") )
		{
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSkillImg ) SetSkillImage(pSkillImg, -1, false);
		}
	}
}

void CDlgAutoPolicy::SaveConfigData()
{
	int nNum = 0;
	unsigned __int64 nTemp;
	CECAutoPolicy::CONFIG cfgData;

	cfgData.attack_iscombo = (m_pImg_Skill1->GetData() == 1);
	if( m_pImg_Skill1->GetData() == 3 )
		cfgData.attack_skill = -1;	// 普攻
	else
	{
		nTemp = m_pImg_Skill1->GetData64();
		cfgData.attack_skill = (int)nTemp;
	}

	cfgData.assist_iscombo = (m_pImg_Skill2->GetData() == 1);
	nTemp = m_pImg_Skill2->GetData64();
	cfgData.assist_skill = (int)nTemp;

	nNum = a_atoi(m_pTxt_Interval->GetText());
	a_Clamp(nNum, 1, 999999);
	cfgData.nAssistInterval = nNum * 1000;
	nNum = a_atoi(m_pTxt_Time->GetText());
	a_Clamp(nNum, 1, 9999);
	cfgData.nTime = nNum * 60000;
	nNum = a_atoi(m_pTxt_Range->GetText());
	a_Clamp(nNum, MIN_RANGE, 9999);
	cfgData.nPetrolRadius = nNum;
	cfgData.iAutoPickMode = m_pCmb_AutoPick->GetCurSel();
	CECAutoPolicy::GetInstance().SetConfigData(cfgData);
}

void CDlgAutoPolicy::SetSkillImage(PAUIIMAGEPICTURE pSkillImg, int skill_id, bool bCombo)
{
	const EC_VIDEO_SETTING& vs = GetGame()->GetConfigs()->GetVideoSettings();

	if( bCombo )
	{
		if( skill_id >= 0 && skill_id < EC_COMBOSKILL_NUM )
		{
			int iIcon = vs.comboSkill[skill_id].nIcon;
			if( iIcon )
			{
				pSkillImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], iIcon + 1);
				pSkillImg->SetData(1);
				pSkillImg->SetData64(skill_id);

				ACString strText;
				strText.Format(GetStringFromTable(804), skill_id);
				pSkillImg->SetHint(strText);
			}
			else
			{
				pSkillImg->ClearCover();
				pSkillImg->SetData(0);
				pSkillImg->SetData64(0);
				pSkillImg->SetHint(_AL(""));
			}
		}
		else
		{
			pSkillImg->ClearCover();
			pSkillImg->SetData(0);
			pSkillImg->SetData64(0);
			pSkillImg->SetHint(_AL(""));
		}
	}
	else
	{
		if( skill_id == -1 && pSkillImg != m_pImg_Skill2 )
		{
			CECShortcutSet* pBscSCS = GetGameRun()->GetGenCmdShortcuts();
			CECShortcut* pSC = pBscSCS->GetShortcut(2);
			if( pSC )
			{
				AString strFile;
				af_GetFileTitle(pSC->GetIconFile(), strFile);
				strFile.MakeLower();
				pSkillImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);
				pSkillImg->SetData(3);
				pSkillImg->SetData64(0);

				const wchar_t* pszDesc = pSC->GetDesc();
				if( pszDesc )
				{
					AUICTranslate trans;
					AWString strHint = trans.Translate(pszDesc);
					pSkillImg->SetHint(strHint);
				}
				else
					pSkillImg->SetHint(_AL(""));
			}
		}
		else
		{
			CECSkill* pSkill = GetHostPlayer()->GetNormalSkill(skill_id);
			if( !pSkill ) pSkill = GetHostPlayer()->GetEquipSkillByID(skill_id);
			if( pSkill )
			{
				GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pSkillImg, pSkill);
				pSkillImg->SetData(2);
				pSkillImg->SetData64(skill_id);
			}
			else
			{
				GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pSkillImg, NULL);
				pSkillImg->SetData(0);
				pSkillImg->SetData64(0);
			}
		}
	}
}

ACString CDlgAutoPolicy::GetRemainTimeTxt()
{
	ACString strText;
	int nSec = CECAutoPolicy::GetInstance().GetRemainTime() / 1000;

	int nHour, nMin;
	nHour = nSec / 3600;
	nSec -= nHour * 3600;
	nMin = nSec / 60;
	nSec -= nMin * 60;
	strText.Format(GetStringFromTable(10927), nHour, nMin, nSec);
	return strText;
}
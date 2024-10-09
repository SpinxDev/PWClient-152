/*
 * FILE: DlgMatchProfile.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD
 *
 * HISTORY: 
 *
 * Copyright (c) 2013, All Rights Reserved.
 */
 
#include "DlgMatchProfile.h"
#include "EC_GameSession.h"
#include "AUICTranslate.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"

#include "DlgAutoLock.h"
#include "EC_CrossServer.h"

#include <AFI.h>

AUI_BEGIN_COMMAND_MAP(CDlgMatchProfileHabit, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Save",	OnCommandSave)
AUI_ON_COMMAND("Btn_Close",	OnCommand_CANCEL)

AUI_ON_COMMAND("Rdo_Info2",	OnCommandHobby)
AUI_ON_COMMAND("Rdo_Info3",	OnCommandAge)
AUI_ON_COMMAND("Rdo_Info4",	OnCommandSetting)

AUI_END_COMMAND_MAP()

void CDlgMatchProfileHabit::ToggleDlg(PAUIDIALOG pOld,PAUIDIALOG pNew)
{
	pOld->Show(false);

	pNew->Show(true);
	pNew->SetPosEx(pOld->GetPos().x,pOld->GetPos().y);
}
bool CDlgMatchProfileHabit::OnInitDialog()
{
	m_checker1.OnInitControl(this);
	m_checker2.OnInitControl(this);

	return CDlgBase::OnInitDialog();
}

void CDlgMatchProfileHabit::OnCommandSave(const char *szCommand)
{
	m_checker1.UpdateData(true);
	m_checker2.UpdateData(true);

	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_Infohobby"));
}
void CDlgMatchProfileHabit::OnShowDialog()
{
	m_checker1.UpdateData(false);
	m_checker2.UpdateData(false);

	CheckRadioButton(0,1);
}
void CDlgMatchProfileHabit::OnHideDialog()
{
	m_checker1.UpdateData(true);
	m_checker2.UpdateData(true);
}
void CDlgMatchProfileHabit::OnCommandHobby(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_Infohobby"));
}
void CDlgMatchProfileHabit::OnCommandAge(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_InfoConstellationAge"));
}
void CDlgMatchProfileHabit::OnCommandSetting(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_InfoMatch"));
}
//////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgMatchProfileHobby, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Save",	OnCommandSave)
AUI_ON_COMMAND("Btn_Close",	OnCommand_CANCEL)

AUI_ON_COMMAND("Rdo_Info1",	OnCommandHabit)
AUI_ON_COMMAND("Rdo_Info3",	OnCommandAge)
AUI_ON_COMMAND("Rdo_Info4",	OnCommandSetting)
AUI_END_COMMAND_MAP()


bool CDlgMatchProfileHobby::OnInitDialog()
{
	m_checker.OnInitControl(this);
	
	return CDlgBase::OnInitDialog();
}

void CDlgMatchProfileHobby::OnCommandSave(const char *szCommand)
{
	m_checker.UpdateData(true);	
	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_InfoConstellationAge"));
}

void CDlgMatchProfileHobby::OnShowDialog()
{
	m_checker.UpdateData(false);
	CheckRadioButton(0,2);
}
void CDlgMatchProfileHobby::OnHideDialog()
{
	m_checker.UpdateData(true);	
}
void CDlgMatchProfileHobby::OnCommandHabit(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_Infohabit"));
}
void CDlgMatchProfileHobby::OnCommandAge(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_InfoConstellationAge"));
}
void CDlgMatchProfileHobby::OnCommandSetting(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_InfoMatch"));
}

//////////////////////////////////////////////////////////////////////


AUI_BEGIN_COMMAND_MAP(CDlgMatchProfileAge, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Save",	OnCommandSave)
AUI_ON_COMMAND("Btn_Close",	OnCommand_CANCEL)

AUI_ON_COMMAND("Rdo_Info1",	OnCommandHabit)
AUI_ON_COMMAND("Rdo_Info2",	OnCommandHobby)
AUI_ON_COMMAND("Rdo_Info4",	OnCommandSetting)

AUI_END_COMMAND_MAP()


bool CDlgMatchProfileAge::OnInitDialog()
{
	m_age = 0;
	m_horoscope = 0;
	
	return CDlgBase::OnInitDialog();
}

void CDlgMatchProfileAge::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);
	DDX_RadioButton(bSave, 1, m_age);
	DDX_RadioButton(bSave, 2, m_horoscope);
}

void CDlgMatchProfileAge::OnCommandSave(const char *szCommand)
{
	UpdateData(true);
	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_InfoMatch"));
}

void CDlgMatchProfileAge::OnShowDialog()
{
	UpdateData(false);
	CheckRadioButton(0,3);
}
void CDlgMatchProfileAge::OnHideDialog()
{
	UpdateData(true);
}
void CDlgMatchProfileAge::OnCommandHabit(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_Infohabit"));
}
void CDlgMatchProfileAge::OnCommandHobby(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_Infohobby"));
}
void CDlgMatchProfileAge::OnCommandSetting(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_InfoMatch"));
}

//////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgMatchProfileSetting, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Save",	OnCommandSave)
AUI_ON_COMMAND("Btn_Close",	OnCommand_CANCEL)

AUI_ON_COMMAND("Rdo_Info1",	OnCommandHabit)
AUI_ON_COMMAND("Rdo_Info2",	OnCommandHobby)
AUI_ON_COMMAND("Rdo_Info3",	OnCommandAge)

AUI_END_COMMAND_MAP()


bool CDlgMatchProfileSetting::OnInitDialog()
{
	m_checker.OnInitControl(this);
	SetMask(0xff);
	
	m_bServerMaskInited = false;
	m_uServerMask = 0;

	m_bShowUIOnGetProfileReturn = false;
	
	return CDlgBase::OnInitDialog();
}

void CDlgMatchProfileSetting::OnCommandSave(const char *szCommand)
{
	m_checker.UpdateData(true);	
	
	Show(false);

	// sent to server
	unsigned short game_time_mask = GetGameUIMan()->m_pDlgMatchHabit->GetMaskTimeSpane();
	unsigned short game_interest_mask = GetGameUIMan()->m_pDlgMatchHabit->GetMaskGamePlay();
	unsigned short personal_interest_mask = GetGameUIMan()->m_pDlgMatchHobby->GetMaskHobby();
	unsigned char age = GetGameUIMan()->m_pDlgMatchAge->GetAge();
	unsigned char zodiac = GetGameUIMan()->m_pDlgMatchAge->GetHoroscope();
	unsigned short match_option_mask = GetMask();

	GetGameSession()->match_SaveProfile(game_time_mask,game_interest_mask,personal_interest_mask,age,zodiac,match_option_mask);
	GetProfile(false, false);
}

void CDlgMatchProfileSetting::OnShowDialog()
{
	m_checker.UpdateData(false);
	CheckRadioButton(0,4);
}
void CDlgMatchProfileSetting::OnHideDialog()
{
	m_checker.UpdateData(true);	
}
void CDlgMatchProfileSetting::OnCommandHabit(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_Infohabit"));
}
void CDlgMatchProfileSetting::OnCommandHobby(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_Infohobby"));
}
void CDlgMatchProfileSetting::OnCommandAge(const char *szCommand)
{	
	CDlgMatchProfileHabit::ToggleDlg(this,GetGameUIMan()->GetDialog("Win_InfoConstellationAge"));
}

void CDlgMatchProfileSetting::SetServerMask(unsigned short uMask){
	m_bServerMaskInited = true;
	m_uServerMask = uMask;
}

bool CDlgMatchProfileSetting::GetProfile(bool bShowErrorMessage, bool bShowUIOnReturn){
	if (GetGameUIMan()->m_pDlgAutoLock->IsLocked()){
		if (bShowErrorMessage){
			GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(10524), GP_CHAT_MISC);
		}
		return false;
	}
	if (CECCrossServer::Instance().IsOnSpecialServer()){
		if (bShowErrorMessage){
			GetGameUIMan()->ShowErrorMsg(10713);
		}
		return false;
	}	
	GetGameSession()->match_GetProfile();
	m_bShowUIOnGetProfileReturn = bShowUIOnReturn;
	return true;
}

//////////////////////////////////////////////////////////////////////
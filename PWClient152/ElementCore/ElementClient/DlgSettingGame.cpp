/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   17:50
	file name:	DlgSettingGame.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/


#include "DlgSettingGame.h"
#include "DlgChat.h"
#include "AUIStillImageButton.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgSettingGame, CDlgSetting)

AUI_ON_COMMAND("set*", OnCommandSet)
AUI_ON_COMMAND("Btn_GotoFunction", OnCommandFunction)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgSettingGame, CDlgSetting)

AUI_ON_EVENT("Ch_*", WM_LBUTTONUP, OnLButtonUpChat)

AUI_END_EVENT_MAP()

CDlgSettingGame::CDlgSettingGame()
{
	m_nChanel = 1;
	memset(&m_setting, 0, sizeof(m_setting));
}

CDlgSettingGame::~CDlgSettingGame()
{
}

bool CDlgSettingGame::OnInitDialog()
{
	if (!CDlgSetting::OnInitDialog())
		return false;
	
	// init diaplay
	UpdateView();

	return true;
}

void CDlgSettingGame::OnShowDialog()
{
	CDlgSetting::OnShowDialog();

	// init control
	m_pBtnSystem->SetPushed(false);
	m_pBtnGame->SetPushed(true);
	m_pBtnVideo->SetPushed(false);
	m_pBtnQuickKey->SetPushed(false);

	// init diaplay
	UpdateView();
}

void CDlgSettingGame::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone) SwitchCountryChannel();
}

void CDlgSettingGame::DoDataExchange(bool bSave)
{
	CDlgSetting::DoDataExchange(bSave);

	DDX_CheckBox(bSave, "Chk_ReverseMouse", m_setting.bReverseWheel);
	DDX_CheckBox(bSave, "Chk_SwitchAD", m_setting.bTurnaround);
	DDX_CheckBox(bSave, "Chk_Noteam", m_setting.bNoTeamRequest);
	DDX_CheckBox(bSave, "Chk_Notrade", m_setting.bNoTradeRequest);
	//DDX_CheckBox(bSave, "Chk_PetBuff", m_setting.bPetAutoSkill);		// 这两项放到优化设置中的功能设置里
	//DDX_CheckBox(bSave, "Chk_LockQBar", m_setting.bLockQuickBar);
	DDX_CheckBox(bSave, "Chk_Autoreply", m_setting.bAutoReply);
	DDX_Slider(bSave, "Slider_ChatFsize", m_setting.nFontSize);
	DDX_EditBox(bSave, "Txt_Reply", m_setting.szAutoReply);
	DDX_RadioButton(bSave, 1, m_nChanel);
	DDX_CheckBox(bSave, "Chk_FontBord", m_setting.bFontBold);
	for (int nChatIndex = 0; nChatIndex < GP_CHAT_MAX; nChatIndex++)
	{
		ASSERT(m_nChanel >= 1 && m_nChanel <= EC_USERCHANNEL_NUM);
		AString strCheckBoxName;
		strCheckBoxName.Format("Ch_%d", nChatIndex + 1);
		DDX_CheckBox(bSave, strCheckBoxName, m_setting.bChannel[m_nChanel - 1][nChatIndex]);
	}
	AString strCheckBoxName;
	strCheckBoxName.Format("Ch_%d", GP_CHAT_FARCRY + 1);
	if( m_nChanel > 1 )
		GetDlgItem(strCheckBoxName)->Enable(true);
	else
		GetDlgItem(strCheckBoxName)->Enable(false);
}

void CDlgSettingGame::SetToDefault()
{
	GetGame()->GetConfigs()->DefaultUserSettings(NULL, NULL, &m_setting);
	
	UpdateData(false);
}

void CDlgSettingGame::Apply()
{
	UpdateData(true);
	
	// save setting
	GetGame()->GetConfigs()->SetGameSettings_NoOptimizeFunction(m_setting);

	// todo : the following lines should be rewritten
	AUIOBJECT_SETPROPERTY sp;
	CDlgChat *pChat;
	PAUITEXTAREA pText;
	
	pChat = (CDlgChat *)GetGameUIMan()->GetDialog("Win_Chat");
	pText = dynamic_cast<PAUITEXTAREA>(pChat->GetDlgItem("Txt_Content"));
	pText->GetProperty("Font", &sp);
	sp.font.nHeight = m_setting.nFontSize + 8;	// 8 to 12.
	sp.font.bBold = m_setting.bFontBold;
	pText->SetProperty("Font", &sp);
	pChat->RebuildChatContents(pText, pChat->GetActiveChannelSet());
	
	pChat = (CDlgChat *)GetGameUIMan()->GetDialog("Win_ChatSmall");
	pText = dynamic_cast<PAUITEXTAREA>(pChat->GetDlgItem("Txt_Content"));
	pText->GetProperty("Font", &sp);
	sp.font.nHeight = m_setting.nFontSize + 8;	// 8 to 12.
	sp.font.bBold = m_setting.bFontBold;
	pText->SetProperty("Font", &sp);
	pChat->RebuildChatContents(pText, EC_USERCHANNEL_NUM - 1);
	
	pChat = (CDlgChat *)GetGameUIMan()->GetDialog("Win_ChatBig");
	pText = dynamic_cast<PAUITEXTAREA>(pChat->GetDlgItem("Txt_Content"));
	pText->GetProperty("Font", &sp);
	sp.font.nHeight = m_setting.nFontSize + 12;	// 12 to 16.
	sp.font.bBold = m_setting.bFontBold;
	pText->SetProperty("Font", &sp);
	ACString strText = pText->GetText();
	pText->SetText(strText);
}


void CDlgSettingGame::UpdateView()
{
	// read setting
	m_setting = GetGame()->GetConfigs()->GetGameSettings();
	int i;
	for(i = 0; i < EC_USERCHANNEL_NUM  - 1; i++ )
	{
		m_setting.bChannel[i][GP_CHAT_BROADCAST] = true;
		m_setting.bChannel[i][GP_CHAT_WHISPER] = true;
		m_setting.bChannel[i][GP_CHAT_SUPERFARCRY] = true;
	}
	m_setting.bChannel[0][GP_CHAT_FARCRY] = true;
	m_setting.bChannel[EC_USERCHANNEL_NUM  - 1][GP_CHAT_FARCRY] = false;
	m_setting.bChannel[EC_USERCHANNEL_NUM  - 1][GP_CHAT_BROADCAST] = false;
	m_setting.bChannel[EC_USERCHANNEL_NUM  - 1][GP_CHAT_WHISPER] = false;
	m_setting.bChannel[EC_USERCHANNEL_NUM  - 1][GP_CHAT_SUPERFARCRY] = false;
	GetGame()->GetConfigs()->SetGameSettings(m_setting);
	AString strCheckBoxName;
	strCheckBoxName.Format("Ch_%d", GP_CHAT_FARCRY + 1);
	GetDlgItem(strCheckBoxName)->Enable(false);
	strCheckBoxName.Format("Ch_%d", GP_CHAT_BROADCAST + 1);
	GetDlgItem(strCheckBoxName)->Enable(false);
	strCheckBoxName.Format("Ch_%d", GP_CHAT_WHISPER + 1);
	GetDlgItem(strCheckBoxName)->Enable(false);

	// show setting
	UpdateData(false);
}

void CDlgSettingGame::OnCommandSet(const char *szCommand)
{
	AString strNum = szCommand + strlen("set");
	m_nChanel = strNum.ToInt();

	UpdateData(false);
}

void CDlgSettingGame::OnCommandFunction(const char *szCommand)
{
	AUIDialog * pDlgOptimize = GetGameUIMan()->m_pDlgOptimizeCurrent;
	if (pDlgOptimize) {
		if (!pDlgOptimize->IsShow()) {
			pDlgOptimize->Show(true);
		}
		pDlgOptimize->OnCommand("Btn_SwitchFunction");
	} else {
		GetGameUIMan()->GetDialog("Win_OptimizeFunc")->Show(true);
	}
}

void CDlgSettingGame::OnLButtonUpChat(WPARAM, LPARAM, AUIObject *)
{
	UpdateData(true);
}

void CDlgSettingGame::SwitchCountryChannel()
{
	CECHostPlayer *pHost = GetHostPlayer();

	PAUIOBJECT pObj = GetDlgItem(AC2AS(_AL("Lb_普通聊天b")));
	if (pObj)
	{
		int idText(0);
		if (!pHost->HasCountryWarChannel())
		{
			idText = 9976;
		}
		else if (pHost->GetBattleCamp() == GP_BATTLE_CAMP_INVADER)
		{
			idText = 9977;
		}
		else
		{
			idText = 9978;
		}
		pObj->SetText(GetStringFromTable(idText));
	}
	
	pObj = GetDlgItem(AC2AS(_AL("Lb_交易信息b")));
	if (pObj)
	{
		int idText(0);
		if (!pHost->HasCountryChannel())
		{
			idText = 9979;
		}
		else
		{
			idText = 9980;
		}
		pObj->SetText(GetStringFromTable(idText));
	}
}

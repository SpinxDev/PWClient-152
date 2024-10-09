/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   15:31
	file base:	DlgAskHelpToGM
	file ext:	cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "DlgAskHelpToGM.h"
#include "DlgAskHelpToGM2.h"
#include "AUIEditBox.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "PredefinedGMHelpMsg.h"
#include "AUIEditBox.h"
#include "AUIComboBox.h"
#include "EC_UIConfigs.h"

#define new A_DEBUG_NEW


AUI_BEGIN_COMMAND_MAP(CDlgAskHelpToGM, CDlgBase)

AUI_ON_COMMAND("confirm", OnCommandConfirm)
AUI_ON_COMMAND("cbxType", OnCommandType)

AUI_ON_COMMAND("Btn_Speak", OnCommandSpeak)

AUI_END_COMMAND_MAP()

CDlgAskHelpToGM::CDlgAskHelpToGM() :
	m_pCbxType(NULL),
	m_pEbxMemo(NULL),
	m_dwSelectedType(-1)
{

}

CDlgAskHelpToGM::~CDlgAskHelpToGM()
{

}

void CDlgAskHelpToGM::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_ComboBoxItemData(bSave, "cbxType", m_dwSelectedType);
}

bool CDlgAskHelpToGM::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	DDX_Control("cbxType", m_pCbxType);
	DDX_Control("ebxMemo", m_pEbxMemo);
	m_pEbxMemo->SetIsAutoReturn(true);

	GetDlgItem("Btn_GACD")->Enable(false);

	return true;
}


void CDlgAskHelpToGM::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_dwSelectedType = -1;
	m_strPlugin.Empty();
	m_idPluginUser = 0;
	m_strPluginUserName.Empty();
	m_pCbxType->ResetContent();
	m_pEbxMemo->SetText(_AL(""));		
	GetDlgItem("Lab_Memo")->Show(false);
	m_pEbxMemo->Show(false);
	m_pEbxMemo->SetIsReadOnly(true);

	UpdateData(false);
}

bool CDlgAskHelpToGM::GetMsgToSend(ACString &strMsg)
{
	bool bOK(false);
	while (true)
	{		
		ACHAR type = static_cast<ACHAR>(m_dwSelectedType);
		GMHelpMsg msg;
		if (!TheGMHelpMsgArray::Instance()->FindByType(type, &msg))
		{
			ASSERT(!"invalid type for predefined gm help message");
			break;
		}
		if (IsReportWithFeedback()){
			strMsg = m_pEbxMemo->GetText();
		}else{
			strMsg = ACString(GM_HELP_MSG_TYPE_BASE, 1) + ACString(type, 1);
			strMsg += GetStringFromTable(msg.msg);
			ACString strMsgAdditional = m_pEbxMemo->GetText();
			GetGameRun()->GetUIManager()->FilterBadWords(strMsgAdditional);
			if (!strMsgAdditional.IsEmpty()){
				strMsg += _AL(",") + strMsgAdditional;
			}
		}
		bOK = true;
		break;
	}
	return bOK;
}

bool CDlgAskHelpToGM::IsReportWithFeedback()
{
	return m_dwSelectedType == GM_HELP_MSG_TYPE_PLUGIN
		&& CECUIConfig::Instance().GetGameUI().bEnableReportPluginWithFeedback;
}

void CDlgAskHelpToGM::OnCommandConfirm(const char *szCommand)
{
	UpdateData(true);

	ACString strMsg;
	if (!GetMsgToSend(strMsg)){
		return;
	}
	if (IsReportWithFeedback()){
		GetGameSession()->ReportPluginToSystem(m_idPluginUser, strMsg);
	}else{
		if (m_dwSelectedType == GM_HELP_MSG_TYPE_PLUGIN)
			GetGameSession()->SubmitProblemToGM(strMsg, m_idPluginUser, m_strPluginUserName);
		else
			GetGameSession()->SubmitProblemToGM(strMsg, 0, NULL);
		GetGameSession()->SendPrivateChatData(_AL("GM"), strMsg);
	}

	// display in local window
	GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(569), GP_CHAT_MISC);
	
	SetData(GetTickCount());
	
	Show(false);
}

void CDlgAskHelpToGM::ShowWithHelpMessage(unsigned int mask, const ACHAR * szPluginMsg /* = NULL */, int idPluginUser/* =0 */, const ACHAR *szPluginUserName/* =NULL */)
{
	if (CECUIConfig::Instance().GetGameUI().bEnableReportPlayerSpeakToGM)
	{
		GetDlgItem("Btn_GACD")->Show(true);
		GetDlgItem("Btn_Speak")->Show(true);
		GetDlgItem("Btn_Speak")->Enable(idPluginUser > 0);
	}
	else
	{
		GetDlgItem("Btn_GACD")->Show(false);
		GetDlgItem("Btn_Speak")->Show(false);
	}
	Show(true);

	m_strPlugin = szPluginMsg;
	m_idPluginUser = idPluginUser;
	m_strPluginUserName = szPluginUserName;

	const CGMHelpMsgArray & ary = *TheGMHelpMsgArray::Instance();
	ACHAR type;
	GMHelpMsg msg;

	if (mask & GM_REPORT_STUCK)
	{
		//	被卡住了

		type = GM_HELP_MSG_TYPE_STUCK;
		if (ary.FindByType(type, &msg))
		{
			//	添加供玩家选择
			int nIndex = m_pCbxType->AddString(GetStringFromTable(msg.msg)) - 1;
			m_pCbxType->SetItemData(nIndex, static_cast<DWORD>(msg.type));

			//	默认选中此条目
			m_dwSelectedType = type;
		}
		else
		{
			ASSERT(false);
		}
	}
	
	if (mask & GM_REPORT_PLUGIN)
	{
		//	举报外挂

		type = GM_HELP_MSG_TYPE_PLUGIN;
		if (ary.FindByType(type, &msg))
		{
			//	添加供玩家选择
			int nIndex = m_pCbxType->AddString(GetStringFromTable(msg.msg)) - 1;
			m_pCbxType->SetItemData(nIndex, static_cast<DWORD>(msg.type));

			//	默认选中此条目
			m_dwSelectedType = type;
		}
		else
		{
			ASSERT(false);
		}
	}
	
	if (mask & GM_REPORT_TALK)
	{
		//	与GM对话
		if (CECUIConfig::Instance().GetGameUI().bEnableTalkToGM)
		{
			type = GM_HELP_MSG_TYPE_TALK;
			if (ary.FindByType(type, &msg))
			{
				//	添加供玩家选择
				int nIndex = m_pCbxType->AddString(GetStringFromTable(msg.msg)) - 1;
				m_pCbxType->SetItemData(nIndex, static_cast<DWORD>(msg.type));
				
				//	默认选中此条目
				m_dwSelectedType = type;
			}
			else
			{
				ASSERT(false);
			}
		}
	}

	//	根据 m_dwSelectedType 内容更新Combobox
	UpdateData(false);

	//	根据Combobox选中内容更新其它内容
	OnCommandType(NULL);

	//	Combobox 只有一个选项时Disable
	m_pCbxType->Enable(m_pCbxType->GetCount()>1);
}

void CDlgAskHelpToGM::OnCommandType(const char * szCommand)
{
	UpdateData(true);

	if (m_dwSelectedType == GM_HELP_MSG_TYPE_STUCK)
	{
		GetDlgItem("Lab_Memo")->Show(false);
		m_pEbxMemo->Show(false);
		m_pEbxMemo->SetText(_AL(""));
		m_pEbxMemo->SetIsReadOnly(true);
	}
	else if (m_dwSelectedType == GM_HELP_MSG_TYPE_PLUGIN)
	{
		GetDlgItem("Lab_Memo")->Show(true);
		m_pEbxMemo->Show(true);
		m_pEbxMemo->SetText(m_strPlugin);
		m_pEbxMemo->SetIsReadOnly(false);
		ChangeFocus(m_pEbxMemo);
	}
	else if (m_dwSelectedType == GM_HELP_MSG_TYPE_TALK)
	{
		m_pEbxMemo->Show(true);
		GetDlgItem("Lab_Memo")->Show(true);
		m_pEbxMemo->SetIsReadOnly(false);
		m_pEbxMemo->SetText(_AL(""));
		ChangeFocus(m_pEbxMemo);
	}
}
	
void CDlgAskHelpToGM::OnCommandSpeak(const char * szCommand)
{
	if (m_idPluginUser <= 0){
		return;
	}
	CDlgAskHelpToGM2 *pDlg = GetGameUIMan()->m_pDlgAskHelpToGM2;
	
	const ACHAR *pszName = GetGameRun()->GetPlayerName(m_idPluginUser, true);
	ACHAR szText[40];
	AUI_ConvertChatString(pszName, szText);
	
	pDlg->ShowWithHelpMessage(m_idPluginUser, pszName);
	POINT pos = GetPos();
	pDlg->SetPosEx(pos.x, pos.y);
	
	Show(false);
}
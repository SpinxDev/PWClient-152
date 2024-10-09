/********************************************************************
	created:	2012/11/12
	created:	12:11:2012   16:31
	file base:	DlgAskHelpToGM2
	file ext:	cpp
	author:		zhougaomin01305
	
	purpose:	举报发言
*********************************************************************/

#include "DlgAskHelpToGM2.h"
#include "DlgAskHelpToGM.h"
#include "AUIEditBox.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "PredefinedGMHelpMsg.h"
#include "AUIEditBox.h"
#include "AUIListBox.h"
#include "AUILabel.h"
#include "AUIComboBox.h"
#include "DlgChat.h"

#define new A_DEBUG_NEW

struct ReportItemsFilter : public CECGameUIMan::AUI_EditboxItemFilter
{
	bool operator()(EditBoxItemBase *pItem)const
	{
		// 一律过滤掉
		return true;
	}
};

AUI_BEGIN_COMMAND_MAP(CDlgAskHelpToGM2, CDlgBase)

AUI_ON_COMMAND("confirm", OnCommandConfirm)

AUI_ON_COMMAND("Btn_GACD", OnCommandGACD)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAskHelpToGM2, CDlgBase)

AUI_ON_EVENT("ListBox_Content",		WM_LBUTTONDOWN,		OnEventLButtonDown_ListBox_Content)

AUI_END_EVENT_MAP()

CDlgAskHelpToGM2::CDlgAskHelpToGM2()
{

}

CDlgAskHelpToGM2::~CDlgAskHelpToGM2()
{

}

void CDlgAskHelpToGM2::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

}

bool CDlgAskHelpToGM2::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	GetDlgItem("Btn_Speak")->Enable(false);

	return true;
}


void CDlgAskHelpToGM2::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_idPluginUser = 0;

	UpdateData(false);
}

void CDlgAskHelpToGM2::OnEventLButtonDown_ListBox_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PAUILISTBOX pText = dynamic_cast<PAUILISTBOX>(GetDlgItem("ListBox_Content"));
	PAUITEXTAREA pArea = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
	int isel = pText->GetCurSel();
	m_index = isel;
	pArea->SetText(_AL(""));
	pArea->SetText(pText->GetText(isel));

	GetDlgItem("Btn_Decide")->Enable( m_index >= 0 && m_index < pText->GetCount() );

}

void CDlgAskHelpToGM2::OnCommandConfirm(const char *szCommand)
{
	UpdateData(true);
	
	ACHAR type = GM_HELP_MSG_TYPE_SPEAK;
	GMHelpMsg msg;
	if (!TheGMHelpMsgArray::Instance()->FindByType(type, &msg))
	{
		ASSERT(!"invalid type for predefined gm help message");
		return;
	}

	if(m_index < 0 || m_index >= m_indexForOrigions.size())
		return;
	
	PAUITEXTAREA pArea = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
	//	获取发送信息内容
	ACString strMsg = m_indexForOrigions[m_index];
	
	// send to gm
	GetGameSession()->SubmitChatSpeakToGM(strMsg, m_idPluginUser, m_strPluginUserName);
	//GetGameSession()->SendPrivateChatData(_AL("GM"), strMsg);	
	
	// display in local window
	GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(569), GP_CHAT_MISC);
	
	SetData(GetTickCount());
	
	Show(false);
}

ACString CDlgAskHelpToGM2::AUI_FilterEditboxItem(const ACHAR *szText, const CECGameUIMan::AUI_EditboxItemFilter& filter,CECIvtrItem* pChatItem)
{
	// 将文本中的某些内容由过滤函数处理补充、或直接过滤掉
	// 参数：szText 待过滤处理文本
	// 参数：filter 过滤函数或过滤器，原型为 bool AUI_EditboxItemFilter(EditboxItemBase *)
	//		 返回 true 则过滤掉（即恢复成由 item 的 m_szName 表示的普通字符），返回 false 保留但可更改其内容
	// 返回值：过滤处理后的合法字符串
	//
	EditBoxItemsSet ItemsSet;
	ACString strText = UnmarshalEditBoxText(szText, ItemsSet);
	int nCount = ItemsSet.GetItemCount();
	
	if (nCount == 0)
		return ACString(szText);
	
	int i = 0;		
	EditBoxItemMap::iterator it = ItemsSet.GetItemIterator();
	do 
	{
		EditBoxItemBase* pItem = it->second;
		
		if (pItem)
		{
			if (filter(pItem))
			{
				if(pItem->GetType() == enumEIIvtrlItem && pChatItem)
				{
					//	将名字替换成带【】的，便于识别
					ACString strItemName;
					A3DCOLOR clrItemName;
					GetGameUIMan()->TransformNameColor(pChatItem, strItemName, clrItemName);

					wchar_t cItem = it->first;
					strText = GetGameUIMan()->AUI_ReplaceEditboxItem(strText, cItem, strItemName);
				}
				else
				{
					wchar_t cItem = it->first;
					strText = GetGameUIMan()->AUI_ReplaceEditboxItem(strText, cItem, pItem->GetName());
				}
				
			}
		}
		
		++it;
		++i;
	} while (i < nCount);
	
	return MarshalEditBoxText(strText, ItemsSet);
}

void CDlgAskHelpToGM2::ShowWithHelpMessage(int idPluginUser/* =0 */, const ACHAR *szPluginUserName/* =NULL */)
{
	PAUILISTBOX pText = (PAUILISTBOX)GetDlgItem("ListBox_Content");
	PAUILABEL pLabel  = (PAUILABEL)GetDlgItem("Label_PlayerName");

	static ACString formatstr = pLabel->GetText();

	int i;
	int count = pText->GetCount();
	for(i = 0;i < count;++i)
		pText->DeleteString(0);

	Show(true);
	GetDlgItem("Btn_GACD")->Enable(idPluginUser > 0);

	m_idPluginUser = idPluginUser;
	m_strPluginUserName = szPluginUserName;
	ACHAR szText[100];

	a_sprintf(szText, formatstr, szPluginUserName);
	pLabel->SetText(szText);

	AUI_ConvertChatString(szPluginUserName, szText);

	CDlgChat *pDlg = GetGameUIMan()->m_pDlgChat;

	abase::vector<CDlgChat::CHAT_MSG> &vecChatMsg = pDlg->GetAllChatMsgs();

	ACString chatName = szText;
	ACString chatMsg;
	chatName = _AL("&") + chatName + _AL("&");

	m_indexForOrigions.clear();
	int index;
	CDlgChat::CHAT_MSG msg;
	for(i = 0;i < (int)vecChatMsg.size();++i)
	{
		msg = vecChatMsg[i];
		// ID过滤
		if(msg.idPlayer != idPluginUser) continue;
		// 频道过滤
		if(msg.cChannel == GP_CHAT_MISC
		|| msg.cChannel == GP_CHAT_DAMAGE
		|| msg.cChannel == GP_CHAT_FIGHT
		|| msg.cChannel == GP_CHAT_SYSTEM
			) continue;
		// 物品表情等特殊格式串过滤
		chatMsg = AUI_FilterEditboxItem(msg.strMsg, ReportItemsFilter(),msg.pItem);
		index = pText->AddString(chatMsg) - 1;
		m_indexForOrigions.push_back(msg.strMsgOrigion);
	}
	pText->SetCurSel(index);
	m_index = index;
	
	OnEventLButtonDown_ListBox_Content(0,0,NULL);
}

void CDlgAskHelpToGM2::OnCommandGACD(const char * szCommand)
{
	if (m_idPluginUser <= 0){
		return;
	}

	CDlgAskHelpToGM *pDlg = GetGameUIMan()->m_pDlgAskHelpToGM;

	const ACHAR *pszName = GetGameRun()->GetPlayerName(m_idPluginUser, true);
	ACHAR szText[40];
	AUI_ConvertChatString(pszName, szText);
	
	ACString msg;
	msg.Format(GetStringFromTable(6007), szText);
	pDlg->ShowWithHelpMessage(CDlgAskHelpToGM::GM_REPORT_PLUGIN, msg, m_idPluginUser, pszName);
	POINT pos = GetPos();
	pDlg->SetPosEx(pos.x, pos.y);
	
	Show(false);
}
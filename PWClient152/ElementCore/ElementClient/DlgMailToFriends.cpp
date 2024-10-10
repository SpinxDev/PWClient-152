#include "AUIEditBox.h"
#include "DlgMailToFriends.h"
#include "DlgFriendList.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Friend.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EL_Precinct.h"
#include "EC_UIConfigs.h"
#include "Network\\gnetdef.h"
#include "A3DFTFont.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgMailToFriends, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCANCAL)
AUI_ON_COMMAND("Btn_Send", OnCommandSendMail)
AUI_ON_COMMAND("Btn_Refresh", OnCommandRefresh)
AUI_ON_COMMAND("Combo_Model", OnCommandSelectTemplate)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgMailToFriends, CDlgBase)

AUI_ON_EVENT("Lst_Friendlist",	WM_LBUTTONUP,	OnEventSelectList)

AUI_END_EVENT_MAP()

static const int ONE_DAY_SECONDS = 24 * 3600;
static const int SECONDS_BETWEEN_REFRESH = 300;
static const ACHAR SENDABLE_COLOR[] = _AL("^00FF00");
static const ACHAR GRAY_COLOR[] = _AL("^AAAAAA");

static inline int GET_DAYS_BETWEEN(int iEnd,int iStart)
{
	return (iEnd - iStart) / ONE_DAY_SECONDS;
}

CDlgMailToFriends::CDlgMailToFriends():
m_pFriendsToMailList(0),
m_pTemplateComboBox(0),
m_iSendMailPerDay(0),
m_bNeedAttrackAttention(false),
m_iRefreshStartTime(0)
{
}

CDlgMailToFriends::~CDlgMailToFriends()
{
}

void CDlgMailToFriends::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

}

bool CDlgMailToFriends::OnInitDialog()
{
	DDX_Control("Combo_Model", m_pTemplateComboBox);
	DDX_Control("Lst_Friendlist", m_pFriendsToMailList);
	GetDlgItem("Btn_Send")->Enable(false);

	int nMax = 9500 + a_Min(a_atoi(GetStringFromTable(9499)), 9);
	PAUICOMBOBOX pCombo = dynamic_cast<PAUICOMBOBOX>(GetDlgItem("Combo_Model"));
	if (pCombo)
	{
		for (int i = 9500;i < nMax;++i)
			pCombo->AddString(m_pAUIManager->GetStringFromTable(i));
	}
	m_pTemplateComboBox->SetCurSel(0);
	OnCommandSelectTemplate("");
	return true;
}

void CDlgMailToFriends::OnShowDialog()
{
	CDlgBase::OnShowDialog();
}

void CDlgMailToFriends::OnCommandSelectTemplate(const char *szCommand)
{
	int iSel = m_pTemplateComboBox->GetCurSel();

	// 选择模板后显示信件内容和发件人名字
	if (iSel >= 0 && iSel < m_pTemplateComboBox->GetCount())
	{
		PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
		ACString str;

		str.Format(m_pAUIManager->GetStringFromTable(9540),
			(m_pAUIManager->GetStringFromTable(9510 + iSel)), GetHostPlayer()->GetName());

		if (pText)
			pText->SetText(str);

		PAUIOBJECT pObj = GetDlgItem("Txt_MailTitle");
		if (pObj)
			pObj->SetText(m_pAUIManager->GetStringFromTable(9530 + iSel));
		
	}
}
void CDlgMailToFriends::OnCommandCANCAL(const char *szCommand)
{
	Show(false);
}

void CDlgMailToFriends::OnCommandSendMail(const char *szCommand)
{
	int iSel = m_pFriendsToMailList->GetCurSel();
	int iSelTemplate = m_pTemplateComboBox->GetCurSel();
	if (iSel >= 0 && iSel < m_pFriendsToMailList->GetCount())
	{
		int iRoleID = m_pFriendsToMailList->GetItemData(iSel);
		if (iRoleID != AUILISTBOX_ERROR)
		{
			g_pGame->GetGameSession()->friend_SendMail(iRoleID,iSelTemplate);
			// disable 好友列表和发信按钮，收到服务器re后enable好友列表
			m_pFriendsToMailList->Enable(false);
			GetDlgItem("Btn_Send")->Enable(false);
		}
		else ASSERT(!"Friend ID Invalid!");
	}
}

void CDlgMailToFriends::OnTick()
{
	if (m_iRefreshStartTime)
	{
		if (g_pGame->GetServerAbsTime() - m_iRefreshStartTime > SECONDS_BETWEEN_REFRESH 
			&& !GetDlgItem("Btn_Refresh")->IsEnabled())
		{
			GetDlgItem("Btn_Refresh")->Enable(true);
		}
	}
}

void CDlgMailToFriends::OnCommandRefresh(const char *szCommand)
{
	if (!m_iRefreshStartTime)
	{
		m_iRefreshStartTime = g_pGame->GetServerAbsTime();
	}
	else if (g_pGame->GetServerAbsTime() - m_iRefreshStartTime < SECONDS_BETWEEN_REFRESH)
	{
		return;
	}
	g_pGame->GetGameSession()->friend_GetList();
	m_iRefreshStartTime = g_pGame->GetServerAbsTime();
	GetDlgItem("Btn_Refresh")->Enable(false);
}

void CDlgMailToFriends::OnEventSelectList(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (m_iSendMailPerDay >= 5)
		return;
	
	CECFriendMan* pFriendMan = GetHostPlayer()->GetFriendMan();
	int iSel = m_pFriendsToMailList->GetCurSel();
	if (iSel >= 0 && iSel < m_pFriendsToMailList->GetCount())
	{
		int iRoleID = m_pFriendsToMailList->GetItemData(iSel);
		int iDaysSentMail = m_pFriendsToMailList->GetItemData(iSel, 1);
		if (iRoleID != AUILISTBOX_ERROR)
		{
			PAUIOBJECT pButton = (GetDlgItem("Btn_Send"));
			if (pButton)
				pButton->Enable(GetHostPlayer()->GetBasicProps().iLevel >= CECUIConfig::Instance().GetGameUI().nMailToFriendsLevel
				&& (-1 == iDaysSentMail || iDaysSentMail >= CECUIConfig::Instance().GetGameUI().nMailToFriendsDaysSendMail ));	

			CECFriendMan::FRIEND* pFriend = pFriendMan->GetFriendByID(iRoleID);
			PAUIOBJECT pObj = GetDlgItem("Txt_Receiver");
			if (pObj && pFriend)
				pObj->SetText(pFriend->GetName());
		}
		else ASSERT(!"Friend ID Invalid!");
	}
}
void CDlgMailToFriends::FriendAction(int idPlayer, int idGroup, int idAction, int nCode)
{
	if (idAction == CDlgFriendList::FRIEND_ACTION_FRIEND_DELETE)
	{
		BuildFriendListEx(NULL);
	}
}
void CDlgMailToFriends::FormatDays(int iDays,ACString& strInOut)
{
	if (iDays < 31)
	{
		strInOut.Format(_AL("%d"),iDays);
	}
	else if (iDays >= 31 && iDays < 61)
	{
		strInOut = m_pAUIManager->GetStringFromTable(9523);
	}
	else if (iDays >= 61 && iDays < 91)
	{
		strInOut = m_pAUIManager->GetStringFromTable(9524);
	}
	else if (iDays >= 91 && iDays < 121)
	{
		strInOut = m_pAUIManager->GetStringFromTable(9525);
	}
	else if (iDays >= 121 && iDays < 151)
	{
		strInOut = m_pAUIManager->GetStringFromTable(9526);
	}
	else if (iDays >= 151 && iDays < 181)
	{
		strInOut = m_pAUIManager->GetStringFromTable(9527);
	}
	else if (iDays >= 181)
	{
		strInOut = m_pAUIManager->GetStringFromTable(9528);
	}

}
void CDlgMailToFriends::FormatName(ACString& strInOut)
{
	int iNameMaxWid = m_pFriendsToMailList->GetSize().cx / 4;
	int iMarkWid = m_pFriendsToMailList->GetFont()->GetTextExtent(_AL("...")).x;
	iNameMaxWid += iMarkWid;
	bool bCutName = false;
	while (m_pFriendsToMailList->GetFont()->GetTextExtent(strInOut).x > iNameMaxWid)
	{
		strInOut.CutRight(1);
		bCutName = true;
	}
	if (bCutName)
	{
		strInOut += _AL("...");
	}
}
void CDlgMailToFriends::BuildFriendListEx(PAUIDIALOG pDlg)
{
	CECFriendMan* pFriendMan = GetHostPlayer()->GetFriendMan();
	ACHAR szText[256];
	m_pFriendsToMailList->ResetContent();
	m_iSendMailPerDay = 0;
	int iTime = g_pGame->GetServerAbsTime();
	int nDaysSendMailConfig = CECUIConfig::Instance().GetGameUI().nMailToFriendsDaysSendMail;
	abase::hash_map<int,int> RoleIDSendTimeMap;
	// 统计当日内发信数量
	tm current_time = g_pGame->GetServerLocalTime(iTime);
	APtrArray<CECFriendMan::SEND_INFO*>& pSendInfoArr = pFriendMan->GetSendInfo();
	size_t j(0);
	for (j = 0;j < pSendInfoArr.GetSize();++j)
	{
		CECFriendMan::SEND_INFO* pSendInfo = pSendInfoArr[j];
		RoleIDSendTimeMap[pSendInfo->rid] = pSendInfo->sendmail_time;
		if (pSendInfo->sendmail_time)
		{
			tm send_time = g_pGame->GetServerLocalTime(pSendInfo->sendmail_time);
			if (current_time.tm_yday == send_time.tm_yday 
				&& current_time.tm_year == send_time.tm_year)
				m_iSendMailPerDay++;
		}
	}

	abase::vector<CECFriendMan::FRIEND_EX*>& pFriendExArr = pFriendMan->GetFriendEx();
	A3DCOLOR colorBase = m_pFriendsToMailList->GetColor();
	ACString colorBackup;
	colorBackup.Format(_AL("^%x%x%x"),A3DCOLOR_GETRED(colorBase),A3DCOLOR_GETGREEN(colorBase), A3DCOLOR_GETBLUE(colorBase));
	int index = 0;
	for (j=0; j < pFriendExArr.size(); j++)
	{
		CECFriendMan::FRIEND_EX* pFriendEx = pFriendExArr[j];
		CECFriendMan::FRIEND* pFriend = pFriendMan->GetFriendByID(pFriendEx->rid);
		int iDaysNoLogin = GET_DAYS_BETWEEN(iTime,pFriendEx->last_logintime);

		abase::hash_map<int,int>::iterator iter = RoleIDSendTimeMap.find(pFriendEx->rid);
		int iDaysSentMail = -1;// never send mail
		if (iter != RoleIDSendTimeMap.end())
			iDaysSentMail = iter->second == 0 ? -1 : GET_DAYS_BETWEEN(iTime,iter->second);	

		if (pFriend && iDaysNoLogin >= CECUIConfig::Instance().GetGameUI().nMailToFriendsDaysNoLogin 
			&& pFriendEx->level >= CECUIConfig::Instance().GetGameUI().nMailToFriendsLevel)
		{
			ACString strColor,strDays,strName(pFriend->GetName());
			if (iDaysSentMail == -1 || iDaysSentMail >= nDaysSendMailConfig)
			{
				strColor = SENDABLE_COLOR;
				m_bNeedAttrackAttention = true;
			}
			else
			{
				strColor = m_pAUIManager->GetStringFromTable(9529);
				if (!strColor.GetLength())
				{
					strColor = GRAY_COLOR;
				}
			}
			FormatDays(iDaysNoLogin,strDays);
			FormatName(strName);

			a_sprintf(szText, _AL("%s%s%s\t%d\t%s"), strColor,strName,colorBackup,pFriendEx->level,strDays);

			m_pFriendsToMailList->AddString(szText);
			m_pFriendsToMailList->SetItemData(index, pFriendEx->rid);
			m_pFriendsToMailList->SetItemData(index, iDaysSentMail, 1);
			// 显示还有多少天可以发信的提示
			if (iDaysSentMail >= 0 && iDaysSentMail < nDaysSendMailConfig)
			{
				a_sprintf(szText,m_pAUIManager->GetStringFromTable(9520),nDaysSendMailConfig - iDaysSentMail);
				m_pFriendsToMailList->SetItemHint(index,szText);
			}
			index++;
		}
	}

	OnEventSelectList(0,0,0);
	OnCommandSelectTemplate(NULL);

	PAUIOBJECT pObj = GetDlgItem("Txt_Left");
	if (pObj)
	{
		ACString strText;
		strText.Format(_AL("%d/5"),min(m_iSendMailPerDay,5));
		pObj->SetText(strText);
	}
	
	pObj = GetDlgItem("Btn_Send");
	if (pObj)
	{
		if (m_iSendMailPerDay >= 5 
			|| GetHostPlayer()->GetBasicProps().iLevel < CECUIConfig::Instance().GetGameUI().nMailToFriendsLevel 
			|| m_iSendMailPerDay >= m_pFriendsToMailList->GetCount())
		{
			pObj->Enable(false);
			m_bNeedAttrackAttention = false;
		}
	}
}

void CDlgMailToFriends::IncSendMailCount(int iFriendID)
{
	m_iSendMailPerDay++;
	PAUIOBJECT pObj = GetDlgItem("Txt_Left");
	if (pObj)
	{
		ACString strText;
		strText.Format(_AL("%d/5"),min(m_iSendMailPerDay,5));
		pObj->SetText(strText);
	}
	int i;
	bool bFound(false);
	for (i = 0;i < m_pFriendsToMailList->GetCount(); ++i)
	{
		if (iFriendID == m_pFriendsToMailList->GetItemData(i))
		{
			bFound = true;
			break;
		}
	}
	
	if (!bFound)
	{
		ASSERT(!"Friend ID returned by server not found in current friend list!");
		return;
	}
	m_pFriendsToMailList->SetItemData(i, 0, 1);
	m_pFriendsToMailList->Enable(true);
	int iSel = i;
	ACString strText(m_pFriendsToMailList->GetText(iSel));
	ACString strNew;
	ACString strGrayColor(m_pAUIManager->GetStringFromTable(9529));
	if (!strGrayColor.GetLength())
	{
		strGrayColor = GRAY_COLOR;
	}
	strNew.Format(_AL("%s%s"),strGrayColor,strText.GetBuffer(0) + (sizeof(GRAY_COLOR) - sizeof(ACHAR)) / sizeof(ACHAR));
	m_pFriendsToMailList->SetText(iSel,strNew);

	pObj = GetDlgItem("Btn_Send");
	if (pObj)
	{
		if (m_iSendMailPerDay >= 5 || m_iSendMailPerDay >= m_pFriendsToMailList->GetCount())
		{
			pObj->Enable(false);
			m_bNeedAttrackAttention = false;
		}
	}
	m_pAUIManager->MessageBox("",m_pAUIManager->GetStringFromTable(9521),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgMailToFriends::SendmailFail()
{
	m_pFriendsToMailList->Enable(true);

	m_pAUIManager->MessageBox("",m_pAUIManager->GetStringFromTable(9522),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
}


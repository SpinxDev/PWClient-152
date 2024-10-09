// Filename	: DlgInfo.cpp
// Creator	: Tom Zhou
// Date		: October 19, 2005

#include "AUICommon.h"
#include "AUIEditBox.h"
#include "AUIListBox.h"
#include "AUIImagePicture.h"
#include "DlgInfo.h"
#include "DlgQuestion.h"
#include "DlgQuestionTask.h"
#include "DlgMinimizeBar.h"
#include "DlgFriendChat.h"
#include "DlgGuildDiplomacyMan.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Friend.h"
#include "ECScriptContext.h"
#include "ECScriptController.h"
#include "ECScript.h"
#include "ECScriptUnitExecutor.h"
#include "EC_TimeSafeChecker.h"
#include "EC_UIHelper.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgInfo, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgInfo, CDlgBase)

AUI_ON_EVENT("F_*",		WM_LBUTTONUP,		OnEventLButtonUp_Icon)
AUI_ON_EVENT("List",	WM_LBUTTONUP,		OnEventLButtonUp_List)

AUI_END_EVENT_MAP()

abase::vector<CDlgInfo::INFORMATION> CDlgInfo::m_vecInfo[INFO_NUM];

CDlgInfo::CDlgInfo()
{
	m_pScriptContext = NULL;
	for( int i = 0; i < INFO_NUM; i++ )
		m_vecInfo[i].clear();
}

CDlgInfo::~CDlgInfo()
{
}

void CDlgInfo::OnCommand_CANCEL(const char * szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgInfo::OnEventLButtonUp_Icon(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int nType = atoi(pObj->GetName() + strlen("F_"));
	CDlgInfo *pDlgList = GetGameUIMan()->m_pDlgInfo;

	if( pDlgList->IsShow() && (int)pDlgList->GetData() == nType )
	{
		pDlgList->Show(false);
		pDlgList->SetData(INFO_NULL);
	}
	else
	{
		pDlgList->BuildInfoList(nType);

		if( m_vecInfo[nType].size() > 1 )
		{
			pDlgList->Show(true);
			pDlgList->SetData(nType);
			pDlgList->AlignTo(this, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT,
				AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_BOTTOM);
		}
		else
		{
			PAUILISTBOX pList = (PAUILISTBOX)pDlgList->GetDlgItem("List");

			pList->SetCurSel(0);
			pDlgList->SetData(nType);
			pDlgList->OnEventLButtonUp_List(0, 0, pList);
			pDlgList->SetData(INFO_NULL);
		}
	}

	((PAUIIMAGEPICTURE)pObj)->FadeInOut(0);
}

void CDlgInfo::OnEventLButtonUp_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PAUILISTBOX pList = (PAUILISTBOX)pObj;
	int nCurSel = pList->GetCurSel();
	if( nCurSel < 0 || nCurSel >= pList->GetCount() ) return;

	char szName[40];
	AString strName;
	int nType = GetData();
	PAUIDIALOG pMsgBox = NULL, pDlgFound = NULL;
	INFORMATION Info, InfoThis = m_vecInfo[nType][nCurSel];

	if( 0 == stricmp(InfoThis.strType, "Game_FriendMsg") )
	{
		int i;

		for( i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
		{
			sprintf(szName, "Win_FriendChat%d", i);
			pMsgBox = GetGameUIMan()->GetDialog(szName);
			if( pMsgBox->GetData() == InfoThis.dwData1 )
			{
				pDlgFound = pMsgBox;
				break;
			}
		}

		if( !pDlgFound )
		{
			for( i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
			{
				sprintf(szName, "Win_FriendChat%d", i);
				pMsgBox = GetGameUIMan()->GetDialog(szName);
				if( pMsgBox->GetData() == 0 )
				{
					pDlgFound = pMsgBox;
					pMsgBox->GetDlgItem("Txt_Chat")->SetText(_AL(""));
					pMsgBox->GetDlgItem("Txt_Content")->SetText(_AL(""));
					break;
				}
			}
		}
		pMsgBox = pDlgFound;

		if( pDlgFound )
		{
			ACHAR szUser[40];
			ACString strText;
			PAUIOBJECT pName = pMsgBox->GetDlgItem("Txt_ToWho");
			PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(pMsgBox->GetDlgItem("Txt_Content"));
			PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(pMsgBox->GetDlgItem("Txt_Chat"));

			CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();
			CECFriendMan::FRIEND *pFriend = pMan->GetFriendByID(InfoThis.dwData1);

			const ACHAR *pszPlayer = NULL;
			if (pFriend)
			{
				//	先从好友列表中查找，即使对方未上线，仍能获取名称
				pszPlayer = pFriend->GetName();
			}
			if (!pszPlayer || a_strlen(pszPlayer) <= 0)
			{
				//	好友列表中名称有误，从缓存中查找，或者自动生成
				pszPlayer = GetGameRun()->GetPlayerName(InfoThis.dwData1, true);
			}

			for( i = 0; i < pList->GetCount(); i++ )
			{
				Info = m_vecInfo[nType][i];
				if( Info.dwData1 != InfoThis.dwData1 ) continue;
				
				CECIvtrItem *pItem = (CECIvtrItem *)Info.dwData3;

				CDlgFriendChat *pDlgFriendChat = (CDlgFriendChat *)(pMsgBox);
				pDlgFriendChat->AddMessageToDisplay(InfoThis.dwData1, pszPlayer, Info.strMsg, pItem);

				AUI_ConvertChatString(pszPlayer, szUser);
				pEdit->SetIsAutoReturn(true);

				pList->DeleteString(i);
				m_vecInfo[nType].erase(m_vecInfo[nType].begin() + i);
				i--;
			}
			if( pFriend && pFriend->nLevel > 0 )
			{
				ACHAR szLevel[10];
				a_sprintf(szLevel, _AL("%d"), pFriend->nLevel);
				strText.Format(GetStringFromTable(548), szUser, szLevel);
			}
			else
				strText.Format(GetStringFromTable(548), szUser, GetStringFromTable(574));
			pName->SetText(strText);
			
			GetGameUIMan()->GetMiniBarMgr()->RestoreDialog(pMsgBox);
			pMsgBox->Show(true);
			pMsgBox->ChangeFocus(pEdit);
			pMsgBox->SetText(pszPlayer);
			pMsgBox->SetData(Info.dwData1);
		}
	}
	else if( 0 == stricmp(InfoThis.strType, "Game_HelpMsg") )
	{
		m_pScriptContext->GetController()->ForceActiveScript(InfoThis.dwData1);
		
		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
		GetGameUIMan()->m_pDlgInfo->SetData(INFO_NULL);
		GetGameUIMan()->m_pDlgInfo->Show(false);
	}
	else if( 0 == stricmp(InfoThis.strType, "Game_InviteBind") )
	{
		GetGameUIMan()->MessageBox("Game_InviteBind", pList->GetText(nCurSel), 
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
	}
	else if( 0 == stricmp(InfoThis.strType, "Game_RequestBind") )
	{
		GetGameUIMan()->MessageBox("Game_RequestBind", pList->GetText(nCurSel), 
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
	}
	else if( 0 == stricmp(InfoThis.strType, "Game_InviteDuel") )
	{
		GetGameUIMan()->MessageBox("Game_InviteDuel", pList->GetText(nCurSel), 
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
	}
	else if( 0 == stricmp(InfoThis.strType, "Game_GivingFor") )
	{
		GetGameUIMan()->MessageBox("", pList->GetText(nCurSel), 
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
	}
	else if( 0 == stricmp(InfoThis.strType, "Game_AskToJoin") )
	{
		GetGameUIMan()->MessageBox("Game_AskToJoin", pList->GetText(nCurSel), 
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(Info.dwData1);
		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
	}
	else if( 0 == stricmp(InfoThis.strType, "Game_MsgBox") )
	{
		GetGameUIMan()->MessageBox("", pList->GetText(nCurSel), 
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
	}
	else if( 0 == stricmp(InfoThis.strType, "Game_Question") )
	{
		GetGameUIMan()->m_pDlgQuestion->Show(true);
		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
	}
	else if (0 == stricmp(InfoThis.strType, "Game_QuestionTask"))
	{
		GetGameUIMan()->m_pDlgQuestionTask->Show(true);
		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
	}
	else if (0 == stricmp(InfoThis.strType, "GDiplomacy_NewApplyNotify"))
	{
		CDlgGuildDiplomacyMan *pDlg = GetGameUIMan()->m_pDlgGuildDiplomacyMan;
		if (!pDlg->IsShow())
			pDlg->Show(true);
		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
	}
	else
	{
		if( 0 == stricmp(InfoThis.strType, "Game_InviteFriend") )
		{
			ACHAR szText[128];

			pMsgBox = GetGameUIMan()->GetDialog("Win_Message1");
			AUI_ConvertChatString(InfoThis.strMsg, szText, false);
			pMsgBox->GetDlgItem("Txt_Message")->SetText(szText);
			pMsgBox->GetDlgItem("Btn_Apply")->Enable(true);
			pMsgBox->GetDlgItem("Btn_Refuse")->Enable(true);
			if( GetHostPlayer()->GetFriendMan()->GetFriendByID(InfoThis.dwData1) )
			{
				pMsgBox->GetDlgItem("Btn_Add")->Enable(false);
				pMsgBox->GetDlgItem("Btn_BlackList")->Enable(false);
			}
			else
			{
				pMsgBox->GetDlgItem("Btn_Add")->Enable(true);
				pMsgBox->GetDlgItem("Btn_BlackList")->Enable(true);
			}
			pMsgBox->Show(true);
		}
		else if( 0 == stricmp(InfoThis.strType, "Game_OK") )
		{
			GetGameUIMan()->MessageBox(InfoThis.strType, InfoThis.strMsg,
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		}
		else if( 0 == stricmp(InfoThis.strType, "Game_InviteChannel") )
		{
			int nPos = InfoThis.strMsg.ReverseFind(_AL(' '));
			int nPWLen = InfoThis.strMsg.Right(InfoThis.strMsg.GetLength() - nPos - 1).ToInt();
			ACString szPW = InfoThis.strMsg.Mid(nPos - nPWLen, nPWLen);
			GetGameUIMan()->MessageBox("Game_ChannelInvite", InfoThis.strMsg.Left(nPos - nPWLen),
				MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetText(szPW);
		}
		else
		{
			GetGameUIMan()->MessageBox(InfoThis.strType, InfoThis.strMsg,
				MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		}

		pList->DeleteString(nCurSel);
		m_vecInfo[nType].erase(m_vecInfo[nType].begin() + nCurSel);
	}

	if( pMsgBox )
	{
		pMsgBox->SetIsModal(false);
		pMsgBox->SetData(InfoThis.dwData1);
		pMsgBox->SetDataPtr((void *)InfoThis.dwData2,"dword");
	}

	if( m_vecInfo[nType].size() <= 0 )
	{
		PAUIIMAGEPICTURE pImage;
		PAUIDIALOG pDlgPop = GetGameUIMan()->m_pDlgInfoIcon;

		sprintf(szName, "Back_%d", nType);
		pImage = (PAUIIMAGEPICTURE)pDlgPop->GetDlgItem(szName);
		pImage->Show(false);

		sprintf(szName, "F_%d", nType);
		pImage = (PAUIIMAGEPICTURE)pDlgPop->GetDlgItem(szName);
		pImage->Show(false);
		pImage->FadeInOut(0);

		Show(false);
	}
}

struct ListboxItemsFilter : public CECGameUIMan::AUI_EditboxItemFilter
{
	bool operator()(EditBoxItemBase *pItem)const
	{
		// 不允许特殊符号存在
		//
		return true;
	}
};

void CDlgInfo::BuildInfoList(int nType)
{
	int i;
	PAUILISTBOX pList = (PAUILISTBOX)GetDlgItem("List");
	CECGameUIMan *pUIMan = GetGameUIMan();

	pList->ResetContent();
	for( i = 0; i < (int)m_vecInfo[nType].size(); i++ )
	{
		ACString strConverted = pUIMan->AUI_FilterEditboxItem(m_vecInfo[nType][i].strMsg, ListboxItemsFilter());
		pList->AddString(strConverted);
		pList->SetItemHint(i, strConverted);
	}
}

void CDlgInfo::AddInfo(int nType, INFORMATION Info)
{
	if( nType == INFO_QUESTION && m_vecInfo[nType].size() >= 1 )
		m_vecInfo[nType].clear();
	if( nType == INFO_QUESTIONTASK && m_vecInfo[nType].size() >= 1 )
		m_vecInfo[nType].clear();
	m_vecInfo[nType].push_back(Info);
}

void CDlgInfo::RemoveInfo(int nType, INFORMATION Info)
{
	// 外部对象不通过点击列表而直接删除提示项
	// 目前只有QuestionTask类型可能使用
	CECGameUIMan* pGameUIMan = CECUIHelper::GetGameUIMan();

	if (INFO_QUESTIONTASK == nType || INFO_QUESTION == nType)
	{
		m_vecInfo[nType].clear();

		PAUIIMAGEPICTURE pImage;
		PAUIDIALOG pDlgPop = CECUIHelper::GetGameUIMan()->m_pDlgInfoIcon;

		char szName[40];
		sprintf(szName, "Back_%d", nType);
		pImage = (PAUIIMAGEPICTURE)pDlgPop->GetDlgItem(szName);
		pImage->Show(false);
		
		sprintf(szName, "F_%d", nType);
		pImage = (PAUIIMAGEPICTURE)pDlgPop->GetDlgItem(szName);
		pImage->Show(false);
		pImage->FadeInOut(0);
	}
	else
	{
		// 未使用，未测试
		abase::vector<INFORMATION>::iterator it;
		for (it = m_vecInfo[nType].begin(); it != m_vecInfo[nType].end(); ++it)
		{
			if (it->strMsg == Info.strMsg) 
			{
				m_vecInfo[nType].erase(it);
				// 删除DlgInfo里的表项
				if (pGameUIMan->m_pDlgInfo->GetData() == nType)
				{
					pGameUIMan->m_pDlgInfo->BuildInfoList(nType);
				}
				return;
			}
		}
	}
}

void CDlgInfo::SetScriptContext(CECScriptContext * pContext)
{
	m_pScriptContext = pContext;
}

void CDlgInfo::OnTick()
{
	if( m_szName != "Win_Pop" ) return;

	char szName[40];
	PAUIIMAGEPICTURE pImage;
	int i, j;
	DWORD dwTimeNow = GetTickCount();
	int nCurType = (int)GetGameUIMan()->m_pDlgInfo->GetData();

	for( i = 0; i < INFO_NUM; i++ )
	{
		for( j = 0; j < (int)m_vecInfo[i].size(); j++ )
		{
			if( CECTimeSafeChecker::ElapsedTime(dwTimeNow, m_vecInfo[i][j].dwLifeBegin) < m_vecInfo[i][j].dwLife )
				continue;

			if( i == INFO_QUESTION )
				GetGameUIMan()->m_pDlgQuestion->Show(true, false, false);
			
			m_vecInfo[i].erase(m_vecInfo[i].begin() + j);
			j--;

			if( nCurType == i )
				GetGameUIMan()->m_pDlgInfo->BuildInfoList(i);

			if( m_vecInfo[i].size() <= 0 )
			{
				sprintf(szName, "Back_%d", i);
				pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
				pImage->Show(false);

				sprintf(szName, "F_%d", i);
				pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
				pImage->Show(false);
				pImage->FadeInOut(0);
			}

			if( m_vecInfo[i].size() <= 0 && nCurType == i )
				GetGameUIMan()->m_pDlgInfo->Show(false);
		}
	}
}

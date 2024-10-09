// Filename	: DlgInputName.cpp
// Creator	: Tom Zhou
// Date		: October 13, 2005

#include "AUIEditBox.h"
#include "AUIListBox.h"
#include "AUIImagePicture.h"
#include "DlgInputName.h"
#include "DlgMiniMap.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Friend.h"
#include "EC_HostPlayer.h"
#include "EC_ManPlayer.h"
#include "EC_World.h"
#include "EC_UIManager.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgInputName, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommand_confirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

CDlgInputName::CDlgInputName()
{
	m_pTxtInput = NULL;
	m_pTxtTitle = NULL;
}

CDlgInputName::~CDlgInputName()
{
}

bool CDlgInputName::OnInitDialog()
{
	DDX_Control("DEFAULT_Txt_Input", m_pTxtInput);
	m_pTxtTitle = GetDlgItem("Lbl_Title");
	return CDlgBase::OnInitDialog();
}

void CDlgInputName::OnCommand_confirm(const char * szCommand)
{
	ACString strName = m_pTxtInput->GetText();
	if( strName.GetLength() <= 0 && 
		!(GetData() == INPUTNAME_FACTION_TITLE || GetData() == INPUTNAME_FRIEND_CHANGE_REMARKS))
		return;
	
	switch (GetData()){
	case INPUTNAME_ADD_GUILD:
		{
			int idPlayer = GetGameRun()->GetPlayerID(strName);
			
			if( ISPLAYERID(idPlayer) )
			{
				CECPlayer *pPlayer = GetWorld()->GetPlayerMan()->GetPlayer(idPlayer);
				if( pPlayer && pPlayer->GetFactionID() > 0 )
					GetGameUIMan()->AddChatMessage(GetStringFromTable(613), GP_CHAT_MISC);
				else
				{
					GetGameSession()->faction_accept_join(idPlayer);
					GetGameUIMan()->AddChatMessage(GetStringFromTable(601), GP_CHAT_MISC);
				}
			}
			else
				GetGameSession()->GetPlayerIDByName(strName, 2);
		}
		break;
	case INPUTNAME_ADD_FRIEND:
		{
			int idPlayer = GetGameRun()->GetPlayerID(strName);
			CECFriendMan::FRIEND* pFriend = GetHostPlayer()->GetFriendMan()->GetFriendByID(idPlayer);
			
			//	In friend list, one player ID may have two names, so check name here !
			if( pFriend && !a_strcmp(strName, pFriend->GetName()) )
				GetGameUIMan()->AddChatMessage(GetStringFromTable(565), GP_CHAT_MISC);
			else
			{
				GetGameSession()->friend_Add(idPlayer, strName);
				GetGameUIMan()->AddChatMessage(GetStringFromTable(533), GP_CHAT_MISC);
			}
		}
		break;
	case INPUTNAME_ADD_GROUP:
		{
			int i, idMax = -1;
			CECFriendMan::GROUP *pGroup;
			CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();
			
			for( i = 0; i < pMan->GetGroupNum(); i++ )
			{
				pGroup = pMan->GetGroupByIndex(i);
				idMax = max(idMax, pGroup->iGroup);
			}
			GetGameSession()->friend_SetGroupName(idMax + 1, strName);
		}
		break;
	case INPUTNAME_RENAME_GROUP:
		{
			int idGroup = (int)GetDataPtr("int");
			
			GetGameSession()->friend_SetGroupName(idGroup, strName);
		}
		break;
	case INPUTNAME_FACTION_TITLE:
		{
			int idPlayer = (int)GetDataPtr("int");
			
			GetGameRun()->GetUIManager()->FilterBadWords(strName);
			GetGameSession()->faction_rename(idPlayer, strName);
		}
		break;
	case INPUTNAME_CHANNEL_INVITE:
		{
			int idRoom = (int)GetDataPtr("int");
			int idPlayer = GetGameRun()->GetPlayerID(strName);
			
			if( idPlayer != 0 )
				GetGameSession()->chatroom_Invite(idRoom, idPlayer);
			else
				GetGameSession()->chatroom_Invite(idRoom, strName);
			GetGameUIMan()->AddChatMessage(GetStringFromTable(633), GP_CHAT_MISC);
		}
		break;
	case INPUTNAME_FRIEND_CHANGE_REMARKS:
		{
			int idPlayer = (int)GetDataPtr("int");
			GetGameSession()->friend_SetRemarks(idPlayer, strName);
		}
		break;
	case INPUTNAME_MAKE_MARK:
	case INPUTNAME_RENAME_MARK:
	case INPUTNAME_RENAME_WORLD_MARK:
		GetGameUIMan()->m_pDlgMiniMapMark->OnNameInput(GetData(), strName);
		break;
	}

	OnCommand_CANCEL("");
}

void CDlgInputName::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
	SetData(INPUTNAME_NULL);
}

void CDlgInputName::ShowForUsage(enumInputNameUsage usage, const ACHAR *defaultText, int nMaxLength, void *pvData, const char *pvDataName)
{
	//	文字长度限制
	m_pTxtInput->SetMaxLength(nMaxLength > 0 ? nMaxLength : 20);

	//	初始文字内容
	ACString strDefaultText;
	if (defaultText){
		int nLength = a_strlen(defaultText);
		if (nLength > 0){
			strDefaultText = defaultText;
			if (nLength > m_pTxtInput->GetMaxLength()){
				strDefaultText.CutRight(nLength - m_pTxtInput->GetMaxLength());
			}
		}
	}
	m_pTxtInput->SetText(strDefaultText);

	//	记录目的
	SetData(usage);

	//	初始化标题
	if (m_pTxtTitle){
		m_pTxtTitle->SetText(GetStringFromTable(usage == INPUTNAME_FRIEND_CHANGE_REMARKS ? 11422 : 11421));
	}

	//	记录辅助参数
	if (pvData){
		SetDataPtr(pvData, pvDataName);
	}

	//	显示
	Show(true, true);
}
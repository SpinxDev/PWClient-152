// Filename	: DlgMailList.cpp
// Creator	: Xiao Zhou
// Date		: 2005/10/31

#include "DlgMailList.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "DlgMailRead.h"
#include "DlgMailWrite.h"
#include "DlgMiniMap.h"
#include "DlgMailOption.h"
#include "CSplit.h"
#include "getmaillist_re.hpp"
#include "deletemail_re.hpp"
#include "preservemail_re.hpp"
#include "getmail_re.hpp"
#include "getmailattachobj_re.hpp"
#include "announcenewmail.hpp"
#include "EC_IvtrItem.h"
#include "DlgQShop.h"
#include "DlgInfo.h"
#include "DlgGivingFor.h"

#include <AUIDef.h>

AUI_BEGIN_COMMAND_MAP(CDlgMailList, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",			OnCommandCancel)
AUI_ON_COMMAND("new",				OnCommandNew)
AUI_ON_COMMAND("read",				OnCommandRead)
AUI_ON_COMMAND("readclick",			OnCommandReadClick)
AUI_ON_COMMAND("delete",			OnCommandDelete)
AUI_ON_COMMAND("sortsubject",		OnCommandSortSubject)
AUI_ON_COMMAND("sortsender",		OnCommandSortSender)
AUI_ON_COMMAND("sortattach",		OnCommandSortAttach)
AUI_ON_COMMAND("sortresttime",		OnCommandSortRestTime)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgMailList, CDlgBase)

AUI_ON_EVENT("Lst_Mail",		WM_RBUTTONUP,		OnEventRButtonUp_ListMail)

AUI_END_EVENT_MAP()

#define CHECK_MAIL_OPERATING if (!m_setToRead.empty() || !m_setToDelete.empty()) return;

CDlgMailList::CDlgMailList()
{
	m_pLst_Mail = NULL;
	m_bSortSubject = false;
	m_bSortSender = false;
	m_bSortAttach = false;
	m_bSortRestTime = false;
	m_pGfxImage = NULL;
}

CDlgMailList::~CDlgMailList()
{
}

bool CDlgMailList::OnInitDialog()
{
	DDX_Control("Lst_Mail", m_pLst_Mail);

	DDX_Control("ImageGfx", m_pGfxImage);
	

	return true;
}

void CDlgMailList::OnShowDialog()
{
	if(m_pGfxImage)
		m_pGfxImage->SetGfx("");
	m_pLst_Mail->ResetContent();
	GetGameSession()->mail_GetList();
}

void CDlgMailList::OnCommandCancel(const char* szCommand)
{
	GetGameUIMan()->EndNPCService();
	if( GetGameUIMan()->m_pDlgMailRead->IsShow() )
		GetGameUIMan()->m_pDlgMailRead->OnCommandCancel("");
	if( GetGameUIMan()->m_pDlgMailWrite->IsShow() )
		GetGameUIMan()->m_pDlgMailWrite->OnCommandCancel("");
	Show(false);
}

void CDlgMailList::OnCommandNew(const char* szCommand)
{
	if( GetGameUIMan()->m_pDlgMailWrite->IsEnabled() )
		GetGameUIMan()->m_pDlgMailWrite->CreateNewMail();
}

void CDlgMailList::OnCommandRead(const char* szCommand)
{
	CHECK_MAIL_OPERATING

	if( GetGameUIMan()->m_pDlgMailRead->IsEnabled() )
	{
		int nSelectCount = m_pLst_Mail->GetSelCount();
		if (nSelectCount == 1) {
			int nSel;
			m_pLst_Mail->GetSelItems(1, &nSel);
			if( nSel >= 0 && nSel < m_pLst_Mail->GetCount() && 
				m_pLst_Mail->GetItemTextColor(nSel) != 0xFF7F7F7F )
			{
				SetWaitingResponse(m_pLst_Mail->GetItemData(nSel));
				GetGameSession()->mail_Get(m_pLst_Mail->GetItemData(nSel));
			}
		} else if (nSelectCount > 1) {
			// TODO
			int * pnSelect = new int[nSelectCount];
			m_pLst_Mail->GetSelItems(nSelectCount, pnSelect);
			m_setToRead.clear();
			int i;
			for (i = 0; i < nSelectCount; i++) {
				m_setToRead.insert(m_pLst_Mail->GetItemData(pnSelect[i]));
			}
			DoReadOne(*m_setToRead.begin());
		}
		
	}
}

void CDlgMailList::OnCommandReadClick(const char* szCommand) {
	if (AUI_PRESS(VK_SHIFT) || AUI_PRESS(VK_CONTROL)) {
		return;
	}
	OnCommandRead(NULL);
}

void CDlgMailList::OnCommandDelete(const char* szCommand)
{
	CHECK_MAIL_OPERATING

	int nSelectCount = m_pLst_Mail->GetSelCount();
	if (nSelectCount == 1) {
		int nSel;
		m_pLst_Mail->GetSelItems(1, &nSel);
		if( nSel >= 0 && nSel < m_pLst_Mail->GetCount() && 
			m_pLst_Mail->GetItemTextColor(nSel) != 0xFF7F7F7F )
		{
			OnEventRButtonUp_ListMail(0, 0, NULL);
			GetGameUIMan()->m_pDlgMailOption->OnCommand("Delete");
		}
	} else if (nSelectCount > 1) {
		int * pnSelect = new int[nSelectCount];
		m_pLst_Mail->GetSelItems(nSelectCount, pnSelect);
		m_setToDelete.clear();
		int i;
		for (i = 0; i < nSelectCount; i++) {
			m_setToDelete.insert(m_pLst_Mail->GetItemData(pnSelect[i]));
		}
		DoDeleteOne(*m_setToDelete.begin());
	}
	
}

void CDlgMailList::OnCommandSortSubject(const char* szCommand)
{
	CHECK_MAIL_OPERATING

	m_bSortSubject = !m_bSortSubject;
	if( m_bSortSubject )
		m_pLst_Mail->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_UNISTRING, 1);
	else
		m_pLst_Mail->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_UNISTRING, 1);
}

void CDlgMailList::OnCommandSortSender(const char* szCommand)
{
	CHECK_MAIL_OPERATING

	m_bSortSender = !m_bSortSender;
	if( m_bSortSender )
		m_pLst_Mail->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_UNISTRING, 2);
	else
		m_pLst_Mail->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_UNISTRING, 2);
}

void CDlgMailList::OnCommandSortAttach(const char* szCommand)
{
	CHECK_MAIL_OPERATING

	m_bSortAttach = !m_bSortAttach;
	if( m_bSortAttach )
		m_pLst_Mail->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_UNISTRING, 3);
	else
		m_pLst_Mail->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_UNISTRING, 3);
}

void CDlgMailList::OnCommandSortRestTime(const char* szCommand)
{
	CHECK_MAIL_OPERATING

	m_bSortRestTime = !m_bSortRestTime;
	if( m_bSortRestTime )
		m_pLst_Mail->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 4);
	else
		m_pLst_Mail->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_INT, 4);
}

void CDlgMailList::OnEventRButtonUp_ListMail(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CHECK_MAIL_OPERATING

	int nSelectCount = m_pLst_Mail->GetSelCount();
	if (nSelectCount == 1) {
		int nSel;
		m_pLst_Mail->GetSelItems(1, &nSel);
		int n = m_pLst_Mail->GetSelCount();
		int idMail = m_pLst_Mail->GetItemData(nSel, 0);
		int idSender = m_pLst_Mail->GetItemData(nSel, 1);
		if( nSel >= 0 && nSel < m_pLst_Mail->GetCount() && 
			m_pLst_Mail->GetItemTextColor(nSel) != 0xFF7F7F7F )
		{
			CSplit s(m_pLst_Mail->GetText(nSel));
			CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
			
			PAUIDIALOG pMenu = (AUIDialog *)GetGameUIMan()->m_pDlgMailOption;
			if( a_stricmp(vec[3], GetStringFromTable(812)) == 0 )
				pMenu->SetData(idMail | 0x10000);
			else
				pMenu->SetData(idMail);
			pMenu->SetDataPtr((void*)idSender);
			
			A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
			int x = GET_X_LPARAM(lParam) - p->X;
			int y = GET_Y_LPARAM(lParam) - p->Y;
			pMenu->SetPosEx(x, y);
			
			pMenu->SetText(m_pLst_Mail->GetText(nSel));
			
			if( a_stricmp(vec[3], GetStringFromTable(812)) == 0 )
			{
				pMenu->GetDlgItem("Preserve")->Enable(false);
				pMenu->GetDlgItem("NoPreserve")->Enable(false);
			}
			else if( a_stricmp(vec[4], GetStringFromTable(795)) == 0 )
			{
				pMenu->GetDlgItem("Preserve")->Enable(false);
				pMenu->GetDlgItem("NoPreserve")->Enable(true);
			}
			else
			{
				pMenu->GetDlgItem("Preserve")->Enable(true);
				pMenu->GetDlgItem("NoPreserve")->Enable(false);
			}
			
			pMenu->GetDlgItem("Reply")->Enable(idSender != 0);
			pMenu->Show(true);
		}
	}
}

void CDlgMailList::MailAction(int idAction, void *pData)
{
	ACString strText;
	if( idAction == MAIL_ACTION_GETMAILLIST_RE )
	{
		if( ((GetMailList_Re *)pData)->retcode == ERR_SUCCESS )
		{
			m_pLst_Mail->ResetContent();
			GMailHeaderVector MailList = ((GetMailList_Re *)pData)->maillist;
			DWORD i;
			for( i = 0; i < MailList.size(); i++ )
			{
				GNET::GMailHeader mail = MailList[i];
				ACString strMailCategory;
				ACString strSubject;
				ACString strMailFrom;
				bool bCategory(false);
				
				switch(mail.sndr_type)
				{
				case _MST_AUCTION:
					strMailCategory = GetStringFromTable(7930);
					strMailFrom = GetStringFromTable(5001);
					switch(mail.sender)
					{
					case _AUCTION_ITEM_SOLD:
						strSubject = GetStringFromTable(5101);
						break;
					case _AUCTION_BID_WIN:
						strSubject = GetStringFromTable(5102);
						break;
					case _AUCTION_BID_LOSE:
						strSubject = GetStringFromTable(5103);
						break;
					case _AUCTION_BID_CANCEL:
						strSubject = GetStringFromTable(5104);
						break;
					case _AUCTION_BID_TIMEOUT:
						strSubject = GetStringFromTable(5105);
						break;
					default:
						strSubject = _AL(" ");
						break;
					}
					break;
				case _MST_BATTLE:
					strMailCategory = GetStringFromTable(7931);
					strMailFrom = GetStringFromTable(5002);
 					switch(mail.sender)
					{
					case _BATTLE_BONUS:
						{
							short bonusType = -1;
							if(mail.title.size() >= sizeof(short) )
							{
								Marshal::OctetsStream(mail.title) >> bonusType;
							}
							strSubject = GetStringFromTable(bonusType == -1 ? 5106:5116);
						}
						break;
					case _BATTLE_WIN_PRIZE:
						strSubject = GetStringFromTable(5107);
						break;
					case _BATTLE_BID_FAILED:
						strSubject = GetStringFromTable(5108);
						break;
					case _BATTLE_BID_WITHDRAW:
						strSubject = GetStringFromTable(5109);
						break;
					default:
						strSubject = _AL(" ");
						break;
					}
					break;

				case _MST_WEBTRADE:
					strMailCategory = GetStringFromTable(7935);
					strMailFrom = GetStringFromTable(5003);
					switch(mail.sender)
					{
					case WEBTRADE_CANCEL_POST:
						strSubject = GetStringFromTable(5110);
						break;
					case WEBTRADE_POST_EXPIRE:
						strSubject = GetStringFromTable(5111);
						break;
					case WEBTRADE_SOLD:
						strSubject = GetStringFromTable(5112);
						break;
					case WEBTRADE_BUY:
						strSubject = GetStringFromTable(5113);
						break;
					case WEBTRADE_POST_FAILED:
						strSubject = GetStringFromTable(5114);
						break;
					default:
						strSubject = _AL(" ");
						break;
					}
					break;
					
				case _MST_SYSAUCTION:
					strMailCategory = GetStringFromTable(7936);
					strMailFrom = GetStringFromTable(5004);
					switch (mail.sender)
					{
					case SYSAUCTION_BID_SUCCESS:
						strSubject = GetStringFromTable(5115);
						break;
					default:
						strSubject = _AL(" ");
						break;
					}
					break;
					
				case _MST_COUNTRYBATTLE:
					strMailCategory = GetStringFromTable(7937);
					strMailFrom = GetStringFromTable(5005);
					switch (mail.sender)
					{
					case COUNTRYBATTLE_PRIZE:
						strSubject = GetStringFromTable(5117);
						break;
					case COUNTRYBATTLE_NOPRIZE:
						strSubject = GetStringFromTable(5118);
						break;
					default:
						strSubject = _AL(" ");
						break;
					}
					break;
				case _MST_PLAYERPRESENT:
					{
						ACString temp;
						int roleid, itemid,count,index,slot;
						switch (mail.sender)
						{
						case PLAYERPRESENT_GIVE:
							{
								Marshal::OctetsStream(mail.title) >> roleid >> itemid >> count;
								strMailCategory = GetStringFromTable(7938);
								strMailFrom = GetStringFromTable(10410);
								strSubject = GetStringFromTable(5119);
								
								break;
							}
						case PLAYERPRESENT_ASK:
							{
								Marshal::OctetsStream(mail.title) >> roleid >> itemid >> index >> slot;
								strMailCategory = GetStringFromTable(7939);
								strMailFrom = GetStringFromTable(10410);
								strSubject = GetStringFromTable(5120);
								
								break;
							}
						default:
							strSubject = _AL(" ");
							break;
						}
						
						break;
					}
				case _MST_PLAYEROFFLINESHOP:
					{
						strMailCategory = GetStringFromTable(7950);
						strMailFrom = GetStringFromTable(5006);					

						switch(mail.sender)
						{
						case _PSHOP_TIMEOUT:
							strSubject = GetStringFromTable(5121);
							break;
						case _PSHOP_DELETED:
							strSubject = GetStringFromTable(5122);
							break;
						case _PSHOP_RETURN_ITEM:
							strSubject = GetStringFromTable(5123);
							break;
						case _PSHOP_RETURN_MONEY:
							strSubject = GetStringFromTable(5124);
							break;
						}
						break;
					}
				case _MST_TANKBATTLE:
					{
						strMailCategory = GetStringFromTable(7951);
						strMailFrom = GetStringFromTable(5007);
						switch (mail.sender)
						{
						case TANKBATTLE_BONOUSE:
							strSubject = GetStringFromTable(5125);
							break;
						case TANKBATTLE_NO_BONOUSE:
							strSubject = GetStringFromTable(5118);
							break;
						default:
							strSubject = _AL(" ");
							break;
						}
					}
					break;
				case _MST_ANTICHEAT:
					{
						strMailCategory = GetStringFromTable(7952);
						strMailFrom = GetStringFromTable(5008);
						strSubject = GetStringFromTable(5126);
						break;
					}
				case _MST_FACTIONRESOURCEBATTLE:
					{
						strMailCategory = GetStringFromTable(7953);
						strMailFrom = GetStringFromTable(5009);
						strSubject = GetStringFromTable(5127);
						break;
					}
				case _MST_NPC:
					if (!bCategory)
					{
						strMailCategory = GetStringFromTable(7933);
						bCategory = true;
					}
					// pass through
				
				case _MST_WEB:
					if (!bCategory)
					{
						strMailCategory = GetStringFromTable(7934);
						bCategory = true;
					}
					// pass through

				default:
					if (!bCategory)
					{
						strMailCategory = GetStringFromTable(7932);
						bCategory = true;
					}
					strMailFrom = ACString((const ACHAR*)mail.sender_name.begin(), mail.sender_name.size() / sizeof (ACHAR));
					strSubject = ACString((const ACHAR*)mail.title.begin(), mail.title.size() / sizeof (ACHAR));
					break;
				}
				ACString strAttach;
				ACString strPreserve;
				ACString strUnRead;
				if( mail.attribute & ( _MA_ATTACH_OBJ | _MA_ATTACH_MONEY) )
				{
					strAttach = GetStringFromTable(812);
					strPreserve = _AL(" ");
				}
				else
				{
					strAttach = _AL(" ");
					if( mail.attribute & _MA_PRESERVE )
						strPreserve = GetStringFromTable(795);
					else
						strPreserve = GetPreserveTime(mail.receiver, mail.attribute & _MA_UNREAD);
				}
				if( mail.attribute & _MA_UNREAD )
					strUnRead = GetStringFromTable(796);
				else
					strUnRead = _AL(" ");

				strText.Format(_AL("%s\t%s\t  %s\t%s\t%s"),strUnRead, strMailCategory + strSubject,
						strMailFrom, strAttach, strPreserve);
				m_pLst_Mail->AddString(strText);
				m_pLst_Mail->SetItemData(i, mail.id, 0);
				if( mail.sndr_type == _MST_PLAYER )
					m_pLst_Mail->SetItemData(i, mail.sender, 1);
				else
					m_pLst_Mail->SetItemData(i, 0, 1);
				m_pLst_Mail->SetItemData(i, mail.receiver, 2);
			}
		}
	}
	else if( idAction == MAIL_ACTION_GETMAIL_RE )
	{
		GetMail_Re *pMail = (GetMail_Re *)pData;
		if( pMail->retcode == ERR_SUCCESS )
		{
			int i;
			bool bUnRead = false;
			for( i = 0; i < m_pLst_Mail->GetCount(); i++ )
			{
				CSplit s(m_pLst_Mail->GetText(i));
				CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
				if( m_pLst_Mail->GetItemData(i) == pMail->mail.header.id )
				{
					m_pLst_Mail->SetItemTextColor(i, 0xFFFFFFFF);
			
					if( a_stricmp(vec[0] , GetStringFromTable(796)) == 0 )
					{
						vec[0] = _AL(" ");
						m_pLst_Mail->SetItemData(i, GetGame()->GetServerGMTTime(), 2);
						if( a_stricmp(vec[3], GetStringFromTable(812)) != 0 )
							vec[4] = GetPreserveTime((int)m_pLst_Mail->GetItemData(i, 2), false);
						strText.Format(_AL("%s\t%s\t%s\t%s\t%s"),
								vec[0], vec[1], vec[2], vec[3], vec[4] );
						m_pLst_Mail->SetText(i, strText);
					}
				}
				if( a_stricmp(vec[0] , GetStringFromTable(796)) == 0 )
					bUnRead = true;
			}
			if( !bUnRead )
			{
				GetGameUIMan()->m_pDlgMiniMap->SetNewMail(-1);
				GetGameSession()->mail_CheckNew();
			}
		}
		if (m_setToRead.empty() || m_setToRead.find(pMail->mail.header.id) == m_setToRead.end()) {
			if( GetGameUIMan()->m_pDlgMailRead->IsEnabled() ) {
				GetGameUIMan()->m_pDlgMailRead->ReadMail(pData);
			}
		} else {
			m_setToRead.erase(pMail->mail.header.id);
			if (!m_setToRead.empty()) {
				DoReadOne(*m_setToRead.begin());
			}
		}
	
	}
	else if( idAction == MAIL_ACTION_GETATTACH_RE )
	{
		GetMailAttachObj_Re *pAttach = (GetMailAttachObj_Re *)pData;
		if( pAttach->retcode == ERR_SUCCESS && 
			pAttach->item_left == 0 && pAttach->money_left == 0 )
		{
			int i;
			for( i = 0; i < m_pLst_Mail->GetCount(); i++ )
			{
				CSplit s(m_pLst_Mail->GetText(i));
				CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
				if( m_pLst_Mail->GetItemData(i) == pAttach->mail_id )
				{
					vec[3] = _AL(" ");
					m_pLst_Mail->SetItemData(i, GetGame()->GetServerGMTTime(), 2);
					vec[4] = GetPreserveTime((int)m_pLst_Mail->GetItemData(i, 2), false);
					strText.Format(_AL("%s\t%s\t%s\t%s\t%s"),
							vec[0], vec[1], vec[2], vec[3], vec[4] );
					m_pLst_Mail->SetText(i, strText);
					break;
				}
			}
		}
		GetGameUIMan()->m_pDlgMailRead->GetAttachRe(pData);
	}
	else if( idAction == MAIL_ACTION_DELETEMAIL_RE )
	{
		DeleteMail_Re *pDeleteMail = (DeleteMail_Re *)pData;
		if( pDeleteMail->retcode == ERR_SUCCESS )
		{
			if( pDeleteMail->mail_id == GetGameUIMan()->m_pDlgMailRead->m_idMail )
				GetGameUIMan()->m_pDlgMailRead->OnCommandCancel("");
			int i;
			for( i = 0; i < m_pLst_Mail->GetCount(); i++ )
				if( m_pLst_Mail->GetItemData(i) == pDeleteMail->mail_id )
				{
					m_pLst_Mail->DeleteString(i);
					if (m_setToDelete.empty() || *m_setToDelete.begin() == *m_setToDelete.rbegin()) {
						// 如果是单条删除或多条删除的最后一条，则选中下一条
						if (m_pLst_Mail->GetCount() > i) {
							// 选中下一条
							SelectRowOnly(i);
						} else if (m_pLst_Mail->GetCount() > 0) {
							// 选中第一条
							SelectRowOnly(0);
						}
					}
					break;
				}
			bool bUnRead = false;
			for( i = 0; i < m_pLst_Mail->GetCount(); i++ )
			{
				CSplit s(m_pLst_Mail->GetText(i));
				CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
				if( a_stricmp(vec[0] , GetStringFromTable(796)) == 0 )
				{
					bUnRead = true;
					break;
				}
			}
			if( !bUnRead )
			{
				GetGameUIMan()->m_pDlgMiniMap->SetNewMail(-1);
				GetGameSession()->mail_CheckNew();
			}
		} else {
			// 删除不成功
			int i;
			for (i = 0; i < m_pLst_Mail->GetCount(); i++) {
				if (m_pLst_Mail->GetItemData(i) == pDeleteMail->mail_id) {
					m_pLst_Mail->SetItemTextColor(i, 0xFFFFFFFF);
					break;
				}
			}
		}

		if (m_setToDelete.find(pDeleteMail->mail_id) != m_setToDelete.end()) {
			m_setToDelete.erase(pDeleteMail->mail_id);
			if (!m_setToDelete.empty()) {
				DoDeleteOne(*m_setToDelete.begin());
			}
		}
	}
	else if( idAction == MAIL_ACTION_PRESERVEMAIL_RE )
	{
		PreserveMail_Re *pPreserveMail = (PreserveMail_Re *)pData;
		if( pPreserveMail->retcode == ERR_SUCCESS )
		{
			int i;
			for( i = 0; i < m_pLst_Mail->GetCount(); i++ )
				if( m_pLst_Mail->GetItemData(i) == pPreserveMail->mail_id )
				{
					m_pLst_Mail->SetItemTextColor(i, 0xFFFFFFFF);
					CSplit s(m_pLst_Mail->GetText(i));
					CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
					if( pPreserveMail->blPreserve )
						vec[4] = GetStringFromTable(795);
					else
					{
						m_pLst_Mail->SetItemData(i, GetGame()->GetServerGMTTime(), 2);
						vec[4] = GetPreserveTime((int)m_pLst_Mail->GetItemData(i, 2), 
							a_stricmp(vec[0], GetStringFromTable(796)) == 0 );
					}
					strText.Format(_AL("%s\t%s\t%s\t%s\t%s"),
							vec[0], vec[1], vec[2], vec[3], vec[4] );
					m_pLst_Mail->SetText(i, strText);

					break;
				}
		}
	}
	else if( idAction == MAIL_ACTION_SENDMAIL_RE )
	{
		GetGameUIMan()->m_pDlgMailWrite->SendMailRe(pData);
	}
	else if( idAction == MAIL_ACTION_ANNOUNCENEWMAIL_RE)
	{
		AnnounceNewMail *p = (AnnounceNewMail *)pData;
		GetGameUIMan()->m_pDlgMiniMap->SetNewMail(p->remain_time);
		if( p->remain_time == 0 && IsShow() )
		{
			m_pLst_Mail->ResetContent();
			GetGameSession()->mail_GetList();
		}
		if( p->present_type & 0x01)
		{
			GetGameUIMan()->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_GivingFor", GetStringFromTable(10402), 30000, 0, 0, 0);
			GetGameUIMan()->m_pDlgGivingFor->ShowGfx(false);
		}
		if( p->present_type & 0x02)
			GetGameUIMan()->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_GivingFor", GetStringFromTable(10403), 30000, 0, 0, 0);
	}
}

ACString CDlgMailList::GetPreserveTime(int nSendTime, bool bUnRead)
{
	ACString strTime;
	if( bUnRead )
		strTime.Format(GetStringFromTable(797), 29 - (GetGame()->GetServerGMTTime() - nSendTime) / (3600 * 24));
	else
		strTime.Format(GetStringFromTable(797), 2 - (GetGame()->GetServerGMTTime() - nSendTime) / (3600 * 24));
	return strTime;
}

void CDlgMailList::SetWaitingResponse(int idMail)
{
	int i;
	for( i = 0; i < m_pLst_Mail->GetCount(); i++ )
		if( m_pLst_Mail->GetItemData(i) == (DWORD)idMail )
		{
			m_pLst_Mail->SetItemTextColor(i, 0xFF7F7F7F);
			break;
		}
}

void CDlgMailList::DoDeleteOne(int iMailID) {
	int i;
	for (i = 0; i < m_pLst_Mail->GetCount(); i++) {
		if (m_pLst_Mail->GetItemData(i) == iMailID) {
			if (IsMailWithItem(i)) {
				// 如果是有附件的，则报错丢弃，然后接着删除下一个
				m_setToDelete.erase(iMailID);
				GetGameUIMan()->AddChatMessage(GetStringFromTable(11411), GP_CHAT_MISC);
				if (!m_setToDelete.empty()) {
					DoDeleteOne(*m_setToDelete.begin());
				}
			} else if (IsMailUnread(i)) {
				// 未读邮件，跳过处理下一个
				m_setToDelete.erase(iMailID);
				if (!m_setToDelete.empty()) {
					DoDeleteOne(*m_setToDelete.begin());
				}
			} else if (IsMailPreserve(i)) {
				// 保留邮件，跳过处理下一个
				m_setToDelete.erase(iMailID);
				if (!m_setToDelete.empty()) {
					DoDeleteOne(*m_setToDelete.begin());
				}
			} else {
				// 没有附件且已读，直接删除
				SetWaitingResponse(iMailID);
				GetGameSession()->mail_Delete(iMailID);
			}
			break;
		}
	}
}

void CDlgMailList::DoReadOne(int iMailID) {
	int i;
	for (i = 0; i < m_pLst_Mail->GetCount(); i++) {
		if (m_pLst_Mail->GetItemData(i) == iMailID) {
			SetWaitingResponse(iMailID);
			GetGameSession()->mail_Get(iMailID);
			break;
		}
	}
}

bool CDlgMailList::IsMailWithItem(int iRow) {
	CSplit s(m_pLst_Mail->GetText(iRow));
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
	if( a_stricmp(vec[3], GetStringFromTable(812)) == 0 ) {
		return true;
	} else {
		return false;
	}
}

bool CDlgMailList::IsMailUnread(int iRow) {
	CSplit s(m_pLst_Mail->GetText(iRow));
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
	if( a_stricmp(vec[0] , GetStringFromTable(796)) == 0 ) {
		return true;
	} else {
		return false;
	}
}

bool CDlgMailList::IsMailPreserve(int iRow) {
	CSplit s(m_pLst_Mail->GetText(iRow));
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
	if (a_stricmp(vec[4], GetStringFromTable(795)) == 0) {
		return true;
	} else {
		return false;
	}
}

void CDlgMailList::SelectRowOnly(int iRow) {
	for (int i = 0; i < m_pLst_Mail->GetCount(); i++) {
		if (i == iRow) {
			m_pLst_Mail->SetSel(i, true);
		} else {
			m_pLst_Mail->SetSel(i, false);
		}
	}
}

// Filename	: DlgMailRead.cpp
// Creator	: Xiao Zhou
// Date		: 2005/10/31

#include "AFI.h"
#include "DlgMailRead.h"
#include "DlgMailList.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_IvtrItem.h"
#include "AUICTranslate.h"
#include "DlgMailWrite.h"
#include "getmail_re.hpp"
#include "getmailattachobj_re.hpp"
#include "gmail"
#include "DlgQShop.h"
#include "DlgGivingFor.h"
#include "EC_Friend.h"
#include "EC_Faction.h"
#include "EC_HostPlayer.h"
#include "EC_UIHelper.h"

AUI_BEGIN_COMMAND_MAP(CDlgMailRead, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCancel)
AUI_ON_COMMAND("getattach",	OnCommandGetAttach)
AUI_ON_COMMAND("reply",		OnCommandReply)

AUI_END_COMMAND_MAP()

CDlgMailRead::CDlgMailRead()
{
	m_pTxt_MailFrom = NULL;
	m_pTxt_Subject = NULL;
	m_pTxt_Content = NULL;
	m_pTxt_AttachGold = NULL;
	m_pImg_AttachItem = NULL;
	m_pBtn_GetAttach = NULL;
	m_pBtn_Reply = NULL;
}

CDlgMailRead::~CDlgMailRead()
{
}

bool CDlgMailRead::OnInitDialog()
{
	DDX_Control("Txt_MailFrom", m_pTxt_MailFrom);
	DDX_Control("Txt_Subject", m_pTxt_Subject);
	DDX_Control("Txt_Content", m_pTxt_Content);
	DDX_Control("Txt_AttachGold", m_pTxt_AttachGold);
	DDX_Control("Img_AttachItem", m_pImg_AttachItem);
	DDX_Control("Btn_GetAttach", m_pBtn_GetAttach);
	DDX_Control("Btn_Reply", m_pBtn_Reply);

	return true;
}

void CDlgMailRead::OnCommandCancel(const char* szCommand)
{
	Show(false);
}

void CDlgMailRead::OnCommandReply(const char* szCommand)
{
	ACString strSubject;
	strSubject.Format(_AL("Re: %s"), m_pTxt_Subject->GetText() );
	if( strSubject.GetLength() > 20 )
		strSubject = strSubject.Left(20);
	GetGameUIMan()->m_pDlgMailWrite->CreateNewMail(m_pTxt_MailFrom->GetText(), strSubject );
	Show(false);
}

void CDlgMailRead::OnCommandGetAttach(const char* szCommand)
{
	m_pBtn_GetAttach->Enable(false);
	GetGameSession()->mail_GetAttachment(m_idMail, _MA_ATTACH_OBJ | _MA_ATTACH_MONEY);
}

void CDlgMailRead::ReadMail(void *pData)
{
	GetMail_Re *pMail = (GetMail_Re *)pData;
	if( pMail->retcode == ERR_MS_MAIL_INV )
	{
		GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(639), GP_CHAT_MISC);
		return;
	}
	else if( pMail->retcode != ERR_SUCCESS)
	{
		GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(642), GP_CHAT_MISC);
		return;
	}

	if( !IsShow() )
		Show(true);
	else
		GetGameUIMan()->BringWindowToTop(this);

	GNET::GMail mail = pMail->mail;
	GNET::GMailHeader mailHeader = mail.header; 
	GNET::GRoleInventory attachItem = mail.attach_obj;
	m_idMail = mailHeader.id;
	m_idSender = mailHeader.sender;

	ACString strSubject;
	ACString strMailFrom;
	ACString strContent;

	// to avoid marshal exception
	try{

	unsigned int aid, item, count;
	__int64 sn;
	int comm_id;
	unsigned int posttype, money;
	unsigned int sa_id;
	ACString strTradeID;
	m_pBtn_Reply->Enable(mailHeader.sndr_type == _MST_PLAYER);
	switch(mailHeader.sndr_type)
	{
	case _MST_AUCTION:
	{
		ACString strCount;
		strMailFrom = GetStringFromTable(5001);
		Marshal::OctetsStream(mailHeader.title) >> aid >> item >> count;		
		if( count > 1 )
			strCount.Format(_AL("(%d)"), count);
		else
			strCount = _AL("");
		CECIvtrItem *pItem = CECIvtrItem::CreateItem(item, 0, count);
		switch(mailHeader.sender)
		{
		case _AUCTION_ITEM_SOLD:
			strSubject = GetStringFromTable(5101);
			strContent.Format(GetStringFromTable(5201),
				aid, pItem->GetName(), strCount);
			break;
		case _AUCTION_BID_WIN:
			strSubject = GetStringFromTable(5102);
			strContent.Format(GetStringFromTable(5202),
				aid, pItem->GetName(), strCount);
			break;
		case _AUCTION_BID_LOSE:
			strSubject = GetStringFromTable(5103);
			strContent.Format(GetStringFromTable(5203),
				aid, pItem->GetName(), strCount);
			break;
		case _AUCTION_BID_CANCEL:
			strSubject = GetStringFromTable(5104);
			strContent.Format(GetStringFromTable(5204),
				aid, pItem->GetName(), strCount);
			break;
		case _AUCTION_BID_TIMEOUT:
			strSubject = GetStringFromTable(5105);
			strContent.Format(GetStringFromTable(5205),
				aid, pItem->GetName(), strCount);
			break;
		}
		delete pItem;
		break;
	}
	case _MST_BATTLE:
		strMailFrom = GetStringFromTable(5002);
 		switch(mailHeader.sender)
		{
		case _BATTLE_BONUS:
			{
				short bonusType = -1;
				if(mailHeader.title.size() >= sizeof(short) )
				{
					Marshal::OctetsStream(mailHeader.title) >> bonusType;
				}
				strSubject = GetStringFromTable(bonusType == -1 ? 5106:5116);
				strContent = GetStringFromTable(bonusType == -1 ? 5206:5221);
			}
			break;
		case _BATTLE_WIN_PRIZE:
			strSubject = GetStringFromTable(5107);
			strContent = GetStringFromTable(5207);
			break;
		case _BATTLE_BID_FAILED:
			strSubject = GetStringFromTable(5108);
			strContent = GetStringFromTable(5208);
			break;
		case _BATTLE_BID_WITHDRAW:
			strSubject = GetStringFromTable(5109);
			strContent = GetStringFromTable(5209);
			break;
		}
		break;
		
	case _MST_WEBTRADE:
		{
			strMailFrom = GetStringFromTable(5003);
			Marshal::OctetsStream(mailHeader.title) >> sn >> posttype >> money >> item >> count >> comm_id;
			char buffer[128] = {0};
			sprintf(buffer, "%d", comm_id);
			//sprintf(buffer, "%I64d", sn);
			strTradeID = AS2AC(buffer);
			if (posttype==1 || posttype==2 || posttype==4)
			{
				ACString strName;
				ACString strCount;
				if (posttype == 2)
				{
					CECIvtrItem *pItem = CECIvtrItem::CreateItem(item, 0, count);
					if (pItem)
					{
						strName = pItem->GetName();
						delete pItem;
						if (count>1){
							strCount.Format(_AL("(%d)"), count);
						}
					}
				}
				int idStringFormat(0);
				switch(mailHeader.sender)
				{
				case WEBTRADE_CANCEL_POST:
					strSubject = GetStringFromTable(5110);
					if (posttype == 1){
						strContent.Format(GetStringFromTable(5210), strTradeID, money);
					}
					else if(posttype==4){
						strContent.Format(GetStringFromTable(5222), strTradeID);
					}
					else{
						strContent.Format(GetStringFromTable(5211), strTradeID, strName, strCount);
					}
					break;

				case WEBTRADE_POST_EXPIRE:
					strSubject = GetStringFromTable(5111);
					if (posttype == 1){
						strContent.Format(GetStringFromTable(5212), strTradeID, money);
					}
					else if(posttype==4){
						strContent.Format(GetStringFromTable(5223), strTradeID);
					}
					else{
						strContent.Format(GetStringFromTable(5213), strTradeID, strName, strCount);
					}
					break;

				case WEBTRADE_SOLD:
					strSubject = GetStringFromTable(5112);
					if (posttype == 1){
						strContent.Format(GetStringFromTable(5214), strTradeID, money);
					}
					else if(posttype==4){
						strContent.Format(GetStringFromTable(5224), strTradeID);
					}
					else{
						strContent.Format(GetStringFromTable(5215), strTradeID, strName, strCount);
					}
					break;
					
				case WEBTRADE_BUY:
					strSubject = GetStringFromTable(5113);
					if (posttype == 1){
						strContent.Format(GetStringFromTable(5216), strTradeID, money);
					}
					else if(posttype==4){
						strContent.Format(GetStringFromTable(5225), strTradeID);
					}
					else{
						strContent.Format(GetStringFromTable(5217), strTradeID, strName, strCount);
					}
					break;
					
				case WEBTRADE_POST_FAILED:
					strSubject = GetStringFromTable(5114);
					if (posttype == 1){
						strContent.Format(GetStringFromTable(5218), strTradeID, money);
					}
					else if(posttype==4){
						strContent.Format(GetStringFromTable(5226), strTradeID);
					}
					else{
						strContent.Format(GetStringFromTable(5219), strTradeID, strName, strCount);
					}
					break;
				}
			}
			else
			{
				// 尚未识别的交易类别
			}
			break;
		}
		
		
	case _MST_SYSAUCTION:
		{
			strMailFrom = GetStringFromTable(5004);
			switch (mailHeader.sender)
			{
			case SYSAUCTION_BID_SUCCESS:
				{
					Marshal::OctetsStream(mailHeader.title) >> sa_id >> item >> count >> money;
					
					ACString strName, strCount;
					CECIvtrItem *pItem = CECIvtrItem::CreateItem(item, 0, count);
					if (pItem)
					{
						strName = pItem->GetName();
						delete pItem;
						if (count>1){
							strCount.Format(_AL("(%d)"), count);
						}
					}
					strSubject = GetStringFromTable(5115);
					strContent.Format(GetStringFromTable(5220), sa_id, strName, strCount, money / 100);
				}
				break;

			default:
				break;
			}
		break;
		}
		
	case _MST_COUNTRYBATTLE:
		{
			strMailFrom = GetStringFromTable(5005);
			switch (mailHeader.sender)
			{
			case COUNTRYBATTLE_PRIZE:
				strSubject = GetStringFromTable(5117);
				strContent = GetStringFromTable(5227);
				break;

			case COUNTRYBATTLE_NOPRIZE:
				strSubject = GetStringFromTable(5118);
				strContent = GetStringFromTable(5228);
				break;
				
			default:
				break;
			}
			break;
		}
	case _MST_PLAYERPRESENT:
		{
			ACString temp;
			int roleid, itemid,index,slot;
			strMailFrom = GetStringFromTable(10410);
			ACString rolename = ACString((const ACHAR*)mailHeader.sender_name.begin(), mailHeader.sender_name.size() / sizeof (ACHAR));
			switch (mailHeader.sender)
			{
			case PLAYERPRESENT_GIVE:
				{
					strSubject = GetStringFromTable(5119);
					strContent.Format(GetStringFromTable(5229), rolename);
				}
				
				break;
				
			case PLAYERPRESENT_ASK:
				{
					Marshal::OctetsStream(mailHeader.title) >> roleid >> itemid >> index >> slot;
					CDlgQShop*pQShopDlg = dynamic_cast<CDlgQShop*>(GetGameUIMan()->GetDialog("Win_QShop"));
					if(pQShopDlg)
					{
						ACString strLevel = _AL("-");
						ACString strProf = _AL("-");
						ACString strHint = GetStringFromTable(10416);
						CECFriendMan::FRIEND* pFriend = GetHostPlayer()->GetFriendMan()->GetFriendByID(roleid);
						if(pFriend)
						{
							strLevel.Format(_AL("%d"),pFriend->nLevel);
							strProf.Format(_AL("%s"),GetGameRun()->GetProfName(pFriend->iProfession));
							strHint = GetStringFromTable(10414);
						}
						else
						{
							Faction_Mem_Info* pMember = g_pGame->GetFactionMan()->GetMember(roleid);	
							if(pMember)
							{
								strLevel.Format(_AL("%d"),pMember->GetLev());
								strProf.Format(_AL("%s"),GetGameRun()->GetProfName(pMember->GetProf()));
								strHint = GetStringFromTable(10415);
							}
						}

						// 目前仅显示好友，帮派成员，陌生人。等级和职业信息先不显示。
						temp.Format(GetStringFromTable(10404), rolename,strHint, GetGameUIMan()->GetItemName(itemid),pQShopDlg->GetFormatTime(index,slot),pQShopDlg->GetCashText(pQShopDlg->GetItemPrice(index,slot)));
						CDlgGivingFor* pDlg = dynamic_cast<CDlgGivingFor*>(GetGameUIMan()->GetDialog("Win_GivingFor"));
						pDlg->SetMailAskForData(roleid,mailHeader.id,itemid,index,slot);
						PAUIDIALOG pMsgDlg;
						if(pDlg->CheckAskForTaskLimitedItem(itemid))
						{
							ACString temp2 = GetStringFromTable(10412);
							temp += _AL("\r") + temp2;
						}

						GetGameUIMan()->MessageBox("Game_MailAskFor",temp,MB_OKCANCEL,A3DCOLORRGB(255,255,255),&pMsgDlg);
						pMsgDlg->SetData(itemid);

						strSubject = GetStringFromTable(5120);
						strContent = temp;
					}
					
					Show(false);
					break;
				}
				
			default:
				break;
			}
			break;
		}
	case _MST_PLAYEROFFLINESHOP:
		{
			strMailFrom = GetStringFromTable(5006);					
			
			switch(mailHeader.sender)
			{
			case _PSHOP_TIMEOUT:
				strSubject = GetStringFromTable(5121);
				strContent = GetStringFromTable(5230);
				break;
			case _PSHOP_DELETED:
				strSubject = GetStringFromTable(5122);
				strContent = GetStringFromTable(5231);
				break;
			case _PSHOP_RETURN_ITEM:
				strSubject = GetStringFromTable(5123);
				strContent = GetStringFromTable(5232);
				break;
			case _PSHOP_RETURN_MONEY:
				strSubject = GetStringFromTable(5124);
				strContent = GetStringFromTable(5233);
				break;
			}
			break;
		}
	case _MST_TANKBATTLE:
		{
			strMailFrom = GetStringFromTable(5007);
			int rank = 0;
			Marshal::OctetsStream(mailHeader.title) >> rank;
			switch (mailHeader.sender)
			{
			case TANKBATTLE_BONOUSE:
				strSubject = GetStringFromTable(5125);
				strContent.Format(GetStringFromTable(5240),rank);
				break;
				
			case TANKBATTLE_NO_BONOUSE:
				strSubject = GetStringFromTable(5118);
				strContent.Format(GetStringFromTable(5241),rank);
				break;
				
			default:
				break;
			}
			break;
		}

	case _MST_ANTICHEAT:
		{
			strMailFrom = GetStringFromTable(5008);
			strSubject = GetStringFromTable(5126);
			int dst_roleid(0), retcode(-1);
			Marshal::OctetsStream(mailHeader.title) >> dst_roleid >> retcode;
			if (!CECUIHelper::GetReportPluginResult(strContent, retcode, dst_roleid, false)){
				GetGameSession()->CacheGetPlayerBriefInfo(1, &dst_roleid, 2);
				CECUIHelper::GetReportPluginResult(strContent, retcode, dst_roleid, true);
			}
			break;
		}
		
	case _MST_FACTIONRESOURCEBATTLE:
		{
			strMailFrom = GetStringFromTable(5009);
			strSubject = GetStringFromTable(5127);
			switch (mailHeader.sender)
			{
			case FACTIONRESOURCEBATTLE_BONUS:
				strContent = GetStringFromTable(5243);
				break;
			case FACTIONRESOURCEBATTLE_NO_BONUS:
				strContent = GetStringFromTable(5244);
				break;
			default:
				ASSERT(false);
				break;
			}
			break;
		}

	default:
		strMailFrom = ACString((const ACHAR*)mailHeader.sender_name.begin(), mailHeader.sender_name.size() / sizeof (ACHAR));
		strSubject = ACString((const ACHAR*)mailHeader.title.begin(), mailHeader.title.size() / sizeof (ACHAR));

		strContent.Empty();
		if (mailHeader.sndr_type != _MST_NPC &&
			mailHeader.sndr_type != _MST_WEB &&
			strMailFrom != _AL("GM"))
		{
			strContent = GetStringFromTable(7940);
		}
		{
			ACString tempContent = ACString((const ACHAR*)mail.context.begin(), mail.context.size() / sizeof (ACHAR));
			if (!tempContent.IsEmpty())
			{
				tempContent = GetGameUIMan()->ConvertChatString(tempContent);
				strContent += tempContent;
			}
		}
	}

	}catch(...) {
		strContent = GetGameUIMan()->GetStringFromTable(889);
	}

	m_pTxt_Subject->SetText(strSubject);
	m_pTxt_MailFrom->SetText(strMailFrom);
	m_pTxt_Content->SetText(strContent);

	ACHAR szText[20];
	if( attachItem.id != 0 && attachItem.count != 0)
	{
		CECIvtrItem *pItem = CECIvtrItem::CreateItem(attachItem.id, attachItem.expire_date, attachItem.count);
		pItem->SetItemInfo((unsigned char*)attachItem.data.begin(), attachItem.data.size());
		pItem->SetProcType(attachItem.proctype);
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_AttachItem->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		const wchar_t* szDesc = pItem->GetDesc();
		AUICTranslate trans;
		if( szDesc )
			m_pImg_AttachItem->SetHint(trans.Translate(szDesc));
		else
			m_pImg_AttachItem->SetHint(_AL(""));
		delete pItem;
		a_sprintf(szText,_AL("%d"), attachItem.count);
		m_pImg_AttachItem->SetText(szText);
	}
	else
	{
		m_pImg_AttachItem->SetCover(NULL, -1);
		m_pImg_AttachItem->SetText(_AL(""));
		m_pImg_AttachItem->SetHint(_AL(""));
	}
	a_sprintf(szText,_AL("%d"), mail.attach_money);
	m_pTxt_AttachGold->SetText(szText);

	m_pBtn_GetAttach->Enable(attachItem.id != 0 && attachItem.count != 0 || mail.attach_money > 0);
}

void CDlgMailRead::GetAttachRe(void *pData)
{
	ACHAR szText[20];
	GetMailAttachObj_Re *pAttach = (GetMailAttachObj_Re *)pData;
	if( pAttach->retcode == ERR_SUCCESS )
	{
		a_sprintf(szText,_AL("%d"), pAttach->money_left);
		m_pTxt_AttachGold->SetText(szText);
		if( pAttach->item_left == 0 )
		{
			m_pImg_AttachItem->SetCover(NULL, -1);
			m_pImg_AttachItem->SetText(_AL(""));
			m_pImg_AttachItem->SetHint(_AL(""));
		}
		else
		{
			a_sprintf(szText,_AL("%d"), pAttach->item_left);
			m_pImg_AttachItem->SetText(szText);
		}
		if( pAttach->item_left > 0 || pAttach->money_left > 0 )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(704), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160));
			m_pBtn_GetAttach->Enable(true);
		}
	}
}
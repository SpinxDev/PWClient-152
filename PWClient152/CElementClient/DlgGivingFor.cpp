/********************************************************************
	created:	2012/1/22
	created:	12:11:2012   16:31
	file base:	DlgGivingFor
	file ext:	cpp
	author:		zhougaomin01305
	
*********************************************************************/

#include "DlgGivingFor.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "CSplit.h"

#include "EC_Friend.h"
#include "EC_Faction.h"

#include "A3DGFXExMan.h"
#include "AUILabel.h"
#include "EC_IvtrItem.h"

#include "EC_HostPlayer.h"
#include "EC_Inventory.h"

#include "EC_Player.h"
#include "playergivepresent_re.hpp"
#include "playeraskforpresent_re.hpp"
#include "getplayeridbyname_re.hpp"

#include "DlgQShop.h"
#include "DlgMailList.h"
#include "EC_ElsePlayer.h"
#include "EC_World.h"
#include "EC_ManPlayer.h"
#include "EC_GFXCaster.h"
#include <locale>
#include <algorithm>
#include "DlgAutoLock.h"
#include "EC_Viewport.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGivingFor, CDlgBase)

AUI_ON_COMMAND("Btn_Search", OnCommandSearch)
AUI_ON_COMMAND("Btn_RightTurn", OnCommandRightTurn )
AUI_ON_COMMAND("Btn_LeftTurn",  OnCommandLeftTurn  )
AUI_ON_COMMAND("Btn_Confirm",   OnCommandConfirm   )
AUI_ON_COMMAND("Rdo_*",			OnCommandRadioFriend   )

AUI_ON_COMMAND("Btn_LeftSortName",   OnCommandLeftSortName)
AUI_ON_COMMAND("Btn_LeftSortLevel",   OnCommandLeftSortLevel)
AUI_ON_COMMAND("Btn_LeftSortProf",    OnCommandLeftSortProf)

AUI_ON_COMMAND("Btn_RightSortName",   OnCommandRightSortName)
AUI_ON_COMMAND("Btn_RightSortLevel",  OnCommandRightSortLevel)
AUI_ON_COMMAND("Btn_RightSortProf",   OnCommandRightSortProf)


AUI_ON_COMMAND("Btn_Cancel", OnCommandCancel)
AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGivingFor, CDlgBase)
AUI_ON_EVENT("Lst_Left",	WM_LBUTTONDBLCLK,	OnEventLButtonDBClick_LeftList)
AUI_ON_EVENT("Lst_Right",	WM_LBUTTONDBLCLK,	OnEventLButtonDBClick_RightList)
AUI_END_EVENT_MAP()

struct SortDataName : public std::binary_function<const SortData&, const SortData&, bool> 
{
	bool operator ()(const SortData& a, const SortData& b) const 
	{ 
		return a.roleName.CompareNoCase(b.roleName) < 0;
	}
};

struct SortDataLevel : public std::binary_function<const SortData&, const SortData&, bool> 
{
	bool operator ()(const SortData& a, const SortData& b) const 
	{ 
		return a.level < b.level;
	}
};

struct SortDataProf : public std::binary_function<const SortData&, const SortData&, bool> 
{
	bool operator ()(const SortData& a, const SortData& b) const 
	{ 
		return a.prof < b.prof;
	}
};

CDlgGivingFor::CDlgGivingFor() :
m_iPlayerType(FRIEND_TYPE)
, m_bGiving(true)
, m_pEditBoxName (NULL)
, m_pListBoxLeft (NULL)
, m_pListBoxRight(NULL)
, m_pLblTitle    (NULL)
, m_pLblUpper    (NULL)
, m_pBtnConfirm  (NULL)
, m_bShowGfx(false)
, m_pBtnSearch(NULL)
{
	
}

CDlgGivingFor::~CDlgGivingFor()
{

}

bool CDlgGivingFor::OnInitDialog()
{
	// // 商城赠送索取
	DDX_Control("Edt_Name",    m_pEditBoxName  );
	DDX_Control("Lst_Left",    m_pListBoxLeft  );
	DDX_Control("Lst_Right",   m_pListBoxRight );
	DDX_Control("Lbl_Title",   m_pLblTitle     );
	DDX_Control("Lbl_Upper",   m_pLblUpper     );
	DDX_Control("Btn_Confirm", m_pBtnConfirm   );
	DDX_Control("Btn_Search" , m_pBtnSearch   );

	
	return true;
}

void CDlgGivingFor::BuildFactionList()
{
	if (m_iPlayerType == FACTION_TYPE)
		BuildLeftList();
}

void CDlgGivingFor::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_rightSortDatas.clear();
	m_pListBoxRight->ResetContent();
	BuildLeftList();
	UpdateUpper();
}


void CDlgGivingFor::BuildLeftList()
{
	m_pListBoxLeft->ResetContent();
	m_leftSortDatas.clear();
	int i,j;

	std::set<int> rightPlayers;
	for(i = 0;i < m_pListBoxRight->GetCount();++i)
	{
		rightPlayers.insert(m_pListBoxRight->GetItemData(i));
	}

	if(m_iPlayerType == FRIEND_TYPE)
	{
		CECFriendMan::GROUP *pGroup;
		CECFriendMan::FRIEND* pFriend;
		CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();
		for( i = 0; i < pMan->GetGroupNum(); i++ )
		{
			pGroup = pMan->GetGroupByIndex(i);
		
			for( j = 0; j < pGroup->aFriends.GetSize(); j++ )
			{
				pFriend = pGroup->aFriends[j];
				if(pFriend && rightPlayers.find(pFriend->id) == rightPlayers.end())
				{
					AddPlayerInfo(true, pFriend->id, pFriend->GetName(), pFriend->nLevel, pFriend->iProfession, pFriend->IsGameOnline(pFriend->status));
				}
			}
		}
	}
	else if(m_iPlayerType == FACTION_TYPE)
	{
		CECFactionMan* pFactionMan = g_pGame->GetFactionMan();
		FactionMemList& memList = pFactionMan->GetMemList();
		Faction_Mem_Info*pMember;
		for( i = 0; i < (int)memList.size(); ++i)
		{
			pMember = memList[i];
			if(rightPlayers.find(pMember->GetID()) == rightPlayers.end() && pMember->GetID() != GetHostPlayer()->GetCharacterID())
			{
				AddPlayerInfo(true, pMember->GetID(), pMember->GetName(), pMember->GetLev(), pMember->GetProf(), pMember->IsOnline());
			}
		}
	}
}

void CDlgGivingFor::OnCommandCancel(const char * szCommand)
{
	Show(false);
}

void CDlgGivingFor::OnCommandRadioFriend(const char *szCommand)
{
	int iPlayerType = atoi(szCommand + strlen("Rdo_")) - 1;
	if(iPlayerType != m_iPlayerType)
	{
		m_iPlayerType = iPlayerType; 
		BuildLeftList();
	}
}

void CDlgGivingFor::OnCommandSearch(const char *szCommand)
{
	ACString strName = m_pEditBoxName->GetText();
	strName.TrimLeft();
	strName.TrimRight();
	if(!strName.IsEmpty() && strName.Compare(GetHostPlayer()->GetName()) != 0 )
	{	
		int idPlayer = GetGameRun()->GetPlayerID(strName);
		if	(idPlayer > 0)
			AddSearchedPlayer(idPlayer, strName);
		else
			GetGameSession()->GetPlayerIDByName(strName, 5);
	}

	m_pEditBoxName->SetText(_AL(""));
}

void CDlgGivingFor::AddSearchedPlayer(int roleid, const ACHAR *szName)
{
	if(AddFriendFactionRole(roleid,false))
	{
		int count = m_pListBoxLeft->GetCount();
		for(int i = 0 ;i < count; ++i)
		{
			if(roleid == (int)m_pListBoxLeft->GetItemData(i))
			{
				RemovePlayerInfo(i,true);
				break;
			}
		}
	}
	else
	{
		CECElsePlayer* pPlayer = GetWorld()->GetPlayerMan()->GetElsePlayer(roleid);
		if (pPlayer)
			AddPlayerInfo(false, roleid, szName, pPlayer->GetBasicProps().iLevel, pPlayer->GetProfession());
		else
			AddPlayerInfo(false, roleid, szName, -1, -1);
	}
}

bool CDlgGivingFor::AddFriendFactionRole(int roleid, bool bLeft)
{
	if(m_iPlayerType == FRIEND_TYPE)
	{
		CECFriendMan::FRIEND* pFriend = GetHostPlayer()->GetFriendMan()->GetFriendByID(roleid);
		if(pFriend)
		{
			AddPlayerInfo(bLeft, roleid, pFriend->GetName(), pFriend->nLevel, pFriend->iProfession, pFriend->IsGameOnline(pFriend->status));
			return true;
		}
	}
	else if(m_iPlayerType == FACTION_TYPE)
	{
		Faction_Mem_Info* pMember = g_pGame->GetFactionMan()->GetMember(roleid);
		
		if(pMember)
		{
			AddPlayerInfo(bLeft, roleid, pMember->GetName(), pMember->GetLev(), pMember->GetProf(), pMember->IsOnline());
			return true;
		}
	}

	return false;
}

void CDlgGivingFor::MultiSelectDeal(bool bLeft)
{
	PAUILISTBOX pListBox = bLeft ? m_pListBoxLeft : m_pListBoxRight;

	int i,count = pListBox->GetSelCount();
	if(count < 1) return;
	
	count = pListBox->GetCount();
	for(i= count - 1;i >= 0;i--)
	{
		if (pListBox->GetSel(i))
		{
			int roleid = pListBox->GetItemData(i);
			
			AddFriendFactionRole(roleid,!bLeft);
			RemovePlayerInfo(i,bLeft);
		}
	}
}

void CDlgGivingFor::OnCommandRightTurn(const char *szCommand)
{
	MultiSelectDeal(true);
}

void CDlgGivingFor::OnCommandLeftTurn(const char *szCommand)
{
	MultiSelectDeal(false);
}

void CDlgGivingFor::SetType(bool bGiving, int itemid, int price, int index, int slot)
{
	m_bGiving = bGiving;
	m_itemId  = itemid;
	m_price   = price;
	m_slot    = slot;
	m_index   = index;

	if(m_bGiving)
	{
		m_pLblTitle->SetText(GetStringFromTable(10405));
		m_pBtnConfirm->SetText(GetStringFromTable(10405));
	}
	else
	{
		m_pLblTitle->SetText(GetStringFromTable(10406));
		m_pBtnConfirm->SetText(GetStringFromTable(10406));
	}
}

bool CDlgGivingFor::CheckAskForTaskLimitedItem(int itemid)
{
	return CECQShopConfig::Instance().CanFilterID(itemid);
}

void CDlgGivingFor::OnCommandConfirm(const char *szCommand)
{
	int count = m_pListBoxRight->GetCount();
	if(count > 0)
	{
		int cash = GetHostPlayer()->GetCash();
		if(m_bGiving && cash < m_price * count)
		{
			GetGameUIMan()->MessageBox("",GetStringFromTable(10401),MB_OK,A3DCOLORRGB(255,255,255));
			return;
		}

		CECIvtrItem *pItem = CECIvtrItem::CreateItem(m_itemId, 0, 1);

		CDlgQShop*pQShop = dynamic_cast<CDlgQShop*>(GetGameUIMan()->GetDialog("Win_QShop"));
		
		ACString temp;
		temp.Format(GetStringFromTable(10408), GetStringFromTable(m_bGiving ? 10405 : 10406), pItem->GetName(), pQShop->GetFormatTime(m_index, m_slot), pQShop->GetCashText(m_price * count) );

		if(CheckAskForTaskLimitedItem(m_itemId))
		{
			ACString temp2 = GetStringFromTable(m_bGiving ? 10412 : 10413);
			temp += _AL("\r") + temp2;
		}

		GetGameUIMan()->MessageBox("Game_GiveOrAskFor",temp,MB_OKCANCEL,A3DCOLORRGB(255,255,255));
		
		delete pItem;
	}
}

void CDlgGivingFor::DoSendGivePresentProtocol()
{
	m_bShowGfx = true;
	int count = m_pListBoxRight->GetCount();
	for(int i = 0;i < count; ++i)
	{
		GetGameSession()->CacheSendGivePresentProtocol(m_pListBoxRight->GetItemData(i), -1, m_itemId, m_index, m_slot);
	}
}
	
void CDlgGivingFor::DoSendAskForPresentProtocol()
{
	int count = m_pListBoxRight->GetCount();
	for(int i = 0;i < count; ++i)
	{
		GetGameSession()->c2s_CmdAskForPresent(m_pListBoxRight->GetItemData(i), m_itemId, m_index, m_slot);
	}
}

void CDlgGivingFor::DoSendProtocol()
{
	Show(false);

	if(m_bGiving)
	{
		if(GetGameUIMan()->m_pDlgAutoLock->IsLocked())
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(10524), GP_CHAT_MISC);
			return;
		}

		DoSendGivePresentProtocol();
	}
	else
		DoSendAskForPresentProtocol();

}

void CDlgGivingFor::SetMailAskForData(int roleid, int mailid, int itemid, int index, int slot)
{
	m_mailAskForDatas.clear();

	m_mailAskForDatas.push_back(roleid);
	m_mailAskForDatas.push_back(mailid);
	m_mailAskForDatas.push_back(itemid);
	m_mailAskForDatas.push_back(index);
	m_mailAskForDatas.push_back(slot);
}

void CDlgGivingFor::DoSendMailGivingFor(bool bSend)
{
	if(!bSend)
	{
		GetGameUIMan()->m_pDlgMailList->OnCommandDelete("");
		return;
	}

	ASSERT(m_mailAskForDatas.size() >= 5 && "DoSendMailGivingFor assert!");

	int roleid = m_mailAskForDatas[0];
	int mailid = m_mailAskForDatas[1];
	int itemid = m_mailAskForDatas[2];
	int index  = m_mailAskForDatas[3]; 
	int slot   = m_mailAskForDatas[4];
	CDlgQShop*pQShop = dynamic_cast<CDlgQShop*>(GetGameUIMan()->GetDialog("Win_QShop"));

	int price = pQShop->GetItemPrice(index,slot);
	int cash = GetHostPlayer()->GetCash();
	if(cash < price)
	{
		GetGameRun()->AddChatMessage(GetStringFromTable(10401), GP_CHAT_MISC);
	}
	else if(GetGameUIMan()->m_pDlgAutoLock->IsLocked())
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(10524), GP_CHAT_MISC);
	}
	else
	{
		m_bShowGfx = true;
		GetGameSession()->CacheSendGivePresentProtocol(roleid, mailid, itemid, index, slot);
	}
}

void CDlgGivingFor::OnEventLButtonDBClick_LeftList(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnCommandRightTurn(NULL);
}
	
void CDlgGivingFor::OnEventLButtonDBClick_RightList(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnCommandLeftTurn(NULL);
}

static bool GetGivePresentGfxPos(float&posX, float&posY, float&scale)
{
	CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUIMan){
		A3DVIEWPORTPARAM* param = g_pGame->GetViewport()->GetA3DViewport()->GetParam();		
		posX = param->X+param->Width*0.5f;
		posY = param->Y+param->Height*0.5f;
		scale = pGameUIMan->GetWindowScale();
		return true;
	}
	return false;
}

void CDlgGivingFor::ShowGfx(bool bSend)
{
	char* pGfxName = NULL;
	if(bSend)
	{
		if(!m_bShowGfx) return;
		m_bShowGfx = false;
		pGfxName = "界面\\小鸟赠送礼物.gfx";
		g_pGame->GetGFXCaster()->PlayTopGFXEx(pGfxName, GetGivePresentGfxPos);
	}
	else
	{
		pGfxName = "界面\\小鸟接收礼物.gfx";
		g_pGame->GetGFXCaster()->PlayAutoGFXEx(pGfxName, 60000, GetHostPlayer()->GetPos(), g_vAxisZ, g_vAxisY, 1.0f);
	}
}

void CDlgGivingFor::OnPrtcPresentRe(GNET::Protocol* pProtocol)
{
	ACString strError;
	using namespace GNET;
	if(pProtocol->GetType() == PROTOCOL_PLAYERGIVEPRESENT_RE)
	{
		PlayerGivePresent_Re*p = dynamic_cast<PlayerGivePresent_Re*>(pProtocol);
		if(p)
		{
			if(p->retcode == 0)
			{
				strError = GetGameUIMan()->GetStringFromTable(10400);
				ShowGfx(true);
			}
			else
				GetGameSession()->GetServerError(p->retcode, strError);
			GetGameRun()->AddChatMessage(strError, GP_CHAT_MISC);
		}
	}
	else
	{
		PlayerAskForPresent_Re*p = dynamic_cast<PlayerAskForPresent_Re*>(pProtocol);
		if(p && p->retcode != 0)
		{		
			GetGameSession()->GetServerError(p->retcode, strError);
			GetGameRun()->AddChatMessage(strError, GP_CHAT_MISC);	
		}
		else if(p && p->retcode == 0)
		{
			GetGameRun()->AddChatMessage(GetStringFromTable(10409), GP_CHAT_MISC);
		}
	}
}

void CDlgGivingFor::OnPrtcGetPlayerIDByNameRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GetPlayerIDByName_Re* p = dynamic_cast<GetPlayerIDByName_Re*>(pProtocol);
	if(p->retcode == ERR_SUCCESS)
	{
		ACString strName((const ACHAR*)p->rolename.begin(), p->rolename.size() / sizeof (ACHAR));
		AddSearchedPlayer(p->roleid, strName);
	}
}

void CDlgGivingFor::AddPlayerInfo(bool bLeft, int roleid, const ACHAR* szRoleName, int level, int profession,bool bOnline)
{
	PAUILISTBOX pListBox = bLeft ? m_pListBoxLeft : m_pListBoxRight;
	std::list<SortData>*pSortDatas = bLeft ? &m_leftSortDatas : &m_rightSortDatas;

	if(!bLeft && m_pListBoxRight->GetCount() >= (m_bGiving ? UPPER_GIVING : UPPER_ASKFOR))
	{
		GetGameUIMan()->MessageBox("",GetStringFromTable(10407),MB_OK,A3DCOLORRGB(255,255,255));
		return;
	}

	ACString temp;
	ACString strLevel;
	ACString strProfession;

	if (level <= 0)
		strLevel = _AL("-");
	else
		strLevel.Format(_AL("%d"),level);

	if(profession < 0)
		strProfession = _AL("-");
	else
		strProfession.Format(_AL("%s"),GetGameRun()->GetProfName(profession));

	int count = pListBox->GetCount();
	for(int i = 0; i < count; ++i)
	{
		if(roleid == (int)pListBox->GetItemData(i))
			return;
	}

	if(bOnline)
		temp.Format(_AL("%s\t%s\t%s"), szRoleName, strLevel, strProfession);
	else
		temp.Format(_AL("^808080%s\t^808080%s\t^808080%s"), szRoleName, strLevel, strProfession);
	pListBox->AddString(temp);
	pListBox->SetItemData(pListBox->GetCount() - 1, roleid);

	SortData sd;
	sd.level  = level;
	sd.roleid = roleid;
	sd.prof   = profession;
	sd.roleName = szRoleName;
	sd.showString = temp;

	pSortDatas->push_back(sd);

	if(!bLeft) UpdateUpper();
}

void CDlgGivingFor::RemovePlayerInfo(int index, bool bLeft)
{
	PAUILISTBOX pListBox = bLeft ? m_pListBoxLeft : m_pListBoxRight;
	
	std::list<SortData>::iterator it,itEnd;
	std::list<SortData>*pSortDatas = bLeft ? &m_leftSortDatas : &m_rightSortDatas;

	int roleid = pListBox->GetItemData(index);

	for(it = pSortDatas->begin(), itEnd = pSortDatas->end(); it != itEnd; ++it)
	{
		if((*it).roleid == roleid)
		{
			pSortDatas->erase(it);
			break;
		}
	}

	pListBox->DeleteString(index);

	if(!bLeft) UpdateUpper();
}

void CDlgGivingFor::UpdateUpper()
{
	int count = m_pListBoxRight->GetCount();

	ACString temp;
	temp.Format(_AL("%d/%d"), count, m_bGiving ? UPPER_GIVING : UPPER_ASKFOR);
	m_pLblUpper->SetText(temp);
}

void CDlgGivingFor::PlayerNameUpdate(int roleid, ACString strNewName, int level, int profession)
{
	int lcount = m_pListBoxLeft->GetCount();
	int rcount = m_pListBoxRight->GetCount();

	ACString temp;
	temp.Format(_AL("%s\t%d\t%s"), strNewName, level, GetGameRun()->GetProfName(profession));

	int i;
	for(i = 0;i < lcount; ++i)
	{
		if(roleid == (int)m_pListBoxLeft->GetItemData(i))
		{
			m_pListBoxLeft->SetText(i,temp);
			break;
		}
	}

	for(i = 0;i < rcount; ++i)
	{
		if(roleid == (int)m_pListBoxRight->GetItemData(i))
		{
			m_pListBoxRight->SetText(i,temp);
			break;
		}
	}

	std::list<SortData>::iterator it,itEnd;
	for(it = m_leftSortDatas.begin(),itEnd = m_leftSortDatas.end(); it != itEnd; ++it)
	{
		if(roleid == (*it).roleid)
		{
			(*it).roleName = strNewName;
			(*it).showString = temp;
			break;
		}
	}

	for(it = m_rightSortDatas.begin(),itEnd = m_rightSortDatas.end(); it != itEnd; ++it)
	{
		if(roleid == (*it).roleid)
		{
			(*it).roleName = strNewName;
			(*it).showString = temp;
			break;
		}
	}
}

void CDlgGivingFor::ResetList(bool bLeft,bool bUp)
{
	PAUILISTBOX pListBox = bLeft ? m_pListBoxLeft : m_pListBoxRight;
	std::list<SortData>*pSortDatas = bLeft ? &m_leftSortDatas : &m_rightSortDatas;

	pListBox->ResetContent();

	if(bUp)
	{
		std::list<SortData>::reverse_iterator it,itEnd;
		for(it = pSortDatas->rbegin(),itEnd = pSortDatas->rend(); it != itEnd; ++it)
		{
			pListBox->AddString((*it).showString);
			pListBox->SetItemData(pListBox->GetCount() - 1, (*it).roleid);
		}
	}
	else
	{
		std::list<SortData>::iterator it,itEnd;
		for(it = pSortDatas->begin(),itEnd = pSortDatas->end(); it != itEnd; ++it)
		{
			pListBox->AddString((*it).showString);
			pListBox->SetItemData(pListBox->GetCount() - 1, (*it).roleid);
		}
	}
	

	if(!bLeft) UpdateUpper();
}

void CDlgGivingFor::OnCommandLeftSortName(const char * szCommand)
{
	static bool bUp = false;
	m_leftSortDatas.sort(SortDataName());
	ResetList(true,bUp);
	bUp = !bUp;
}

void CDlgGivingFor::OnCommandLeftSortLevel(const char * szCommand)
{
	static bool bUp = false;
	m_leftSortDatas.sort(SortDataLevel());
	ResetList(true,bUp);
	bUp = !bUp;
}

void CDlgGivingFor::OnCommandLeftSortProf(const char * szCommand)
{
	static bool bUp = false;
	m_leftSortDatas.sort(SortDataProf());
	ResetList(true,bUp);
	bUp = !bUp;
}

void CDlgGivingFor::OnCommandRightSortName(const char * szCommand)
{
	static bool bUp = false;
	m_rightSortDatas.sort(SortDataName());
	ResetList(false,bUp);
	bUp = !bUp;
}

void CDlgGivingFor::OnCommandRightSortLevel(const char * szCommand)
{
	static bool bUp = false;
	m_rightSortDatas.sort(SortDataLevel());
	ResetList(false,bUp);
	bUp = !bUp;
}

void CDlgGivingFor::OnCommandRightSortProf(const char * szCommand)
{
	static bool bUp = false;
	m_rightSortDatas.sort(SortDataProf());
	ResetList(false,bUp);
	bUp = !bUp;
}
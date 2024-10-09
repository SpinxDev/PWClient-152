// File		: DlgWebTradeBase.cpp
// Creator	: Feng Ning
// Date		: 2010/3/18

#include "DlgWebTradeBase.h"
#include "DlgChat.h"
#include "AUICTranslate.h"
#include "CSplit.h"
#include "AFI.h"
#include "AIniFile.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_IvtrItem.h"
#include "webtradelist_re.hpp"
#include "webtradegetitem_re.hpp"
#include "gwebtraderolebrief"

CDlgWebTradeBase::HintMap CDlgWebTradeBase::s_KnownHints;
CDlgWebTradeBase::CategoryMap CDlgWebTradeBase::s_Category;

#define new A_DEBUG_NEW
#define INVALID_TAG _AL("--")

AUI_BEGIN_COMMAND_MAP(CDlgWebTradeBase, CDlgBase)

AUI_ON_COMMAND("pageup",			OnCommandPageUp)
AUI_ON_COMMAND("pagedown",			OnCommandPageDown)
AUI_ON_COMMAND("pagehome",			OnCommandPageHome)
AUI_ON_COMMAND("pageend",			OnCommandPageEnd)
AUI_ON_COMMAND("sort_*",			OnCommandSortByCommand)
AUI_ON_COMMAND("sortdata_*",		OnCommandSortDataByCommand)
AUI_ON_COMMAND("goto_onlineshop",	OnCommandGotoOnlineShop)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWebTradeBase, CDlgBase)

AUI_ON_EVENT("Lst_Item",	WM_RBUTTONUP,	OnEventRButtonUp_Item)

AUI_END_EVENT_MAP()

using GNET::GWebTradeItem;
using GNET::GWebTradeRoleBrief;

CDlgWebTradeBase::ItemInfo::ItemInfo()
:m_pBasicInfo(NULL)
,m_pItem(NULL)
{
	
}

CDlgWebTradeBase::ItemInfo::ItemInfo(const GWebTradeItem& tradeItem)
{
	m_pBasicInfo = (GWebTradeItem*)tradeItem.Clone();
	m_pItem = NULL;

	CECBaseUIMan* pUIMan = dynamic_cast<CECBaseUIMan*>(g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr());

	if(tradeItem.posttype == POSTTYPE_MONEY)
	{
		m_szItemName.Format(
			pUIMan->GetStringFromTable(5524),
			pUIMan->GetFormatNumber(tradeItem.money) );
	}
	else if(tradeItem.posttype == POSTTYPE_ITEM)
	{
		m_pItem = CECIvtrItem::CreateItem(tradeItem.item_id, 0, tradeItem.item_count);
		
		// format name string
		if( tradeItem.item_count > 1) {
			m_szItemName.Format(_AL("%s(%d)"), m_pItem->GetName(), tradeItem.item_count);
		} else {
			m_szItemName = m_pItem->GetName();
		}

	}
	else if(tradeItem.posttype == POSTTYPE_ROLE)
	{
		m_szItemName = pUIMan->GetStringFromTable(5527);
	}
	else
	{
		// unknown type
		m_szItemName = INVALID_TAG;
	}
}

int CDlgWebTradeBase::ItemInfo::GetTimeValue() const
{
	const GWebTradeItem& tradeItem = GetBasicInfo();

	// these values were used in sort method

	if(STATE_PRE_CANCEL_POST == tradeItem.state)
	{
		return -1;
	}
	else if(STATE_PRE_POST == tradeItem.state)
	{
		return -2; // 
	}
	else if(STATE_POST == tradeItem.state)
	{
		return -3; // tradeItem.post_endtime - mktime(&g_pGame->GetServerLocalTime());
	}
	else if(STATE_SHOW == tradeItem.state)
	{
		return -4; // tradeItem.show_endtime - mktime(&g_pGame->GetServerLocalTime());
	}
	else if(STATE_SELL == tradeItem.state)
	{
		return tradeItem.sell_endtime - mktime(&g_pGame->GetServerLocalTime());
	}
	else
	{
		return 0;
	}
};

ACString CDlgWebTradeBase::ItemInfo::GetTime() const
{
	const GWebTradeItem& tradeItem = GetBasicInfo();
	CECBaseUIMan* pUIMan = dynamic_cast<CECBaseUIMan*>(g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr());

	int nRestTime = -1;

	if(STATE_PRE_POST == tradeItem.state || STATE_PRE_CANCEL_POST == tradeItem.state)
	{
		return pUIMan->GetStringFromTable(5540);
	}
	else if(STATE_POST == tradeItem.state)
	{
		return pUIMan->GetStringFromTable(5543);
	}
	else if(STATE_SHOW == tradeItem.state)
	{
		return pUIMan->GetStringFromTable(5544);
	}
	else if(STATE_SELL == tradeItem.state)
	{
		nRestTime = tradeItem.sell_endtime - mktime(&g_pGame->GetServerLocalTime());
	}
	
	// check the time value
	if(nRestTime < 0)
	{
		return INVALID_TAG;
	}

	return pUIMan->GetFormatTime(nRestTime);
}

ACString CDlgWebTradeBase::ItemInfo::GetFormatedPrice(int price)
{
	CECBaseUIMan* pUIMan = dynamic_cast<CECBaseUIMan*>(g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr());
		
	ACString szRes;
	szRes.Format( pUIMan->GetStringFromTable(5530), 
				  pUIMan->GetFormatNumber(price / 100), price % 100 );
	
	return szRes;
}

ACString CDlgWebTradeBase::ItemInfo::GetFormatedPrice() const
{
	return GetFormatedPrice(GetBasicInfo().price);
}

CDlgWebTradeBase::ItemInfo::~ItemInfo()
{
	delete m_pBasicInfo;
	if(m_pItem) delete m_pItem; 
}

const ACHAR* CDlgWebTradeBase::ItemInfo::GetSellerName() const
{
	const ACHAR* ret = m_szSellerName.IsEmpty() ?
		g_pGame->GetGameRun()->GetPlayerName(m_pBasicInfo->seller_roleid, false):		m_szSellerName;

	return ret != NULL ? ret:INVALID_TAG;
}

const ACHAR* CDlgWebTradeBase::ItemInfo::GetBuyerName() const
{
	const ACHAR* ret = m_szBuyerName.IsEmpty() ?
		g_pGame->GetGameRun()->GetPlayerName(m_pBasicInfo->buyer_roleid, false):
		m_szBuyerName;

	return ret != NULL ? ret:INVALID_TAG;
}

ACString CDlgWebTradeBase::ItemInfo::GetTradeIDHint() const
{
	CECBaseUIMan* pUIMan = dynamic_cast<CECBaseUIMan*>(g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr());

// TODO: remove sn formatting
// #ifdef UNICODE
// 	wchar_t strID[256]={0};
// 	wsprintf(strID, pUIMan->GetStringFromTable(5523), GetBasicInfo().sn);
// #else
// 	char strID[256]={0};
// 	sprintf(strID, pUIMan->GetStringFromTable(5523), GetBasicInfo().sn);
// #endif

	ACString strID, strNum;
	if(GetBasicInfo().commodity_id > 0)
	{
		strNum.Format(_AL("%d"), GetBasicInfo().commodity_id);
	}
	else
	{
		strNum = pUIMan->GetStringFromTable(574);
	}
	
	strID.Format(pUIMan->GetStringFromTable(5526), strNum);
	return strID;
}

bool CDlgWebTradeBase::ItemInfo::CheckNameKnown(AArray<int, int>* pBuf)
{
	bool ret = true;
	
	bool bInGame = g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME;

	//	seller 玩家可能改名，因此始终从ID获取最新名字
	m_szSellerName = g_pGame->GetGameRun()->GetPlayerName(m_pBasicInfo->seller_roleid, false);
	if (m_szSellerName.IsEmpty() && !bInGame)
	{
		//	选人界面如果无法获取玩家名称，使用协议中自带名称（只是显示）
		m_szSellerName = ACString((const ACHAR*)m_pBasicInfo->seller_name.begin(), m_pBasicInfo->seller_name.size() / sizeof (ACHAR));
	}
	if(m_szSellerName.IsEmpty())
	{
		if(m_pBasicInfo->seller_roleid)
		{
			if(pBuf) pBuf->Add(m_pBasicInfo->seller_roleid);
			ret = false;
		}
		else
		{
			m_szSellerName = INVALID_TAG;
		}
	}
	
	//	buyer 玩家可能改名，因此始终从ID获取最新名字
	m_szBuyerName = g_pGame->GetGameRun()->GetPlayerName(m_pBasicInfo->buyer_roleid, false);
	if (m_szBuyerName.IsEmpty() && !bInGame)
	{
		//	选人界面如果无法获取玩家名称，使用协议中自带名称（只是显示）
		m_szBuyerName = ACString((const ACHAR*)m_pBasicInfo->buyer_name.begin(), m_pBasicInfo->buyer_name.size() / sizeof (ACHAR));
	}
	if(m_szBuyerName.IsEmpty())
	{
		if(m_pBasicInfo->buyer_roleid)
		{
			if(pBuf) pBuf->Add(m_pBasicInfo->buyer_roleid);
			ret = false;
		}
		else
		{
			m_szBuyerName = INVALID_TAG;
		}
	}
	return ret;
}

void CDlgWebTradeBase::ItemInfo::SetState(int state)
{
	m_pBasicInfo->state = state;
}

CDlgWebTradeBase::CDlgWebTradeBase()
:m_pLst_Item(NULL)
,m_dwLastUpdateTime(0)
,m_PageMode(PAGE_INVALID)
{
	// clear the flags
	memset(m_bSortFlag, 0, sizeof(m_bSortFlag));
}

CDlgWebTradeBase::~CDlgWebTradeBase()
{
	
}

bool CDlgWebTradeBase::Release(void)
{
	if(m_pLst_Item) ClearItemInfo();
	return CDlgBase::Release();
}

bool CDlgWebTradeBase::OnInitDialog()
{
	m_pLst_Item = dynamic_cast<PAUILISTBOX>(GetDlgItem("Lst_Item"));

	char szItem[20];
	PAUIIMAGEPICTURE pCell;
	for(int i = 0; ; i++ )
	{
		sprintf(szItem, "Item_%02d", i + 1);
		pCell = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pCell ) break;
		m_vecImgCell.push_back(pCell);
	}
	
	InitKnownID();

	return CDlgBase::OnInitDialog();
}

void CDlgWebTradeBase::OnShowDialog()
{
	GetGameUIMan()->m_pDlgWebTradeCurrent = this;
	CDlgBase::OnShowDialog();
}

void CDlgWebTradeBase::OnCommandPageUp(const char* szCommand)
{
	OnPageSwitch(PAGE_UP_FLAG);
}

void CDlgWebTradeBase::OnCommandPageDown(const char* szCommand)
{
	OnPageSwitch(PAGE_DOWN_FLAG);
}

void CDlgWebTradeBase::OnCommandPageHome(const char* szCommand)
{
	OnPageSwitch(PAGE_HOME_FLAG);
}

void CDlgWebTradeBase::OnCommandPageEnd(const char* szCommand)
{
	OnPageSwitch(PAGE_END_FLAG);
}

void CDlgWebTradeBase::OnPageSwitch(int mode)
{
	m_PageMode = mode;

	GetDlgItem("Lab_Searching")->Show(m_PageMode != PAGE_INVALID);
}

void CDlgWebTradeBase::OnCommandGotoOnlineShop(const char* szCommand)
{
	AString strText = GetBaseUIMan()->GetURL("OnlineShop", "URL");
	GetBaseUIMan()->NavigateURL(strText, NULL);
}

void CDlgWebTradeBase::OnCommandSortByCommand(const char* szCommand)
{
	// command format must be "sort_<type>_<id>"

	const char* szCompare = szCommand;

	// check head
	if(strstr(szCompare, "sort_") != szCompare) return;

	// skip the head
	szCompare += 5;

	// check type
	int nSortType = 0;
	if(strstr(szCompare, "uni_") == szCompare) {
		nSortType = AUILISTBOX_SORTTYPE_UNISTRING;
	} else if(strstr(szCompare, "str_") == szCompare) {
		nSortType = AUILISTBOX_SORTTYPE_STRING;
	} else if(strstr(szCompare, "int_") == szCompare) {
		nSortType = AUILISTBOX_SORTTYPE_INT;
	} else if(strstr(szCompare, "flt_") == szCompare) {
		nSortType = AUILISTBOX_SORTTYPE_FLOAT;
	} else {
		ASSERT(false);
	}

	// skip the type
	szCompare += 4;

	// check column
	int nColumn = atoi(szCompare);
	ASSERT( nColumn >= 0 && nColumn < MAX_COLUMN );
	m_bSortFlag[nColumn] = !m_bSortFlag[nColumn];

	// sort by parameters
	m_pLst_Item->SortItems( m_bSortFlag[nColumn] ? AUILISTBOX_SORT_ASCENT : AUILISTBOX_SORT_DESCENT,
							nSortType,
							nColumn);

	// reset all icon
	for(int i = 0; i<m_pLst_Item->GetCount();i++)
	{
		SetIcon(i, NULL);
	}
}


void CDlgWebTradeBase::OnCommandSortDataByCommand(const char* szCommand)
{
	// command format must be "sortdata_<type>_<id>"
	
	const char* szCompare = szCommand;
	
	// check head
	if(strstr(szCompare, "sortdata_") != szCompare) return;
	
	// skip the head
	szCompare += 9;
	
	// check type
	int nSortType = 0;
	if(strstr(szCompare, "int_") == szCompare) {
		nSortType = AUILISTBOX_DATASORTTYPE_INT;
	} else if(strstr(szCompare, "dwd_") == szCompare) {
		nSortType = AUILISTBOX_DATASORTTYPE_DWORD;
	} else {
		ASSERT(false);
	}
	
	// skip the type
	szCompare += 4;
	
	// check column
	int nColumn = atoi(szCompare);
	ASSERT( nColumn >= 0 && nColumn < MAX_COLUMN );
	m_bSortFlag[nColumn] = !m_bSortFlag[nColumn];
	
	// sort by parameters
	m_pLst_Item->SortItemsByData( m_bSortFlag[nColumn] ? AUILISTBOX_SORT_ASCENT : AUILISTBOX_SORT_DESCENT,
								  nSortType,
								  nColumn);
							
	// reset all icon
	for(int i = 0; i<m_pLst_Item->GetCount();i++)
	{
		SetIcon(i, NULL);
	}
}

void CDlgWebTradeBase::OnEventRButtonUp_Item(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int nSel = m_pLst_Item->GetCurSel();
	if(nSel >= 0 && nSel < m_pLst_Item->GetCount())
	{
		ItemInfo* pInfo = GetItemInfo(nSel);
		if(pInfo)
		{
			PAUIDIALOG pMenu = (AUIDialog *)GetGameUIMan()->m_pDlgWebTradeOption;
			pMenu->SetDataPtr(pInfo);
			A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
			int x = GET_X_LPARAM(lParam) - p->X;
			int y = GET_Y_LPARAM(lParam) - p->Y;
			pMenu->SetPosEx(x, y);
			
			pMenu->Show(true);
		}
	}
}

ACString CDlgWebTradeBase::GetRowString(const CDlgWebTradeBase::ItemInfo& tradeItem)
{
	ACString szText;
	
	// rewrite this in derived class to make specific string format

	return szText;
}

void CDlgWebTradeBase::SetItemInfo(int index, CDlgWebTradeBase::ItemInfo* pInfo)
{
	ItemInfo* pOld = (ItemInfo*)m_pLst_Item->GetItemDataPtr(index);
	if(pOld) delete pOld;
	m_pLst_Item->SetItemDataPtr(index, pInfo);
}

void CDlgWebTradeBase::ClearItemInfo()
{
	for(int i=0;i<m_pLst_Item->GetCount();i++)
	{
		SetItemInfo(i, NULL);
	}
}

CDlgWebTradeBase::ItemInfo* CDlgWebTradeBase::GetItemInfo(int index)
{
	return (ItemInfo*)m_pLst_Item->GetItemDataPtr(index);
}

CDlgWebTradeBase::ItemInfo* CDlgWebTradeBase::GetItemInfo(__int64 sn)
{
	for(int i = 0; i < m_pLst_Item->GetCount(); i++)
	{
		__int64 idTrade = GetTradeID(i);
		if(idTrade == sn)
		{
			return GetItemInfo(i);
		}
	}
	
	return NULL;
}

__int64 CDlgWebTradeBase::GetTradeID(int index)
{
	return GetItemInfo(index)->GetBasicInfo().sn;
}

void CDlgWebTradeBase::UpdateHints()
{
	int i;
	for(i = 0; i < m_pLst_Item->GetCount(); i++ )
	{
		if(a_strlen(m_pLst_Item->GetItemHint(i)))
		{
			continue;
		}

		__int64 sn = GetTradeID(i);

		HintMap::iterator it = s_KnownHints.find(sn);
		if(it != s_KnownHints.end())
		{
			ACString szHint;
			szHint.Format(_AL("%s\r%s"), GetItemInfo(i)->GetTradeIDHint(),  it->second);
			m_pLst_Item->SetItemHint(i, szHint);
		}
	}
}

void CDlgWebTradeBase::UpdateHint(int i) {
	if (i >=  m_pLst_Item->GetCount() || i < 0) {
		return;
	}

	__int64 sn = GetTradeID(i);
	
	HintMap::iterator it = s_KnownHints.find(sn);
	if(it != s_KnownHints.end())
	{
		ACString szHint;
		szHint.Format(_AL("%s\r%s"), GetItemInfo(i)->GetTradeIDHint(),  it->second);

		CECIvtrItem* pItem = GetItemInfo(i)->GetIvtrItem();
		if (pItem) {
			pItem->GetDetailDataFromLocal();
			ACString strDesc = GetGameUIMan()->GetItemDescHint(pItem, true, true);
			int index = strDesc.Find(_AL("\t"));
			if (index != -1) {
				strDesc = strDesc.Right(strDesc.GetLength() - index);
				szHint = szHint + strDesc;
			}
		}

		m_pLst_Item->SetItemHint(i, szHint);
	}
}


bool CDlgWebTradeBase::AddHint(__int64 sn, const GNET::Octets& info)
{
	if(CheckHintKnown(sn))
	{
		return false;
	}
	
	ACString strHint;
	try
	{
		GNET::GWebTradeRoleBrief brief;
		Marshal::OctetsStream(info) >> brief;

		ACString gender = (brief.gender < 0 || brief.gender > 1) ? 
							GetStringFromTable(8720) : GetStringFromTable(8650 + brief.gender);

		strHint.Format(GetStringFromTable(5528)
			, GetGameRun()->GetProfName(brief.cls)
			, gender
			, brief.level
			, GetStringFromTable(1001 + brief.level2)
		);

		AUICTranslate trans;
		strHint = trans.Translate(strHint);
	}catch(...) {
		strHint = GetGameUIMan()->GetStringFromTable(857);
	}

	s_KnownHints[sn] = strHint;
	return true;
}

bool CDlgWebTradeBase::AddHint(__int64 sn, const GNET::GRoleInventory& info)
{
	if(CheckHintKnown(sn))
	{
		return false;
	}

	CECIvtrItem *pItem = CECIvtrItem::CreateItem(info.id, info.expire_date, info.count);
	pItem->SetItemInfo((unsigned char*)info.data.begin(), info.data.size());
	pItem->SetProcType(info.proctype);

	ACString strImage = GetGameUIMan()->GetIconsImageString(CECGameUIMan::ICONS_INVENTORY, pItem->GetIconFile());
	ACString strHint = GetGameUIMan()->GetItemDescHint(pItem, true, false);
	if (!strHint.IsEmpty() && !strImage.IsEmpty())
		strHint = strImage + _AL("\r") + strHint;
	s_KnownHints[sn] = strHint;

	delete pItem;

	return true;
}

bool CDlgWebTradeBase::CheckHintKnown(__int64 sn)
{
	HintMap::iterator it = s_KnownHints.find(sn);
	return it != s_KnownHints.end();
}

bool CDlgWebTradeBase::AddCategory(int id, const ACHAR* szName)
{
	if(CheckCategoryKnown(id))
	{
		return false;
	}

	s_Category[id] = szName;

	return true;
}

bool CDlgWebTradeBase::CheckCategoryKnown(int id)
{
	CategoryMap::iterator it = s_Category.find(id);
	return it != s_Category.end();
}

void CDlgWebTradeBase::UpdateItemList(GWebTradeItem* pBegin, size_t size, bool bClear, int bInsert)
{
	DWORD i;

	if(bClear)
	{
		ClearItemInfo();
		m_pLst_Item->ResetContent();
		for(i = 0; i<m_vecImgCell.size();i++)
		{
			m_vecImgCell[i]->SetCover( NULL, -1 );
		}
	}

	AArray<int, int> aUnknowNames;
	AArray<__int64, __int64> aUnknowHints;

	GWebTradeItem* pItem = pBegin;

	int baseIndex = m_pLst_Item->GetCount();
	for(i = 0;i<size;i++)
	{
		GWebTradeItem& tradeItem = *pItem++;

		ItemInfo* pInfo = new ItemInfo(tradeItem);

		// must check the name before add info to list
		pInfo->CheckNameKnown(&aUnknowNames);
		
		if( pInfo->GetBasicInfo().posttype != ItemInfo::POSTTYPE_MONEY
			&& !CheckHintKnown(tradeItem.sn) )
		{
			aUnknowHints.Add(tradeItem.sn);
		}

		if(bClear)
		{
			// insert directly
			if(m_pLst_Item->GetCount() <WEBTRADE_PAGE_SIZE)
			{
				InsertTradeInfo(m_pLst_Item->GetCount(), pInfo);
				pInfo = NULL;
			}
		}
		else
		{
			int ret = DeleteTradeInfo(pInfo->GetBasicInfo().sn);
			if(ret != -1)
			{
				// update the existed information
				InsertTradeInfo(ret, pInfo);
				pInfo = NULL;
			}
			else if(bInsert != INSERT_DISABLE)
			{
				if(bInsert == INSERT_TAIL)
				{
					if(m_pLst_Item->GetCount() <WEBTRADE_PAGE_SIZE)
					{
						InsertTradeInfo(m_pLst_Item->GetCount(), pInfo);
						pInfo = NULL;
					}
				}
				else
				{
					ASSERT(bInsert >= 0 && bInsert < WEBTRADE_PAGE_SIZE);

					InsertTradeInfo(bInsert + i, pInfo);
					pInfo = NULL;
					
					// delete the item out of range
					if(m_pLst_Item->GetCount() > WEBTRADE_PAGE_SIZE)
					{
						__int64 sn = GetTradeID(m_pLst_Item->GetCount() - 1);
						int deleted = DeleteTradeInfo(sn);
						ASSERT(deleted != -1);
					}
				}
			}
		}
		
		if(pInfo != NULL) delete pInfo;
	}

	if(aUnknowNames.GetSize() > 0)
	{
		if (GetGameRun()->GetGameState() == CECGameRun::GS_GAME)
		{
			//	此协议只在ingame状态下调用
			GetGameSession()->CacheGetPlayerBriefInfo(aUnknowNames.GetSize(), aUnknowNames.GetData(), 2);
		}
		else
		{
			ASSERT(false);
			a_LogOutput(1, "CDlgWebTradeBase::UpdateItemList, cannot get player name in login state");
		}
	}

	if(aUnknowHints.GetSize() > 0)
	{
		GetGameSession()->webTrade_GetItems(aUnknowHints.GetSize(), aUnknowHints.GetData());
	}
}

void CDlgWebTradeBase::UpdateText()
{
	for(int i=0;i<m_pLst_Item->GetCount();i++)
	{
		ItemInfo* pInfo = GetItemInfo(i);
		if(pInfo)
		{
			m_pLst_Item->SetText(i, GetRowString(*pInfo));
		}
	}
}

void CDlgWebTradeBase::OnTick()
{
	CDlgBase::OnTick();

	if( !IsShow() )
		return;

	if(m_pLst_Item)
	{
		DWORD dwTime = timeGetTime();
		if(m_dwLastUpdateTime == 0 || dwTime >= m_dwLastUpdateTime + 1000)
		{
			UpdateText();
			m_dwLastUpdateTime = dwTime;
		}
		
		UpdateHints();
	}	
}

void CDlgWebTradeBase::WebTradeAction(int idAction, void *pData)
{
	if( idAction == PROTOCOL_WEBTRADEPREPOST_RE) {

	} else if( idAction == PROTOCOL_WEBTRADEPRECANCELPOST_RE) {

	} else if( idAction == PROTOCOL_WEBTRADELIST_RE) {
		
	} else if( idAction == PROTOCOL_WEBTRADEGETITEM_RE) {

		WebTradeGetItem_Re* pInfos = (WebTradeGetItem_Re*)pData;
		
		size_t item_cnt = 0;
		size_t role_cnt = 0;
		for(size_t i=0;i<pInfos->sns.size();i++)
		{
			ItemInfo* pInfo = GetItemInfo(pInfos->sns[i]);
			if(!pInfo) continue;
			
			if(pInfo->GetBasicInfo().posttype == ItemInfo::POSTTYPE_ITEM)
			{
				AddHint(pInfos->sns[i], pInfos->items[item_cnt++]);
			}
			else if(pInfo->GetBasicInfo().posttype == ItemInfo::POSTTYPE_ROLE)
			{
				AddHint(pInfos->sns[i], pInfos->rolebriefs[role_cnt++]);
			}
		}

	} else if( idAction == PROTOCOL_WEBTRADEATTENDLIST_RE) {

	} else if( idAction == PROTOCOL_WEBTRADEGETDETAIL_RE) {

	} else if( idAction == PROTOCOL_WEBTRADEUPDATE_RE) {
		
	}
}

void CDlgWebTradeBase::SetIcon(int i, CECIvtrItem* pItem)
{
	if((size_t)i<m_vecImgCell.size())
	{
		if(!pItem) pItem = GetItemInfo(i)->GetIvtrItem();
		
		// money has no icon
		if(!pItem) return;

		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_vecImgCell[i]->SetCover( GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
								   GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile] );
	}
}

void CDlgWebTradeBase::InsertTradeInfo(int i, ItemInfo* pInfo)
{
	ASSERT(i >= 0);

	m_pLst_Item->InsertString(i, GetRowString(*pInfo));
	
	SetItemInfo(i, pInfo);
	
	// set row color
	A3DCOLOR rowColor = A3DCOLORRGB(255, 255, 255);
	if((int)pInfo->GetBasicInfo().buyer_roleid == GetHostPlayer()->GetCharacterID()) {
		rowColor = A3DCOLORRGB(255, 255, 0);
	} else if((int)pInfo->GetBasicInfo().seller_roleid == GetHostPlayer()->GetCharacterID()) {
		rowColor = A3DCOLORRGB(255, 255, 255);
	}
	m_pLst_Item->SetItemTextColor(i, rowColor );
	
	// set item icon
	SetIcon(i, pInfo->GetIvtrItem());

	if(pInfo->GetBasicInfo().posttype == ItemInfo::POSTTYPE_MONEY)
	{
		ACString szHint;
		szHint.Format(_AL("%s\r%s"), pInfo->GetTradeIDHint(),  GetStringFromTable(5510));
		m_pLst_Item->SetItemHint(i, szHint);
	}
}

int CDlgWebTradeBase::DeleteTradeInfo(__int64 sn)
{
	for(int i = 0; i < m_pLst_Item->GetCount(); i++)
	{
		__int64 idTrade = GetTradeID(i);
		if(idTrade == sn)
		{
			SetItemInfo(i, NULL);
			m_pLst_Item->DeleteString(i);
			return i;
		}
	}

	return -1;
}


void CDlgWebTradeBase::InitKnownID()
{
	AFileImage fList;
	if( !fList.Open("surfaces\\ingame\\webtradeid.txt",AFILE_OPENEXIST | AFILE_TYPE_BINARY | AFILE_TEMPMEMORY) )
	{
		AUI_ReportError(__LINE__, 1,"CDlgWebViewProduct::InitKnownID(), web trade id file error!");
	}
	else
	{
		ACHAR *szData = (ACHAR*)a_malloctemp(sizeof(ACHAR) * (fList.GetFileLength() / sizeof(ACHAR) + 1));
		DWORD dwLen;
		fList.Read(szData, fList.GetFileLength(), &dwLen);
		fList.Close();
		dwLen /= sizeof(ACHAR);

		ACHAR szCurLine[100];
		int start = 1;
		DWORD pos;
		do
		{
			//	跳过空白字符，找到配置开始位置
			while( start < dwLen && (szData[start] == '\r' || szData[start] == '\n') )
				start++;
			if (start >= dwLen)
				break;

			//	找到配置结束位置
			pos = start;
			while( pos < dwLen && szData[pos] != '\r' && szData[pos] != '\n' )
				pos++;

			//	构造配置并解析
			a_strncpy(szCurLine, szData + start, pos - start);
			szCurLine[pos - start] = 0;

			CSplit s(szCurLine);
			CSplit::VectorAWString vec = s.Split(_AL("\t"));
			AddCategory(a_atoi(vec[0]), vec[2]);

			//	移往下个配置
			start = pos;
		} while(true);
		fList.Close();
		a_freetemp(szData);
	}
}



bool CDlgWebTradeBase::ConvertToDouble(const ACHAR* szInput, int precision, int& iOut, int& iOutTime) const
{
	ACString strInput(szInput);
	strInput.TrimLeft();
	strInput.TrimRight();

	int iPart = 0;
	int iTime = 1;

	int point = -1;
	for(int index=0; index<strInput.GetLength(); index++)
	{
		if(strInput[index] == _AL('.'))
		{
			if(point >= 0)
			{
				return false;
			}

			point++;
			continue;
		}
		else if(strInput[index] < _AL('0') || strInput[index] > _AL('9'))
		{
			return false;
		}

		int v = strInput[index] - _AL('0');
		if(point == -1)
		{
			iPart = iPart * 10 + v;
		}
		else if(++point > precision && precision >= 0)
		{
			return false;
		}
		else
		{
			iPart = iPart * 10 + v;
			iTime *= 10;
		}
	}
	
	// dOut = (double)iPart / iTime;

	// TODO: temp return two result
	// because the FPU CTRL was changed to 24bit float mode
	iOut = iPart;
	iOutTime = iTime;

	return true;
}
// Filename	: DlgAuctionBuyList.cpp
// Creator	: Xiao Zhou
// Date		: 2005/11/10

#include "AFileImage.h"
#include "DlgAuctionBuyList.h"
#include "DlgAuctionList.h"
#include "DlgAuctionSellList.h"
#include "DlgAuctionSearch.h"
#include "DlgFittingRoom.h"
#include "CSplit.h"
#include "AUICommon.h"
#include "AUICTranslate.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrFashion.h"
#include "EC_Utility.h"
#include "AWScriptFile.h"
#include "auctionget_re.hpp"
#include "elementdataman.h"
#include "EC_TimeSafeChecker.h"
#include "A3DDevice.h"
#include "AIniFile.h"

#define new A_DEBUG_NEW

#define LIST_TICK 5000

AUI_BEGIN_COMMAND_MAP(CDlgAuctionBuyList, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",			OnCommandCancel)
AUI_ON_COMMAND("auctionbuylist",	OnCommandAuctionBuyList)
AUI_ON_COMMAND("auctionlist",		OnCommandAuctionList)
AUI_ON_COMMAND("auctionselllist",	OnCommandAuctionSellList)
AUI_ON_COMMAND("auctionmyfavor",	OnCommandAuctionMyFavor)
AUI_ON_COMMAND("searchid",			OnCommandSearchID)
AUI_ON_COMMAND("searchmode",		OnCommandSearchMode)
AUI_ON_COMMAND("buy",				OnCommandBuy)
AUI_ON_COMMAND("binbuy",			OnCommandBinBuy)
AUI_ON_COMMAND("last",				OnCommandLast)
AUI_ON_COMMAND("refresh",			OnCommandRefresh)
AUI_ON_COMMAND("next",				OnCommandNext)
AUI_ON_COMMAND("addtomyfavor",		OnCommandAddToFavor)
AUI_ON_COMMAND("save",				OnCommandSave)
AUI_ON_COMMAND("deleteitem",		OnCommandDelete)
AUI_ON_COMMAND("fit",               OnCommandFit)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAuctionBuyList, CDlgBase)

AUI_ON_EVENT("Lst_Item",WM_LBUTTONDOWN,	OnEventLButtonDown_Lst_Item)

AUI_END_EVENT_MAP()

CDlgAuctionBuyList::CDlgAuctionBuyList()
{
	m_pTxt_AuctionID = NULL;
	m_pTxt_PackGold = NULL;
	m_pTxt_RefreshStatus = NULL;
	m_pTxt_Search = NULL;
	m_nIDNext = -1;
	m_nIDNow = 0;
	m_nSearchID = 0;
	m_bReverse = false;
	m_nPageBegin = 0;
	m_nPageEnd = 0;
	m_nSubID = 0;
}

CDlgAuctionBuyList::~CDlgAuctionBuyList()
{
}

bool CDlgAuctionBuyList::OnInitDialog()
{
	if( stricmp(m_szName, "Win_AuctionBuyList") == 0 )
	{
		DDX_Control("Txt_AuctionID", m_pTxt_AuctionID);
		DDX_Control("Txt_Search", m_pTxt_Search);
		m_pTxt_AuctionID->SetIsNumberOnly(true);
	}
	DDX_Control("Txt_PackGold", m_pTxt_PackGold);
	DDX_Control("Txt_RefreshStatus", m_pTxt_RefreshStatus);
	
	m_pTxt_PackGold->SetIsNumberOnly(true);
	m_mapAuctionID.clear();
	m_mapAuctionName.clear();
	m_mapAuctionHint.clear();
	m_dwLastListTime = GetTickCount();

	if( stricmp(m_szName, "Win_AuctionBuyList") == 0 )
	{
		AWScriptFile s;
		AFileImage aFile;
		if( aFile.Open("surfaces\\ingame\\auctionid.txt", AFILE_TYPE_BINARY | AFILE_OPENEXIST | AFILE_TEMPMEMORY) )
		{
			ACHAR szText[300];
			DWORD dwLen;
			aFile.Read((char*)szText, 2, &dwLen);
			do 
			{
				DWORD len = 0;
				bool b;
				while(  b = aFile.Read((char*)(szText + len), 2, &dwLen) && dwLen > 0 )
				{
					if( szText[len] == '\n' )
					{
						while(len > 0 && (szText[len - 1] == '\r' || szText[len - 1] == '\n') )
							len--;
						int id = a_atoi(szText);
						szText[len] = '\0';
						DWORD i = 0;
						while(i < len && szText[i] != ' ' && szText[i] != '\t')
							i++;
						while(i < len && (szText[i] == ' ' || szText[i] == '\t'))
							i++;
						int auctionid = a_atoi(szText + i);
						while(i < len && szText[i] != ' ' && szText[i] != '\t')
							i++;
						while(i < len && (szText[i] == ' ' || szText[i] == '\t'))
							i++;
						m_mapAuctionID[id] = auctionid;
						/*if( id == auctionid || auctionid > 10000000 )*/
						m_mapAuctionName[id] = szText + i;
						break;
					}
					len++;
				}
				if( !b )
					break;
			} while(true);
			aFile.Close();
		}
		else
			AUI_ReportError(__LINE__, 1,"CDlgAuctionBuyList::OnInitDialog(), auction id file error!");
	}
	
	return CDlgAuctionBase::OnInitDialog();
}

void CDlgAuctionBuyList::OnShowDialog()
{
	CDlgAuctionBase::OnShowDialog();

	if( stricmp(m_szName, "Win_AuctionBuyList") == 0 )
	{
		m_pBtn_AuctionBuyList->SetPushed(true);
	}
	else if (stricmp(m_szName, "Win_AuctionMyAttention") == 0)
	{
		m_pBtn_Btn_MyAttention->SetPushed(true);
		GetDlgItem("Btn_NextPage")->Show(false);
		GetDlgItem("Btn_LastPage")->Show(false);
		OnCommandRefresh("");
	}
	m_pTxt_Price->SetText(_AL("0"));
}

void CDlgAuctionBuyList::OnTick()
{
	CDlgAuctionBase::OnTick();

	DWORD dwTick = GetTickCount();

	if( CECTimeSafeChecker::ElapsedTime(dwTick, m_dwLastListTime) > LIST_TICK && m_nIDNext != -1 )
	{
		if( stricmp(m_szName, "Win_AuctionBuyList") == 0 )
		{
			GetGameSession()->auction_List(m_nIDNext, m_nPageNext, m_bReverse, m_nSubID);
		}
		else if (stricmp(m_szName, "Win_AuctionMyAttention") == 0 )
		{
			int pTemp[20];
			for (int i = 0; i < GetHostPlayer()->GetFavorAucItemNum(); i++)
			{
				pTemp[i] = GetHostPlayer()->GetFavorAucItem(i);
			}
			GetGameSession()->auction_ListUpdate(GetHostPlayer()->GetFavorAucItemNum(), pTemp);
		}
		m_nIDNow = m_nIDNext;
		m_nIDNext = -1;
// 		m_nPage = m_nPageNext;
 		m_nPageNext = 0;
		m_bReverse = false;
		m_dwLastListTime = dwTick;
	}

	m_pTxt_PackGold->SetColor(GetGameUIMan()->GetPriceColor(GetHostPlayer()->GetMoneyAmount()));
	m_pTxt_PackGold->SetText(GetGameUIMan()->GetFormatNumber(GetHostPlayer()->GetMoneyAmount()));

	// Update Btn_TrySkirt status in AuctionBuyList
	if (!stricmp(m_szName, "Win_AuctionBuyList"))
	{
		PAUISTILLIMAGEBUTTON pBtn_Fit = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_TrySkirt");
		int sel = m_pLst_Item->GetCurSel();
		bool flag = (sel >= 0 && sel < m_pLst_Item->GetCount());
		pBtn_Fit->Enable(flag);
	}
}

void CDlgAuctionBuyList::OnCommandSearchID(const char* szCommand)
{
	if( glb_IsTextNotEmpty(m_pTxt_AuctionID) &&
		isdigit(m_pTxt_AuctionID->GetText()[0]) )
	{
		m_pTxt_Price->SetText(_AL("0"));
		m_pLst_Item->ResetContent();
		m_nSearchID = a_atoi(m_pTxt_AuctionID->GetText());
		GetGameSession()->auction_Get(m_nSearchID);
		ACString szText;
		szText.Format(GetStringFromTable(3502), m_nSearchID);
		m_pTxt_RefreshStatus->SetText(szText);
	}
}

void CDlgAuctionBuyList::OnCommandAddToFavor(const char* szCommand)
{
	int nSel = m_pLst_Item->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
	{
		int id = m_pLst_Item->GetItemData(nSel, 0);
		AddToFavor(id);
	}
}

void CDlgAuctionBuyList::OnCommandSearchMode(const char* szCommand)
{
	GetGameUIMan()->m_pDlgAuctionSearch->Show(true, true);
}

void CDlgAuctionBuyList::OnCommandDelete(const char* szCommand)
{
	int nSel = m_pLst_Item->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
	{
		int id = m_pLst_Item->GetItemData(nSel, 0);
		GetHostPlayer()->RemoveFavorAucItem(id);
		m_pLst_Item->DeleteString(nSel);
	}
}
void CDlgAuctionBuyList::OnCommandSave(const char* szCommand)
{

}

void CDlgAuctionBuyList::OnCommandLast(const char* szCommand)
{
	if( m_nPageBegin > 0 )
	{
		m_pTxt_RefreshStatus->SetText(GetStringFromTable(3507));
		m_nPageNext = m_nPageBegin - 1;
		m_nPageBegin--;
		m_nIDNext = m_nIDNow;
		m_bReverse = true;
	}
	else
		GetGameUIMan()->MessageBox("",GetStringFromTable(758), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgAuctionBuyList::OnCommandNext(const char* szCommand)
{
	m_pTxt_RefreshStatus->SetText(GetStringFromTable(3508));
	m_nPageNext = m_nPageEnd + 1;
	m_nPageBegin = m_nPageEnd + 1;
	m_nIDNext = m_nIDNow;
	m_bReverse = false;
}

void CDlgAuctionBuyList::OnCommandRefresh(const char* szCommand)
{
	m_nPageNext = m_nPageBegin;
	m_nIDNext = m_nIDNow;
	m_bReverse = false;
}

void CDlgAuctionBuyList::OnCommandFit(const char *szCommand)
{
	CECIvtrItem * pItem = NULL;
	PAUIIMAGEPICTURE pImage = NULL;

	// Get current selection in auction list
	int sel = m_pLst_Item->GetCurSel();
	if (sel < 0 || sel >= m_pLst_Item->GetCount())
		return;
	
	// Create equipment item for later use
	int templateID = (int)m_pLst_Item->GetItemDataPtr(sel);
	pItem = CECIvtrItem::CreateItem(templateID, 0, 1);
	while (pItem)
	{		
		CDlgFittingRoom * pFittingRoom = (CDlgFittingRoom *)GetGameUIMan()->GetDialog("Win_FittingRoom");

		// Validate it is fashion
		if (!pItem->IsEquipment() || !((CECIvtrEquip *)pItem)->IsFashion())
		{
			// Not fashion
			GetGameUIMan()->AddChatMessage(GetStringFromTable(7731), GP_CHAT_MISC);
			break;
		}
		
		// Get more information from auction id
		int auctionID = (int)m_pLst_Item->GetItemData(sel, 0);
		AuctionHintMap::iterator it = m_mapAuctionHint.find(auctionID);
		if (it == m_mapAuctionHint.end())
		{
			// Auction id invalid
			GetGameUIMan()->AddChatMessage(GetStringFromTable(7732), GP_CHAT_MISC);
			break;
		}
		
		// Set required level¡¢color and gender etc.
		const GNET::GRoleInventory &info = it->second;
		pItem->SetItemInfo((BYTE *)info.data.begin(), info.data.size());
		pItem->SetProcType(info.proctype);
		
		// Validate gender info
		if (((CECIvtrFashion *)pItem)->GetGenderRequirement() != GetHostPlayer()->GetGender())
		{
			// Gender mis-match
			GetGameUIMan()->AddChatMessage(GetStringFromTable(7733), GP_CHAT_MISC);
			break;
		}

		// Validate level info
		if (((CECIvtrFashion *)pItem)->GetLevelRequirement() > GetHostPlayer()->GetMaxLevelSofar())
		{
			// Level mis-match
			GetGameUIMan()->AddChatMessage(GetStringFromTable(7734), GP_CHAT_MISC);
			break;
		}
		
		PAUIOBJECT pImage = pFittingRoom->GetEquipIcon(pItem);
		if(!pImage)
		{
			return;
		}

		// Now everything is ready

		// Show the FittingRoom first
		if (!pFittingRoom->IsShow())
			pFittingRoom->Show(true);

		// Bring the FittingRoom to the top
		GetGameUIMan()->BringWindowToTop(pFittingRoom);
		
		// Use(Override) the item in the FittingRoom later
		pFittingRoom->SetEquipIcon(pItem, pImage);
		break;
	}

	// Delete the item after use
	delete pItem;
}

void CDlgAuctionBuyList::UpdateItem(void *pData)
{
	AuctionGet_Re *pLstItem = (AuctionGet_Re *)pData;
	if( pLstItem->retcode != ERR_SUCCESS )
	{
		m_pTxt_RefreshStatus->SetText(_AL(""));
		return;
	}

	m_pTxt_RefreshStatus->SetText(_AL(""));
	m_nSearchID = 0;
	if( pLstItem->retcode == ERR_AS_BID_NOTFOUND )
	{
		GetGameUIMan()->MessageBox("",GetStringFromTable(668), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	GAuctionDetail pDetail = pLstItem->item;
	GRoleInventory pIvtr = pDetail.item;
	GAuctionItem pAuctionItem = pDetail.info;
	CECIvtrItem *pItem = CECIvtrItem::CreateItem(pIvtr.id, pIvtr.expire_date, pIvtr.count);
	ACString szName,szText, szTime;
	int nRestTime = pAuctionItem.end_time - GetGame()->GetServerGMTTime();
	if( nRestTime > 3600 )
		szTime.Format(GetStringFromTable(1603), nRestTime / 3600);
	else
		szTime.Format(GetStringFromTable(1602), nRestTime / 60);
	if( pAuctionItem.count > 1)
		szName.Format(_AL("%s(%d)"), pItem->GetName(), pAuctionItem.count);
	else
		szName = pItem->GetName();
	if( pAuctionItem.binprice > 0 )
		szText.Format(_AL("%d\t%s\t%s\t%s\t%s"), pAuctionItem.auctionid, szName, 
			szTime, GetGameUIMan()->GetFormatNumber(pAuctionItem.bidprice), 
			GetGameUIMan()->GetFormatNumber(pAuctionItem.binprice));
	else
		szText.Format(_AL("%d\t%s\t%s\t%s\t "), pAuctionItem.auctionid, szName, 
			szTime, GetGameUIMan()->GetFormatNumber(pAuctionItem.bidprice));
	m_pLst_Item->AddString(szText);
	m_pLst_Item->SetItemTextColor(m_pLst_Item->GetCount()-1, GetGameUIMan()->GetPriceColor(pAuctionItem.bidprice), 3);
	m_pLst_Item->SetItemTextColor(m_pLst_Item->GetCount()-1, GetGameUIMan()->GetPriceColor(pAuctionItem.binprice), 4);

	pItem->SetItemInfo((unsigned char*)pIvtr.data.begin(), pIvtr.data.size());
	pItem->SetProcType(pIvtr.proctype);

	ACString strHint = GetGameUIMan()->GetItemDescHint(pItem, true, true);
	m_pLst_Item->SetItemHint(m_pLst_Item->GetCount() - 1, strHint);

	delete pItem;
	m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.auctionid);
	m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.seller, 1);
	m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.bidprice, 2);
	m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.binprice, 3);
	m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.bidder, 4);
	m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.end_time, 5);
	m_pLst_Item->SetCurSel(m_pLst_Item->GetCount());
}

void CDlgAuctionBuyList::UpdateList(void *pData)
{
	m_pTxt_Price->SetText(_AL("0"));
	m_pTxt_RefreshStatus->SetText(_AL(""));
	CDlgAuctionBase::UpdateList(pData);
	if (stricmp(m_szName, "Win_AuctionMyAttention") == 0 )
	{
		GetHostPlayer()->RemoveAllFavorAucItems();
		for (int i= 0; i < m_pLst_Item->GetCount(); i++)
		{
			GetHostPlayer()->AddToFavorAucItem(m_pLst_Item->GetItemData(i, 0));
		}
	}
}

void CDlgAuctionBuyList::AddToFavor(int id)
{
	if (GetHostPlayer()->GetFavorAucItemNum() < 20)
	{
		bool bAdd = GetHostPlayer()->AddToFavorAucItem(id);
		int pTemp[20];
		if (bAdd)
		{
			for (int i = 0; i <GetHostPlayer()->GetFavorAucItemNum(); i++)
			{
				pTemp[i] = GetHostPlayer()->GetFavorAucItem(i);
			}
			GetGameSession()->auction_ListUpdate(GetHostPlayer()->GetFavorAucItemNum(), pTemp);
		}
	}
	else
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7601), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

// Filename	: DlgShop.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005

#include "AFI.h"
#include "DlgShop.h"
#include "EC_ShortcutMgr.h"
#include "DlgInputNO.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_SendC2SCmds.h"
#include "EC_HostPlayer.h"
#include "EC_Resource.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrMoneyConvertible.h"
#include "EC_NPC.h"
#include "EC_ManNPC.h"
#include "EC_NPCServer.h"
#include "EC_DealInventory.h"
#include "EC_World.h"
#include "elementdataman.h"
#include "EC_UIManager.h"
#include "AUIDef.h"
#include "EC_ForceMgr.h"
#include "AUICTranslate.h"
#include "EC_Utility.h"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgShop, CDlgBase)

AUI_ON_COMMAND("set*",			OnCommand_set)
AUI_ON_COMMAND("buy",			OnCommand_buy)
AUI_ON_COMMAND("sell",			OnCommand_sell)
AUI_ON_COMMAND("repair",		OnCommand_repair)
AUI_ON_COMMAND("repairall",		OnCommand_repairall)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgShop, CDlgBase)

AUI_ON_EVENT("U_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("Buy_*",	WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("Sell_*",	WM_LBUTTONDOWN,		OnEventLButtonDown)

AUI_ON_EVENT("U_*",		WM_LBUTTONDBLCLK,	OnEventLButtonDblclk)
AUI_ON_EVENT("Buy_*",	WM_LBUTTONDBLCLK,	OnEventLButtonDblclk)
AUI_ON_EVENT("Sell_*",	WM_LBUTTONDBLCLK,	OnEventLButtonDblclk)

AUI_ON_EVENT("U_*",		WM_RBUTTONUP,	    OnEventLButtonDblclk)
AUI_ON_EVENT("Buy_*",	WM_RBUTTONUP,	    OnEventLButtonDblclk)
AUI_ON_EVENT("Sell_*",	WM_RBUTTONUP,	    OnEventLButtonDblclk)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgShop
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgShop> ShopClickShortcut;
//------------------------------------------------------------------------

CDlgShop::CDlgShop()
{
	m_nCurShopSet = 1;
	m_pNPCSellService = NULL;
}

CDlgShop::~CDlgShop()
{
}

bool CDlgShop::OnInitDialog()
{
	int i;
	char szItem[40];

	m_pTxt_Gold = (PAUILABEL)GetDlgItem("Txt_Gold");
	m_pTxt_Worth1 = (PAUILABEL)GetDlgItem("Txt_Worth1");
	m_pTxt_Worth2 = (PAUILABEL)GetDlgItem("Txt_Worth2");
	m_pTxt_Contrib = (PAUILABEL)GetDlgItem("Txt_Contrib");
	m_pTxt_Contrib01 = (PAUILABEL)GetDlgItem("Txt_Contrib01");

	m_pTxt_TotalForceContrib = (PAUILABEL)GetDlgItem("Txt_TotalForceContrib");
	m_pTxt_PlayerForceContrib = (PAUILABEL)GetDlgItem("Txt_PlayerForceContrib");

	for( i = 0; i < IVTRSIZE_NPCPACK; i++ )
	{
		sprintf(szItem, "U_%02d", i + 1);
		m_pImgU[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	for( i = 0; i < IVTRSIZE_BUYPACK; i++ )
	{
		sprintf(szItem, "Buy_%02d", i + 1);
		m_pImgBuy[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	for( i = 0; i < IVTRSIZE_SELLPACK; i++ )
	{
		sprintf(szItem, "Sell_%02d", i + 1);
		m_pImgSell[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	for( i = 0; i < NUM_NPCIVTR; i++ )
	{
		sprintf(szItem, "Btn_Set%d", i + 1);
		m_pBtnSet[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(szItem);
	}

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ShopClickShortcut(this));

	return true;
}

void CDlgShop::OnShowDialog()
{
	// 如果包内有元宝换钱的物品，取最新汇率
	CECInventory* pPack = GetHostPlayer()->GetPack();
	if( pPack->FindItemByType(CECIvtrItem::ICID_MONEYCONVERTIBLE) )
	{
		::c2s_SendCmdGetCashMoneyRate();
	}
}

void CDlgShop::OnCommand_set(const char * szCommand)
{
	UpdateShop(atoi(szCommand + strlen("set")));
}

void CDlgShop::OnCommand_buy(const char * szCommand)
{
	while (true)
	{
		//	检查金钱数量
		int nGold = (int)m_pTxt_Gold->GetData();
		int nCost = (int)m_pTxt_Worth1->GetData();
		if( nGold < nCost )
		{
			//	金钱不足
			PAUIDIALOG pMsgBox;
			m_pAUIManager->MessageBox("", m_pAUIManager->GetStringFromTable(222),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			break;
		}

		//	检查贡献度数量
		int nContrib = (int)m_pTxt_Contrib->GetData();
		int nContribCost = (int)m_pTxt_Contrib01->GetData();
		if (nContrib < nContribCost)
		{
			//	贡献度不足
			PAUIDIALOG pMsgBox;
			m_pAUIManager->MessageBox("", m_pAUIManager->GetStringFromTable(9163),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			break;
		}

		//	检查战功数量
		int nForceContribPlayer = (int)m_pTxt_PlayerForceContrib->GetData();
		int nForceContribCost = (int)m_pTxt_TotalForceContrib->GetData();
		if (nForceContribPlayer < nForceContribCost)
		{
			//	战功不足
			PAUIDIALOG pMsgBox;
			m_pAUIManager->MessageBox("", m_pAUIManager->GetStringFromTable(9430),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			break;
		}

		//	条件满足，购买物品
		GetHostPlayer()->BuyItemsFromNPC();
		break;
	}
	GetGameUIMan()->m_bRepairing = false;
}

void CDlgShop::OnCommand_sell(const char * szCommand)
{
	__int64 nTotleMoney = (__int64)m_pTxt_Worth2->GetData64();
	if( nTotleMoney + GetHostPlayer()->GetMoneyAmount() > GetHostPlayer()->GetMaxMoneyAmount() )
	{
		m_pAUIManager->MessageBox("", m_pAUIManager->GetStringFromTable(10016), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	if(szCommand == NULL)
	{
		// double check information
		m_pAUIManager->MessageBox("Game_SellOut", m_pAUIManager->GetStringFromTable(306),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else
	{
		CECDealInventory* pSellPack = GetHostPlayer()->GetSellPack();
		for (int i=0; i < pSellPack->GetSize(); i++)
		{
			CECIvtrItem* pItem = pSellPack->GetItem(i);
			if(pItem)
			{
				if(pItem->IsRare())
				{
					// make the double check
					m_pAUIManager->MessageBox("Game_SellOutDbCheck", m_pAUIManager->GetStringFromTable(243),
								MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
					return;
				}
			}
		}

		// otherwise call the normal process
		m_pAUIManager->MessageBox("Game_SellOut", m_pAUIManager->GetStringFromTable(243),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgShop::OnCommand_repair(const char * szCommand)
{
	GetGameUIMan()->m_bRepairing = true;
	GetGame()->ChangeCursor(RES_CUR_REPAIR);
}

void CDlgShop::OnCommand_repairall(const char * szCommand)
{
	CECInventory *pPackEquip = GetHostPlayer()->GetPack(IVTRTYPE_EQUIPPACK);

	if( pPackEquip->HasDisrepairItem() )
	{
		ACString strText;

		strText.Format(m_pAUIManager->GetStringFromTable(242),
			max(pPackEquip->GetRepairAllItemCost(), 1));
		m_pAUIManager->MessageBox("Game_RepairAll", strText, MB_OKCANCEL,
			A3DCOLORRGBA(255, 255, 255, 160));
	}
	else
	{
		m_pAUIManager->MessageBox("", m_pAUIManager->GetStringFromTable(252),
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgShop::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
	m_pNPCSellService = NULL;
	GetGameUIMan()->EndNPCService();
	GetGameUIMan()->m_bRepairing = false;
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

bool CDlgShop::Tick(void)
{
	if( IsShow() ) UpdatePlayer();

	return CDlgBase::Tick();
}

int CDlgShop::GetConsumeContrib()
{
	int count(0);
	const CECHostPlayer::CONTRIB_INFO *pCInfo = GetHostPlayer()->GetContribInfo();
	if (pCInfo)
		count = pCInfo->consume_contrib;
	return count;
}

int CDlgShop::GetCumulateContrib()
{
	int count(0);
	const CECHostPlayer::CONTRIB_INFO *pCInfo = GetHostPlayer()->GetContribInfo();
	if (pCInfo)
		count = pCInfo->cumulate_contrib;
	return count;
}

int	CDlgShop::GetForceContrib()
{
	if(!m_pNPCSellService || m_nCurShopSet > NUM_NPCIVTR)
		return 0;

	int forceID = m_pNPCSellService->pages[m_nCurShopSet - 1].require_force;
	const CECHostPlayer::FORCE_INFO* pInfo = !forceID ? NULL : GetHostPlayer()->GetForceInfo(forceID);

	return pInfo ? pInfo->contribution : 0;
}

void CDlgShop::UpdateNPCSellService()
{
	//	更新当前NPC服务的售卖服务指针，无售卖服务或NPC无服务时为空
	//
	m_pNPCSellService = NULL;

	while (GetGameUIMan()->m_pCurNPCEssence)
	{
		unsigned int idSellService = GetGameUIMan()->m_pCurNPCEssence->id_sell_service;
		if (!idSellService)
			break;

		DATA_TYPE DataType;
		const void* pBuf = GetGame()->GetElementDataMan()->get_data_ptr(idSellService, ID_SPACE_ESSENCE, DataType);
		if (DataType != DT_NPC_SELL_SERVICE)
			break;

		m_pNPCSellService = (NPC_SELL_SERVICE *)pBuf;
		break;
	}
}

bool CDlgShop::UpdatePageButton(int pageIndex, PAUISTILLIMAGEBUTTON pButton)
{
	ACString strHint;
	bool invalidPage = false;

	if (m_pNPCSellService)
	{
		int nRequireContrib = m_pNPCSellService->pages[pageIndex].require_contrib;
		if (nRequireContrib > GetCumulateContrib())
		{
			//	玩家的累积贡献度不够，不能购买当前商品页的商品
			return false;
		}

		// restriction about player's force
		int nRequireForce = m_pNPCSellService->pages[pageIndex].require_force;
		const FORCE_CONFIG* pConfig = CECForceMgr::GetForceData(nRequireForce);
		if(pConfig)
		{
			ACString strMsg;
			bool invalid = (pConfig->id != GetHostPlayer()->GetForce());
			int idTxt =  invalid ? 9427:9426;
			if(!strHint.IsEmpty()) strHint += _AL("\r");
			strHint += strMsg.Format(GetStringFromTable(idTxt), pConfig->name);

			invalidPage |= invalid;
		}

		// restriction about player's force reputation
		int nRequireForceRep = m_pNPCSellService->pages[pageIndex].require_force_reputation;
		const CECHostPlayer::FORCE_INFO* pInfo = GetHostPlayer()->GetForceInfo(nRequireForce);
		if(nRequireForceRep > 0)
		{
			ACString strMsg;
			bool invalid = (!pInfo || nRequireForceRep > pInfo->reputation);
			int idTxt =  invalid ? 9429:9428;
			if(!strHint.IsEmpty()) strHint += _AL("\r");
			strHint += strMsg.Format(GetStringFromTable(idTxt), nRequireForceRep);

			invalidPage |= invalid;
		}
	}

	pButton->SetHint(strHint);

	// store the invalid flag
	pButton->SetData64(invalidPage ? 1 : 0);

	return true;
}

bool CDlgShop::IsPageInvalid(int page)
{
	// get the flag from button
	PAUIOBJECT pButton = m_pBtnSet[page - 1];
	return pButton->GetData64() != 0;
}

int CDlgShop::GetTotalContrib(int itemID, int amount, int* pCnotrib, int* pForceContrib)
{
	int nTotalContrib = 0;
	int nTotalForceContrib = 0;
	int nPageFound = -1;

	//	对每个物品，根据ID查找在当前可见的哪个页面，并从NPCSellService中获取其贡献度消耗
	if (m_pNPCSellService)
	{
		PAUISTILLIMAGEBUTTON pButton = NULL;
		int nPageMaxGoods = sizeof(m_pNPCSellService->pages[0].goods)/sizeof(m_pNPCSellService->pages[0].goods[0]);
		int j(0), k(0);
		for(j = 0; j < NUM_NPCIVTR; j++ )
		{
			pButton = m_pBtnSet[j];
			if (!pButton || !pButton->IsShow())
				continue;

			int nPage = pButton->GetData();
			for (k = 0; k < nPageMaxGoods; ++ k)
			{
				int id = (int)m_pNPCSellService->pages[nPage].goods[k].id;
				if (id == itemID)
				{
					//	已找到对应物品，增加贡献度消耗
					nTotalContrib += amount * m_pNPCSellService->pages[nPage].goods[k].contrib_cost;
					nTotalForceContrib += amount * m_pNPCSellService->pages[nPage].goods[k].force_contribution_cost;
					nPageFound = nPage;
					break;
				}
			}
			if (k < nPageMaxGoods)
			{
				//	上面已找到
				break;
			}
		}
		if (j >= NUM_NPCIVTR)
		{
			//	没有在当前包裹中找到对应物品，不可能发生的情况
			ASSERT(false);
		}
	}

	if(pCnotrib)
		*pCnotrib = nTotalContrib;

	if(pForceContrib)
		*pForceContrib = nTotalForceContrib;

	return nPageFound;
}

void CDlgShop::UpdateHint(PAUIOBJECT pObj, CECIvtrItem* pItem, const ACHAR* pszHint)
{
	int con(0), forceCon(0);
	int page(-1);
	
	if (strstr(pObj->GetName(), "Sell_") != pObj->GetName())
		page = GetTotalContrib(pItem->GetTemplateID(), 1, &con, &forceCon);

	// append extra information
	ACString strExtra;
	if(con)
	{
		ACString strMsg;
		strMsg.Format(GetStringFromTable(9166), con);
		strExtra += _AL("\r") + strMsg;
	}

	if(forceCon && m_pNPCSellService && page>=0)
	{
		const FORCE_CONFIG* pConfig = 
			CECForceMgr::GetForceData(m_pNPCSellService->pages[page].require_force);

		ACString strMsg;
		strMsg.Format(GetStringFromTable(9431), pConfig ? pConfig->name : _AL(""), forceCon);
		strExtra += _AL("\r") + strMsg;
	}
	
	AUICTranslate trans;
	ACString strHint = trans.Translate(pszHint);
	
	if(!strExtra.IsEmpty())
	{
		strHint += strExtra;
	}
	
	pObj->SetHint(strHint);
}

int CDlgShop::UpdatePages()
{
	int i;
	int nTotalSets = 0;

	//	需先更新NPC Sell Service信息才能进行贡献度相关计算
	UpdateNPCSellService();

	for(i = 0; i < NUM_NPCIVTR; i++ )
	{
		CECNPCInventory *pDeal = GetHostPlayer()->GetNPCSevPack(i);
		if( a_strlen(pDeal->GetName()) <= 0 ) continue;

		PAUISTILLIMAGEBUTTON pButton = m_pBtnSet[nTotalSets];

		if(!UpdatePageButton(i, pButton))
		{
			continue;
		}

		pButton->SetText(pDeal->GetName());
		pButton->SetData((DWORD)i);
		pButton->SetPushed(false);
		pButton->Show(true);

		nTotalSets++;
	}

	for( i = nTotalSets; i < NUM_NPCIVTR; i++ )
		m_pBtnSet[i]->Show(false);

	return nTotalSets;
}

bool CDlgShop::UpdateShop(int nSet)
{
	int i;
	AString strFile;
	CECIvtrItem *pItem;
	PAUIIMAGEPICTURE pObj;

	if( nSet > 0 )		// NPC
	{
		if( nSet == 1 )		// Only build buttons when nSet is 1.
		{
			UpdatePages();
		}

		PAUISTILLIMAGEBUTTON pButton = m_pBtnSet[m_nCurShopSet - 1];
		pButton->SetPushed(false);

		m_nCurShopSet = nSet;
		
		pButton = m_pBtnSet[m_nCurShopSet - 1];
		pButton->SetPushed(true);

		// get the flag from button
		bool invalidPage = IsPageInvalid(m_nCurShopSet);

		CECNPCInventory *pDeal = GetHostPlayer()->GetNPCSevPack(pButton->GetData());
		for( i = 0; i < IVTRSIZE_NPCPACK; i++ )
		{
			pObj = m_pImgU[i];
			if( !pObj ) continue;

			pItem = pDeal->GetItem(i);
			if( pItem )
			{
				pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");
				af_GetFileTitle(pItem->GetIconFile(), strFile);
				strFile.MakeLower();
				pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

				bool itemInvalid = pItem->IsEquipment() && !GetHostPlayer()->CanUseEquipment((CECIvtrEquip *)pItem);

				if( invalidPage || itemInvalid )
					pObj->SetColor(A3DCOLORRGB(192, 0, 0));
				else if (pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
					pObj->SetColor(A3DCOLORRGB(128, 128, 128));
				else
					pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			}
			else
			{
				pObj->SetCover(NULL, -1);
				pObj->SetDataPtr(NULL);
				pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			}
			pObj->SetHint(_AL(""));
		}
	}

	return true;
}

bool CDlgShop::UpdatePlayer()
{
	int i;
	AString strFile;
	CECIvtrItem *pItem;
	PAUIIMAGEPICTURE pObj;

	// Player
	{
		__int64 nTotalMoney;
		CECDealInventory *pDeal = GetHostPlayer()->GetDealPack();

		int idServiceNPC = GetHostPlayer()->GetCurServiceNPC();
		CECNPC *pNPC = GetWorld()->GetNPCMan()->GetNPC(idServiceNPC);
		CECNPCServer *pServer = (CECNPCServer *)pNPC;

		// Buy.
		nTotalMoney = 0;
		int nTotalContrib(0);
		int nTotalForceContrib(0);

		pDeal = GetHostPlayer()->GetBuyPack();
		for( i = 0; i < IVTRSIZE_BUYPACK; i++ )
		{
			pObj = m_pImgBuy[i];
			if( !pObj ) continue;

			pItem = pDeal->GetItem(i);
			if( pItem )
			{
				pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");

				pObj->SetColor(
					(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
					? A3DCOLORRGB(128, 128, 128)
					: A3DCOLORRGB(255, 255, 255));

				const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(i);
				if( ii.iAmount > 1 )
				{
					ACString strAmount;
					strAmount.Format(_AL("%d"), ii.iAmount);
					pObj->SetText(strAmount);
				}
				else
					pObj->SetText(_AL(""));
				af_GetFileTitle(pItem->GetIconFile(), strFile);
				strFile.MakeLower();
				pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

				float fPriceScale = 1.0f;
				if (pServer)
					fPriceScale = (1.0f + pServer->GetTaxRate()) * pServer->GetPriceScale();

				nTotalMoney += CECIvtrItem::GetScaledPrice(pItem->GetShopPrice(), ii.iAmount, 
					CECIvtrItem::SCALE_BUY, fPriceScale);

				int con, forceCon;
				GetTotalContrib(pItem->GetTemplateID(), ii.iAmount, &con, &forceCon);

				nTotalContrib += con;
				nTotalForceContrib += forceCon;
			}
			else
			{
				pObj->SetCover(NULL, -1);
				pObj->SetText(_AL(""));
				pObj->SetDataPtr(NULL);
			}
		}

		m_pTxt_Worth1->SetText(GetGameUIMan()->GetFormatNumber(nTotalMoney));
		m_pTxt_Worth1->SetData((int)nTotalMoney);

		m_pTxt_Gold->SetText(GetGameUIMan()->GetFormatNumber(GetHostPlayer()->GetMoneyAmount()));
		m_pTxt_Gold->SetData(GetHostPlayer()->GetMoneyAmount());

		if( nTotalMoney > GetHostPlayer()->GetMoneyAmount() )
			m_pTxt_Gold->SetColor(A3DCOLORRGB(255, 0, 0));
		else
			m_pTxt_Gold->SetColor(GetGameUIMan()->GetPriceColor(GetHostPlayer()->GetMoneyAmount()));

		// update faction contrib value
		m_pTxt_Contrib->SetText(GetGameUIMan()->GetFormatNumber(GetConsumeContrib()));
		m_pTxt_Contrib->SetData(GetConsumeContrib());

		m_pTxt_Contrib01->SetText(GetGameUIMan()->GetFormatNumber(nTotalContrib));
		m_pTxt_Contrib01->SetData(nTotalContrib);

		if (nTotalContrib > GetConsumeContrib())
			m_pTxt_Contrib01->SetColor(A3DCOLORRGB(255, 0, 0));
		else
			m_pTxt_Contrib01->SetColor(A3DCOLORRGB(255, 255, 255));

		// update the force contrib value
		m_pTxt_PlayerForceContrib->SetText(GetGameUIMan()->GetFormatNumber(GetForceContrib()));
		m_pTxt_PlayerForceContrib->SetData(GetForceContrib());

		m_pTxt_TotalForceContrib->SetText(GetGameUIMan()->GetFormatNumber(nTotalForceContrib));
		m_pTxt_TotalForceContrib->SetData(nTotalForceContrib);

		if(nTotalForceContrib > GetForceContrib())
			m_pTxt_PlayerForceContrib->SetColor(A3DCOLORRGB(255, 0, 0));
		else
			m_pTxt_PlayerForceContrib->SetColor(A3DCOLORRGB(255, 255, 255));
		
		// Sell.
		nTotalMoney = 0;
		pDeal = GetHostPlayer()->GetSellPack();
		for( i = 0; i < IVTRSIZE_SELLPACK; i++ )
		{
			pObj = m_pImgSell[i];
			if( !pObj ) continue;

			pItem = pDeal->GetItem(i);
			if( pItem )
			{
				pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");

				pObj->SetColor(
					(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
					? A3DCOLORRGB(128, 128, 128)
					: A3DCOLORRGB(255, 255, 255));

				const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(i);
				if( ii.iAmount > 1 )
				{
					ACString strAmount;
					strAmount.Format(_AL("%d"), ii.iAmount);
					pObj->SetText(strAmount);
				}
				else
					pObj->SetText(_AL(""));
				af_GetFileTitle(pItem->GetIconFile(), strFile);
				strFile.MakeLower();
				pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

				//	We must conside endurance for equipments like weapon, armor,
				//	decoration
				if (pItem->IsEquipment() && pItem->GetClassID() != CECIvtrItem::ICID_ARROW)
					nTotalMoney += pItem->GetScaledPrice();
				else if (pItem->GetClassID() == CECIvtrItem::ICID_MONEYCONVERTIBLE)
					nTotalMoney += (__int64)pItem->GetUnitPrice()*GetHostPlayer()->GetCashMoneyRate()*ii.iAmount;
				else
				{
					nTotalMoney += CECIvtrItem::GetScaledPrice(pItem->GetUnitPrice(), 
						ii.iAmount, CECIvtrItem::SCALE_SELL, PLAYER_PRICE_SCALE);
				}
			}
			else
			{
				pObj->SetCover(NULL, -1);
				pObj->SetText(_AL(""));
				pObj->SetDataPtr(NULL);
			}
		}

		m_pTxt_Worth2->SetText(GetGameUIMan()->GetFormatNumber(nTotalMoney));
		m_pTxt_Worth2->SetData64(nTotalMoney);
	}

	return true;
}

int CDlgShop::GetCurShopSet()
{
	return m_nCurShopSet;
}

void CDlgShop::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") ) return;

	// get the invalid flag from button
	if( strstr(pObj->GetName(), "U_") && IsPageInvalid(m_nCurShopSet))
		return;

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	POINT ptCapture = { GET_X_LPARAM(lParam) - p->X, GET_Y_LPARAM(lParam) - p->Y };

	GetGameUIMan()->m_ptLButtonDown = ptCapture;

	GetGameUIMan()->InvokeDragDrop(this, pObj, ptCapture);
	GetGameUIMan()->PlayItemSound((CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem"), false);
}

void CDlgShop::OnEventLButtonDblclk(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") ) return;

	// get the invalid flag from button
	if( strstr(pObj->GetName(), "U_") && IsPageInvalid(m_nCurShopSet))
		return;

	ACHAR szText[40];
	CECHostPlayer *pHost = GetHostPlayer();
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");

	bool bInputMsgBox(false);

	if (pItem->GetPileLimit() > 1)
	{
		if (!strstr(pObj->GetName(), "Sell_"))
		{
			bInputMsgBox = true;
		}
		else
		{
			CECDealInventory *pDeal = pHost->GetSellPack();
			int iSrc = atoi(pObj->GetName() + strlen("Sell_")) - 1;
			const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(iSrc);
			if (ii.iAmount > 1)
				bInputMsgBox = true;
		}
	}
	if (bInputMsgBox)
	{
		int nInputNOMode = 0;
		CDlgInputNO *pShow = GetGameUIMan()->m_pDlgInputNO;
		PAUIOBJECT pEdit = pShow->GetDlgItem("DEFAULT_Txt_No.");

		pShow->Show(true, true);
		pShow->SetDataPtr(pObj,"ptr_AUIObject");
		pShow->SetData(GetData());

		if( strstr(pObj->GetName(), "U_") )
		{
			pEdit->SetData(pItem->GetPileLimit());
			a_sprintf(szText, _AL("%d"), 1);
			nInputNOMode = CDlgInputNO::INPUTNO_BUY_ADD;
		}
		else if( strstr(pObj->GetName(), "Buy_") )
		{
			CECDealInventory *pDeal = pHost->GetBuyPack();
			int iSrc = atoi(pObj->GetName() + strlen("Buy_")) - 1;
			const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(iSrc);

			pEdit->SetData(ii.iAmount);
			a_sprintf(szText, _AL("%d"), ii.iAmount);
			nInputNOMode = CDlgInputNO::INPUTNO_BUY_REMOVE;
		}
		else if( strstr(pObj->GetName(), "Sell_") )
		{
			CECDealInventory *pDeal = pHost->GetSellPack();
			int iSrc = atoi(pObj->GetName() + strlen("Sell_")) - 1;
			const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(iSrc);

			pEdit->SetData(ii.iAmount);
			a_sprintf(szText, _AL("%d"), ii.iAmount);
			nInputNOMode = CDlgInputNO::INPUTNO_SELL_REMOVE;
		}

		pEdit->SetText(szText);
		pShow->SetType(nInputNOMode);
	}
	else
	{
		if( strstr(pObj->GetName(), "U_") )
		{
			int nPage, iSrc;
			char szItem[40];
			PAUIOBJECT pButton;
			CECDealInventory *pDeal = pHost->GetBuyPack();

			sprintf(szItem, "Btn_Set%d", m_nCurShopSet);
			pButton = GetDlgItem(szItem);
			nPage = pButton->GetData();

			iSrc = atoi(pObj->GetName() + strlen("U_")) + nPage * IVTRSIZE_NPCPACK - 1;
			pDeal->AddItemFree(pItem, iSrc, 1);
		}
		else if( strstr(pObj->GetName(), "Buy_") )
		{
			CECDealInventory *pDeal = pHost->GetBuyPack();
			int iSrc = atoi(pObj->GetName() + strlen("Buy_")) - 1;
			
			pDeal->RemoveItemByIndex(iSrc, 1);
		}
		else if( strstr(pObj->GetName(), "Sell_") )
		{
			CECDealInventory *pDeal = pHost->GetSellPack();
			int iSrc = atoi(pObj->GetName() + strlen("Sell_")) - 1;
			
			pDeal->RemoveItemByIndex(iSrc, pItem->GetCount());
			pItem->Freeze(false);
		}
	}
}

void CDlgShop::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
	{
		// refresh the button text
		if(IsShow())
		{
			UpdateShop(1);
		}
	}
}

void CDlgShop::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	CECHostPlayer* pHost = GetHostPlayer();
	CECDealInventory *pSell = pHost->GetSellPack();

	// check whether this item can be added to sell package
	int sellCount = 0;
	int iSlot = pSell->GetItemIndex(iSrc);	
	if (iSlot >= 0)
	{
		// already in the sell package
		const CECDealInventory::ITEMINFO &ii = pSell->GetItemInfo(iSlot);
		sellCount = pIvtrSrc->GetCount()-ii.iAmount;
	}
	else if( pSell->SearchEmpty()>=0 )
	{
		// still have empty place
		sellCount = pIvtrSrc->GetCount();
	}
	
	if( (!pObjOver || AUI_PRESS(VK_MENU)) && // right-click or press VK_MENU when drag-drop
		pIvtrSrc->GetPileLimit() > 1 && sellCount > 1 )
	{
		GetGameUIMan()->InvokeNumberDialog(pObjSrc, NULL, CDlgInputNO::INPUTNO_SELL_ADD, sellCount);
		GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	}
	else if(sellCount > 0)
	{
		if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_MONEYCONVERTIBLE )
		{
			ACString strDesc, strPrice;
			__int64 iiMoney = (__int64)pIvtrSrc->GetUnitPrice()*GetHostPlayer()->GetCashMoneyRate()*sellCount;
			strPrice = A3DCOLOR_TO_STRING(GetGameUIMan()->GetPriceColor(iiMoney)) + g_pGame->GetFormattedPrice(iiMoney);
			strDesc.Format(GetGameUIMan()->GetStringFromTable(10015), strPrice);
			GetGameUIMan()->MessageBox("", strDesc, MB_OK, A3DCOLORRGB(255, 255, 255));
		}

		pSell->AddItem(pIvtrSrc, iSrc, sellCount);
		pIvtrSrc->Freeze(true);
		GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	}
}
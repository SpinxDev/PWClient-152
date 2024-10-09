// File		: DlgTrade.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/16

#include "AFI.h"
#include "AUIEditBox.h"
#include "DlgDragDrop.h"
#include "DlgTrade.h"
#include "DlgInputNO.h"
#include "DlgShop.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_DealInventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_GameRun.h"
#include "EC_ElsePlayer.h"
#include "EC_World.h"
#include "EC_ManPlayer.h"
#include "EC_FixedMsg.h"
#include "Network\\gnetdef.h"
#include "AUIDef.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgTrade, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("lock",			OnCommandLock)
AUI_ON_COMMAND("complete",		OnCommandComplete)
AUI_ON_COMMAND("choosemoney",	OnCommandChoosemoney)
AUI_ON_COMMAND("disclaimer",	OnCommandDisclaimer)
AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgTrade, CDlgBase)

AUI_ON_EVENT("I_*", WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("I_*", WM_LBUTTONDBLCLK,	OnEventLButtonDBLCLK)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgTrade
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgTrade> TradeClickShortcut;
//------------------------------------------------------------------------

CDlgTrade::CDlgTrade()
{
	m_pBtnLock = NULL;
	m_pBtnLock2 = NULL;
	m_pBtnConfirm = NULL;
	m_pBtnConfirm2 = NULL;
	m_pTxtGold = NULL;
	m_pTxtGold2 = NULL;
	m_pTxtName = NULL;
	for( int i = 0; i < IVTRSIZE_DEALPACK; i++ )
	{
		m_pYourItem[i] = NULL;
		m_pMyItem[i] = NULL;
	}
	m_idTradeAction = TRADE_ACTION_NONE;
	m_bNameComplete = false;
}

CDlgTrade::~CDlgTrade()
{
}

bool CDlgTrade::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new TradeClickShortcut(this));
	return CDlgBase::OnInitDialog();
}

void CDlgTrade::OnShowDialog()
{
	m_bNameComplete = false;
	m_pTxtName->SetText(_AL(""));
}

void CDlgTrade::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Btn_Lock", m_pBtnLock);
	DDX_Control("Btn_Lock2", m_pBtnLock2);
	DDX_Control("Btn_Confirm", m_pBtnConfirm);
	DDX_Control("Btn_Confirm2", m_pBtnConfirm2);
	DDX_Control("Txt_Gold", m_pTxtGold);
	DDX_Control("Txt_Gold2", m_pTxtGold2);
	DDX_Control("Txt_Name", m_pTxtName);
	char szItem[40];
	for( int i = 0; i < IVTRSIZE_DEALPACK; i++ )
	{
		sprintf(szItem, "U_%02d", i + 1);
		DDX_Control(szItem, m_pYourItem[i]);
		sprintf(szItem, "I_%02d", i + 1);
		DDX_Control(szItem, m_pMyItem[i]);
	}
}

void CDlgTrade::OnCommandCANCEL(const char * szCommand)
{
	GetGameSession()->trade_Cancel((int)GetDataPtr("int"));
}

void CDlgTrade::OnCommandLock(const char * szCommand)
{
	GetGameSession()->trade_Submit((int)GetDataPtr("int"));
}

void CDlgTrade::OnCommandComplete(const char * szCommand)
{
	CECHostPlayer *pHost = GetHostPlayer();
	pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	GetGameSession()->trade_Confirm((int)GetDataPtr("int"));
}

void CDlgTrade::OnCommandChoosemoney(const char * szCommand)
{
	if( !m_pBtnLock->IsShow() )
		return;

	if( m_pBtnLock->IsShow() && !m_pBtnLock->IsEnabled() )
		return;

	CECHostPlayer *pHost = GetHostPlayer();
	CDlgInputNO* pShow = GetGameUIMan()->m_pDlgInputNO;
	pShow->SetData((int)GetDataPtr("int"));
	pShow->Show(true, true);
	pShow->SetType(CDlgInputNO::INPUTNO_TRADE_MONEY);

	PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(pShow->GetDlgItem("DEFAULT_Txt_No."));
	pEdit->SetIsNumberOnly(true);
	pEdit->SetData(pHost->GetMoneyAmount());

	ACHAR szText[40];
	CECDealInventory *pDeal = pHost->GetDealPack();
	a_sprintf(szText, _AL("%d"), pDeal->GetMoney());
	pEdit->SetText(szText);
}

void CDlgTrade::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") || !m_pBtnLock->IsShow() || !m_pBtnLock->IsEnabled() )
		return;

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	POINT ptCapture = { GET_X_LPARAM(lParam) - p->X, GET_Y_LPARAM(lParam) - p->Y };

	GetGameUIMan()->InvokeDragDrop(this, pObj, ptCapture);
	GetGameUIMan()->PlayItemSound((CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem"), false);
}

void CDlgTrade::OnEventLButtonDBLCLK(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") || !m_pBtnLock->IsShow() )
		return;
	if( m_pBtnLock->IsShow() && !m_pBtnLock->IsEnabled() )
		return;

	int idTrade = (int)GetDataPtr("int");
	CECIvtrItem *pIvtrSrc = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	int idItem = pIvtrSrc->GetTemplateID();
	int iSrc = atoi(pObj->GetName() + strlen("I_")) - 1;
	CECHostPlayer *pHost = GetHostPlayer();
	CECDealInventory *pDeal = pHost->GetDealPack();
	const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(iSrc);

	GetGameSession()->trade_RemoveGoods(idTrade, idItem, ii.iOrigin, ii.iAmount, 0);
	pIvtrSrc->Freeze(false);
	
}

bool CDlgTrade::Tick(void)
{
	if( IsShow() ) UpdateTradeInfo();

	return CDlgBase::Tick();
}

bool CDlgTrade::UpdateTradeInfo()
{
	int i;
	AString strFile;
	ACHAR szText[40];
	CECIvtrItem *pItem;
	PAUIIMAGEPICTURE pObj;
	CECHostPlayer *pHost = GetHostPlayer();	
	
	if (!m_bNameComplete)
	{
		int idPlayer = (int)GetData();

		//	名称
		ACString strTitle;
		const ACHAR *pszName = GetGameRun()->GetPlayerName(idPlayer, true);		
		AUI_ConvertChatString(pszName, szText);

		ACString strProfLevel2;
		CECElsePlayer *pElsePlayer = GetWorld()->GetPlayerMan()->GetElsePlayer(idPlayer);
		if (pElsePlayer && pElsePlayer->IsBaseInfoReady())
		{
			strProfLevel2.Format(GetStringFromTable(10066)
				, GetGameRun()->GetProfName(pElsePlayer->GetProfession())
				, GetGameRun()->GetLevel2Name(pElsePlayer->GetBasicProps().iLevel2));

			m_bNameComplete = true;
		}

		strTitle.Format(GetStringFromTable(583), szText, strProfLevel2);
		m_pTxtName->SetText(strTitle);
	}

	CECDealInventory *pEPDeal = pHost->GetEPDealPack();
	for( i = 0; i < IVTRSIZE_DEALPACK; i++ )
	{
		pObj = m_pYourItem[i];
		if( !pObj ) continue;

		pItem = pEPDeal->GetItem(i);
		if( pItem )
		{
			pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");

			pObj->SetColor(
				(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
				? A3DCOLORRGB(128, 128, 128)
				: A3DCOLORRGB(255, 255, 255));

			if( pItem->GetCount() > 1 )
			{
				a_sprintf(szText, _AL("%d"), pItem->GetCount());
				pObj->SetText(szText);
			}
			else
				pObj->SetText(_AL(""));
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		}
		else
		{
			pObj->SetCover(NULL, -1);
			pObj->SetText(_AL(""));
			pObj->SetDataPtr(NULL);
		}
	}
	m_pTxtGold2->SetColor(GetGameUIMan()->GetPriceColor(pEPDeal->GetMoney()));
	m_pTxtGold2->SetText(GetGameUIMan()->GetFormatNumber(pEPDeal->GetMoney()));

	CECDealInventory *pDeal = pHost->GetDealPack();
	for( i = 0; i < IVTRSIZE_DEALPACK; i++ )
	{
		pObj = m_pMyItem[i];
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
				a_sprintf(szText, _AL("%d"), ii.iAmount);
				pObj->SetText(szText);
			}
			else
				pObj->SetText(_AL(""));
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		}
		else
		{
			pObj->SetCover(NULL, -1);
			pObj->SetText(_AL(""));
			pObj->SetDataPtr(NULL);
		}
	}
	m_pTxtGold->SetColor(GetGameUIMan()->GetPriceColor(pDeal->GetMoney()));
	m_pTxtGold->SetText(GetGameUIMan()->GetFormatNumber(pDeal->GetMoney()));

	return true;
}

void CDlgTrade::TradeAction(int idPlayer, int idTrade, int idAction, int nCode)
{
	CECHostPlayer *pHost = GetHostPlayer();
	PAUIDIALOG pInventory = GetGameUIMan()->GetDialog("Win_Inventory");

	if( idAction == TRADE_ACTION_ALTER )
	{
		if( pHost->GetCharacterID() != idPlayer &&
			!m_pBtnLock->IsEnabled() )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(230), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			GetGameUIMan()->AddChatMessage(GetStringFromTable(230), GP_CHAT_MISC);
		}
		
		m_pBtnLock->Show(true);
		m_pBtnLock->Enable(true);
		m_pBtnConfirm->Show(false);

		m_pBtnLock2->Show(true);
		m_pBtnLock2->Enable(true);
		m_pBtnConfirm2->Show(false);
	}
	else if( idAction == TRADE_ACTION_LOCK )
	{
		if( nCode == GNET::ERR_TRADE_READY )
		{
			m_pBtnLock2->Show(false);
			m_pBtnConfirm2->Show(true);
			m_pBtnConfirm2->Enable(true);

			m_pBtnLock->Show(false);
			m_pBtnConfirm->Show(true);
			m_pBtnConfirm->Enable(true);
		}
		else if( nCode == GNET::ERR_TRADE_READY_HALF )
		{
			if( pHost->GetCharacterID() == idPlayer )
			{
				m_pBtnLock->Enable(false);
				GetGameUIMan()->AddChatMessage(GetStringFromTable(219), GP_CHAT_MISC);
			}
			else
				m_pBtnLock2->Enable(false);
		}
	}
	else if( idAction == TRADE_ACTION_UNLOCK )
	{
		m_pBtnLock->Show(true);
		m_pBtnLock->Enable(true);
		m_pBtnConfirm->Show(false);

		m_pBtnLock2->Show(true);
		m_pBtnLock2->Enable(true);
		m_pBtnConfirm2->Show(false);
		
		GetGameUIMan()->AddChatMessage(GetStringFromTable(275), GP_CHAT_MISC);
	}
	else if( idAction == TRADE_ACTION_CANCEL )
	{
		if( pHost->GetCharacterID() != idPlayer )
			GetGameUIMan()->AddChatMessage(GetStringFromTable(215), GP_CHAT_MISC);
		m_idTradeAction = idAction;
		if( GetGameUIMan()->m_pDlgDragDrop->IsShow() )
			GetGameUIMan()->m_pDlgDragDrop->OnCommand("IDCANCEL");
	}
	else if( idAction == TRADE_ACTION_DEAL )
	{
		if( pHost->GetCharacterID() == idPlayer )
		{
			if( nCode == GNET::ERR_TRADE_HALFDONE )
			{
				m_pBtnConfirm->Enable(false);
				GetGameUIMan()->AddChatMessage(GetStringFromTable(216), GP_CHAT_MISC);
			}
			else if( nCode == GNET::ERR_TRADE_DONE )
				m_pBtnConfirm->Show(false);
		}
		else
		{
			if( nCode == GNET::ERR_TRADE_HALFDONE )
				m_pBtnConfirm2->Enable(false);
			else if( nCode == GNET::ERR_TRADE_DONE )
				m_pBtnConfirm2->Show(false);
		}
		m_idTradeAction = idAction;
	}
	else if( idAction == TRADE_ACTION_END )
	{
		Show(false);
		pInventory->Show(false);
		if( nCode == _TRADE_END_NORMAL )
		{
			if( m_idTradeAction == TRADE_ACTION_DEAL )
				GetGameUIMan()->AddChatMessage(GetStringFromTable(217), GP_CHAT_MISC);
		}
		else
			GetGameUIMan()->AddChatMessage(GetStringFromTable(218), GP_CHAT_MISC);

		m_idTradeAction = TRADE_ACTION_NONE;
	}
}

void CDlgTrade::OnCommandDisclaimer(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Disclaimer");
	if(pDlg)
	{
		pDlg->Show(!pDlg->IsShow());
	}
}

void CDlgTrade::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if( m_pBtnLock->IsShow() && m_pBtnLock->IsEnabled() )
	{
		//	检查交易包裹是否已满
		if (GetHostPlayer()->GetDealPack()->GetEmptySlotNum() > 0)
		{
			if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetPileLimit() > 1 && pIvtrSrc->GetCount() > 1 )
			{
				GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
					CDlgInputNO::INPUTNO_TRADE_ADD, pIvtrSrc->GetCount());
			}
			else
			{
				GetGameSession()->trade_AddGoods(GetGameUIMan()->m_idTrade, pIvtrSrc->GetTemplateID(), iSrc, pIvtrSrc->GetCount(), 0);
				pIvtrSrc->Freeze(true);
			}
			GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
		}
	}
}
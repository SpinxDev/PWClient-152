// File		: DlgEquipRefine.cpp
// Creator	: Xiao Zhou
// Date		: 2005/3/29

#include "AFI.h"
#include "AUICTranslate.h"
#include "DlgEquipRefine.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrStone.h"
#include "EC_Shop.h"
#include "EC_ShopSearch.h"
#include "EC_UIConfigs.h"
#include "EC_UIHelper.h"
#include "EC_TimeSafeChecker.h"

#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEquipRefine, CDlgQuickBuyBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("Btn_Shop",		OnCommandShop)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgEquipRefine, CDlgQuickBuyBase)

AUI_ON_EVENT("Img_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown_Item)
AUI_ON_EVENT("Img_Ticket",	WM_LBUTTONDOWN,	OnEventLButtonDown_Ticket)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgEquipRefine
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgEquipRefine> EquipRefineClickShortcut;
//------------------------------------------------------------------------

CDlgEquipRefine::CDlgEquipRefine()
{
	m_pImg_Item = NULL;
	m_pTxt_Name = NULL;
	m_pTxt_Effect = NULL;
	m_pTxt_Gold = NULL;
	m_pImg_Ticket = NULL;
	m_pTxt_Ticket = NULL;
	m_pTxt_RefineList = NULL;
	m_pBtn_Confirm = NULL;
	m_nSolt = -1;
	m_nSoltTicket = -1;
}

CDlgEquipRefine::~CDlgEquipRefine()
{
}

bool CDlgEquipRefine::OnInitDialog()
{
	CDlgQuickBuyBase::OnInitDialog();

	DDX_Control("Img_Item", m_pImg_Item);
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Txt_Effect", m_pTxt_Effect);
	DDX_Control("Txt_Gold", m_pTxt_Gold);
	DDX_Control("Img_Ticket", m_pImg_Ticket);
	DDX_Control("Txt_Ticket", m_pTxt_Ticket);
	DDX_Control("Txt_RefineList", m_pTxt_RefineList);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	m_dwEnalbeTick = 0;

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new EquipRefineClickShortcut(this));
	return true;
}

void CDlgEquipRefine::OnCommandConfirm(const char *szCommand)
{
	CECIvtrEquip* pEquip = (CECIvtrEquip*)m_pImg_Item->GetDataPtr();
	if( pEquip && m_nSolt >= 0 ) 
	{
		if (m_nSoltTicket >= 0)
		{
			CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(m_nSoltTicket);
			if (!pItem || pItem->GetClassID() != CECIvtrItem::ICID_REFINETICKET)
				return;
			
			CECIvtrRefineTicket *pIvtrTicket = dynamic_cast<CECIvtrRefineTicket*>(pItem);
			if (pIvtrTicket->GetDBEssence()->binding_only && !pEquip->IsBinding())
			{
				GetGameUIMan()->MessageBox("", GetStringFromTable(9760), MB_OK, A3DCOLORRGB(255, 255, 255));
				return;
			}
			
			if (pIvtrTicket->GetDBEssence()->require_level_max > 0 &&
				pIvtrTicket->GetDBEssence()->require_level_max < pEquip->GetItemLevel())
			{
				GetGameUIMan()->MessageBox("", GetStringFromTable(9761), MB_OK, A3DCOLORRGB(255, 255, 255));
				return;
			}
		}

		if( pEquip->GetRefineMaterialNum() > GetHostPlayer()->GetPack()->GetItemTotalNum(11208) )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(768), MB_OK, A3DCOLORRGB(255, 255, 255));
			return;
		}
		GetGameSession()->c2s_CmdNPCSevRefine(m_nSolt, pEquip->GetTemplateID(), m_nSoltTicket);
		m_nLastSolt = m_nSolt;
		m_nLastSoltTicket = m_nSoltTicket;
		OnEventLButtonDown_Item(0, 0, NULL);
		OnEventLButtonDown_Ticket(0, 0, NULL);
		m_pBtn_Confirm->Enable(false);
	}
}

void CDlgEquipRefine::OnCommandCancel(const char *szCommand)
{
	OnEventLButtonDown_Item(0, 0, NULL);
	OnEventLButtonDown_Ticket(0, 0, NULL);
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgEquipRefine::OnCommandShop(const char * szCommand)
{
	int mainType(-1), subType(-1);
	if (CECQShopConfig::Instance().FindCategory(CECQShopConfig::CID_REFINE, mainType, subType)){
		CECUIHelper::QShopShowCategory(mainType, subType);
	}
}

void CDlgEquipRefine::OnShowDialog()
{
	QuickBuyDisappear();

	OnEventLButtonDown_Item(0, 0, NULL);
	OnEventLButtonDown_Ticket(0, 0, NULL);
	m_pTxt_RefineList->SetText(_AL(""));
	m_nLastSolt = -1;
	m_nLastSoltTicket = -1;
}

void CDlgEquipRefine::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_Item->GetDataPtr();
	if( pItem )
		pItem->Freeze(false);
	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(_AL(""));
	m_pTxt_Name->SetText(_AL(""));
	m_pTxt_Effect->SetText(_AL(""));
	m_pTxt_Effect->SetHint(_AL(""));
	m_pTxt_Gold->SetText(_AL(""));
	m_pImg_Item->SetDataPtr(NULL);
	m_nSolt = -1;
}

void CDlgEquipRefine::OnEventLButtonDown_Ticket(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_Ticket->GetDataPtr();
	if( pItem )
		pItem->Freeze(false);
	m_pImg_Ticket->ClearCover();
	m_pImg_Ticket->SetHint(_AL(""));
	m_pImg_Ticket->SetDataPtr(NULL);
	m_pTxt_Ticket->SetText(_AL(""));
	m_nSoltTicket = -1;
}

void CDlgEquipRefine::SetEquip(CECIvtrItem *pItem, int nSlot)
{
	CECIvtrEquip* pEquip = (CECIvtrEquip*)pItem;
	if( pEquip->GetRefineMaterialNum() == 0 )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(774), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}

	OnEventLButtonDown_Item(0, 0, NULL);
	m_pImg_Item->SetDataPtr(pEquip);
	m_nSolt = nSlot;
	pItem->Freeze(true);
	AString strFile;
	af_GetFileTitle(pEquip->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	m_pImg_Item->SetColor(
		(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	m_pTxt_Name->SetText(pEquip->GetName());

	UpdateEquipDesc();
}

void CDlgEquipRefine::UpdateEquipDesc()
{
	if (m_pImg_Item->GetDataPtr()){
		CECIvtrEquip *pEquip = (CECIvtrEquip*)GetHostPlayer()->GetPack()->GetItem(m_nSolt);
		if (pEquip){
			AUICTranslate trans;
			const ACHAR * szDesc = pEquip->GetDesc();
			m_pImg_Item->SetHint(szDesc ? trans.Translate(szDesc) : _AL(""));
			
			ACString strShow, strHint;
			ACString clrAttribute(_AL("^ffcb4a")), clrEffect(_AL("^00e83a"));
			abase::vector<ACString> strEffects;
			if (pEquip->GetRefineEffect(strEffects, clrAttribute, clrEffect)){
				strShow = trans.Translate(strEffects.front());
				strHint = strShow;
				if (strEffects.size() > 1){
					strShow += clrAttribute + _AL(" ...");
					for (size_t i(1); i < strEffects.size(); ++ i)
					{
						strHint += _AL("\r");
						strHint += trans.Translate(strEffects[i]);
					}
				}
			}
			m_pTxt_Effect->SetText(strShow);
			m_pTxt_Effect->SetHint(strHint);
		}
	}
}

CECIvtrItem * CDlgEquipRefine::GetRefineTicket()
{
	CECIvtrItem *pTicket = NULL;
	if (m_nSoltTicket >= 0){
		CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(m_nSoltTicket);
		if (pItem != NULL && pItem->GetClassID() == CECIvtrItem::ICID_REFINETICKET){
			pTicket = pItem;
		}
	}
	return pTicket;
}

void CDlgEquipRefine::SetTicket(CECIvtrItem *pItem, int nSlot)
{
	CECIvtrRefineTicket* pTicket = (CECIvtrRefineTicket*)pItem;
	OnEventLButtonDown_Ticket(0, 0, NULL);
	m_pImg_Ticket->SetDataPtr(pTicket);
	m_nSoltTicket = nSlot;
	pItem->Freeze(true);
	AString strFile;
	af_GetFileTitle(pTicket->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Ticket->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pTicket->GetDesc();
	if( szDesc )
		m_pImg_Ticket->SetHint(trans.Translate(szDesc));
	else
		m_pImg_Ticket->SetHint(_AL(""));
	m_pTxt_Ticket->SetText(pItem->GetName());
}

void CDlgEquipRefine::OnTick()
{
	CDlgQuickBuyBase::OnTick();

	ACHAR szText[20];
	if( m_pImg_Item->GetDataPtr() ) 
	{
		CECIvtrEquip *pEquip = (CECIvtrEquip*)GetHostPlayer()->GetPack()->GetItem(m_nSolt);
		if (pEquip){
			UpdateEquipDesc();
			m_pBtn_Confirm->Enable(true);
			if(	GetHostPlayer()->GetPack()->GetItemTotalNum(11208) >= (int)pEquip->GetRefineMaterialNum() )
				m_pTxt_Gold->SetColor(A3DCOLORRGB(255, 255, 255));
			else
				m_pTxt_Gold->SetColor(A3DCOLORRGB(255, 0, 0));
			a_sprintf(szText, _AL("%d/%d"), pEquip->GetRefineMaterialNum(), GetHostPlayer()->GetPack()->GetItemTotalNum(11208));
			m_pTxt_Gold->SetText(szText);
		}
	}
	else
	{
		m_pBtn_Confirm->Enable(false);
		a_sprintf(szText, _AL("0/%d"), GetHostPlayer()->GetPack()->GetItemTotalNum(11208));
		m_pTxt_Gold->SetText(szText);
		m_pTxt_Gold->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	if( m_dwEnalbeTick != 0 )
		if( CECTimeSafeChecker::ElapsedTimeFor(m_dwEnalbeTick) >= 1000 )
			m_dwEnalbeTick = 0;
		else
			m_pBtn_Confirm->Enable(false);
}

void CDlgEquipRefine::RefineResult(int result)
{
	m_pTxt_RefineList->AppendText(GetStringFromTable(770 + result));
	m_pTxt_RefineList->ScrollToBottom();
	CECIvtrItem *pItem;
	if( m_nLastSolt != -1 )
	{
		pItem = GetHostPlayer()->GetPack()->GetItem(m_nLastSolt);
		if( pItem )
			SetEquip(pItem, m_nLastSolt);
	}
	if( m_nLastSoltTicket != -1 )
	{
		pItem = GetHostPlayer()->GetPack()->GetItem(m_nLastSoltTicket);
		if( pItem && pItem->GetCount() > 1 )
			SetTicket(pItem, m_nLastSoltTicket);
	}
	m_pBtn_Confirm->Enable(false);
	m_dwEnalbeTick = GetTickCount();

	//	根据情况弹出购买提示
	int recommendMessage(-1), recommendItem(-1);
	switch (result)
	{
	case 1:
		recommendMessage = 11214;
		recommendItem = CECUIConfig::Instance().GetGameUI().GetRecommendShopItem(CECUIConfig::GameUI::RECOMMEND_REFINE_1);
		break;
	case 3:
		recommendMessage = 11215;
		recommendItem = CECUIConfig::Instance().GetGameUI().GetRecommendShopItem(CECUIConfig::GameUI::RECOMMEND_REFINE_2);
		break;
	}
	CECIvtrItem *pTicket = GetRefineTicket();
	if (recommendItem > 0 && !pTicket){
		QuickBuyAppear(GetStringFromTable(recommendMessage), recommendItem);
	}else{
		QuickBuyDisappear();
	}
}

void CDlgEquipRefine::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_WEAPON ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_ARMOR ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_DECORATION )
	{
		// no need to check name "Img_Item", only one available drag-drop target
		SetEquip(pIvtrSrc, iSrc);
		
		//	根据精炼等级弹出购买提示
		int recommendMessage(-1), recommendItem(-1);
		int refineLevel = dynamic_cast<CECIvtrEquip *>(pIvtrSrc)->GetRefineLevel();
		if (refineLevel >= 1 && refineLevel <= 3){			
			recommendMessage = 11216;
			recommendItem = CECUIConfig::Instance().GetGameUI().GetRecommendShopItem(CECUIConfig::GameUI::RECOMMEND_REFINE_3);
		}else if (refineLevel >= 4 && refineLevel <= 6){
			recommendMessage = 11217;
			recommendItem = CECUIConfig::Instance().GetGameUI().GetRecommendShopItem(CECUIConfig::GameUI::RECOMMEND_REFINE_4);
		}else if (refineLevel >= 7 && refineLevel <= 11){			
			recommendMessage = 11218;
			recommendItem = CECUIConfig::Instance().GetGameUI().GetRecommendShopItem(CECUIConfig::GameUI::RECOMMEND_REFINE_5);
		}
		CECIvtrItem *pTicket = GetRefineTicket();
		if (recommendItem > 0 && !pTicket){
			QuickBuyAppear(GetStringFromTable(recommendMessage), recommendItem);
		}else{
			QuickBuyDisappear();
		}
	}
	else if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_REFINETICKET )
	{
		// no need to check name "Img_Ticket", only one available drag-drop target
		SetTicket(pIvtrSrc, iSrc);
		QuickBuyDisappear();
	}
}


CECShopBase * CDlgEquipRefine::GetShopData()
{
	return &CECQShop::Instance();
}

CECShopSearchPolicyBase * CDlgEquipRefine::GetShopSearchPolicy(int itemID)
{
	//	此处不添加背包空间、价格等检查，用于后续弹出提示
	CECShopSearchPolicyBase *p1 = new CECShopSearchIDPolicy(itemID);
	CECShopSearchPolicyBase *p2 = new CECShopSearchValidPolicy();
	CECShopSearchPolicyBase *p3 = new CECShopSearchLevelPolicy(GetHostPlayer()->GetMaxLevelSofar());
	CECShopSearchPolicyBase *p4 = new CECShopSearchTaskPolicy();
	return
		new CECShopSearchCompositePolicy(p1,
		new CECShopSearchCompositePolicy(p2,
		new CECShopSearchCompositePolicy(p3, p4)));
}

void CDlgEquipRefine::OnItemAppearInPack(int itemID, int iSlot)
{
	//	精炼道具出现在包裹中
	CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(iSlot);
	if (!pItem || pItem->GetTemplateID() != itemID || pItem->GetClassID() != CECIvtrItem::ICID_REFINETICKET){
		assert(false);
		return;
	}
	SetTicket(pItem, iSlot);
	QuickBuyDisappear();
}

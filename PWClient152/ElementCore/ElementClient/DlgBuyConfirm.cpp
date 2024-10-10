// Filename	: DlgBuyConfirm.cpp
// Creator	: Xu Wenbin
// Date		: 2012/8/29

#include "DlgBuyConfirm.h"
#include "EC_GameUIMan.h"
#include "DlgQShopItem.h"
#include "DlgQShop.h"
#include "globaldataman.h"
#include "EC_Shop.h"
#include "EC_UIHelper.h"
#include "EC_RandMallShopping.h"
#include "EC_IvtrItem.h"
#include "ExpTypes.h"

#include <AIniFile.h>

#include <algorithm>

//	class CECBuyCommand
void CECBuyCommand::NotifyObservers(CECBuyCommandObserver::Action a, bool bClearNotifyOnceObserver)
{
	Observers observers;
	for (Observers::iterator it = m_observers.begin(); it != m_observers.end(); ++ it)
	{
		Observer & o = *it;
		(o.m_p->*a)(this);
		if (!bClearNotifyOnceObserver || !o.m_once){
			observers.push_back(o);
		}
	}
	if (observers.size() != m_observers.size()){
		m_observers.swap(observers);
	}
}

void CECBuyCommand::RegisterObserver(CECBuyCommandObserver *pObserver, bool once)
{
	if (pObserver && std::find(m_observers.begin(), m_observers.end(), pObserver) == m_observers.end()){
		m_observers.push_back(Observer(pObserver, once));
	}else{
		assert(false);
	}
}

void CECBuyCommand::UnregisterObserver(CECBuyCommandObserver *pObserver)
{
	Observers::iterator it = std::find(m_observers.begin(), m_observers.end(), pObserver);
	if (it != m_observers.end()){
		m_observers.erase(it);
	}else{
		assert(false);
	}
}

//	class CECShopBuyCommand
CECShopBuyCommand::CECShopBuyCommand(CECShopBase *pShop, int itemIndex, int buyIndex)
: m_pShop(pShop)
, m_itemIndex(itemIndex)
, m_buyIndex(buyIndex)
{
}

void CECShopBuyCommand::OnShow()
{
}

void CECShopBuyCommand::OnConfirm()
{
	if (m_pShop){
		m_pShop->Buy(m_itemIndex, m_buyIndex);
	}
}

void CECShopBuyCommand::OnCancel()
{	
	m_pShop = NULL;
	m_itemIndex = -1;
	m_buyIndex = -1;
}

ACString CECShopBuyCommand::GetMessage()const
{
	ACString strText;
	if (m_pShop){		
		const GSHOP_ITEM *pItem = m_pShop->GetItem(m_itemIndex);
		if (pItem){
			CECGameUIMan *pGameUIMan = CECUIHelper::GetGameUIMan();
			CDlgQShop *pDlgQShop = dynamic_cast<CDlgQShop *>(pGameUIMan->GetDialog("Win_QShop"));		
			strText.Format(pGameUIMan->GetStringFromTable(822),
				pDlgQShop->GetCashText(pItem->buy[m_buyIndex].price), pItem->szName,
				CDlgQShopItem::GetFormatTime(pItem->buy[m_buyIndex].time));
		}
	}
	return strText;
}

int	CECShopBuyCommand::GetID()const
{
	int	id = 0;
	if (m_pShop){
		const GSHOP_ITEM *pItem = m_pShop->GetItem(m_itemIndex);
		if (pItem){
			id = pItem->id;
		}
	}
	return id;
}

int	CECShopBuyCommand::GetItemIndex()const
{
	return m_itemIndex;
}

int	CECShopBuyCommand::GetBuyIndex()const
{
	return m_buyIndex;
}

//	class CECShopBuyCommandObserver
void CECShopBuyCommandObserver::OnShow(const CECBuyCommand *p)
{
	if (const CECShopBuyCommand *pCmd = dynamic_cast<const CECShopBuyCommand *>(p)){
		OnBuyConfirmShow(pCmd->GetID(), pCmd->GetItemIndex(), pCmd->GetBuyIndex());
	}
}
void CECShopBuyCommandObserver::OnConfirm(const CECBuyCommand *p)
{
	if (const CECShopBuyCommand *pCmd = dynamic_cast<const CECShopBuyCommand *>(p)){
		OnBuyConfirmed(pCmd->GetID(), pCmd->GetItemIndex(), pCmd->GetBuyIndex());
	}
}
void CECShopBuyCommandObserver::OnCancel(const CECBuyCommand *p)
{
	if (const CECShopBuyCommand *pCmd = dynamic_cast<const CECShopBuyCommand *>(p)){
		OnBuyCancelled(pCmd->GetID(), pCmd->GetItemIndex(), pCmd->GetBuyIndex());
	}
}

//	class CECRandShopBuyCommand
CECRandShopBuyCommand::CECRandShopBuyCommand(RandMallShopping *p)
: m_pShopping(p)
{
}

void CECRandShopBuyCommand::OnShow()
{
}

void CECRandShopBuyCommand::OnConfirm()
{
	if (m_pShopping && m_pShopping->CanPay()){
		m_pShopping->Pay();
	}
}

void CECRandShopBuyCommand::OnCancel()
{
}

ACString CECRandShopBuyCommand::GetMessage()const
{
	ACString strText;
	if (m_pShopping && m_pShopping->CanPay()){
		CECGameUIMan *pGameUIMan = CECUIHelper::GetGameUIMan();
		CECIvtrItem *pItem = CECIvtrItem::CreateItem(m_pShopping->GetItemToPay(), 0, 1);		
		CDlgQShop *pDlgQShop = dynamic_cast<CDlgQShop *>(pGameUIMan->GetDialog("Win_QShop"));
		if (m_pShopping->IsFirstTime()){
			const RAND_SHOP_CONFIG *pConfig = m_pShopping->GetConfig();
			strText.Format(pGameUIMan->GetStringFromTable(11260), pItem->GetName(), pDlgQShop->GetCashText(pConfig->price), pDlgQShop->GetCashText(pConfig->first_buy_price));
		}else{
			strText.Format(pGameUIMan->GetStringFromTable(11261), pDlgQShop->GetCashText(m_pShopping->GetItemPrice()), pItem->GetName());
		}
		delete pItem;
	}
	return strText;
}

//	class CDlgBuyConfirm
AUI_BEGIN_COMMAND_MAP(CDlgBuyConfirm, CDlgBase)
AUI_ON_COMMAND("IDOK",			OnCommandConfirm)
AUI_ON_COMMAND("Btn_Protocol",	OnCommandProtocol)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_END_COMMAND_MAP()

CDlgBuyConfirm::CDlgBuyConfirm()
: m_pBuyCmd(NULL)
, m_pBtn_Confirm(NULL)
, m_pBtn_Cancel(NULL)
, m_pTxt_Message(NULL)
, m_pBtn_Protocol(NULL)
, m_messageDefaultHeight(0)
{
}

bool CDlgBuyConfirm::OnInitDialog()
{
	DDX_Control("Txt_Message", m_pTxt_Message);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	DDX_Control("Btn_Cancel", m_pBtn_Cancel);
	DDX_Control("Btn_Protocol", m_pBtn_Protocol);

	m_pTxt_Message->SetForceRenderScroll(true);
	m_messageDefaultHeight = m_pTxt_Message->GetDefaultSize().cy;
	m_strProtocol = GetBaseUIMan()->GetURL("SHOP", "PROTOCOL");
	return true;
}

void CDlgBuyConfirm::OnShowDialog()
{
	if (m_pBtn_Protocol)
		m_pBtn_Protocol->Show(!m_strProtocol.IsEmpty());

	if (m_pBuyCmd){
		m_pBuyCmd->Show();
	}
}

bool CDlgBuyConfirm::Release()
{
	delete m_pBuyCmd;
	m_pBuyCmd = NULL;
	return CDlgBase::Release();
}

void CDlgBuyConfirm::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	CDlgBase::Resize(rcOld, rcNew);
	ResizeByContent();
}

void CDlgBuyConfirm::OnChangeLayoutEnd(bool bAllDone)
{
	ResizeByContent();
}

int CDlgBuyConfirm::GetContentMaxHeight(){
	return 200;
}

int CDlgBuyConfirm::GetContentRequiredHeight(){
	int scaledHeight = m_pTxt_Message->GetRequiredHeight();
	float fWindowScale = GetAUIManager()->GetWindowScale() * GetScale();
	return (int)ceilf(scaledHeight / fWindowScale);
}

int CDlgBuyConfirm::CalculateContentHeight(){
	int result = GetContentRequiredHeight();
	a_Clamp(result, m_messageDefaultHeight, GetContentMaxHeight());
	return result;
}

int CDlgBuyConfirm::GetCotentCurrentHeight(){
	return m_pTxt_Message->GetDefaultSize().cy;
}

bool CDlgBuyConfirm::GetShowContentScroll(){
	return GetContentRequiredHeight() > CalculateContentHeight();
}

void CDlgBuyConfirm::AdjustContent(int deltaHeight){
	m_pTxt_Message->SetForceRenderScroll(GetShowContentScroll());

	SIZE size = m_pTxt_Message->GetDefaultSize();
	size.cy += deltaHeight;
	m_pTxt_Message->SetDefaultSize(size.cx, size.cy);
}

void CDlgBuyConfirm::AdjustButtons(int deltaHeight){
	PAUIOBJECT pObjectsToAdjust[] = {m_pBtn_Confirm, m_pBtn_Cancel, m_pBtn_Protocol};
	for (int i(0); i < sizeof(pObjectsToAdjust)/sizeof(pObjectsToAdjust[0]); ++ i){
		PAUIOBJECT pObj = pObjectsToAdjust[i];
		POINT point = pObj->GetDefaultPos();
		point.y += deltaHeight;
		pObj->SetDefaultPos(point.x, point.y);
	}
}

void CDlgBuyConfirm::AdjustDialog(int deltaHeight){
	SIZE size = GetDefaultSize();
	size.cy += deltaHeight;
	SetDefaultSize(size.cx, size.cy);
}

void CDlgBuyConfirm::ResizeByContent()
{
	int deltaHeight = CalculateContentHeight() - GetCotentCurrentHeight();
	AdjustContent(deltaHeight);
	AdjustButtons(deltaHeight);
	AdjustDialog(deltaHeight);
}

void CDlgBuyConfirm::SetMessage(const ACHAR *szText)
{
	m_pTxt_Message->SetText(szText ? szText : _AL(""));
	ResizeByContent();
}

void CDlgBuyConfirm::OnCommandProtocol(const char *szCommand)
{
	if (!m_strProtocol.IsEmpty())
		GetGameUIMan()->NavigateURL(m_strProtocol);
}

void CDlgBuyConfirm::OnCommandCancel(const char *szCommand)
{
	if (m_pBuyCmd){
		m_pBuyCmd->Cancel();
	}
	Clear();
	Show(false);
}

void CDlgBuyConfirm::Clear()
{
	delete m_pBuyCmd;
	m_pBuyCmd = NULL;
	SetMessage(_AL(""));
}

void CDlgBuyConfirm::OnCommandConfirm(const char *szCommand)
{
	if (m_pBuyCmd){
		m_pBuyCmd->Confirm();
		Clear();
	}
	Show(false);
}

void CDlgBuyConfirm::Buy(CECShopBase *pShop, int itemIndex, int buyIndex, bool needConfirm, CECShopBuyCommandObserver *pObserver, bool once)
{
	Buy(new CECShopBuyCommand(pShop, itemIndex, buyIndex), needConfirm, pObserver, once);
}

void CDlgBuyConfirm::Buy(CECBuyCommand *pCmd, bool needConfirm, CECBuyCommandObserver *pObserver, bool once)
{
	//	清除已有内容
	OnCommandCancel("");

	//	构建新命令操作
	m_pBuyCmd = pCmd;
	if (pObserver){
		m_pBuyCmd->RegisterObserver(pObserver, once);
	}

	//	操作新内容
	ACString strText = m_pBuyCmd->GetMessage();	
	SetMessage(strText);

	if (!needConfirm){
		OnCommandConfirm(NULL);
	}else{
		Show(true, true);
	}
}

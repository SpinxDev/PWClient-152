// Filename	: DlgQuickPay.cpp
// Creator	: Han Guanghui
// Date		: 2012/10/9

#include "DlgQuickPay.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_UIConfigs.h"
#include "qpannouncediscount.hpp"
#include "qpgetactivatedservices_re.hpp"
#include "qpaddcash_re.hpp"

AUI_BEGIN_COMMAND_MAP(CDlgQuickPay, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("BtnAddCash", OnCommandAddCash)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgQuickPay, CDlgBase)
AUI_ON_EVENT("ListMerchants",	WM_LBUTTONUP,	OnEventSelectList)
AUI_ON_EVENT("EdtGold", WM_KEYUP, OnEventInputGold)
AUI_END_EVENT_MAP()

/* 
discount是折扣信息，根据充值金额的多少，有不同的折扣，即会分级；举例来说：
1、	如果无论充值多少都无折扣，则discount vector的size=1，且其中那个元素的ammount_begin=0,discount=100;
2、	如果>=20000时有95折，则discount vector的size=2，且两个元素的amount_begin=0，discount=100，及amount_begin=20000，discount=95；
3、	如果>=20000时有95折且>=50000时有9折，则discount vector的size=3，且三个元素的amount_begin=0，discount=100； amount_begin=20000，
discount=95；amount_begin=50000，discount=90；
*/
int CDlgQuickPay::MERCHANT_INFO::GetDiscount(__int64 amount)
{
	int ret(1);
	if (discount.size() < 1)
		ASSERT(!"NO DISCOUNT INFO!");
	else
	{
		int i(0);
		int len = (int)discount.size();
		for (; i < len; ++i)
		{
			if(i == len - 1)
				ret = discount[i].discount;
			else
			{
				if (amount >= discount[i + 1].amount_begin)	continue;
				else
				{
					ret = discount[i].discount;
					break;
				}
			}
		}
	}
	return ret;
}
CDlgQuickPay::CDlgQuickPay():
m_pMerchantsList(NULL),
m_pTextDiscountLevel(NULL),
m_pEdtGold(NULL),
m_pEdtMoney(NULL),
m_iAddedGold(0),
m_iAddedMoney(0),
m_iState(STATE_NULL),
m_bAddingCash(false),
m_iSelMerchant(0),
m_bGetMerchantInfo(false),
m_pMerchantsCombo(NULL)
{

}

CDlgQuickPay::~CDlgQuickPay()
{

}

bool CDlgQuickPay::OnInitDialog()
{
	DDX_Control("ListMerchants", m_pMerchantsList);
	DDX_Control("TextDiscountLevel", m_pTextDiscountLevel);
	DDX_Control("EdtGold", m_pEdtGold);
	DDX_Control("EdtMoney", m_pEdtMoney);
	DDX_Control("ComboMerchants", m_pMerchantsCombo);
	EnableItems(false);
	return true;
}

void CDlgQuickPay::OnTick()
{
	
}

void CDlgQuickPay::OnShowDialog()
{
	if (!m_bGetMerchantInfo)
	{
		m_bGetMerchantInfo = true;
		GetMerchantsActivatedInfo();
	}
}

void CDlgQuickPay::OnCommandCancel(const char * szCommand)
{
	Show(false);
}

void CDlgQuickPay::OnCommandAddCash(const char * szCommand)
{
	if (STATE_READY == m_iState)
	{
		SetDiscountMoney();
		if (IsMerchantActivated(m_iSelMerchant)) AddCash();
		else
		{
			m_bAddingCash = true;
			GetMerchantsActivatedInfo();
		}
	}
}

void CDlgQuickPay::OnEventSelectList(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int iSel = m_pMerchantsList->GetCurSel();
	if (iSel >= 0 && iSel < m_pMerchantsList->GetCount())
		m_pTextDiscountLevel->SetText(FormatDiscountLevel(m_pMerchantsList->GetItemData(iSel)));
}
void CDlgQuickPay::OnEventInputGold(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	SetDiscountMoney();
}
void CDlgQuickPay::GetMerchantsActivatedInfo()
{
	GetGameSession()->quickpay_GetActivatedServices();
	ChangeState(STATE_REQUEST_ACTIVATED_MERCHANTS);
}

void CDlgQuickPay::SetDiscountMoney()
{
	m_iAddedGold = a_atoi(m_pEdtGold->GetText());
	m_iAddedMoney = GetMoneyByGold(m_iAddedGold);
	int index = m_pMerchantsCombo->GetCurSel();
	m_iSelMerchant = m_pMerchantsCombo->GetItemData(index);
	m_iAddedMoneyAfterDiscount = m_iAddedMoney * 
		GetMerchantDiscount(m_iSelMerchant, m_iAddedMoney) / 100;
	ACString strMoney;
	strMoney.Format(_AL("%I64d.%d%d"), m_iAddedMoneyAfterDiscount / 100, (int)(m_iAddedMoneyAfterDiscount % 100) / 10, (int)(m_iAddedMoneyAfterDiscount % 10));
	m_pEdtMoney->SetText(strMoney);
}
bool CDlgQuickPay::IsMerchantActivated(int id)
{
	bool ret(false);
	for (int i = 0; i < (int)m_ActivatedMerchants.size(); ++i)
	{
		if (m_ActivatedMerchants[i] == id)
		{
			ret = true;
			break;
		}
	}
	return ret;
}

void CDlgQuickPay::ActivateMerchant()
{
	GetGameUIMan()->SSOGetQuickPayTicket();
	ChangeState(STATE_READY);
}

void CDlgQuickPay::EnableItems(bool bEnable)
{
	GetDlgItem("BtnAddCash")->Enable(bEnable);
}

void CDlgQuickPay::AddCash()
{
	if (m_iAddedMoneyAfterDiscount && m_iAddedMoney && m_iSelMerchant)
	{	
		GetGameSession()->quickpay_AddCash((int)m_iAddedMoney, 
			(int)m_iAddedMoneyAfterDiscount, m_iSelMerchant);
		ChangeState(STATE_ADDING_CASH);
	}
	
	else ASSERT(!"Add cash param invalid!");

	m_bAddingCash = false;
}
void CDlgQuickPay::ChangeState(int iState)
{
	if (!CECUIConfig::Instance().GetGameUI().bEnableQuickPay) return;
	
	if (STATE_NULL == m_iState)
	{
		switch(iState)
		{
		case STATE_READY:
			EnableItems(true);
			m_iState = STATE_READY;
			break;
		case STATE_REQUEST_ACTIVATED_MERCHANTS:
			m_iState = STATE_REQUEST_ACTIVATED_MERCHANTS;
			EnableItems(false);
			break;
		default:
			ASSERT(!"State Transform error 1!");
			break;
		}
	}
	else if (STATE_READY == m_iState)
	{
		switch(iState)
		{
		case STATE_REQUEST_ACTIVATED_MERCHANTS:
			EnableItems(false);
			m_iState = STATE_REQUEST_ACTIVATED_MERCHANTS;
			break;
		case STATE_READY:
			m_iState = STATE_READY;
			break;
		case STATE_ADDING_CASH:
			EnableItems(false);
			m_iState = STATE_ADDING_CASH;
			break;
		default:
			ASSERT(!"State Transform error 2!");
			break;
		}
	}
	else if (STATE_REQUEST_ACTIVATED_MERCHANTS == m_iState)
	{
		switch(iState)
		{
		case STATE_READY:
			EnableItems(true);
			m_iState = STATE_READY;
			break;
		default: 
			ASSERT(!"State Transform error 3!");
			break;
		}
	}
	else if (STATE_ADDING_CASH == m_iState)
	{
		switch(iState)
		{
		case STATE_READY:
			EnableItems(true);
			m_iState = STATE_READY;
			break;
		default:
			ASSERT(!"State Transform error 5!");
			break;
		}
	}
}
ACString CDlgQuickPay::GetMerchantName(int id)
{
	ACString ret;
	int i(0);
	int len = (int)m_MerchantsDiscount.size();
	for (; i < len; ++i)
	{
		if (id == m_MerchantsDiscount[i].id)
		{
			ret = m_MerchantsDiscount[i].name;
			break;
		}
	}
	return ret;
}

const CDlgQuickPay::DISCOUNTS* CDlgQuickPay::GetMerchantDiscountLevel(int id)
{
	DISCOUNTS* ret = NULL;
	int i(0);
	int len = (int)m_MerchantsDiscount.size();
	for (; i < len; ++i)
	{
		if (id == m_MerchantsDiscount[i].id)
		{
			ret = &m_MerchantsDiscount[i].discount;
			break;
		}
	}	
	return ret;
}

int CDlgQuickPay::GetMerchantDiscount(int id, __int64 amount)
{
	int ret(1);
	bool bFound(false);
	int i(0);
	int len = (int)m_MerchantsDiscount.size();
	for (; i < len; ++i)
	{
		if (id == m_MerchantsDiscount[i].id)
		{
			ret = m_MerchantsDiscount[i].GetDiscount(amount);
			bFound = true;
			break;
		}
	}
	ASSERT(bFound && "can't find merchant discount!");
	return ret;
}
ACString CDlgQuickPay::FormatDiscountLevel(int id)
{
	ACString ret;
	ACString strAmount;
	ACString strDiscount;
	const DISCOUNTS* discount = GetMerchantDiscountLevel(id);
	if (discount)
	{
		int i(0);
		int len = (int)discount->size();
		for (; i < len; ++i)
		{
			if (i == len - 1)
				strAmount.Format(GetGameUIMan()->GetStringFromTable(10010), (*discount)[i].amount_begin / 100);
			else
				strAmount.Format(GetGameUIMan()->GetStringFromTable(10013), (*discount)[i].amount_begin / 100, (*discount)[i + 1].amount_begin / 100 - 1);
			if ((*discount)[i].discount == 100)
				strDiscount = GetGameUIMan()->GetStringFromTable(10018);
			else
				strDiscount.Format(GetGameUIMan()->GetStringFromTable(10009), (*discount)[i].discount / 10, (*discount)[i].discount % 10);
			ret += strAmount + strDiscount + _AL("\r");
		}
	}
	else ASSERT(!"Can't find Discount Info!");
	return ret;
}

void CDlgQuickPay::SetMerchantsList()
{
	int iSize = (int)m_MerchantsDiscount.size();
	int i(0);
	m_pMerchantsCombo->ResetContent();
	if (iSize)
	{
		for (; i < iSize; ++i)
		{
			m_pMerchantsCombo->AddString(m_MerchantsDiscount[i].name);
			m_pMerchantsCombo->SetItemData(i, m_MerchantsDiscount[i].id);
		}
		m_pMerchantsCombo->SetCurSel(0);
		m_pTextDiscountLevel->SetText(FormatDiscountLevel(m_pMerchantsCombo->GetItemData(0)));
		if (!m_bAddingCash) ChangeState(STATE_READY);
	}
	else ASSERT(!"AnnounceDiscount has null info!");
}

void CDlgQuickPay::OnPrtcQuickPay(GNET::Protocol* pProtocol)
{
	switch(pProtocol->GetType())
	{
	case PROTOCOL_QPANNOUNCEDISCOUNT:
		OnPrtcAnnounceDiscount(pProtocol);
		break;
	case PROTOCOL_QPGETACTIVATEDSERVICES_RE:
		OnPrtcGetActivatedServices_Re(pProtocol);
		break;
	case PROTOCOL_QPADDCASH_RE:
		OnPrtcAddCash_Re(pProtocol);
		break;
	default:
		break;
	}
}

void CDlgQuickPay::OnPrtcAnnounceDiscount(GNET::Protocol* pProtocol)
{
	m_MerchantsDiscount.clear();
	QPAnnounceDiscount* p = (QPAnnounceDiscount*)pProtocol;
	int iSize = (int)p->discount.size();
	int i(0);
	for (; i < iSize; ++i)
	{
		MERCHANT_INFO info;
		info.id = p->discount[i].id;
		ACString strName((const ACHAR*)p->discount[i].name.begin(), 
			p->discount[i].name.size() / sizeof (ACHAR));
		info.name = strName;
		int j(0);
		for (; j < (int)p->discount[i].discount.size(); ++j)
		{
			DISCOUNT_LEVEL level;
			level.amount_begin = p->discount[i].discount[j].amount_begin;
			level.discount = p->discount[i].discount[j].discount;
			info.discount.push_back(level);
		}
		m_MerchantsDiscount.push_back(info);
	}
	SetMerchantsList();
}

void CDlgQuickPay::OnPrtcGetActivatedServices_Re(GNET::Protocol* pProtocol)
{
	m_ActivatedMerchants.clear();
	QPGetActivatedServices_Re* p = (QPGetActivatedServices_Re*)pProtocol;
	if (0 == p->retcode)
	{
		int i(0);
		for (; i < (int)p->merchants.size(); ++i)
			m_ActivatedMerchants.push_back(p->merchants[i]);
		if (m_bAddingCash)
		{
			if (IsMerchantActivated(m_iSelMerchant)) AddCash();
			else ActivateMerchant();
		}
		else ChangeState(STATE_READY);
	}
	else if (m_bAddingCash)
	{
		ACString str;
		str.Format(GetStringFromTable(10012), GetMerchantName(m_iSelMerchant));
		GetGameUIMan()->MessageBox("QuickPayActivateMerchant", str, 
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

//Game2AU和AU2Game的消息均为异步，所以考虑到游戏客户端给玩家的提示不至于太突兀，将提示信息放在聊天信息栏显示
void CDlgQuickPay::OnPrtcAddCash_Re(GNET::Protocol* pProtocol)
{
	QPAddCash_Re* p = (QPAddCash_Re*)pProtocol;
	ACString strHint;
	strHint.Format(_AL("%s: %s"), 
		GetMerchantName(p->merchant_id), 
		GetGameUIMan()->GetStringFromTable(10000 + p->retcode));
	GetGameUIMan()->AddChatMessage(strHint, GP_CHAT_SYSTEM);
	// 返回错误吗为尚未开通或折扣错误时，更新信息
	if (p->retcode == 3 || p->retcode == 7)
		GetMerchantsActivatedInfo();
	else
		ChangeState(STATE_READY);
}
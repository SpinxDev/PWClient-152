// Filename	: DlgQuickPay.h
// Creator	: Han Guanghui
// Date		: 2012/10/9

#ifndef _DLGQUICKPAY_H_
#define _DLGQUICKPAY_H_

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUITextArea.h"
#include "AUIEditBox.h"
#include "AUIComboBox.h"

namespace GNET{
	class Protocol;
}

class CDlgQuickPay : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgQuickPay();
	virtual ~CDlgQuickPay();

	void				OnCommandCancel(const char * szCommand);
	void				OnCommandAddCash(const char * szCommand);

	void				OnEventSelectList(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void				OnEventInputGold(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void				OnPrtcQuickPay(GNET::Protocol* pProtocol);

	struct	DISCOUNT_LEVEL{
		int amount_begin;
		int discount;
	};
	typedef	abase::vector<DISCOUNT_LEVEL> DISCOUNTS;
	
	struct MERCHANT_INFO{
		int id;
		ACString name;
		DISCOUNTS discount;

		int GetDiscount(__int64 amount);
	};
	typedef abase::vector<MERCHANT_INFO> MERCHANTSDISCOUNT;
	
	enum{
		STATE_NULL,
		STATE_REQUEST_ACTIVATED_MERCHANTS,
		STATE_READY,
		STATE_ADDING_CASH,
	};

	void				ActivateMerchant();
	void				EnableItems(bool bEnable);
	void				ChangeState(int iState);

protected:
	virtual void		OnTick();
	virtual bool		OnInitDialog();
	virtual void		OnShowDialog();

	void				OnPrtcAnnounceDiscount(GNET::Protocol* pProtocol);
	void				OnPrtcGetActivatedServices_Re(GNET::Protocol* pProtocol);
	void				OnPrtcAddCash_Re(GNET::Protocol* pProtocol);

	void				AddCash();
	ACString			GetMerchantName(int id);
	const DISCOUNTS*	GetMerchantDiscountLevel(int id);
	int					GetMerchantDiscount(int id, __int64 amount);
	ACString			FormatDiscountLevel(int id);
	void				SetMerchantsList();
	__int64				GetMoneyByGold(__int64 gold){return 100 * gold;};
	bool				IsMerchantActivated(int id);
	void				GetMerchantsActivatedInfo();
	void				SetDiscountMoney();

	MERCHANTSDISCOUNT	m_MerchantsDiscount;
	abase::vector<int>	m_ActivatedMerchants;
	PAUILISTBOX			m_pMerchantsList;
	PAUICOMBOBOX		m_pMerchantsCombo;
	PAUITEXTAREA		m_pTextDiscountLevel;
	PAUIEDITBOX			m_pEdtGold;
	PAUIEDITBOX			m_pEdtMoney;
	__int64				m_iAddedGold;
	__int64				m_iAddedMoney;
	__int64				m_iAddedMoneyAfterDiscount;
	int					m_iState;
	int					m_iSelMerchant;
	bool				m_bAddingCash;
	bool				m_bGetMerchantInfo;
};












#endif //_DLGQUICKPAY_H_

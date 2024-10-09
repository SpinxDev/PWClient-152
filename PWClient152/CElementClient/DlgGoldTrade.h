// File		: DlgGoldTrade.h
// Creator	: Xiao Zhou
// Date		: 2006/9/20

#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUILabel.h"
#include "AUIEditBox.h"

class CDlgGoldTrade : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	struct StockInfo
	{
		int volume;
		int	price;
		int time;
		int tid;
	};

	enum
	{
		GOLD_ACTION_BILL_RE,
		GOLD_ACTION_ACCOUNT_RE,
		GOLD_ACTION_COMMISSION_RE,
		GOLD_ACTION_TRANSACTION_RE,
		GOLD_ACTION_CANCEL_RE,
	};

	CDlgGoldTrade();
	virtual ~CDlgGoldTrade();

	void OnCommandAccount(const char * szCommand);
	void OnCommandInquire(const char * szCommand);
	void OnCommandSell(const char * szCommand);
	void OnCommandBuy(const char * szCommand);
	void OnCommandConfirm(const char * szCommand);
	void OnCommandRefresh(const char * szCommand);
	void OnCommandPwd(const char * szCommand);
	void OnCommandUnlock(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
	
	void GoldAction(int idAction, void *pData);
	void SetLocked(bool bLock);
	bool IsLocked() { return m_bLocked; }

	abase::vector<StockInfo>	m_vecAccountOrders;
	
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();
	bool CheckRetcode(int retcode, int defaulErrMsg);

	PAUILISTBOX		m_pLst_Sell;
	PAUILISTBOX		m_pLst_Buy;
	PAUIEDITBOX		m_pTxt_Num;
	PAUILABEL		m_pTxt_Gold;
	PAUILABEL		m_pTxt_Gold1;
	PAUIOBJECT		m_pTxt_Money1;
	PAUIOBJECT		m_pTxt_Money2;
	PAUIOBJECT		m_pTxt_Money4;
	PAUIOBJECT		m_pTxt_Money5;
	PAUILABEL		m_pTxt_Poundage;
	int				m_nBuy;
	bool			m_bLocked;
};

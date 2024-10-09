// File		: DlgGoldAccount.h
// Creator	: Xiao Zhou
// Date		: 2006/9/20

#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUILabel.h"
#include "AUIEditBox.h"

class CDlgGoldAccount : public CDlgBase  
{
	friend class CDlgGoldTrade;

	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgGoldAccount();
	virtual ~CDlgGoldAccount();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandGoldMax(const char * szCommand);
	void OnCommandMoneyMax(const char * szCommand);
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();

	PAUILABEL		m_pLab_Gold;
	PAUILABEL		m_pLab_Gold1;
	PAUILABEL		m_pLab_Money;
	PAUILABEL		m_pLab_Money1;
	PAUIEDITBOX		m_pTxt_Gold;
	PAUIEDITBOX		m_pTxt_Silver;
	PAUIOBJECT		m_pTxt_Money;
};

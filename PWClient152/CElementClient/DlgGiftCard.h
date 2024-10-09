// Filename	: DlgGiftCard.h
// Creator	: Han Guanghui
// Date		: 2013/07/30

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"

class CDlgGiftCard : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgGiftCard();
	virtual ~CDlgGiftCard();

	void OnCommandConfirm(const char* szCommand);
	void OnCommandCancel(const char* szCommand);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	PAUIEDITBOX		m_pTxt_Input;
};

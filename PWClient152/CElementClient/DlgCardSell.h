// File		: DlgCardSell.h
// Creator	: Xiao Zhou
// Date		: 2006/1/19

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIListBox.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"

class CDlgCardSell : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgCardSell();
	virtual ~CDlgCardSell();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancelSell(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void CardAction(int idAction, void *pData);

	enum
	{
		CARD_ACTION_GETSELFSELLLIST_RE = 0,
		CARD_ACTION_GETSHOPSELLLIST_RE,
		CARD_ACTION_SELLPOINT_RE,
		CARD_ACTION_CANCELPOINT_RE,
		CARD_ACTION_BUYPOINT_RE,
		CARD_ACTION_ANNOUNCESELLRESULT,
	};

	
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();

	PAUIEDITBOX		m_pTxt_Price;
	PAUILISTBOX		m_pLst_Item;
	PAUILABEL		m_pTxt_RestPoint;
	PAUILABEL		m_pTxt_SellPoint;
	PAUILABEL		m_pTxt_MaxPoint;
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	int				m_nSellPoint;
	
};

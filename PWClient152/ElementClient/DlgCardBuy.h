// File		: DlgCardBuy.h
// Creator	: Xiao Zhou
// Date		: 2006/1/19

#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUILabel.h"

class CDlgCardBuy : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgCardBuy();
	virtual ~CDlgCardBuy();

	void OnCommandNext(const char * szCommand);
	void OnCommandLast(const char * szCommand);
	void OnCommandRefresh(const char * szCommand);
	void OnCommandBuy(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
	
	void CardAction(int idAction, void *pData);
	
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();

	PAUILISTBOX		m_pLst_Item;
	PAUILABEL		m_pTxt_RestPoint;
	int				m_nCurPage;
};

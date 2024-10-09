// File		: DlgTrade.h
// Creator	: Xiao Zhou
// Date		: 2005/8/16

#pragma once

#include "DlgBase.h"
#include "EC_IvtrTypes.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CDlgTrade : public CDlgBase  
{
	friend class CDlgInputNO;
	friend class CECGameUIMan;

	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgTrade();
	virtual ~CDlgTrade();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Tick(void);

	void OnCommandCANCEL(const char * szCommand);
	void OnCommandLock(const char * szCommand);
	void OnCommandComplete(const char * szCommand);
	void OnCommandChoosemoney(const char * szCommand);
	void OnCommandDisclaimer(const char * szCommand);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDBLCLK(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void TradeAction(int idPlayer, int idTrade, int idAction, int nCode);

	enum
	{
		TRADE_ACTION_NONE = 0,
		TRADE_ACTION_ALTER,
		TRADE_ACTION_LOCK,
		TRADE_ACTION_UNLOCK,
		TRADE_ACTION_CANCEL,
		TRADE_ACTION_DEAL,
		TRADE_ACTION_END
	};

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	bool UpdateTradeInfo();
	virtual void DoDataExchange(bool bSave);
	
	int						m_idTradeAction;
	AUIStillImageButton *	m_pBtnLock;
	AUIStillImageButton *	m_pBtnLock2;
	AUIStillImageButton *	m_pBtnConfirm;
	AUIStillImageButton *	m_pBtnConfirm2;
	AUIImagePicture *		m_pMyItem[IVTRSIZE_DEALPACK];
	AUIImagePicture *		m_pYourItem[IVTRSIZE_DEALPACK];
	PAUIOBJECT			m_pTxtGold;
	PAUIOBJECT			m_pTxtGold2;
	AUIObject *				m_pTxtName;

	bool					m_bNameComplete;		//	职业、等级是否已显示
};

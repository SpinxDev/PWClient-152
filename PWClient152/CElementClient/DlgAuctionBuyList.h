// Filename	: DlgAuctionBuyList.h
// Creator	: Xiao Zhou
// Date		: 2005/11/10


#pragma once

#include "DlgAuctionBase.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIComboBox.h"
#include "AUIListBox.h"
#include "AUIStillImageButton.h"
#include "vector.h"

class AFileImage;
class elementdataman;

class CDlgAuctionBuyList : public CDlgAuctionBase  
{
	friend class CDlgAuctionSellList;
	friend class CDlgAuctionList;
	friend class CDlgAuctionBase;
	friend class CDlgAuctionSearch;
	friend class CDlgNPC;

	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgAuctionBuyList();
	virtual ~CDlgAuctionBuyList();

	void OnCommandSearchID(const char* szCommand);
	void OnCommandSearchMode(const char* szCommand);
	void OnCommandNext(const char* szCommand);
	void OnCommandLast(const char* szCommand);
	void OnCommandRefresh(const char* szCommand);
	void OnCommandAddToFavor(const char* szCommand);
	void OnCommandSave(const char* szCommand);
	void OnCommandDelete(const char* szCommand);
	void OnCommandFit(const char *szCommand);

	void AddToFavor(int id);
	
	void UpdateItem(void *pData);
	virtual void UpdateList(void *pData);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	
	PAUIEDITBOX				m_pTxt_AuctionID;
	PAUIEDITBOX				m_pTxt_PackGold;
	PAUILABEL				m_pTxt_RefreshStatus;
	PAUILABEL				m_pTxt_Search;
	AuctionIDMap			m_mapAuctionID;
	AuctionNameMap			m_mapAuctionName;
	AuctionHintMap			m_mapAuctionHint;
	int						m_nSearchID;
	DWORD					m_dwLastListTime;
	int						m_nPageBegin;
	int						m_nPageEnd;
	int						m_nPageNext;
	int						m_nIDNow;
	int						m_nIDNext;
	int						m_nSubID;
	int						m_nPageReturned;
	bool					m_bReverse;
};


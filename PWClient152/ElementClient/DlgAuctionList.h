// Filename	: DlgAuctionList.h
// Creator	: Xiao Zhou
// Date		: 2005/11/10


#pragma once

#include "DlgAuctionBase.h"
#include "AUIListBox.h"
#include "AUIStillImageButton.h"
#include "AUIListBox.h"

class CDlgAuctionList : public CDlgAuctionBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgAuctionList();
	virtual ~CDlgAuctionList();
	
	void OnCommandGiveUp(const char* szCommand);

	void AuctionAction(int idAction, void *pData);

	enum
	{
		AUCTION_ACTION_OPEN_RE,
		AUCTION_ACTION_BID_RE,
		AUCTION_ACTION_LIST_RE,
		AUCTION_ACTION_CLOSE_RE,
		AUCTION_ACTION_GET_RE,
		AUCTION_ACTION_ATTENDLIST_RE,
		AUCTION_ACTION_EXITBID_RE,
		AUCTION_ACTION_GETITEM_RE,
		AUCTION_ACTION_LISTUPDATE_RE,
	};
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
};


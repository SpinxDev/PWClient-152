// Filename	: DlgChannalJoin.h
// Creator	: Xiao Zhou
// Date		: 2005/10/21

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUIEditBox.h"

class CDlgChannelJoin : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgChannelJoin();
	virtual ~CDlgChannelJoin();

	void OnCommandJoinAsCreator(const char* szCommand);
	void OnCommandJoinAsNumber(const char* szCommand);
	void OnCommandJump(const char* szCommand);
	void OnCommandRefresh(const char* szCommand);
	void OnCommandLast(const char* szCommand);
	void OnCommandNext(const char* szCommand);
	void OnCommandJoin(const char* szCommand);

	void BuildChannelList(void* pChannelList);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	PAUILABEL		m_pTxt_RoomTotal;
	PAUILISTBOX		m_pLst_AllChannel;
	PAUIOBJECT		m_pTxt_CreatorName;
	PAUIEDITBOX		m_pTxt_Number;
	PAUIEDITBOX		m_pTxt_RoomIDStart;
	int				m_nRoomTotal;
	int				m_nPageTotal;
};

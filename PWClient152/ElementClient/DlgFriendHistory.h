// File		: DlgFriendHistory.h
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUITextArea.h"

class CDlgFriendHistory : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgFriendHistory();
	virtual ~CDlgFriendHistory();
	
	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void BuildChatHistory(int idPlayer);

protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnChangeLayoutEnd(bool bAllDone);

	AUITextArea *	m_pTxtContent;
	AUIListBox *	m_pListMessage;
};

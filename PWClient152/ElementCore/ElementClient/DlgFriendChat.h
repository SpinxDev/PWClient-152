// File		: DlgFriendChat.h
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUICheckBox.h"
#include "AUITextArea.h"
#include "DlgChat.h"

class CECIvtrItem;

class CDlgFriendChat : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	void OnCommandCANCAL(const char *szCommand);
	void OnCommandSend(const char *szCommand);
	void OnCommandPopface(const char *szCommand);
	void OnCommandHistory(const char *szCommand);
	void OnCommandSave(const char *szCommand);
	void OnCommandDelall(const char *szCommand);
	void OnCommandMinimize(const char *szCommand);

	void OnEventSysChar(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnEventMouseMove_Txt_Chat(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Txt_Chat(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Txt_Chat(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp_Txt_Chat(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	CDlgFriendChat();
	virtual ~CDlgFriendChat();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);

	void AddMessageToDisplay(int idOther, const ACHAR *pszSayer, const ACHAR *pszMsg, CECIvtrItem *pItem = NULL);
	
protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	
	PAUIEDITBOX	m_pTxtContent;
	AUITextArea *	m_pTxtChat;
	AUICheckBox *	m_pChkSave;	
	
	// 保存聊天记录中 LINK 物品的部分，用于点击查询
	typedef CDlgChat::LINKED_MSG LINKED_MSG;
	typedef abase::vector<LINKED_MSG> LinkedChatMessages;
	static LinkedChatMessages m_vecLinkedChatMsg;
	static int m_nMsgIndex;

	static LinkedChatMessages & GetLinkedChatMsg();
};

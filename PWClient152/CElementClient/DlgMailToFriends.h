#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUIComboBox.h"

class CDlgMailToFriends : public CDlgBase  
{	
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	void OnCommandCANCAL(const char *szCommand);
	void OnCommandRefresh(const char *szCommand);
	void OnCommandSendMail(const char *szCommand);
	void OnCommandSelectTemplate(const char *szCommand);

	void OnEventSelectList(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	CDlgMailToFriends();
	virtual ~CDlgMailToFriends();

	void BuildFriendListEx(PAUIDIALOG pDlg = NULL);
	void IncSendMailCount(int iFriendID);
	void FriendAction(int idPlayer, int idGroup, int idAction, int nCode);
	bool NeedAttrackAttention(){return m_bNeedAttrackAttention;}
	void SendmailFail();
	void OnTick();
	void FormatDays(int iDays,ACString& strInOut);
	void FormatName(ACString& strInOut);


protected:
	virtual void DoDataExchange(bool bSave);
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	AUIListBox*	m_pFriendsToMailList;
	AUIComboBox* m_pTemplateComboBox;
	int m_iSendMailPerDay;
	bool m_bNeedAttrackAttention;
	int m_iRefreshStartTime;
};

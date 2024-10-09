// Filename	: DlgMailList.h
// Creator	: Xiao Zhou
// Date		: 2005/10/31

#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUIImagePicture.h"

#include <set>

class CDlgMailList : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgMailList();
	virtual ~CDlgMailList();

	PAUIIMAGEPICTURE GetImageGfx()
	{
		return m_pGfxImage;
	}

	void OnCommandCancel(const char* szCommand);
	void OnCommandNew(const char* szCommand);
	void OnCommandRead(const char* szCommand);
	void OnCommandReadClick(const char* szCommand);
	void OnCommandDelete(const char* szCommand);
	void OnCommandSortSubject(const char* szCommand);
	void OnCommandSortSender(const char* szCommand);
	void OnCommandSortAttach(const char* szCommand);
	void OnCommandSortRestTime(const char* szCommand);

	void OnEventRButtonUp_ListMail(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void MailAction(int idAction, void *pData);
	
	void SetWaitingResponse(int idMail);

	enum
	{
		MAIL_ACTION_GETMAILLIST_RE = 0,
		MAIL_ACTION_GETMAIL_RE,
		MAIL_ACTION_GETATTACH_RE,
		MAIL_ACTION_DELETEMAIL_RE,
		MAIL_ACTION_PRESERVEMAIL_RE,
		MAIL_ACTION_ANNOUNCENEWMAIL_RE,
		MAIL_ACTION_SENDMAIL_RE
	};
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	ACString GetPreserveTime(int nSendTime, bool bUnRead);

	void DoDeleteOne(int iMailID);
	void DoReadOne(int iMailID);

	bool IsMailWithItem(int iRow);
	bool IsMailUnread(int iRow);
	bool IsMailPreserve(int iRow);

	void SelectRowOnly(int iRow);

	PAUILISTBOX		m_pLst_Mail;
	bool			m_bSortSubject;
	bool			m_bSortSender;
	bool			m_bSortAttach;
	bool			m_bSortRestTime;

	PAUIIMAGEPICTURE		m_pGfxImage;

	std::set<int> m_setToDelete;
	std::set<int> m_setToRead;

	friend class CDlgMailOption;
};

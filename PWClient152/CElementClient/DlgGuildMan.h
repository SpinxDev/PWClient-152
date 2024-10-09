// Filename	: DlgGuildMan.h
// Creator	: Tom Zhou
// Date		: October 25, 2005

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUITextArea.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CDlgGuildMan : public CDlgBase  
{
	friend class CDlgSystem2;

	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgGuildMan();
	virtual ~CDlgGuildMan();

	void OnCommand_confirm(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_sortname(const char * szCommand);
	void OnCommand_sortjob(const char * szCommand);
	void OnCommand_sorttitle(const char * szCommand);
	void OnCommand_sortlevel(const char * szCommand);
	void OnCommand_sortprof(const char * szCommand);
	void OnCommand_sortol(const char * szCommand);
	void OnCommand_sortcontrib(const char *szCommand);
	void OnCommand_Edit(const char * szCommand);
	void OnCommand_gquit(const char * szCommand);
	void OnCommand_gpass(const char * szCommand);
	void OnCommand_gdemiss(const char * szCommand);
	void OnCommand_gpromote(const char * szCommand);
	void OnCommand_gdemote(const char * szCommand);
	void OnCommand_gkick(const char * szCommand);
	void OnCommand_gtitle(const char * szCommand);
	void OnCommand_ginvite(const char * szCommand);
	void OnCommand_refresh(const char * szCommand);
	void OnCommand_find(const char *szCommand);
	void OnCommand_Diplomacy(const char *szCommand);
	void OnCommandGTTalk(const char *szCommand);
	void OnCommand_SendMassMail(const char *szCommand);

	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void BuildFactionList();
	void SetLastRefreshTime(DWORD dwTime);
	
	bool FindAndSelect(const ACHAR *szPlayerName);

	void ProcessGTEnter();

protected:
	PAUILISTBOX m_pList_Member;
	PAUILABEL m_pTxt_GuildName;
	PAUILABEL m_pTxt_GuildName2;
	PAUILABEL m_pTxt_GuildLevel;
	PAUILABEL m_pTxt_PplCap;
	PAUILABEL m_pTxt_PplOnline;
	PAUIOBJECT m_pTxt_Message;
	PAUISTILLIMAGEBUTTON m_pBtn_GuildPass;
	PAUISTILLIMAGEBUTTON m_pBtn_GuildDemission;
	PAUISTILLIMAGEBUTTON m_pBtn_GuildPromote;
	PAUISTILLIMAGEBUTTON m_pBtn_GuildDemote;
	PAUISTILLIMAGEBUTTON m_pBtn_GuildKick;
	PAUISTILLIMAGEBUTTON m_pBtn_GuildTitle;
	PAUISTILLIMAGEBUTTON m_pBtn_GuildQuit;
	PAUISTILLIMAGEBUTTON m_pBtn_GuildInvite;
	PAUISTILLIMAGEBUTTON m_pBtn_Edit;
	PAUISTILLIMAGEBUTTON m_pBtn_Refresh;
	PAUISTILLIMAGEBUTTON m_pBtn_Diplomacy;
	PAUIIMAGEPICTURE m_pImg_GuildIcon;
	PAUISTILLIMAGEBUTTON m_pBtn_SendMassMail;

	int m_nSortMode;
	DWORD m_dwLastRefreshTime;
	bool m_bGTWaiting;

	virtual void OnTick();
	virtual bool OnInitDialog();

	enum
	{
		SORT_BY_ORDER,
		SORT_BY_NAME,
		SORT_BY_JOB,
		SORT_BY_TITLE,
		SORT_BY_LEVEL,
		SORT_BY_PROFESSION,
		SORT_BY_ONLINE,
		SORT_BY_CONTRIB
	};
};

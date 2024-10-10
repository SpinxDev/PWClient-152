#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"


// ����һ��������ʾ��
// 1���½��࣬�̳���IOnlineReminder
// 2��UIConfig.ini�����ÿ���
// 3��Ingame.stf�����ñ��⡢���ݺͰ�ť���֡�
class IOnlineReminder
{
public:
	virtual ~IOnlineReminder(){}
	virtual bool CanShow() = 0;
	virtual ACString GetTitle() = 0;
	virtual ACString GetContent() = 0;
	virtual ACString GetButtonText() = 0;
	virtual void OnCommandConfirm() = 0;
	inline virtual ACString GetString(int index);
};

class CMailtoFriendReminder : public IOnlineReminder
{
public:
	CMailtoFriendReminder(){};
	~CMailtoFriendReminder(){};

	bool CanShow();
	ACString GetTitle();
	ACString GetContent();
	ACString GetButtonText();
	void OnCommandConfirm();
};

class CActivityReminder : public IOnlineReminder
{
public:
	CActivityReminder(){};
	~CActivityReminder(){};
	
	bool CanShow();
	ACString GetTitle();
	ACString GetContent();
	ACString GetButtonText();
	void OnCommandConfirm();
};

class CDlgOnlineRemind : public CDlgBase  
{	
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:

	void					OnCommandCANCAL(const char *szCommand);
	void					OnCommandConfirm(const char *szCommand);
	void					OnCommandPageNext(const char *szCommand);
	void					OnCommandPagePrevious(const char *szCommand);

	bool CanShow();

	CDlgOnlineRemind();
	virtual ~CDlgOnlineRemind();


protected:
	virtual void			DoDataExchange(bool bSave);
	virtual void			OnShowDialog();
	bool					OnInitDialog();
	IOnlineReminder*		CreateReminder(AString str);
	void					Select(int index);

	typedef abase::vector<IOnlineReminder*> Reminders;
	Reminders				m_Reminders;
	int						m_iIndex;

	PAUILABEL				m_pTitle;
	PAUILABEL				m_pContent;
	PAUISTILLIMAGEBUTTON	m_pButton;
	PAUISTILLIMAGEBUTTON	m_pNext;
	PAUISTILLIMAGEBUTTON	m_pPrevious;
};

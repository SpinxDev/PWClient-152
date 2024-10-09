// Filename	: DlgChannelChat.h
// Creator	: Xiao Zhou
// Date		: 2005/10/21

#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUIEditBox.h"
#include "AUITextArea.h"
#include "AUILabel.h"
#include "AUICheckBox.h"

enum
{
	CDLGCHANNELCHAT_COLOR_PUBLIC = 0,
	CDLGCHANNELCHAT_COLOR_PRIVATE,
	CDLGCHANNELCHAT_COLOR_SPEAKTO,
	CDLGCHANNELCHAT_COLOR_MISC,
	CDLGCHANNELCHAT_COLOR_MAX
};

class CDlgChannelChat : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgChannelChat();
	virtual ~CDlgChannelChat();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);

	void OnCommandCancel(const char* szCommand);
	void OnCommandLeave(const char* szCommand);
	void OnCommandSend(const char* szCommand);
	void OnCommandInvite(const char* szCommand);
	void OnCommandPopface(const char* szCommand);
	void OnCommandMinimize(const char *szCommand);

	void OnEventSysChar(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonUp_ChatMember(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp_ChatMember(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void ChannelAction(int idAction, void *pData);
	void AddChannelChatMessage(const ACHAR *szText, int idType = CDLGCHANNELCHAT_COLOR_MISC);

	int	GetRoomID();
	bool IsCreator();
	void OpenNewChatRoom();

	enum
	{
		CHANNEL_ACTION_CREATE_RE = 0,
		CHANNEL_ACTION_JOIN_RE,
		CHANNEL_ACTION_INVITE,
		CHANNEL_ACTION_INVITE_RE,
		CHANNEL_ACTION_LEAVE,
		CHANNEL_ACTION_EXPEL,
		CHANNEL_ACTION_SPEAK,
	};

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	void PublicSpeak(int idSpeakTo, const ACHAR *szText);

	PAUILABEL				m_pTxt_RoomID;
	PAUILABEL				m_pTxt_Subject;
	PAUILABEL				m_pTxt_Owner;
	PAUILABEL				m_pTxt_Capacity;
	PAUILABEL				m_pTxt_SpeakTo;
	PAUILISTBOX				m_pList_ChatMember;
	PAUIEDITBOX				m_pTxt_Content;
	PAUITEXTAREA			m_pTxt_Chat;
	PAUICHECKBOX			m_pChk_Whisper;
	bool					m_bCreator;
	int						m_nRoomId;
	int						m_idSpeakTo;
	int						m_nCapacity;
	int						m_nNumber;
	static const ACHAR *	m_pszColor[CDLGCHANNELCHAT_COLOR_MAX];
};

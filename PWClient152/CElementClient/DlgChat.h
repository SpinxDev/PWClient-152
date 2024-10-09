// Filename	: DlgChat.h
// Creator	: Tom Zhou
// Date		: October 15, 2005

#pragma once

#include "DlgBase.h"
#include "AUITextArea.h"
#include "Network/EC_GPDataType.h"

#define CDLGCHAT_MAX_CHAT_COLORS	6
#define SUPER_FAR_CRY_EMOTION_SET   6

class AUIImagePicture;
class CPaletteControl;
class A2DSprite;
class CECIvtrItem;

class CDlgChat : public CDlgBase  
{
	friend class CECGameUIMan;
	friend class CDlgFriendChat;
	friend class CDlgChannelChat;

	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	typedef struct _CHAT_MSG
	{
		DWORD dwTime;
		char cChannel;
		char cEmotion;
		ACString strMsg;
		ACString strMsgOrigion;// 玩家原始发言字串
		CECIvtrItem* pItem;
		int nMsgIndex;
		int nPack;
		int nSlot;
		int idPlayer;

		_CHAT_MSG()
			: dwTime(0)
			, cChannel(0)
			, cEmotion(0)
			, pItem(NULL)
			, nMsgIndex(-1)
			, nPack(-1)
			, nSlot(-1)
			, idPlayer(0)
		{}
	} CHAT_MSG, * P_CHAT_MSG;
	
	
	// 私聊相关聊天信息，用于点击查询
	struct LINKED_MSG
	{
		int nMsgIndex;
		CECIvtrItem* pItem;

		LINKED_MSG()
			: nMsgIndex(-1)
			, pItem(NULL)
		{}
	};

public:
	CDlgChat();
	virtual ~CDlgChat();

	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);

	void OnCommand_lock(const char * szCommand);
	void OnCommand_speak(const char * szCommand);
	void OnCommand_popface(const char * szCommand);
	void OnCommand_speakmode(const char * szCommand);
	void OnCommand_channel(const char * szCommand);
	void OnCommand_chgcolor(const char * szCommand);
	void OnCommand_chgsize(const char * szCommand);
	void OnCommand_clear(const char * szCommand);
	void OnCommand_mini(const char * szCommand);
	void OnCommand_set(const char * szCommand);
	void OnCommand_send(const char * szCommand);
	void OnCommand_list(const char * szCommand);
	void OnCommand_createchannel(const char * szCommand);
	void OnCommand_joinchannel(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommandMinimize(const char *szCommand);
	void OnCommand_popface01(const char *szCommand);
	void OnCommand_popface02(const char *szCommand);

	void OnEventChar_Txt_Speech(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventKeyDown_Txt_Speech(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventSysChar_Txt_Speech(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMouseMove_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Face(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	void OnLButtonDownPicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnMouseMovePicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	bool IsLocked();
	int GetChatWinSize();
	int GetChatColor();
	int GetChannelSet();
	int GetActiveChannelSet();
	int GetMsgCount(int nWin);
	void IncreaseMsgCount(int nWin);
	abase::vector<CHAT_MSG> & GetAllChatMsgs();
	abase::vector<LINKED_MSG> & GetWhisperChatMsgs();
	abase::vector<CHAT_MSG> & GetSuperFarCryMsgs();
	void RebuildChatContents(PAUITEXTAREA pText, int nChannelSet);

	static const ACHAR *GetChatColor(int iChannel, int iParameter = -1);
	static abase::vector<CHAT_MSG> m_vecHistory;
	static int m_nCurHistory;

	static bool IsHaveFarCryMsg();
	void UpdateSuperFarCryMsg(bool bForceUpdateContent);
	
	void SwitchToPrivateChat(const ACHAR* szUserName);

	void SwitchCountryChannel();
	void RebuildSpeakway();

	void RefreshWhisperPlayerName(int role = -1);

protected:

	static const ACHAR *m_pszColor[GP_CHAT_MAX];
	static const ACHAR *m_pszWhisperFriendColor;
	static const ACHAR *m_pszKingColor;

	static bool m_bLocked;
	static int m_nChatWinSize;
	static int m_nCurChatColor;
	static int m_nCurChannelSet;
	static int m_nActiveChannelSet;
	static int m_nChatMsgCount1;
	static int m_nChatMsgCount2;
	static DWORD m_dwTickFarCry;
	static DWORD m_dwTickFarCry2;

	static abase::vector<CHAT_MSG> m_vecChatMsg;
	static int m_nMsgIndex;

	static abase::vector<LINKED_MSG> m_whisperChatMsg;    // 私聊中用于可点击查询的聊天信息

	// 号角相关
	static abase::vector<CHAT_MSG> m_vecSuperFarCryMsg;  // 当前剩余要显示的号角信息
	static bool     m_curSuperFarCryMsgValid;            // 当前正处理的号角信息是否合法
	static CHAT_MSG m_curSuperFarCryMsg;                 // 当前正处理的号角信息
	static DWORD    m_curSuperFarCryTick;                // 当前正处理的号角信息时间戳信息
	static int      m_curSuperFarCryShowTime;            // 当前正处理的号角信息正显示次数
	static bool     m_curSuperFarCryShowMode;            // 当前正处理的号角信息显示情况

	static bool IsSuperFarCryMode();  // 判断当前 Win_Chat 聊天栏输入是否为“号角”频道模式
	
	static AUIImagePicture * m_pPicPane;
	static AUIImagePicture * m_pPicIndicator;
	static AUIObject       * m_pDisplay;
	static CPaletteControl * m_pPallete;
	static A2DSprite	   * m_pA2DSpriteColorPic;

	PAUIOBJECT m_pObjLock;

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual	void OnTick();
	virtual bool Render();
	virtual void OnChangeLayoutEnd(bool bAllDone);

	void DetermineChannelByText(const ACHAR *pszText);
	bool GetNameLinkMouseOn(int x, int y, PAUIOBJECT pObj, P_AUITEXTAREA_NAME_LINK pLink);
	bool GetItemLinkItemOn(int x, int y, PAUIOBJECT pObj, AUITEXTAREA_EDITBOX_ITEM * pLink);
	
	void FitSizeToSuperFarCryMsg(bool bAdjustPos);
	void ReAlignChatDialogs();
	void DoSendCountryMessage(int iPack, int iSlot, const ACHAR* pszText, bool bSendMsg = true);
};

/*
 * FILE: EC_GameUIMan.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <windowsx.h>

#include "A3DEngine.h"
#include "A3DViewport.h"
#include "EC_StringTab.h"
#include "EC_RoleTypes.h"
#include "AUILuaManager.h"
#include "AUITextArea.h"
#include "EC_BaseUIMan.h"
#include "EC_Handler.h"
#include "EC_IvtrItem.h"

#include <vector>

struct talk_proc;
struct NPC_ESSENCE;


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////
#define CECGAMEUIMAN_MAX_MARKS			5
#define CECGAMEUIMAN_MARK_NAME_LEN		8
#define CECGAMEUIMAN_MAX_GROUPS			10
#define CECGAMEUIMAN_MAX_CHATS			10
#define CECGAMEUIMAN_MAX_QSHOPITEMS		8
#define CECGAMEUIMAN_MAX_FASHIONSHOPITEMS	6

enum {FASHION_SHOP_ITEM_WIDTH	=	3};
enum {FASHION_SHOP_ITEM_HEIGHT	=	2};
enum {FASHION_SHOP_ITEM_COUNT	=	FASHION_SHOP_ITEM_WIDTH*FASHION_SHOP_ITEM_HEIGHT};

#define TALKPROC_IS_TERMINAL(id)		(((id) & 0x80000000) && ((id) & 0x40000000))
#define TALKPROC_IS_FUNCTION(id)		((id) & 0x80000000)
#define TALKPROC_IS_WINDOW(id)			(!TALKPROC_IS_FUNCTION(id))
#define TALKPROC_GET_FUNCTION_ID(id)	((id) & 0x7FFFFFFF)

#define CECGAMEUIMAN_DISCOUNT(m)		int(0.3f * float(m))

#define FVF_MAPTLVERTEX  (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////
class A3DStream;
class A3DPixelShader;
class CECShortcutSet;
class CECInventory;
class CECIvtrItem;
class CELPrecinct;
class CECCustomizeMgr;
class CECHomeDlgsMgr;
class CECMiniBarMgr;
class CECMapDlgsMgr;
class CECShortcutMgr;
class CECIconStateMgr;
class AUIImagePicture;
class CECScreenEffectMan;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECGameUIMan
//	
///////////////////////////////////////////////////////////////////////////

class CECGameUIMan : public CECBaseUIMan, public CECSSOTicketHandler
{
#include "EC_GameUIMan1.inl"

public:		//	Constructor and Destructor
	CECGameUIMan();
	virtual ~CECGameUIMan();

public:		//	Attributes
	enum
	{
		NOTIFY_PRODUCE_START = 0,
		NOTIFY_PRODUCE_END_ONE,
		NOTIFY_PRODUCE_END,
		NOTIFY_PRODUCE_NULL,
		NOTIFY_SPLIT_START,
		NOTIFY_SPLIT_END,
		NOTIFY_IDENTIFY_END,
		NOTIFY_ENGRAVE_START,
		NOTIFY_ENGRAVE_END,
		NOTIFY_ENGRAVE_RESULT,
		NOTIFY_ADDONREGEN,
		NOTIFY_PRODUCE_PREVIEW,
	};

	enum
	{
		ICONS_ACTION = 0,
		ICONS_SKILL,
		ICONS_INVENTORY,
		ICONS_STATE,
		ICONS_SKILLGRP,
		ICONS_GUILD,
		ICONS_PET,
		ICONS_ELF,
		ICONS_SUITE,
		ICONS_CALENDAR,
		ICONS_PQ,
		ICONS_MAX
	};

	enum  // wallow hint info type
	{
		WHT_DEFAULT = 0,
		WHT_KOREA = 1,
	};

public:		//	Operations

	virtual bool Init(A3DEngine* pA3DEngine, A3DDevice* pA3DDevice, const char* szDCFile=NULL);
	virtual bool Release(void);
	virtual bool Tick(DWORD dwDeltaTime);
	virtual bool Render(void);
	virtual bool OnCommand(const char* szCommand, AUIDialog* pDlg);
	virtual bool OnMessageBox(int iRetVal, AUIDialog* pDlg);
	virtual bool DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void RearrangeWindows(A3DRECT rcOld, A3DRECT rcNew);
	virtual void CalcWindowScale();	
	virtual bool ChangeLayout(const char *pszFilename);
	virtual CECCustomizeMgr * GetCustomizeMgr();

	virtual void HandleRequest(const CECSSOTicketHandler::Request *p);

	bool DealMessageBoxQuickKey(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnWindowScalePolicyChange();

	void AddConsoleLine(const ACHAR* szText, A3DCOLOR col);
	void ChangePrecinct(CELPrecinct *pPrecinct);
	void AddTaskHint(int idTask, int idEvent, const ACHAR *pszHint = NULL);
    void AddHeartBeatHint(const ACHAR* pszMsg);
	void AddChatMessage(const ACHAR *pszMsg, char cChannel,
		int idPlayer = -1, const ACHAR *pszPlayer = NULL, char byFlag = 0, char cEmotion = 0, CECIvtrItem *pItem = NULL,const ACHAR *pszMsgOrigion = NULL);
	void AddFriendMessage(const ACHAR *pszMsg, 
		int idPlayer, const ACHAR *pszPlayer, char byFlag, char cEmotion = 0, CECIvtrItem *pItem = NULL,const ACHAR *pszMsgOrigion = NULL);
	void AddInformation(int nType, const char *pszType, const ACHAR *pszMsg,
		DWORD dwLife, DWORD dwData1, DWORD dwData2, DWORD dwData3);
	void RemoveInformation(int nType, const ACHAR* pszMsg);
	void AddSysAuctionMessage(int idType, const void* pBuf, size_t sz);

	bool UpdateTask(unsigned long idTask, int reason);
	void UpdateFactionList();
	void UpdateChannelList(void* pChannelList);
	bool UpdateTeach(int nPage = -1);
	bool UpdateTeam(bool bUpdateNear = false);
	bool UpdateGuide();

	void PopupReviveDialog(bool bOpen = true);
	void PopupStorageDialog(bool bClose = false);
	void PopupAccountBoxDialog(bool bClose=false);
	void PopupInviteGroupMenu(int idLeader, const ACHAR* szLeaderName, int iLeaderProf, int iTeamSeq, int nMode = 0);
	void PopupTradeInviteMenu(int idPlayer, DWORD dwHandle);
	void PopupTradeResponseMenu(int idPlayer, int idTrade, bool bAgree);
	void PopupFactionInviteMenu(int idPlayer, const ACHAR *pszFaction, DWORD dwHandle);
	void PopupPlayerContextMenu(int idPlayer = -1, int x = -1, int y = -1);
	void PopupFriendInviteMenu(int idPlayer, const ACHAR *pszName, DWORD dwHandle);
	void PopupBoothDialog(bool bOpen = true, bool bOwner = true, int idOwner=0);
	void PopupNPCDialog(talk_proc *pTalk);
	void PopupNPCDialog(const NPC_ESSENCE *pEssence);
	void PopupTaskFinishDialog(int idTask, talk_proc *pTalk);
	void PopupFaceLiftDialog();
	void PopupInstanceCountDown(int nTime,int nReason = 1);
	void PopupWorldCountDown(int nTime);
	void PopupNewbieGiftRemind();

	void TraceTask(unsigned long idTask);
	bool UIControlCursor();
	void ChangeWorldInstance(int idInstance);
	void ServiceNotify(int idService, void *pData);
	void TradeAction(int idPlayer, int idTrade, int idAction, int nCode);
	void FriendAction(int idPlayer, int idGroup, int idAction, int nCode);
	void ChannelAction(int idAction, void *pData);
	void MailAction(int idAction, void *pData);
	void AuctionAction(int idAction, void *pData);
	void BattleAction(int idAction, void *pData);
	void GoldAction(int idAction, void *pData);
	void WebTradeAction(int idAction, void *pData);
	void UserCouponAction(int idAction, void *pData);
	void UserCashAction(void* pProtocol);
	void SysAuctionAction(int idAction, void *pData);
	void WeddingBookList(void *pData);
	void WeddingBookSuccess(void *pData);
	void CountryAction(DWORD cmd, void *pData);
	void KingAction(void* pData);
	void SetMarqueeMsg(const ACHAR *pszMsg);
	void GetUserLayout(void *pvData, DWORD &dwSize);
	bool SetUserLayout(void *pvData, DWORD dwSize);
	void BuildFriendList(PAUIDIALOG pDlg = NULL);
	void BuildFriendListEx();
	void EndNPCService();
	void GetEmotionList(A2DSprite **ppA2DSprite, abase::vector<AUITEXTAREA_EMOTION> **ppEmotion, char cEmotion = 0);
	void MeridiansNotify();
	void MeridiansImpactResult(int index, int result);
	
	A2DSprite* GetGuildIcon(const char *szName, int *pIndex);
	bool PlayerIsBlack(int idPlayer);

	//	Set UI enable flag
	void EnableUI(bool bEnable);
	bool IsUIEnabled() { return m_bUIEnable; }

	void ChangeAutoHome(bool bEnter);
	
	bool IsNeedShowReferralHint();
	
	bool CustomizeHotKey(int iUsageCustomize, int nHotKey, DWORD dwModifier);
	
	void VerifyComboSkillUI();

	ACString GetFormatClass(unsigned int class_masks, bool reverse);
	int GetIconIndex(int type, const AString& szFile) const;
	A2DSprite* GetIconCover(int type);
	void SetCover(AUIImagePicture *pImg, const char *szIconFile, int iconset=ICONS_INVENTORY);
	void SetHint(AUIObject *pObj, CECIvtrItem *pItem);
	
	// 将时装的描述字符串中加入颜色文字
	bool AddFashionDescByColor(const ACString& fashionDesc, ACString& newDesc, A3DCOLOR color);
	// 获取物品在交易时的悬浮提示，bShowFahionColor=true则显示时装颜色，bCompareEquip=true则显示当前装备
	ACString GetItemDescHint(CECIvtrItem* pItem, bool bShowFahionColor = false, bool bCompareEquip = false, int iDescType = CECIvtrItem::DESC_NORMAL, CECInventory* pInventory = NULL);
	// 获取与某物品对应的当前装备或卡牌。戒指可能有两件对应装备
	void GetRelatedEquipItems(CECIvtrItem* pItem, std::vector<CECIvtrItem*>& relatedItems);
	// 获取物品和当前装备物品比较时显示的Hint字符串
	ACString GetItemDescForCompare(CECIvtrItem* pItem, int iDescType = CECIvtrItem::DESC_NORMAL, CECInventory* pInventory = NULL);
	// 获取包含“已装备”的物品描述,“已装备”几个字放在第一行下方
	ACString GetItemDescWithAlreadyEquipped(CECIvtrItem* pItem, int iDescType = CECIvtrItem::DESC_NORMAL, CECInventory* pInventory = NULL);

	static ACString ConvertChatString(const ACHAR *pszChat);
	static ACString FilterInvalidTags(const ACHAR *szText, bool bFilterItem);
	void TransformNameColor(CECIvtrItem *pItem, ACString &strName, A3DCOLOR &clrName);
	PAUIDIALOG GetActiveChatDialog();
	bool LinkItem(int nPack, int nSlot);
	void OnLinkItemClicked(CECIvtrItem *pItem, WPARAM wParam, LPARAM lParam);

	ACString GetChatChannelImage(char cChannel);

	void UpdateAutoDelTask(unsigned long taskID, unsigned long remainTime);

	// auto move related methods
	void AutoMoveStart(int x, int y, bool bShowDlg = true);
	A3DPOINT2 AutoMoveTarget();
	void AutoMoveShowDialog(bool isShow);
	void SetAutoMoveShowDialogTarget(int x, int y);

	//
	void ContinueDealMessage() { m_bContinueDealMessage = true; }

	// render the item icon cover
	class ItemDataExtractor { public: virtual CECIvtrItem* GetItemPtr(PAUIOBJECT pObj) = 0; };
	void RenderItemCover(PAUIOBJECT* pObjs, int size, ItemDataExtractor* pExtractor);

	void RenderCover(PAUIOBJECT pObj);
	// 收益时间相关
	void UpdateProfitUI();

	// 玩家确认完成任务相关
	void UpdateTaskConfirm();
	void AddNewTaskConfirm();

protected:	//	Attributes
	typedef struct _SAVE_MARK
	{
		int nNPC;
		A3DVECTOR3 vecPos;
		ACHAR szName[CECGAMEUIMAN_MARK_NAME_LEN + 1];
	} SAVE_MARK, * P_SAVE_MARK;
	typedef struct _USER_LAYOUT_6
	{
		__int8 nVersion;
		__int8 nMapMode;
		bool bQuickbar1Mode;
		bool bChecked1[3];
		bool bQuickbar2Mode;
		bool bChecked2[3];
		bool bQuickbarPetMode;
		int nChatWinSize;
		int nCurChatColor;
		SAVE_MARK a_Mark[CECGAMEUIMAN_MAX_MARKS];
		bool bOnlineNotify;
		bool bAutoReply;
		bool bSaveHistory;
		int idGroup[CECGAMEUIMAN_MAX_GROUPS];
		A3DCOLOR clrGroup[CECGAMEUIMAN_MAX_GROUPS];
	} USER_LAYOUT_6, * P_USER_LAYOUT_6;
	typedef struct _USER_LAYOUT_7
	{
		__int8 nVersion;
		__int8 nMapMode;
		bool bQuickbar1Mode;
		bool bChecked1[5];
		bool bQuickbar2Mode;
		bool bChecked2[3];
		bool bQuickbarPetMode;
		int nChatWinSize;
		int nCurChatColor;
		SAVE_MARK a_Mark[CECGAMEUIMAN_MAX_MARKS];
		bool bOnlineNotify;
		bool bAutoReply;
		bool bSaveHistory;
		int idGroup[CECGAMEUIMAN_MAX_GROUPS];
		A3DCOLOR clrGroup[CECGAMEUIMAN_MAX_GROUPS];
	} USER_LAYOUT_7, * P_USER_LAYOUT_7;
	typedef struct _USER_LAYOUT_8 : public _USER_LAYOUT_7
	{
		bool bTraceAll;
		bool bQuickbarShowAll1;
		bool bQuickbarShowAll2;
		int  nQuickbarCurPanel1;
		int  nQuickbarCurPanel2;
		int  nQuickbarDisplayPanels1;
		int  nQuickbarDisplayPanels2;
		_USER_LAYOUT_8();
	} USER_LAYOUT_8, * P_USER_LAYOUT_8;
	typedef struct _USER_LAYOUT_9 : public _USER_LAYOUT_8
	{
		short a_MarkMapID[CECGAMEUIMAN_MAX_MARKS];
		_USER_LAYOUT_9() { for(int i=0;i<CECGAMEUIMAN_MAX_MARKS;i++) a_MarkMapID[i]=1; }
	} USER_LAYOUT_9, * P_USER_LAYOUT_9;
	typedef struct _USER_LAYOUT_10 : public _USER_LAYOUT_9
	{
		DWORD dwTraceMask;
		_USER_LAYOUT_10() { dwTraceMask = 0; }
	} USER_LAYOUT_10, * P_USER_LAYOUT_10;
	
	typedef struct _USER_LAYOUT_11 : public _USER_LAYOUT_10
	{
		 unsigned char ucCurSystemModuleSC;
		_USER_LAYOUT_11() { ucCurSystemModuleSC = 0; }
	} USER_LAYOUT_11, * P_USER_LAYOUT_11;

	typedef struct _USER_LAYOUT_12 : public _USER_LAYOUT_11
	{
		bool bSystemModuleQuickBarMini;
		_USER_LAYOUT_12() { bSystemModuleQuickBarMini = false;}
	} USER_LAYOUT_12, * P_USER_LAYOUT_12;

	typedef struct _USER_LAYOUT_13 : public _USER_LAYOUT_12
	{
		bool bMenuMode;
		_USER_LAYOUT_13() { bMenuMode = false;}
	} USER_LAYOUT_13, * P_USER_LAYOUT_13;

	typedef struct _USER_LAYOUT_14 : public _USER_LAYOUT_13
	{
		bool bShowCompareDesc;
		bool bShowLowHP;
		_USER_LAYOUT_14() { bShowCompareDesc = true; bShowLowHP = true; }
	} USER_LAYOUT_14, * P_USER_LAYOUT_14;

	typedef struct _USER_LAYOUT : public _USER_LAYOUT_14
	{
		bool bShowTargetOfTarget;
		_USER_LAYOUT() { bShowTargetOfTarget = true; }
	} USER_LAYOUT, * P_USER_LAYOUT;

	// direct convert old version to current version
	template<class LayoutVersionPtr>
	void LoadLayoutSettings(LayoutVersionPtr pData, P_USER_LAYOUT pul)
	{
		LayoutVersionPtr pul_old = (LayoutVersionPtr)pData;
		LayoutVersionPtr temp = (LayoutVersionPtr)pul;
		*temp = *pul_old;
	}

	// special cases
	void LoadVersion6LayoutSettings(void* pData, P_USER_LAYOUT pul);

	typedef struct _TASK_HINT
	{
		DWORD dwTime;
		ACString strHint;
	} TASK_HINT, *P_TASK_HINT;


protected:
	
	int	m_nAccountTimeIndex;
	CECStringTab m_ConsoleCmds;		//	Console command strings

	POINT m_ptLButtonDown;

	A2DSprite *m_pA2DSpriteMask;
	A2DSprite *m_pA2DSpriteIcons[ICONS_MAX];
	abase::vector<A2DSprite*>	m_vecIconList;
	abase::hash_map<int, AString> m_IconSound;
	abase::hash_map<AString, int> m_IconMap[ICONS_MAX];
	abase::hash_map<AString,A2DSprite*> m_GuildIconMapOfSepFile;
	A2DSprite *m_pA2DSpriteItemExpire;
	
	bool m_bContinueDealMessage; // true means the msg will be handled by other module
	
	abase::vector<A2DSprite*>	m_pA2DSpriteImage;

	bool m_bRepairing;
	int m_idCurFinishTask;
	NPC_ESSENCE *m_pCurNPCEssence;

	A3DVECTOR3 m_vecTargetPos;
	CELPrecinct *m_pCurPrecinct;
	A2DSprite *m_pA2DSpriteTarget;
	A2DSprite *m_pA2DSpriteMoveArrow;

	int m_idTrade;
	bool m_bShowAllPanels;
	PAUIDIALOG m_pShowOld;
	TASK_HINT m_ZoneHint;
	abase::vector<int> m_vecBrokenList;
	abase::vector<TASK_HINT> m_vecTaskHint;
	bool m_bAutoReply, m_bOnlineNotify, m_bSaveHistory;
	
	bool m_bUIEnable;		//	UI enable flag

	CECCustomizeMgr *m_CustomizeMgr;
	CECHomeDlgsMgr *m_HomeDlgsMgr;
	CECMiniBarMgr *m_pMiniBarMgr;
	CECMapDlgsMgr *m_pMapDlgsMgr;
	CECShortcutMgr *m_pShortcutMgr;
	CECIconStateMgr *m_pIconStateMgr;

	bool m_bReferralHintShown;
	time_t m_WorldLifeTime;
	time_t m_WorldLifeTimeBase;

	bool m_bItemNotifyShown;
	bool m_bTrashPwdReminded;

	DWORD m_dwLastGTLogin;	//	上次调用 GT 登录时间（用于冷却）

	CECScreenEffectMan* m_pScreenEffectMan;

	abase::vector<A2DSprite*> m_pSpriteIconSysModule;
	
	//	帮派 PVP 相关图标
	enum{
		FACTION_PVP_SMALL_ICON,					//	一级M图用
		FACTION_PVP_SMALL_ICON_FOR_MY_FACTION,	//	“本帮“一级M图用
		FACTION_PVP_NORMAL_ICON,				//	二级M图用
		FACTION_PVP_NORMAL_ICON_FOR_MY_FACTION,	//	“本帮“二级M图用
		FACTION_PVP_MINI_ICON,					//	小地图用
		FACTION_PVP_ICON_NUM,
	};
	A2DSprite*	m_pFactionPVPMineBaseSprite[FACTION_PVP_ICON_NUM];
	A2DSprite*	m_pFactionPVPMineSprite[FACTION_PVP_ICON_NUM];
	A2DSprite*	m_pA2DSpriteFactionPVPHasMine;				//	领土内有矿提示

	CECSSOTicketHandler	*	m_ssoTicketHandler;		//	SSOGetTicket_Re 协议的处理责任链

	bool m_bShowItemDescCompare;
	bool m_bShowLowHP;
	bool m_bShowTargetOfTarget;

protected:	//	Operations
	bool LoadIconSet();
	void InitDialogs();

	void ParseUserConsoleCommand();
	void ParseDebugConsoleCommand();
	void ParseGMConsoleCommand();
	bool CompConsoleCommand(int iCmd);
	bool OnDlgCmd_Console(const ACHAR* szCommand);
	
	void OnConsoleQuery(ACString strName);
	void OnConsoleQueryNPC(int idTarget);
	void OnConsoleQueryModel(AString strName);

	void RefreshItemNotify();
	void RefreshQuickBarName();
	void RefreshHint();
	void RefreshBrokenList();
	bool UpdateHint();
	bool UpdateArrowInfo();
	void RemindTrashboxPassword();

	bool SwitchHotkeyPanel(PAUIDIALOG pPanel, int nDelta);
	void InvokeDragDrop(PAUIDIALOG pDlg, PAUIOBJECT pObj, POINT ptCapture);
	void AddHistory(int idPlayer, DWORD dwTime, const ACHAR *pszPlayer, const ACHAR *pszMsg);

	static CECShortcutSet * GetSCSByDlg(const char *pszDlg);
	virtual PAUIDIALOG CreateDlgInstance(const AString strTemplName);
	bool OnNewMessageBox(int iRetVal);
	bool SSOOpenLink(const AString& strFormat, const AString& strTicket, bool bTicketFirst = true);
	void HideGeneralCardHover();

	void InitSSOTicketHandler();

public:
	void PlayItemSound(CECIvtrItem *pItem, bool bDown);
	void InvokeNumberDialog(void* param1, void* param2, int nMode, int nMax);

	int GetItemCount(int idItem, CECInventory *pPack = NULL);
	ACString GetItemName(int itemid);

	CECMiniBarMgr* GetMiniBarMgr() { return m_pMiniBarMgr;}
	CECMapDlgsMgr* GetMapDlgsMgr() { return m_pMapDlgsMgr;}
	CECShortcutMgr* GetShortcutMgr() { return m_pShortcutMgr;}
	CECIconStateMgr* GetIconStateMgr() { return m_pIconStateMgr;}

	CECScreenEffectMan* GetScreenEffectMan() { return m_pScreenEffectMan; }

	bool OnHotKeyDown(int iUsage, bool bFirstPress);
	
	static ACString AUI_ReplaceEditboxItem(const ACHAR *szText, wchar_t cItem, const ACHAR *szSubText);

	// use virtual to avoid code implemented in header file
	struct AUI_EditboxItemFilter
	{
		virtual bool operator()(EditBoxItemBase *pItem) const = 0;
	};
	struct AUI_EditboxItemMaskFilter : public AUI_EditboxItemFilter
	{
		int	m_nItemMask;
		AUI_EditboxItemMaskFilter(int nItemMask)
			: m_nItemMask(nItemMask)
		{}
		virtual bool operator()(EditBoxItemBase *pItem) const{
			return (m_nItemMask & (1 << pItem->GetType())) != 0;
		}
	};
	static ACString AUI_FilterEditboxItem(const ACHAR *szText, const AUI_EditboxItemFilter& filter);
	
	static ACString AUI_ConvertHintString(const ACHAR *szText);

	ACString GetIconsImageString(int nIconset, const char *szIconFile);
	
	void FriendOnlineNotify(int idFriend, char oldStatus, char newStatus);
	void ProcessOfflineMsgs();
	void RefreshFriendChatPlayerName(int roleid);

	// get current NPC service data
	const NPC_ESSENCE* GetCurNPCEssence() const { return m_pCurNPCEssence; }

	void UpdateSkillRelatedUI();

	virtual void OnSSOGetTicket_Re(const GNET::SSOGetTicket_Re *pProtocol);

	bool NewMessageBox(const char *szContext, const ACHAR *szMsg, DWORD dwType, bool bModal=true);
	CDlgMessageBox * GetNewMessageBox(){ return m_pDlgMessageBox; }

	void DrawSpriteTarget(int nItem, A3DPOINT2 pt);
	void SwitchCountryChannel();
	void SSOGetQuickPayTicket();
	void SSOGetTouchTicket();

	//	GT 相关
	bool SSOGetGTTicket();
	void UpdateGTWindow();
	void UpdateGTTeam();
	void UpdateGTFaction();

	// 任务动作
	void PopTaskEmotionDlg(unsigned int task_id,unsigned int uiEmotion,bool bShow);
	// 战车
	void ChariotWarAction(DWORD cmd, void *pData);

	A2DSprite* GetSysModuleIcon(int n);	
	bool LoadSprite(const char *szPath, A2DSprite *&pSprite);

	ACString GetCashText(int nCash, bool bFullText = false);
	void SetCashText(PAUIOBJECT pObj, int nCash, bool bFullText = false);

	void SetShowAllPanlesFlag(bool bShow) { m_bShowAllPanels = bShow;}
	bool IsShowAllPanels() const { return m_bShowAllPanels;}
	bool CanToggleAllPannelsByInput();

	bool IsShowItemDescCompare();
	void SetShowItemDescCompare(bool bShow);

	bool IsShowLowHP();
	void SetShowLowHP(bool bShow);

	bool IsShowTargetOfTarget();
	void SetShowTargetOfTarget(bool bShow);
};

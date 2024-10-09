 /*
 * FILE: EC_GameUIMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4284)

#include "A3DDevice.h"
#include "A2DSprite.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DEngine.h"
#include "A3DCamera.h"
#include "A3DFlatCollector.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"

#include "AF.h"
#include "AMSoundEngine.h"
#include "AMSoundBufferMan.h"
#include "A3DFont.h"
#include "A3DGDI.h"
#include "A3DTerrainWater.h"
#include "AUICTranslate.h"
#include "CSplit.h"
#include "AUIDef.h"
#include "AUICommon.h"
#include "Task\\TaskTemplMan.h"
#include "Task\\TaskTempl.h"
#include "rpcdata\\roleinfo"
#include "EL_Precinct.h"
#include "gnetdef.h"
#include "globaldataman.h"
#include "elementdataman.h"

#include "EC_Configs.h"
#include "EC_UIConfigs.h"
#include "EC_FullGlowRender.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_InputCtrl.h"
#include "EC_HostInputFilter.h"
#include "EC_ManPlayer.h"
#include "EC_ManNPC.h"
#include "EC_Monster.h"
#include "EC_NPC.h"
#include "EC_HostPlayer.h"
#include "EC_ElsePlayer.h"
#include "EC_Friend.h"
#include "EC_FixedMsg.h"
#include "EC_DealInventory.h"
#include "EC_IvtrTypes.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrFlySword.h"
#include "EC_IvtrStone.h"
#include "EC_NPCServer.h"
#include "EC_Resource.h"
#include "EC_ShadowRender.h"
#include "EC_Shortcut.h"
#include "EC_ShortcutSet.h"
#include "EC_Skill.h"
#include "EC_Team.h"
#include "EC_TaskInterface.h"
#include "EC_Utility.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include "EC_CustomizeMgr.h"
#include "EC_Faction.h"
#include "EC_HomeDlgsMgr.h"
#include "EC_UIManager.h"
#include "EC_GFXCaster.h"
#include "ECScriptMan.h"
#include "ECScriptContext.h"
#include "ECScriptCheckState.h"
#include "auto_delete.h"
#include "EC_GameUIMan2.inl"
#include "PredefinedGMHelpMsg.h"
#include "EC_GameTalk.h"
#include "EC_ShortcutMgr.h"
#include "Network/ssogetticket_re.hpp"
#include "kegetstatus_re.hpp"
#include "Network/EC_GPDataType.h"
#include "EC_DomainGuild.h"
#include "EC_CountryConfig.h"
#include "EC_GPDataType.h"
#include "EC_CountryConfig.h"
#include "GT/gt_overlay.h"
#include "Arc/Asia/EC_ArcAsia.h"
#include "EC_CrossServer.h"
#include "EC_MCDownload.h"
#include "EC_AutoPolicy.h"
#include "EC_IntelligentRoute.h"
#include "EC_QuickBuyPopMan.h"
#include "EC_UseUniversalToken.h"
#include "EC_UniversalTokenVisitHTTPCommand.h"
#include "EC_TimeSafeChecker.h"
#include "EC_ProfConfigs.h"
#include "EC_ScreenEffect.h"
#include "EC_CommandLine.h"
#include "EC_InstanceReenter.h"
#include "EC_BaseColor.h"
#include "EC_IvtrMaterial.h"
#include "EC_Reconnect.h"
#include "EC_CustomizePolicy.h"

/*
#include "AutoScene.h"

#include "DlgAutoGenLightMapProgress.h"

#include "DlgHomeBase.h"
#include "render.h"
*/
#define new A_DEBUG_NEW

#define LAYOUTDATA_VERSION			15
#define CECGAMEUIMAN_MAX_MSGS		200
#define CECGAMEUIMAN_MAX_BROKEN		8
#define CECGAMEUIMAN_MAX_TASKHINT	6

enum
{
	DLG_CONSOLE = 0,		//	Console
	DLG_PLAYERCONTEXT,		//	Player context
	DLG_CHAT,				//	Chat window
	NUM_DIALOG,
};
static const char* l_aDlgNames[NUM_DIALOG] = 
{
	"Dlg_Console",
	"Win_QuickAction",
	"Win_Chat"
};
static bool RemoveFilesAndDirectory(char *pszPath);
static inline int MakeNextCountTime(int nTime);

static const int TWO_WEEKS_SECONDS = 14 * 24 * 3600;

#define CECGAMEUIMAN_ACCOUNTTIME_TIPMAX 28
static const DWORD l_dwAccountTimeTip[CECGAMEUIMAN_ACCOUNTTIME_TIPMAX] = 
{
	1800, 1500, 1200, 900, 600, 540, 480, 420, 360, 300, 240, 
	180, 120, 60, 50, 40, 30, 20, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
};

static GNET::Octets GetAccountTicketContext()
{
	const char *szContext = "ticket_account";
	return GNET::Octets(szContext, strlen(szContext));
}

static GNET::Octets GetQuickPayTicketContext()
{
	const char *szContext = "ticket_quickpay";
	return GNET::Octets(szContext, strlen(szContext));
}

static GNET::Octets GetGTTicketContext()
{
	const char *szContext = "ticket_gt";
	return GNET::Octets(szContext, strlen(szContext));
}

static GNET::Octets GetTouchTicketContext()
{
	const char *szContext = "ticket_touch";
	return GNET::Octets(szContext, strlen(szContext));
}

static AString GetGTUpdater()
{
	AString str = g_szWorkDir;
	if (!str.IsEmpty())
	{
		str += "\\GT_updater.exe";
		if (!af_IsFileExist(str))
			str.Empty();
	}
	return str;
}
static bool s_GTUpdaterRunned = false;

static GNET::Octets GetNewbieGiftTicketContext()
{
    const char *szContext = "ticket_newbiegift";
	return GNET::Octets(szContext, strlen(szContext));
}

CECGameUIMan::CECGameUIMan()
: m_CustomizeMgr(NULL)
{
	int i;

	m_bContinueDealMessage = false;
	m_pA2DSpriteMask = NULL;
	for( i = 0; i < ICONS_MAX; i++ )
		m_pA2DSpriteIcons[i] = NULL;
	m_pA2DSpriteItemExpire = NULL;

	m_ptLButtonDown.x = m_ptLButtonDown.y = -1;
	
	m_bRepairing = false;
	m_pCurPrecinct = NULL;
	m_idCurFinishTask = -1;
	m_pCurNPCEssence = NULL;

	m_vecTargetPos = A3DVECTOR3(0.0f);
	m_pA2DSpriteTarget = NULL;
	m_pA2DSpriteMoveArrow = NULL;

	m_idTrade = -1;
	m_pShowOld = NULL;
	m_bShowAllPanels = true;
	m_bAutoReply = false;
	m_bOnlineNotify = false;
	m_bSaveHistory = false;
	m_bUIEnable = false;
	m_HomeDlgsMgr = NULL;
	m_pMiniBarMgr = NULL;
	m_pMapDlgsMgr = NULL;
	m_pIconStateMgr = NULL;
	m_nAccountTimeIndex = 0;
	m_bReferralHintShown = false;
	m_bShowItemDescCompare = true;

	m_pScreenEffectMan = NULL;

	m_bItemNotifyShown = false;
	m_bTrashPwdReminded = false;
	m_dwLastGTLogin = 0;
	::memset(m_pFactionPVPMineBaseSprite, 0, sizeof(m_pFactionPVPMineBaseSprite));
	::memset(m_pFactionPVPMineSprite, 0, sizeof(m_pFactionPVPMineSprite));
	m_pA2DSpriteFactionPVPHasMine = NULL;
	m_ssoTicketHandler = NULL;

	PopupWorldCountDown(-1);

	CECGameTalk::Instance().Clear();
}

CECGameUIMan::~CECGameUIMan()
{
	CECGameTalk::Instance().Clear();
}

//	Initialize manager
bool CECGameUIMan::Init(A3DEngine* pA3DEngine, A3DDevice* pA3DDevice, const char* szDCFile/* NULL */)
{
	int i, j;
	bool bval;
	char szName[40];
	PAUISTILLIMAGEBUTTON pButton;

	// force to load the default setting
	ImportStringTable("ingame.stf");
	ImportUIParam("ingame.xml");

	m_pShortcutMgr = new CECShortcutMgr();

	// init manager
	if (!CECBaseUIMan::Init(pA3DEngine, pA3DDevice, szDCFile))
	{
		a_LogOutput(1, "CECGameUIMan::Init, Failed to call AUIManager::Init.");
		return false;
	}
	
	InitDialogs();

	for (i = 0; i < CECGAMEUIMAN_MAX_QSHOPITEMS; i++)
	{
		AString strDlgName;
		strDlgName.Format("Win_QShopItemDefault%d", i);
		m_pDlgQShopDefault[i] = static_cast<CDlgQShopItem *>(GetDialog(strDlgName));
	}
	for (i = 0; i < FASHION_SHOP_ITEM_COUNT; i++)
	{
		AString strDlgName;
		strDlgName.Format("Win_FashionShopItem%d", i);
		m_pDlgFashionShopItem[i] = static_cast<CDlgFashionShopItem *>(GetDialog(strDlgName));
	}
	
	m_CustomizeMgr = new CECCustomizeMgr;

	m_pDlgSettingCurrent = m_pDlgSettingSystem;
	m_pDlgOptimizeCurrent = GetDialog("Win_OptimizeMem");

	m_pMiniBarMgr = new CECMiniBarMgr();
	m_pMiniBarMgr->Init(this);

	m_pDlgDoubleExpS->AlignTo(m_pDlgMiniMap,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP);
	m_pDlgDoubleExpS->SetCanMove(false);
		
	//	Load configs strings
	if (af_IsFileExist("Configs\\console_cmd.txt"))
	{
		if (!m_ConsoleCmds.Init("Configs\\console_cmd.txt", false))
			glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGameUIMan::Init", __LINE__);
	}

	m_pDlgDragDrop->SetCanOutOfWindow(true);
	m_pDlgWorldMapDetail->SetCanOutOfWindow(true);

	GNET::RoleInfo Info = g_pGame->GetGameRun()->GetSelectedRoleInfo();
	static const char *a_szDlgInventoryName[] = { "Win_InventoryM", "Win_InventoryF" };
	static const char *a_szDlgFittingRoomName[] = { "Win_FittingRoomM", "Win_FittingRoomF" };
	PAUIDIALOG pDlg = GetDialog(a_szDlgFittingRoomName[Info.gender]);
	pDlg->SetName("Win_FittingRoom");
	m_pDlgFittingRoom = (CDlgFittingRoom *)GetDialog("Win_FittingRoom");
	pDlg = GetDialog(a_szDlgInventoryName[Info.gender]);
	pDlg->SetName("Win_Inventory");
	pButton = (PAUISTILLIMAGEBUTTON)pDlg->GetDlgItem("Btn_NormalItem");
	pButton->SetPushed(true);
	pButton->SetColor(A3DCOLORRGB(255, 203, 74));
	pButton = (PAUISTILLIMAGEBUTTON)pDlg->GetDlgItem("Btn_NormalMode");
	pButton->SetPushed(true);
	m_pDlgInventory = (CDlgInventory *)GetDialog("Win_Inventory");
	m_pDlgInventory->OnCommand_normalmode("normalmode");

	// HACK: duplicate this pointer for theme support
	this->m_DlgName[a_szDlgInventoryName[Info.gender]] = m_pDlgInventory;
	this->m_DlgName[a_szDlgFittingRoomName[Info.gender]] = m_pDlgFittingRoom;

	pDlg = m_pDlgInputNO;
	PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(pDlg->GetDlgItem("DEFAULT_Txt_No."));
	pEdit->SetIsNumberOnly(true);

	pDlg = m_pDlgShop;
	for( i = 0; ; i++ )
	{
		sprintf(szName, "Btn_Set%d", i + 1);
		pButton = (PAUISTILLIMAGEBUTTON)pDlg->GetDlgItem(szName);
		if( !pButton ) break;
		pButton->SetPushLike(true);
	}

	pDlg = m_pDlgProduce;
	for( i = 0; ; i++ )
	{
		sprintf(szName, "Btn_Set%d", i + 1);
		pButton = (PAUISTILLIMAGEBUTTON)pDlg->GetDlgItem(szName);
		if( !pButton ) break;
		pButton->SetPushLike(true);
	}

	m_pMapDlgsMgr = new CECMapDlgsMgr();
	if(!m_pMapDlgsMgr->Init())
	{
		a_LogOutput(1, "CECGameUIMan::Init, Failed to call CECMapDlgsMgr::Init.");
	}

	m_pIconStateMgr = new CECIconStateMgr();
	if(!m_pIconStateMgr)
	{
		a_LogOutput(1, "CECGameUIMan::Init, Failed to create CECIconStateMgr.");
	}

	pDlg = m_pDlgNPC;
	((PAUILISTBOX)pDlg->GetDlgItem("Lst_Main"))->SetAutoWrap(true);

	PAUIDIALOG a_pDlgName[] =
	{
		m_pDlgSettingSystem, m_pDlgSettingVideo,
		m_pDlgSettingGame, //m_pDlgSettingBlackList
	};
	char *a_pszBtnName[] = { "Btn_System", "Btn_Video", "Btn_Game", "Btn_Blacklist" };
	for( i = 0; i < sizeof(a_pDlgName) / sizeof(PAUIDIALOG); i++ )
	{
		pDlg = a_pDlgName[i];
		for( j = 0; j < sizeof(a_pszBtnName) / sizeof(char *); j++ )
		{
			pButton = (PAUISTILLIMAGEBUTTON)pDlg->GetDlgItem(a_pszBtnName[j]);
			pButton->SetPushLike(true);
		}
	}
//	m_pDlgSettingSystem->GetDlgItem("Chk_Advwater")->Show(false);
//	m_pDlgSettingSystem->GetDlgItem("lb_advwater")->Show(false);
	
	AString dlgName;
	for (i = 0; i < NUM_HOSTSCSETS1; ++ i)
	{
		dlgName.Format("Win_Quickbar%dHb_%d", SIZE_HOSTSCSET1, i+1);
		((PAUICHECKBOX)GetDialog(dlgName)->GetDlgItem("Chk_Normal"))->Check(true);

		dlgName.Format("Win_Quickbar%dVb_%d", SIZE_HOSTSCSET1, i+1);
		((PAUICHECKBOX)GetDialog(dlgName)->GetDlgItem("Chk_Normal"))->Check(true);
	}
	dlgName.Format("Win_Quickbar%dHa", SIZE_HOSTSCSET1);
	GetDialog(dlgName)->AlignTo(GetDialog("Win_Main"),
		AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
		AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
	dlgName.Format("Win_Quickbar%dHa", SIZE_HOSTSCSET2);
	GetDialog(dlgName)->AlignTo(GetDialog("Win_Main"),
		AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
		AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);

	#define AUI_TRYCALL(getter, method) { PAUIOBJECT pObj = getter; if(pObj) pObj->method; }
	AUI_TRYCALL( GetDialog("Win_Hpmpxp")->GetDlgItem("Back1"), SetTransparent(true) );
	AUI_TRYCALL( GetDialog("Win_Hpmpxp")->GetDlgItem("Back2"), SetTransparent(true) );
	AUI_TRYCALL( GetDialog("Win_Hpmpxp")->GetDlgItem("Img_BGclose"), SetTransparent(true) );
	AUI_TRYCALL( GetDialog("Win_HpOther")->GetDlgItem("Back"), SetTransparent(true) );
	AUI_TRYCALL( GetDialog("Win_HpmpOther")->GetDlgItem("Back"), SetTransparent(true) );
	AUI_TRYCALL( GetDialog("Win_HpmpOtherB")->GetDlgItem("Back"), SetTransparent(true) );

	GetDialog("Win_Hpmpxp")->GetDlgItem("Size")->SetAcceptMouseMessage(true);
	GetDialog("Win_HpmpOther")->GetDlgItem("Size")->SetAcceptMouseMessage(true);
	GetDialog("Win_HpmpOtherB")->GetDlgItem("Size")->SetAcceptMouseMessage(true);

	pDlg = GetDialog("Win_Broken");
	pDlg->SetPosEx(0, GetDialog("Win_Map")->GetRect().bottom, AUIDialog::alignMax);

	GetDialog("Win_Hint")->GetDlgItem("Txt_Hint")->SetTransparent(true);
    GetDialog("Win_Popmsg2")->GetDlgItem("Txt_Message")->SetTransparent(true);

	bval = LoadIconSet();
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

	PAUITEXTAREA pText;
	PAUIEDITBOX pEmotionEdit;
	char *a_pszBack[] = { "Img_Back1", "Img_Back2",
		"Img_Back3", "Img_Back4", "Img_Back5" };

	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgChat2->GetDlgItem("Txt_Content"));
	pText->SetMaxLines(CECGAMEUIMAN_MAX_MSGS);
	pText->SetLeftScrollBar(true);
	pText->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);
	for( i = 0; i < sizeof(a_pszBack) / sizeof(char *); i++ )
		m_pDlgChat3->GetDlgItem(a_pszBack[i])->Show(false);
	
	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgChat3->GetDlgItem("Txt_Content"));
	pText->SetMaxLines(1);
	pText->SetLeftScrollBar(false);
	pText->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);
	for( i = 0; i < sizeof(a_pszBack) / sizeof(char *); i++ )
		m_pDlgChat2->GetDlgItem(a_pszBack[i])->Show(false);

	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgChat->GetDlgItem("Txt_Content"));
	pText->SetMaxLines(CECGAMEUIMAN_MAX_MSGS);
	pText->SetLeftScrollBar(true);
	pText->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);
	pText->SetImageList(&m_pA2DSpriteImage);
	pEmotionEdit = dynamic_cast<PAUIEDITBOX>(m_pDlgChat->GetDlgItem("DEFAULT_Txt_Speech"));
	pEmotionEdit->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);
	for( i = 0; i < sizeof(a_pszBack) / sizeof(char *); i++ )
		m_pDlgChat->GetDlgItem(a_pszBack[i])->Show(false);
	m_pDlgChat->OnCommand_set("set1");
	m_pDlgChat->ChangeFocus(NULL);

	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgChannelChat->GetDlgItem("Txt_Chat"));
	pText->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);
	pEmotionEdit = dynamic_cast<PAUIEDITBOX>(m_pDlgChannelChat->GetDlgItem("Txt_Content"));
	pEmotionEdit->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);

	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgChatWhisper->GetDlgItem("Txt_Chat"));
	pText->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);
	pEmotionEdit = dynamic_cast<PAUIEDITBOX>(m_pDlgChatWhisper->GetDlgItem("DEFAULT_Txt_Speech"));
	pEmotionEdit->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);

	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgFriendHistory->GetDlgItem("Txt_Content"));
	pText->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);

	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgBooth1->GetDlgItem("Txt_MsgList"));
	pText->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);

	for( i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
	{
		sprintf(szName, "Win_FriendChat%d", i);
		PAUIDIALOG pDlgFriend = GetDialog(szName);
		pText = dynamic_cast<PAUITEXTAREA>(pDlgFriend->GetDlgItem("Txt_Chat"));
		pText->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);
		pEmotionEdit = dynamic_cast<PAUIEDITBOX>(pDlgFriend->GetDlgItem("Txt_Content"));
		pEmotionEdit->SetEmotionList(m_pA2DSpriteEmotion, m_vecEmotion);
	}

	m_pDlgChat->RebuildSpeakway();

	pDlg = GetDialog("Dlg_Console");
	((PAUICONSOLE)pDlg->GetDlgItem("Command_Edit"))->
		SetEditBox(pDlg->GetDlgItem("DEFAULT_Command"));
	((PAUICONSOLE)pDlg->GetDlgItem("Command_Edit"))->FitScreen();

	for( i = 1; i < CDlgInfo::INFO_NUM; i++ )
	{
		sprintf(szName, "F_%d", i);
		m_pDlgInfoIcon->GetDlgItem(szName)->Show(false);
		sprintf(szName, "Back_%d", i);
		m_pDlgInfoIcon->GetDlgItem(szName)->Show(false);
	}

	m_pA2DSpriteTarget = new A2DSprite;
	if( !m_pA2DSpriteTarget ) return AUI_ReportError(__LINE__, __FILE__);

	bval = m_pA2DSpriteTarget->Init(m_pA3DDevice, "InGame\\ArrowSet.dds", 0);
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	m_pA2DSpriteTarget->SetLinearFilter(true);

	int W = 64, H = 64;
	A3DRECT a_rc[CECGAMEUIMAN_MAX_MARKS + 4];
	for( i = 0; i < CECGAMEUIMAN_MAX_MARKS + 4; i++ )
		a_rc[i].SetRect(W * i, 0, W * (i + 1), H);
	m_pA2DSpriteTarget->ResetItems(CECGAMEUIMAN_MAX_MARKS + 4, a_rc);

	m_pA2DSpriteMoveArrow = new A2DSprite;
	if( !m_pA2DSpriteMoveArrow ) return AUI_ReportError(__LINE__, __FILE__);

	bval = m_pA2DSpriteMoveArrow->Init(m_pA3DDevice, "InGame\\MoveArrow.tga", 0);
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	m_pA2DSpriteMoveArrow->SetLinearFilter(true);
	
	A3DVIEWPORTPARAM *param = g_pGame->GetViewport()->GetA3DViewport()->GetParam();
	m_pDlgBBS->ChangeSize(param->Width,param->Height);

	m_pDlgWebTradeCurrent = NULL;

	m_webBrowserDialogs.clear();
	const char *szWebBrowserDialogs[] = {"Win_Explorer"};
	for (i = 0; i < sizeof(szWebBrowserDialogs)/sizeof(szWebBrowserDialogs[0]); ++ i)
	{
		PAUIDIALOG pDlg = GetDialog(szWebBrowserDialogs[i]);
		if (!pDlg)	continue;
		CDlgExplorer *pDlgExplorer = dynamic_cast<CDlgExplorer *>(pDlg);
		if (!pDlgExplorer)	continue;
		m_webBrowserDialogs.push_back(pDlgExplorer);
	}

	m_pDlgMiniMap->CheckMailToFriendsButton();

	CECCountryConfig::Instance().Init();

	if (!g_pGame->GetConfigs()->GetEnableGT() ||
		(CECCrossServer::Instance().IsOnSpecialServer() || CECCrossServer::Instance().IsToSpecial()) &&
		!CECUIConfig::Instance().GetGameUI().bEnableGTOnSpecialServer)
	{
		PAUIOBJECT pBtn_GT = m_pDlgHost->GetDlgItem("Btn_GT");
		if (pBtn_GT) pBtn_GT->Show(false);
	}

	//自建家园
//	m_HomeDlgsMgr = new CECHomeDlgsMgr;
	
	if (CDlgQuickBuyPop *pDlg = dynamic_cast<CDlgQuickBuyPop *>(GetDialog("Win_QuickBuyPop1"))){
		CECQuickBuyPopManager::Instance().AddPop(pDlg);
	}
	if (CDlgQuickBuyPop *pDlg = dynamic_cast<CDlgQuickBuyPop *>(GetDialog("Win_QuickBuyPop2"))){
		CECQuickBuyPopManager::Instance().AddPop(pDlg);
	}

	InitSSOTicketHandler();

	CECFactionPVPModel::Instance().RegisterObserver(m_pDlgFactionPVPStatus);
	CECFactionPVPModel::Instance().RegisterObserver(m_pDlgFactionPVPRank);

	m_pScreenEffectMan = new CECScreenEffectMan();

	return true;
}

bool CECGameUIMan::LoadIconSet()
{
	bool bval;
	DWORD dwRead;
	AFileImage fi;
	int h, i, j, nIndex;
	A3DRECT *a_rc[ICONS_MAX];
	char szFile[MAX_PATH], szLine[AFILE_LINEMAXLEN];
	int W = 32, H = 32, a_nCountX[ICONS_MAX], a_nCountY[ICONS_MAX];
	GNET::RoleInfo Info = g_pGame->GetGameRun()->GetSelectedRoleInfo();
	char a_szIconFile[ICONS_MAX][40] = { "Action", "Skill", "Ivtr", "State", "SkillGrp", "Guild", "Pet", "ELF", "Suite", "Calendar", "PQ" };

	if( 0 == Info.gender )
		strcat(a_szIconFile[ICONS_INVENTORY], "M");
	else
		strcat(a_szIconFile[ICONS_INVENTORY], "F");

	for( h = 0; h < ICONS_MAX; h++ )
	{
		sprintf(szFile, "%s\\Surfaces\\IconSet\\IconList_%s.txt",
			af_GetBaseDir(), a_szIconFile[h]);
		bval = fi.Open(szFile, AFILE_OPENEXIST | AFILE_TEXT | AFILE_TEMPMEMORY);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

		fi.ReadLine(szLine, sizeof(szLine), &dwRead);
		W = atoi(szLine);

		fi.ReadLine(szLine, sizeof(szLine), &dwRead);
		H = atoi(szLine);

		fi.ReadLine(szLine, sizeof(szLine), &dwRead);
		a_nCountY[h] = atoi(szLine);

		fi.ReadLine(szLine, sizeof(szLine), &dwRead);
		a_nCountX[h] = atoi(szLine);

		a_rc[h] = (A3DRECT*)a_malloctemp(sizeof(A3DRECT)*(a_nCountX[h] * a_nCountY[h]));
		if( !a_rc[h] ) return AUI_ReportError(__LINE__, __FILE__);

		for( i = 0; i < a_nCountY[h]; i++ )
		{
			for( j = 0; j < a_nCountX[h]; j++ )
			{
				nIndex = i * a_nCountX[h] + j;
				a_rc[h][nIndex].SetRect(j * W, i * H, j * W + W, i * H + H);

				bval = fi.ReadLine(szLine, sizeof(szLine), &dwRead);
				if( dwRead > 0 && strlen(szLine) > 0 )
					m_IconMap[h][AString(szLine)] = nIndex;
			}
		}

		fi.Close();

		m_pA2DSpriteIcons[h] = new A2DSprite;
		if( !m_pA2DSpriteIcons[h] )
		{
			a_freetemp(a_rc[h]);
			return AUI_ReportError(__LINE__, __FILE__);
		}

		sprintf(szFile, "IconSet\\IconList_%s.dds", a_szIconFile[h]);
		bval = m_pA2DSpriteIcons[h]->Init(m_pA3DDevice, szFile, AUI_COLORKEY);
		if( !bval )
		{
			a_freetemp(a_rc[h]);
			return AUI_ReportError(__LINE__, __FILE__);
		}

		m_vecIconList.push_back( m_pA2DSpriteIcons[h] );	// add for imaged hints
	}
	
	SetImageList(&m_vecIconList);	// add for imaged hints

	for( h = 0; h < ICONS_MAX; h++ )
	{
		bval = m_pA2DSpriteIcons[h]->ResetItems(
			a_nCountX[h] * a_nCountY[h], a_rc[h]);
		a_freetemp(a_rc[h]);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);		
	}

	m_pA2DSpriteMask = new A2DSprite;
	if( !m_pA2DSpriteMask ) return AUI_ReportError(__LINE__, __FILE__);

	bval = m_pA2DSpriteMask->Init(m_pA3DDevice,
		"InGame\\IconHighlight.dds", AUI_COLORKEY);
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

	// load the expire icon cover
	m_pA2DSpriteItemExpire = new A2DSprite;
	if( !m_pA2DSpriteItemExpire ) return AUI_ReportError(__LINE__, __FILE__);
	bval = m_pA2DSpriteItemExpire->Init(m_pA3DDevice, "InGame\\IconItemExpire.dds", AUI_COLORKEY);
	if( !bval )
	{ 
		delete m_pA2DSpriteItemExpire;
		m_pA2DSpriteItemExpire = NULL;
		AUI_ReportError(__LINE__, "CECGameUIMan::LoadIconSet(), failed to load InGame\\IconItemExpire.dds");
	}

	//	加载帮派PVP相关图标
	const char *szFactionPVPMineBase[FACTION_PVP_ICON_NUM] = {
		"InGame\\存矿点_小.tga",
		"InGame\\存矿点_小_本帮.tga",
		"InGame\\存矿点.tga",
		"InGame\\存矿点_本帮.tga",
		"InGame\\存矿点_雷达.tga",
	};
	for (i = 0; i < FACTION_PVP_ICON_NUM; ++ i){
		if (!LoadSprite(szFactionPVPMineBase[i], m_pFactionPVPMineBaseSprite[i])){
			AUI_ReportError(__LINE__, 1, "CECGameUIMan::LoadIconSet(), failed to load %s", szFactionPVPMineBase[i]);
		}
	}
	const char *szFactionPVPMine[FACTION_PVP_ICON_NUM] = {
		"InGame\\产矿点_小.tga",
		"InGame\\产矿点_小_本帮.tga",
		"InGame\\产矿点.tga",
		"InGame\\产矿点_本帮.tga",
		"InGame\\产矿点_雷达.tga",
	};
	for (i = 0; i < FACTION_PVP_ICON_NUM; ++ i){
		if (!LoadSprite(szFactionPVPMine[i], m_pFactionPVPMineSprite[i])){
			AUI_ReportError(__LINE__, 1, "CECGameUIMan::LoadIconSet(), failed to load %s", szFactionPVPMine[i]);
		}
	}
	const char *szFactionPVPHasMine = "InGame\\有矿提示.tga";
	if (!LoadSprite(szFactionPVPHasMine, m_pA2DSpriteFactionPVPHasMine)){
		AUI_ReportError(__LINE__, 1, "CECGameUIMan::LoadIconSet(), failed to load %s", szFactionPVPHasMine);
	}

	// Emotions.父类已加载

	// Images
	char a_szImageFile[GP_CHAT_MAX][40] = { "普通.tga", "世界.tga", "组队.tga", "帮派.tga", "密语.tga", "", "", "交易.tga", "", "系统.tga", "", "", "号角.tga", "战场.tga", "国家.tga"};
	for (i = 0; i < GP_CHAT_MAX; ++ i)
	{
		AString strFile = AString("InGame\\") + a_szImageFile[i];
		AString strFullPath = AString("Surfaces\\") + strFile;
		if (af_IsFileExist(strFullPath))
		{
			A2DSprite *pSprite = new A2DSprite;
			if (!pSprite) return AUI_ReportError(__LINE__, __FILE__);
			if (pSprite->Init(m_pA3DDevice, strFile, AUI_COLORKEY))
			{
				m_pA2DSpriteImage.push_back(pSprite);
				continue;
			}
			else
			{
				A3DRELEASE(pSprite);
			}
		}

		// 插入占位符
		m_pA2DSpriteImage.push_back(NULL);
	}
	
	PAUIDIALOG pShow = GetDialog("Win_Popface");
	pShow->SetData(AUIMANAGER_MAX_EMOTIONGROUPS);
	pShow = GetDialog("Win_Popface01");
	pShow->SetData(AUIMANAGER_MAX_EMOTIONGROUPS);
	pShow = GetDialog("Win_Popface02");
	pShow->SetData(AUIMANAGER_MAX_EMOTIONGROUPS);

	AScriptFile sf;
	if( sf.Open("Configs\\Iconsound.txt") )
	{
		int idSubType;
		AString strWave;

		while( sf.GetNextToken(true) )
		{
			idSubType = atoi(sf.m_szToken);
			sf.GetNextToken(true);
			strWave = sf.m_szToken;
			m_IconSound[idSubType] = strWave;
		}

		sf.Close();
	}

	for (i=0;i<CECSCSysModule::FM_NUM;i++)
	{
		const char* pIconFile = res_SysModuleIconFile(i);
		A2DSprite *pSprite = new A2DSprite;
		if (!pSprite) return AUI_ReportError(__LINE__, __FILE__);
		if (!pSprite->Init(m_pA3DDevice, pIconFile, AUI_COLORKEY))
		{
			A3DRELEASE(pSprite);
			a_LogOutput(1, "CECGameUIMan::LoadIconSet , %s",pIconFile);
			continue;
		}
		pSprite->SetLinearFilter(true);

		m_pSpriteIconSysModule.push_back(pSprite);
	}
	
	return true;
}

//	Release manager
bool CECGameUIMan::Release(void)
{
	int i;
	
	CECFactionPVPModel::Instance().UnregisterObserver(m_pDlgFactionPVPStatus);
	CECFactionPVPModel::Instance().UnregisterObserver(m_pDlgFactionPVPRank);

	CECQuickBuyPopManager::Instance().ClearPops();

	CECCountryConfig::Instance().Destroy();

	g_pGame->GetConfigs()->SaveSystemSettings();

	m_ConsoleCmds.Release();

	m_vecIconList.clear();
	for (i = 0; i < (int)m_pA2DSpriteImage.size(); ++ i)
	{
		A3DRELEASE(m_pA2DSpriteImage[i]);
	}
	m_pA2DSpriteImage.clear();

	A3DRELEASE(m_pA2DSpriteTarget);
	A3DRELEASE(m_pA2DSpriteMoveArrow);
	A3DRELEASE(m_pA2DSpriteMask);
	for( i = 0; i < ICONS_MAX; i++ )
	{
		m_IconMap[i].clear();
		A3DRELEASE(m_pA2DSpriteIcons[i]);
	}
	A3DRELEASE(m_pA2DSpriteItemExpire);

	m_IconSound.clear();
	for(abase::hash_map<AString,A2DSprite*>::iterator it = m_GuildIconMapOfSepFile.begin();it!=m_GuildIconMapOfSepFile.end();++it)
	{
		A3DRELEASE(it->second);
		it->second = NULL;
	}
	m_GuildIconMapOfSepFile.clear();
	A3DRELEASE(m_CustomizeMgr);
	delete m_HomeDlgsMgr;
	delete m_pMiniBarMgr;
	delete m_pMapDlgsMgr;
	delete m_pShortcutMgr;
	delete m_pIconStateMgr;

	delete m_pScreenEffectMan;

	for(i=0; i< (int)m_pSpriteIconSysModule.size();i++)
	{
		A3DRELEASE(m_pSpriteIconSysModule[i]);
	}
	m_pSpriteIconSysModule.clear();
	
	for (i = 0; i < FACTION_PVP_ICON_NUM; ++ i){
		A3DRELEASE(m_pFactionPVPMineBaseSprite[i]);
		A3DRELEASE(m_pFactionPVPMineSprite[i]);
	}
	A3DRELEASE(m_pA2DSpriteFactionPVPHasMine);

	return CECBaseUIMan::Release();
}

bool CECGameUIMan::DealMessageBoxQuickKey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 处理消息框的 Y/N 快捷键
	//
	bool bRet(false);

	if (m_pDlgActive && m_pDlgActive->GetType() == AUIDIALOG_TYPE_MESSAGEBOX && uMsg == WM_CHAR){
		// 是消息框的按键消息
		PAUIDIALOG pMessageBox = m_pDlgActive;
		if (wParam == 'Y' || wParam == 'y'){
			// 查找是否有 IDOK/IDYES
			PAUIOBJECTLISTELEMENT pElement = pMessageBox->GetFirstControl();
			while( pElement ){
				if (pElement->pThis->IsShow()){
					const char *szCommand = pElement->pThis->GetCommand();
					if (szCommand){
						if (!stricmp(szCommand, "IDOK")){
							if (pMessageBox->OnCommand("IDOK")){
								bRet = true;
							}
							break;
						}if (!stricmp(szCommand, "IDYES")){
							if (pMessageBox->OnCommand("IDYES")){
								bRet = true;
							}
							break;
						}
					}
				}
				pElement = pMessageBox->GetNextControl(pElement);
			}
		}else if (wParam == 'N' || wParam == 'n'){
			// 查找是否有 IDCANCEL/IDNO
			PAUIOBJECTLISTELEMENT pElement = pMessageBox->GetFirstControl();
			while( pElement ){
				if (pElement->pThis->IsShow()){
					const char *szCommand = pElement->pThis->GetCommand();
					if (szCommand){
						if (!stricmp(szCommand, "IDNO")){
							if (pMessageBox->OnCommand("IDNO")){
								bRet = true;
							}
							break;
						}if (!stricmp(szCommand, "IDCANCEL")){
							if (pMessageBox->OnCommand("IDCANCEL")){
								bRet = true;
							}
							break;
						}
					}
				}
				pElement = pMessageBox->GetNextControl(pElement);
			}
		}
	}
	else if (m_pDlgActive == m_pDlgBuyConfirm)
	{
		if (wParam == 'Y' || wParam == 'y')
		{
			m_pDlgBuyConfirm->OnCommandConfirm("IDOK");
			bRet = true;
		}
		else if (wParam == 'N' || wParam == 'n')
		{
			m_pDlgBuyConfirm->OnCommandCancel("IDCANCEL");
			bRet = true;
		}
	}
	return bRet;
}

//	Handle windows message
bool CECGameUIMan::DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( ( uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP
		|| (uMsg>= WM_MOUSEMOVE && uMsg<=WM_MBUTTONDBLCLK) ) && !IsCustomizeCharacter() )
	{
		if(!m_pDlgAutoHelp->IsShow())
			m_pDlgAutoHelp->ResetOpenTime();
		if (uMsg == WM_RBUTTONUP && CDlgAutoHelp::IsAutoHelp())
		{
			m_pDlgAutoHelp->SetAutoHelpState(false);
			return true;
		}
		if (uMsg==WM_MOUSEMOVE)
		{
			m_pDlgAutoHelp->HideCloseButton();
			HideGeneralCardHover();
		}
	}

	// Check for key message for Hot key customize
	// Message is then ignored here and passed to customize interface
	if ( (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP ||
		uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP) && !IsCustomizeCharacter() )
	{
		// For key message only
		if (g_pGame->GetGameRun()->GetHostInputFilter()->IsInCustomize())
		{
			// Is now in customize

			if (m_pDlgActive && m_pDlgActive == m_pDlgSettingQuickKey)
			{
				// Active UI is customize quick key

				// Return false to pass message to HostInputFilter
				return false;
			}
		}
	}

	// 处理消息框
	if (DealMessageBoxQuickKey(uMsg, wParam, lParam))
	{
		return true;
	}

	if (!m_bUIEnable)
	{
		PAUIDIALOG pDlg = NULL;
		if (pDlg = GetDialog("MsgBox_LinkBroken")){
			pDlg->DealWindowsMessage(uMsg, wParam, lParam, false);
		}else if (pDlg = GetDialog("MsgBox_RemindReconnect")){
			pDlg->DealWindowsMessage(uMsg, wParam, lParam, false);
		}
		return false;
	}

	bool bRet = false;

	if( m_pDlgMouseOn && !m_pMapDlgsMgr->IsMarking() &&
		!m_pMapDlgsMgr->IsDraging() && !m_bRepairing &&
		(m_pDlgMouseOn->GetAlpha() > 0 || m_pObjMouseOn) &&
		0 != stricmp(m_pDlgMouseOn->GetName(), "Win_Chat") && !CDlgAutoHelp::IsAutoHelp()
		&& !m_pDlgGeneralCard->IsSwallowing() && !IsCustomizeCharacter() )
	{
		g_pGame->ChangeCursor(RES_CUR_NORMAL);
	}

	if( m_bShowAllPanels )
	{
		m_bContinueDealMessage = false;

		if( uMsg == WM_INPUTLANGCHANGEREQUEST &&
			ImmIsIME(reinterpret_cast<HKL>(lParam)) &&
			(!m_pDlgActive || !m_pDlgActive->GetFocus() ||
			m_pDlgActive->GetFocus()->GetType() != AUIOBJECT_TYPE_EDITBOX) )
		{
			return true;
		}

		PAUIDIALOG pLastActive = m_pDlgActive;
		bool bFocusComboBox = false;
		if( m_pDlgActive && m_pDlgActive->GetFocus() && 
			((m_pDlgActive->GetFocus()->GetType() == AUIOBJECT_TYPE_COMBOBOX &&
			((PAUICOMBOBOX)m_pDlgActive->GetFocus())->GetState() == AUICOMBOBOX_STATE_POPUP) ||
			(m_pDlgActive->GetFocus()->GetType() == AUIOBJECT_TYPE_LISTBOX &&
			((PAUILISTBOX)m_pDlgActive->GetFocus())->GetComboBox() != NULL)) )
			bFocusComboBox = true;

		bRet = CECBaseUIMan::DealWindowsMessage(uMsg, wParam, lParam);
	
		if (bFocusComboBox && pLastActive == m_pDlgActive && bRet == 1 && uMsg == WM_RBUTTONUP){
			//	处理 COMBOBOX 未处理右键但仍返回已处理的情况
			m_pDlgActive->ChangeFocus(NULL);
			bRet = 0;
		}

		if( bRet && m_bContinueDealMessage )
			bRet = false;
		else if( m_pDlgMouseOn )
		{
			if( WM_RBUTTONUP == uMsg )
			{
				CECHostInputFilter::PRESS press = g_pGame->GetGameRun()->GetHostInputFilter()->GetRBtnPressInfo();
				if( press.bTurnCamera ) bRet = false;
			}
			else if( WM_LBUTTONDBLCLK == uMsg )
				bRet = true;
		}

		if( GetDialog("Win_Popface")->IsShow() &&
			WM_LBUTTONUP == uMsg && (!m_pDlgMouseOn || (m_pDlgMouseOn
			&& !strstr(m_pDlgMouseOn->GetName(), "Win_FriendChat")
			&& 0 != stricmp(m_pDlgMouseOn->GetName(), "Win_Chat")
			&& 0 != stricmp(m_pDlgMouseOn->GetName(), "Win_Popface")
			&& 0 != stricmp(m_pDlgMouseOn->GetName(), "Win_WhisperChat") 
			&& 0 != stricmp(m_pDlgMouseOn->GetName(), "Win_ChannelChat"))) )
		{
			GetDialog("Win_Popface")->Show(false);
		}

		if ((GetDialog("Win_Popface01")->IsShow() ||
			GetDialog("Win_Popface02")->IsShow()) &&
			WM_LBUTTONUP == uMsg && (!m_pDlgMouseOn || (m_pDlgMouseOn
			&& 0 != stricmp(m_pDlgMouseOn->GetName(), "Win_Chat")
			&& 0 != stricmp(m_pDlgMouseOn->GetName(), "Win_Popface01")
			&& 0 != stricmp(m_pDlgMouseOn->GetName(), "Win_Popface02"))))
		{
			GetDialog("Win_Popface01")->Show(false);
			GetDialog("Win_Popface02")->Show(false);
		}

		// Return all key message as processed when the active dialog is modal (such as message box)
		// to keep hot key mechanism from processing it
		// for example, to generate a command message, WM_KEYDOWN is not enough
		// but WM_KEYDOWN only can result in hot key
		//
		if (!bRet)
		{
			if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP ||
				uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP)
			{
				if (m_pDlgActive && m_pDlgActive->IsModal())
					bRet = true;
			}
		}
		
		// Process speed switch hot key here in main thread because put it in render thread
		// will cause deadlock for SetWindowLong called in render thread will result in WM_STYLECHANGE
		// message in main thread which will then try to lock render thread
		//
		if (!bRet)
		{
			if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP ||
				uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP)
			{
				// Get hot key
				int nHotKey = wParam;
				
				// Get modifiers
				//
				DWORD dwModifier(0);
				if (AUI_PRESS(VK_SHIFT))
					dwModifier |= EC_KSF_SHIFT;
				if (AUI_PRESS(VK_CONTROL))
					dwModifier |= EC_KSF_CONTROL;
				if (AUI_PRESS(VK_MENU))
					dwModifier |= EC_KSF_ALT;

				// Test usage
				CECHostInputFilter *pHostInputFilter = g_pGame->GetGameRun()->GetHostInputFilter();
				int iUsage(0);
				if (pHostInputFilter->IsHotKeyUsed(nHotKey, dwModifier, &iUsage) &&
					iUsage == LKEY_UI_SWITCH_SPEEDSETTING)
				{
					// Found it, return as processed
					if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP)
					{
						// Process when key up to avoid repeated key press	
						PAUICHECKBOX pCheck = (PAUICHECKBOX)m_pDlgHost->GetDlgItem("Chk_Sight");
						if(pCheck)
							pCheck->Check(!pCheck->IsChecked());
						m_pDlgHost->OnCommand_sight("sight");
					}

					bRet = true;
				}
			}
		}
	}

	// Compensate for those keys that's impossible to be processed by UI but currently returned as processed
	if (bRet && uMsg==WM_KEYDOWN)
	{
		if (wParam>=VK_F1 && wParam<=VK_F12 || wParam==VK_TAB)
			bRet = false;
	}
	
	if (!bRet)
	{
		bool chatFocused(false);

		if (uMsg == WM_KEYDOWN)
		{
			CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
			if (!pHost->IsChangingFace())
			{
				if( uMsg == WM_KEYDOWN && wParam == VK_RETURN && m_bShowAllPanels && 
					!GetMapDlgsMgr()->IsWorldMapShown() &&
					(!m_pDlgActive || !m_pDlgActive->GetFocus() || m_pDlgActive->
					GetFocus()->GetType() != AUIOBJECT_TYPE_EDITBOX) )
				{
					// Change focus to Chat with VK_RETURN
					//
					PAUIOBJECT pObj = m_pDlgChat->GetDlgItem("DEFAULT_Txt_Speech");
					
					BringWindowToTop(m_pDlgChat);
					pObj->SetFocus(true);
					m_pDlgChat->ChangeFocus(pObj);
					chatFocused = true;
				}
			}
		}
		
		if (!chatFocused)
		{
			if (uMsg == WM_LBUTTONDOWN ||
				uMsg == WM_LBUTTONDBLCLK)
			{
				// No chat now
				m_pDlgChat->ChangeFocus(NULL);
				m_pDlgChannelChat->ChangeFocus(NULL);
				
				char szName[40];
				for(int i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
				{
					sprintf(szName, "Win_FriendChat%d", i);
					GetDialog(szName)->ChangeFocus(NULL);
				}
			}
		}
	}

	if( m_bRepairing || m_pDlgGeneralCard->IsSwallowing())
	{
		if( uMsg == WM_MOUSEMOVE )
			bRet = true;
		else if( uMsg == WM_RBUTTONDOWN )
		{
			m_bRepairing = false;
			m_pDlgGeneralCard->SetSwallowing(false);
			g_pGame->ChangeCursor(RES_CUR_NORMAL);
			bRet = true;
		}
		else
			bRet = bRet;
	}
	else
		bRet = bRet;

	return bRet;
}

bool CECGameUIMan::OnHotKeyDown(int iUsage, bool bFirstPress)
{
	bool bInAutoMode = CECAutoPolicy::GetInstance().IsAutoPolicyEnabled();

	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if( pHost->IsChangingFace() ) return true;
	
	if (iUsage == LKEY_UI_SHOW_MAP)
	{
		if (m_bShowAllPanels && bFirstPress)
		{
			if (m_pDlgGuildMap->IsShow())
				m_pDlgGuildMap->OnCommandCancel("IDCANCEL");
			else GetMapDlgsMgr()->SwitchWorldMapShow();
		}
		return true;
	}

	if( m_pDlgShop->IsShow() || m_pDlgTrade->IsShow() || GetMapDlgsMgr()->IsWorldMapShown() )
	{
		return true;
	}

	int idServiceNPC = pHost->GetCurServiceNPC();
	if (0 != idServiceNPC)
		return true;
	
	switch (iUsage)
	{
	case LKEY_UI_SHOW_ALL:
		if (bFirstPress && CanToggleAllPannelsByInput())
			m_bShowAllPanels = !m_bShowAllPanels;
		break;
		
	case LKEY_UI_SHOW_CHARACTER:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem->OnCommandCharacter("wcharacter");
		break;
	case LKEY_UI_SHOW_INVENTORY:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem->OnCommandInventory("winventory");
		break;
	case LKEY_UI_SHOW_SKILL:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem->OnCommandSkill("wskill");
		break;
	case LKEY_UI_SHOW_PET:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem2->OnCommandPet("wpet");
		break;
	case LKEY_UI_SHOW_QUEST:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem->OnCommandQuest("wquest");
		break;
	case LKEY_UI_SHOW_QSHOP:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem->OnCommandShop("shop");
		break;
	case LKEY_UI_SHOW_ACTION:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem2->OnCommandAction("waction");
		break;
	case LKEY_UI_SHOW_TEAM:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem2->OnCommandTeam("wteam");
		break;
	case LKEY_UI_SHOW_FRIEND:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem2->OnCommandFriend("wfriend");
		break;
	case LKEY_UI_SHOW_FACTION:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem2->OnCommandFaction("wfaction");
		break;
	case LKEY_UI_SHOW_BBS:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem2->OnCommandBBS("wbbs");
		break;
	case LKEY_UI_SHOW_HELP:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem3->OnCommandHelp("whelp");
		break;
	case LKEY_UI_SHOW_CONSOLE:
		if (m_bShowAllPanels && bFirstPress)
		{
			if (glb_IsConsoleEnable())
			{
				PAUIDIALOG pShow = GetDialog(l_aDlgNames[DLG_CONSOLE]);
				pShow->SetCanMove(false);
				pShow->Show(!pShow->IsShow());
			}
		}
		break;
	case LKEY_UI_SHOW_GM:
		if (m_bShowAllPanels && bFirstPress)
		{
			if (g_pGame->GetGameRun()->GetHostPlayer()->IsGM())
				m_pDlgGMConsole->Show(!m_pDlgGMConsole->IsShow());
		}
		break;
	case LKEY_UI_SHOW_SYS:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem3->OnCommandSetting("wsetting");
		break;
	case LKEY_UI_SHOW_CAMERA:
		if (m_bShowAllPanels && bFirstPress)
		{
			g_pGame->GetGameRun()->PostMessage(MSG_HST_CAMERAMODE, MAN_PLAYER, 0);
			m_pDlgCamera->Show(!m_pDlgCamera->IsShow());
		}
		break;
	case LKEY_UI_SHOW_QUICKKEY:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgSystem3->OnCommandQuickKey("wquickkey");
		break;
		
	case LKEY_UI_QUICK9_CHANGECONTENT:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgQuickBar1->OnCommand_add("add");
		break;
	case LKEY_UI_QUICK9_SC1:
	case LKEY_UI_QUICK9_SC2:
	case LKEY_UI_QUICK9_SC3:
	case LKEY_UI_QUICK9_SC4:
	case LKEY_UI_QUICK9_SC5:
	case LKEY_UI_QUICK9_SC6:
	case LKEY_UI_QUICK9_SC7:
	case LKEY_UI_QUICK9_SC8:
	case LKEY_UI_QUICK9_SC9:
		{
			if( bInAutoMode ) break;
			if (pHost->GetBattleInfo().IsChariotWar())
			{	// 释放战车技能，屏蔽角色技能
				CChariot* pChariot = pHost->GetChariot();
				if(pChariot)
				{
					CECSkill* pSkill = pChariot->GetCurChariotSkillByIndex(iUsage-LKEY_UI_QUICK9_SC1);
					if(pSkill)
						pHost->ApplySkillShortcut(pSkill->GetSkillID());
				}				
			}
			else
			{
				int nCurPanel1 = CDlgQuickBar::GetCurPanel1();
				CECShortcutSet *pSCS = pHost->GetShortcutSet1(nCurPanel1 - 1);
				CECShortcut *pSC = pSCS->GetShortcut(iUsage-LKEY_UI_QUICK9_SC1);
				if (pSC)
				pSC->Execute();
			}			
		}
		break;
		
	case LKEY_UI_QUICK8_CHANGECONTENT:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgQuickBar2->OnCommand_add("add");
		break;
	case LKEY_UI_QUICK8_SC1:
	case LKEY_UI_QUICK8_SC2:
	case LKEY_UI_QUICK8_SC3:
	case LKEY_UI_QUICK8_SC4:
	case LKEY_UI_QUICK8_SC5:
	case LKEY_UI_QUICK8_SC6:
	case LKEY_UI_QUICK8_SC7:
	case LKEY_UI_QUICK8_SC8:
		{
			if( bInAutoMode ) break;
			
			int nCurPanel2 = CDlgQuickBar::GetCurPanel2();
			CECShortcutSet *pSCS = pHost->GetShortcutSet2(nCurPanel2 - 1);
			CECShortcut *pSC = pSCS->GetShortcut(iUsage-LKEY_UI_QUICK8_SC1);
			if (pSC)
				pSC->Execute();			
		}
		break;
		
	case LKEY_UI_CHAT_CLEAR:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgChat->OnCommand_clear("clear");
		break;
	case LKEY_UI_CHAT_REPLY:
		if (bFirstPress)
		{			
			PAUIOBJECT pObj = m_pDlgInfo->GetDlgItem("List");
			PAUIOBJECT pInfo = m_pDlgInfoIcon->GetDlgItem("F_3");
			
			m_pDlgInfo->BuildInfoList(CDlgInfo::INFO_FRIEND);
			m_pDlgInfo->SetData(CDlgInfo::INFO_FRIEND);
			m_pDlgInfo->OnEventLButtonUp_List(0, 0, pObj);
		}
		break;
		
	case LKEY_UI_TEAM_SELECT1:
	case LKEY_UI_TEAM_SELECT2:
	case LKEY_UI_TEAM_SELECT3:
	case LKEY_UI_TEAM_SELECT4:
	case LKEY_UI_TEAM_SELECT5:
	case LKEY_UI_TEAM_SELECT6:
	case LKEY_UI_TEAM_SELECT7:
	case LKEY_UI_TEAM_SELECT8:
	case LKEY_UI_TEAM_SELECT9:
		if( bInAutoMode ) break;
		if (bFirstPress)
		{
			CDlgTeamMate *pDlgMate = m_pDlgTeamMain->GetTeamMateDlg(iUsage-LKEY_UI_TEAM_SELECT1);
			if( pDlgMate && pDlgMate->IsShow() )
				pDlgMate->OnEventLButtonUp(0, 0, NULL);
		}
		break;
		
	case LKEY_UI_PET_ATTACK:
		if( bInAutoMode ) break;
		if (bFirstPress && m_pDlgQuickBarPet->IsShow())
			m_pDlgQuickBarPet->OnCommandAttack("");
		break;
	case LKEY_UI_PET_SKILL1:
	case LKEY_UI_PET_SKILL2:
	case LKEY_UI_PET_SKILL3:
	case LKEY_UI_PET_SKILL4:
	case LKEY_UI_PET_SKILL5:
	case LKEY_UI_PET_SKILL6:
	case LKEY_UI_PET_SKILL7:
		if( bInAutoMode ) break;
		if (m_pDlgQuickBarPet->IsShow())
		{
			AString szName;
			szName.Format("Skill_%d", iUsage-LKEY_UI_PET_SKILL1+1);
			PAUIOBJECT pObj = m_pDlgQuickBarPet->GetDlgItem(szName);
			m_pDlgQuickBarPet->OnEventLButtonDown_Skill(0, 0, pObj);
		}
		break;
		
	case LKEY_UI_MONSTER_AUTOSELECT:
		if( bInAutoMode ) break;
		pHost->AutoSelectTarget();
		break;
		
	case LKEY_UI_GOBLIN_SKILL1:
	case LKEY_UI_GOBLIN_SKILL2:
	case LKEY_UI_GOBLIN_SKILL3:
	case LKEY_UI_GOBLIN_SKILL4:
	case LKEY_UI_GOBLIN_SKILL5:
	case LKEY_UI_GOBLIN_SKILL6:
	case LKEY_UI_GOBLIN_SKILL7:
	case LKEY_UI_GOBLIN_SKILL8:
		{
			if( bInAutoMode ) break;
			CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
			CECHostGoblin *pHostGoblin = (CECHostGoblin *)pHost->GetGoblinModel();
			int i = iUsage - LKEY_UI_GOBLIN_SKILL1;
			if (pHostGoblin && i < pHostGoblin->GetSkillNum())
			{
				CECSkill *pSkill = (CECSkill *)pHostGoblin->GetSkill(i);
				if( pSkill && pSkill->ReadyToCast() && !pHostGoblin->CheckSkillCastCondition(pSkill))
				{
					int idSelected = pHost->GetSelectedTarget();
					bool bForctAttack = glb_GetForceAttackFlag(NULL);
					pHostGoblin->CastSkill(i, idSelected, bForctAttack);
				}
			}
		}
		break;
		
	case LKEY_UI_SHOW_AUTOHPMP:
		if (m_bShowAllPanels && bFirstPress)
			m_pDlgHost->OnCommand_autohpmp("autohpmp");
		break;

	case LKEY_UI_SHOW_AUTOHELPPOP:
		{
			if(!m_pDlgAutoHelp->IsForbidAutoHelp())
			{
				PAUIDIALOG pDlg = GetDialog("Win_WikiPop");
				pDlg->Show(!pDlg->IsShow());
				m_pDlgAutoHelp->SetAutoHelpState(pDlg->IsShow());
			}
		}
		break;
	case LKEY_UI_SHOW_AUTOPOLICY:
		{
			PAUIDIALOG pDlg = GetDialog("Win_AutoPolicy");
			if( pDlg ) pDlg->Show(!pDlg->IsShow());
		}
		break;
	case LKEY_UI_SWITCH_AUTOPOLICY:
		if( m_pDlgAutoPolicy )
			m_pDlgAutoPolicy->OnCommand_StartOrStop(NULL);
		break;
	case LKEY_UI_SYSMODEULE_SC1:
	case LKEY_UI_SYSMODEULE_SC2:
	case LKEY_UI_SYSMODEULE_SC3:
	case LKEY_UI_SYSMODEULE_SC4:
		if (m_pDlgSysModuleQuickBar->IsShow())
		{
			AString szName;
			szName.Format("Item_%d", iUsage-LKEY_UI_SYSMODEULE_SC1+1);
			PAUIOBJECT pObj = m_pDlgSysModuleQuickBar->GetDlgItem(szName);
			if(pObj)
			{
				CECSCSysModule *pSC = (CECSCSysModule *)pObj->GetDataPtr("ptr_CECSCSysModule");
				if (pSC)				
					pSC->Execute();				
			}

		}
		break;
	}
	
	return true;
}

bool CECGameUIMan::CustomizeHotKey(int iUsageCustomize, int nHotKey, DWORD dwModifier)
{
	// Test if hot key can be another customize for given usage
	// Message box will pop up when conflict
	//
	bool result(false);	
	
	while (true)
	{
		if (!m_pDlgSettingQuickKey->IsShow())
			break;

		if (m_pDlgActive && m_pDlgActive!=m_pDlgSettingQuickKey)
			break;

		result = m_pDlgSettingQuickKey->CustomizeHotKey(iUsageCustomize, nHotKey, dwModifier);

		break;
	}

	return result;
}

void CECGameUIMan::VerifyComboSkillUI()
{
	// 验证并修复连续技图标相关错误
	// 在config数据从服务器加载后调用

	EC_VIDEO_SETTING &vs = const_cast<EC_VIDEO_SETTING &>(g_pGame->GetConfigs()->GetVideoSettings());
	A2DSprite * pSkillGroupIcons = m_pA2DSpriteIcons[ICONS_SKILLGRP];
	int nIconNum = pSkillGroupIcons->GetItemNum();
	for (int i = 0; i < EC_COMBOSKILL_NUM; ++ i)
	{
		EC_COMBOSKILL &cur = vs.comboSkill[i];
		if (cur.nIcon < 0 || cur.nIcon+1 >= nIconNum)
			cur.nIcon = 1;
	}
}

bool CECGameUIMan::OnCommand(const char* szCommand, AUIDialog* pDlg)
{
	if (CECBaseUIMan::OnCommand(szCommand, pDlg))
	{
		if(pDlg && pDlg->IsPopup()) pDlg->Show(false);
		return true;
	}

	if( 0 == stricmp(pDlg->GetName(), "Win_Popmsg") ||
		0 == stricmp(pDlg->GetName(), "Win_EnterWait") )
	{
		return false;
	}

	//Customize Dlgs
	if( 0 == stricmp(pDlg->GetName(), "Win_ChooseEar") ||
		0 == stricmp(pDlg->GetName(), "Win_ChooseEye") ||
		0 == stricmp(pDlg->GetName(), "Win_ChooseEyebrow") ||
		0 == stricmp(pDlg->GetName(), "Win_ChooseFace") ||
		0 == stricmp(pDlg->GetName(), "Win_ChooseHair") ||
		0 == stricmp(pDlg->GetName(), "Win_ChooseMouth") ||
		0 == stricmp(pDlg->GetName(), "Win_ChooseNose") ||
		0 == stricmp(pDlg->GetName(), "Win_ChooseFacePainting") ||
		0 == stricmp(pDlg->GetName(), "Win_ChoosePreCustomize") ||
		0 == stricmp(pDlg->GetName(), "Win_ChooseStature") ||
		0 == stricmp(pDlg->GetName(), "Win_AdvancedOption") ||
		0 == stricmp(pDlg->GetName(), "Win_ChoosePreCustomize1"))
	{
		return false;
	}

	//Auto Home Dlgs
	if( 0 == stricmp(pDlg->GetName(), "Dlg_Building") ||
		0 == stricmp(pDlg->GetName(), "Dlg_Light") ||
		0 == stricmp(pDlg->GetName(), "Dlg_LocalTexture") ||
		0 == stricmp(pDlg->GetName(), "Dlg_Mountain") ||
		0 == stricmp(pDlg->GetName(), "Dlg_Plant") ||
		0 == stricmp(pDlg->GetName(), "Dlg_Road") ||
		0 == stricmp(pDlg->GetName(), "Dlg_Terrain") ||
		0 == stricmp(pDlg->GetName(), "Dlg_Water") )
	{
		return false;
	}

	if (0 == stricmp(pDlg->GetName(), "win_returnreward") && 0 == stricmp(szCommand,"btn_getreward"))
	{
		g_pGame->GetGameRun()->GetHostPlayer()->GetTaskInterface()->GetLoginReward();
		pDlg->Show(false);
	}

	if( 0 == stricmp(szCommand, "IDCANCEL") )
	{
		pDlg->Show(false);
		return true;
	}

	return false;
}

bool CECGameUIMan::OnMessageBox(int iRetVal, AUIDialog* pDlg)
{
	if (pDlg == m_pDlgMessageBox) return OnNewMessageBox(iRetVal);

	CECGameSession *pSession = g_pGame->GetGameSession();
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();

	if( 0 == stricmp(pDlg->GetName(), "Game_Quit") && IDYES == iRetVal )
	{
		// Cancel hotkey customize because hot key state is determinted by CECHostInputFilter
		// which is shared between repick role
		//
		if (m_pDlgSettingQuickKey->IsShow())
			m_pDlgSettingQuickKey->Show(false);

		if( pSession->IsConnected() )
		{
			if (CECCrossServer::Instance().IsOnSpecialServer())
				g_pGame->GetGameRun()->GetPendingLogOut().AppendForSaveConfig(new CECPendingLogoutCrossServer());
			else
				g_pGame->GetGameRun()->GetPendingLogOut().AppendForSaveConfig(new CECPendingLogoutHalf());
		}
		else
			g_pGame->GetGameRun()->SetLogoutFlag(2);
	}
	else if(m_pDlgModifyName->IsShow() && !stricmp(pDlg->GetName(), m_pDlgModifyName->GetMessageBoxName()) && IDYES == iRetVal)
	{
		m_pDlgModifyName->SendRenameProtocol();
	}	
	else if(m_pDlgSimpleCostItemService->IsShow() && !stricmp(pDlg->GetName(), m_pDlgSimpleCostItemService->GetMessageBoxName()))
	{
		m_pDlgSimpleCostItemService->OnMessageBox(iRetVal);
	}
	else if(!stricmp(pDlg->GetName(), "MeridiansImpact") && IDYES == iRetVal)
	{
		m_pDlgMeridiansImpact->SendImpactProtocol();
	}
	else if(!stricmp(pDlg->GetName(), "Game_GiveOrAskFor") && IDOK == iRetVal)
	{
		m_pDlgGivingFor->DoSendProtocol();
	}
	else if(!stricmp(pDlg->GetName(), "Game_MailAskFor"))
	{
		if(IDOK != iRetVal)
		{
			m_pDlgGivingFor->DoSendMailGivingFor(false);
		}
		else
		{		
			ACString temp = GetStringFromTable(10411);
			
			
			if (NewMessageBox("Game_MailAskFor",temp,MB_YESNO)){
				m_pDlgMessageBox->SetText(CDlgMessageBox::BTN_YES, GetStringFromTable(9772));
				m_pDlgMessageBox->SetText(CDlgMessageBox::BTN_NO, GetStringFromTable(9773));
			}
		}
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_InviteGroup") )
	{
		int idLeader = pDlg->GetData();
		int idTeamSeq = (DWORD)pDlg->GetDataPtr("dword");
		if( IDYES == iRetVal )
			pSession->c2s_CmdTeamAgreeInvite(idLeader, idTeamSeq);
		else
			pSession->c2s_CmdTeamRejectInvite(idLeader);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_AskToJoin") )
	{
		pSession->c2s_CmdTeamReplyJoinAsk(pDlg->GetData(), iRetVal == IDYES);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_InviteTrade") )
	{
		if( !m_pDlgTrade->IsShow())
		{
			DWORD dwHandle = (DWORD)pDlg->GetDataPtr("dword");
			pSession->trade_StartResponse(dwHandle, IDYES == iRetVal ? true : false);
		}
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_Disenchase") && IDOK == iRetVal )
	{
		PAUIDIALOG pMsgBox;
		CECIvtrItem *pIvtr = (CECIvtrItem *)m_pDlgUninstall->m_pItema->GetDataPtr("ptr_CECIvtrItem");

		pSession->c2s_CmdNPCSevClearEmbeddedChip(
			(WORD)m_pDlgUninstall->m_pItema->GetData(), pIvtr->GetTemplateID());

		m_pDlgUninstall->Show(false);
		pHost->EndNPCService();
		m_pCurNPCEssence = NULL;
		m_pDlgInventory->Show(false);
		pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();

		MessageBox("", GetStringFromTable(228), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
	}

	else if( (0 == stricmp(pDlg->GetName(), "Game_TeachSkill") && IDOK == iRetVal) ||
		(0 == stricmp(pDlg->GetName(), "Game_LearnSkill") && IDOK == iRetVal) )
	{
		if (0 == stricmp(pDlg->GetName(), "Game_TeachSkill")) 
		{
			CECSkill *pSkill = (CECSkill *)pDlg->GetDataPtr("ptr_CECSkill");
			int nCondition = pHost->CheckSkillLearnCondition(pSkill->GetSkillID(), true);
			
			if( 0 == nCondition )
				pSession->c2s_CmdNPCSevLearnSkill(pSkill->GetSkillID());
			else if( 1 == nCondition )
				AddChatMessage(GetStringFromTable(270), GP_CHAT_MISC);
			else if( 6 == nCondition )
				AddChatMessage(GetStringFromTable(527), GP_CHAT_MISC);
			else if( 7 == nCondition )
				AddChatMessage(GetStringFromTable(541), GP_CHAT_MISC);
			else if( 8 == nCondition )
				AddChatMessage(GetStringFromTable(271), GP_CHAT_MISC);
			else if( 9 == nCondition )
				AddChatMessage(GetStringFromTable(556), GP_CHAT_MISC);
			else if( 10 == nCondition )
				AddChatMessage(GetStringFromTable(557), GP_CHAT_MISC);
			else if( 12 == nCondition )
			AddChatMessage(GetStringFromTable(11168), GP_CHAT_MISC);
		} else if (0 == stricmp(pDlg->GetName(), "Game_LearnSkill")) {
			int skillID = pDlg->GetData();
			int nCondition = pHost->CheckSkillLearnCondition(skillID, true);
			
			if( 0 == nCondition ) {
				pSession->c2s_CmdNPCSevLearnSkill(skillID);
			}
			else if( 1 == nCondition )
				AddChatMessage(GetStringFromTable(270), GP_CHAT_MISC);
			else if( 6 == nCondition )
				AddChatMessage(GetStringFromTable(527), GP_CHAT_MISC);
			else if( 7 == nCondition )
				AddChatMessage(GetStringFromTable(541), GP_CHAT_MISC);
			else if( 8 == nCondition )
				AddChatMessage(GetStringFromTable(271), GP_CHAT_MISC);
			else if( 9 == nCondition )
				AddChatMessage(GetStringFromTable(556), GP_CHAT_MISC);
			else if( 10 == nCondition )
				AddChatMessage(GetStringFromTable(557), GP_CHAT_MISC);
			else if( 12 == nCondition )
			AddChatMessage(GetStringFromTable(11168), GP_CHAT_MISC);
		}

	}
	/*
	else if (0 == stricmp(pDlg->GetName(), "Game_LearnSkill") && IDOK == iRetVal)
	{
		int skillID = pDlg->GetData();
		int nCondition = pHost->CheckSkillLearnCondition(skillID, true);

		if( 0 == nCondition )
			pSession->c2s_CmdNPCSevLearnSkill(skillID);
		else if( 1 == nCondition )
			AddChatMessage(GetStringFromTable(270), GP_CHAT_MISC);
		else if( 6 == nCondition )
			AddChatMessage(GetStringFromTable(527), GP_CHAT_MISC);
		else if( 7 == nCondition )
			AddChatMessage(GetStringFromTable(541), GP_CHAT_MISC);
		else if( 8 == nCondition )
			AddChatMessage(GetStringFromTable(271), GP_CHAT_MISC);
		else if( 9 == nCondition )
			AddChatMessage(GetStringFromTable(556), GP_CHAT_MISC);
		else if( 10 == nCondition )
			AddChatMessage(GetStringFromTable(557), GP_CHAT_MISC);
		else if( 12 == nCondition )
			AddChatMessage(GetStringFromTable(11168), GP_CHAT_MISC);
	}
	*/
	else if( 0 == stricmp(pDlg->GetName(), "Game_TeachPetSkill") && IDOK == iRetVal )
	{
		CECSkill *pSkill = (CECSkill *)pDlg->GetDataPtr("ptr_CECSkill");
		int nCondition = pHost->CheckPetSkillLearnCondition(pSkill->GetSkillID(), true);

		if( 0 == nCondition )
			pSession->c2s_CmdNPCSevPetSkillLearn(pSkill->GetSkillID());
		else if( 1 == nCondition )
			AddChatMessage(GetStringFromTable(270), GP_CHAT_MISC);
		else if( 6 == nCondition )
			AddChatMessage(GetStringFromTable(527), GP_CHAT_MISC);
		else if( 7 == nCondition )
			AddChatMessage(GetStringFromTable(541), GP_CHAT_MISC);
		else if( 8 == nCondition )
			AddChatMessage(GetStringFromTable(271), GP_CHAT_MISC);
		else if( 9 == nCondition )
			AddChatMessage(GetStringFromTable(556), GP_CHAT_MISC);
		else if( 10 == nCondition )
			AddChatMessage(GetStringFromTable(557), GP_CHAT_MISC);
		else if( 12 == nCondition )
			AddChatMessage(GetStringFromTable(11168), GP_CHAT_MISC);
	}
	//add for goblin by czx
	else if (0 == stricmp(pDlg->GetName(), "Game_ELFToProtect") && IDYES == iRetVal)
	{
		pSession->c2s_CmdGoblinChangeSecureStatus(0);
		m_pDlgELFTransaction1->Show(false);
		m_pDlgELFTransaction2->Show(false);
	}
	else if (0 == stricmp(pDlg->GetName(), "Game_ELFToFree") && IDYES == iRetVal)
	{
		pSession->c2s_CmdGoblinChangeSecureStatus(1);
		m_pDlgELFTransaction2->Show(false);
	}
	else if (0 == stricmp(pDlg->GetName(), "Game_GoblinAddGenius") && IDYES == iRetVal)
	{
		int pt[5] = {0};
		int index = pDlg->GetData();
		pt[index] = 1;
		pSession->c2s_CmdGoblinAddGenius(pt[0], pt[1], pt[2], pt[3], pt[4]);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_TeachGoblinSkill") && IDYES == iRetVal )
	{
		CECSkill *pSkill = (CECSkill *)pDlg->GetDataPtr("ptr_CECSkill");
		int index = pDlg->GetData();
		pSession->c2s_CmdNPCSevGoblinLearnSkill(index, pSkill->GetSkillID());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_TeamDisband") && IDOK == iRetVal )
	{
		pSession->c2s_CmdTeamLeaveParty();
		m_pDlgArrangeTeam->Show(false);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_AbortTask") && IDOK == iRetVal )
	{
		CECTaskInterface *pTask = pHost->GetTaskInterface();
		pTask->GiveUpTask(pDlg->GetData());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_RepairAll") && IDOK == iRetVal )
	{
		pSession->c2s_CmdNPCSevRepairAll();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_SellOutDbCheck") && IDOK == iRetVal )
	{
		m_pDlgShop->OnCommand_sell(NULL);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_SellOut") && IDOK == iRetVal )
	{
		m_bRepairing = false;
		pHost->SellItemsToNPC();
		pHost->GetDealPack()->RemoveAllItems();
		pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_PSellOut") && IDOK == iRetVal )
	{
		pHost->SellItemsToBooth();
		pHost->GetDealPack()->RemoveAllItems();
		pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	}
	else if( strstr(pDlg->GetName(), "Game_Mark_") == pDlg->GetName() )
	{
		m_pDlgMiniMapMark->OnMessageBox(pDlg, iRetVal);
	}
	else if( 0 == stricmp(pDlg->GetName(), "MsgBox_LinkBroken") && IDOK == iRetVal )
	{
		g_pGame->GetGameRun()->SetLogoutFlag(2);
	}
	else if( 0 == stricmp(pDlg->GetName(), "MsgBox_RemindReconnect"))
	{
		if (IDYES != iRetVal && IDOK != iRetVal ||
			!CECReconnect::Instance().CanRequestReconnect() ||
			!CECReconnect::Instance().RequestReconnect()){
			CECReconnect::Instance().ResetState();
			g_pGame->GetGameRun()->SetLogoutFlag(2);
		}
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_DelFriend") && IDOK == iRetVal )
	{
		PAUITREEVIEW pTree = (PAUITREEVIEW)m_pDlgFriendList->GetDlgItem("Tv_FriendList");
		P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
		if( !pItem ) return true;

		int idFriend = (int)pTree->GetItemData(pItem);
		pSession->friend_Delete(idFriend);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_DelGroup") && IDOK == iRetVal )
	{
		PAUITREEVIEW pTree = (PAUITREEVIEW)m_pDlgFriendList->GetDlgItem("Tv_FriendList");
		P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
		if( !pItem ) return true;

		int idGroup = (int)pTree->GetItemData(pItem);
		pSession->friend_SetGroupName(idGroup, _AL(""));
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_PVPOpen") && IDOK == iRetVal )
	{
		pSession->c2s_CmdEnalbePVP();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_PVPClose") && IDOK == iRetVal )
	{
		pSession->c2s_CmdDisablePVP();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_ForgetSkill") && IDOK == iRetVal )
	{
		CECSkill *pSkill = (CECSkill *)pDlg->GetDataPtr("ptr_CECSkill");

		pSession->c2s_CmdNPCSevForgetSkill(pSkill->GetSkillID());
		m_pDlgNPC->Show(false);
		EndNPCService();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_DelHistory") && IDOK == iRetVal )
	{
		char szPath[MAX_PATH];
		PAUIDIALOG pMsgBox = NULL;

		sprintf(szPath, "%s\\Userdata\\Msg\\%d", af_GetBaseDir(), pHost->GetCharacterID());
		if( RemoveFilesAndDirectory(szPath) )
		{
			MessageBox("", GetStringFromTable(567), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		}
		else
		{
			MessageBox("", GetStringFromTable(568), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		}
		if( pMsgBox ) pMsgBox->SetLife(3);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Faction_Quit") && IDOK == iRetVal )
	{
		pSession->faction_leave();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Faction_Pass") && IDOK == iRetVal )
	{
		pSession->faction_master_resign(pDlg->GetData());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Faction_Demiss") && IDOK == iRetVal )
	{
		pSession->faction_resign();
	}
	else if( IDOK == iRetVal &&
		(0 == stricmp(pDlg->GetName(), "Faction_Promote") ||
		0 == stricmp(pDlg->GetName(), "Faction_Demote")) )
	{
		int i, idPlayer = pDlg->GetData();
		FactionMemList &fml = g_pGame->GetFactionMan()->GetMemList();

		for( i = 0; i < (int)fml.size(); i++ )
		{
			if( fml[i]->GetID() == idPlayer )
			{
				if( 0 == stricmp(pDlg->GetName(), "Faction_Promote") )
					pSession->faction_appoint(idPlayer, fml[i]->GetFRoleID() - 1);
				else
					pSession->faction_appoint(idPlayer, fml[i]->GetFRoleID() + 1);
				break;
			}
		}
	}
	else if( 0 == stricmp(pDlg->GetName(), "Faction_Kick") && IDOK == iRetVal )
	{
		pSession->faction_expel_member(pDlg->GetData());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Faction_CancelKick") && IDOK == iRetVal )
	{
		pSession->faction_cancel_expel_member(pDlg->GetData());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Faction_Upgrade") && IDOK == iRetVal )
	{
		if( pHost->GetMoneyAmount() >= 2000000 )
			pSession->faction_upgrade();
		else
		{
			MessageBox("", GetStringFromTable(611),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}
	else if( 0 == stricmp(pDlg->GetName(), "Faction_Dismiss") && IDOK == iRetVal )
	{
		pSession->faction_dismiss();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Faction_PVP_Open"))
	{
		if (IDYES == iRetVal){
			pSession->c2s_CmdNPCSevOpenFactionPVP();
		}
		EndNPCService();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_InviteFaction") && pHost->GetFactionID() <= 0 )
	{
		DWORD dwHandle = (DWORD)pDlg->GetDataPtr("dword");
		if( iRetVal == IDYES )
			m_pDlgGuildMan->SetLastRefreshTime(0);
		pSession->faction_invite_response(dwHandle, IDYES == iRetVal ? true : false);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_ChannelInvite") )
	{
		if( iRetVal == IDYES )
			pSession->chatroom_Join(pDlg->GetData(), pDlg->GetText() );
		else
			pSession->chatroom_RejectInvite(pDlg->GetData(), (DWORD)pDlg->GetDataPtr("dword") );
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_ChannelJoinAsCreator") && iRetVal == IDYES )
	{
		pSession->chatroom_Join(pDlg->GetText(), _AL("") );
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_ChannelJoinAsNumber") && iRetVal == IDYES )
	{
		pSession->chatroom_Join(pDlg->GetData(), _AL("") );
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_ChannelCreate") && iRetVal == IDYES )
	{
		ACString strText = pDlg->GetText();
		int n = strText.Find(_AL("\r"));
		pSession->chatroom_Create(strText.Left(n), strText.Mid(n + 1), 30);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_ChannelClose") && iRetVal == IDYES )
	{
		pSession->chatroom_Leave(pDlg->GetData());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_MailDelete") && IDOK == iRetVal )
	{
		m_pDlgMailList->SetWaitingResponse(pDlg->GetData());
		pSession->mail_Delete(pDlg->GetData());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_AuctionBid") && iRetVal == IDYES )
	{
		pSession->auction_Bid(pDlg->GetData(), (int)pDlg->GetDataPtr(), false);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_AuctionBin") && iRetVal == IDYES )
	{
		pSession->auction_Bid(pDlg->GetData(), (int)pDlg->GetDataPtr(), true);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_AuctionClose") && iRetVal == IDYES )
	{
		pSession->auction_Close(pDlg->GetData(), 0);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_AuctionGiveUp") && iRetVal == IDYES )
	{
		pSession->auction_ExitBid(pDlg->GetData());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_InviteBind") )
	{
		if (pHost->CanAcceptBind())
			pSession->c2s_CmdBindPlayerInviteRe(pDlg->GetData(), iRetVal == IDNO);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_RequestBind") )
	{
		if (pHost->CanAcceptBind())
			pSession->c2s_CmdBindPlayerRequestRe(pDlg->GetData(), iRetVal == IDNO);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_InviteDuel") )
	{
		pSession->c2s_CmdDuelReply(pDlg->GetData(), iRetVal == IDNO ? 2 : 0);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_PetBanish") && iRetVal == IDOK)
	{
		pSession->c2s_CmdPetBanish(pDlg->GetData());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_BattleLeave") )
	{
		if( iRetVal == IDOK )
			pSession->c2s_CmdNPCSevLeaveBattle();
		EndNPCService();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_ChatWorld") )
	{
		int nPack = (int)pDlg->GetData();
		int nSlot = (int)pDlg->GetDataPtr();

		CDlgChat::CHAT_MSG cm;
		cm.nPack = nPack;
		cm.nSlot = nSlot;
		
		if (iRetVal == IDYES || iRetVal == IDOK)
		{
			pSession->SendChatData(GP_CHAT_FARCRY, pDlg->GetText(), nPack, nSlot);
			m_pDlgChat->m_dwTickFarCry = GetTickCount();
			cm.dwTime = m_pDlgChat->m_dwTickFarCry;
		}
		else
			cm.dwTime = 0;
		cm.cChannel = GP_CHAT_FARCRY;
		cm.strMsg = ACString(_AL("!@")) + pDlg->GetText();
		m_pDlgChat->m_vecHistory.push_back(cm);
		if( m_pDlgChat->m_vecHistory.size() >= 20 )
			m_pDlgChat->m_vecHistory.erase(m_pDlgChat->m_vecHistory.begin());
		m_pDlgChat->m_nCurHistory = m_pDlgChat->m_vecHistory.size();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_ChatWorld2") )
	{
		int nPack = (int)pDlg->GetData();
		int nSlot = (int)pDlg->GetDataPtr();

		CDlgChat::CHAT_MSG cm;
		cm.nPack = nPack;
		cm.nSlot = nSlot;

		if (iRetVal == IDYES || iRetVal == IDOK)
		{
			pSession->SendChatData(GP_CHAT_SUPERFARCRY, pDlg->GetText(), nPack, nSlot);
			m_pDlgChat->m_dwTickFarCry2 = GetTickCount();
			cm.dwTime = m_pDlgChat->m_dwTickFarCry2;
		}
		else
			cm.dwTime = 0;
		cm.cChannel = GP_CHAT_SUPERFARCRY;
		cm.strMsg = ACString(_AL("!#")) + pDlg->GetText();
		cm.strMsg.CutRight(8); // 删除附加的颜色代码等信息
		m_pDlgChat->m_vecHistory.push_back(cm);
		if( m_pDlgChat->m_vecHistory.size() >= 20 )
			m_pDlgChat->m_vecHistory.erase(m_pDlgChat->m_vecHistory.begin());
		m_pDlgChat->m_nCurHistory = m_pDlgChat->m_vecHistory.size();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_ChatCountry") )
	{
		int iPack = (int)pDlg->GetData();
		int iSlot = (int)pDlg->GetDataPtr();
		bool bSendMsg = (iRetVal == IDOK || iRetVal == IDYES);
		m_pDlgChat->DoSendCountryMessage(iPack, iSlot, pDlg->GetText(), bSendMsg);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_SellPoint") && iRetVal == IDOK )
	{
		pSession->account_SellPoint(pDlg->GetData(), (int)pDlg->GetDataPtr());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_CancelSellPoint") && iRetVal == IDOK )
	{
		pSession->account_CancelSellPoint(pDlg->GetData());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_BuyPoint") && iRetVal == IDOK )
	{
		pSession->account_BuyPoint(pDlg->GetData(), (int)pDlg->GetDataPtr());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_EquipBind1") && iRetVal == IDYES )
	{
		pSession->c2s_CmdEquipItem((int)pDlg->GetData(), (int)pDlg->GetDataPtr());
		CECIvtrItem *pItem = pHost->GetPack()->GetItem((int)pDlg->GetData());
		if( pItem )
			PlayItemSound(pItem, true);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_EquipBind2") && iRetVal == IDYES )
	{
		pHost->UseItemInPack(IVTRTYPE_PACK, (int)pDlg->GetData());
		CECIvtrItem *pItem = pHost->GetPack()->GetItem((int)pDlg->GetData());
		if( pItem )
			PlayItemSound(pItem, true);
	}
	else if (0 == stricmp(pDlg->GetName(), "Game_GoblinEquip") && iRetVal == IDYES )
	{
		pSession->c2s_CmdGoblinEquipItem((int)pDlg->GetData());
	}
	else if (0 == stricmp(pDlg->GetName(), "Game_ELFRetrain") && iRetVal == IDYES )
	{
		DWORD data = pDlg->GetData();
		int level = data & 0xFF;
		data >>= 16;
		int slot = data;
		int skillID = (int)pDlg->GetDataPtr();
		pSession->c2s_CmdNPCSevGoblinForgetSkill(slot, skillID, level);
	}
	else if (0 == stricmp(pDlg->GetName(), "Game_ELFEquipRemove"))
	{
		m_pDlgELFEquipRemove->ConfirmOK(iRetVal == IDYES );
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_GuildChallenge") && iRetVal == IDOK )
	{
		pSession->battle_Challenge(pDlg->GetData(), 
			pHost->GetFactionID(), (int)pDlg->GetDataPtr());
		m_pDlgGMapChallenge->Show(false);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_ExchangeAutoItem") && iRetVal == IDOK )
	{
		pHost->UseItemInPack(IVTRTYPE_PACK, pDlg->GetData());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_GoldBuy") && iRetVal == IDYES )
	{
		pSession->stock_commission(1, pDlg->GetData(), int(pDlg->GetDataPtr()));
		m_pDlgGoldTrade->GetDlgItem("Txt_Num")->SetText(_AL(""));
		m_pDlgGoldTrade->GetDlgItem("Txt_Num")->SetData(0);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_GoldSell") && iRetVal == IDYES )
	{
		pSession->stock_commission(0, pDlg->GetData(), int(pDlg->GetDataPtr()));
		m_pDlgGoldTrade->GetDlgItem("Txt_Num")->SetText(_AL(""));
		m_pDlgGoldTrade->GetDlgItem("Txt_Num")->SetData(0);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_GoldGiveUp") && iRetVal == IDYES )
	{
		pSession->stock_cancel(pDlg->GetData(), (int)pDlg->GetDataPtr());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_SetAutoLock") && iRetVal == IDOK )
	{
		pSession->autolock_set(pDlg->GetData());
	}
	else if (0 == stricmp(pDlg->GetName(), "Game_GoblinToPill") && iRetVal == IDYES)
	{
		int slot = (int)pDlg->GetData();
		pSession->c2s_CmdNPCSevGoblinDestroy(slot);
		m_pDlgELFToPill->OnEventLButtonDown_ELF(0, 0, NULL);
	}
	else if (0 == stricmp(pDlg->GetName(), "Game_Over40Point") && iRetVal == IDYES)
	{
		m_pDlgELFProp->OnCommand_YES();
	}
	else if (0 == stricmp(pDlg->GetName(), "Game_ConfirmTrade") && iRetVal == IDYES)
	{
		int num = pDlg->GetData();
		g_pGame->GetGameRun()->GetHostPlayer()->BuyItemsFromBooth(num);
	}
	else if (!stricmp(pDlg->GetName(), "Game_ReferralHint") && iRetVal == IDYES)
	{
		int idReferrer = pDlg->GetData();
		const ACHAR *pszName = g_pGame->GetGameRun()->GetPlayerName(idReferrer, false);
		g_pGame->GetGameSession()->friend_Add(idReferrer, pszName);
		AddChatMessage(GetStringFromTable(533), GP_CHAT_MISC);
		pDlg->Show(false);
	}
	else if (!stricmp(pDlg->GetName(), "HotKey_Apply") && iRetVal == IDYES)
	{
		int nType = pDlg->GetData();
		m_pDlgSettingQuickKey->ConfirmHotKeyCustomize(nType==2);
	}
	else if (!stricmp(pDlg->GetName(), "HotKey_Customize") && iRetVal == IDYES)
	{
		m_pDlgSettingQuickKey->CustomizeHotKey(pDlg->GetData(), (DWORD)pDlg->GetDataPtr());
	}	
	else if (!stricmp(pDlg->GetName(), "Game_Exp") && iRetVal == IDYES)
	{
		int iChoice = pDlg->GetData();
		switch(iChoice)
		{
		case 1:
			// 选择聚灵套餐
			pSession->c2s_CmdChooseMultiExp((int)pDlg->GetDataPtr());
			break;
		case 2:
			// 重新开启聚灵
			pSession->c2s_CmdToggleMultiExp(true);
			break;
		case 3:
			// 暂停聚灵
			pSession->c2s_CmdToggleMultiExp(false);
			break;
		}
	}
	else if (!stricmp(pDlg->GetName(), "Game_DropIvtrItem") && iRetVal == IDYES)
	{
		int iSrc = pDlg->GetData();
		int count = (int)pDlg->GetDataPtr();
		if (count > 0)
			pSession->c2s_CmdDropIvtrItem(iSrc, count);
	}
	else if (!stricmp(pDlg->GetName(), "Game_DropEquipItem") && iRetVal == IDYES)
	{
		int iSrc = pDlg->GetData();
		pSession->c2s_CmdDropEquipItem(iSrc);
	}
	else if (strstr(pDlg->GetName(), "Game_WebTrade_") == pDlg->GetName())
	{
		if(m_pDlgWebTradeCurrent)
		{
			m_pDlgWebTradeCurrent->OnMessageBox(pDlg, iRetVal);
		}
	}
	else if (strstr(pDlg->GetName(), "Game_Coupon_") == pDlg->GetName())
	{
		CDlgCoupon* pDlgCoupon = dynamic_cast<CDlgCoupon*>(GetDialog("Win_Coupon"));
		if(pDlgCoupon) pDlgCoupon->OnMessageBox(pDlg, iRetVal);
	}
	else if (strstr(pDlg->GetName(), "Game_BShop_") == pDlg->GetName())
	{
		if(m_pDlgBShop)
		{
			m_pDlgBShop->OnMessageBox(pDlg, iRetVal);
		}
	}
	else if (!stricmp(pDlg->GetName(), "Game_WriteCard"))
	{
		if (m_pDlgWeddingInvite->IsShow())
			m_pDlgWeddingInvite->WriteCard(iRetVal==IDYES);
	}
	else if (!stricmp(pDlg->GetName(), "Game_CancelBook"))
	{
		if (m_pDlgWedding->IsShow())
			m_pDlgWedding->CancelBook(iRetVal==IDYES);
	}
	else if(!stricmp(pDlg->GetName(), "Game_TaskRefresh"))
	{
		CDlgTaskList* pTaskDlg = (CDlgTaskList*)GetDialog("Win_QuestList");
		pTaskDlg->ConfirmRefresh(pDlg, iRetVal);
	}
	else if(!stricmp(pDlg->GetName(), "Game_TaskListAccept"))
	{
		CDlgTaskList* pTaskDlg = (CDlgTaskList*)GetDialog("Win_QuestList");
		pTaskDlg->ConfirmAccept(pDlg, iRetVal);
	}
	else if (!stricmp(pDlg->GetName(), "Fortress_Create"))
	{
		if (iRetVal == IDYES)
			g_pGame->GetGameSession()->factionFortress_Create();
		EndNPCService();
	}
	else if (!stricmp(pDlg->GetName(), "Fortress_TechLevelup"))
	{
		if (iRetVal == IDOK)
		{
			int iTech = pDlg->GetData();
			g_pGame->GetGameSession()->c2s_CmdNPCSevFactionFortressTechLevelup(iTech);
		}
	}
	else if (!stricmp(pDlg->GetName(), "Fortress_TechReset"))
	{
		if (iRetVal == IDOK)
		{
			int iTech = (int)(pDlg->GetData() >> 16);
			int iSlot = (int)(pDlg->GetData() & 0xffff);
			g_pGame->GetGameSession()->c2s_CmdNPCSevFactionFortressResetTech(iTech, iSlot, CDlgFortressInfo::RESETTECH_ITEM);
		}
	}
	else if (!stricmp(pDlg->GetName(), "Fortress_Build"))
	{
		if (iRetVal == IDOK)
		{
			int idBuilding = pDlg->GetData();
			int nAccelerate = (int)pDlg->GetDataPtr();
			g_pGame->GetGameSession()->c2s_CmdNPCSevFactionFortressConstructLevelup(idBuilding, nAccelerate);
		}
	}
	else if (!stricmp(pDlg->GetName(), "Fortress_Destroy"))
	{
		if (iRetVal == IDOK)
		{
			int idBuilding = pDlg->GetData();
			g_pGame->GetGameSession()->c2s_CmdNPCSevFactionFortressConstructDestroy(idBuilding);
		}
	}
	else if (!stricmp(pDlg->GetName(), "Fortress_DeclareWar"))
	{
		if (iRetVal == IDOK)
		{
			int idTarget = pDlg->GetData();
			g_pGame->GetGameSession()->factionFortress_Challenge(idTarget);
		}
	}
	else if (!stricmp(pDlg->GetName(), "GDiplomacy_Alliance_Apply"))
	{
		if (iRetVal == IDYES)
		{
			int idFaction = pDlg->GetData();
			g_pGame->GetGameSession()->faction_relation_apply(idFaction, true);
			m_pDlgGuildDiplomacyMan->SetLastSendTime(time(NULL));
		}
	}
	else if (!stricmp(pDlg->GetName(), "GDiplomacy_Hostility_Apply"))
	{
		if (iRetVal == IDYES)
		{
			int idFaction = pDlg->GetData();
			g_pGame->GetGameSession()->faction_relation_apply(idFaction, false);
			m_pDlgGuildDiplomacyMan->SetLastSendTime(time(NULL));
		}
	}
	else if (!stricmp(pDlg->GetName(), "GDiplomacy_Relation_Remove_Apply"))
	{
		if (iRetVal == IDYES)
		{
			int idFaction = pDlg->GetData();
			bool bForce = (pDlg->GetDataPtr() != 0);
			g_pGame->GetGameSession()->faction_remove_relation_apply(idFaction, bForce);
			m_pDlgGuildDiplomacyMan->SetLastSendTime(time(NULL));
		}
	}
	else if (!stricmp(pDlg->GetName(), "GDiplomacy_Reply_Agree"))
	{
		if (iRetVal == IDYES)
		{
			int idFaction = pDlg->GetData();
			bool bAlliance = (pDlg->GetDataPtr() != 0);
			g_pGame->GetGameSession()->faction_relation_reply(idFaction, bAlliance, true);
			m_pDlgGuildDiplomacyMan->SetLastSendTime(time(NULL));
		}
	}
	else if (!stricmp(pDlg->GetName(), "GDiplomacy_Reply_Refuse"))
	{
		if (iRetVal == IDYES)
		{
			int idFaction = pDlg->GetData();
			bool bAlliance = (pDlg->GetDataPtr() != 0);
			g_pGame->GetGameSession()->faction_relation_reply(idFaction, bAlliance, false);
			m_pDlgGuildDiplomacyMan->SetLastSendTime(time(NULL));
		}
	}
	else if (!stricmp(pDlg->GetName(), "GDiplomacy_Relation_Remove_Reply"))
	{
		if (iRetVal == IDYES)
		{
			int idFaction = pDlg->GetData();
			bool bAgree = (pDlg->GetDataPtr() != 0);
			g_pGame->GetGameSession()->faction_remove_relation_reply(idFaction, bAgree);
			m_pDlgGuildDiplomacyMan->SetLastSendTime(time(NULL));
		}
	}
	else if (strstr(pDlg->GetName(), "GMQueryItem_") == pDlg->GetName())
	{
		CDlgGMQueryItem* pQueryDlg = dynamic_cast<CDlgGMQueryItem*>(GetDialog("Win_GMQueryItem"));
		if(pQueryDlg) pQueryDlg->OnMessageBox(pDlg, iRetVal);
	}
	else if (!stricmp(pDlg->GetName(), "Game_ForceJoin"))
	{
		CDlgForceNPC* pForceDlg = dynamic_cast<CDlgForceNPC*>(GetDialog("Win_ForceJoin"));
		if(pForceDlg) pForceDlg->OnMessageBox(pDlg, iRetVal);
	}
	else if (!stricmp(pDlg->GetName(), "Game_ForceQuit"))
	{
		CDlgForceNPC* pForceDlg = dynamic_cast<CDlgForceNPC*>(GetDialog("Win_ForceQuit"));
		if(pForceDlg) pForceDlg->OnMessageBox(pDlg, iRetVal);
	}
	else if (!stricmp(pDlg->GetName(), "Game_ProduceConfirm"))
	{
		CDlgProduce* pProduce = dynamic_cast<CDlgProduce*>(GetDialog("Win_Produce"));
		if(pProduce) pProduce->OnMessageBox(pDlg, iRetVal);
	}
	else if (!stricmp(pDlg->GetName(), "Game_WebTradeBindConfirm"))
	{
		if (iRetVal == IDYES)
			m_pDlgEquipBind->OnCommandConfirm("MessageBox_Confirm");
	}
	else if (!stricmp(pDlg->GetName(),"GMForbidPlayerRoleByConsume"))
	{
		if(iRetVal == IDOK)
			m_pDlgGmConsoleForbid->ForbidRole(pDlg->GetData());

	}
	else if (!stricmp(pDlg->GetName(),"GMForbidMultiPlayerRoleByConsume"))
	{
		if(iRetVal == IDOK)
			m_pDlgGmConsoleForbid->ForbidMultiRole();

	}
	else if (!stricmp(pDlg->GetName(),"Account_CompleteInfo") && iRetVal == IDOK)
	{
		g_pGame->GetGameSession()->sso_GetTicket(GetAccountTicketContext());
	}
	else if (!stricmp(pDlg->GetName(),"ChallengeConfirm") && iRetVal == IDOK)
	{
		int id = pDlg->GetData();
		int num = (int)pDlg->GetDataPtr("int");
		g_pGame->GetGameSession()->battle_Challenge(id, g_pGame->GetGameRun()->GetHostPlayer()->GetFactionID(), num);
		m_pDlgGMapChallenge->Show(false);
	}
	else if (!stricmp(pDlg->GetName(), "Country_JoinLeave"))
	{
		if (iRetVal == IDYES)
			g_pGame->GetGameSession()->c2s_CmdNPCSevCountryJoinLeave(pDlg->GetData() == 1);
		EndNPCService();
	}
	else if (!stricmp(pDlg->GetName(), "Country_Move"))
	{
		m_pDlgCountryMap->MoveConfirm(iRetVal == IDYES);
	}
	else if (!stricmp(pDlg->GetName(), "Country_LeaveWar"))
	{
		if (iRetVal == IDYES)
			g_pGame->GetGameSession()->c2s_CmdNPCSevCountryLeaveWar();
		EndNPCService();
	}
	else if (!stricmp(pDlg->GetName(), "Country_LeaveWar2"))
	{
		if (iRetVal == IDYES)
			g_pGame->GetGameSession()->c2s_CmdSendCountryLeaveWar();
	}
	else if (!stricmp(pDlg->GetName(),"QuickPayActivateMerchant"))
	{
		if (iRetVal == IDYES)
			m_pDlgQuickPay->ActivateMerchant();
		else
		{
			m_pDlgQuickPay->ChangeState(CDlgQuickPay::STATE_READY);
			m_pDlgQuickPay->EnableItems(true);
		}
	}
	else if (!stricmp(pDlg->GetName(), "EquipMark_Clear") && iRetVal == IDYES)
	{
		m_pDlgEquipMark->OnCommandConfirm("Confirm_Clear");
	}
	else if (!stricmp(pDlg->GetName(), "GT_Install") && iRetVal == IDYES)
	{
		AString strUpdater = GetGTUpdater();
		if (!strUpdater.IsEmpty())
		{
			ShellExecuteA(NULL, "open", strUpdater, NULL, NULL, SW_SHOWNOACTIVATE);
			a_LogOutput(1, "%s started", strUpdater);
			s_GTUpdaterRunned = true;		
			ShowErrorMsg(10065);
		}
	}
	else if (!stricmp(pDlg->GetName(), "CrossServer_GetIn"))
	{
		if (iRetVal == IDYES)
			g_pGame->GetGameRun()->GetPendingLogOut().AppendForSaveConfig(new CECPendingCrossServerGetInOut(true));
		else EndNPCService();
	}
	else if (!stricmp(pDlg->GetName(), "CrossServer_GetOut"))
	{
		if (iRetVal == IDYES)
			g_pGame->GetGameRun()->GetPendingLogOut().AppendForSaveConfig(new CECPendingCrossServerGetInOut(false));
		else EndNPCService();
	}
	else if (!stricmp(pDlg->GetName(),"StoneTransfer") && iRetVal == IDOK)
	{
		m_pDlgStoneTransfer->DoTransfer();
	}
	else if (!stricmp(pDlg->GetName(),"StoneReplace") && iRetVal == IDOK)
	{
		m_pDlgStoneReplace->DoReplace();
	}
	else if (!stricmp(pDlg->GetName(),"Game_NewbieGift") && iRetVal == IDYES)
	{
	  g_pGame->GetGameSession()->sso_GetTicket(GetNewbieGiftTicketContext());
	}
	else if (!stricmp(pDlg->GetName(),"Game_KingAuction") && iRetVal == IDOK)
	{
		int iCount = (int)pDlg->GetData();
		g_pGame->GetGameSession()->king_CandidateAuction(21652, iCount);
	}

	// fatal error C1061: compiler limit : blocks nested too deeply	
	if (!stricmp(pDlg->GetName(),"Game_AssignAssault") && iRetVal == IDOK)
	{
		bool bUseBig = (pDlg->GetData() == 1);
		m_pDlgKingCommand->DoAssignAssault(bUseBig);
	}
	if (!stricmp(pDlg->GetName(),"Game_CancelDomainLimit") && iRetVal == IDOK)
	{
		m_pDlgKingCommand->DoCancelDomainLimit();
	}
	else if (!stricmp(pDlg->GetName(),"Game_PetEvolution") && iRetVal == IDYES)
	{
		int iSel = (int)pDlg->GetData();
		m_pDlgPetEvolution->OnSendEvolutonCmd(iSel);
	}
	else if (!stricmp(pDlg->GetName(), "Game_FashionSplitConfirm"))
	{
		if (iRetVal == IDYES)
			m_pDlgFashionSplit->OnCommandConfirm("MessageBox_Confirm");
	}
	else if (!stricmp(pDlg->GetName(),"OfflineShopClear") && iRetVal == IDYES)
	{
		g_pGame->GetGameSession()->OffShop_ClearMyShop();
	}
	else if (!stricmp(pDlg->GetName(),"OfflineShopItemSell"))
	{
		int* pData = (int*)pDlg->GetDataPtr();
		if(iRetVal == IDOK)
			g_pGame->GetGameSession()->OffShop_BuyItemFrom(pData[0],pData[1],pData[2],pData[3],pData[4],pData[5]);
		a_freetemp(pData);
	}
	else if (!stricmp(pDlg->GetName(),"OfflineShopItemBuy"))
	{
		int* pData = (int*)pDlg->GetDataPtr();
		if(iRetVal == IDOK)
			g_pGame->GetGameSession()->OffShop_SellItemTo(pData[0],pData[1],pData[2],pData[3],(unsigned int)pData[4],pData[5]);
		a_freetemp(pData);
	}
	else if (!stricmp(pDlg->GetName(), "Game_DownloadOK") && iRetVal == IDYES)
	{
		// 直接退出，避免当前处于战斗状态的情况
		CECMCDownload::GetInstance().SendInstall();
	}
	else if (!stricmp(pDlg->GetName(), "Game_StartDownload") && iRetVal == IDYES)
	{
		m_pDlgDownloadSpeed->Show(true);
	}
	else if (!stricmp(pDlg->GetName(), "SwallowGeneralCard"))
	{
		if (iRetVal == IDYES)
			m_pDlgGeneralCard->SendSwallowCardCmd();
	}
	else if(!stricmp(pDlg->GetName(), "GeneralCardRespawn") && iRetVal == IDOK)
	{
		m_pDlgGenCardRespawn->DoRespawn();
	}
	else if (!stricmp(pDlg->GetName(), "ChariotBattle") && iRetVal == IDOK)
	{
		g_pGame->GetGameSession()->c2s_CmdLevaveChariotBattle();
	}
	else if (!stricmp(pDlg->GetName(),"FlySword_Improve") && iRetVal == IDOK)
	{
		m_pDlgFlySwordImprove->DoImprove();
	}

	//自家家园
	/*
	else if( 0 == stricmp(pDlg->GetName(), "Auto_GenLightMap") && iRetVal == IDOK )
	{
		m_HomeDlgsMgr->GetAutoGenLightMapProgress()->Show(true, true);
		m_HomeDlgsMgr->GetAutoGenLightMapProgress()->SetRunLightMap(true);
	}
	*/


	return CECBaseUIMan::OnMessageBox(iRetVal, pDlg);
}

bool CECGameUIMan::OnNewMessageBox(int iRetVal)
{
	if (!stricmp(m_pDlgMessageBox->GetContext(), "Country_EnterWar")){
		if (iRetVal == IDOK &&
			m_pDlgCountryMap->IsOpen() &&
			g_pGame->GetGameSession()->IsConnected())
		{
			int domain_id = m_pDlgMessageBox->GetData();
			g_pGame->GetGameSession()->country_Enter(domain_id);
		}
	}else if(!stricmp(m_pDlgMessageBox->GetContext(), "Game_MailAskFor")){
		m_pDlgGivingFor->DoSendMailGivingFor(IDYES == iRetVal);
	}else if (!stricmp(m_pDlgMessageBox->GetContext(), CECInstanceReenter::GetNotifyContext())){
		CECInstanceReenter::Instance().Reenter(IDYES == iRetVal);
	}
	
	m_pDlgMessageBox->SetContext("");
	m_pDlgMessageBox->Show(false);
	return true;
}

bool CECGameUIMan::Tick(DWORD dwDeltaTime)
{
	bool bval;
	ACHAR szText[128];
	ACString strText;

	ProcessOfflineMsgs();
	UpdateGTWindow();
	UpdateGTTeam();
	UpdateGTFaction();

//	if(!m_pDlgAutoHelp->IsShow())
//		m_pDlgAutoHelp->IncOpenTime(dwDeltaTime);

	if(m_WorldLifeTimeBase > 0 && m_WorldLifeTime > 0)
	{
		int nTime = m_WorldLifeTimeBase - mktime(&g_pGame->GetServerLocalTime());
		if(nTime >=0 && nTime <= m_WorldLifeTime)
		{
			ACString szTxt;
			szTxt.Format(GetStringFromTable(888), GetFormatTime(nTime));
			AddChatMessage(szTxt, GP_CHAT_MISC);
			m_WorldLifeTime = MakeNextCountTime(nTime);
		}
	}
	
	if( !m_bShowAllPanels || !m_bUIEnable)
	{
		if( !m_bUIEnable )
		{
			DWORD dwTotalTime, dwCnt;
			PAUIDIALOG pDlg = GetDialog("Win_EnterWait");
			PAUILABEL pLabel = (PAUILABEL)pDlg->GetDlgItem("Txt_Message");

			if( g_pGame->GetGameSession()->GetOvertimeCnt(dwTotalTime, dwCnt) )
			{
				dwTotalTime /= 1000;
				dwCnt /= 1000;
				strText.Format(GetStringFromTable(516), dwTotalTime, dwCnt);
			}
			else
				strText = GetStringFromTable(515);

			pLabel->SetText(strText);
		}
		else if( !m_bShowAllPanels )
		{
			if( m_pDlgCamera->m_bPrinting )
			{
				m_pDlgCamera->m_nPrintTimes++;
				if( m_pDlgCamera->m_nPrintTimes > 5 )
					m_pDlgCamera->m_bPrinting = false;
				m_bShowAllPanels = true;
			}
		}
		if (m_bUIEnable){
			return CECBaseUIMan::Tick(dwDeltaTime);		//	可用于删除不需要的对话框
		}
		return true;
	}

	if (IsNeedShowReferralHint())
	{
		const GNET::RoleInfo &info = g_pGame->GetGameRun()->GetSelectedRoleInfo();
		const ACHAR *szReferralName = g_pGame->GetGameRun()->GetPlayerName(info.referrer_role, false);
		if (szReferralName)
		{
			// When we already get referral player name
			//
			CECFriendMan *pFriendMan = g_pGame->GetGameRun()->GetHostPlayer()->GetFriendMan();
			if (pFriendMan->CheckInit())
			{
				// When we can check whether referral is our friend
				if (!pFriendMan->GetFriendByID(info.referrer_role))
				{
					// Referral is not our friend
					ACString strReferralHint;
					strReferralHint.Format(GetStringFromTable(7970), szReferralName);
					
					PAUIDIALOG pMsgBox;
					MessageBox("Game_ReferralHint", strReferralHint, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
					pMsgBox->SetData(info.referrer_role);
					
					m_bReferralHintShown = true;
				}
			}
		}
	}

	if( g_pGame->GetGameRun()->GetDoubleExpMode() != CECGameRun::DBEXP_NONE ||
		g_pGame->GetGameRun()->GetWallowInfo().anti_wallow_active &&
		CECUIConfig::Instance().GetGameUI().nWallowHintType != CECUIConfig::GameUI::WHT_KOREA)
	{
		if( !m_pDlgDoubleExpS->IsShow() )
		{
			m_pDlgDoubleExpS->AlignTo(m_pDlgMiniMap,
					AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
					AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP);
			m_pDlgDoubleExpS->Show(true, false, false);

			if (g_pGame->GetGameRun()->GetWallowInfo().anti_wallow_active &&
				CECUIConfig::Instance().GetGameUI().nWallowHintType != CECUIConfig::GameUI::WHT_KOREA)
			{
				int idMsg = g_pGame->GetGameRun()->GetWallowInfo().reason == 0 ? 9341 : 9342;
				AddChatMessage(GetStringFromTable(idMsg), GP_CHAT_BROADCAST);
			}
		}
	}
	else if (m_pDlgDoubleExpS->IsShow()){
		m_pDlgDoubleExpS->Show(false);
	}

	g_pGame->GetGameRun()->ShowAccountLoginInfo();
	g_pGame->GetGameRun()->ShowAccountInfo();

	if( g_pGame->GetGameRun()->GetWallowInfo().anti_wallow_active )
	{
		int stime = g_pGame->GetServerGMTTime();
		int nTime = stime - g_pGame->GetGameRun()->GetWallowInfo().play_time;

		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)m_pDlgDoubleExpS->GetDlgItem("Img_Captivation");
		
		int h = nTime / 3600; //	小时

		if (CECUIConfig::Instance().GetGameUI().nWallowHintType == CECUIConfig::GameUI::WHT_KOREA)
		{			
			// 检查提示框
			PAUIDIALOG pMsgBoxK = GetDialog("WallowHintKOREA");
			if (pMsgBoxK && pMsgBoxK->IsShow())
			{
				int period = g_pGame->GetServerGMTTime() - pMsgBoxK->GetData();
				if(period>=5) // 大于5秒关闭
				{
					pMsgBoxK->Show(false);
					pMsgBoxK->SetData(0);
				}
			}
			//
			if (h>1)
			{
				if((int)pImage->GetData()==0)
					pImage->SetData(h);				
			}
		
			// 到达1小时防沉迷提示
			if ((int)pImage->GetData() != h)
			{
				ACString strWallow;
				strWallow.Format(GetStringFromTable(910),h);
				AddChatMessage(strWallow, GP_CHAT_BROADCAST);
				pImage->SetData(h);

				// 弹出对话框
				strWallow.Format(GetStringFromTable(9630),h);
				MessageBox("WallowHintKOREA",strWallow,MB_NULL,A3DCOLORRGBA(255, 255, 255, 160),&pMsgBoxK);
				pMsgBoxK->SetData(g_pGame->GetServerGMTTime());
			}

		}
		else // 默认方式
		{
			//	悬浮提示
			ACString strTime, strText;
			strTime.Format(GetStringFromTable(1611), nTime / 3600, ((nTime % 3600) / 600) * 10);
			strText.Format(GetStringFromTable(893), strTime);
			pImage->SetHint(strText);

			//	防沉迷图标
			pImage->FixFrame(h<1 ? 0 : (h < 2 ? 1 : 2));

			//	防沉迷提示
			if (h == 1)
			{
				//	到达1小时
				if ((int)pImage->GetData() != h)
				{
					AddChatMessage(GetStringFromTable(890), GP_CHAT_BROADCAST);
					pImage->SetData(h);
				}
			}
			else if (h == 2)
			{
				int m = (nTime%3600) / 60;				//	分钟
				if (m < 50)
				{
					//	到达2小时
					if ((int)pImage->GetData() != h)
					{
						AddChatMessage(GetStringFromTable(891), GP_CHAT_BROADCAST);
						pImage->SetData(h);
					}
				}
				else
				{
					//	到达2小时50分钟
					if ((int)pImage->GetData() != 50)
					{
						AddChatMessage(GetStringFromTable(892), GP_CHAT_BROADCAST);
						pImage->SetData(50);
					}
				}
			}
		}
	}
//	if( g_pGame->GetGameSession()->GetNewbieTime() >= 
//		g_pGame->GetGameSession()->GetAccCreatedTime() )
//	{
//		DWORD dwAccountTime = (DWORD)(
//			(g_pGame->GetGameSession()->GetAccountTime() +
//			g_pGame->GetGameSession()->GetFreeGameTime()) / 1000);
//		bool bTip = false;
//		while( dwAccountTime <= l_dwAccountTimeTip[m_nAccountTimeIndex] &&
//			m_nAccountTimeIndex < CECGAMEUIMAN_ACCOUNTTIME_TIPMAX )
//		{
//			m_nAccountTimeIndex++;
//			bTip = true;
//		}
//		PAUIDIALOG pDlg = GetDialog("Win_AccountTime");
//		if( bTip )
//		{
//			ACString szTip, szTime;
//			int nIndexOld = m_nAccountTimeIndex - 1;
//			if( dwAccountTime > 60 )
//				szTime.Format(GetStringFromTable(1602), l_dwAccountTimeTip[nIndexOld] / 60);
//			else
//				szTime.Format(GetStringFromTable(1601), l_dwAccountTimeTip[nIndexOld]);
//			szTip.Format(GetStringFromTable(696), szTime);
//			if( dwAccountTime > 600 )
//				szTip = _AL("^FFFF00") + szTip;
//			else
//				szTip = _AL("^FF0000") + szTip;
//			pDlg->GetDlgItem("Message")->SetText(szTip);
//			pDlg->SetPosEx(0, GetDialog("Win_HpmpOther")->GetSize().cy + 30, AUIDialog::alignCenter);
//			pDlg->Show(true, false, false);
//			pDlg->SetData(GetTickCount());
//		}
//		else
//			if( pDlg->IsShow() && CECTimeSafeChecker::ElapsedTimeFor(pDlg->GetData()) > 5000 )
//				pDlg->Show(false);
//	}
	RefreshHint();
	RefreshBrokenList();
	RefreshQuickBarName();
	m_pDlgTask->TickTaskTrace();
	m_pMiniBarMgr->ArrangeMinimizeBar(false);
	RefreshItemNotify();
	RemindTrashboxPassword();

	// trying to sync the bshop state
	if(m_pDlgBShop && !CECCrossServer::Instance().IsOnSpecialServer())
		m_pDlgBShop->OnTick();

	bval = m_pDlgTeamMain->UpdateTeamInfo();
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

	bval = m_pDlgQuickBar1->UpdateShortcuts();
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

	char szName[40];
	int nCurChatColor = m_pDlgChat->GetChatColor();
	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(m_pDlgChat->GetDlgItem("Txt_Content"));
	if( m_pDlgMouseOn && 0 == stricmp(m_pDlgMouseOn->GetName(), "Win_Chat") )
	{
		pText->SetOnlyShowText(false);

		sprintf(szName, "Img_Back%d", nCurChatColor + 0);
		m_pDlgChat->GetDlgItem(szName)->Show(false);

		sprintf(szName, "Img_Back%d", nCurChatColor + 1);
		m_pDlgChat->GetDlgItem(szName)->Show(true);
	}
	else if( !m_pDlgActive || !m_pDlgActive->GetCaptureObject() )
	{
		pText->SetOnlyShowText(true);

		sprintf(szName, "Img_Back%d", nCurChatColor + 0);
		m_pDlgChat->GetDlgItem(szName)->Show(true);

		sprintf(szName, "Img_Back%d", nCurChatColor + 1);
		m_pDlgChat->GetDlgItem(szName)->Show(false);
	}

	PAUIDIALOG pDlgTrace = GetDialog("Win_QuestMinion");
	if(pDlgTrace && pDlgTrace->IsShow())
	{
		PAUITEXTAREA pTrace = dynamic_cast<PAUITEXTAREA>(pDlgTrace->GetDlgItem("Txt_Link_Trace"));
		if( pTrace )
		{
			pTrace->SetOnlyShowText(m_pDlgMouseOn != pDlgTrace);
		}
	}

	if( AUIEditBox::IsIMEAlphaNumeric() )
		((PAUIIMAGEPICTURE)m_pDlgChat->GetDlgItem("Input"))->FixFrame(0);
	else
		((PAUIIMAGEPICTURE)m_pDlgChat->GetDlgItem("Input"))->FixFrame(1);
	a_strcpy(szText, AUIEditBox::GetCurrentIMEName());
	m_pDlgChat->GetDlgItem("Txt_1")->SetHint(szText);
	szText[1] = 0;
	m_pDlgChat->GetDlgItem("Txt_1")->SetText(szText);

	bool bMini = ((PAUICHECKBOX)m_pDlgChat2->GetDlgItem("Chk_Mini"))->IsChecked();

	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgChat2->GetDlgItem("Txt_Content"));
	if( m_pDlgMouseOn && 0 == stricmp(m_pDlgMouseOn->GetName(), "Win_ChatSmall") )
	{
		sprintf(szName, "Img_Back%d", nCurChatColor + 0);
		m_pDlgChat2->GetDlgItem(szName)->Show(false);

		if( bMini )
		{
			sprintf(szName, "Img_Back%d", nCurChatColor + 1);
			m_pDlgChat2->GetDlgItem(szName)->Show(false);
		}
		else
		{
			pText->SetOnlyShowText(false);

			sprintf(szName, "Img_Back%d", nCurChatColor + 1);
			m_pDlgChat2->GetDlgItem(szName)->Show(true);
		}
	}
	else if( !m_pDlgActive || !m_pDlgActive->GetCaptureObject() )
	{
		sprintf(szName, "Img_Back%d", nCurChatColor + 1);
		m_pDlgChat2->GetDlgItem(szName)->Show(false);

		if( bMini )
		{
			sprintf(szName, "Img_Back%d", nCurChatColor + 0);
			m_pDlgChat2->GetDlgItem(szName)->Show(false);
		}
		else
		{
			pText->SetOnlyShowText(true);

			sprintf(szName, "Img_Back%d", nCurChatColor + 0);
			m_pDlgChat2->GetDlgItem(szName)->Show(true);
		}
	}
	
	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgChat3->GetDlgItem("Txt_Content"));
	PAUIDIALOG pDlgActive = m_pDlgActive;
	PAUIOBJECT pObjFocus = pDlgActive ? pDlgActive->GetFocus() : NULL;
	if (m_pDlgChat3->IsHaveFarCryMsg())
	{
		if (!m_pDlgChat3->IsShow())
			m_pDlgChat3->Show(true);

		pText->SetOnlyShowText(true);
		
		sprintf(szName, "Img_Back%d", nCurChatColor + 0);
		m_pDlgChat3->GetDlgItem(szName)->Show(true);
		
		sprintf(szName, "Img_Back%d", nCurChatColor + 1);
		m_pDlgChat3->GetDlgItem(szName)->Show(false);
	}
	else if (m_pDlgChat3->IsShow())
	{
		m_pDlgChat3->Show(false);
	}
	if (m_pDlgActive != pDlgActive && pDlgActive)
	{
		// 如果号角频道更改了对话框焦点，恢复之
		BringWindowToTop(pDlgActive);
		pDlgActive->ChangeFocus(pObjFocus);
	}

	if( m_pDlgDragDrop->IsShow() && m_pDlgActive != m_pDlgDragDrop )
	{
		BringWindowToTop(m_pDlgDragDrop);
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		POINT ptPos;
		ptPos = m_pDlgDragDrop->GetPos();
		POINT pt = m_pDlgDragDrop->GetDragPos();
		m_pDlgDragDrop->SetCapture(true, ptPos.x + pt.x, ptPos.y + pt.y);
	}
	
	int idBuddy = g_pGame->GetGameRun()->GetWorld()->GetHostPlayer()->GetBuddyID();
	if( !m_pDlgBuddyState->IsShow() && idBuddy != 0 )
		m_pDlgBuddyState->SetBuddyID(idBuddy);

	PAUIDIALOG pDlg = GetDialog("Win_Popface");
	int nCurSet = g_pGame->GetGameRun()->GetHostPlayer()->GetCurEmotionSet();
	if( pDlg->GetData() != (DWORD)nCurSet )
	{
		pDlg->SetData(nCurSet);

		PAUIEDITBOX pEmotionEdit;
		pEmotionEdit = dynamic_cast<PAUIEDITBOX>(m_pDlgChannelChat->GetDlgItem("Txt_Content"));
		pEmotionEdit->SetCurrentEmotion(nCurSet);
		pEmotionEdit = dynamic_cast<PAUIEDITBOX>(m_pDlgChatWhisper->GetDlgItem("DEFAULT_Txt_Speech"));
		pEmotionEdit->SetCurrentEmotion(nCurSet);
		int i(0);
		for(i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
		{
			sprintf(szName, "Win_FriendChat%d", i);
			PAUIDIALOG pDlgFriend = GetDialog(szName);
			pEmotionEdit = dynamic_cast<PAUIEDITBOX>(pDlgFriend->GetDlgItem("Txt_Content"));
			pEmotionEdit->SetCurrentEmotion(nCurSet);
		}
		PAUIIMAGEPICTURE pImage;
		ACString strHint;
		AUICTranslate trans;
		for( i = 0; ; i++ )
		{
			sprintf(szName, "Face_%02d", i);
			pImage = (PAUIIMAGEPICTURE)pDlg->GetDlgItem(szName);
			if( !pImage ) break;

			if( i < (int)m_vecEmotion[0]->size() )
			{
				pImage->SetCover(m_pA2DSpriteEmotion[nCurSet], (*m_vecEmotion[nCurSet])[i].nStartPos);
				
				strHint = (*m_vecEmotion[nCurSet])[i].strHint;
				strHint = trans.Translate(strHint);
				pImage->SetHint(strHint);
			}
			else
				pImage->ClearCover();
		}
	}

	PAUIDIALOG pDlgPopface1 = GetDialog("Win_Popface01");
	PAUIDIALOG pDlgPopface2 = GetDialog("Win_Popface02");
	PAUIEDITBOX pEmotionEdit = dynamic_cast<PAUIEDITBOX>(m_pDlgChat->GetDlgItem("DEFAULT_Txt_Speech"));	
	if (CDlgChat::IsSuperFarCryMode())
	{
		pEmotionEdit->SetCurrentEmotion(SUPER_FAR_CRY_EMOTION_SET);
	}
	else
	{
		pEmotionEdit->SetCurrentEmotion(nCurSet);
	}
	nCurSet = SUPER_FAR_CRY_EMOTION_SET;
	pDlg = pDlgPopface1;
	if (pDlg->GetData() != (DWORD)nCurSet)
	{
		pDlg->SetData(nCurSet);
		
		PAUIIMAGEPICTURE pImage;
		ACString strHint;
		AUICTranslate trans;
		for(int i = 0; ; i++ )
		{
			sprintf(szName, "Face_%02d", i);
			pImage = (PAUIIMAGEPICTURE)pDlg->GetDlgItem(szName);
			if( !pImage ) break;
			
			if( i < (int)m_vecEmotion[0]->size() )
			{
				pImage->SetCover(m_pA2DSpriteEmotion[nCurSet], (*m_vecEmotion[nCurSet])[i].nStartPos);
				
				strHint = (*m_vecEmotion[nCurSet])[i].strHint;
				strHint = trans.Translate(strHint);
				pImage->SetHint(strHint);
			}
			else
				pImage->ClearCover();
		}
	}
	
	// trying to lock the item in booth pack
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	AUIDialog* pPShop1 = GetDialog("Win_PShop1");
	if( pPShop1 && pPShop1->IsShow() &&
		pHost && (pHost->GetBoothState() == 2 || pHost->GetBoothState() == 1))
	{
		CECInventory* pPack = pHost->GetPack();
		CECDealInventory *pBoothBuyPack = pHost->GetBoothBuyPack();
		CECDealInventory *pBoothSellPack = pHost->GetBoothSellPack();
		for(int slot = 0; slot < pPack->GetSize(); ++slot)
		{
			CECIvtrItem* pItem = pPack->GetItem(slot);
			if(pItem)
			{
				int tid = pItem->GetTemplateID();
				if(pBoothBuyPack->ValidateItem(slot, tid) || pBoothSellPack->ValidateItem(slot, tid))
				{
					pItem->Freeze(true);
					continue;
				}
			}
		}
	}

	// force tick the guild dlg to show message
	CDlgGuildMap* pGuildMap = (CDlgGuildMap*)GetDialog("Win_GuildMap");
	pGuildMap->ShowCachedMessage();

	// force to tick the congregate dialog to show message
	for(int conID = 0 ; ; conID++)
	{
		AString conName;
		PAUIDIALOG pCongregate = GetDialog(conName.Format("Win_Congregate%d", conID));
		if(!pCongregate) break;
		if(!pCongregate->IsShow()) pCongregate->Tick();
	}

	// show icon in PENGLAI map
	PAUIDIALOG pForceIcon = GetDialog("Win_ForceActivityIcon");
	if(pForceIcon)
	{
		//	暂时不显示 Win_ForceActivityIcon 及 Win_ForceActivity，以避免玩家查看到势力活跃度相关数据
		if (pForceIcon->IsShow())
			pForceIcon->Show(false);

		//if(137 == g_pGame->GetGameRun()->GetWorld()->GetInstanceID())
		//{
		//	if(!pForceIcon->IsShow()) pForceIcon->Show(true);
		//}
		//else
		//{
		//	if(pForceIcon->IsShow()) pForceIcon->Show(false);
		//}
	}

	// 挂机岛
	if (g_pGame->GetGameRun()->GetHostPlayer()->GetBasicProps().iLevel > 30 && m_pDlgOnlineAward)
		m_pDlgOnlineAward->SwitchGuajiIsland();
	
	//	国战进入战场倒计时
	m_pDlgCountryMap->UpdateEnterWarCountDown();
	
	// 自动任务奖励宝箱
	m_pDlgAutoTask->UpdateShow();

	// 屏幕特效
	m_pScreenEffectMan->Tick();

	PAUIDIALOG pDlgConsole = GetDialog("Dlg_Console");
	if (pDlgConsole->IsShow()){
		BringWindowToTop(pDlgConsole);
	}
	return CECBaseUIMan::Tick(dwDeltaTime);
}

bool CECGameUIMan::Render(void)
{
	bool bval;

	g_pGame->GetGameRun()->GetWorld()->GetHostPlayer()->RenderForUI(g_pGame->GetViewport());

	if( !m_bUIEnable || !m_bShowAllPanels )
	{
		if( !m_bUIEnable )
		{
			PAUIDIALOG pDlg;

			pDlg = GetDialog("Win_EnterWait");
			if (!pDlg->IsShow()){
				pDlg->Show(true);
			}
			pDlg->Render();

			if (pDlg = GetDialog("MsgBox_LinkBroken")){
				pDlg->Render();
			}
			if (pDlg = GetDialog("MsgBox_RemindReconnect")){
				pDlg->Render();
			}
		}
		return true;
	}
	else
	{
		PAUIDIALOG pDlg = GetDialog("Win_EnterWait");
		if( pDlg->GetData() > 0 )
		{
			ACString strMsg;
			int nTime = g_pGame->GetGameRun()->GetWorld()
				->GetHostPlayer()->GetTimeToExitInstance();

			pDlg->SetData(nTime);
			strMsg.Format(GetStringFromTable(599), nTime / 1000);
			pDlg->GetDlgItem("Txt_Message")->SetText(strMsg);
		}
		else if( pDlg->IsShow() && pDlg->GetData() == 0 )
			pDlg->Show(false);
	}

	m_pDlgTarget->RefreshTargetStat();

	bval = UpdateHint();
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

	bval = UpdateArrowInfo();
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

	int i;
	ALISTPOSITION posMiniMap, posMark, posQShop;
	posMiniMap = m_DlgZOrder.Find(m_pDlgMiniMap);
	CDlgQShop *pShop = m_pDlgQShop->IsShow() ? m_pDlgQShop : m_pDlgBackShop;
	if (pShop->IsShow())
	{
		posQShop = m_DlgZOrder.Find(pShop);
		for(i = 0; i < CECGAMEUIMAN_MAX_QSHOPITEMS; i++)
		{
			if( m_pDlgQShopDefault[i]->IsShow() )
			{
				posMark = m_DlgZOrder.Find(m_pDlgQShopDefault[i]);
				m_DlgZOrder.RemoveAt(posMark);
				m_DlgZOrder.InsertBefore(posQShop, m_pDlgQShopDefault[i]);
			}
		}
	}
	
	if (m_pDlgFashionShop->IsShow()){
		ALISTPOSITION posShop = m_DlgZOrder.Find(m_pDlgFashionShop);
		//	将显示的 CDlgPalette2 位置调整到 CDlgFashionShop 对话框之前
		if (m_pDlgPalette2->IsShow()){
			ALISTPOSITION posPalette2 = m_DlgZOrder.Find(m_pDlgPalette2);
			m_DlgZOrder.RemoveAt(posPalette2);
			m_DlgZOrder.InsertBefore(posShop, m_pDlgPalette2);
		}
		//	将显示的 CDlgFashionShopItem 位置调整到 CDlgFashionShop 对话框之前
		for(int i = 0; i < FASHION_SHOP_ITEM_COUNT; i++){
			CDlgFashionShopItem *pDlgFashionShopItem = m_pDlgFashionShopItem[i];
			if (pDlgFashionShopItem->IsShow()){
				ALISTPOSITION posShopItem = m_DlgZOrder.Find(pDlgFashionShopItem);
				m_DlgZOrder.RemoveAt(posShopItem);
				m_DlgZOrder.InsertBefore(posShop, pDlgFashionShopItem);
			}
		}
	}
	
	if (m_pDlgSkillAction->IsShow() && m_pDlgSkillEdit->IsShow()) {
		ALISTPOSITION posSkill;
		ALISTPOSITION posSkillEdit = m_DlgZOrder.Find(m_pDlgSkillEdit);
	
		if (m_pDlgSkillSubOther->IsShow()) {
			posSkill = m_DlgZOrder.Find(m_pDlgSkillSubOther);
		} else if (m_pDlgSkillSubTreeBase->IsShow()) {
			posSkill = m_DlgZOrder.Find(m_pDlgSkillSubTreeBase);
		} else if (m_pDlgSkillSubTreeEvil->IsShow()) {
			posSkill = m_DlgZOrder.Find(m_pDlgSkillSubTreeEvil);
		} else if (m_pDlgSkillSubTreeGod->IsShow()) {
			posSkill = m_DlgZOrder.Find(m_pDlgSkillSubTreeGod);
		}
		
		
		m_DlgZOrder.RemoveAt(posSkillEdit);
		m_DlgZOrder.InsertBefore(posSkill, m_pDlgSkillEdit);
	}
	
	// Tip框激活时显示在最上层，平时显示在最下层
	if (!m_pDlgScriptTip->IsFree()) {
		ALISTPOSITION posScriptTip = m_DlgZOrder.Find(m_pDlgScriptTip);
		m_DlgZOrder.RemoveAt(posScriptTip);
		m_DlgZOrder.AddHead(m_pDlgScriptTip);
	} else {
		ALISTPOSITION posScriptTip = m_DlgZOrder.Find(m_pDlgScriptTip);
		m_DlgZOrder.RemoveAt(posScriptTip);
		m_DlgZOrder.AddTail(m_pDlgScriptTip);
	}

	// 染色纯色提示放在最上层
	if (m_pDlgPureColorFashionTip->IsShow()) {
		ALISTPOSITION posPureColorFashionTip = m_DlgZOrder.Find(m_pDlgPureColorFashionTip);
		m_DlgZOrder.RemoveAt(posPureColorFashionTip);
		m_DlgZOrder.AddHead(m_pDlgPureColorFashionTip);
	}

	// 红色边框闪烁特效放到顶层
	if (m_pDlgRedSpark->IsShow()) {
		ALISTPOSITION posRedSpark = m_DlgZOrder.Find(m_pDlgRedSpark);
		m_DlgZOrder.RemoveAt(posRedSpark);
		m_DlgZOrder.AddHead(m_pDlgRedSpark);
	}

	// 黑屏特效放到顶层
	if (m_pDlgBlackScreen->IsShow()) {
		ALISTPOSITION posBlackScreen = m_DlgZOrder.Find(m_pDlgBlackScreen);
		m_DlgZOrder.RemoveAt(posBlackScreen);
		m_DlgZOrder.AddHead(m_pDlgBlackScreen);
	}
	

	bval = CECBaseUIMan::Render();
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

	return true;
}

struct BroadCastItemsFilter : public CECGameUIMan::AUI_EditboxItemFilter
{
	bool operator()(EditBoxItemBase *pItem)const
	{
		// 特殊符号中、不允许物品存在
		//
		return (pItem->GetType() == enumEIIvtrlItem) || (pItem->GetType() == enumEICoord);
	}
};

void CECGameUIMan::AddChatMessage(const ACHAR *pszMsg,
	char cChannel, int idPlayer, const ACHAR *pszPlayer, char byFlag, char cEmotion, CECIvtrItem *pItem,const ACHAR *pszMsgOrigion)
{
	auto_delete<CECIvtrItem> tmp(pItem);

	bool bIsKing = false;
	if( cChannel == GP_CHAT_COUNTRY && (cEmotion & 0x80) )
	{
		cEmotion &= ~0x80;
		bIsKing = true;
	}

	//	修正表情
	ACString strModified = FilterEmotionSet(pszMsg, cEmotion);
	
	//	修正给GM的额外信息
	ACString	GM_MsgShow;
	ACHAR	GM_MsgType(0);
	bool		GM_MsgValid(false);
	if (g_pGame->GetGameRun()->GetHostPlayer()->IsGM() &&
		GP_CHAT_WHISPER == cChannel)
	{
		int nSymbolPos = strModified.Find(GM_HELP_MSG_TYPE_BASE);
		if (nSymbolPos >= 0 && nSymbolPos+1 < strModified.GetLength())
		{
			ACHAR type = strModified[nSymbolPos + 1];
			if (TheGMHelpMsgArray::Instance()->FindByType(type, NULL))
			{
				GM_MsgValid = true;
				GM_MsgType = type;
				GM_MsgShow = strModified;
				GM_MsgShow.CutLeft(nSymbolPos + 2);

				strModified = strModified.Left(nSymbolPos) + strModified.Right(strModified.GetLength() - (nSymbolPos+2));
			}
		}
	}

	//	考虑本地化对某些内容不显示的要求，当ingame.stf找不到相关字符串、并导致此处字符中为空时，隐藏显示
	if (strModified.IsEmpty())
		return;

	//	标明来自GT频道的消息
	if (byFlag == CHANNEL_GAMETALK)
		strModified += GetStringFromTable(9312);

	pszMsg = strModified;

	if( PlayerIsBlack(idPlayer) )
		return;
	if( cChannel == GP_CHAT_SYSTEM && a_stricmp(pszMsg, GetStringFromTable(809)) == 0 )
		return;

	if( byFlag == CHANNEL_FRIEND || byFlag == CHANNEL_FRIEND_RE || byFlag == CHANNEL_GAMETALK)
	{
		AddFriendMessage(pszMsg, idPlayer, pszPlayer, byFlag, cEmotion, pItem ? pItem->Clone() : NULL,pszMsgOrigion);
		return;
	}
	else if( byFlag == CHANNEL_USERINFO )
	{
		if( pszMsg[0] == 'R' )
		{
			CECFriendMan *pMan = g_pGame->GetGameRun()->GetHostPlayer()->GetFriendMan();
			if (!pMan)
				return;
			if( pMan->GetFriendByID(idPlayer) )
				FriendAction(idPlayer, -1, CDlgFriendList::FRIEND_ACTION_INFO_REFRESH, 0);
		}
		else if( pszMsg[0] == 'L' )
			FriendAction(idPlayer, -1, CDlgFriendList::FRIEND_ACTION_INFO_LEVEL, a_atoi(pszMsg + 1));
		else if( pszMsg[0] == 'A' )
			FriendAction(idPlayer, -1, CDlgFriendList::FRIEND_ACTION_INFO_AREA, a_atoi(pszMsg + 1));
		return;
	}

	ACHAR *pszText;
	CDlgChat::CHAT_MSG msg;
	ACHAR szText[512], szMsg[512];
	CDlgChat *pChat1 = m_pDlgChat;
	CDlgChat *pChat2 = m_pDlgChat2;
	CDlgChat *pChat3 = m_pDlgChat3;
	EC_GAME_SETTING gs = g_pGame->GetConfigs()->GetGameSettings();
	PAUITEXTAREA pShow1 = dynamic_cast<PAUITEXTAREA>(pChat1->GetDlgItem("Txt_Content"));
	PAUITEXTAREA pShow2 = dynamic_cast<PAUITEXTAREA>(pChat2->GetDlgItem("Txt_Content"));
	PAUITEXTAREA pShow3 = dynamic_cast<PAUITEXTAREA>(pChat3->GetDlgItem("Txt_Content"));
	abase::vector<CDlgChat::CHAT_MSG> &vecChatMsg = m_pDlgChat->GetAllChatMsgs();
	abase::vector<CDlgChat::CHAT_MSG> &superFarCryMsg = m_pDlgChat->GetSuperFarCryMsgs();
	abase::vector<CDlgChat::LINKED_MSG> &whisperMsg = m_pDlgChat->GetWhisperChatMsgs();

	msg.idPlayer = idPlayer;
	msg.strMsg = pszMsg;
	msg.cChannel = cChannel;
	msg.cEmotion = cEmotion;
	msg.strMsgOrigion = pszMsgOrigion;
	if( cChannel == GP_CHAT_LOCAL ||
		cChannel == GP_CHAT_FARCRY ||
		cChannel == GP_CHAT_TEAM ||
		cChannel == GP_CHAT_FACTION ||
		cChannel == GP_CHAT_WHISPER ||
		cChannel == GP_CHAT_TRADE ||
		cChannel == GP_CHAT_SUPERFARCRY ||
		cChannel == GP_CHAT_BATTLE ||
		cChannel == GP_CHAT_COUNTRY)
	{
		if (ISPLAYERID(idPlayer)){
			g_pGame->GetGameRun()->GetUIManager()->FilterBadWords(msg.strMsg);
		}
	}

	int nMsgLen = a_strlen(pszMsg);
	// Booth Message
	if( cChannel == GP_CHAT_WHISPER && 
		pszMsg[nMsgLen - 2] == '!' &&
		pszMsg[nMsgLen - 1] == '#' )
	{
		if( m_pDlgBooth1->IsShow() )
			m_pDlgBooth1->AddBoothMessage(pszMsg);
		return;
	}
	
	ACString strName;
	A3DCOLOR clrName;
	TransformNameColor(pItem, strName, clrName);	
	AWString msgWithColor = _AL("");
	if( ISNPCID(idPlayer) )
		msgWithColor += _AL("^C8FF64");
	else
	{
		if( cChannel == GP_CHAT_COUNTRY && bIsKing )
			msgWithColor += CDlgChat::m_pszKingColor;
		else
			msgWithColor += CDlgChat::GetChatColor(cChannel, idPlayer);
	}

	msgWithColor += GetChatChannelImage(cChannel);
	if( cChannel == GP_CHAT_COUNTRY && bIsKing )
		msgWithColor += GetStringFromTable(10310);
	msgWithColor += msg.strMsg;

	if( cChannel == GP_CHAT_WHISPER &&
		(int)m_pDlgChatWhisper->GetData() == idPlayer )
	{
		pShow1 = dynamic_cast<PAUITEXTAREA>(m_pDlgChatWhisper->GetDlgItem("Txt_Chat"));
		pszText = (ACHAR *)pShow1->GetText();

		if( a_strlen(pShow1->GetText()) > 0 )
			pShow1->AppendText(_AL("\r"));

		pShow1->AppendText(msgWithColor, pItem ? CDlgChat::m_nMsgIndex : 0, strName, clrName);

		if( !m_pDlgChat->IsLocked() )
		{
			pShow1->ScrollToTop();
			pShow1->ScrollToBottom();
		}
		if(	!m_pDlgChatWhisper->IsShow() )
			m_pMiniBarMgr->FlashDialog(m_pDlgChatWhisper);

		// 将可点击的消息，保存起来供点击查询
		if (pItem)
		{
			CDlgChat::LINKED_MSG linkedMsg;
			linkedMsg.pItem = pItem;
			tmp._ptr = NULL;
			linkedMsg.nMsgIndex = CDlgChat::m_nMsgIndex++;
			
			whisperMsg.push_back(linkedMsg);
			if( (int)whisperMsg.size() >= CECGAMEUIMAN_MAX_MSGS )
			{
				CDlgChat::LINKED_MSG &msgDelete = *(whisperMsg.begin());
				delete msgDelete.pItem;
				msgDelete.pItem = NULL;
				whisperMsg.erase(whisperMsg.begin());
			}
		}		
	}
	else
	{
		msg.pItem = pItem;
		msg.nMsgIndex = CDlgChat::m_nMsgIndex++;
		tmp._ptr = NULL;
		msg.strMsg = msgWithColor;
		
		int nActiveChannelSet = m_pDlgChat->GetActiveChannelSet();
		if( gs.bChannel[nActiveChannelSet][cChannel] )
		{
			pszText = (ACHAR *)pShow1->GetText();
			
			/*
			if( m_pDlgChat->GetMsgCount(1) >= CECGAMEUIMAN_MAX_MSGS )
			{
			pch = a_strstr(pszText, _AL("\r"));
			if( pch )
			{
			ACString strText = pszText;
			strText.CutLeft(pch - pszText + 1);
			pShow1->SetText(strText);
			}
			}
			*/
			
			if( glb_IsTextNotEmpty(pShow1))
				pShow1->AppendText(_AL("\r"));
			
			pShow1->AppendText(msgWithColor, msg.nMsgIndex, strName, clrName);
			
			if( !pChat1->IsLocked() )
			{
				pShow1->ScrollToTop();
				pShow1->ScrollToBottom();
			}
			
			if( pChat1->IsLocked() )
				pChat1->GetDlgItem("Btn_Lock")->BlinkText(true);
			pChat1->IncreaseMsgCount(1);
		}
		
		if( gs.bChannel[EC_USERCHANNEL_NUM - 1][cChannel] )
		{
			pszText = (ACHAR *)pShow2->GetText();
			
			/*
			if( m_pDlgChat->GetMsgCount(2) >= CECGAMEUIMAN_MAX_MSGS )
			{
			pch = a_strstr(pszText, _AL("\r"));
			if( pch )
			{
			ACString strText = pszText;
			strText.CutLeft(pch - pszText + 1);
			pShow2->SetText(strText);
			}
			}
			*/
			
			if( glb_IsTextNotEmpty(pShow2))
				pShow2->AppendText(_AL("\r"));
			
			pShow2->AppendText(msgWithColor, msg.nMsgIndex, strName, clrName);
			
			if( !pChat2->IsLocked() )
			{
				pShow2->ScrollToTop();
				pShow2->ScrollToBottom();
			}
			
			pChat2->IncreaseMsgCount(2);
		}

		if (cChannel == GP_CHAT_SUPERFARCRY)
		{
			// 添加到号角消息专用列表，用于单独的显示控制
			//
			CDlgChat::CHAT_MSG msg2 = msg;
			if (msg.pItem){
				msg2.pItem = msg.pItem->Clone();
			}
			superFarCryMsg.push_back(msg2);
			if( (int)superFarCryMsg.size() >= CECGAMEUIMAN_MAX_MSGS )
			{
				CDlgChat::CHAT_MSG &msgDelete = *(superFarCryMsg.begin());
				delete msgDelete.pItem;
				msgDelete.pItem = NULL;
				superFarCryMsg.erase(superFarCryMsg.begin());
			}

			// 号角消息也添加到其它频道
		}
		
		vecChatMsg.push_back(msg);
		if( (int)vecChatMsg.size() >= CECGAMEUIMAN_MAX_MSGS )
		{
			CDlgChat::CHAT_MSG &msgDelete = *(vecChatMsg.begin());
			delete msgDelete.pItem;
			msgDelete.pItem = NULL;
			vecChatMsg.erase(vecChatMsg.begin());
		}
	}

	if( cChannel == GP_CHAT_BROADCAST && byFlag == 1 )
	{
		if( idPlayer == 2 || idPlayer == 3 )
			m_pDlgGuildMap->SetMapStatus(false);
		if( idPlayer == 1 )
			m_pDlgGuildMap->SetBattleChallengeOpen(true);
		if( idPlayer == 2 )
			m_pDlgGuildMap->SetBattleChallengeOpen(false);
	}
	if( cChannel == GP_CHAT_BROADCAST && byFlag == 0 )
	{
		ACString strConverted = AUI_FilterEditboxItem(pszMsg, BroadCastItemsFilter());
		SetMarqueeMsg(strConverted);
	}
	else if( gs.bAutoReply && a_strlen(gs.szAutoReply) > 0 &&
		cChannel == GP_CHAT_WHISPER && idPlayer > 0 && pszPlayer 
		&& byFlag != CHANNEL_NORMALRE)
	{
		CECGameSession *pSession = g_pGame->GetGameSession();
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();

		AUI_ConvertChatString(gs.szAutoReply, szText, false);
		GNET::PRIVATE_CHANNEL channel = CECGameTalk::Instance().GetChannelToSend(idPlayer, true, true);
		pSession->SendPrivateChatData(pszPlayer, szText, channel, idPlayer);
		a_sprintf(szMsg, GetStringFromTable(264), pHost->GetName(), szText);
		
		AddChatMessage(szMsg, GP_CHAT_WHISPER, idPlayer);
	}	
	
	if (g_pGame->GetGameRun()->GetHostPlayer()->IsGM() &&
		GP_CHAT_WHISPER == cChannel)
	{
		// get original msg, 
		// in format : "&[playerName]& 悄悄对你说：[startSymbol][type][msg]"
		if (GM_MsgValid)
		{
			// get playerid
			int nPlayerID = idPlayer;
			if (nPlayerID <= 0)
			{
				if (!pszPlayer) return;
				
				CECGameRun *pGameRun = g_pGame->GetGameRun();
				nPlayerID = pGameRun->GetPlayerID(pszPlayer);
				if (nPlayerID <= 0) return;
			}
			
			// put to gmconsole dialog
			m_pDlgGMConsole->AddUserMessage(GM_MsgShow, GM_MsgType, nPlayerID);
		}
	}
}

void CECGameUIMan::AddFriendMessage(const ACHAR *pszMsg,
	int idPlayer, const ACHAR *pszPlayer, char byFlag, char cEmotion, CECIvtrItem *pItem,const ACHAR *pszMsgOrigion)
{
	int i;
	char szName[40];
	PAUIDIALOG pDlg;
	ACHAR szText[80];
	ACHAR szMsg[256];
	bool bWindow = false;

	auto_delete<CECIvtrItem> tmp(pItem);

	CECFriendMan *pMan = g_pGame->GetGameRun()->GetHostPlayer()->GetFriendMan();
	if (!pMan)
	{
		// 处理 FriendMan 还未初始化时即收到 delivery 发送的 Friend 消息
		return;
	}

	AUI_ConvertChatString(pszPlayer, szText);
	AUI_ConvertChatString(pszMsg, szMsg);

	for( i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
	{
		sprintf(szName, "Win_FriendChat%d", i);
		pDlg = GetDialog(szName);
		if( (int)pDlg->GetData() == idPlayer )
		{
			CDlgFriendChat * pDlgFriendChat = static_cast<CDlgFriendChat *>(pDlg);
			pDlgFriendChat->AddMessageToDisplay(idPlayer, pszPlayer, pszMsg, pItem);
			bWindow = true;
			CECFriendMan::FRIEND *pFriend = pMan->GetFriendByID(idPlayer);
			if( pFriend )
			{
				if( pFriend->nLevel < 0 && pFriend->IsGameOnline() )
				{
					g_pGame->GetGameSession()->SendPrivateChatData(szText,
						_AL("R"), GNET::CHANNEL_USERINFO, idPlayer);
				}
			}
			if( !pDlg->IsShow() )
				m_pMiniBarMgr->FlashDialog(pDlg);

			break;
		}
	}

	if( !bWindow && (byFlag == CHANNEL_FRIEND || byFlag == CHANNEL_GAMETALK))
	{
		CECFriendMan::FRIEND *pFriend = pMan->GetFriendByID(idPlayer);
		if( pFriend )
		{
			if( pFriend->nLevel < 0 && pFriend->IsGameOnline() )
			{
				g_pGame->GetGameSession()->SendPrivateChatData(szText,
					_AL("R"), GNET::CHANNEL_USERINFO, idPlayer);
			}
			AddInformation(CDlgInfo::INFO_FRIEND, "Game_FriendMsg", szMsg, 0xFFFFFFF, idPlayer, 0, (DWORD)(pItem ? pItem->Clone() : NULL));
		}
		else
		{
			//	单向好友发来的好友消息、或GT消息，转发到密语频道

			CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();
			ACString str;
			ACHAR szText[80], szText1[1024];
			AUI_ConvertChatString(pszPlayer, szText);
			AUI_ConvertChatString(pszMsg, szText1, false);
			str.Format(pStrTab->GetWideString(FIXMSG_PRIVATECHAT1), szText, szText1);
		
			//	聊天频道客户端在 OnPrtcPrivateChat 协议中已记录，此处可借用游戏客户端密语频道复用显示代码
			AddChatMessage(str, GP_CHAT_WHISPER, idPlayer, pszPlayer, CHANNEL_NORMAL, 0, pItem ? pItem->Clone() : NULL,pszMsgOrigion);
			return;
		}
	}

	// Auto reply.
	if( m_bAutoReply && (byFlag == CHANNEL_FRIEND || byFlag == CHANNEL_GAMETALK))
	{
		ACString strReply = GetStringFromTable(560);
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();

		if( m_bSaveHistory )
			AddHistory(pHost->GetCharacterID(), time(NULL), pHost->GetName(), strReply);
		GNET::PRIVATE_CHANNEL channel = CECGameTalk::Instance().GetChannelToSend(idPlayer, true, false);
		g_pGame->GetGameSession()->SendPrivateChatData(pszPlayer, strReply, channel, idPlayer);
		AddFriendMessage(strReply, idPlayer, pHost->GetName(), CHANNEL_FRIEND_RE);
	}
}

void CECGameUIMan::AddSysAuctionMessage(int idType, const void* pBuf, size_t sz)
{
	if(m_pDlgBShop)
	{
		m_pDlgBShop->OnSysAuctionMessage(idType, pBuf, sz);
	}
}

void CECGameUIMan::PopupPlayerContextMenu(int idPlayer, int x, int y)
{
	PAUIDIALOG pDlg = NULL;
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();

	idPlayer = idPlayer > 0 ? idPlayer : pHost->GetClickedMan();

	if( idPlayer > 0 && ISNPCID(idPlayer) )
	{
		pDlg = m_pDlgNPC;
		if( !pDlg->IsShow() ) pDlg->Show(true);
	}
	else if( pHost->GetCharacterID() != idPlayer )
	{
		pDlg = m_pDlgQuickAction;
		pDlg->SetData(idPlayer);
		pDlg->Show(true);
		BringWindowToTop(pDlg);
	}

	// old : if( pDlg ) pDlg->SetPos(x, y);
	if( pDlg ) pDlg->SetPosEx(x, y);
}

static inline int MakeNextCountTime(int nTime)
{
	const int MIN_WORLDLIFE_TIME = 60 * 5;
	if(nTime <= MIN_WORLDLIFE_TIME)
	{
		return 0;
	}

	const int MIN_WORLDLIFE_UNIT = 60 * 15;
	int scale = ((nTime - 1) / MIN_WORLDLIFE_UNIT);
	int bit = 0;
	while(scale > 0)
	{
		scale = scale >> 1;
		bit++;
	}

	return bit ? (1 << (bit-1)) * MIN_WORLDLIFE_UNIT : MIN_WORLDLIFE_TIME;
}

void CECGameUIMan::PopupWorldCountDown(int nTime)
{
	if(nTime < 0)
	{
		m_WorldLifeTime = 0;
		m_WorldLifeTimeBase = 0;
	} else {
		ACString szTxt;
		szTxt.Format(GetStringFromTable(888), GetFormatTime(nTime));
		AddChatMessage(szTxt, GP_CHAT_MISC);
		m_WorldLifeTimeBase = nTime + mktime(&g_pGame->GetServerLocalTime());

		// only send notification more than 5 mins
		m_WorldLifeTime = MakeNextCountTime(nTime);
	}
}

//	Add text string to console window
void CECGameUIMan::AddConsoleLine(const ACHAR* szText, A3DCOLOR col)
{
	AUIDialog* pDlg = GetDialog(l_aDlgNames[DLG_CONSOLE]);
	if (!pDlg)
		return;

	AUIConsole* pConsole = (AUIConsole*)pDlg->GetDlgItem("Command_Edit");
	if (!pConsole)
		return;

	pConsole->AddLine(szText, col);
}

bool CECGameUIMan::UpdateTask(unsigned long idTask, int reason)
{
	CDlgTaskTrace* pDlg = dynamic_cast<CDlgTaskTrace*>(GetDialog("Win_QuestMinion"));
	if (pDlg) {
		pDlg->SetBtnUnTraceY(-1, 0);
		pDlg->UpdateContributionTask();
		if (reason == TASK_SVR_NOTIFY_NEW)
			pDlg->OnTaskNew(idTask);
	} 

	// 更新答题任务界面
	if (reason == TASK_SVR_NOTIFY_NEW)
	{
		m_pDlgQuestionTask->AddQuestionTask(idTask);
	}
	else if (reason == TASK_SVR_NOTIFY_COMPLETE || reason == TASK_SVR_NOTIFY_GIVE_UP)
	{
		m_pDlgQuestionTask->RemoveQuestionTask(idTask);
	}

	if(reason == TASK_SVR_NOTIFY_STORAGE)
	{
		CDlgTaskList* pDlg = (CDlgTaskList*)GetDialog("Win_QuestList");
		if (pDlg && pDlg->IsShow())
		{
			// refresh data in OnShow()
			pDlg->RefreshTaskList();
		}
		return true;
	}
	else
	{
		// zhangyitian 20140521
		// 任务更新时，可接任务列表也要更新，修正了可接任务列表不更新的问题
		return m_pDlgTask->UpdateQuestView();
	}
}

void CECGameUIMan::PopupInviteGroupMenu(int idLeader, const ACHAR* szLeaderName, 
							int iLeaderProf, int iTeamSeq, int nMode)
{
	EC_GAME_SETTING m_setting = g_pGame->GetConfigs()->GetGameSettings();
	if( !ISPLAYERID(idLeader) || PlayerIsBlack(idLeader) || m_setting.bNoTeamRequest )
		return;

	ACString strMsg;
	strMsg.Format(GetStringFromTable(204), szLeaderName,
		g_pGame->GetGameRun()->GetProfName(iLeaderProf),
		GetStringFromTable(208 + nMode));
	AddInformation(CDlgInfo::INFO_SYSTEM, "Game_InviteGroup", strMsg, 20000, idLeader, iTeamSeq, 0);
	g_pGame->GetGameRun()->GetHostPlayer()->AddRelatedPlayer(idLeader);
}

void CECGameUIMan::PopupTradeInviteMenu(int idPlayer, DWORD dwHandle)
{
	EC_GAME_SETTING m_setting = g_pGame->GetConfigs()->GetGameSettings();
	if( PlayerIsBlack(idPlayer) || m_setting.bNoTradeRequest )
		return;

	if (g_pGame->GetGameRun()->GetHostPlayer()->IsInvisible())
		return;

	ACHAR szName[40];
	ACString strMsg;
	const ACHAR *pszName = g_pGame->GetGameRun()->GetPlayerName(idPlayer, true);

	AUI_ConvertChatString(pszName, szName);
	strMsg.Format(GetStringFromTable(213), szName);
	AddInformation(CDlgInfo::INFO_SYSTEM, "Game_InviteTrade",
		strMsg, 30000, idPlayer, dwHandle, 0);
	g_pGame->GetGameRun()->GetHostPlayer()->AddRelatedPlayer(idPlayer);
}

void CECGameUIMan::PopupTradeResponseMenu(int idPlayer, int idTrade, bool bAgree)
{
	if( bAgree )
	{
		if (g_pGame->GetGameRun()->GetHostPlayer()->IsInvisible())
			return;

		m_pDlgTrade->Show(true);
		m_pDlgTrade->SetData(idPlayer);
		m_pDlgTrade->SetDataPtr((void *)idTrade,"int");
		m_pDlgTrade->m_pBtnLock->Show(true);
		m_pDlgTrade->m_pBtnLock->Enable(true);
		m_pDlgTrade->m_pBtnLock2->Show(true);
		m_pDlgTrade->m_pBtnLock2->Enable(true);
		m_pDlgTrade->m_pBtnConfirm->Show(false);
		m_pDlgTrade->m_pBtnConfirm2->Show(false);

		m_pDlgInventory->Show(true);

		m_idTrade = idTrade;
	}
	else
	{
		if( idPlayer > 0 )
		{
			ACString strMsg;
			const ACHAR *pszName = g_pGame->GetGameRun()->GetPlayerName(idPlayer, true);

			strMsg.Format(GetStringFromTable(214), pszName);
			AddChatMessage(strMsg, GP_CHAT_MISC);
		}
		else
			AddChatMessage(GetStringFromTable(221), GP_CHAT_MISC);
	}
}

void CECGameUIMan::PopupFriendInviteMenu(
	int idPlayer, const ACHAR *pszName, DWORD dwHandle)
{
	if( PlayerIsBlack(idPlayer) )
		return;

	
	ACString strMsg;

	strMsg.Format(GetStringFromTable(534), pszName);
	AddInformation(CDlgInfo::INFO_SYSTEM, "Game_InviteFriend", strMsg, 20000, idPlayer, dwHandle, 0);
	g_pGame->GetGameRun()->GetHostPlayer()->AddRelatedPlayer(idPlayer);
}

void CECGameUIMan::TradeAction(int idPlayer, int idTrade, int idAction, int nCode)
{
	m_pDlgTrade->TradeAction(idPlayer, idTrade, idAction, nCode);
}

bool CECGameUIMan::UpdateHint()
{
	PAUIDIALOG pDlg = m_pDlgMouseOn;
	PAUIOBJECT pObj = m_pObjMouseOn;
	if( !pDlg || !pObj ) {
		return true;
	}

	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();

	if( 0 == stricmp(pDlg->GetName(), "Win_Inventory") ||
		0 == stricmp(pDlg->GetName(), "Win_Bag") ||
		0 == stricmp(pDlg->GetName(), "Win_Shop") ||
		0 == stricmp(pDlg->GetName(), "Win_Produce") ||
		//0 == stricmp(pDlg->GetName(), "Win_Trade") ||
		strstr(pDlg->GetName(), "Win_Storage") )
	{
		if( pObj->GetDataPtr("ptr_CECIvtrItem") )
		{
			int iDescType = CECIvtrItem::DESC_NORMAL;
			if (m_bRepairing &&
				(0 == stricmp(pDlg->GetName(), "Win_Inventory") || 0 == stricmp(pDlg->GetName(), "Win_Bag")))
				iDescType = CECIvtrItem::DESC_REPAIR;

			CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
			if( 0 == stricmp(pDlg->GetName(), "Win_Storage") ||
				0 == stricmp(pDlg->GetName(), "Win_Inventory") ||
				0 == stricmp(pDlg->GetName(), "Win_Bag") )
			{
				pItem->SetPriceScale(CECIvtrItem::SCALE_SELL, PLAYER_PRICE_SCALE);
			}

			const wchar_t *pszHint = pItem->GetDesc(iDescType, pHost->GetEquipment());
			if( pszHint)
			{
				if( 0 == stricmp(pDlg->GetName(), "Win_Shop") )
				{
					((CDlgShop*)pDlg)->UpdateHint(pObj, pItem, pszHint);
				}
				else
				{
					if (0 == stricmp(pDlg->GetName(), "Win_Inventory") && strstr(pObj->GetName(), "Equip_")) {
						AUICTranslate trans;
						AWString strHint = trans.Translate(pszHint);
						pObj->SetHint(strHint);
					} else {
						pObj->SetHint(GetItemDescHint(pItem, false, true));
					}
				}
			}
			else
			{
				if( 0 == stricmp(pDlg->GetName(), "Win_Inventory") ||
					0 == stricmp(pDlg->GetName(), "Win_Bag"))
				{
					int nPack, nSlot;
					if( strstr(pObj->GetName(), "Item_") )
					{
						nPack = IVTRTYPE_PACK;
						nSlot = atoi(pObj->GetName() + strlen("Item_"));
					}
					else
					{
						nPack = IVTRTYPE_EQUIPPACK;
						nSlot = atoi(pObj->GetName() + strlen("Equip_"));
					}
					pHost->GetIvtrItemDetailData(nPack, nSlot);
					pItem->SetPriceScale(CECIvtrItem::SCALE_SELL, PLAYER_PRICE_SCALE);
				}
				else if( 0 == stricmp(pDlg->GetName(), "Win_Shop")
					|| 0 == stricmp(pDlg->GetName(), "Win_Produce") )
				{
					pItem->GetDetailDataFromLocal();
				}
			}
		}
		else
		{
			if( 0 == stricmp(pDlg->GetName(), "Win_Inventory") ||
				0 == stricmp(pDlg->GetName(), "Win_Bag"))
			{
				if( strstr(pObj->GetName(), "Equip_") )
				{
					int nIndex = atoi(pObj->GetName() + strlen("Equip_"));
					pObj->SetHint(GetStringFromTable(310 + nIndex));
				}
				else if( strstr(pObj->GetName(), "Item_") )
					pObj->SetHint(_AL(""));
			}
			else if( strstr(pDlg->GetName(), "Win_Storage") 
				&& (strstr(pObj->GetName(), "Item") || stricmp(pObj->GetName(), "Img_Dice") == 0) )
			{
				pObj->SetHint(_AL(""));
			}
			else if( 0 == stricmp(pDlg->GetName(), "Win_Shop") )
			{
				if(strstr(pObj->GetName(), "U_") || strstr(pObj->GetName(), "Buy_") || strstr(pObj->GetName(), "Sell_"))
					pObj->SetHint(_AL(""));
			}
			else if( 0 == stricmp(pDlg->GetName(), "Win_Trade") )
			{
				if(strstr(pObj->GetName(), "U_") || strstr(pObj->GetName(), "I_"))
					pObj->SetHint(_AL(""));
			}
			else if ( 0 == stricmp(pDlg->GetName(), "Win_Produce") && 0 == stricmp(pObj->GetName(), "Item_out"))
			{				
				int recipeID = pObj->GetData();
				DATA_TYPE dt;
				const RECIPE_ESSENCE *pRecipe = (const RECIPE_ESSENCE *)g_pGame->GetElementDataMan()->get_data_ptr(recipeID, ID_SPACE_RECIPE, dt);
				if (DT_RECIPE_ESSENCE == dt) {
					CECIvtrItem *pProduceItem = CECIvtrItem::CreateItem((int)pRecipe->targets[0].id_to_make, 0, 1);
					pProduceItem->GetDetailDataFromLocal();
					pObj->SetHint(GetItemDescHint(pProduceItem, false, true));
					delete pProduceItem;
				}
			}
		}
	}
	else if (0 == stricmp(pDlg->GetName(), "Win_Trade")) 
	{
		if( pObj->GetDataPtr("ptr_CECIvtrItem") )
		{
			CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
			ACString strHint = GetItemDescHint(pItem, true, true);
			pObj->SetHint(strHint);
		}
		else
		{
			if(strstr(pObj->GetName(), "U_") || strstr(pObj->GetName(), "I_"))
					pObj->SetHint(_AL(""));
		}
	}
	else if( (strstr(pDlg->GetName(), "Win_Quickbar") && strstr(pObj->GetName(), "Item_")) )
	{
		if( pObj->GetDataPtr("ptr_CECShortcut") )
		{
			CECShortcut *pSC = (CECShortcut *)pObj->GetDataPtr("ptr_CECShortcut");
			const wchar_t *pszHint = pSC->GetDesc();
			if( pszHint)
			{
				AUICTranslate trans;
				AWString strHint = trans.Translate(pszHint);
				pObj->SetHint(strHint);
			}
			else
				pObj->SetHint(_AL(""));
		}
		else
			pObj->SetHint(_AL(""));
	} else if (dynamic_cast<CDlgAuctionBase*>(pDlg) != NULL &&
		dynamic_cast<AUIListBox*>(pObj) != NULL) {
		// 更新拍卖行的悬浮提示
		CDlgAuctionBase* pDlgAuctionBase = dynamic_cast<CDlgAuctionBase*>(pDlg);
		AUIListBox* pListBox = dynamic_cast<AUIListBox*>(pObj);
		int nHoverItem = pListBox->GetHOverItem();
		if (nHoverItem != -1) {
			int auctionid = pListBox->GetItemData(nHoverItem);
			pDlgAuctionBase->UpdateHint(auctionid);
		}
	} else if (dynamic_cast<CDlgWebTradeBase*>(pDlg) != NULL &&
		dynamic_cast<AUIListBox*>(pObj) != NULL) {
		// 更新寻宝天行的悬浮提示
		CDlgWebTradeBase* pDlgWebTradeBase = dynamic_cast<CDlgWebTradeBase*>(pDlg);
		AUIListBox* pListBox = dynamic_cast<AUIListBox*>(pObj);
		int nHoverItem = pListBox->GetHOverItem();
		if (nHoverItem != -1) {
			pDlgWebTradeBase->UpdateHint(nHoverItem);
		}
	} else if (0 == stricmp(pDlg->GetName(), "Win_EPEquip")) {
		// 其他玩家的装备
		if (strstr(pObj->GetName(), "Equip_")) {
			// 装备
			CECIvtrItem* pItem = (CECIvtrItem*)pObj->GetDataPtr("ptr_CECIvtrItem");
			if (pItem) {
				pObj->SetHint(GetItemDescHint(pItem, false, true));
			}
		} else if (strstr(pObj->GetName(), "Img_Card")) {
			// 卡牌
			CECIvtrItem* pItem = (CECIvtrItem*)pObj->GetDataPtr("ptr_CECIvtrItem");
			if (pItem) {
				pObj->SetHint(GetItemDescHint(pItem, false, true));
			}
		}
	}

	return true;
}

bool CECGameUIMan::UIControlCursor()
{
	if( m_bRepairing || m_pDlgGeneralCard->IsSwallowing())
		return true;
	else
	{
		if( m_pDlgMouseOn && (m_pDlgMouseOn->GetAlpha() > 0 || m_pObjMouseOn) )
			return true;
		else
			return false;
	}
}

CECShortcutSet * CECGameUIMan::GetSCSByDlg(const char *pszDlg)
{
	CECShortcutSet *pSCS = NULL;
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CDlgQuickBar *pDlg = (CDlgQuickBar *)g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog(pszDlg);

	int size(0), num(0), index(-1);
	bool horizontal(true);
	pDlg->ParseName(size, num, index, horizontal);

	if (size == SIZE_HOSTSCSET1)
	{
		int panel = (index < 0 ? CDlgQuickBar::GetCurPanel1() : index) -1;
		pSCS = pHost->GetShortcutSet1(panel);
	}
	else
	{
		int panel = (index < 0 ? CDlgQuickBar::GetCurPanel2() : index) -1;
		pSCS = pHost->GetShortcutSet2(panel);
	}

	return pSCS;
}

bool CECGameUIMan::UpdateTeach(int nPage)
{
	return m_pDlgTeach->UpdateTeach(nPage);
}

int CECGameUIMan::GetItemCount(int idItem, CECInventory *pPack)
{
	int i, nCount = 0;
	CECIvtrItem *pItem;
	CECInventory *pInventory = pPack ? pPack : g_pGame->
		GetGameRun()->GetHostPlayer()->GetPack(IVTRTYPE_PACK);

	for( i = 0; i < pInventory->GetSize(); i++ )
	{
		pItem = pInventory->GetItem(i);
		if( pItem && pItem->GetTemplateID() == idItem )
			nCount += pItem->GetCount();
	}

	return nCount;
}

void CECGameUIMan::ServiceNotify(int idService, void *pData)
{
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();

	if( idService == NOTIFY_PRODUCE_START )
	{
		const S2C::cmd_produce_start *pCmd = (const S2C::cmd_produce_start *)pData;
		PAUIPROGRESS pProgress = m_pDlgProduce->m_pPrgs_1;
		DWORD dwMSecs = pCmd->use_time * 50;

		pProgress->SetData(dwMSecs);
		pProgress->SetAutoProgress(dwMSecs, 0, AUIPROGRESS_MAX);
		
		m_pDlgPreviewProduce->OnProduceStart(pCmd->type);
	}
	else if( idService == NOTIFY_PRODUCE_END_ONE )
	{
		ACHAR szText[40];
		PAUIPROGRESS pProgress = m_pDlgProduce->m_pPrgs_1;
		PAUIPROGRESS pAbility = m_pDlgProduce->m_pPrgs_Level;
		int idSkill = pAbility->GetData();
		int nPercent = idSkill > 0 ? pHost->GetSkillAbilityPercent(idSkill) : 0;
		DWORD dwMSecs = pProgress->GetData();
		PAUIOBJECT pNum = m_pDlgProduce->m_pTxt_Number;
		int nNum = a_atoi(pNum->GetText());
		if( nNum == 0 ) return;
		PAUIOBJECT pCount = m_pDlgProduce->m_pTxt_out;
		int nTotal = a_atoi(pCount->GetText());
		PAUIOBJECT pMoney = m_pDlgProduce->m_pTxt_Gold;
		int nMoney = a_atoi(pMoney->GetText());
		int nPrice = nMoney / nNum;

		nNum--;
		a_sprintf(szText, _AL("%d"), nNum);
		pNum->SetText(szText);

		nTotal -= ((S2C::cmd_produce_once *)pData)->amount;
		a_sprintf(szText, _AL("%d"), nTotal);
		pCount->SetText(szText);

		pProgress->SetAutoProgress(dwMSecs, 0, AUIPROGRESS_MAX);

		pAbility->SetProgress(nPercent);
		a_sprintf(szText, _AL("%d%c"), nPercent, '%');
		pAbility->SetHint(szText);

		a_sprintf(szText, _AL("%d"), nNum * nPrice);
		pMoney->SetText(szText);

		// 响应本次合成成功
		m_pDlgProduce->OnProduceSucc();
	}
	else if( idService == NOTIFY_PRODUCE_END )
	{
		PAUIPROGRESS pProgress = m_pDlgProduce->m_pPrgs_1;
		NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)m_pDlgProduce->GetDataPtr("ptr_NPC_MAKE_SERVICE");

		pProgress->SetData(0);
		pProgress->SetAutoProgress(0, 0, 0);
		m_pDlgProduce->UpdateProduce(m_pDlgProduce->GetCurShopSet(), m_pDlgProduce->GetCurThing());
		m_pDlgPreviewProduce->OnProduceEnd();
		m_pDlgProduce->OnProduceEnd();
	}
	else if( idService == NOTIFY_PRODUCE_NULL )
	{
		ACHAR szText[40];
		PAUIPROGRESS pProgress = m_pDlgProduce->m_pPrgs_1;
		PAUIPROGRESS pAbility = m_pDlgProduce->m_pPrgs_Level;
		int idSkill = pAbility->GetData();
		int nPercent = idSkill > 0 ? pHost->GetSkillAbilityPercent(idSkill) : 0;
		DWORD dwMSecs = pProgress->GetData();
		PAUIOBJECT pNum = m_pDlgProduce->m_pTxt_Number;
		int nNum = a_atoi(pNum->GetText());
		if( nNum == 0 ) return;
		PAUIOBJECT pCount = m_pDlgProduce->m_pTxt_out;
		int nTotal = a_atoi(pCount->GetText());
		PAUIOBJECT pMoney = m_pDlgProduce->m_pTxt_Gold;
		int nMoney = a_atoi(pMoney->GetText());
		int nPrice = nMoney / nNum;

		nNum--;
		a_sprintf(szText, _AL("%d"), nNum);
		pNum->SetText(szText);

		a_sprintf(szText, _AL("%d"), nTotal);
		pCount->SetText(szText);

		pProgress->SetAutoProgress(dwMSecs, 0, AUIPROGRESS_MAX);

		pAbility->SetProgress(nPercent);
		a_sprintf(szText, _AL("%d%c"), nPercent, '%');
		pAbility->SetHint(szText);

		a_sprintf(szText, _AL("%d"), nNum * nPrice);
		pMoney->SetText(szText);
		S2C::cmd_produce_null* pCmd = (S2C::cmd_produce_null*)pData;
		DATA_TYPE datatype;
		const void *dataptr = g_pGame->GetElementDataMan()->get_data_ptr(pCmd->idRecipe, ID_SPACE_RECIPE, datatype);
		if( datatype != DT_INVALID)
		{
			ACString strText;
			strText = GetStringFromTable(616);
			AddChatMessage(strText, GP_CHAT_MISC);
		}
	}
	else if (idService == NOTIFY_PRODUCE_PREVIEW)
	{
		CDlgPreviewProduce* pDlg = dynamic_cast<CDlgPreviewProduce*>(GetDialog("Win_PreviewProduce"));
		if (pDlg)
		{
			pDlg->SetPreviewInfo(pData);
		}
	}
	else if( idService == NOTIFY_SPLIT_START )
	{
		PAUIPROGRESS pProgress = m_pDlgSplit->m_pPrgs_1;
		DWORD dwMSecs = ((S2C::cmd_decompose_start *)pData)->use_time * 50;

		pProgress->SetAutoProgress(dwMSecs, 0, AUIPROGRESS_MAX);
	}
	else if( idService == NOTIFY_SPLIT_END )
	{
		PAUIIMAGEPICTURE pImage;
		PAUIPROGRESS pProgress = m_pDlgSplit->m_pPrgs_1;

		pImage = m_pDlgSplit->m_pImgItem_a;
		pImage->SetCover(NULL, -1);
		pImage->SetDataPtr(NULL);

		pImage = m_pDlgSplit->m_pImgItem_b;
		pImage->SetCover(NULL, -1);
		pImage->SetDataPtr(NULL);

		pProgress->SetData(0);
		pProgress->SetAutoProgress(0, 0, 0);

		m_pDlgSplit->m_pTxt_no1->SetText(_AL(""));
		m_pDlgSplit->m_pTxt_no2->SetText(_AL(""));
		m_pDlgSplit->m_pTxt_Gold->SetText(_AL(""));
		m_pDlgSplit->m_pBtn_Cancel->Enable(true);

		pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	}
	else if( idService == NOTIFY_IDENTIFY_END )
	{
		S2C::cmd_item_identify *pCmd = (S2C::cmd_item_identify *)pData;

		if( 0 == pCmd->result )
		{
			m_pDlgIdentify->GetDlgItem("Txt")->SetText(_AL(""));
			m_pDlgIdentify->GetDlgItem("Txt_Gold")->SetText(_AL(""));
			PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)m_pDlgIdentify->GetDlgItem("Item");
			pImage->SetDataPtr(NULL);
			pImage->ClearCover();
			pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
			MessageBox("", GetStringFromTable(505),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		else
		{
			MessageBox("", GetStringFromTable(506),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}
	else if (idService == NOTIFY_ENGRAVE_START)
	{
		m_pDlgEngrave->StartEngrave((const S2C::cmd_engrave_start *)pData);
	}
	else if (idService == NOTIFY_ENGRAVE_END)
	{
		m_pDlgEngrave->EndEngrave();
	}
	else if (idService == NOTIFY_ENGRAVE_RESULT)
	{
		m_pDlgEngrave->EngraveResult((const S2C::cmd_engrave_result *)pData);
	}
	else if(idService == NOTIFY_ADDONREGEN)
	{
		CDlgRandProp* pDlg = dynamic_cast<CDlgRandProp*>(GetDialog("Win_RandProp"));
		const ECMSG* pMsg = (const ECMSG*)pData;
		if(pDlg) pDlg->OnRandPropAction(pMsg->dwParam2, (void*)pMsg->dwParam1);
		ASSERT(pDlg);
	}
}

void CECGameUIMan::GetUserLayout(void *pvData, DWORD &dwSize)
{
	if( pvData )
	{
		int i;
		USER_LAYOUT ul;
		char szName[40];
		PAUICHECKBOX pCheck;
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();

		memset(&ul, 0, sizeof(USER_LAYOUT));

		ul.nVersion = LAYOUTDATA_VERSION;
		ul.nMapMode = m_pDlgMiniMap->GetMode();

		ul.bQuickbarPetMode = GetDialog("Win_QuickbarPetV")->IsShow();

		AString dlgName;

		dlgName.Format("Win_Quickbar%dHa", SIZE_HOSTSCSET1);
		ul.bQuickbar1Mode = GetDialog(dlgName)->IsShow();
		for( i = 0; i < NUM_HOSTSCSETS1; i++ )
		{
			sprintf(szName, "Win_Quickbar%dHb_%d", SIZE_HOSTSCSET1, i+1);
			pCheck = (PAUICHECKBOX)GetDialog(szName)->GetDlgItem("Chk_Normal");
			ul.bChecked1[i] = pCheck->IsChecked();
		}
		
		dlgName.Format("Win_Quickbar%dHa", SIZE_HOSTSCSET2);
		ul.bQuickbar2Mode = GetDialog(dlgName)->IsShow();
		for( i = 0; i < NUM_HOSTSCSETS2; i++ )
		{
			sprintf(szName, "Win_Quickbar%dHb_%d", SIZE_HOSTSCSET2, i+1);
			pCheck = (PAUICHECKBOX)GetDialog(szName)->GetDlgItem("Chk_Normal");
			ul.bChecked2[i] = pCheck->IsChecked();
		}
		
		m_pDlgTask->SyncTrace(&ul, false);
		ul.bQuickbarShowAll1 = CDlgQuickBar::m_bShowAll1;
		ul.bQuickbarShowAll2 = CDlgQuickBar::m_bShowAll2;
		ul.nQuickbarCurPanel1 = CDlgQuickBar::m_nCurPanel1;
		ul.nQuickbarCurPanel2 = CDlgQuickBar::m_nCurPanel2;
		ul.nQuickbarDisplayPanels1 = CDlgQuickBar::m_nDisplayPanels1;
		ul.nQuickbarDisplayPanels2 = CDlgQuickBar::m_nDisplayPanels2;

		ul.nChatWinSize = m_pDlgChat->GetChatWinSize();
		ul.nCurChatColor = m_pDlgChat->GetChatColor();

		for( i = 0; i < (int)m_pDlgMiniMap->GetMarks().size(); i++ )
		{
			ul.a_Mark[i].nNPC = m_pDlgMiniMap->GetMarks()[i].nNPC;
			ul.a_Mark[i].vecPos = m_pDlgMiniMap->GetMarks()[i].vecPos;
			ul.a_MarkMapID[i] = (short)m_pDlgMiniMap->GetMarks()[i].mapID;
			a_strncpy(ul.a_Mark[i].szName, m_pDlgMiniMap->GetMarks()[i].strName, CECGAMEUIMAN_MARK_NAME_LEN);
		}

		CECFriendMan::GROUP *pGroup;
		CECFriendMan *pMan = g_pGame->GetGameRun()->GetHostPlayer()->GetFriendMan();
		if (pMan)
		{
			int j(0);
			for( i = 0; i < pMan->GetGroupNum() && j < CECGAMEUIMAN_MAX_GROUPS; i++ )
			{
				pGroup = pMan->GetGroupByIndex(i);
				if (pGroup->iGroup)
				{
					ul.idGroup[j] = pGroup->iGroup;
					ul.clrGroup[j] = pGroup->color;
					++ j;
				}
			}
		}
		ul.bAutoReply = m_bAutoReply;
		ul.bOnlineNotify = m_bOnlineNotify;
		ul.bSaveHistory = m_bSaveHistory;

		ul.ucCurSystemModuleSC = pHost->GetCurSysModShortcutSetIndex(); // 当前使用的系统模块快捷栏

		ul.bSystemModuleQuickBarMini = m_pDlgSysModuleQuickBar->GetMiniMode();

		ul.bMenuMode = m_pDlgSystemb->IsShow();
		
		ul.bShowCompareDesc = m_bShowItemDescCompare;
		ul.bShowLowHP = m_bShowLowHP;
		ul.bShowTargetOfTarget = m_bShowTargetOfTarget;

		memcpy(pvData, &ul, sizeof(USER_LAYOUT));

		POINT ptPos;
		float fx, fy;
		AIniFile theIni;
		char szIniFile[MAX_PATH], szKey[40];
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();

		for( i = 0; i < (int)m_aDialog.size() - 1; i++ )
		{
			ptPos = m_aDialog[i]->GetPos();

			fx = ptPos.x * 100.0f / p->Width;
			fy = ptPos.y * 100.0f / p->Height;

			//自建家园UI位置不保存
			/*
			if( dynamic_cast<CDlgHomeBase*>( m_aDialog[i])  != NULL) 
			{
				continue;
			}
			*/

			sprintf(szKey, "%s_x", m_aDialog[i]->GetName());
			theIni.WriteFloatValue("Windows", szKey, fx);

			sprintf(szKey, "%s_y", m_aDialog[i]->GetName());
			theIni.WriteFloatValue("Windows", szKey, fy);
		}
		
		// 同步保存 "Win_ELFZoom"、"Win_ELF" 的位置
		//
		PAUIDIALOG pDlg(NULL);
		if (GetDialog("Win_ELF")->IsShow())
		{
			pDlg = GetDialog("Win_ELFZoom");
			ptPos = GetDialog("Win_ELF")->GetPos();
		}
		else if (GetDialog("Win_ELFZoom")->IsShow())
		{
			pDlg = GetDialog("Win_ELF");
			ptPos = GetDialog("Win_ELFZoom")->GetPos();
		}
		if (pDlg)
		{
			fx = ptPos.x * 100.0f / p->Width;
			fy = ptPos.y * 100.0f / p->Height;

			sprintf(szKey, "%s_x", pDlg->GetName());
			theIni.WriteFloatValue("Windows", szKey, fx);

			sprintf(szKey, "%s_y", pDlg->GetName());
			theIni.WriteFloatValue("Windows", szKey, fy);
		}

		sprintf(szIniFile, "%s\\Userdata\\Layout", af_GetBaseDir());
		CreateDirectoryA(szIniFile, NULL);

		sprintf(szIniFile, "%s\\UserData\\Layout\\%d.ini",
			af_GetBaseDir(), pHost->GetCharacterID());
		theIni.Save(szIniFile);
		theIni.Close();
	}
	else
	{
		dwSize = sizeof(USER_LAYOUT);
	}
}

bool CECGameUIMan::SetUserLayout(void *pvData, DWORD dwSize)
{
	int i;
	char szName[40];
	PAUITEXTAREA pText;
	PAUICHECKBOX pCheck;
	/*P_USER_LAYOUT pul = (P_USER_LAYOUT)pvData;*/
	P_USER_LAYOUT pul = NULL;
/*	CECDataReader dr((void*)pvData, dwSize);*/
	__int8 nVersion = ((__int8 *)pvData)[0];
	bool bDelete = false;
	
	CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CECFriendMan *pMan = pHost->GetFriendMan();

	char szPath[MAX_PATH];

	sprintf(szPath, "%s\\Userdata\\Msg", af_GetBaseDir());
	CreateDirectoryA(szPath, NULL);

	sprintf(szPath, "%s\\Userdata\\Msg\\%d", af_GetBaseDir(), pHost->GetCharacterID());
	CreateDirectoryA(szPath, NULL);

	BuildFriendList();

	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgChat->GetDlgItem("Txt_Content"));
	m_pDlgChat->RebuildChatContents(pText, m_pDlgChat->GetActiveChannelSet());

	pText = dynamic_cast<PAUITEXTAREA>(m_pDlgChat2->GetDlgItem("Txt_Content"));
	m_pDlgChat->RebuildChatContents(pText, EC_USERCHANNEL_NUM - 1);

	m_pDlgActive = NULL;

	if (6 == nVersion && sizeof(_USER_LAYOUT_6) == dwSize)
	{
		pul = new USER_LAYOUT();
		LoadVersion6LayoutSettings(pvData, pul);
		bDelete = true;
	}
	else if (7 == nVersion && sizeof(_USER_LAYOUT_7) == dwSize)
	{
		pul = new USER_LAYOUT();
		LoadLayoutSettings(static_cast<P_USER_LAYOUT_7>(pvData), pul);
		bDelete = true;
	}
	else if (8 == nVersion && sizeof(_USER_LAYOUT_8) == dwSize)
	{
		pul = new USER_LAYOUT();
		LoadLayoutSettings(static_cast<P_USER_LAYOUT_8>(pvData), pul);
		bDelete = true;
	}
	else if (9 == nVersion && sizeof(_USER_LAYOUT_8) == dwSize)
	{
		pul = new USER_LAYOUT();
		LoadLayoutSettings(static_cast<P_USER_LAYOUT_9>(pvData), pul);
		bDelete = true;
	}
	else if (10 == nVersion && sizeof(_USER_LAYOUT_10) == dwSize)
	{
		pul = new USER_LAYOUT();
		LoadLayoutSettings(static_cast<P_USER_LAYOUT_10>(pvData), pul);
		bDelete = true;
	}
	else if (11 == nVersion && sizeof(_USER_LAYOUT_11) == dwSize)
	{
		pul = new USER_LAYOUT();
		LoadLayoutSettings(static_cast<P_USER_LAYOUT_11>(pvData),pul);
		bDelete = true;
	}
	else if (12 == nVersion && sizeof(_USER_LAYOUT_12) == dwSize)
	{
		pul = new USER_LAYOUT();
		LoadLayoutSettings(static_cast<P_USER_LAYOUT_12>(pvData), pul);
		bDelete = true;
	}
	else if (13 == nVersion && sizeof(_USER_LAYOUT_13) == dwSize)
	{
		pul = new USER_LAYOUT();
		LoadLayoutSettings(static_cast<P_USER_LAYOUT_13>(pvData), pul);
		bDelete = true;
	}
	else if (14 == nVersion && sizeof(_USER_LAYOUT_14) == dwSize)
	{
		pul = new USER_LAYOUT();
		LoadLayoutSettings(static_cast<P_USER_LAYOUT_14>(pvData), pul);
		bDelete = true;
	}
	else 
	{
		if( nVersion < LAYOUTDATA_VERSION )
			return false;
		if( dwSize != sizeof(USER_LAYOUT) )
			return false;
		pul = (P_USER_LAYOUT)pvData;
	}
	if (nVersion <= 9 && !pul->bTraceAll)
	{
		//	假定服务器端新建角色 config 数据版本不高于9，令新建角色默认打开任务追踪
		pul->bTraceAll = true;
		a_LogOutput(1, "CECGameUIMan::SetUserLayout, bTraceAll is forced to true.");
	}

	m_bShowItemDescCompare = pul->bShowCompareDesc;
	m_bShowLowHP = pul->bShowLowHP;
	m_bShowTargetOfTarget = pul->bShowTargetOfTarget;

	m_pDlgMiniMap->SetMode(pul->nMapMode);

	if( pul->bQuickbarPetMode )
		m_pDlgQuickBarPet = (CDlgQuickBarPet*)GetDialog("Win_QuickbarPetV");
	else
		m_pDlgQuickBarPet = (CDlgQuickBarPet*)GetDialog("Win_QuickbarPetH");

	CDlgQuickBar *pDlgQuickBar1 = NULL;
	if( pul->bQuickbar1Mode )
	{
		AString dlgName;
		dlgName.Format("Win_Quickbar%dHa", SIZE_HOSTSCSET1);
		pDlgQuickBar1 = static_cast<CDlgQuickBar *>(GetDialog(dlgName));
		pDlgQuickBar1->Show(true);
		dlgName.Format("Win_Quickbar%dVa", SIZE_HOSTSCSET1);
		GetDialog(dlgName)->Show(false);
	}
	else
	{
		AString dlgName;
		dlgName.Format("Win_Quickbar%dVa", SIZE_HOSTSCSET1);
		pDlgQuickBar1 = static_cast<CDlgQuickBar *>(GetDialog(dlgName));
		pDlgQuickBar1->Show(true);
		dlgName.Format("Win_Quickbar%dHa", SIZE_HOSTSCSET1);
		GetDialog(dlgName)->Show(false);
	}
	for( i = 0; i < NUM_HOSTSCSETS1; i++ )
	{
		sprintf(szName, "Win_Quickbar%dHb_%d", SIZE_HOSTSCSET1, i+1);
		pCheck = (PAUICHECKBOX)GetDialog(szName)->GetDlgItem("Chk_Normal");
		pCheck->Check(pul->bChecked1[i]);

		sprintf(szName, "Win_Quickbar%dVb_%d", SIZE_HOSTSCSET1, i+1);
		pCheck = (PAUICHECKBOX)GetDialog(szName)->GetDlgItem("Chk_Normal");
		pCheck->Check(pul->bChecked1[i]);
	}
	
	CDlgQuickBar *pDlgQuickBar2 = NULL;
	if( pul->bQuickbar2Mode )
	{
		AString dlgName;
		dlgName.Format("Win_Quickbar%dHa", SIZE_HOSTSCSET2);
		pDlgQuickBar2 = static_cast<CDlgQuickBar *>(GetDialog(dlgName));
		pDlgQuickBar2->Show(true);
		dlgName.Format("Win_Quickbar%dVa", SIZE_HOSTSCSET2);
		GetDialog(dlgName)->Show(false);
	}
	else
	{
		AString dlgName;
		dlgName.Format("Win_Quickbar%dVa", SIZE_HOSTSCSET2);
		pDlgQuickBar2 = static_cast<CDlgQuickBar *>(GetDialog(dlgName));
		pDlgQuickBar2->Show(true);
		dlgName.Format("Win_Quickbar%dHa", SIZE_HOSTSCSET2);
		GetDialog(dlgName)->Show(false);
	}
	for( i = 0; i < NUM_HOSTSCSETS2; i++ )
	{
		sprintf(szName, "Win_Quickbar%dHb_%d", SIZE_HOSTSCSET2, i+1);
		pCheck = (PAUICHECKBOX)GetDialog(szName)->GetDlgItem("Chk_Normal");
		pCheck->Check(pul->bChecked2[i]);

		sprintf(szName, "Win_Quickbar%dVb_%d", SIZE_HOSTSCSET2, i+1);
		pCheck = (PAUICHECKBOX)GetDialog(szName)->GetDlgItem("Chk_Normal");
		pCheck->Check(pul->bChecked2[i]);
	}	
	
	m_pDlgTask->SyncTrace(pul, true);

	if (CDlgQuickBar::m_nCurPanel1 != pul->nQuickbarCurPanel1)
	{
		int nDelta = pul->nQuickbarCurPanel1-CDlgQuickBar::m_nCurPanel1;
		pDlgQuickBar1->SwitchHotkeyPanel(nDelta);
	}
	if (CDlgQuickBar::m_bShowAll1 != pul->bQuickbarShowAll1)
		pDlgQuickBar1->OnCommand_openextra("");
	if (!pul->bQuickbarShowAll1)
	{
		if (pul->nQuickbarDisplayPanels1>=0 && pul->nQuickbarDisplayPanels1<NUM_HOSTSCSETS1)
		{
			int nTestTimes = NUM_HOSTSCSETS1;
			while (nTestTimes-- && CDlgQuickBar::m_nDisplayPanels1 != pul->nQuickbarDisplayPanels1)
				pDlgQuickBar1->OnCommand_new("");
		}
	}

	if (CDlgQuickBar::m_nCurPanel2 != pul->nQuickbarCurPanel2)
	{
		int nDelta = pul->nQuickbarCurPanel2-CDlgQuickBar::m_nCurPanel2;
		pDlgQuickBar2->SwitchHotkeyPanel(nDelta);
	}
	if (CDlgQuickBar::m_bShowAll2 != pul->bQuickbarShowAll2)
		pDlgQuickBar2->OnCommand_openextra("");
	if (!pul->bQuickbarShowAll2)
	{
		if (pul->nQuickbarDisplayPanels2>=0 && pul->nQuickbarDisplayPanels2<NUM_HOSTSCSETS2)
		{
			int nTestTimes = NUM_HOSTSCSETS2;
			while (nTestTimes-- && CDlgQuickBar::m_nDisplayPanels2 != pul->nQuickbarDisplayPanels2)
				pDlgQuickBar2->OnCommand_new("");
		}
	}

	pHost->SetCurSysModShortcutSetIndex(pul->ucCurSystemModuleSC);

	for(i = 0 ; i < pul->nChatWinSize; i++ )
		m_pDlgChat->OnCommand_chgsize("chgsize");

	m_pDlgChat->m_nCurChatColor = pul->nCurChatColor - 2;
	if( m_pDlgChat->m_nCurChatColor < 0 )
		m_pDlgChat->m_nCurChatColor = CDLGCHAT_MAX_CHAT_COLORS - 2;
	m_pDlgChat->OnCommand_chgcolor("chgcolor");
	m_pDlgChat->ChangeFocus(NULL);

	m_pDlgMiniMapMark->LoadPlayerMarks(pul);
	ChangePrecinct(m_pCurPrecinct);

	m_bAutoReply = pul->bAutoReply;
	m_bOnlineNotify = pul->bOnlineNotify;
	m_bSaveHistory = pul->bSaveHistory;

	
	((PAUISTILLIMAGEBUTTON)m_pDlgFriendOptionNormal->GetDlgItem("SetAfk"))->SetPushed(m_bAutoReply);
	((PAUISTILLIMAGEBUTTON)m_pDlgFriendOptionNormal->GetDlgItem("OnOffline"))->SetPushed(m_bOnlineNotify);
	for( i = 0; i < CECGAMEUIMAN_MAX_GROUPS; i++ )
		if (pul->idGroup[i])
			pMan->SetGroupColor(pul->idGroup[i], pul->clrGroup[i]);
	m_pDlgFriendList->BuildFriendList();
	for( i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
	{
		sprintf(szName, "Win_FriendChat%d", i);
		pCheck = (PAUICHECKBOX)GetDialog(szName)->GetDlgItem("Chk_Save");
		pCheck->Check(m_bSaveHistory);
	}

	m_pDlgSystem->Show(!pul->bMenuMode);
	m_pDlgSystemb->Show(pul->bMenuMode);

	POINT ptPos;
	float fx, fy;
	AIniFile theIni;
	char szKey[40], szIniFile[MAX_PATH];
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();

	sprintf(szIniFile, "%s\\UserData\\Layout\\%d.ini",
		af_GetBaseDir(), pHost->GetCharacterID());
	if( theIni.Open(szIniFile) )
	{
		// Ignore customize related dialogs
		//
		abase::hash_map<AString, bool> dlgIgnored;
		
		dlgIgnored["Win_ChooseEye"] = true;
		dlgIgnored["Win_ChooseEye2"] = true;
		dlgIgnored["Win_ChooseFace"] = true;
		dlgIgnored["Win_ChooseFace2"] = true;
		dlgIgnored["Win_ChooseHair"] = true;
		dlgIgnored["Win_ChooseNoseMouth"] = true;
		dlgIgnored["Win_ChooseNoseMouth2"] = true;
		dlgIgnored["Win_ChooseFacePainting"] = true;
		dlgIgnored["Win_ChoosePreCustomize"] = true;
		dlgIgnored["Win_ChooseStature"] = true;
		dlgIgnored["Win_Facename"] = true;
		dlgIgnored["Win_Customize"] = true;

		for( i = 0; i < (int)m_aDialog.size(); i++ )
		{
			if( m_aDialog[i]->GetAlpha() <= 5 )
			{
				if (stricmp(m_aDialog[i]->GetName(), "Win_ELF") &&
					stricmp(m_aDialog[i]->GetName(), "Win_ELFZoom") &&
					!strstr(m_aDialog[i]->GetName(), "Win_HpmpxpPet"))
					continue;
			}

			//自建家园UI位置不保存
			/*
			if( dynamic_cast<CDlgHomeBase*>( m_aDialog[i])  != NULL) 
			{
				continue;
			}
			*/

			if (dlgIgnored.find(m_aDialog[i]->GetName()) != dlgIgnored.end())
				continue;

			sprintf(szKey, "%s_x", m_aDialog[i]->GetName());
			fx = theIni.GetValueAsFloat("Windows", szKey, -1.0f);

			sprintf(szKey, "%s_y", m_aDialog[i]->GetName());
			fy = theIni.GetValueAsFloat("Windows", szKey, -1.0f);

			if( fx >= 0.0f && fy >= 0.0f )
			{
				ptPos.x = int((float)p->Width * fx / 100.0f);
				ptPos.y = int((float)p->Height * fy / 100.0f);
				
				// old : m_aDialog[i]->SetPos(ptPos.x, ptPos.y);
				m_aDialog[i]->SetPosEx(ptPos.x, ptPos.y);
			}
		}

		theIni.Close();
	}

	if( !m_pDlgGuildMap->GetMapStatus() )
		m_pDlgGuildMap->GetMap();
	
	CECFactionPVPModel::Instance().GetFactionPVPMap();

	m_pDlgActive = NULL;

	m_pDlgSysModuleQuickBar->SetMiniMode(pul->bSystemModuleQuickBarMini);

	if (bDelete)
	{
		delete pul;
	}
	
	A3DVIEWPORTPARAM param = *g_pGame->GetViewport()->GetA3DViewport()->GetParam();
	A3DRECT rcNew(param.X, param.Y, param.X+param.Width, param.Y+param.Height);
	RearrangeWindows(GetRect(), rcNew);

	return true;
}

//	Set UI enable flag
void CECGameUIMan::EnableUI(bool bEnable)
{
	//	Build configs
	m_pDlgSettingSystem->UpdateView();
	m_pDlgSettingVideo->UpdateView();
	m_pDlgSettingGame->UpdateView();
	m_pDlgSettingQuickKey->UpdateView();
	
	AUIOBJECT_SETPROPERTY sp;
	PAUIOBJECT pObj = m_pDlgChat->GetDlgItem("Txt_Content");
	EC_GAME_SETTING gs = g_pGame->GetConfigs()->GetGameSettings();

	pObj->GetProperty("Font", &sp);
	sp.font.nHeight = gs.nFontSize + 8;	// 8 to 12.
	sp.font.bBold = gs.bFontBold;
	a_Clamp(sp.font.nHeight, 8, 12);
	pObj->SetProperty("Font", &sp);

	pObj = GetDialog("Win_ChatSmall")->GetDlgItem("Txt_Content");
	pObj->GetProperty("Font", &sp);
	sp.font.nHeight = gs.nFontSize + 8;	// 8 to 12.
	sp.font.bBold = gs.bFontBold;
	pObj->SetProperty("Font", &sp);
	
	pObj = GetDialog("Win_ChatBig")->GetDlgItem("Txt_Content");
	pObj->GetProperty("Font", &sp);
	sp.font.nHeight = gs.nFontSize + 12; // 12 to 16.
	sp.font.bBold = gs.bFontBold;
	pObj->SetProperty("Font", &sp);

	g_pGame->GetConfigs()->LoadBlockedList();

	if (m_pDlgOnlineRemind->CanShow())
	{
		m_pDlgOnlineRemind->Show(true);
	}
	if (CECUIConfig::Instance().GetGameUI().bMailToFriendsSwitch &&
		!CECCrossServer::Instance().IsOnSpecialServer())
	{
		if (g_pGame->GetServerAbsTime() - g_pGame->GetGameRun()->GetHostPlayer()->GetRoleLastLoginTime() > 1296000
			&& g_pGame->GetGameRun()->GetHostPlayer()->GetTaskInterface()->CanGetLoginReward()
			&& g_pGame->GetGameRun()->GetHostPlayer()->GetBasicProps().iLevel >= CECUIConfig::Instance().GetGameUI().nMailToFriendsLevel)
		{
			PAUIDIALOG pDlg = GetDialog("win_returnreward");
			if (pDlg)
			{
				pDlg->Show(true);
			}
		}
	}

	CDlgTitleList::QueryTitleList();
	m_bUIEnable = bEnable;
	if (CECCommandLine::GetClearAllCoolDown()){
		OnDlgCmd_Console(_AL("d 8903 73125"));
	}
}

bool CECGameUIMan::UpdateTeam(bool bUpdateNear)
{
	return m_pDlgArrangeTeam->UpdateTeam(bUpdateNear);
}

bool CECGameUIMan::UpdateGuide()
{
	// show guide automatically
	if( !g_pGame->GetConfigs()->GetGameSettings().bHideAutoGuide )
	{
		PAUIDIALOG pDlg = GetDialog("Win_WikiGuideSearch");
		ASSERT(pDlg);
		if(pDlg) pDlg->Show(true);
		return true;
	}

	return false;
}

bool CECGameUIMan::UpdateArrowInfo()
{
	if( GetMapDlgsMgr()->IsWorldMapShown() )
	{
		return true;
	}

	int i, W = 64, H = 64;
	A3DCamera *pA3DCamera = g_pGame->GetViewport()->GetA3DCamera();
	A3DMATRIX4 mr = RotateY(DEG2RAD(-(int)pA3DCamera->GetDeg() - 90));
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	A3DVECTOR3 vecPos = pHost->GetPos();

#define DRAW_TARGET_ARROW(vec, nItem) \
{\
A3DVECTOR3 vecDiff = vec - vecPos;\
if( vecDiff.MagnitudeH() > 10.0f )\
{\
	vecDiff.y = 0.0f;\
	vecDiff.Normalize();\
	vecDiff = vecDiff * mr;\
	int x = p->Width / 2 + int(vecDiff.x * (p->Width / 2) * 4 / 5);\
	int y = p->Height / 2 - int(vecDiff.z * (p->Height / 2) * 4 / 5);\
	int nDegree = 90 - (int)RAD2DEG((float)atan2(vecDiff.z, vecDiff.x));\
	m_pA2DSpriteTarget->SetCurrentItem(nItem);\
	m_pA2DSpriteTarget->SetDegree(nDegree);\
	m_pA2DSpriteTarget->DrawToInternalBuffer((float)(p->X + x), (float)(p->Y + y));\
}\
}

	m_pA2DSpriteTarget->SetLocalCenterPos(W / 2, H / 2);
	CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();

	if( m_pDlgMiniMap->IsShowTargetArrow() )
	{
		for( i = 0; i < (int)m_pDlgMiniMap->GetMarks().size(); i++ )
		{
			if(pWorld->GetInstanceID() != m_pDlgMiniMap->GetMarks()[i].mapID)
				continue;

			DRAW_TARGET_ARROW(m_pDlgMiniMap->GetMarks()[i].vecPos, i)
		}
	}

	if( m_pDlgTeamMain->IsShowTeamArrow() )
	{
		CECTeam *pTeam = pHost->GetTeam();

		if( pTeam && pTeam->GetLeader() &&
			pTeam->GetLeader()->IsSameInstanceTeamMember())
		{
			DRAW_TARGET_ARROW(pTeam->GetLeader()->GetPos(), CECGAMEUIMAN_MAX_MARKS + 1);
		}
	}
	
	const CECHostPlayer::BATTLEINFO &bInfo = pHost->GetBattleInfo();
	if (pHost->IsInBattle() && pHost->GetBattleInfo().IsFlagCarrier())
	{
		bool bAttack = (pHost->GetBattleCamp() == GP_BATTLE_CAMP_INVADER);
		const float *flagGoal = CECCountryConfig::Instance().GetFlagGoal(bAttack);
		A3DVECTOR3 vecFlagGoal(flagGoal[0], flagGoal[1], flagGoal[2]);
		DRAW_TARGET_ARROW(vecFlagGoal, CECGAMEUIMAN_MAX_MARKS + 3);
	}

	if( m_pObjMouseOn == m_pDlgMiniMap->GetDlgItem("Hide_2") ||
		m_pDlgMouseOn && strncmp(m_pDlgMouseOn->GetName(), "mMark", 5) == 0 )
	{
		A3DVECTOR3 vecDiff = m_pDlgMiniMap->GetCurTarget() - vecPos;
		if( vecDiff.MagnitudeH() > 10.0f )
		{
			vecDiff.y = 0.0f;
			vecDiff.Normalize();
			vecDiff = vecDiff * mr;
			int x = p->Width / 2 + int(vecDiff.x * (p->Width / 2) * 4 / 5);
			int y = p->Height / 2 - int(vecDiff.z * (p->Height / 2) * 4 / 5);
			int nDegree = 90 - (int)RAD2DEG((float)atan2(vecDiff.z, vecDiff.x));
			m_pA2DSpriteMoveArrow->SetLocalCenterPos(64, 64);
			m_pA2DSpriteMoveArrow->SetDegree(nDegree);
			m_pA2DSpriteMoveArrow->DrawToInternalBuffer((float)(p->X + x), (float)(p->Y + y));
		}

	}

	return true;
}

void CECGameUIMan::PopupNPCDialog(talk_proc *pTalk)
{
	m_pDlgNPC->PopupNPCDialog(pTalk);
}

void CECGameUIMan::PopupNPCDialog(const NPC_ESSENCE *pEssence)
{
	m_pDlgNPC->PopupDialog(pEssence);
}

void CECGameUIMan::PopupTaskFinishDialog(int idTask, talk_proc *pTalk)
{
	m_pDlgNPC->PopupTaskFinishDialog(idTask, pTalk);
}

void CECGameUIMan::PopupStorageDialog(bool bClose)
{
	if( bClose )
	{
		m_pDlgStorage->Show(false);
		m_pDlgInventory->Show(false);
		EndNPCService();
	}
	else
	{
		m_pDlgStorage->Show(true);
		m_pDlgInventory->Show(true);
	}
}

void CECGameUIMan::PopupAccountBoxDialog(bool bClose)
{
	if( bClose )
	{
		m_pDlgAccountBox->Show(false);
		m_pDlgInventory->Show(false);
		EndNPCService();
	}
	else
	{
		m_pDlgAccountBox->Show(true);
		m_pDlgInventory->Show(true);
		
		// Set script state for show help information for first open of account box
		// The script will be executed once if we successfully set true here
		// Don't worry about the FIRST
		CECScriptMan * pScriptMan = g_pGame->GetGameRun()->GetUIManager()->GetScriptMan();
		if (pScriptMan)
			pScriptMan->GetContext()->GetCheckState()->SetAccountBoxIsTriggered(true);
	}
}

void CECGameUIMan::InvokeDragDrop(PAUIDIALOG pDlg, PAUIOBJECT pObj, POINT ptCapture)
{
	pDlg->SetCapture(false);

	POINT ptPos = pObj->GetPos();

	// old : m_pDlgDragDrop->SetPos(ptPos.x, ptPos.y);
	m_pDlgDragDrop->SetPosEx(ptPos.x, ptPos.y);

	m_pDlgDragDrop->Show(true);
	
	void *ptr;
	AString szType;
	pObj->ForceGetDataPtr(ptr,szType);
	m_pDlgDragDrop->SetDataPtr(ptr,szType);
	m_pDlgDragDrop->SetCapture(true, ptCapture.x, ptCapture.y);

	int nItem;
	A2DSprite *pA2DSprite;
	PAUIIMAGEPICTURE pItem = (PAUIIMAGEPICTURE)pObj;
	
	pItem->GetCover(&pA2DSprite, &nItem);
	pItem = (PAUIIMAGEPICTURE)m_pDlgDragDrop->GetDlgItem("Goods");
	pItem->SetCover(pA2DSprite, nItem);
	pItem->SetDataPtr(pObj,"ptr_AUIObject");
	m_pDlgDragDrop->SetCaptureObject(pItem);

	SIZE sizeSrc = pObj->GetSize();
	pItem->SetSize(sizeSrc.cx,sizeSrc.cy); // 把拖拽图标设置为原始图标的大小
	// 关闭卡牌悬浮显示
	CDlgItemDesc* pDlgDesc = dynamic_cast<CDlgItemDesc*>(GetDialog("Win_GeneralCardDescHover"));
	if (pDlgDesc && pDlgDesc->IsShowForHover()) pDlgDesc->OnCommandCancel(NULL);
}

void CECGameUIMan::InvokeNumberDialog(void* param1, void* param2, int nMode, int nMax)
{
	ACHAR szText[80];
	PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(m_pDlgInputNO->GetDlgItem("DEFAULT_Txt_No."));

	m_pDlgInputNO->Show(true, true);
	m_pDlgInputNO->SetDataPtr(param1, "ptr_AUIObject");
	m_pDlgInputNO->SetData((DWORD)param2,"ptr_AUIObject");
	m_pDlgInputNO->SetType(nMode);

	pEdit->SetData(nMax);
	pEdit->SetIsNumberOnly(true);
	a_sprintf(szText, _AL("%d"), 1);
	pEdit->SetText(szText);
}

void CECGameUIMan::GetEmotionList(A2DSprite **ppA2DSprite,
	abase::vector<AUITEXTAREA_EMOTION> **ppEmotion, char cEmotion)
{
	if (ppA2DSprite)
		*ppA2DSprite = m_pA2DSpriteEmotion[cEmotion];

	if (ppEmotion)
		*ppEmotion = m_vecEmotion[cEmotion];
}

bool CECGameUIMan::PlayerIsBlack(int idPlayer)
{
	if (!CECCrossServer::Instance().IsOnSpecialServer())
		return g_pGame->GetConfigs()->IsPlayerBlocked(idPlayer);
	return false;
}

void CECGameUIMan::SetMarqueeMsg(const ACHAR *pszMsg)
{
	PAUIDIALOG pDlg = GetDialog("Win_Popmsg");
	PAUILABEL pLabel = (PAUILABEL)pDlg->GetDlgItem("Message");

	pLabel->SetText(pszMsg);
	pLabel->SetMarquee(true, 3, 40);
	if( !pDlg->IsShow() ) pDlg->Show(true, false, false);

	// old : pDlg->SetPos(-1, GetDialog("Win_HpmpOther")->GetSize().cy);
	pDlg->SetPosEx(0, GetDialog("Win_HpmpOther")->GetSize().cy + 35, AUIDialog::alignCenter);
}

void CECGameUIMan::PopupReviveDialog(bool bOpen)
{
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if(!pHost) return;

	const CECHostPlayer::BATTLEINFO &bInfo = pHost->GetBattleInfo();

	// 战车战场有单独的复活界面
	if (bInfo.IsChariotWar())
	{
		if( !bOpen )
		{
			m_pDlgChariotRevive->Show(false);
			return;
		}

		if( !m_pDlgChariotRevive->IsShow() ) m_pDlgChariotRevive->Show(true);

		return;
	}

	if( !bOpen )
	{
		m_pDlgRevive->Show(false);
		return;
	}
	
	//	国战若复活次数已用完，则不再弹出
	if (pHost->IsInCountryWar() && bInfo.iReviveTimes <= 0)
		return;

	// in action scenes, we will not pop up the revive dialog
	int nInstanceID = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
	if( nInstanceID == 120 || nInstanceID == 201 || nInstanceID == 202 || nInstanceID == 203 || nInstanceID == 204 )
		return;

	if( !m_pDlgRevive->IsShow() ) m_pDlgRevive->Show(true);
	m_bShowAllPanels = true;
}

static const char *GetTaskFinishGFX(ATaskTempl *pTemp, int idEvent){
	const char *result = NULL;	
	if (pTemp && pTemp->GetShowGfxFlag()){
		switch (pTemp->GetType()){
		case enumTTQuestion:
			switch (idEvent){
			case TASK_MSG_SUCCESS:
				result = "界面\\答题任务成功.gfx";
				break;
			case TASK_MSG_FAIL:
				result = "界面\\答题任务失败.gfx";
				break;
			}
			break;
		default:
			result = "任务完成.gfx";
			break;
		}
	}
	return result;
}

void CECGameUIMan::AddTaskHint(int idTask, int idEvent, const ACHAR *pszHint)
{
	ATaskTemplMan *pMan = g_pGame->GetTaskTemplateMan();
	ATaskTempl *pTemp = pMan->GetTaskTemplByID(idTask);
	
	if (pTemp->GetShowGfxFlag()){
		if (const char *szGFX = GetTaskFinishGFX(pTemp, idEvent)){
			// Show GFX effect and return
			A3DVECTOR3 vPos;
			A3DVIEWPORTPARAM* param = g_pGame->GetViewport()->GetA3DViewport()->GetParam();
			vPos.Set(param->X+param->Width*0.5f, param->Y+param->Height*0.382f, 0);
			g_pGame->GetGFXCaster()->PlayAutoGFXEx(szGFX, 60000, vPos, A3DVECTOR3(0, 0, 1), A3DVECTOR3(0, 1, 0));
			return;
		}
	}
	
	TASK_HINT th;
	ACString strHint;
	PAUIDIALOG pDlg = GetDialog("Win_Hint");

	strHint.Format(GetStringFromTable(275 + idEvent), CDlgTask::GetTaskNameWithColor(pTemp));
	th.strHint = strHint;
	th.dwTime = GetTickCount();

	if( m_vecTaskHint.size() >= CECGAMEUIMAN_MAX_TASKHINT )
		m_vecTaskHint.erase(m_vecTaskHint.begin());
	m_vecTaskHint.push_back(th);

	if( !pDlg->IsShow() ) pDlg->Show(true);
}

void CECGameUIMan::AddHeartBeatHint(const ACHAR* pszMsg)
{
	m_pDlgPopMsg->Add(pszMsg);
}

void CECGameUIMan::ChangePrecinct(CELPrecinct *pPrecinct)
{
	if( !pPrecinct ) return;

	PAUILABEL pLabel;
	PAUIDIALOG pDlg = GetDialog("Win_Zone");
	pDlg->Show(true, false, false);

	// name
	pLabel = (PAUILABEL)pDlg->GetDlgItem("Txt_Hint");
	const CECHostPlayer::BATTLEINFO &bInfo = g_pGame->GetGameRun()->GetHostPlayer()->GetBattleInfo();
	int idBattle = bInfo.idBattle;
	if( idBattle )
	{
		pLabel->SetText(_AL(""));
		if (bInfo.IsGuildWar())
		{
			const DOMAIN_INFO *pInfo = CECDomainGuildInfo::Instance().Find(idBattle);
			if (pInfo)
			{
				ACString szText;
				szText.Format(GetStringFromTable(756), pInfo->name);
				pLabel->SetText(szText);
			}
		}
		else
		{
			const DOMAIN2_INFO *pInfo = CECDomainCountryInfo::GetInstance()->Find(idBattle);
			if (pInfo)
			{
				ACString szText;
				szText.Format(GetStringFromTable(756), pInfo->name);
				pLabel->SetText(szText);
			}
		}
	}
	else
		pLabel->SetText(pPrecinct->GetName());
	// pk 
	int nWid,nHei,nLine;
	AUI_GetTextRect(pLabel->GetFont(),pLabel->GetText(),nWid,nHei,nLine);
	int y_pos = pLabel->GetPos(true).y + nHei + 3;

	PAUILABEL pLabelPK = (PAUILABEL)pDlg->GetDlgItem("Txt_PKProtect");
	if (pLabelPK)
	{
		pLabelPK->SetText(_AL(""));
		if (pPrecinct->IsPKProtect())
		{
			pLabelPK->SetPos(0,y_pos);
			ACString strPK = GetStringFromTable(9635);
			pLabelPK->SetText(strPK);
			AUI_GetTextRect(pLabelPK->GetFont(),strPK,nWid,nHei,nLine);
			y_pos = pLabelPK->GetPos(true).y + nHei + 5;		
		}
	}
	// owner
	pLabel = (PAUILABEL)pDlg->GetDlgItem("Txt_Faction");
	pLabel->SetPos(0,y_pos);
	int idDomain = pPrecinct->GetDomainID();
	int	idFaction = 0;
	if( !m_pDlgGuildMap->GetMapStatus() )
		m_pDlgGuildMap->GetMap();

	CECFactionMan *pMan = g_pGame->GetFactionMan();
	pLabel->SetData(idDomain);
	
	const DOMAIN_INFO *pInfo = CECDomainGuildInfo::Instance().Find(idDomain);
	if (pInfo) idFaction = pInfo->id_owner;
	
	pLabel->SetText(_AL(""));
	if( idFaction )
	{
		Faction_Info *finfo = pMan->GetFaction(idFaction, true);
		if( finfo )
		{
			ACString szText = GetStringFromTable(710);
			pLabel->SetText(szText + finfo->GetName());
			pLabel->SetData(0);
		}
	}
	m_ZoneHint.strHint = pPrecinct->GetName();
	m_ZoneHint.dwTime = GetTickCount();

	m_pDlgMiniMapMark->LoadMapMarks(pPrecinct);
	m_pCurPrecinct = pPrecinct;
}

void CECGameUIMan::RefreshBrokenList()
{
	int i, j;
	bool bFound;
	CECIvtrEquip *pItem;
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CECInventory *pInventory = pHost->GetEquipment();

	for( i = 0; i < SIZE_EQUIPIVTR; i++ )
	{
		pItem = (CECIvtrEquip *)pInventory->GetItem(i);
		if( pItem && ( pItem->GetCurEndurance() <= pItem->GetMaxEndurance() / 10 
			|| pItem->GetClassID() == CECIvtrItem::ICID_ARROW
			&& pHost->CheckAmmoAmount() < 0.15 ) )
		{
			if( pItem->GetClassID() == CECIvtrItem::ICID_WING ||
				pItem->GetClassID() == CECIvtrItem::ICID_FLYSWORD ||
				pItem->GetClassID() == CECIvtrItem::ICID_FASHION ||
				m_vecBrokenList.size() >= CECGAMEUIMAN_MAX_BROKEN )
			{
				continue;
			}

			bFound = false;
			for( j = 0; j < (int)m_vecBrokenList.size(); j++ )
			{
				if( i != m_vecBrokenList[j] ) continue;

				bFound = true;
				break;
			}
			if( !bFound ) m_vecBrokenList.push_back(i);
		}
		else
		{
			for( j = 0; j < (int)m_vecBrokenList.size(); j++ )
			{
				if( i != m_vecBrokenList[j] ) continue;

				m_vecBrokenList.erase(m_vecBrokenList.begin() + j);
				break;
			}
		}
	}

	char szName[40];
	AString strFile;
	PAUIIMAGEPICTURE pImage;
	PAUIDIALOG pDlg = GetDialog("Win_Broken");

	for( i = 0; i < CECGAMEUIMAN_MAX_BROKEN; i++ )
	{
		sprintf(szName, "Slot_%d", i + 1);
		pImage = (PAUIIMAGEPICTURE)pDlg->GetDlgItem(szName);

		if( i < (int)m_vecBrokenList.size() )
		{
			pItem = (CECIvtrEquip *)pInventory->GetItem(m_vecBrokenList[i]);
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pImage->SetCover(m_pA2DSpriteIcons[ICONS_INVENTORY],
				m_IconMap[ICONS_INVENTORY][strFile]);
			pImage->SetTransparent(false);

			if( pItem->GetCurEndurance() > 0 )
			{
				pImage->SetHint(GetStringFromTable(281));
				pImage->SetColor(A3DCOLORRGB(192, 192, 0));
			}
			else
			{
				pImage->SetHint(GetStringFromTable(282));
				pImage->SetColor(A3DCOLORRGB(192, 0, 0));
			}
		}
		else
		{
			pImage->ClearCover();
			pImage->SetHint(_AL(""));
			pImage->SetTransparent(true);
			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
		}
	}
}

void CECGameUIMan::PlayItemSound(CECIvtrItem *pItem, bool bDown)
{
	if( bDown )
	{
		AString strSound;
		int idSubType = -1;
		DATA_TYPE DataType;
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		const void *pData = pDataMan->get_data_ptr(
			pItem->GetTemplateID(), ID_SPACE_ESSENCE, DataType);

		if( DataType == DT_WEAPON_ESSENCE )
		{
			const WEAPON_ESSENCE *pEssence = (const WEAPON_ESSENCE *)pData;
			idSubType = pEssence->id_sub_type;
		}
		else if( DataType == DT_ARMOR_ESSENCE )
		{
			const ARMOR_ESSENCE *pEssence = (const ARMOR_ESSENCE *)pData;
			idSubType = pEssence->id_sub_type;
		}
		else if( DataType == DT_DECORATION_ESSENCE )
		{
			const DECORATION_ESSENCE *pEssence = (const DECORATION_ESSENCE *)pData;
			idSubType = pEssence->id_sub_type;
		}

		strSound = m_IconSound[idSubType];
		if( strSound.GetLength() <= 0 )
			strSound = "SFX\\Item\\EquipSound\\_DefaultDown.wav";
		m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(strSound);
	}
	else
	{
		m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->
			Play2DAutoSound("SFX\\Item\\EquipSound\\_DefaultUp.wav");
	}
}

void CECGameUIMan::RefreshHint()
{
	PAUIDIALOG pDlg = GetDialog("Win_Popmsg");
	if( pDlg->IsShow() )
	{
		PAUILABEL pLabel = (PAUILABEL)pDlg->GetDlgItem("Message");
		if( !pLabel->IsMarquee() ) pDlg->Show(false);
	}

	int i;
	ACString strText = _AL("");
	DWORD dwTime = GetTickCount();

	for( i = 0; i < (int)m_vecTaskHint.size(); i++ )
	{
		if( CECTimeSafeChecker::ElapsedTime(dwTime, m_vecTaskHint[i].dwTime) < 5000 )
		{
			strText += m_vecTaskHint[i].strHint;
			strText += _AL("\r");
		}
		else
		{
			m_vecTaskHint.erase(m_vecTaskHint.begin() + i);
			i--;
		}
	}

	pDlg = GetDialog("Win_Hint");
	pDlg->GetDlgItem("Txt_Hint")->SetText(strText);
	if( m_vecTaskHint.size() <= 0 ){
		if (pDlg->IsShow()){
			pDlg->Show(false);
		}
	}

	if( CECTimeSafeChecker::ElapsedTime(dwTime, m_ZoneHint.dwTime) >= 5000 ){
		pDlg = GetDialog("Win_Zone");
		if (pDlg->IsShow()){
			pDlg->Show(false);
		}
	}else{
		PAUILABEL pLabel = (PAUILABEL)GetDialog("Win_Zone")->GetDlgItem("Txt_Faction");
		if( pLabel->GetData() )
		{
			const DOMAIN_INFO *pInfo = CECDomainGuildInfo::Instance().Find((int)pLabel->GetData());
			int idFaction = pInfo ? pInfo->id_owner : 0;

			if( idFaction )
			{
				CECFactionMan *pMan = g_pGame->GetFactionMan();
				Faction_Info *finfo = pMan->GetFaction(idFaction);
				if( finfo )
				{
					ACString szText = GetStringFromTable(710);
					pLabel->SetText(szText + finfo->GetName());
					pLabel->SetData(0);
				}
			}
		}
	}
	pDlg = GetDialog("Win_AutoPF");
	if (pDlg){
		bool bShouldShow = CECIntelligentRoute::Instance().IsMoveOn();
		if (pDlg->IsShow() != bShouldShow){
			pDlg->Show(bShouldShow, false, false);
			if (pDlg->IsShow()){
				pDlg->GetDlgItem("Txt_Hint")->SetTransparent(true);
			}
		}
	}
	pDlg = GetDialog("Win_AutoPolicyPF");
	if (pDlg){
		bool bShouldShow = CECAutoPolicy::GetInstance().IsAutoPolicyEnabled();
		if (pDlg->IsShow() != bShouldShow){
			pDlg->Show(bShouldShow, false, false);
			if (pDlg->IsShow()){
				pDlg->GetDlgItem("Txt_Hint")->SetTransparent(true);
			}
		}
	}
}

void CECGameUIMan::RearrangeWindows(A3DRECT rcOld, A3DRECT rcNew)
{
	A3DVIEWPORTPARAM *param = g_pGame->GetViewport()->GetA3DViewport()->GetParam();
	PAUICONSOLE pConsole = (PAUICONSOLE)GetDialog("Dlg_Console")->GetDlgItem("Command_Edit");

	CECBaseUIMan::RearrangeWindows(rcOld, rcNew);
	m_pDlgBBS->ChangeSize(param->Width, param->Height);
	m_pMapDlgsMgr->ResizeWorldMap(&rcNew);
	pConsole->FitScreen();
	m_pDlgSettingCurrent->UpdateView();
	m_pDlgPetEvoAnim->ResizeWindow(&rcNew);

	{
		PAUIDIALOG pDlgBase = m_pDlgSystem;
		m_pDlgSystem->AlignTo(NULL,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
		m_pDlgSystem2->AlignTo(pDlgBase,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);
		m_pDlgSystem3->AlignTo(pDlgBase,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);
		m_pDlgSystem4->AlignTo(pDlgBase,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);
		m_pDlgSystem5->AlignTo(pDlgBase,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);
	}
	{
		PAUIDIALOG pDlgBase = m_pDlgSystemb;
		m_pDlgSystemb->AlignTo(NULL,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
		m_pDlgSystem2b->AlignTo(pDlgBase,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
		m_pDlgSystem3b->AlignTo(pDlgBase,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
		m_pDlgSystem4b->AlignTo(pDlgBase,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
		m_pDlgSystem5b->AlignTo(pDlgBase,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
	}
	m_pDlgInfo->AlignTo(m_pDlgInfoIcon,
		AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP);
	m_pDlgMiniMap->AlignTo(NULL,
		AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
		AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP);
	
	m_pDlgPopMsg->Align();

	m_pMiniBarMgr->ArrangeMinimizeBar(true);
	RepositionAnimation();
	
	PAUIDIALOG pDlgPopmsg = GetDialog("Win_Popmsg");
	if (pDlgPopmsg->IsShow())
		pDlgPopmsg->SetPosEx(0, GetDialog("Win_HpmpOther")->GetSize().cy, AUIDialog::alignCenter);	
	
	m_pDlgChat->ReAlignChatDialogs();

	m_pDlgGuildMap->AlignFactionPVPStatus();
	m_pDlgGuildMap->AlignMyFactionColor();

	{
		// 处理夜影的连续技存在的情况
		m_pDlgComboShortCutBar->SetPosEx(0, rcNew.Height() - 200, AUIDialog::alignCenter);
	}
}

void CECGameUIMan::CalcWindowScale()
{
	CECConfigs *pConfig = g_pGame->GetConfigs();
	if (!pConfig || pConfig->GetSystemSettings().bScaleUI)
	{
		// 如果选项中允许缩放，则调用基类默认处理缩放的方法
		CECBaseUIMan::CalcWindowScale();
		a_Clamp(m_fWindowScale, 0.78125f, 1.0f);
	}
	else
	{
		// 否则，保持不缩放
		m_fWindowScale = 1.0f;
	}
}

void CECGameUIMan::OnWindowScalePolicyChange()
{
	// 计算缩放系数于 m_fWindowScale 中
	CalcWindowScale();

	// 应用缩放系统
	SetWindowScale(GetWindowScale());

	// 界面可能还需要一些对齐调整（设置相同的 rect）	
	A3DVIEWPORTPARAM param = *g_pGame->GetViewport()->GetA3DViewport()->GetParam();
	A3DRECT rcNew(param.X, param.Y, param.X+param.Width, param.Y+param.Height);
	RearrangeWindows(GetRect(), rcNew);
}

void CECGameUIMan::AddInformation(int nType, const char *pszType,
	const ACHAR *pszMsg, DWORD dwLife, DWORD dwData1, DWORD dwData2, DWORD dwData3)
{
	char szName[40];
	PAUIIMAGEPICTURE pImage;
	CDlgInfo::INFORMATION Info;

	if( !m_pDlgInfoIcon->IsShow() )
		m_pDlgInfoIcon->Show(true);

	Info.dwLifeBegin = GetTickCount();
	Info.dwLife = dwLife;
	Info.strMsg = pszMsg;
	Info.strType = pszType;
	Info.dwData1 = dwData1;
	Info.dwData2 = dwData2;
	Info.dwData3 = dwData3;

	m_pDlgInfo->AddInfo(nType, Info);

	sprintf(szName, "Back_%d", nType);
	pImage = (PAUIIMAGEPICTURE)m_pDlgInfoIcon->GetDlgItem(szName);
	if( !pImage->IsShow() )
	{
		AMSoundBufferMan *pMan = m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan();
		
		if( nType == CDlgInfo::INFO_SYSTEM )
			pMan->Play2DAutoSound(AC2AS(GetStringFromTable(570)));
		else if( nType == CDlgInfo::INFO_FRIEND )
			pMan->Play2DAutoSound(AC2AS(GetStringFromTable(571)));
	}
	pImage->Show(true);

	sprintf(szName, "F_%d", nType);
	pImage = (PAUIIMAGEPICTURE)m_pDlgInfoIcon->GetDlgItem(szName);
	pImage->Show(true);
	pImage->FadeInOut(1000);
}

void CECGameUIMan::RemoveInformation(int nType, const ACHAR* pszMsg)
{
	CDlgInfo::INFORMATION Info;
	Info.strMsg = pszMsg ? pszMsg : _AL("");
	CDlgInfo::RemoveInfo(nType, Info);
}

void CECGameUIMan::BuildFriendList(PAUIDIALOG pDlg)
{
	m_pDlgFriendList->BuildFriendList(pDlg);
}
void CECGameUIMan::BuildFriendListEx()
{
	if (CECUIConfig::Instance().GetGameUI().bMailToFriendsSwitch &&
		!CECCrossServer::Instance().IsOnSpecialServer())
	{
		m_pDlgMailToFriends->BuildFriendListEx();
	}
}
void CECGameUIMan::FriendAction(int idPlayer, int idGroup, int idAction, int nCode)
{
	m_pDlgFriendList->FriendAction(idPlayer, idGroup, idAction, nCode);
	m_pDlgMailToFriends->FriendAction(idPlayer, idGroup, idAction, nCode);
}

void CECGameUIMan::ChannelAction(int idAction, void *pData)
{
	m_pDlgChannelChat->ChannelAction(idAction, pData);
}

void CECGameUIMan::MailAction(int idAction, void *pData)
{
	m_pDlgMailList->MailAction(idAction, pData);
}

void CECGameUIMan::AuctionAction(int idAction, void *pData)
{
	m_pDlgAuctionList->AuctionAction(idAction, pData);
}

void CECGameUIMan::BattleAction(int idAction, void *pData)
{
	m_pDlgGuildMap->BattleAction(idAction, pData);
}

void CECGameUIMan::GoldAction(int idAction, void *pData)
{
	m_pDlgGoldTrade->GoldAction(idAction, pData);
}

void CECGameUIMan::WebTradeAction(int idAction, void *pData)
{
	m_pDlgWebViewProduct->WebTradeAction(idAction, pData);
	m_pDlgWebMyShop->WebTradeAction(idAction, pData);
}

void CECGameUIMan::UserCouponAction(int idAction, void *pData)
{
	CDlgCoupon* pDlg = dynamic_cast<CDlgCoupon*>(GetDialog("Win_Coupon"));
	if(pDlg) pDlg->OnUserCouponAction(idAction, pData);
}

void CECGameUIMan::UserCashAction(void* pProtocol)
{
	m_pDlgQShop->OnUserCashAction(pProtocol);
}

void CECGameUIMan::SysAuctionAction(int idAction, void *pData)
{
	if(m_pDlgBShop)
	{
		m_pDlgBShop->OnSysAuctionAction(idAction, pData);
	}
}

void CECGameUIMan::WeddingBookList(void *pData)
{
	m_pDlgWedding->OnWeddingBookList(pData);
}

void CECGameUIMan::WeddingBookSuccess(void *pData)
{
	m_pDlgWedding->OnWeddingBookSuccess(pData);
}

void CECGameUIMan::AddHistory(int idPlayer, DWORD dwTime,
	const ACHAR *pszPlayer, const ACHAR *pszMsg)
{
	int nLen;
	FILE *file;
	char szFile[MAX_PATH];
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();

	sprintf(szFile, "%s\\Userdata\\Msg\\%d\\%d.txt",
		af_GetBaseDir(), pHost->GetCharacterID(), idPlayer);
	file = fopen(szFile, "ab");
	if( !file ) return;

	fseek(file, 0, SEEK_END);

	// Player name.
	nLen = a_strlen(pszPlayer);
	fwrite(&nLen, sizeof(int), 1, file);
	fwrite(pszPlayer, sizeof(ACHAR), nLen, file);

	// Message time.
	fwrite(&dwTime, sizeof(DWORD), 1, file);

	// Message content.
	nLen = a_strlen(pszMsg);
	fwrite(&nLen, sizeof(int), 1, file);
	fwrite(pszMsg, sizeof(ACHAR), nLen, file);

	fclose(file);
}

void CECGameUIMan::PopupBoothDialog(bool bOpen, bool bOwner, int idOwnder/*= 0*/)
{
	if( bOpen )
	{
		m_bShowAllPanels = true;
		if( bOwner )
		{
			m_pDlgBooth1->Show(true);
			m_pDlgBooth1->GetDlgItem("Btn_Reset")->Enable(true);
			m_pDlgBooth1->GetDlgItem("DEFAULT_Txt_Name")->Enable(true);
		}
		else
		{
			ACHAR szText[40];
			ACString strTitle;
			PAUILABEL pLabel = (PAUILABEL)m_pDlgBooth2->GetDlgItem("Txt_Name");
			int idTarget = idOwnder;
			m_pDlgBooth2->SetData(idTarget,"int");
			const ACHAR *pszName = g_pGame->GetGameRun()->GetPlayerName(idTarget, true);

			AUI_ConvertChatString(pszName, szText);
			strTitle.Format(GetStringFromTable(582), szText);
			pLabel->SetText(strTitle);
			m_pDlgBooth2->Show(true);
		}

		m_pDlgInventory->Show(true);
	}
	else
	{
		if( bOwner )
		{
			m_pDlgInventory->Show(false);
			m_pDlgBooth1->GetDlgItem("Btn_Reset")->Enable(false);
			m_pDlgBooth1->GetDlgItem("Btn_Confirm")->Enable(false);
			m_pDlgBooth1->GetDlgItem("DEFAULT_Txt_Name")->Enable(false);
		}
		else
		{
			m_pDlgBooth2->Show(false);
			m_pDlgInventory->Show(false);
			m_pDlgInputNO->Show(false);
		}
	}
}

void CECGameUIMan::EndNPCService()
{
	m_pCurNPCEssence = NULL;
	g_pGame->GetGameRun()->GetHostPlayer()->EndNPCService();
}

void CECGameUIMan::ChangeWorldInstance(int idInstance)
{
	m_pDlgMiniMap->ChangeWorldInstance(idInstance);
	m_pDlgTitleChallenge->ChangeWorldInstance(idInstance);
	m_pDlgHost->UpdateParallelWorldUI();
	m_pDlgWorldMap->ClearControls();
	m_pDlgWorldMapDetail->ClearControls();
}

static bool RemoveFilesAndDirectory(char *pszPath)
{
	BOOL bval;
	HANDLE hFile;
	WIN32_FIND_DATAA wfd;
	AString strFind = AString(pszPath) + AString("\\*.*");

	hFile = FindFirstFileA(strFind, &wfd);
	if( INVALID_HANDLE_VALUE == hFile ) return false;

	while( true )
	{
		if( 0 != stricmp(wfd.cFileName, ".") && 0 != stricmp(wfd.cFileName, "..") )
		{
			bval = DeleteFileA(pszPath + AString("\\") + wfd.cFileName);
			if( !bval )
			{
				FindClose(hFile);
				return false;
			}
		}

		bval = FindNextFileA(hFile, &wfd);
		if( !bval ) break;
	}

	FindClose(hFile);

	return GetLastError() == ERROR_NO_MORE_FILES ? true : false;
}

void CECGameUIMan::PopupFaceLiftDialog()
{
	CECCustomizeMgr::CustomizeInitParam_t cip;
	CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	int idTarget = pHost->GetSelectedTarget();
	CECNPC *pNPC = pWorld->GetNPCMan()->GetNPC(idTarget);
	if( !pNPC ) return;

	cip.pAUImanager = this;
	cip.pCurPlayer = pHost;
	cip.nFaceTicketID = ((CECIvtrItem *)m_pDlgFaceLift->m_pImgItem->GetDataPtr("ptr_CECIvtrItem"))->GetTemplateID();
	cip.vPos = pNPC->GetPos();
	cip.vDir = pNPC->GetDir();

	m_bShowAllPanels = true;
	m_CustomizeMgr->Init(cip, &CECCustomizeHostPolicy::Instance());
	m_pDlgFaceLift->m_pImgItem->ClearCover();
}

void CECGameUIMan::PopupInstanceCountDown(int nTime,int nReason)
{
	PAUIDIALOG pDlg = GetDialog("Win_EnterWait");

	if( nTime > 0 )
	{
		ACString strMsg;

		pDlg->Show(true);
		pDlg->SetData(nTime);
		strMsg.Format(GetStringFromTable(9598 + nReason), nTime);
		pDlg->GetDlgItem("Txt_Message")->SetText(strMsg);
	}
	else
	{
		pDlg->SetData(0);
		pDlg->Show(false);
	}
}

void CECGameUIMan::PopupFactionInviteMenu(int idPlayer,
	const ACHAR *pszFaction, DWORD dwHandle)
{
	if( PlayerIsBlack(idPlayer) )
		return;

	ACHAR szName[40];
	ACString strMsg;
	const ACHAR *pszName = g_pGame->GetGameRun()->GetPlayerName(idPlayer, true);

	AUI_ConvertChatString(pszName, szName);
	strMsg.Format(GetStringFromTable(602), szName, pszFaction);
	AddInformation(CDlgInfo::INFO_SYSTEM, "Game_InviteFaction",
		strMsg, 30000, idPlayer, dwHandle, 0);
	g_pGame->GetGameRun()->GetHostPlayer()->AddRelatedPlayer(idPlayer);
}

void CECGameUIMan::UpdateFactionList()
{
	m_pDlgGuildMan->BuildFactionList();
	m_pDlgGivingFor->BuildFactionList();
}

void CECGameUIMan::UpdateChannelList(void* pChannelList)
{
	m_pDlgChannelJoin->BuildChannelList(pChannelList);
}

void CECGameUIMan::ChangeAutoHome(bool bEnter)
{
//自建家园
/*	if(bEnter)
	{
		CECHomeDlgsMgr::HomeDlgsMgrInitParams_t p;
		p.pAUIMgr = this;
		m_HomeDlgsMgr->Init(p);
		
		m_HomeDlgsMgr->Show(true);
	}
	else
	{
		m_HomeDlgsMgr->Show(false);
	}
*/
}

A2DSprite* CECGameUIMan::GetGuildIcon(const char *szName, int *pIndex)
{
	int nIndex;
	A2DSprite *pIconRet = NULL;
	abase::hash_map<AString, int>::iterator it = m_IconMap[ICONS_GUILD].find(szName);
	if( it != m_IconMap[ICONS_GUILD].end() )
	{
		pIconRet = m_pA2DSpriteIcons[ICONS_GUILD];
		nIndex = it->second;
	}
	else
	{
		abase::hash_map<AString, A2DSprite*>::iterator itIcon = m_GuildIconMapOfSepFile.find(szName);
		if (itIcon != m_GuildIconMapOfSepFile.end())
		{
			pIconRet = itIcon->second;
			nIndex = 0;
		}
		else
		{
			AString szIconFile;
			szIconFile.Format("surfaces\\iconset\\%s", szName);
			if(!af_IsFileExist(szIconFile))
			{
				pIconRet = m_pA2DSpriteIcons[ICONS_GUILD];
				nIndex = m_IconMap[ICONS_GUILD]["0_0.dds"];
			}
			else
			{
				A2DSprite *pGuildIcon = new A2DSprite;
				if( !pGuildIcon ) 
				{
					AUI_ReportError(__LINE__, __FILE__);
					pIconRet = m_pA2DSpriteIcons[ICONS_GUILD];
					nIndex = m_IconMap[ICONS_GUILD]["0_0.dds"];
				}
				else
				{
					szIconFile.Format("iconset\\%s",szName);
					pGuildIcon->Init(m_pA3DDevice,szIconFile,AUI_COLORKEY);
					m_GuildIconMapOfSepFile[szName] = pGuildIcon;
					pIconRet = pGuildIcon;
					nIndex = 0;
				}
				
			}
		}
	}
	if( pIndex )
		*pIndex = nIndex;
	return pIconRet;
}

A2DSprite* CECGameUIMan::GetIconCover(int type)
{
	if( type >= ICONS_MAX ) return NULL;

	return m_pA2DSpriteIcons[type];
}

void CECGameUIMan::SetCover(AUIImagePicture *pImg, const char *szIconFile, int iconset)
{
	if (!pImg) return;

	if (!szIconFile)
	{
		pImg->ClearCover();
		return;
	}
	if (iconset<0 || iconset>=ICONS_MAX)
	{
		ASSERT(false);
		pImg->ClearCover();
		return;
	}

	AString strFile;
	af_GetFileTitle(szIconFile, strFile);
	strFile.MakeLower();
	pImg->SetCover(m_pA2DSpriteIcons[iconset], m_IconMap[iconset][strFile]);
}

void CECGameUIMan::SetHint(AUIObject *pObj, CECIvtrItem *pItem)
{
	if (!pObj) return;
	
	const ACHAR * szDesc = pItem ? pItem->GetDesc() : NULL;
	if (szDesc)
	{
		AUICTranslate trans;
		pObj->SetHint(trans.Translate(szDesc));
	}
	else pObj->SetHint(_AL(""));
}

bool CECGameUIMan::AddFashionDescByColor(const ACString& fashionDesc, ACString& newDesc, A3DCOLOR color) {
	const CECBaseColor::BaseColor* baseColor = CECBaseColor::Instance().GetBaseColorFromColor(color);
	if (!baseColor) {
		return false;
	}
	int index = fashionDesc.Find(ACString().Format(g_pGame->GetItemDesc()->GetWideString(ITEMDESC_COLORRECT), ""));
	if (index == -1) {
		return false;
	}
	index += 3;
	ACString str1 = fashionDesc.Left(index);
	ACString str2 = fashionDesc.Right(fashionDesc.GetLength() - index);
	newDesc = str1 + _AL("^ffffff(") + baseColor->m_name + _AL(")") + str2;
	return true;
}

ACString CECGameUIMan::GetItemDescHint(CECIvtrItem* pItem, bool bShowFashionColor, bool bCompareEquip, int iDescType, CECInventory* pInventory) {
	AUICTranslate trans;
	const wchar_t* szDesc = pItem->GetDesc(iDescType, pInventory);
	if( szDesc ) {
		ACString desc = trans.Translate(szDesc);
		if (pItem->GetClassID() == CECIvtrItem::ICID_FASHION && bShowFashionColor) {
			A3DCOLOR fashionColor = dynamic_cast<CECIvtrFashion*>(pItem)->GetColor();
			ACString newDesc;
			if (AddFashionDescByColor(desc, newDesc, fashionColor)) {
				return newDesc;
			} else {
				return desc;
			}
		} else {
			if (bCompareEquip && IsShowItemDescCompare()) {
				return GetItemDescForCompare(pItem, iDescType, pInventory);
			} else {
				return desc;
			}
		}				
	} else {
		return pItem->GetName();
	}
}

namespace {
	void AddItemToVector(std::vector<CECIvtrItem*> &vecItems, CECIvtrItem* pItem) {
		if (pItem) {
			vecItems.push_back(pItem);
		}
	}
}

void CECGameUIMan::GetRelatedEquipItems(CECIvtrItem* pItem, std::vector<CECIvtrItem*>& relatedItems) {

	relatedItems.clear();

	if (pItem->GetClassID() == CECIvtrItem::ICID_RECIPE) {
		pItem = dynamic_cast<CECIvtrRecipe*>(pItem)->GetItem1();
	}

	int tid = pItem->GetTemplateID();
	DATA_TYPE dt;
	elementdataman* pDB = g_pGame->GetElementDataMan();
	const void* pData = pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, dt);

	if (!pData) {
		return;
	}

	CECInventory* pEquipInventory = g_pGame->GetGameRun()->GetHostPlayer()->GetPack(IVTRTYPE_EQUIPPACK);

	switch (dt) {
	case DT_WEAPON_ESSENCE:
		{
			// 武器
			AddItemToVector(relatedItems, pEquipInventory->GetItem(0));
			break;
		}
	case DT_ARMOR_ESSENCE:
		{
			// 防具
			ARMOR_ESSENCE* armorEssence = (ARMOR_ESSENCE*)pData;
			ARMOR_SUB_TYPE* armorSubType = (ARMOR_SUB_TYPE*)pDB->get_data_ptr(armorEssence->id_sub_type, ID_SPACE_ESSENCE, dt);
			if (armorSubType->equip_mask & 0x0002) {
				// 头部
				AddItemToVector(relatedItems, pEquipInventory->GetItem(1));
			}
			if (armorSubType->equip_mask & 0x0008) {
				// 肩部
				AddItemToVector(relatedItems, pEquipInventory->GetItem(3));
			}
			if (armorSubType->equip_mask & 0x0010) {
				// 上身
				AddItemToVector(relatedItems, pEquipInventory->GetItem(4));
			}
			if (armorSubType->equip_mask & 0x0040) {
				// 腿部
				AddItemToVector(relatedItems, pEquipInventory->GetItem(6));
			}
			if (armorSubType->equip_mask & 0x0080) {
				// 脚
				AddItemToVector(relatedItems, pEquipInventory->GetItem(7));
			}
			if (armorSubType->equip_mask & 0x0100) {
				// 手腕
				AddItemToVector(relatedItems, pEquipInventory->GetItem(8));
			}
			break;
		}
	case DT_DECORATION_ESSENCE:
		{
			// 饰品
			DECORATION_ESSENCE* decorationEssence = (DECORATION_ESSENCE*)pData;
			DECORATION_SUB_TYPE* decorationSubType = (DECORATION_SUB_TYPE*)pDB->get_data_ptr(decorationEssence->id_sub_type, ID_SPACE_ESSENCE, dt);
			if (decorationSubType->equip_mask & 0x0004) {
				// 项链
				AddItemToVector(relatedItems, pEquipInventory->GetItem(2));
			}
			if (decorationSubType->equip_mask & 0x0020) {
				// 腰部
				AddItemToVector(relatedItems, pEquipInventory->GetItem(5));
			}
			if (decorationSubType->equip_mask & 0x0200) {
				// 戒指1
				AddItemToVector(relatedItems, pEquipInventory->GetItem(9));
			}
			if (decorationSubType->equip_mask & 0x0400) {
				// 戒指2
				AddItemToVector(relatedItems, pEquipInventory->GetItem(10));
			}
			break;
		}
	/*
	case DT_DAMAGERUNE_ESSENCE:
		{
			// 攻击优化符
			AddItemToVector(relatedItems, pEquipInventory->GetItem(17));
			break;
		}
	case DT_ARMORRUNE_ESSENCE:
		{
			// 防御优化符
			AddItemToVector(relatedItems, pEquipInventory->GetItem(17));
			break;
		}
	case DT_FLYSWORD_ESSENCE:
		{
			// 飞剑
			AddItemToVector(relatedItems, pEquipInventory->GetItem(12));
			break;
		}
	case DT_WINGMANWING_ESSENCE:
		{
			// 翅膀
			AddItemToVector(relatedItems, pEquipInventory->GetItem(12));
			break;
		}
	*/
	case DT_POKER_ESSENCE:
		{
			// 卡牌
			POKER_ESSENCE* pokerEssence = (POKER_ESSENCE*)pData;
			POKER_SUB_TYPE* pokerSubType = (POKER_SUB_TYPE*)pDB->get_data_ptr(pokerEssence->id_sub_type, ID_SPACE_ESSENCE, dt);
			if (pokerSubType->type == 0) {
				AddItemToVector(relatedItems, pEquipInventory->GetItem(EQUIPIVTR_GENERALCARD1));
			}
			if (pokerSubType->type == 1) {
				AddItemToVector(relatedItems, pEquipInventory->GetItem(EQUIPIVTR_GENERALCARD2));
			}
			if (pokerSubType->type == 2) {
				AddItemToVector(relatedItems, pEquipInventory->GetItem(EQUIPIVTR_GENERALCARD3));
			}
			if (pokerSubType->type == 3) {
				AddItemToVector(relatedItems, pEquipInventory->GetItem(EQUIPIVTR_GENERALCARD4));
			}
			if (pokerSubType->type == 4) {
				AddItemToVector(relatedItems, pEquipInventory->GetItem(EQUIPIVTR_GENERALCARD5));
			}
			if (pokerSubType->type == 5) {
				AddItemToVector(relatedItems, pEquipInventory->GetItem(EQUIPIVTR_GENERALCARD6));
			}
			break;
		}
	case DT_BIBLE_ESSENCE:
		{
			// 天书
			AddItemToVector(relatedItems, pEquipInventory->GetItem(18));
			break;
		}
	default:
		break;
	}
}

ACString CECGameUIMan::GetItemDescForCompare(CECIvtrItem* pItem, int iDescType /* = CECIvtrItem::DESC_NORMAL */, CECInventory* pInventory /* = NULL */) {
	std::vector<CECIvtrItem*> relatedItems;
	GetRelatedEquipItems(pItem, relatedItems);
	
	AUICTranslate trans;
	ACString strDesc = pItem->GetDesc(iDescType, pInventory);
	strDesc = trans.Translate(strDesc);
	
	for (int i = 0; i < relatedItems.size(); i++) {
		strDesc = strDesc + _AL("\t");
		strDesc = strDesc + GetItemDescWithAlreadyEquipped(relatedItems[i], iDescType, pInventory);
	}
	return strDesc;
}

ACString CECGameUIMan::GetItemDescWithAlreadyEquipped(CECIvtrItem* pItem, int iDescType /* = CECIvtrItem::DESC_NORMAL */, CECInventory* pInventory /* = NULL */) {
	AUICTranslate trans;
	ACString strDesc = trans.Translate(pItem->GetDesc(iDescType, pInventory));
	int changeLineIndex = strDesc.Find(_AL("\r"));
	ACString str1 = strDesc.Left(changeLineIndex + 1);
	ACString str2 = strDesc.Right(strDesc.GetLength() - changeLineIndex - 1);
	return str1 + GetStringFromTable(11400) + str2;
}

int CECGameUIMan::GetIconIndex(int type, const AString& szFile) const
{
	if( type >= ICONS_MAX ) return 0;

	abase::hash_map<AString, int>::const_iterator itr = 
		m_IconMap[type].find(szFile);

	return (itr != m_IconMap[type].end()) ? itr->second : 0;
}

void CECGameUIMan::LoadVersion6LayoutSettings(void* pData, P_USER_LAYOUT pul)
{
	P_USER_LAYOUT_6 pul_6 = (P_USER_LAYOUT_6)pData;

	int i;
	pul->nVersion = pul_6->nVersion;
	pul->nMapMode = pul_6->nMapMode;	
	pul->bQuickbar1Mode = pul_6->bQuickbar1Mode;
	for (i = 0; i < 3; i++)
	{
		pul->bChecked1[i] = pul_6->bChecked1[i];
	}

	pul->bChecked1[3] = true;
	pul->bChecked1[4] = true;

	pul->bQuickbar2Mode = pul_6->bQuickbar2Mode;
	for (i = 0; i < 3; i++)
	{
		pul->bChecked2[i] = pul_6->bChecked2[i];
	}
	pul->bQuickbarPetMode = pul_6->bQuickbarPetMode;
	pul->nChatWinSize = pul_6->nChatWinSize;
	pul->nCurChatColor = pul_6->nCurChatColor;

	for (i = 0; i < CECGAMEUIMAN_MAX_MARKS; i++)
	{
		memcpy(&(pul->a_Mark[i]), &(pul_6->a_Mark[i]), sizeof(SAVE_MARK));
	}
	pul->bOnlineNotify = pul_6->bOnlineNotify;
	pul->bAutoReply = pul_6->bAutoReply;
	pul->bSaveHistory = pul_6->bSaveHistory;
	for (i = 0; i < CECGAMEUIMAN_MAX_GROUPS; i++)
	{
		pul->idGroup[i] = pul_6->idGroup[i];
	}
	for (i = 0; i < CECGAMEUIMAN_MAX_GROUPS; i++)
	{
		pul->clrGroup[i] = pul_6->clrGroup[i];
	}
}

CECGameUIMan::_USER_LAYOUT_8::_USER_LAYOUT_8()
{
	bTraceAll = true;
	bQuickbarShowAll1 = CDlgQuickBar::m_bShowAll1;
	bQuickbarShowAll2 = CDlgQuickBar::m_bShowAll2;
	nQuickbarCurPanel1 = CDlgQuickBar::m_nCurPanel1;
	nQuickbarCurPanel2 = CDlgQuickBar::m_nCurPanel2;
	nQuickbarDisplayPanels1 = CDlgQuickBar::m_nDisplayPanels1;
	nQuickbarDisplayPanels2 = CDlgQuickBar::m_nDisplayPanels2;
}

bool CECGameUIMan::IsNeedShowReferralHint()
{
	if (!m_bReferralHintShown)
	{
		const GNET::RoleInfo &info = g_pGame->GetGameRun()->GetSelectedRoleInfo();
		return info.referrer_role > 0
			&& info.lastlogin_time <= (info.create_time+1);
	}
	return false;
}

ACString CECGameUIMan::ConvertChatString(const ACHAR *pszChat)
{
	ACString strConv;
	int nChatLen = (int)a_strlen(pszChat);
	for (int i = 0; i < nChatLen; i++)
	{
		if( pszChat[i] == '^' )
		{
			strConv += '^';
			strConv += '^';
		}
		else if( pszChat[i] == '&' )
		{
			strConv += '^';
			strConv += '&';
		}
		else
		{
			strConv += pszChat[i];
		}
	}
	return strConv;
}

struct InvalidItemsFilter : public CECGameUIMan::AUI_EditboxItemFilter
{
	bool m_bFilterItem;
	InvalidItemsFilter(bool bFilterItem)
		: m_bFilterItem(bFilterItem)
	{}

	bool operator()(EditBoxItemBase *pItem)const
	{
		// 特殊符号中、只允许表情和合法的 LINK 物品
		//
		EditboxItemType type = pItem->GetType();
		if (type == enumEIEmotion)
		{
			// 表情不能过滤掉
			return false;
		}

		// 不是表情
		if (type == enumEIIvtrlItem)
		{
			// 是 LINK 物品

			//	清除物品中带的Info信息，客户端不需要
			pItem->SetInfo(_AL(""));

			return m_bFilterItem;
		}

		// 其它一律过滤掉
		return true;
	}
};

ACString CECGameUIMan::FilterInvalidTags(const ACHAR *szText, bool bFilterItem)
{
	// 过滤聊天文字中的非法特殊符号
	//
	return AUI_FilterEditboxItem(szText, InvalidItemsFilter(bFilterItem));
}

ACString CECGameUIMan::AUI_ReplaceEditboxItem(const ACHAR *szText, wchar_t cItem, const ACHAR *szSubText)
{
	// 将 szText 中的 EditboxItem c 替换为 szSubText
	//	
	
	if (!szText || !IsEditboxItemCode(cItem))
	{
		// 参数非法，返回原字符串
		return szText;
	}
	
	const ACHAR* szStart = szText;
	ACString str;
	
	while (*szText)
	{
		wchar_t ch = *szText;
		
		if (ch == cItem)
		{
			// 找到替换字符
			
			// 拷贝两次替换字符之间的内容
			if (szText > szStart)
				str += ACString(szStart, szText - szStart);
			
			// 拷贝新字符
			str += szSubText;
			
			// 指向下一个等检查对象
			szText++;
			szStart = szText;
		}
		else
		{
			szText++;
		}
	}
	
	// 拷贝最后一次替换到字符末尾
	if (szText > szStart)
		str += ACString(szStart, szText - szStart);
	
	return str;
}

ACString CECGameUIMan::AUI_FilterEditboxItem(const ACHAR *szText, const CECGameUIMan::AUI_EditboxItemFilter& filter)
{
	// 将文本中的某些内容由过滤函数处理补充、或直接过滤掉
	// 参数：szText 待过滤处理文本
	// 参数：filter 过滤函数或过滤器，原型为 bool AUI_EditboxItemFilter(EditboxItemBase *)
	//		 返回 true 则过滤掉（即恢复成由 item 的 m_szName 表示的普通字符），返回 false 保留但可更改其内容
	// 返回值：过滤处理后的合法字符串
	//
	EditBoxItemsSet ItemsSet;
	ACString strText = UnmarshalEditBoxText(szText, ItemsSet);
	int nCount = ItemsSet.GetItemCount();
	
	if (nCount == 0)
		return ACString(szText);
	
	int i = 0;		
	EditBoxItemMap::iterator it = ItemsSet.GetItemIterator();
	do 
	{
		EditBoxItemBase* pItem = it->second;
		
		if (pItem)
		{
			if (filter(pItem))
			{
				wchar_t cItem = it->first;
				strText = AUI_ReplaceEditboxItem(strText, cItem, pItem->GetName());
			}
		}
		
		++it;
		++i;
	} while (i < nCount);
	
	return MarshalEditBoxText(strText, ItemsSet);
}

ACString CECGameUIMan::AUI_ConvertHintString(const ACHAR *szText)
{
	ACString str;
	if (szText) str = szText;

	if (!str.IsEmpty())
	{
		//	转换 ^ & 字符
		ACHAR szTemp[1024];
		AUI_ConvertChatString(str, szTemp);
		str = szTemp;
		
		//	转换 \ 字符
		int count = str.GetLength();
		int nLen = 0;
		szTemp[0] = 0;
		for(int i = 0; i < count; i++ )
		{
			if( str[i] == '\\' )
			{
				szTemp[nLen] = '\\';
				szTemp[nLen + 1] = '\\';
				nLen += 2;
			}
			else
			{
				szTemp[nLen] = str[i];
				nLen++;
			}
		}
		szTemp[nLen] = 0;
		str = szTemp;
	}

	return str;
}

void CECGameUIMan::TransformNameColor(CECIvtrItem *pItem, ACString &strName, A3DCOLOR &clrName)
{
	if (pItem)
	{
		strName.Format(GetStringFromTable(8721), pItem->GetName());
		clrName = pItem->GetNameColor();
	}
	else
	{
		strName.Empty();
		clrName = 0xffffffff;
	}
}

PAUIDIALOG CECGameUIMan::GetActiveChatDialog()
{
	// 查找当前活跃的聊天窗口
	//
	PAUIDIALOG pDlgChat = NULL;

	// 收集所有的对话框
	//	
	PAUIDIALOG pDlg(NULL);
	AUIManagerDialogIteratorByZOrder it(this);

	// 查找最近接触过的、仍在显示的聊天窗口
	//
	while (it.GetCurrentAndLoopToNext(pDlg))
	{
		if (!pDlg || !pDlg->IsShow())
		{
			// 不考虑隐藏的聊天框
			continue;
		}

		const char *szName = pDlg->GetName();

		if (!stricmp(szName, "Win_Chat") ||
			!stricmp(szName, "Win_WhisperChat") ||
			strstr(szName, "Win_FriendChat"))
		{
			pDlgChat = pDlg;
			break;
		}
	}

	return pDlgChat;
}

bool CECGameUIMan::LinkItem(int nPack, int nSlot)
{	
	// 向聊天栏输入装备图
	//
	bool bSuccess(false);

	// 查找当前活跃的聊天窗口
	PAUIDIALOG pDlgChat = GetActiveChatDialog();

	// 向活跃聊天窗口发送LINK信息
	while (pDlgChat)
	{
		PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(pDlgChat->GetDlgItem("DEFAULT_Txt_Speech"));
		if (!pEdit)
		{
			pEdit = dynamic_cast<PAUIEDITBOX>(pDlgChat->GetDlgItem("Txt_Content"));
		}
		if (!pEdit)
		{
			break;
		}
		if (pEdit->GetItemCount(enumEIIvtrlItem) < 1)
		{
			// 每次只能发一个装备图
			//
			if (nPack >= 0)
			{
				CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
				CECInventory *pPack = pHost->GetPack(nPack);
				if (pPack && nSlot >= 0)
				{
					CECIvtrItem *pItem = pPack->GetItem(nSlot);
					if (pItem)
					{
						// 成功获取物品
						ACString strName;
						A3DCOLOR clrName;
						TransformNameColor(pItem, strName, clrName);

						//	加入ID信息，通过消息传递供GT显示用
						ACString strInfo;
						strInfo.Format(_AL("%s%s"), A3DCOLOR_TO_STRING(clrName), strName);

						if (pEdit->AppendItem(enumEIIvtrlItem, clrName, strName, strInfo))
						{
							// 成功添加物品
							pEdit->SetIvtrItemLocInfo(nPack, nSlot);
							bSuccess = true;
						}
					}
				}
			}
		}
		BringWindowToTop(pDlgChat);
		pDlgChat->ChangeFocus(pEdit);
		break;
	}

	return bSuccess;
}

void CECGameUIMan::OnLinkItemClicked(CECIvtrItem *pItem, WPARAM wParam, LPARAM lParam)
{
	// LINK 物品被点击
	//
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	if( pItem )
	{
		CDlgItemDesc* pDlg = (CDlgItemDesc*)GetDialog("Win_ItemDesc");
		pDlg->ShowDesc(pItem, x, y);
	}
}

ACString CECGameUIMan::GetChatChannelImage(char cChannel)
{
	// 获取频道对应的图片（以字符串方式返回），用于区分是哪个频道 
	//

	ACString str;

	// 查找对应图片是否存在
	A2DSprite *pSprite = NULL;
	if (cChannel >= 0 &&
		cChannel < GP_CHAT_MAX &&
		cChannel < (int)m_pA2DSpriteImage.size())
	{
		pSprite = m_pA2DSpriteImage[cChannel];
	}

	if (pSprite)
	{
		// 对应图片存在，生成相应字符
		EditBoxItemBase item(enumEIImage);
		item.SetImageIndex(cChannel);
		item.SetImageFrame(0);
		str = (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize() + _AL(" ");
	}

	return str;
}

ACString CECGameUIMan::GetIconsImageString(int nIconset, const char *szIconFile)
{
	ACString strImage;
	if (nIconset >= 0 && nIconset < ICONS_MAX)
	{
		AString strFile;
		af_GetFileTitle(szIconFile, strFile);
		strFile.MakeLower();
		
		EditBoxItemBase item(enumEIImage);
		item.SetImageIndex(nIconset);
		item.SetImageFrame(m_IconMap[nIconset][strFile]);
		item.SetImageScale(1.0f);

		strImage = (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize();
	}
	return strImage;
}

void CECGameUIMan::UpdateAutoDelTask(unsigned long taskID, unsigned long remainTime)
{
	CDlgAutoTask* pDlg = dynamic_cast<CDlgAutoTask*>(GetDialog("Win_AutoTask"));
	if(pDlg)
	{
		pDlg->UpdateAutoDelTask(taskID, remainTime);
	}
}


bool CECGameUIMan::ChangeLayout(const char *pszFilename)
{
	bool ret = CECBaseUIMan::ChangeLayout(pszFilename);

	if(ret)
	{
		OnWindowScalePolicyChange();
		
		// apply the font size in game-setting
		m_pDlgSettingGame->Apply();
	}

	return ret;
}

CECCustomizeMgr * CECGameUIMan::GetCustomizeMgr()
{
	return m_CustomizeMgr;
}

void CECGameUIMan::RefreshQuickBarName()
{
	CECHostInputFilter *pHostInputFilter = g_pGame->GetGameRun()->GetHostInputFilter();
	PAUIDIALOG pDlg = NULL;
	
	AString itemName;
	AWString strTemp;
	int i;

	#define QUICKBAR_REFRESH(name, start, len) \
	if(pDlg = GetDialog(name)) \
	{ \
		for(i=1;i<=len;i++) \
		{ \
			PAUILABEL pObj = dynamic_cast<PAUILABEL>(pDlg->GetDlgItem(itemName.Format("%d", i))); \
			if(!pObj) break; \
			pHostInputFilter->GetUsageShowDescription(start + i, strTemp, true); \
			pObj->SetText(strTemp); \
		} \
	} \
	
	QUICKBAR_REFRESH("Win_Quickbar9Ha", 25, 9);
	QUICKBAR_REFRESH("Win_Quickbar9Va", 25, 9);
	QUICKBAR_REFRESH("Win_Quickbar8Ha", 35, 8);
	QUICKBAR_REFRESH("Win_Quickbar8Va", 35, 8);
}

ACString CECGameUIMan::GetFormatClass(unsigned int class_masks, bool reverse)
{
	unsigned int mask = CECProfConfig::Instance().GetAllProfessionMask();
	
	unsigned int bits = class_masks & mask; // remove the invalid bit
	if(bits == 0 || bits == mask)
	{
		return GetStringFromTable(8720);
	}
	
	// count how many bit was used
	int count = 0;
	while(bits)
	{
		++count;
		bits &= (bits - 1);
	}
	
	// whether we use a reversed result
	bool doreverse = false;
	bits = class_masks & mask;
	if(reverse && count > (int)(NUM_PROFESSION * 0.6) )
	{
		bits = (~bits) & mask;
		doreverse = true;
	}
	
	ACString strCls;
	for (int i=0; i < NUM_PROFESSION; i++)
	{
		if (bits & (1 << i))
		{
			if(!strCls.IsEmpty()) strCls += _AL(",");
			strCls += g_pGame->GetGameRun()->GetProfName(i);
		}
	}
	
	if(doreverse)
	{
		ACString strTxt;
	
		return strTxt.Format(GetStringFromTable(8724), strCls);
	}
	
	return strCls;
}

int CECBaseUIMan::ReplaceColor(ACString* pStr, A3DCOLOR keyColor, A3DCOLOR newColor) const
{
	const int COLOR_STR_SIZE = 7;
	
	int replaced = 0;
	
	ACString& szText = *pStr;
	const int len = szText.GetLength();
	int left = len;
	
	// just skip
	if(keyColor == newColor || len < COLOR_STR_SIZE)
	{
		return replaced;
	}
	
	// try to get the color info in name
	ACString szNewColor = A3DCOLOR_TO_STRING(newColor);
	A3DCOLOR color = A3DCOLORRGB(255, 255, 255);
	
	ACHAR* pBuf = szText.LockBuffer();
	while(left > COLOR_STR_SIZE)
	{
		if(*pBuf == _AL('^'))
		{
			ACString szColor = szText.Mid(len - left, COLOR_STR_SIZE);
			if(STRING_TO_A3DCOLOR(szColor, color))
			{
				if(color == keyColor)
				{
					memcpy(pBuf, (const ACHAR*)szNewColor, sizeof(ACHAR) * COLOR_STR_SIZE);
					replaced++;
				}
				
				left -= COLOR_STR_SIZE;
				pBuf += COLOR_STR_SIZE;
				continue;
			}
		}
		
		--left;
		++pBuf;
	}
	szText.UnlockBuffer();
	
	return replaced;
}

void CECGameUIMan::AutoMoveStart(int x, int y, bool bShowDlg)
{
	m_pDlgAutoMove->Set2DTargetPos(x, y);	
	g_pGame->GetGameRun()->PostMessage(MSG_HST_AUTOMOVE, MAN_PLAYER, 0, 0, (DWORD)x, (DWORD)y, (DWORD)bShowDlg);
}

A3DPOINT2 CECGameUIMan::AutoMoveTarget()
{
	return m_pDlgAutoMove->GetTargetPosAs2D();
}

void CECGameUIMan::AutoMoveShowDialog(bool isShow)
{
	m_pDlgAutoMove->RefreshHigh();

	if( isShow != m_pDlgAutoMove->IsShow() )
	{
		m_pDlgAutoMove->Show(isShow);
		if(!isShow) m_pDlgAutoMove->Clear2DTargetPos();
	}
	
	if( m_pDlgHideMove->IsShow() )
	{
		m_pDlgHideMove->Show(false);
	}
}

void CECGameUIMan::FriendOnlineNotify(int idFriend, char oldStatus, char newStatus)
{
	//	好友在线状态改变时调用，以通知客户端状态改变
	if (!m_bOnlineNotify)
	{
		//	客户端不需要通知
		return;
	}

	//	判断是否是好友
	CECFriendMan *pFriendMan = g_pGame->GetGameRun()->GetHostPlayer()->GetFriendMan();
	CECFriendMan::FRIEND *pFriend = NULL;
	if (!pFriendMan->CheckInit() || !(pFriend = pFriendMan->GetFriendByID(idFriend)))
	{
		//	无法查询或不是好友
		return;
	}

	ACString strMsg;

	//	先输出下线消息
	if (CECFriendMan::FRIEND::IsGameOnline(oldStatus) && !CECFriendMan::FRIEND::IsGameOnline(newStatus))
	{
		//	游戏下线
		strMsg.Format(GetStringFromTable(550), pFriend->GetName());
		AddChatMessage(strMsg, GP_CHAT_SYSTEM);
	}
	if (CECFriendMan::FRIEND::IsGTOnline(oldStatus) && !CECFriendMan::FRIEND::IsGTOnline(newStatus))
	{
		//	GT 下线
		strMsg.Format(GetStringFromTable(9311), pFriend->GetName());
		AddChatMessage(strMsg, GP_CHAT_SYSTEM);
	}

	//	再输出上线消息
	if (!CECFriendMan::FRIEND::IsGameOnline(oldStatus) && CECFriendMan::FRIEND::IsGameOnline(newStatus))
	{
		//	游戏上线
		strMsg.Format(GetStringFromTable(549), pFriend->GetName());
		AddChatMessage(strMsg, GP_CHAT_SYSTEM);
	}
	if (!CECFriendMan::FRIEND::IsGTOnline(oldStatus) && CECFriendMan::FRIEND::IsGTOnline(newStatus))
	{
		//	GT 下线
		strMsg.Format(GetStringFromTable(9310), pFriend->GetName());
		AddChatMessage(strMsg, GP_CHAT_SYSTEM);
	}
}

void CECGameUIMan::ProcessOfflineMsgs()
{
	while (true)
	{
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		if (!pHost)	break;

		CECFriendMan *pMan = pHost->GetFriendMan();
		if (!pMan || !pMan->CheckInit())	break;
		
		//	（检查好友列表初始化后再显示离线消息，以便能查询好友名称，即使好友不在线时也正确显示名称）
		
		int nMsgNum = pMan->GetOfflineMsgNum();
		if (nMsgNum <= 0)	break;
		
		//	弹框显示好友消息、或在聊天栏显示单向好友密语
		for(int i = 0; i < nMsgNum; i++ )
		{
			CECFriendMan::MESSAGE *pMsg= pMan->GetOfflineMsg(i);
			if( pMsg->idSender )
				AddFriendMessage(pMsg->strMsg, pMsg->idSender, pMsg->strName, pMsg->byFlag);
			else
				AddInformation(CDlgInfo::INFO_SYSTEM, "Game_MsgBox", pMsg->strMsg, 0xFFFFFFF, 0, 0, 0);
		}
		
		//	移除所有已处理的离线消息，有再接到服务器主动推动的GT客户端离线消息的可能
		pMan->RemoveAllOfflineMsgs();

		break;
	}
}

void CECGameUIMan::TraceTask(unsigned long idTask)
{
	m_pDlgTask->TraceTask(idTask);
}

void CECGameUIMan::RefreshItemNotify()
{
	// only show once
	if(m_bItemNotifyShown)
	{
		return;
	}
	m_bItemNotifyShown = true;

	// skip checking if no string template
	ACString strTmpl = GetStringFromTable(9346);
	if(strTmpl.IsEmpty())
	{
		return;
	}

	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();

	// check the expired date
	int curTime = g_pGame->GetServerGMTTime();
	const int timeLimit = 3600 * 24 * 2; // 2 days
	
	int packs[] = { IVTRTYPE_PACK,		//	Normal pack
					IVTRTYPE_EQUIPPACK,	//	Equipment
	};
	size_t count = sizeof(packs) / sizeof(packs[0]);
	for(size_t i=0;i<count;i++)
	{
		CECInventory* pPack = pHost->GetPack(i);
		if(!pPack) continue;

		size_t packsize = pPack->GetSize();
		for(size_t j=0;j<packsize;j++)
		{
			CECIvtrItem* pItem = pPack->GetItem(j);
			if(!pItem) continue;

			int timeLeft = pItem->GetExpireDate() - curTime;
			if(timeLeft < 0 || timeLeft > timeLimit) continue;

			ACString strMsg;
			strMsg.Format(strTmpl, pItem->GetName(), GetFormatTime(timeLeft));
			AddChatMessage(strMsg, GP_CHAT_MISC);
		}
	}
}

// default ItemDataExtractor
class ItemDataExtractorDefault : public CECGameUIMan::ItemDataExtractor
{
public:
	virtual CECIvtrItem* GetItemPtr(PAUIOBJECT pObj)
	{
		return (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	}
};

// render the item icon cover
void CECGameUIMan::RenderItemCover(PAUIOBJECT* pObjs, int size, ItemDataExtractor* pExtractor)
{
	if(!pObjs || !size || !m_pA2DSpriteItemExpire)
	{
		return;
	}

	// use default extractor if not set
	ItemDataExtractorDefault def;
	if(pExtractor == NULL)
	{
		pExtractor = &def;
	}

	m_pA2DSpriteItemExpire->SetScaleX(GetWindowScale());
	m_pA2DSpriteItemExpire->SetScaleY(GetWindowScale());

	
	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	for(int i=0;i<size;i++)
	{
		PAUIOBJECT pObj = *pObjs++;
		CECIvtrItem *pItem = (!pObj || !pObj->IsShow()) ? NULL : pExtractor->GetItemPtr(pObj);
		if(pItem && pItem->GetExpireDate() > 0)
		{
			POINT pt = pObj->GetPos();
			m_pA2DSpriteItemExpire->DrawToBack((int)(p->X + pt.x), (int)(p->Y + pt.y));
		}
	}
}

void CECGameUIMan::RenderCover(PAUIOBJECT pObj)
{
	if(pObj && m_pA2DSpriteItemExpire)
	{
		m_pA2DSpriteItemExpire->SetScaleX(GetWindowScale());
		m_pA2DSpriteItemExpire->SetScaleY(GetWindowScale());

		A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
		POINT pt = pObj->GetPos();
		m_pA2DSpriteItemExpire->DrawToBack((int)(p->X + pt.x), (int)(p->Y + pt.y));
	}
}

void CECGameUIMan::UpdateSkillRelatedUI()
{
	//	更新技能相关的界面显示

	if (m_pDlgQuickBar1)
		m_pDlgQuickBar1->UpdateShortcuts();

	/*
	if (m_pDlgSkill && m_pDlgSkill->IsShow())
		m_pDlgSkill->UpdateView();
	*/

	if (m_pDlgSkillEdit && m_pDlgSkillEdit->IsShow())
	{
		//	技能编辑界面只在 Show(true) 的时候才能更新
		m_pDlgSkillEdit->Show(false);
	}
}

void CECGameUIMan::RemindTrashboxPassword()
{
	//	提醒用户设置仓库密码
	if (!m_bTrashPwdReminded)
	{
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		if (pHost->HostIsReady())
		{
			if (CECUIConfig::Instance().GetGameUI().bEnableTrashPwdRemind && !pHost->TrashBoxHasPsw())
				ShowErrorMsg(9610);
			m_bTrashPwdReminded = true;
		}
	}
}

bool CECGameUIMan::SSOOpenLink(const AString& strFormat, const AString& strTicket, bool bTicketFirst)
{
	bool ret(false);
	while (true)
	{
		if (strFormat.IsEmpty())
			break;
		if (strTicket.IsEmpty())
		{
			a_LogOutput(1, "CECGameUIMan::SSOOpenLink, ticket is empty.");
			break;
		}
		AString strAccount = g_pGame->GetGameSession()->GetTicketAccount();
		AString strURL;
		if (bTicketFirst)
			strURL.Format(strFormat, strTicket, strAccount);
		else
			strURL.Format(strFormat, strAccount, strTicket);
		NavigateURL(strURL);
		ret = true;
		break;
	}
	return ret;
}

void CECGameUIMan::HandleRequest(const CECSSOTicketHandler::Request *p)
{
	AString strText;
	AString strTicket((const char *)p->ticket.begin(), p->ticket.size());
	if (p->retcode == 0)
	{
		//	GT 客户端登录
		if (p->local_context == GetGTTicketContext())
		{
			using namespace overlay;
			if (!GTOverlay::Instance().IsLogin())
			{
				//	防止已登录后又调用
				int zoneid = g_pGame->GetGameSession()->GetZoneID();
				__int64 roleid = g_pGame->GetGameRun()->GetSelectedRoleInfo().roleid;
				AString strAccount = g_pGame->GetGameSession()->GetTicketAccount();
				GTOverlay::Instance().Login(zoneid, roleid, strAccount, strTicket);
			}
			else
			{
				a_LogOutput(1, "CECGameUIMan::OnSSOGetTicket_Re, GT ticket(%s) ignored because already login", strTicket);
			}
			m_pDlgHost->ProcessGTEnter();
			m_pDlgGuildMan->ProcessGTEnter();
			m_pDlgTeamMain->ProcessGTEnter();
			return;
		}
		// 进入Touch
		if (p->local_context == GetTouchTicketContext()) {
			strText = GetURL("WEB", "ENTER_TOUCH");
			AString strURL;
			strURL.Format(
				strText,
				g_pGame->GetGameSession()->GetTicketAccount(),
				strTicket,
				g_pGame->GetGameSession()->GetZoneID());
			NavigateURL(strURL, NULL);
			return;
		}
		
		bool bTicketFirst(true);
		
		//	账号信息补填
		if (p->local_context == GetAccountTicketContext())
		{
			strText = GetURL("WEB", "COMPLETE_ACCOUNT");
			bTicketFirst = true;
		}
		//  开通快捷支付
		else if (p->local_context == GetQuickPayTicketContext())
		{
			strText = GetURL("QUICKPAY", "ACTIVATE_MERCHANT");
			bTicketFirst = false;
		}
		//  新手卡补绑
		else if (p->local_context == GetNewbieGiftTicketContext())
		{
			strText = GetURL("WEB", "NEWBIE_GIFT");
			bTicketFirst = true;
		}
		SSOOpenLink(strText, strTicket, bTicketFirst);
	}
	//	错误码由 CECGameUIMan 统一处理
	else
	{
		ACString strError = GetStringFromTable(9680+p->retcode);
		if (strError.IsEmpty())
			strError.Format(GetStringFromTable(9680), p->retcode);
		ShowErrorMsg(strError);
	}
}

void CECGameUIMan::InitSSOTicketHandler()
{
	//	将 GNET::SSOGetTicket_Re 协议的 Handler 放于此处的原因是，各 Handler 的生命周期较好控制
	CECHandlerChainConstructor<CECSSOTicketHandler::Request> handlerContructors;
	handlerContructors.Append(dynamic_cast<CDlgWebList *>(GetDialog("Win_WebList")));
	handlerContructors.Append(m_pDlgWebViewProduct);
	handlerContructors.Append(&CECUseUniversalTokenCommandManager::Instance());
	handlerContructors.Append(&CECUniversalTokenHTTPOSNavigatorTicketHandler::Instance());
	handlerContructors.Append(this);
	m_ssoTicketHandler = handlerContructors.Construct();
}

void CECGameUIMan::OnSSOGetTicket_Re(const GNET::SSOGetTicket_Re *pProtocol)
{
	if (!pProtocol){
		ASSERT(false);
		return;
	}
	m_ssoTicketHandler->HandleRequest(pProtocol);
}
void CECGameUIMan::UpdateProfitUI()
{
	m_pDlgMiniMap->UpdateProfitUI();
}

void CECGameUIMan::SwitchCountryChannel()
{
	m_pDlgChat->SwitchCountryChannel();
	m_pDlgSettingGame->SwitchCountryChannel();
}

void CECGameUIMan::SSOGetQuickPayTicket()
{
	g_pGame->GetGameSession()->sso_GetTicket(GetQuickPayTicketContext());
}
void CECGameUIMan::SSOGetTouchTicket()
{
	GNET::Marshal::OctetsStream info;
	info << CECGameSession::SSO_TI_ROLEINFO_1;
	g_pGame->GetGameSession()->sso_GetTicket(GetTouchTicketContext(), info, 30000, 0);
}
void CECGameUIMan::ChariotWarAction(DWORD cmd, void *pData)
{
	using namespace S2C;
	switch(cmd)
	{
	case ENTER_TRICKBATTLE:
		{
			CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
			m_pDlgChariotUI->Show(pHost->GetBattleInfo().IsChariotWar());
			m_pDlgChariotMsgScore->Show(pHost->GetBattleInfo().IsChariotWar());
			if(m_pDlgChariotWarApply->IsShow())
				m_pDlgChariotWarApply->OnCommandCancel(NULL);
			if (!pHost->GetBattleInfo().IsChariotWar())
			{
				if(m_pDlgChariotRankList->IsShow())
					m_pDlgChariotRankList->Show(false);
				if (m_pDlgChariotRevive->IsShow())
					m_pDlgChariotRevive->Show(false);
				if(m_pDlgChariotMsgMulti->IsShow())
					m_pDlgChariotMsgMulti->Show(false);
			}
			if(pHost->GetBattleInfo().IsChariotWar())
			{
				/*
				if(m_pDlgSkill->IsShow())
					m_pDlgSkill->Show(false);
				*/
				if (m_pDlgSkillAction->IsShow()) {
					m_pDlgSkillAction->Show(false);
				}
				if(m_pDlgSkillEdit->IsShow())
					m_pDlgSkillEdit->Show(false);
			}
		}
		break;
	case TRICKBATTLE_PERSONAL_SCORE:
		{
			m_pDlgChariotMsgScore->UpdateScore();
			m_pDlgChariotMsgMulti->UpdateKill();
		}
		break;
	case TRICKBATTLE_CHARIOT_INFO:
		{

		}
		break;
	case PLAYER_QUERY_CHARIOTS:
		{
			cmd_player_query_chariots *pCmd = (cmd_player_query_chariots *)pData;
			if (m_pDlgChariotAmount)
			{
				m_pDlgChariotAmount->SetInfo(pCmd->attacker_count,pCmd->chariots,pCmd->defender_count,pCmd->chariots + pCmd->attacker_count);
				m_pDlgChariotAmount->Show(true);
			}
		}
		break;
	}
}
void CECGameUIMan::CountryAction(DWORD cmd, void *pData)
{
	switch(cmd)
	{
	case S2C::SELF_COUNTRY_NOTIFY:
	case S2C::PLAYER_COUNTRY_CHANGED:
		m_pDlgCountryMap->OnCountryChange();
		SwitchCountryChannel();
		break;

	case S2C::ENTER_COUNTRYBATTLE:
		m_pDlgCountryMap->OnCountryEnterWar();
		m_pDlgCountryScore->OnCountryEnterWar();
		SwitchCountryChannel();
		break;

	case S2C::COUNTRYBATTLE_RESURRECT_REST_TIMES:
		{
			ACString strText;			
			CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
			strText.Format(GetStringFromTable(9884), pHost->GetBattleInfo().iReviveTimes);
			AddChatMessage(strText, GP_CHAT_MISC);
		}
		break;

	case S2C::COUNTRYBATTLE_BECAME_FLAG_CARRIER:
		{
			CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
			AddChatMessage(GetStringFromTable(pHost->GetBattleInfo().bFlagCarrier ? 9885 : 9886), GP_CHAT_MISC);
		}
		break;

	case S2C::COUNTRYBATTLE_INFO:
	case S2C::COUNTRYBATTLE_SCORE:
		m_pDlgCountryScore->UpdateScore();
		break;

	case S2C::COUNTRYBATTLE_PERSONAL_SCORE:
		m_pDlgCountryWarMap->OnGetPersonalScore();
		break;

	case S2C::COUNTRYBATTLE_RESULT:
		m_pDlgCountryScore->Show(false);
		break;

	case S2C::COUNTRYBATTLE_FLAG_MSG_NOTIFY:
		{
			using namespace S2C;
			cmd_countrybattle_flag_msg_notify *pCmd = (cmd_countrybattle_flag_msg_notify*)pData;
			ACString strMsg;
			switch (pCmd->msg)
			{
			case FLAG_MSG_GENERATE: strMsg = GetStringFromTable(9920); break;
			case FLAG_MSG_PICKUP: strMsg.Format(GetStringFromTable(9921), GetStringFromTable(pCmd->offense ? 9918 : 9919)); break;
			case FLAG_MSG_HANDIN: strMsg.Format(GetStringFromTable(9922), GetStringFromTable(pCmd->offense ? 9918 : 9919)); break;
			case FLAG_MSG_LOST: strMsg.Format(GetStringFromTable(9923), GetStringFromTable(pCmd->offense ? 9918 : 9919)); break;
			default: ASSERT(false); break;
			}
			if (!strMsg.IsEmpty())
			{
				AddHeartBeatHint(strMsg);
				AddChatMessage(strMsg, GP_CHAT_MISC);
			}
		}
		break;
	}
}

bool CECGameUIMan::NewMessageBox(const char *szContext, const ACHAR *szMsg, DWORD dwType, bool bModal/* = true */)
{
	if (m_pDlgMessageBox->IsShow())
	{
		a_LogOutput(1, "CECGameUIMan::NewMessageBox(szContent=%s, szMsg=%s), already used with (szContent=%s, szMsg=%s)",
			szContext ? szContext : "", szMsg ? AC2AS(szMsg) : "",
			m_pDlgMessageBox->GetContext(), m_pDlgMessageBox->GetMessage());
		ASSERT(false);
		return false;
	}
	
	switch (dwType)
	{
	case MB_OK:
		m_pDlgMessageBox->SetShow(CDlgMessageBox::BTN_OK, true);
		m_pDlgMessageBox->SetText(CDlgMessageBox::BTN_OK, m_pDlgMessageBox->GetStringFromTable(IDOK));

		m_pDlgMessageBox->SetShow(CDlgMessageBox::BTN_YES, false);
		m_pDlgMessageBox->SetShow(CDlgMessageBox::BTN_NO, false);
		break;

	case MB_YESNO:
		m_pDlgMessageBox->SetShow(CDlgMessageBox::BTN_OK, false);

		m_pDlgMessageBox->SetShow(CDlgMessageBox::BTN_YES, true);
		m_pDlgMessageBox->SetText(CDlgMessageBox::BTN_YES, m_pDlgMessageBox->GetStringFromTable(IDYES));
		m_pDlgMessageBox->SetShow(CDlgMessageBox::BTN_NO, true);
		m_pDlgMessageBox->SetText(CDlgMessageBox::BTN_NO, m_pDlgMessageBox->GetStringFromTable(IDNO));
		break;

	default:
		ASSERT(false); return false;
	}

	m_pDlgMessageBox->SetContext(szContext);
	m_pDlgMessageBox->SetMessage(szMsg);

	m_pDlgMessageBox->Show(true, bModal);
	return true;
}

void CECGameUIMan::DrawSpriteTarget(int nItem, A3DPOINT2 pt)
{
	if (nItem < 0 || nItem > CECGAMEUIMAN_MAX_MARKS + 3)
	{
		ASSERT(false);
		return;
	}
	A3DRECT rcSize = m_pA2DSpriteTarget->GetItem(nItem)->GetRect();
	m_pA2DSpriteTarget->SetCurrentItem(nItem);
	m_pA2DSpriteTarget->SetDegree(0);
	m_pA2DSpriteTarget->DrawToBack(pt.x, pt.y);
}

bool CECGameUIMan::SSOGetGTTicket()
{
	bool bRet(false);
	while (true)
	{
		if (!g_pGame->GetConfigs()->GetEnableGT())
		{
			//	GT 已禁用
			ASSERT(false);
			break;
		}

		if (!overlay::GTOverlay::Instance().IsLoaded())
		{
			//	GT 模块未加载

			if (s_GTUpdaterRunned)
			{
				//	本次客户端启动后已经运行过安装程序
				ShowErrorMsg(10062);
				break;
			}
			
			AString strUpdater = GetGTUpdater();
			if (strUpdater.IsEmpty())
			{
				ShowErrorMsg(10063);
				break;
			}

			MessageBox("GT_Install", GetStringFromTable(10064), MB_YESNO, A3DCOLORRGB(255, 255, 255));
			break;
		}
		
		//	检查冷却
		DWORD curTime = timeGetTime();
		if (m_dwLastGTLogin != 0 && (curTime < m_dwLastGTLogin + CECUIConfig::Instance().GetGameUI().nGTLoginCoolDown))
		{
			ShowErrorMsg(GetStringFromTable(10061));
			break;
		}
		m_dwLastGTLogin = curTime;
		
		//	获取登录 ticket
		g_pGame->GetGameSession()->sso_GetTicket(GetGTTicketContext(), GNET::Octets(), 24, 2400);
		bRet = true;
		break;
	}
	return bRet;
}

void CECGameUIMan::UpdateGTWindow()
{
	using namespace overlay;
	
	if (!GTOverlay::Instance().IsLogin())
	{
		//	GT 未登录时，由GT自己处理图标的显示状态
		return;
	}

	bool bShowGT(false);

	while (true)
	{
		if (GetMapDlgsMgr()->IsWorldMapShown() ||
			m_pDlgGuildMap->IsShow())
			break;

		if (!m_bShowAllPanels)
			break;

		bShowGT = true;
		break;
	}

	GTOverlay::Instance().Toggle(bShowGT);
}

void CECGameUIMan::UpdateGTTeam()
{
	using namespace overlay;

	bool bShowJoin(false);

	while (true)
	{
		if (!GTOverlay::Instance().IsLoaded() ||
			CECCrossServer::Instance().IsOnSpecialServer() &&
			!CECUIConfig::Instance().GetGameUI().bEnableGTOnSpecialServer)
			break;

		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		if (pHost->GetTeam() == NULL)
			break;

		bShowJoin = true;
		break;
	}

	PAUIOBJECT pObj = m_pDlgTeamMain->GetDlgItem("Btn_GTalk");
	if (pObj && pObj->IsShow() != bShowJoin)
		pObj->Show(bShowJoin);
}

void CECGameUIMan::UpdateGTFaction()
{
	using namespace overlay;
	
	bool bShowJoin(false);
	
	while (true)
	{
		if (!GTOverlay::Instance().IsLoaded() ||
			CECCrossServer::Instance().IsOnSpecialServer() &&
			!CECUIConfig::Instance().GetGameUI().bEnableGTOnSpecialServer)
			break;
		
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		if (pHost->GetFactionID() <= 0)
			break;
		
		bShowJoin = true;
		break;
	}
	
	PAUIOBJECT pObj = m_pDlgGuildMan->GetDlgItem("Btn_GTalk");
	if (pObj && pObj->IsShow() != bShowJoin)
		pObj->Show(bShowJoin);
}

void CECGameUIMan::UpdateTaskConfirm()
{
	if(m_pDlgTaskConfirm)
		m_pDlgTaskConfirm->UpdateTaskList();
}

void CECGameUIMan::AddNewTaskConfirm()
{
	if (m_pDlgTaskHintPop)
		m_pDlgTaskHintPop->OnNewConfirmTask();
}

void CECGameUIMan::RefreshFriendChatPlayerName(int roleid)
{	
	char szName[40];
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CECFriendMan *pMan = pHost->GetFriendMan();
	for(int i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
	{
		sprintf(szName, "Win_FriendChat%d", i);
		PAUIDIALOG pDlg = GetDialog(szName);
		if( (int)pDlg->GetData() == roleid )
		{
			//	聊天框更新
			CECFriendMan::FRIEND *pFriend = pMan->GetFriendByID(roleid);
			ACString strName;
			int nLevel = -1;
			if (pFriend)
			{
				strName = pFriend->GetName();
				nLevel = pFriend->nLevel;
			}
			if (strName.IsEmpty())
				strName = g_pGame->GetGameRun()->GetPlayerName(roleid, true);

			ACString strText;
			if (nLevel > 0 )
			{
				ACHAR szLevel[10];
				a_sprintf(szLevel, _AL("%d"), nLevel);
				strText.Format(GetStringFromTable(548), strName, szLevel);
			}
			else
				strText.Format(GetStringFromTable(548), strName, GetStringFromTable(574));
			
			PAUIOBJECT pName = pDlg->GetDlgItem("Txt_ToWho");
			pName->SetText(strText);
			
			//	最小化界面更新
			GetMiniBarMgr()->UpdateDialog(pDlg,	CECMiniBarMgr::BarInfo(strName, A3DCOLORRGB(0, 255, 0)));
			break;
		}
	}
}

void CECGameUIMan::MeridiansNotify()
{
	if(m_pDlgMeridians->IsShow())
		m_pDlgMeridians->OnMeridiansProcess();
	if(m_pDlgMeridiansImpact->IsShow())
		m_pDlgMeridiansImpact->SetBtnInfo();
}
void CECGameUIMan::MeridiansImpactResult(int index, int result)
{
	if (m_pDlgMeridiansImpact->IsShow())
		m_pDlgMeridiansImpact->OnImpactResult(index, result);
}


void CECGameUIMan::PopupNewbieGiftRemind()
{
	if (!CECUIConfig::Instance().GetGameUI().bEnableCheckNewbieGift) return;
	ACString strMsg = GetStringFromTable(10230);
	AddInformation(CDlgInfo::INFO_SYSTEM, "Game_NewbieGift", strMsg, 0x0fffffff, 0, 0, 0);
}

void CECGameUIMan::KingAction(void* pData)
{
	using namespace GNET;
	Protocol* p = (Protocol*)pData;

	enum
	{
		ST_CLOSE = 0,
		ST_INIT,
		ST_OPEN,
		ST_CANDIDATE_APPLY,
		ST_WAIT_VOTE,
		ST_VOTE,
	};

	switch(p->GetType())
	{
	case PROTOCOL_KECANDIDATEAPPLY_RE:
		m_pDlgKingAuction->OnKECandidateApply_Re(p);
		break;
	case PROTOCOL_KEVOTING_RE:
		m_pDlgKingElection->OnKEVoting_Re(p);
		break;
	case PROTOCOL_KEGETSTATUS_RE:
		{
			ACString strErr;
			KEGetStatus_Re* pStatus = (KEGetStatus_Re*)p;

			if( ((pStatus->status == ST_WAIT_VOTE || pStatus->status == ST_VOTE) && !m_pDlgKingElection->IsShow() && !CDlgKingAuction::CheckPlayerCondition(false, strErr)) ||
				(pStatus->status == ST_CANDIDATE_APPLY && !m_pDlgKingAuction->IsShow() && !CDlgKingAuction::CheckPlayerCondition(true, strErr)))
			{
				MessageBox("", strErr, MB_OK, A3DCOLORRGB(255, 255, 255));
				EndNPCService();
				return;
			}

			switch( pStatus->status )
			{
			case ST_OPEN:
				if( m_pDlgKingElection->IsShow() )
					m_pDlgKingElection->Show(false);
				if( !m_pDlgKingResult->IsShow() )
				{
					m_pDlgKingResult->Show(true);
					m_pDlgKingResult->SetKingID(pStatus->king.roleid);
				}
				break;
			case ST_CANDIDATE_APPLY:
				if( !m_pDlgKingAuction->IsShow() )
					m_pDlgKingAuction->Show(true);
				break;
			case ST_WAIT_VOTE:
			case ST_VOTE:
				if( !m_pDlgKingElection->IsShow() )
					m_pDlgKingElection->Show(true);
				m_pDlgKingElection->SetWaitVote(pStatus->status == ST_WAIT_VOTE);
				m_pDlgKingElection->OnKEGetStatus_Re(p);
				break;
			case ST_CLOSE:
				MessageBox("", GetStringFromTable(10322), MB_OK, A3DCOLORRGB(255, 255, 255));
				EndNPCService();
				break;
			default:
				ASSERT(0);
				a_LogOutput(1, "CECGameUIMan::KingAction, Unknown king service status(%d)!", pStatus->status);
				EndNPCService();
				break;
			}
		}
		break;
	}
}

ACString CECGameUIMan::GetItemName(int itemid)
{
	CECIvtrItem*pItem = CECIvtrItem::CreateItem(itemid,0,1);
	ACString strName = pItem->GetName();
	delete pItem;
	
	return strName;
}
void CECGameUIMan::PopTaskEmotionDlg(unsigned int task_id,unsigned int uiEmotion,bool bShow)
{
	PAUIDIALOG dlg = GetDialog("Win_ActionPop");
	if(!dlg || (bShow == dlg->IsShow())) return;
	dlg->SetData(uiEmotion);
	dlg->SetData64(task_id);
	dlg->Show(bShow);
}
A2DSprite* CECGameUIMan::GetSysModuleIcon(int n)
{
	if (n>=0 && n<(int)m_pSpriteIconSysModule.size())
	{
		return m_pSpriteIconSysModule[n];
	}
	return NULL;
}
void CECGameUIMan::HideGeneralCardHover()
{
	CDlgItemDesc* pDlgDesc = dynamic_cast<CDlgItemDesc*>(GetDialog("Win_GeneralCardDescHover"));
	if (pDlgDesc){
		if (!pDlgDesc->IsShowForHover()) return;
		if (!m_pObjMouseOn || !m_pDlgMouseOn) {
			pDlgDesc->OnCommandCancel(NULL);
			return;
		}
		if (strstr(m_pObjMouseOn->GetName(), "Item_") 
			|| strstr(m_pObjMouseOn->GetName(), "Img_CardFrame")
			|| strstr(m_pObjMouseOn->GetName(), "Img_Card")) {
			CECIvtrItem* pItem = (CECIvtrItem*)m_pObjMouseOn->GetDataPtr("ptr_CECIvtrItem");
			if (pItem != pDlgDesc->GetItemHover()) pDlgDesc->OnCommandCancel(NULL);
		} else pDlgDesc->OnCommandCancel(NULL);
	}
}

bool CECGameUIMan::LoadSprite(const char *szPath, A2DSprite *&pSprite)
{
	if (!pSprite){
		pSprite = new A2DSprite;
		if (!pSprite->Init(m_pA3DDevice, szPath, 0)){
			A3DRELEASE(pSprite);
			return false;
		}		
		pSprite->SetLocalCenterPos(pSprite->GetWidth() / 2, pSprite->GetHeight() / 2);
		pSprite->SetLinearFilter(true);
	}
	return true;
}

ACString CECGameUIMan::GetCashText(int nCash, bool bFullText)
{
	ACString strText;
	if( bFullText )
		strText.Format(GetStringFromTable(1453), nCash / 100, nCash % 100);
	else if( nCash < 100 )
		strText.Format(GetStringFromTable(1452), nCash);
	else if( nCash % 100 == 0 )
		strText.Format(GetStringFromTable(1451), nCash / 100);
	else
		strText.Format(GetStringFromTable(1453), nCash / 100, nCash % 100);
	return strText;
}

void CECGameUIMan::SetCashText(PAUIOBJECT pObj, int nCash, bool bFullText/*=false*/){
	if (pObj){
		pObj->SetText(GetCashText(nCash, bFullText));
	}
}

void CECGameUIMan::SetAutoMoveShowDialogTarget( int x, int y )
{
	m_pDlgAutoMove->Set2DTargetPos(x,y);
}

bool CECGameUIMan::CanToggleAllPannelsByInput()
{
	bool ret = true;
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost )
		ret = !pHost->IsInForceNavigateState();

	return ret;
}

bool CECGameUIMan::IsShowItemDescCompare() {
	return m_bShowItemDescCompare;
}

void CECGameUIMan::SetShowItemDescCompare(bool bShow) {
	m_bShowItemDescCompare = bShow;
}

bool CECGameUIMan::IsShowLowHP() {
	return m_bShowLowHP;
}

void CECGameUIMan::SetShowLowHP(bool bShow) {
	m_bShowLowHP = bShow;
}

bool CECGameUIMan::IsShowTargetOfTarget() {
	return m_bShowTargetOfTarget;
}

void CECGameUIMan::SetShowTargetOfTarget(bool bShow) {
	m_bShowTargetOfTarget = bShow;
}
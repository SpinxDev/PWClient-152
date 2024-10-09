// Filename	: DlgChat.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005

#include "privilege.hxx"
#include "CSplit.h"
#include "AUIDef.h"
#include "AUICommon.h"
#include "AUIComboBox.h"
#include "AUICheckBox.h"
#include "AUIEditBox.h"
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include "DlgChat.h"
#include "DlgSetting.h"
#include "DlgAskHelpToGM.h"
#include "DlgMinimizeBar.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "EC_World.h"
#include "EC_HostPlayer.h"
#include "EC_Resource.h"
#include "EC_Configs.h"
#include "EC_Inventory.h"
#include "EC_Friend.h"
#include "EC_Viewport.h"
#include "EC_Utility.h"
#include "EC_IvtrItem.h"
#include "EC_GameTalk.h"
#include "EC_ManPlayer.h"
#include "AUICTranslate.h"
#include "EC_UIHelper.h"
#include "EC_TimeSafeChecker.h"

#include "DlgCustomizePaint.h"
#include "AFI.h"
#include <A2DSprite.h>
#include <A3DSurface.h>
#include <A3DSurfaceMan.h>
#include <A3DFTFont.h>

#define new A_DEBUG_NEW
#define CDLGCHAT_MAX_HISTORY	20

AUI_BEGIN_COMMAND_MAP(CDlgChat, CDlgBase)

AUI_ON_COMMAND("lock",			OnCommand_lock)
AUI_ON_COMMAND("speak",			OnCommand_speak)
AUI_ON_COMMAND("popface",		OnCommand_popface)
AUI_ON_COMMAND("speakmode",		OnCommand_speakmode)
AUI_ON_COMMAND("channel",		OnCommand_channel)
AUI_ON_COMMAND("chgcolor",		OnCommand_chgcolor)
AUI_ON_COMMAND("chgsize",		OnCommand_chgsize)
AUI_ON_COMMAND("clear",			OnCommand_clear)
AUI_ON_COMMAND("mini",			OnCommand_mini)
AUI_ON_COMMAND("set*",			OnCommand_set)
AUI_ON_COMMAND("send",			OnCommand_send)
AUI_ON_COMMAND("list",			OnCommand_list)
AUI_ON_COMMAND("createchannel",	OnCommand_createchannel)
AUI_ON_COMMAND("joinchannel",	OnCommand_joinchannel)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("minimize",		OnCommandMinimize)
AUI_ON_COMMAND("popface01",		OnCommand_popface01)
AUI_ON_COMMAND("popface02",		OnCommand_popface02)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgChat, CDlgBase)

AUI_ON_EVENT("DEFAULT_Txt_Speech",	WM_CHAR,			OnEventChar_Txt_Speech)
AUI_ON_EVENT("DEFAULT_Txt_Speech",	WM_KEYDOWN,			OnEventKeyDown_Txt_Speech)
AUI_ON_EVENT("DEFAULT_Txt_Speech",	WM_SYSCHAR,			OnEventSysChar_Txt_Speech)
AUI_ON_EVENT("Txt_Content",			WM_LBUTTONDOWN,		OnEventLButtonDown_Txt_Content)
AUI_ON_EVENT("Txt_Content",			WM_LBUTTONUP,		OnEventLButtonUp_Txt_Content)
AUI_ON_EVENT("Txt_Content",			WM_RBUTTONUP,		OnEventRButtonUp_Txt_Content)
AUI_ON_EVENT("Txt_Content",			WM_MOUSEMOVE,		OnEventMouseMove_Txt_Content)
AUI_ON_EVENT("Txt_Chat",			WM_LBUTTONDOWN,		OnEventLButtonDown_Txt_Content)
AUI_ON_EVENT("Txt_Chat",			WM_LBUTTONUP,		OnEventLButtonUp_Txt_Content)
AUI_ON_EVENT("Txt_Chat",			WM_RBUTTONUP,		OnEventRButtonUp_Txt_Content)
AUI_ON_EVENT("Txt_Chat",			WM_MOUSEMOVE,		OnEventMouseMove_Txt_Content)
AUI_ON_EVENT("Face_*",				WM_LBUTTONUP,		OnEventLButtonUp_Face)
AUI_ON_EVENT("ImagePicture_ColorPic", WM_LBUTTONDOWN,	OnLButtonDownPicPallete)
AUI_ON_EVENT("ImagePicture_ColorPic", WM_MOUSEMOVE,		OnMouseMovePicPallete)

AUI_END_EVENT_MAP()

bool CDlgChat::m_bLocked = false;
int CDlgChat::m_nChatWinSize = 0;
int CDlgChat::m_nCurChatColor = 0;
int CDlgChat::m_nCurChannelSet = 0;
int CDlgChat::m_nActiveChannelSet = -1;
int CDlgChat::m_nChatMsgCount1 = 0;
int CDlgChat::m_nChatMsgCount2 = 0;
int CDlgChat::m_nCurHistory = -1;
DWORD CDlgChat::m_dwTickFarCry = 0;
DWORD CDlgChat::m_dwTickFarCry2 = 0;
abase::vector<CDlgChat::CHAT_MSG> CDlgChat::m_vecHistory;
abase::vector<CDlgChat::CHAT_MSG> CDlgChat::m_vecChatMsg;
int CDlgChat::m_nMsgIndex = 1;

abase::vector<CDlgChat::LINKED_MSG> CDlgChat::m_whisperChatMsg;

abase::vector<CDlgChat::CHAT_MSG> CDlgChat::m_vecSuperFarCryMsg;
bool				CDlgChat::m_curSuperFarCryMsgValid;
CDlgChat::CHAT_MSG	CDlgChat::m_curSuperFarCryMsg;
DWORD				CDlgChat::m_curSuperFarCryTick;
int					CDlgChat::m_curSuperFarCryShowTime;
bool				CDlgChat::m_curSuperFarCryShowMode;

AUIImagePicture * CDlgChat::m_pPicPane = NULL;
AUIImagePicture * CDlgChat::m_pPicIndicator = NULL;
AUIObject		* CDlgChat::m_pDisplay = NULL;
CPaletteControl * CDlgChat::m_pPallete = NULL;
A2DSprite		* CDlgChat::m_pA2DSpriteColorPic = NULL;

const ACHAR * CDlgChat::m_pszColor[GP_CHAT_MAX] =
{
	_AL("^FFFFFF"),		// GP_CHAT_LOCAL
	_AL("^FFE400"),		// GP_CHAT_FARCRY
	_AL("^00FF00"),		// GP_CHAT_TEAM
	_AL("^00FFFC"),		// GP_CHAT_FACTION
	_AL("^0065FE"),		// GP_CHAT_WHISPER
	_AL("^C0C0C0"),		// GP_CHAT_DAMAGE
	_AL("^FF7E00"),		// GP_CHAT_FIGHT
	_AL("^FF742E"),		// GP_CHAT_TRADE
	_AL("^BED293"),		// GP_CHAT_SYSTEM
	_AL("^FF3600"),		// GP_CHAT_BROADCAST
	_AL("^9AA6FF"),		// GP_CHAT_MISC
	_AL("^EC0D3C"),		// GP_CHAT_INSTANCE
	_AL("^ff9b3e"),		// GP_CHAT_SUPERFARCRY
	_AL("^FFFFFF"),		// GP_CHAT_BATTLE
	_AL("^FFFFFF"),		// GP_CHAT_COUNTRY
};

const ACHAR * CDlgChat::m_pszWhisperFriendColor = _AL("^FF4AB0");
const ACHAR * CDlgChat::m_pszKingColor = _AL("^8A2BE2");

const ACHAR * CDlgChat::GetChatColor(int iChannel, int iParameter /* = -1 */)
{
	if (iChannel == GP_CHAT_WHISPER)
	{
		int idPlayer = iParameter;
		CECFriendMan *pFriendMan = g_pGame->GetGameRun()->GetHostPlayer()->GetFriendMan();
		if (idPlayer > 0 &&
			pFriendMan &&
			pFriendMan->GetFriendByID(idPlayer))
		{
			return m_pszWhisperFriendColor;
		}
	}

	return m_pszColor[iChannel];
}

CDlgChat::CDlgChat()
{
	m_pObjLock = NULL;
}

CDlgChat::~CDlgChat()
{
	if (m_szName == "Win_Popface02")
	{
		A3DRELEASE(m_pA2DSpriteColorPic);
		
		delete m_pPallete;
		m_pPallete = NULL;
	}
	
	size_t i(0);
	for (i = 0; i < m_vecChatMsg.size(); i++)
	{
		delete m_vecChatMsg[i].pItem;
	}
	m_vecChatMsg.clear();
	
	for (i = 0; i < m_whisperChatMsg.size(); i++)
	{
		delete m_whisperChatMsg[i].pItem;
	}
	m_whisperChatMsg.clear();
	
	for (i = 0; i < m_vecSuperFarCryMsg.size(); i++)
	{
		delete m_vecSuperFarCryMsg[i].pItem;
	}
	m_vecSuperFarCryMsg.clear();

	delete m_curSuperFarCryMsg.pItem;
	m_curSuperFarCryMsg.pItem = NULL;
	m_curSuperFarCryMsg.strMsg.Empty();
}

bool CDlgChat::OnInitDialog()
{
	size_t i(0);
	for (i = 0; i < m_vecChatMsg.size(); i++)
	{
		delete m_vecChatMsg[i].pItem;
	}
	m_vecChatMsg.clear();
	m_vecHistory.clear();
	
	for (i = 0; i < m_whisperChatMsg.size(); i++)
	{
		delete m_whisperChatMsg[i].pItem;
	}
	m_whisperChatMsg.clear();
	
	for (i = 0; i < m_vecSuperFarCryMsg.size(); i++)
	{
		delete m_vecSuperFarCryMsg[i].pItem;
	}
	m_vecSuperFarCryMsg.clear();
	
	delete m_curSuperFarCryMsg.pItem;
	m_curSuperFarCryMsg.pItem = NULL;
	m_curSuperFarCryMsg.strMsg.Empty();
	m_curSuperFarCryMsgValid = false;

	m_bLocked = false;
	m_nChatWinSize = 0;
	m_nCurChatColor = 0;
	m_nCurChannelSet = 0;
	m_nActiveChannelSet = -1;
	m_nChatMsgCount1 = 0;
	m_nChatMsgCount2 = 0;
	m_nCurHistory = -1;

	m_pObjLock = GetDlgItem("Btn_Lock");

	if (m_szName == "Win_Popface02")
	{
		// 调色板相关
		//
		m_pPicPane		= (PAUIIMAGEPICTURE)(GetDlgItem("ImagePicture_ColorPic"));
		m_pPicIndicator = (PAUIIMAGEPICTURE)(GetDlgItem("Pic_ColorPos"));
		m_pDisplay		= GetDlgItem("Lab_Colorlab");

		// 因为支持MouseMove选颜色，需要ImagePicture接受鼠标消息，避免导致对话框跟着一起移动
		m_pPicPane->SetAcceptMouseMessage(true);

		A3DSurface* pSurface = NULL;
		A2DSprite* pSprite = NULL;
		
		char *pszPicFileName = "surfaces\\图标\\个性化\\调色板.tga";
		bool success(false);
		while (true)
		{
			if (!af_IsFileExist(pszPicFileName))
			{
				a_LogOutput(1, "CDlgChat::OnInitDialog, 打开调色板图片文件 %s 失败！号角频道需要此图片才能正常调色", pszPicFileName);
				break;
			}
			
			pszPicFileName += strlen("surfaces\\");
			
			// 创建查询颜色的 surface
			A3DSurfaceMan* pA3DSurfaceMan = GetGame()->GetA3DEngine ()->GetA3DSurfaceMan();
			if (!pA3DSurfaceMan->LoadCursorSurfaceFromFile(pszPicFileName, 0, &pSurface))
				break;
			
			// 创建显示的图片
			pSprite = new A2DSprite;
			A3DDevice* pDevice = GetGame()->GetA3DDevice();
			if (!pSprite->Init(pDevice, pszPicFileName, 0))
				break;
			
			success = true;
			break;
		}
		
		if (success)
		{
			m_pPallete = new CPaletteControl(m_pPicPane, m_pPicIndicator, m_pDisplay, this);
			m_pPallete->ResetContext(pSurface, pSprite);
			A3DCOLOR clr;
			if (STRING_TO_A3DCOLOR(GetChatColor(GP_CHAT_SUPERFARCRY), clr))
				m_pPallete->SetColor(clr);
			m_pA2DSpriteColorPic = pSprite;
		}
		else
		{
			if (pSurface)
			{
				A3DSurfaceMan* pA3DSurfaceMan = GetGame()->GetA3DEngine()->GetA3DSurfaceMan();
				pA3DSurfaceMan->ReleaseSurface(pSurface);
			}
			A3DRELEASE(pSprite);
		}

		// 附加状态文字
		AString strName;
		ACString strHint;
		PAUIOBJECT pObj(NULL);
		int i(0);
		while (true)
		{
			strName.Format("Rdo_%d", i);
			pObj = GetDlgItem(strName);
			if (!pObj)
				break;

			strHint = GetStringFromTable(8540+i);

			pObj->SetHint(strHint);
			
			strName.Format("Lab_%d", i);
			pObj = GetDlgItem(strName);
			if (pObj)
				pObj->SetHint(strHint);

			++i;
		}

		// 默认为没有附加文字
		CheckRadioButton(1, 1);
	}

	return true;
}

void CDlgChat::OnCommand_lock(const char * szCommand)
{
	m_bLocked = !m_bLocked;
	m_pObjLock->SetColor(m_bLocked ? A3DCOLORRGB(255, 203, 74) : A3DCOLORRGB(255, 255, 255));
	
	PAUISTILLIMAGEBUTTON pLockBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(m_pObjLock);
	if(pLockBtn) pLockBtn->SetPushed(m_bLocked);
	
	if( !m_bLocked )
	{
		PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
		pText->SetData(pText->GetFirstLine());
		pText->ScrollToBottom();
		GetDlgItem("Btn_Lock")->BlinkText(false);
	}
}

#ifdef _PROFILE_MEMORY
static const char * GetHeapTypeDescription(ULONG heapInfo){	
	static const char *s_heapNames[3] = {"Regular", "Look-aside", "LFH"};
	return heapInfo >= 0 && heapInfo < ARRAY_SIZE(s_heapNames) ? s_heapNames[heapInfo] : "Unknown-type";
}

static AString FormatMemorySize(__int64 size){
	return AString().Format("%I64dMB %I64dKB %I64dB", size/1024/1024, size%(1024*1024)/1024, size%1024);
}

static const char * GetDefaultHeapDescription(HANDLE heap){
	return heap == GetProcessHeap() ? "(Default)" : "";
}

static void WalkHeap(HANDLE heap, __int64 &allocatedSize){
	allocatedSize = 0;
	PROCESS_HEAP_ENTRY heapEntry;
	memset(&heapEntry, 0, sizeof(heapEntry));
	int iEntry(0);
	while (::HeapWalk(heap, &heapEntry)){
		if (heapEntry.wFlags & PROCESS_HEAP_ENTRY_BUSY){
			a_LogOutput(1, "Allocated Entry %d: size=%u, overhead=%u", iEntry++, heapEntry.cbData, heapEntry.cbOverhead);
			allocatedSize += heapEntry.cbData + heapEntry.cbOverhead;
		}
	}
}

static void PrintHeap(HANDLE heaps[], int i, __int64 &allocatedSize){
	allocatedSize = 0;
	ULONG	heapInfo = 0;
	SIZE_T	returnLength = 0;
	if (::HeapQueryInformation(heaps[i], HeapCompatibilityInformation, &heapInfo, sizeof(heapInfo), &returnLength)){
		a_LogOutput(1, "%s%s (index=%d)", GetHeapTypeDescription(heapInfo), GetDefaultHeapDescription(heaps[i]), i);		
		WalkHeap(heaps[i], allocatedSize);
		a_LogOutput(1, "Total(current heap) Allocated size = %s", FormatMemorySize(allocatedSize));
		CECUIHelper::GetGameUIMan()->AddChatMessage(
			AS2AC(AString().Format("%s%s (index=%d) Total=%s",
			GetHeapTypeDescription(heapInfo), GetDefaultHeapDescription(heaps[i]), i, FormatMemorySize(allocatedSize))),
			GP_CHAT_LOCAL);
	}else{
		a_LogOutput(1, "HeapQueryInformation return 0, GetLastError()=%u", GetLastError());
	}
}

static void PrintHeaps(){	
	HANDLE heaps[100] = {0};
	DWORD heapCount = ::GetProcessHeaps(ARRAY_SIZE(heaps), heaps);
	if (heapCount > 0 && heapCount <= ARRAY_SIZE(heaps)){
		__int64 totalAllocatedSize(0);
		for (int i(0); i < heapCount; ++ i){
			__int64 allocatedSize(0);
			PrintHeap(heaps, i, allocatedSize);
			totalAllocatedSize += allocatedSize;
		}
		a_LogOutput(1, "Total Allocated size = %s", FormatMemorySize(totalAllocatedSize));
		CECUIHelper::GetGameUIMan()->AddChatMessage(
			AS2AC(AString().Format("Total memory size=%s", FormatMemorySize(totalAllocatedSize))),
			GP_CHAT_LOCAL);
	}else{
		a_LogOutput(1, "GetProcessHeaps returns %u", heapCount);
	}
}
#endif

void CDlgChat::OnCommand_speak(const char * szCommand)
{
	PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(GetDlgItem("DEFAULT_Txt_Speech"));

	ACString strText = pEdit->GetText();
	if( strText.GetLength() <= 0 )
	{
		ChangeFocus(NULL);
		return;
	}
	
	int nPack(-1), nSlot(-1);
	CECIvtrItem *pItem = NULL;
	if (pEdit->HasIvtrItem())
	{
		nPack = pEdit->GetIvtrItemPack();
		nSlot = pEdit->GetIvtrItemIndex();
		if (nPack >= 0)
		{
			CECInventory *pPack = GetHostPlayer()->GetPack(nPack);
			if (pPack)
			{
				if (nSlot >= 0)
				{
					pItem = pPack->GetItem(nSlot);
				}
			}
		}
	}

	GetGameRun()->GetUIManager()->FilterBadWords(strText);
	OnCommand_speakmode("speakmode");

	if (strText.Compare(_AL("##debug")) == 0)
	{
		bool bEnable = !glb_IsConsoleEnable();

		//	控制控制台界面是否显示
		glb_EnableConsole(bEnable);

		//	控制命令是否生效
		CECConfigs *pConfigs = 	GetGame()->GetConfigs();
		pConfigs->EnableConsole(bEnable);
		return;
	}
#ifdef _PROFILE_MEMORY
	if (strText.Compare(_AL("##heap")) == 0){
		PrintHeaps();
		return;
	}	
#endif

	strText.TrimLeft();
	int i;
	CHAT_MSG cm;
	DWORD dwTime = GetTickCount();

	if( strText.GetLength() > 2 && strText[0] == '!' && strText[1] == '@' )
	{
		int num = GetHostPlayer()->GetPack()->GetItemTotalNum(12979);
		num += GetHostPlayer()->GetPack()->GetItemTotalNum(36092);
		if( num < 1 || GetHostPlayer()->GetBasicProps().iLevel < 5 )
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(731), GP_CHAT_MISC);
			return;
		}
		if(CECTimeSafeChecker::ElapsedTime(dwTime, m_dwTickFarCry) <= 1000 )
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(730), GP_CHAT_MISC);
			return;
		}
	}
	
	if( strText.GetLength() > 2 && strText[0] == '!' && strText[1] == '#' )
	{
		int num = GetHostPlayer()->GetPack()->GetItemTotalNum(27728);
		num += GetHostPlayer()->GetPack()->GetItemTotalNum(27729);
		if (num < 1)
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(8531), GP_CHAT_MISC);
			return;
		}
		if(CECTimeSafeChecker::ElapsedTime(dwTime, m_dwTickFarCry2) <= 1000 )
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(8530), GP_CHAT_MISC);
			return;
		}
	}

	if( m_vecHistory.size() > 0 )
	{
		cm = m_vecHistory[m_vecHistory.size() - 1];
		if( CECTimeSafeChecker::ElapsedTime(dwTime, cm.dwTime) <= 1000 )
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(272), GP_CHAT_MISC);
			return;
		}

		for( i = 0; i < (int)m_vecHistory.size(); i++ )
		{
			cm = m_vecHistory[i];
			if( cm.cChannel != GP_CHAT_WHISPER &&
				cm.dwTime != 0 &&
				0 == strText.Compare(cm.strMsg) &&
				CECTimeSafeChecker::ElapsedTime(dwTime, cm.dwTime) <= 6000 )
			{
				GetGameUIMan()->AddChatMessage(GetStringFromTable(273), GP_CHAT_MISC);
				return;
			}
		}
	}

	char cChannel;
	const ACHAR *pszMsg;
	ACHAR szName[80], szText[512], szMsg[512] = _AL("");
	CECGameSession *pSession = GetGameSession();
	CECHostPlayer *pHost = GetGameRun()->GetHostPlayer();
	if( strText.GetLength() >= 2 && strText[0] == '!' && strText[1] == '!' )
	{
		cChannel = GP_CHAT_TEAM;
		pszMsg = (const ACHAR *)strText + 2;
		if( *pszMsg == _AL('\0') )
		{
			ChangeFocus(NULL);
			return;
		}
	}
	else if( strText.GetLength() >= 2 && strText[0] == '!' && strText[1] == '~' )
	{
		cChannel = GP_CHAT_FACTION;
		pszMsg = (const ACHAR *)strText + 2;
		if( *pszMsg == _AL('\0') )
		{
			ChangeFocus(NULL);
			return;
		}

		pSession->faction_chat(pszMsg, nPack, nSlot);
	}
	else if( strText.GetLength() >= 2 && strText[0] == '!' && strText[1] == '@' )
	{
		cChannel = GP_CHAT_FARCRY;
		pszMsg = (const ACHAR *)strText + 2;
		if( *pszMsg == _AL('\0') )
		{
			ChangeFocus(NULL);
			return;
		}
		ChangeFocus(NULL);
		PAUIDIALOG pMsgBox;
		GetGameUIMan()->MessageBox("Game_ChatWorld", GetStringFromTable(732), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetText(pszMsg);
		pMsgBox->SetData(nPack);
		pMsgBox->SetDataPtr((void *)nSlot);

		return;
	}
	else if( strText.GetLength() >= 2 && strText[0] == '!' && strText[1] == '#' )
	{
		cChannel = GP_CHAT_SUPERFARCRY;
		pszMsg = (const ACHAR *)strText + 2;
		if( *pszMsg == _AL('\0') )
		{
			ChangeFocus(NULL);
			return;
		}
		ChangeFocus(NULL);
		PAUIDIALOG pMsgBox;
		GetGameUIMan()->MessageBox("Game_ChatWorld2", GetStringFromTable(8532), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);

		// 向消息中附加颜色信息
		ACString strMsg = pszMsg;
		ACString strColor = A3DCOLOR_TO_STRING(m_pPallete->GetColor());
		strColor.CutLeft(1);  // 除去左侧的 '^' 字符以防被过滤掉
		strMsg += strColor;

		// 向消息加附加情绪信息
		int nEmotion(0);
		AString strName;
		PAUIDIALOG pDlgPopface02 = GetGameUIMan()->GetDialog("Win_Popface02");
		for (int i=0; ;++i)
		{
			// 查询哪个 Radio Button 当前处于 check 状态
			strName.Format("Rdo_%d", i);
			PAUIOBJECT pObj = pDlgPopface02->GetDlgItem(strName);
			if (!pObj)
				break;
			PAUICHECKBOX pCheckBox = static_cast<PAUICHECKBOX>(pObj);
			if (pCheckBox->IsChecked())
			{
				nEmotion = i;
				break;
			}
		}
		ACString strEmotion;
		strEmotion.Format(_AL("%02x"), nEmotion);
		strMsg += strEmotion;

		pMsgBox->SetText(strMsg);
		pMsgBox->SetData(nPack);
		pMsgBox->SetDataPtr((void *)nSlot);
		
		return;
	}
	else if( strText.GetLength() >= 1 && strText[0] == '$' )
	{
		pszMsg = (const ACHAR *)strText + 1;
		if( *pszMsg == _AL('\0') )
		{
			ChangeFocus(NULL);
			return;
		}

		if (GetHostPlayer()->HasCountryChannel())
		{
			if( !GetHostPlayer()->IsKing() && GetHostPlayer()->GetMoneyAmount() < 10000 )
			{
				GetGameUIMan()->AddChatMessage(GetStringFromTable(9981), GP_CHAT_MISC);
				return;
			}

			if( GetHostPlayer()->GetCoolTime(GP_CT_COUNTRY_CHAT))
			{
				GetGameUIMan()->AddChatMessage(GetStringFromTable(9982), GP_CHAT_MISC);
				return;
			}
			
			ChangeFocus(NULL);
			if( !GetHostPlayer()->IsKing() )
			{
				PAUIDIALOG pMsgBox;
				GetGameUIMan()->MessageBox("Game_ChatCountry", GetStringFromTable(9983), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
				pMsgBox->SetText(pszMsg);
				pMsgBox->SetData(nPack);
				pMsgBox->SetDataPtr((void *)nSlot);
			}
			else
			{
				DoSendCountryMessage(nPack, nSlot, pszMsg, true);
			}
			return;
		}

		cChannel = GP_CHAT_TRADE;
		if (GetHostPlayer()->GetBasicProps().iLevel <= 30)
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(530), GP_CHAT_MISC);
			ChangeFocus(NULL);
			return;
		}
	}
	else if( strText.GetLength() >= 1 && strText[0] == '/' )
	{
		if( strText[1] == _AL('\0') )
		{
			ChangeFocus(NULL);
			return;
		}
		CSplit s((const ACHAR *)strText + 1);
		CSPLIT_STRING_VECTOR vec = s.Split(_AL(" "));

		if( vec.size() >= 1 )
		{
			if( a_stricmp(_AL("GM"), vec[0]) == 0 )
			{
				CDlgAskHelpToGM *pDlg = GetGameUIMan()->m_pDlgAskHelpToGM;
				//	检查时间限制
#ifndef _DEBUG
				if(CECTimeSafeChecker::ElapsedTimeFor(pDlg->GetData()) < 300 * 1000 )
				{
					GetGameUIMan()->AddChatMessage(GetStringFromTable(612), GP_CHAT_MISC);
					ChangeFocus(NULL);
					return;
				}
#endif
				pDlg->ShowWithHelpMessage(CDlgAskHelpToGM::GM_REPORT_STUCK);
				return;
			}
			
			if( vec.size() == 1 )
			{
				a_strcpy(szMsg, GetStringFromTable(234));
				cChannel = GP_CHAT_MISC;
			}
			else
			{
				pszMsg = (const ACHAR *)strText + vec[0].GetLength() + 2;
				int idPlayer(0);
				GNET::PRIVATE_CHANNEL channel = CECGameTalk::Instance().GetChannelToSend(vec[0], false, true, idPlayer);
				pSession->SendPrivateChatData(vec[0], pszMsg, channel, idPlayer, nPack, nSlot);
				AUI_ConvertChatString(vec[0], szName);
				AUI_ConvertChatString(pszMsg, szText, false);
//				if( a_stricmp(szName, _AL("GM")) == 0 )
//					GetGameUIMan()->AddChatMessage(
//						GetGameUIMan()->GetStringFromTable(569), GP_CHAT_MISC);

				a_sprintf(szMsg, GetStringFromTable(233), szName, szText);
				cChannel = GP_CHAT_WHISPER;
				pEdit->SetText(pEdit->GetText() + vec[0] + _AL(" "));

				CECFriendMan *pFriendMan = g_pGame->GetGameRun()->GetHostPlayer()->GetFriendMan();
				CECFriendMan::FRIEND *pFriend = pFriendMan->GetFriendByName(vec[0]);
				int idFriend = pFriend ? pFriend->id : -1;
				GetGameUIMan()->AddChatMessage(szMsg, cChannel, idFriend, NULL, 0, pHost->GetCurEmotionSet(), pItem ? pItem->Clone() : NULL);
			}
		}
		else
			return;
	}
	else
	{
		cChannel = GetHostPlayer()->HasCountryWarChannel() ? GP_CHAT_BATTLE : GP_CHAT_LOCAL;
		pszMsg = (const ACHAR *)strText;
	}
	
	if( cChannel != GP_CHAT_WHISPER &&
		cChannel != GP_CHAT_FACTION &&
		cChannel != GP_CHAT_MISC )
	{
		pSession->SendChatData(cChannel, pszMsg, nPack, nSlot);
		AUI_ConvertChatString(pszMsg, szText, false);
		AUI_ConvertChatString(pHost->GetName(), szName);
		a_sprintf(szMsg, GetStringFromTable(210), szName, szText);
	}

	if( cChannel != GP_CHAT_WHISPER &&
		a_strlen(szMsg) > 0 )
		GetGameUIMan()->AddChatMessage(szMsg, cChannel, -1, NULL, 0, pHost->GetCurEmotionSet(), pItem ? pItem->Clone() : NULL);

	if( m_vecHistory.size() >= CDLGCHAT_MAX_HISTORY )
		m_vecHistory.erase(m_vecHistory.begin());

	cm.cChannel = cChannel;
	cm.dwTime = dwTime;
	cm.strMsg = strText;
	cm.nPack = nPack;
	cm.nSlot = nSlot;
	m_vecHistory.push_back(cm);
	m_nCurHistory = m_vecHistory.size();

	ChangeFocus(NULL);
}

void CDlgChat::OnCommand_popface(const char * szCommand)
{
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_Speech");
	
	PAUIDIALOG pShow = GetGameUIMan()->GetDialog("Win_Popface");
	PAUIDIALOG pShow1 = GetGameUIMan()->GetDialog("Win_Popface01");
	PAUIDIALOG pShow2 = GetGameUIMan()->GetDialog("Win_Popface02");

	if (m_szName == "Win_Chat" && IsSuperFarCryMode())
	{
		// 号角频道采用单独表情界面
		//
		pShow->Show(false);

		if (pShow1->IsShow() || pShow2->IsShow())
		{
			pShow1->Show(false);
			pShow2->Show(false);
		}
		else
		{
			pShow1->Show(true);
		}

		pShow1->AlignTo(this,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_RIGHT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
		pShow1->SetDataPtr(this, "ptr_AUIDialog");

		pShow2->AlignTo(this,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_RIGHT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
		pShow2->SetDataPtr(this, "ptr_AUIDialog");
	}
	else
	{
		pShow1->Show(false);
		pShow2->Show(false);

		pShow->Show(!pShow->IsShow());
		pShow->AlignTo(this,
			AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_RIGHT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
		pShow->SetDataPtr(this, "ptr_AUIDialog");
	}
	

	GetGameUIMan()->BringWindowToTop(this);
	ChangeFocus(pEdit);
}

void CDlgChat::OnCommand_speakmode(const char * szCommand)
{
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_Speech");
	PAUICOMBOBOX pCombo = (PAUICOMBOBOX)GetDlgItem("Txt_Speakway");
	int nSel = pCombo->GetCurSel();
	static ACHAR *a_szFlag[] = { _AL("!#"), _AL("!@"), _AL("$"), _AL("!~"), _AL("!!"), _AL("/"), _AL("") };

	pEdit->SetText(a_szFlag[nSel]);
	GetGameUIMan()->BringWindowToTop(this);
	ChangeFocus(pEdit);
}

void CDlgChat::OnCommand_channel(const char * szCommand)
{
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_Speech");
	GetGameUIMan()->m_pDlgSettingCurrent->OnCommandSetting("gamesetting");
	GetGameUIMan()->BringWindowToTop(this);
	ChangeFocus(pEdit);
}

void CDlgChat::OnCommand_chgcolor(const char * szCommand)
{
	int i;
	char szName[40];
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_Speech");

	m_nCurChatColor = (m_nCurChatColor + 2) % CDLGCHAT_MAX_CHAT_COLORS;
	for( i = 0; i < CDLGCHAT_MAX_CHAT_COLORS; i++ )
	{
		sprintf(szName, "Img_Back%d", i);
		GetDlgItem(szName)->Show(i == m_nCurChatColor);
		GetGameUIMan()->GetDialog("Win_ChatSmall")->GetDlgItem(szName)->Show(i == m_nCurChatColor);
		GetGameUIMan()->GetDialog("Win_ChatBig")->GetDlgItem(szName)->Show(i == m_nCurChatColor);
	}

	GetGameUIMan()->BringWindowToTop(this);
	ChangeFocus(pEdit);
}

void CDlgChat::OnCommand_chgsize(const char * szCommand)
{
	SIZE s;
	POINT ptPos;
	int a_nHeight[] = { 128, 217, 400, 60 };
	int nHeightOld = a_nHeight[m_nChatWinSize];
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_Speech");

	m_nChatWinSize = (m_nChatWinSize + 1) % (sizeof(a_nHeight) / sizeof(int));
	int nHeightChanged = a_nHeight[m_nChatWinSize] - nHeightOld;

	SIZE s1 = GetDefaultSize();
	s1.cy += nHeightChanged;
	SetDefaultSize(s1.cx, s1.cy);

	SIZE s2 = GetSize();

	float fScale = GetGameUIMan()->GetWindowScale();

	ptPos = GetPos();
	ptPos.y -= int(nHeightChanged * fScale);
	
	//  old : SetPos(ptPos.x, ptPos.y);
	SetPosEx(ptPos.x, ptPos.y);

	PAUIOBJECTLISTELEMENT pElement = GetFirstControl();
	while( pElement )
	{
		if( strstr(pElement->pThis->GetName(), "Img_Back") ||
			0 == stricmp(pElement->pThis->GetName(), "Txt_Content") )
		{
			s = pElement->pThis->GetSize();
			pElement->pThis->SetSize(s2.cx, (int)(a_nHeight[m_nChatWinSize]*fScale));
		}
		else
		{
			ptPos = pElement->pThis->GetPos(true);
			ptPos.y += int(nHeightChanged * fScale);
			pElement->pThis->SetPos(ptPos.x, ptPos.y);
		}

		pElement = GetNextControl(pElement);
	}

	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
	pText->ScrollToTop();
	pText->ScrollToBottom();

	//	重新处理对话框对齐，以处理大小调整带来的相互遮挡问题
	ReAlignChatDialogs();

	GetGameUIMan()->BringWindowToTop(this);
	ChangeFocus(pEdit);
}

void CDlgChat::OnCommand_clear(const char * szCommand)
{
	PAUITEXTAREA pText;
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_Speech");

	pText = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
	pText->SetText(_AL(""));
	pText->ScrollToTop();

	pText = dynamic_cast<PAUITEXTAREA>(GetGameUIMan()->GetDialog("Win_ChatSmall")->GetDlgItem("Txt_Content"));
	pText->SetText(_AL(""));
	pText->ScrollToTop();

	pText = dynamic_cast<PAUITEXTAREA>(GetGameUIMan()->GetDialog("Win_ChatBig")->GetDlgItem("Txt_Content"));
	pText->SetText(_AL(""));
	
	m_vecChatMsg.clear();

	GetGameUIMan()->BringWindowToTop(this);
	ChangeFocus(pEdit);
}

void CDlgChat::OnCommand_mini(const char * szCommand)
{
	char szName[40];
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_Speech");
	PAUICHECKBOX pCheck = (PAUICHECKBOX)GetDlgItem("Chk_Mini");

	//GetDlgItem("Movebar")->Show(!pCheck->IsChecked());
	GetDlgItem("Txt_Content")->Show(!pCheck->IsChecked());

	sprintf(szName, "Img_Back%d", m_nCurChatColor);
	GetDlgItem(szName)->Show(!pCheck->IsChecked());

	GetGameUIMan()->BringWindowToTop(GetGameUIMan()->GetDialog("Win_Chat"));
	GetGameUIMan()->GetDialog("Win_Chat")->ChangeFocus(pEdit);
}

void CDlgChat::OnCommand_set(const char * szCommand)
{
	int nNewSet = atoi(szCommand + strlen("set")) - 1;
	PAUICOMBOBOX pCombo = (PAUICOMBOBOX)GetDlgItem("Txt_Speakway");
	int nSel = pCombo->GetCurSel();
	if( nSel != 0 && nNewSet == m_nActiveChannelSet ) return;

	char szName[40];
	PAUITEXTAREA pText;
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_Speech");
	ACHAR *a_pszPrefix[] = { _AL(""), _AL("!!"), _AL("!~"), _AL("/"), _AL("$"), _AL("!@"), _AL("!#") };

	pEdit->SetText(a_pszPrefix[nNewSet]);
	if( nNewSet < 5 )
	{
		if( m_nActiveChannelSet >= 0 )
		{
			sprintf(szName, "Btn_Set%d", m_nActiveChannelSet + 1);
			GetDlgItem(szName)->SetColor(A3DCOLORRGB(255, 255, 255));
			((PAUISTILLIMAGEBUTTON)GetDlgItem(szName))->SetPushed(false);
		}

		m_nActiveChannelSet = nNewSet;
		OnEventChar_Txt_Speech(0, 0, pEdit);

		sprintf(szName, "Btn_Set%d", m_nActiveChannelSet + 1);
		GetDlgItem(szName)->SetColor(A3DCOLORRGB(255, 203, 74));
		((PAUISTILLIMAGEBUTTON)GetDlgItem(szName))->SetPushed(true);

		pText = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
		RebuildChatContents(pText, m_nActiveChannelSet);

		if( m_bLocked )
			OnCommand_lock("");
	}
	else
	{
		OnEventChar_Txt_Speech(0, 0, pEdit);
	}

	GetGameUIMan()->BringWindowToTop(this);
	ChangeFocus(pEdit);
}

void CDlgChat::OnCommand_CANCEL(const char * szCommand)
{
	if( m_szName == "Win_Popface" ||
		m_szName == "Win_WhisperChat" ||
		m_szName == "Win_WhisperChoose")
	{
		SetData(0);
		Show(false);
	}
	else if (m_szName == "Win_Popface01" ||
		m_szName == "Win_Popface02")
	{
		Show(false);
	}
	else
	{
		PAUIOBJECT pEdit = GetGameUIMan()->m_pDlgChat->GetDlgItem("DEFAULT_Txt_Speech");
		if( pEdit->IsFocus() )
		{
			pEdit->SetText(_AL(""));
			ChangeFocus(NULL);
		}
		else
			GetGameUIMan()->RespawnMessage();
	}
}

void CDlgChat::OnCommandMinimize(const char *szCommand)
{
	ACString strName = GetGameRun()->GetPlayerName(GetData(), true);
	GetGameUIMan()->GetMiniBarMgr()->MinimizeDialog(this, 
		CECMiniBarMgr::BarInfo(strName, A3DCOLORRGB(255, 74, 176)));
}

void CDlgChat::OnCommand_popface01(const char *szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Popface01");
	PAUIDIALOG pDlg2 = GetGameUIMan()->GetDialog("Win_Popface02");
	if (pDlg2->IsShow() && !pDlg->IsShow())
	{
		pDlg->Show(true);
		POINT pos = pDlg2->GetPos();
		pDlg->SetPosEx(pos.x, pos.y);
		pDlg2->Show(false);
	}
}

void CDlgChat::OnCommand_popface02(const char *szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Popface02");
	PAUIDIALOG pDlg2 = GetGameUIMan()->GetDialog("Win_Popface01");
	if (pDlg2->IsShow() && !pDlg->IsShow())
	{
		pDlg->Show(true);
		POINT pos = pDlg2->GetPos();
		pDlg->SetPosEx(pos.x, pos.y);
		pDlg2->Show(false);
	}
}

void CDlgChat::RebuildChatContents(PAUITEXTAREA pText, int nChannelSet)
{
	int i;
	EC_GAME_SETTING gs = g_pGame->GetConfigs()->GetGameSettings();

	pText->SetText(_AL(""));
	pText->ScrollToTop();
	if( nChannelSet < EC_USERCHANNEL_NUM - 1 )
		m_nChatMsgCount1 = 0;
	else
		m_nChatMsgCount2 = 0;

	ACString strName;
	A3DCOLOR clrName;

	for( i = 0; i < (int)m_vecChatMsg.size(); i++ )
	{
		if( !gs.bChannel[nChannelSet][m_vecChatMsg[i].cChannel] )
			continue;

		if( a_strlen(pText->GetText()) > 0 )
			pText->AppendText(_AL("\r"));
		//pText->AppendText(GetChatColor(m_vecChatMsg[i].cChannel));

		CDlgChat::CHAT_MSG &msg = m_vecChatMsg[i];
		GetGameUIMan()->TransformNameColor(msg.pItem, strName, clrName);
		pText->AppendText(msg.strMsg, msg.nMsgIndex, strName, clrName);

		if( nChannelSet < EC_USERCHANNEL_NUM - 1 )
			m_nChatMsgCount1++;
		else
			m_nChatMsgCount2++;
	}
	
	pText->ScrollToBottom();
}

int CDlgChat::GetChatWinSize()
{
	return m_nChatWinSize;
}

int CDlgChat::GetChatColor()
{
	return m_nCurChatColor;
}

int CDlgChat::GetChannelSet()
{
	return m_nCurChannelSet;
}

int CDlgChat::GetActiveChannelSet()
{
	return m_nActiveChannelSet;
}

abase::vector<CDlgChat::CHAT_MSG> & CDlgChat::GetAllChatMsgs()
{
	return m_vecChatMsg;
}

abase::vector<CDlgChat::LINKED_MSG> & CDlgChat::GetWhisperChatMsgs()
{
	return m_whisperChatMsg;
}

abase::vector<CDlgChat::CHAT_MSG> & CDlgChat::GetSuperFarCryMsgs()
{
	return m_vecSuperFarCryMsg;
}

int CDlgChat::GetMsgCount(int nWin)
{
	return 1 == nWin ? m_nChatMsgCount1 : m_nChatMsgCount2;
}

void CDlgChat::IncreaseMsgCount(int nWin)
{
	if( 1 == nWin )
		m_nChatMsgCount1++;
	else
		m_nChatMsgCount2++;
}

void CDlgChat::OnEventMouseMove_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	AUITEXTAREA_NAME_LINK Link;	
	GetNameLinkMouseOn(x, y, pObj, &Link);
	if( Link.strName.GetLength() == 0 )
	{
		AUITEXTAREA_EDITBOX_ITEM Item;
		GetItemLinkItemOn(x, y, pObj, &Item);
	}
}

void CDlgChat::OnEventLButtonDown_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	// check whether clicked a name link
	bool bClickUserName = false;
	AUITEXTAREA_NAME_LINK Link;
	bool bClickChatPart = GetNameLinkMouseOn(x, y, pObj, &Link);
	if (bClickChatPart && Link.strName.GetLength() != 0 )
	{
		bClickUserName = true;
		CDlgChat *pChat = GetGameUIMan()->m_pDlgChat;
		pChat->SwitchToPrivateChat(Link.strName);
	}
	else
	{
		// check whether clicked a editbox item
		AUITEXTAREA_EDITBOX_ITEM Item;
		Item.m_pItem = NULL;
		if( GetItemLinkItemOn(x, y, pObj, &Item) && Item.m_pItem != NULL )
		{
			bClickUserName = true;
			switch (Item.m_pItem->GetType())
			{
			case enumEIIvtrlItem:
				{
					bool bFound(false);
					size_t i(0);
					if (!bFound)
					{
						for (i = 0; i < m_vecChatMsg.size(); i++)
						{
							if (m_vecChatMsg[i].nMsgIndex == Item.m_pItem->GetMsgIndex() )
							{
								CECIvtrItem *pItem = m_vecChatMsg[i].pItem;
								GetGameUIMan()->OnLinkItemClicked(pItem, wParam, lParam);
								bFound = true;
								break;
							}
						}
					}
					if (!bFound)
					{
						for (i = 0; i < m_whisperChatMsg.size(); i++)
						{
							if (m_whisperChatMsg[i].nMsgIndex == Item.m_pItem->GetMsgIndex() )
							{
								CECIvtrItem *pItem = m_whisperChatMsg[i].pItem;
								GetGameUIMan()->OnLinkItemClicked(pItem, wParam, lParam);
								bFound = true;
								break;
							}
						}
					}
				}
				break;
			case enumEICoord:
				CECUIHelper::FollowCoord(Item.m_pItem);
				break;
			}
		}
	}

	if (bClickChatPart && !bClickUserName)
		GetGameUIMan()->ContinueDealMessage();
}

void CDlgChat::OnEventLButtonUp_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	// check whether clicked a name link
	bool bClickUserName = false;
	AUITEXTAREA_NAME_LINK Link;
	bool bClickChatPart = GetNameLinkMouseOn(x, y, pObj, &Link);
	if( !bClickChatPart || Link.strName.GetLength() == 0 )
	{
		// check whether clicked a editbox item
		AUITEXTAREA_EDITBOX_ITEM Item;
		Item.m_pItem = NULL;
		if(GetItemLinkItemOn(x, y, pObj, &Item) && Item.m_pItem != NULL)
			bClickUserName = true;
		
		ChangeFocus(NULL);
	}
	else
	{
		bClickUserName = true;
	}

	if (bClickChatPart && !bClickUserName)
		GetGameUIMan()->ContinueDealMessage();
}

void CDlgChat::OnEventRButtonUp_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	AUITEXTAREA_NAME_LINK Link;
	GetNameLinkMouseOn(x, y, pObj, &Link);
	if( Link.strName.GetLength() == 0 ) return;

	GNET::Privilege *pPrivilege = GetGame()->GetPrivilege();
	int idPlayer = GetGameRun()->GetPlayerID(Link.strName);

	if( (wParam & MK_CONTROL) && pPrivilege->Has_Toggle_NameID() )
	{
		ACString strText;

		strText.Format(_AL("ID of %s: %d"), Link.strName, idPlayer);
		GetGameUIMan()->MessageBox("", strText, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else if( idPlayer > 0 )
	{
		x -= p->X;
		y -= p->Y + GetGameUIMan()->GetDialog("Win_QuickAction")->GetSize().cy;
		GetGameUIMan()->PopupPlayerContextMenu(idPlayer, x, y);
	}
}

bool CDlgChat::GetNameLinkMouseOn(int x, int y,
	PAUIOBJECT pObj, P_AUITEXTAREA_NAME_LINK pLink)
{
	bool bClickedChatPart = false;
	A3DRECT rcWindow = m_pAUIManager->GetRect();
	POINT ptPos = pObj->GetPos();
	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(pObj);

	GetGame()->ChangeCursor(RES_CUR_NORMAL);
	if( pText->GetHitArea(x - ptPos.x, y - ptPos.y) == AUITEXTAREA_RECT_FRAME )
	{
		int i;
		abase::vector<AUITEXTAREA_NAME_LINK> &vecNameLink = pText->GetNameLink();

		x += rcWindow.left;
		y += rcWindow.top;
		for( i = 0; i < (int)vecNameLink.size(); i++ )
		{
			if( vecNameLink[i].rc.PtInRect(x, y) )
			{
				GetGame()->ChangeCursor(RES_CUR_HAND);
				*pLink = vecNameLink[i];
				break;
			}
		}
		bClickedChatPart = true;
	}

	return bClickedChatPart;
}


bool CDlgChat::GetItemLinkItemOn(int x, int y,
	PAUIOBJECT pObj, AUITEXTAREA_EDITBOX_ITEM *pLink)
{
	bool bClickedChatPart = false;
	A3DRECT rcWindow = m_pAUIManager->GetRect();
	POINT ptPos = pObj->GetPos();
	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(pObj);

	GetGame()->ChangeCursor(RES_CUR_NORMAL);
	if( pText->GetHitArea(x - ptPos.x, y - ptPos.y) == AUITEXTAREA_RECT_FRAME )
	{
		int i;
		abase::vector<AUITEXTAREA_EDITBOX_ITEM> &vecItemLink = pText->GetEditBoxItems();

		x += rcWindow.left;
		y += rcWindow.top;
		for( i = 0; i < (int)vecItemLink.size(); i++ )
		{
			if( (vecItemLink[i].m_pItem->GetType() == enumEIIvtrlItem ||
				vecItemLink[i].m_pItem->GetType() == enumEICoord) &&
				vecItemLink[i].rc.PtInRect(x, y) )
			{
				GetGame()->ChangeCursor(RES_CUR_HAND);
				*pLink = vecItemLink[i];
				break;
			}
		}
		bClickedChatPart = true;
	}

	return bClickedChatPart;
}

void CDlgChat::OnEventChar_Txt_Speech(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( m_szName == "Win_Chat" )
		DetermineChannelByText(pObj->GetText());
}

void CDlgChat::OnEventKeyDown_Txt_Speech(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( m_szName == "Win_Chat" )
	{
		if (wParam == VK_ESCAPE)
		{
			ChangeFocus(NULL);
			return;
		}

		PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(pObj);

		if( wParam == VK_UP && m_nCurHistory > 0 )
		{
			m_nCurHistory--;
			const CHAT_MSG &msg = m_vecHistory[m_nCurHistory];
			pEdit->SetText(msg.strMsg);
			if( msg.nPack >= 0 )
			{
				CECInventory *pPack = GetHostPlayer()->GetPack(msg.nPack);
				if( pPack )
				{
					if (msg.nSlot >= 0)
					{
						CECIvtrItem *pItem = pPack->GetItem(msg.nSlot);
						if( pItem )
						{
							ACString strName;
							A3DCOLOR clrName;
							GetGameUIMan()->TransformNameColor(pItem, strName, clrName);
							pEdit->SetTextAndItem(msg.strMsg, strName, clrName);
							pEdit->SetIvtrItemLocInfo(msg.nPack, msg.nSlot);
						}
					}
				}
			}
		}
		else if( wParam == VK_DOWN && m_nCurHistory < (int)m_vecHistory.size() )
		{
			m_nCurHistory++;
			if( m_nCurHistory < (int)m_vecHistory.size() )
			{
				const CHAT_MSG &msg = m_vecHistory[m_nCurHistory];
				pEdit->SetText(msg.strMsg);
				if( msg.nPack >= 0 )
				{
					CECInventory *pPack = GetHostPlayer()->GetPack(msg.nPack);
					if( pPack )
					{
						if (msg.nSlot >= 0)
						{
							CECIvtrItem *pItem = pPack->GetItem(msg.nSlot);
							if( pItem )
							{
								ACString strName;
								A3DCOLOR clrName;
								GetGameUIMan()->TransformNameColor(pItem, strName, clrName);
								pEdit->SetTextAndItem(msg.strMsg, strName, clrName);
								pEdit->SetIvtrItemLocInfo(msg.nPack, msg.nSlot);
							}
						}
					}
				}
			}
			else
			{
				pEdit->SetText(_AL(""));
			}
		}

		DetermineChannelByText(pObj->GetText());

	}
	else if( m_szName == "Win_WhisperChat" )
	{
		if( int(wParam) == VK_RETURN && AUI_PRESS(VK_CONTROL) ) 
			OnCommand_send("send");
	}
}

void CDlgChat::OnEventSysChar_Txt_Speech(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( m_szName == "Win_WhisperChat" )
	{
		if( (TCHAR(wParam) == 's' || TCHAR(wParam) == 'S') )
			OnCommand_send("send");
	}
}

void CDlgChat::DetermineChannelByText(const ACHAR *pszText)
{
	int i;
	PAUICOMBOBOX pCombo = (PAUICOMBOBOX)GetDlgItem("Txt_Speakway");
	static ACHAR *a_szFlag[] = { _AL("!#"), _AL("!@"), _AL("$"), _AL("!~"), _AL("!!"), _AL("/") };

	pCombo->SetCurSel(pCombo->GetCount()-1);
	for( i = 0; i < sizeof(a_szFlag) / sizeof(ACHAR *); i++ )
	{
		if( a_strstr(pszText, a_szFlag[i]) == pszText )
		{
			pCombo->SetCurSel(i);
			break;
		}
	}
}

void CDlgChat::OnEventLButtonUp_Face(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PAUIDIALOG pChat;
	PAUIEDITBOX pEdit;
	int nIndex = atoi(pObj->GetName() + strlen("Face_"));

	if( GetDataPtr("ptr_AUIDialog") )
		pChat = (PAUIDIALOG)GetDataPtr("ptr_AUIDialog");
	else
		pChat = GetGameUIMan()->m_pDlgChat;

	if( strstr(pChat->GetName(), "Win_FriendChat") )
		pEdit = dynamic_cast<PAUIEDITBOX>(pChat->GetDlgItem("Txt_Content"));
	else if( stricmp(pChat->GetName(), "Win_ChannelChat") == 0 )
		pEdit = dynamic_cast<PAUIEDITBOX>(pChat->GetDlgItem("Txt_Content"));
	else
		pEdit = dynamic_cast<PAUIEDITBOX>(pChat->GetDlgItem("DEFAULT_Txt_Speech"));

	int nEmotion = GetHostPlayer()->GetCurEmotionSet();
	if (m_szName == "Win_Popface01")
		nEmotion = SUPER_FAR_CRY_EMOTION_SET;
	if( nIndex < (int)GetGameUIMan()->m_vecEmotion[nEmotion]->size() )
	{
		if (pEdit->GetItemCount(enumEIEmotion) < 3)
			pEdit->AppendItem(enumEIEmotion, 0, L"W", MarshalEmotionInfo(nEmotion, nIndex));
	}

	GetGameUIMan()->BringWindowToTop(pChat);
	pChat->ChangeFocus(pEdit);
}

void CDlgChat::OnLButtonDownPicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	OnMouseMovePicPallete(MK_LBUTTON, lParam, pObj);
}

void CDlgChat::OnMouseMovePicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (!(wParam & MK_LBUTTON) || !m_pPallete)
		return;
	
	A3DVIEWPORTPARAM* param = GetGame()->GetViewport()->GetA3DViewport()->GetParam();
	POINT pt = 
	{
		GET_X_LPARAM(lParam) - m_pPicPane->GetPos().x - param->X,
		GET_Y_LPARAM(lParam) - m_pPicPane->GetPos().y - param->Y
	};	
	m_pPallete->MoveIndicator(pt);
}

bool CDlgChat::IsLocked()
{
	return m_bLocked;
}

void CDlgChat::OnCommand_send(const char * szCommand)
{
	PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(GetDlgItem("DEFAULT_Txt_Speech"));
	ACString strText = pEdit->GetText();
	if( strText.GetLength() <= 0 ) return;
	GetGameRun()->GetUIManager()->FilterBadWords(strText);
	
	// 获取 LINK 的物品
	int nPack(-1), nSlot(-1);
	CECIvtrItem *pItem = NULL;
	if (pEdit->HasIvtrItem())
	{
		nPack = pEdit->GetIvtrItemPack();
		nSlot = pEdit->GetIvtrItemIndex();
		if (nPack >= 0)
		{
			CECInventory *pPack = GetHostPlayer()->GetPack(nPack);
			if (pPack)
			{
				if (nSlot >= 0)
				{
					pItem = pPack->GetItem(nSlot);
				}
			}
		}
	}

	int idPlayer = GetData();
	const ACHAR *pszName = GetGameRun()->GetPlayerName(idPlayer, true);
	GNET::PRIVATE_CHANNEL channel = CECGameTalk::Instance().GetChannelToSend(idPlayer, false, true);
	GetGameSession()->SendPrivateChatData(pszName, strText, channel, idPlayer, nPack, nSlot);

	ACHAR szName[80], szText[512], szMsg[512];
	AUI_ConvertChatString(pszName, szName);
	AUI_ConvertChatString(strText, szText, false);
	a_sprintf(szMsg, GetStringFromTable(233), szName, szText);
	GetGameUIMan()->AddChatMessage(szMsg, GP_CHAT_WHISPER, idPlayer, NULL, 0, GetHostPlayer()->GetCurEmotionSet(), pItem ? pItem->Clone() : NULL);

	pEdit->SetText(_AL(""));
	ChangeFocus(pEdit);
}

void CDlgChat::OnCommand_list(const char * szCommand)
{
}

void CDlgChat::OnCommand_createchannel(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_ChannelCreate");
	if( pDlg )
		pDlg->Show(!pDlg->IsShow());
}

void CDlgChat::OnCommand_joinchannel(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_ChannelJoin");
	if( pDlg  )
		pDlg->Show(!pDlg->IsShow());
}

void CDlgChat::OnShowDialog()
{
	CDlgBase::OnShowDialog();
	
	if( m_szName == "Win_WhisperChat" )
	{
		GetGameUIMan()->GetMiniBarMgr()->RestoreDialog(this);
		ChangeFocus(GetDlgItem("DEFAULT_Txt_Speech"));
	}
}

void CDlgChat::OnTick()
{
	if( stricmp(m_szName, "Win_Popface") == 0)
	{
		PAUIDIALOG pDlg = (PAUIDIALOG)GetDataPtr("ptr_AUIDialog");
		if (pDlg)
		{
			if (!pDlg->IsShow())
				Show(false);

			if (IsShow() &&
				!stricmp(pDlg->GetName(), "Win_Chat") &&
				IsSuperFarCryMode())
			{
				// 切换使用号角频道专用表情
				GetGameUIMan()->m_pDlgChat->OnCommand_popface("");
			}
		}
	}
	else if (stricmp(m_szName, "Win_Popface01") == 0
		|| stricmp(m_szName, "Win_Popface02") == 0)
	{
		PAUIDIALOG pDlg = (PAUIDIALOG)GetDataPtr("ptr_AUIDialog");
		if (pDlg)
		{
			if (!pDlg->IsShow())
				Show(false);

			if (IsShow() &&
				!stricmp(pDlg->GetName(), "Win_Chat") &&
				!IsSuperFarCryMode())
			{
				// 切换使用非号角频道表情
				GetGameUIMan()->m_pDlgChat->OnCommand_popface("");
			}
		}
	}
	else if (stricmp(m_szName, "Win_ChatBig") == 0)
	{
		// 更新号角消息

		const int SUPER_FAR_CRY_REPEAT = 3;
		const int SUPER_FAR_CRY_SHOW_TIME = 3000; // in ms
		const int SUPER_FAR_CRY_HIDE_TIME = 500;  // in ms
		const int SUPER_FAR_CRY_SHOW_EXTEND_TIME = 20000; // in ms
		DWORD dwTickCount = GetTickCount();
		if (m_curSuperFarCryMsgValid)
		{
			// 更新当前号角消息显示状态

			DWORD dwElapsed = CECTimeSafeChecker::ElapsedTime(dwTickCount, m_curSuperFarCryTick);
			while (true)
			{
				if (m_curSuperFarCryShowMode == true)
				{
					// 当前处于显示状态，判断是否进入隐藏状态
					if (m_curSuperFarCryShowTime+1<SUPER_FAR_CRY_REPEAT &&   // 前几次，并且显示时间已经结束即隐藏
						dwElapsed >= SUPER_FAR_CRY_SHOW_TIME ||
						m_curSuperFarCryShowTime+1>=SUPER_FAR_CRY_REPEAT &&  // 最后一次，
						(!m_vecSuperFarCryMsg.empty() && dwElapsed >= SUPER_FAR_CRY_SHOW_TIME ||     // 有后续队列时，并且显示结束时即隐藏
						m_vecSuperFarCryMsg.empty() && dwElapsed >= SUPER_FAR_CRY_SHOW_EXTEND_TIME)) // 无后续队列时，显示延长时间已到
					{
						m_curSuperFarCryShowMode = false;
						m_curSuperFarCryTick = dwTickCount;
					}
				}
				else
				{
					// 当前处于隐藏状态，判断是否进入下一次显示
					if (dwElapsed >= SUPER_FAR_CRY_HIDE_TIME)
					{
						m_curSuperFarCryShowTime++;
						m_curSuperFarCryShowMode = true;
						m_curSuperFarCryTick = dwTickCount;
					}
				}

				if (m_curSuperFarCryShowTime+1 > SUPER_FAR_CRY_REPEAT)
				{
					// 已经完成最后一次的隐藏
					m_curSuperFarCryMsgValid = false;
					break;
				}

				break;
			}
		}
		if (!m_curSuperFarCryMsgValid && !m_vecSuperFarCryMsg.empty())
		{
			// 开始处理新的号角消息

			m_curSuperFarCryMsg = m_vecSuperFarCryMsg[0];
			m_vecSuperFarCryMsg.erase(m_vecSuperFarCryMsg.begin());
			m_curSuperFarCryShowMode = true;
			m_curSuperFarCryShowTime = 0;
			m_curSuperFarCryTick = dwTickCount;
			m_curSuperFarCryMsgValid = true;
		}

		UpdateSuperFarCryMsg(false);
	}
}

void CDlgChat::UpdateSuperFarCryMsg(bool bForceUpdateContent)
{
	if (stricmp(m_szName, "Win_ChatBig") != 0)
		return;
	
	// 根据当前号角消息更新界面
	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
	if (m_curSuperFarCryMsgValid && m_curSuperFarCryShowMode == true)
	{
		ACString strTextOld = pText->GetText();
		if (bForceUpdateContent || m_curSuperFarCryMsg.strMsg != strTextOld)
		{
			ACString strName;
			A3DCOLOR clrName;
			GetGameUIMan()->TransformNameColor(m_curSuperFarCryMsg.pItem, strName, clrName);
			pText->SetText(_AL(""));
			pText->AppendText(m_curSuperFarCryMsg.strMsg, m_curSuperFarCryMsg.nMsgIndex, strName, clrName);
		}
		
		FitSizeToSuperFarCryMsg(true);
	}
	else
		pText->SetText(_AL(""));
}

void CDlgChat::FitSizeToSuperFarCryMsg(bool bAdjustPos)
{
	// 调整对话框的大小，使容纳当前号角内容
	//

	if (stricmp(m_szName, "Win_ChatBig") != 0)
		return;
	
	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
	int height = pText->GetRequiredHeight();
	if (height>0)
	{
		POINT pos = GetPos();
		SIZE size = GetSize();
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		
		int deltaHeight = size.cy - height;
		if (deltaHeight == 0)
		{
			//	已经是正确状态，不需要重复调整
			return;
		}
		
		if (bAdjustPos)
		{
			// 调整对话框的位置，使与上一次底部对齐
			pos.y += deltaHeight;
			if (pos.y<0)
				pos.y = 0;
			else if (pos.y+height >= (int)(p->Height))
				pos.y = p->Height-height-1;
			SetPosEx(pos.x, pos.y);
		}
		
		// 调整大小，使与当前号角内容高度一致
		SetSize(size.cx, height);
		PAUIOBJECTLISTELEMENT pElement = GetFirstControl();
		while (pElement)
		{
			SIZE size2 = pElement->pThis->GetSize();
			pElement->pThis->SetSize(size2.cx, height);
			pElement = GetNextControl(pElement);
		}
	}
}

bool CDlgChat::Render()
{
	bool bval = CDlgBase::Render();
	if( stricmp(m_szName, "Win_Chat") == 0 )
	{
		PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
		if( pText->GetFirstLine() != (int)pText->GetData() )
			if( m_bLocked ) 
			{
				if( pText->GetFirstLine() + pText->GetLinesPP() == pText->GetLines() )
					OnCommand_lock("");
			}
			else if( pText->GetFirstLine() + pText->GetLinesPP() != pText->GetLines() )
				OnCommand_lock("");
		pText->SetData(pText->GetFirstLine());
	}
	return bval;
}

bool CDlgChat::IsSuperFarCryMode()
{
	bool ret(false);
	CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUIMan)
	{
		CDlgChat *pChat = static_cast<CDlgChat *>(pGameUIMan->GetDialog("Win_Chat"));
		PAUIOBJECT pEdit = pChat->GetDlgItem("DEFAULT_Txt_Speech");		
		const ACHAR * strText = pEdit->GetText();
		if (strText!=NULL && a_strstr(strText, _AL("!#"))==strText)
			ret = true;
	}
	return ret;
}

bool CDlgChat::IsHaveFarCryMsg()
{
	// 判断当前是否会有号角内容（要）显示
	return !m_vecSuperFarCryMsg.empty()  // 号角里有其它消息要显示
		|| m_curSuperFarCryMsgValid;     // 最后一条消息还在处理中
}

void CDlgChat::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	// 调用基类处理
	CDlgBase::Resize(rcOld, rcNew);

	// 重新生成聊天内容，使表情大小跟上变化
	//
	if( stricmp(m_szName, "Win_Chat") == 0 )
	{
		if (GetActiveChannelSet() >= 0)
		{
			PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
			RebuildChatContents(pText, GetActiveChannelSet());
		}
	}
	else if (stricmp(m_szName, "Win_ChatBig") == 0)
	{
		UpdateSuperFarCryMsg(true);
	}
	else if (stricmp(m_szName, "Win_WhisperChat") == 0)
	{
		PAUITEXTAREA pTxt_Chat = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Chat"));
		if (pTxt_Chat)
		{
			ACString strText = pTxt_Chat->GetText();
			pTxt_Chat->SetText(strText);
			pTxt_Chat->ScrollToTop();
			pTxt_Chat->ScrollToBottom();
		}
	}
}

void CDlgChat::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
	{
		if( m_szName == "Win_Chat" )
		{
			m_nChatWinSize = 0;
			SwitchCountryChannel();
		}

		//	调整 Win_ChatSmall 和 Win_ChatBig 的位置
		//	以免因尺寸变化带来相互遮挡
		//	在Win_Chat、Win_ChatSmall、Win_ChatBig三者调整结束时都调用此处处理
		//	以解决三个文件更换界面的先后关系不同带来的问题
		if (m_szName == "Win_Chat" ||
			m_szName == "Win_ChatSmall" ||
			m_szName == "Win_ChatBig")
		{
			ReAlignChatDialogs();
		}
	}
}

void CDlgChat::ReAlignChatDialogs()
{
	//	将聊天栏、系统消息栏、号角栏的位置重新调整，以处理相互遮挡带来的不便等问题
	//

	//	在进行对齐操作前，先需要匹配内容大小和对话框本身大小
	//	以避免匹配大小后，在Tick中又会进行调整，导致匹配过程失效
	CDlgChat *pChatBig = GetGameUIMan()->m_pDlgChat3;
	pChatBig->FitSizeToSuperFarCryMsg(false);
	
	//	Win_ChatSmall 在Win_ChatBig 顶端左对齐
	CDlgChat *pChat = GetGameUIMan()->m_pDlgChat;
	CDlgChat *pChatSmall = GetGameUIMan()->m_pDlgChat2;
	pChatSmall->AlignTo(pChat, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP, 0, -2);
	
	//	Win_ChatBig 在 Win_ChatSmall 顶端左对齐
	pChatBig->AlignTo(pChatSmall, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP, 0, -2);
}

void CDlgChat::SwitchToPrivateChat(const ACHAR* szUserName)
{
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_Speech");
	ACString strText = _AL("/") + ACString(szUserName) + _AL(" ");

	pEdit->SetText(strText);
	DetermineChannelByText(strText);

	GetGameUIMan()->BringWindowToTop(this);
	ChangeFocus(pEdit);
}

void CDlgChat::SwitchCountryChannel()
{
	if (m_szName != "Win_Chat") return;
	CECHostPlayer *pHost = GetHostPlayer();

	PAUIOBJECT pObjChannel = GetDlgItem("Btn_Set1");
	if (pObjChannel)
	{
		int idText(0), idHint(0);
		if (!pHost->HasCountryWarChannel())
		{
			idText = 9776;
			idHint = 9777;
		}
		else if (pHost->GetBattleCamp() == GP_BATTLE_CAMP_INVADER)
		{
			idText = 9778;
			idHint = 9779;
		}
		else
		{
			idText = 9780;
			idHint = 9781;
		}
		pObjChannel->SetText(GetStringFromTable(idText));
		pObjChannel->SetHint(GetStringFromTable(idHint));
	}

	pObjChannel = GetDlgItem("Btn_Set5");
	if (pObjChannel)
	{
		int idText(0), idHint(0);
		if (!pHost->HasCountryChannel())
		{
			idText = 9972;
			idHint = 9973;
		}
		else
		{
			idText = 9974;
			idHint = 9975;
		}
		pObjChannel->SetText(GetStringFromTable(idText));
		pObjChannel->SetHint(GetStringFromTable(idHint));
	}

	RebuildSpeakway();
}

void CDlgChat::RebuildSpeakway()
{
	if (m_szName != "Win_Chat") return;

	bool bHasCountryChannel = GetHostPlayer()->HasCountryChannel();
	bool bHasCountryWarChannel = GetHostPlayer()->HasCountryWarChannel();

	PAUICOMBOBOX pCombo = (PAUICOMBOBOX)GetDlgItem("Txt_Speakway");
	pCombo->ResetContent();
	for(int i = 439; i <= 445; i++ )
	{
		int j = i;
		if (i == 441 && bHasCountryChannel) j = 447;
		if (i == 445 && bHasCountryWarChannel) j = 446;
		pCombo->AddString(GetStringFromTable(j));
	}
	pCombo->SetCurSel(pCombo->GetCount()-1);
}

void CDlgChat::RefreshWhisperPlayerName(int roleid/*=-1*/)
{
	if (m_szName != "Win_WhisperChat" ||
		roleid != -1 && roleid != GetData())
		return;
	
	ACHAR szUser[40];
	ACString strText;
	PAUIOBJECT pName = GetDlgItem("Txt_ToWho");
	const ACHAR *pszName = GetGameRun()->GetPlayerName(GetData(), true);
	AUI_ConvertChatString(pszName, szUser);
	strText.Format(GetStringFromTable(818), szUser);
	pName->SetText(strText);
	
	//	更新最小化界面
	GetGameUIMan()->GetMiniBarMgr()->UpdateDialog(this,
		CECMiniBarMgr::BarInfo(pszName, A3DCOLORRGB(255, 74, 176)));
}

void CDlgChat::DoSendCountryMessage(int iPack, int iSlot, const ACHAR* pszText, bool bSendMsg)
{
	CDlgChat::CHAT_MSG cm;
	cm.nPack = iPack;
	cm.nSlot = iSlot;

	if (bSendMsg)
	{
		GetGameSession()->SendChatData(GP_CHAT_COUNTRY, pszText, iPack, iSlot);
		cm.dwTime = GetTickCount();
	}
	else
		cm.dwTime = 0;

	cm.cChannel = GP_CHAT_COUNTRY;
	cm.strMsg = ACString(_AL("$")) + pszText;
	m_vecHistory.push_back(cm);
	if( m_vecHistory.size() >= 20 )
		m_vecHistory.erase(m_vecHistory.begin());
		m_nCurHistory = m_vecHistory.size();
}

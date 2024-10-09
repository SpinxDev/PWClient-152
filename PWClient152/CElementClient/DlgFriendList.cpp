// File		: DlgFriendList.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#include "AUIEditBox.h"
#include "DlgFriendList.h"
#include "DlgInfo.h"
#include "DlgInputName.h"
#include "DlgSetting.h"
#include "DlgFriendOptionName.h"
#include "DlgFriendOptionGroup.h"
#include "DlgFriendOptionNormal.h"
#include "DlgBlackList.h"
#include "DlgChannelChat.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EL_Precinct.h"
#include "Network\\gnetdef.h"
#include <AFI.h>
#include <A2DSprite.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFriendList, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCANCAL)
AUI_ON_COMMAND("blacklist",	OnCommandBlackList)
AUI_ON_COMMAND("add",		OnCommandAdd)
AUI_ON_COMMAND("remove",	OnCommandRemove)
AUI_ON_COMMAND("action",	OnCommandAction)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFriendList, CDlgBase)

AUI_ON_EVENT("Tv_FriendList",	WM_LBUTTONDBLCLK,	OnEventButtonDBCLK)
AUI_ON_EVENT("Tv_FriendList",	WM_RBUTTONUP,		OnEventRButtonUp)

AUI_END_EVENT_MAP()

CDlgFriendList::CDlgFriendList()
{
	m_pTvFriendList = NULL;
}

CDlgFriendList::~CDlgFriendList()
{
	ReleaseImages();
}

bool CDlgFriendList::OnInitDialog()
{
	DDX_Control("Tv_FriendList", m_pTvFriendList);

	LoadImages();

	if (m_pTvFriendList && IsImageReady())
	{
		m_pTvFriendList->SetItemMask(-1);
		m_pTvFriendList->SetImageList(&m_pA2DSpriteImage);
	}
	return true;
}

void CDlgFriendList::OnCommandCANCAL(const char *szCommand)
{
	Show(false);
}

void CDlgFriendList::OnCommandBlackList(const char *szCommand)
{
	//GetGameUIMan()->m_pDlgSettingCurrent->OnCommandSetting("blacklist");
	GetGameUIMan()->m_pDlgBlackList->Show(!GetGameUIMan()->m_pDlgBlackList->IsShow());
}

void CDlgFriendList::OnCommandAdd(const char *szCommand)
{
	GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_ADD_FRIEND);
}

void CDlgFriendList::OnCommandRemove(const char *szCommand)
{
	P_AUITREEVIEW_ITEM pItem = m_pTvFriendList->GetSelectedItem();

	if( pItem && m_pTvFriendList->GetParentItem(pItem) != m_pTvFriendList->GetRootItem() )
	{
		GetGameUIMan()->MessageBox("Game_DelFriend", GetGameUIMan()->GetStringFromTable(538),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgFriendList::OnCommandAction(const char *szCommand)
{
	PAUIDIALOG pMenu = GetGameUIMan()->m_pDlgFriendOptionNormal;
	pMenu->AlignTo(this, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_CENTER, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_CENTER);
	pMenu->Show(!pMenu->IsShow());
}

void CDlgFriendList::OnEventButtonDBCLK(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	bool bOnIcon;
	POINT ptPos = pObj->GetPos();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;
	PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
	P_AUITREEVIEW_ITEM pItem = pTree->HitTest(x, y, &bOnIcon);

	if( pItem && !bOnIcon && pTree->GetParentItem(pItem) != pTree->GetRootItem() )
		GetGameUIMan()->m_pDlgFriendOptionName->OnCommandChat("Chat");
}

void CDlgFriendList::OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	bool bOnIcon;
	POINT ptPos = pObj->GetPos();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;
	PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
	PAUIDIALOG pMenu = NULL;
	P_AUITREEVIEW_ITEM pItem = pTree->HitTest(x, y, &bOnIcon);

	if( pItem && !bOnIcon )
	{
		if( pTree->GetParentItem(pItem) == pTree->GetRootItem() )
		{
			pMenu = GetGameUIMan()->m_pDlgFriendOptionGroup;
			if( pTree->GetItemData(pItem) == 0)
			{
				pMenu->GetDlgItem("RenameGroup")->Enable(false);
				pMenu->GetDlgItem("GroupColor")->Enable(false);
				pMenu->GetDlgItem("DeleteGroup")->Enable(false);
			}
			else
			{
				pMenu->GetDlgItem("RenameGroup")->Enable(true);
				pMenu->GetDlgItem("GroupColor")->Enable(true);
				pMenu->GetDlgItem("DeleteGroup")->Enable(true);
			}
		}
		else
		{
			pMenu = GetGameUIMan()->m_pDlgFriendOptionName;
			char status(0);
			if (!GetFriendStatus(pItem, status))
			{
				ASSERT(false);
				return;
			}
			if (!CECFriendMan::FRIEND::IsGameOnline(status))
			{
				pMenu->GetDlgItem("Invite")->Enable(false);
				pMenu->GetDlgItem("Update")->Enable(false);
				pMenu->GetDlgItem("JoinChannel")->Enable(false);
				pMenu->GetDlgItem("InviteChannel")->Enable(false);
			}
			else
			{
				pMenu->GetDlgItem("Invite")->Enable(true);
				pMenu->GetDlgItem("Update")->Enable(true);
				pMenu->GetDlgItem("JoinChannel")->Enable(true);				
				pMenu->GetDlgItem("InviteChannel")->Enable(GetGameUIMan()->m_pDlgChannelChat->GetRoomID() != 0);
			}

			//	游戏在线、GT在线时均可密语
			pMenu->GetDlgItem("Whisper")->Enable(CECFriendMan::FRIEND::IsGameOnline(status) || CECFriendMan::FRIEND::IsGTOnline(status));
			
			//	有好友附加信息时才允许修改备注
			if (PAUIOBJECT pObjChangeRemarks = pMenu->GetDlgItem("ChangeRemarks")){
				pObjChangeRemarks->Enable(GetHostPlayer()->GetFriendMan()->CanSetFriendRemarks(GetFriendID(pItem)));
			}
		}
	}
	else
		pMenu = GetGameUIMan()->m_pDlgFriendOptionNormal;

	x = GET_X_LPARAM(lParam) - p->X;
	y = GET_Y_LPARAM(lParam) - p->Y;
	
	// old : pMenu->SetPos(x, y);
	pMenu->SetPosEx(x, y);
	
	pMenu->Show(true);
}

void CDlgFriendList::FriendAction(int idPlayer, int idGroup, int idAction, int nCode)
{
	PAUITREEVIEW pTree = m_pTvFriendList;
	if( pTree->GetCount() <= 0 ) return;

	ACString strHint;
	P_AUITREEVIEW_ITEM pItem = NULL;
	CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();

	if( idAction == FRIEND_ACTION_INFO_LEVEL ||
		idAction == FRIEND_ACTION_INFO_AREA )
	{
		P_AUITREEVIEW_ITEM pItemPlayer = SearchItem(idPlayer, true);
		if( !pItemPlayer ) return;

		CECFriendMan::FRIEND *pFriend = pMan->GetFriendByID(idPlayer);
		if( idAction == FRIEND_ACTION_INFO_LEVEL )
		{
			pMan->SetFriendLevel(idPlayer, nCode);
			if( pFriend )
			{
				for(int i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
				{
					char szName[40];
					sprintf(szName, "Win_FriendChat%d", i);
					PAUIDIALOG pDlg = GetGameUIMan()->GetDialog(szName);
					if( (int)pDlg->GetData() == idPlayer )
					{
						ACString strText;
						ACHAR szLevel[10];
						a_sprintf(szLevel, _AL("%d"), nCode);
						strText.Format(GetStringFromTable(548), pFriend->GetName(), szLevel);
						pDlg->GetDlgItem("Txt_ToWho")->SetText(strText);
					}
				}
			}
		}
		else if( idAction == FRIEND_ACTION_INFO_AREA )
			pMan->SetFriendArea(idPlayer, nCode);

		strHint = GetFriendInfoStr(idPlayer);
		pTree->SetItemHint(pItemPlayer, strHint);
	}
	else if( idAction == FRIEND_ACTION_INFO_REFRESH )
	{
		ACHAR szInfo[20];

		a_sprintf(szInfo, _AL("L%d"), GetHostPlayer()->GetBasicProps().iLevel);
		GetGameSession()->SendPrivateChatData(_AL(""), szInfo, GNET::CHANNEL_USERINFO, idPlayer);

		if( GetGameUIMan()->m_pCurPrecinct )
		{
			a_sprintf(szInfo, _AL("A%d"), GetGameUIMan()->m_pCurPrecinct->GetID());
			GetGameSession()->SendPrivateChatData(_AL(""), szInfo, GNET::CHANNEL_USERINFO, idPlayer);
		}
	}
	else if( idAction == FRIEND_ACTION_FRIEND_ADD )
	{
		ACString strFriendName, strNameToShow;
		A3DCOLOR clrShow(0);
		char status(0);
		if (!GetFriendToShow(idPlayer, strFriendName, strNameToShow, clrShow, status))
		{
			ASSERT(false);
			return;
		}
		else
		{
			P_AUITREEVIEW_ITEM pItemGroup = SearchItem(idGroup, false);
			
			pItem = pTree->InsertItem(strNameToShow, pItemGroup, AUITREEVIEW_FIRST);
			pTree->SetItemTextColor(pItem, clrShow);
			pTree->SetItemData(pItem, idPlayer);
			SetFriendStatus(pItem, status);
			
			ACString strUnknow = GetStringFromTable(574);
			strHint.Format(GetStringFromTable(573),
				GetGameRun()->GetProfName(pMan->GetFriendByID(idPlayer)->iProfession),
				strUnknow, strUnknow, strUnknow, strUnknow, strUnknow);
			pTree->SetItemHint(pItem, strHint);
			
			ACString strMsg;
			strMsg.Format(GetStringFromTable(555), strFriendName);
			GetGameUIMan()->AddInformation(CDlgInfo::INFO_SYSTEM,
				"Game_OK", strMsg, 0xFFFFFFF, idPlayer, 0, 0);
		}
	}
	else if( idAction == FRIEND_ACTION_FRIEND_DELETE )
	{
		P_AUITREEVIEW_ITEM pItemPlayer = SearchItem(idPlayer, true);

		if( pItemPlayer ) pTree->DeleteItem(pItemPlayer);
	}
	else if( idAction == FRIEND_ACTION_FRIEND_UPDATE )
	{
		P_AUITREEVIEW_ITEM pItemPlayer = SearchItem(idPlayer, true);
		if( !pItemPlayer ) return;

		if( idGroup >= 0 )		// Move friend to another group.
		{
			P_AUITREEVIEW_ITEM pItemGroup = SearchItem(idGroup, false);
			pTree->MoveItem(pItemPlayer, pItemGroup);
		}
		else					// Online or offline notify.
		{
			ACString strFriendName, strNameToShow;
			A3DCOLOR clrShow(0);
			char status(0);
			if (!GetFriendToShow(idPlayer, strFriendName, strNameToShow, clrShow, status))
			{
				ASSERT(false);
				return;
			}

			P_AUITREEVIEW_ITEM pItemGroup = pTree->GetParentItem(pItemPlayer);
			
			if (CECFriendMan::FRIEND::IsGameOnline(status))
			{
				pItem = pTree->InsertItem(strNameToShow, pItemGroup, AUITREEVIEW_FIRST);
			}
			else
			{
				pItem = pTree->InsertItem(strNameToShow, pItemGroup);
			}
			pTree->SetItemTextColor(pItem, clrShow);
			pTree->SetItemData(pItem, pTree->GetItemData(pItemPlayer));
			SetFriendStatus(pItem, status);
			pTree->SetItemHint(pItem, pTree->GetItemHint(pItemPlayer));
			pTree->DeleteItem(pItemPlayer);
		}
	}
	else if( idAction == FRIEND_ACTION_GROUP_ADD )
	{
		CECFriendMan::GROUP *pGroup = pMan->GetGroupByID(idGroup);

		if( pGroup )
		{
			pItem = pTree->InsertItem(pGroup->strName);
			pTree->SetItemData(pItem, idGroup);
			pTree->SetItemTextColor(pItem, pGroup->color);
		}
	}
	else if( idAction == FRIEND_ACTION_GROUP_DELETE )
	{
		BuildFriendList();
	}
	else if( idAction == FRIEND_ACTION_GROUP_RENAME )
	{
		P_AUITREEVIEW_ITEM pItemGroup = SearchItem(idGroup, false);
		CECFriendMan::GROUP *pGroup = pMan->GetGroupByID(idGroup);

		if( pItemGroup && pGroup ) pTree->SetItemText(pItemGroup, pGroup->strName);
	}
	else if (idAction == FRIEND_ACTION_FRIEND_UPDATE_INPLACE){
		P_AUITREEVIEW_ITEM pItemPlayer = SearchItem(idPlayer, true);
		if (!pItemPlayer){
			return;
		}
		ACString strFriendName, strNameToShow;
		A3DCOLOR clrShow(0);
		char status(0);
		if (!GetFriendToShow(idPlayer, strFriendName, strNameToShow, clrShow, status)){
			ASSERT(false);
			return;
		}
		pTree->SetItemText(pItemPlayer, strNameToShow);
		pTree->Resize();	//	Resize 以正确处理替换文字后的滚动条
		pTree->SetItemTextColor(pItemPlayer, clrShow);
		SetFriendStatus(pItemPlayer, status);
		pTree->SetItemHint(pItemPlayer, GetFriendInfoStr(idPlayer));
	}
}

void CDlgFriendList::BuildFriendList(PAUIDIALOG pDlg)
{
	int i, j;
	CECFriendMan::GROUP *pGroup;
	P_AUITREEVIEW_ITEM pRoot, pItem;
	ACString strText, strProf, strLevel, strArea;

	ACString strFriendName, strNameToShow;
	A3DCOLOR clrShow(0);
	char status(0);

	if( !pDlg ) pDlg = this;
	CECWorld *pWorld = GetWorld();
	CELPrecinctSet *pSet = pWorld->GetPrecinctSet();
	CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();
	PAUITREEVIEW pTree = (PAUITREEVIEW)pDlg->GetDlgItem("Tv_FriendList");

	pTree->DeleteAllItems();
	for( i = 0; i < pMan->GetGroupNum(); i++ )
	{
		pGroup = pMan->GetGroupByIndex(i);
		if( pGroup->strName.GetLength() > 0 )
			pRoot = pTree->InsertItem(pGroup->strName);
		else
			pRoot = pTree->InsertItem(GetStringFromTable(537));
		pTree->SetItemData(pRoot, pGroup->iGroup);
		if( pGroup->iGroup > 0 )
			pTree->SetItemTextColor(pRoot, pGroup->color);

		for( j = 0; j < pGroup->aFriends.GetSize(); j++ )
		{
			strText = pGroup->aFriends[j]->GetName();
			if( 0 == stricmp(pDlg->GetName(), "Win_FriendList") )
			{
				GetFriendToShow(pGroup->aFriends[j]->id, strFriendName, strNameToShow, clrShow, status);

				if( pGroup->aFriends[j]->IsGameOnline(status) )
				{
					pItem = pTree->InsertItem(strNameToShow, pRoot, AUITREEVIEW_FIRST);
				}
				else
				{
					pItem = pTree->InsertItem(strNameToShow, pRoot);
				}
				pTree->SetItemTextColor(pItem, clrShow);
				SetFriendStatus(pItem, status);
			}
			else
				pItem = pTree->InsertItem(strText, pRoot);
			pTree->SetItemData(pItem, pGroup->aFriends[j]->id);

			if( 0 == stricmp(pDlg->GetName(), "Win_FriendList") )
			{
				strText = GetFriendInfoStr(pGroup->aFriends[j]->id);
				pTree->SetItemHint(pItem, strText);
			}
		}

		pTree->Expand(pRoot, AUITREEVIEW_EXPAND_EXPAND);
	}
}

P_AUITREEVIEW_ITEM CDlgFriendList::SearchItem(int id, bool bSearchPlayer)
{
	PAUITREEVIEW pTree = m_pTvFriendList;
	P_AUITREEVIEW_ITEM pRoot = pTree->GetRootItem(), pItemGroup, pItemFriend;

	pItemGroup = pTree->GetFirstChildItem(pRoot);
	while( pItemGroup )
	{
		if( bSearchPlayer )
		{
			pItemFriend = pTree->GetFirstChildItem(pItemGroup);
			while( pItemFriend )
			{
				if( id == (int)pTree->GetItemData(pItemFriend) )
					return pItemFriend;
				pItemFriend = pTree->GetNextSiblingItem(pItemFriend);
			}
		}
		else
		{
			if( id == (int)pTree->GetItemData(pItemGroup) )
				return pItemGroup;
		}
		pItemGroup = pTree->GetNextSiblingItem(pItemGroup);
	}

	return NULL;
}

void CDlgFriendList::OnChangeLayoutEnd(bool bAllDone)
{
	BuildFriendList(this);
}

bool CDlgFriendList::GetFriendNameByID(int idFriend, ACString &strFriendName)
{
	//	根据 id，查找玩家名称，查找成功时返回 true

	bool bRet(false);

	while (true)
	{
		CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();
		if (!pMan)	break;

		const CECFriendMan::FRIEND *pFriend = pMan->GetFriendByID(idFriend);
		if (!pFriend)	break;

		strFriendName = pFriend->GetName();
		if (strFriendName.IsEmpty())
			strFriendName = GetGameRun()->GetPlayerName(idFriend, true);

		if (strFriendName.IsEmpty())	break;

		bRet = true;
		break;
	}

	return bRet;
}


bool CDlgFriendList::GetFriendToShow(int idFriend, ACString &strFriendName, ACString &strNameToShow, A3DCOLOR &clrShow, char &status)
{
	//	根据好友 id，生成此玩家在好友列表中的显示信息
	//
	bool bRet(false);

	while (true)
	{
		//	查找玩家名称
		CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();
		if (!pMan)	break;

		const CECFriendMan::FRIEND *pFriend = pMan->GetFriendByID(idFriend);
		if (!pFriend)	break;

		if (!GetFriendNameByID(idFriend, strFriendName))	break;

		if (IsImageReady())
		{
			//	最终显示文字 = 玩家名称 + 额外图片（用于表示 GT 在线等状态）
			EditBoxItemBase item(enumEIImage);
			item.SetImageIndex(pFriend->IsGTOnline() ? IMAGE_GT_ONLINE : IMAGE_GT_OFFLINE);
			item.SetImageFrame(0);
			strNameToShow = (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize() + _AL(" ") + strFriendName;
		}
		else
		{
			//	图片有问题，只显示普通名称
			strNameToShow = strFriendName;
		}

		ACString strRemarks;
		if (pMan->GetFriendRemarks(idFriend, strRemarks) && !strRemarks.IsEmpty()){
			strNameToShow += ACString().Format(GetStringFromTable(11420), strRemarks);
		}

		//	查找玩家颜色
		clrShow = pFriend->IsGameOnline() ? A3DCOLORRGB(255, 203, 74) : A3DCOLORRGB(128, 128, 128);

		//	查找玩家状态
		status = pFriend->status;

		bRet = true;
		break;
	}

	return bRet;
}

int CDlgFriendList::GetFriendID(P_AUITREEVIEW_ITEM pItem){
	int result(0);
	if (pItem){
		result = m_pTvFriendList->GetItemData(pItem);
	}
	return result;
}

bool CDlgFriendList::GetFriendName(P_AUITREEVIEW_ITEM pItem, ACString &strFriendName)
{
	//	根据好友列表的显示情况，查找玩家名称，用于聊天等
	//
	bool bRet(false);

	while (true)
	{
		//	确认是好友列表界面中的好友名称项（而非根结点、group结点等）
		if (!pItem)	break;
		
		P_AUITREEVIEW_ITEM pItemPrent = m_pTvFriendList->GetParentItem(pItem);
		if (!pItemPrent || m_pTvFriendList->GetParentItem(pItemPrent) != m_pTvFriendList->GetRootItem())
			break;

		//	获取好友名称
		int idFriend = GetFriendID(pItem);
		if (!GetFriendNameByID(idFriend, strFriendName))
			break;

		bRet = true;
		break;
	}

	return bRet;
}

bool CDlgFriendList::GetSelectedFriendID(int &idPlayer){
	bool result(false);
	if (P_AUITREEVIEW_ITEM pItem = m_pTvFriendList->GetSelectedItem()){
		idPlayer = GetFriendID(pItem);
		result = true;
	}
	return result;
}

bool CDlgFriendList::GetSelectedFriendName(ACString &strFriendName){
	bool result(false);
	if (P_AUITREEVIEW_ITEM pItem = m_pTvFriendList->GetSelectedItem()){
		result = GetFriendName(pItem, strFriendName);
	}
	return result;
}

bool CDlgFriendList::GetSelectedFriendGroup(int &idGroup){
	bool result(false);
	if (P_AUITREEVIEW_ITEM pItem = m_pTvFriendList->GetSelectedItem()){		
		P_AUITREEVIEW_ITEM pItemGroup = m_pTvFriendList->GetParentItem(pItem);
		idGroup = m_pTvFriendList->GetItemData(pItemGroup);
		result = true;
	}
	return result;
}

bool CDlgFriendList::GetSelectedFriendRemarks(ACString &strRemarks){
	bool result(false);
	int idFriend(0);
	if (GetSelectedFriendID(idFriend)){
		result = GetHostPlayer()->GetFriendMan()->GetFriendRemarks(idFriend, strRemarks);
	}
	return result;
}

bool CDlgFriendList::GetFriendStatus(P_AUITREEVIEW_ITEM pItem, char &status)
{
	//	根据好友列表的显示情况，查找玩家状态
	//
	bool bRet(false);

	while (true)
	{
		//	确认是好友列表界面中的好友名称项（而非根结点、group结点等）
		if (!pItem)	break;
		
		P_AUITREEVIEW_ITEM pItemPrent = m_pTvFriendList->GetParentItem(pItem);
		if (!pItemPrent || m_pTvFriendList->GetParentItem(pItemPrent) != m_pTvFriendList->GetRootItem())
			break;

		//	获取界面中记录的好友状态标识
		status = (char)m_pTvFriendList->GetItemDataPtr(pItem);

		bRet = true;
		break;
	}

	return bRet;
}

bool CDlgFriendList::SetFriendStatus(P_AUITREEVIEW_ITEM pItem, char status)
{
	bool bRet(false);

	while (true)
	{
		//	确认是好友列表界面中的好友名称项（而非根结点、group结点等）
		if (!pItem)	break;
		
		P_AUITREEVIEW_ITEM pItemPrent = m_pTvFriendList->GetParentItem(pItem);
		if (!pItemPrent || m_pTvFriendList->GetParentItem(pItemPrent) != m_pTvFriendList->GetRootItem())
			break;

		m_pTvFriendList->SetItemDataPtr(pItem, (void *)(unsigned char)status);

		bRet = true;
		break;
	}

	return bRet;
}

bool CDlgFriendList::LoadImages()
{
	bool bRet(true);
	char a_szImageFile[IMAGE_NUM][40] = {"GT.tga", "GT_offline.tga"};
	for (int i = 0; i < IMAGE_NUM; ++ i)
	{
		AString strFile = AString("InGame\\") + a_szImageFile[i];
		AString strFullPath = AString("Surfaces\\") + strFile;

		A2DSprite *pSprite = NULL;
		if (af_IsFileExist(strFullPath))
		{
			pSprite = new A2DSprite;
			if (pSprite && pSprite->Init(m_pA3DDevice, strFile, AUI_COLORKEY))
			{
				//	本次图片加载成功
				m_pA2DSpriteImage.push_back(pSprite);
				continue;
			}
		}	
		
		//	图片加载失败
		A3DRELEASE(pSprite);
		ReleaseImages();
		a_LogOutput(1, "CDlgFriendList::LoadImages, failed to load file: %s", strFullPath);
		bRet = false;
		break;
	}

	return bRet;
}

void CDlgFriendList::ReleaseImages()
{
	for (int i = 0; i < (int)m_pA2DSpriteImage.size(); ++ i)
	{
		A3DRELEASE(m_pA2DSpriteImage[i]);
	}
	m_pA2DSpriteImage.clear();
}

ACString CDlgFriendList::GetFriendInfoStr(int idFriend)
{
	ACString strRet;
	ACString strProf, strLevel, strArea, strReincarnation, strLastLoginTime, strRemarks;
	CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();
	CECFriendMan::FRIEND *pFriend = pMan->GetFriendByID(idFriend);

	if( pFriend )
	{
		strProf = GetGameRun()->GetProfName(pFriend->iProfession);
		
		if( pFriend->nLevel < 0 )
			strLevel = GetStringFromTable(574);
		else
			strLevel.Format(_AL("%d"), pFriend->nLevel);
		
		if( pFriend->idArea < 0 )
			strArea = GetStringFromTable(574);
		else
		{
			int k;
			CELPrecinctSet *pSet = GetWorld()->GetPrecinctSet();
			
			for( k = 0; k < pSet->GetPrecinctNum(); k++ )
			{
				if( (int)pSet->GetPrecinct(k)->GetID() != pFriend->idArea )
					continue;
				
				strArea = pSet->GetPrecinct(k)->GetName();
				break;
			}
		}
		if (const CECFriendMan::FRIEND_EX* pFriendExt = pMan->GetValidFriendExByID(idFriend)) {
			int count = pFriendExt->reincarnation_count;
			if (count) strReincarnation = GetStringFromTable(10800 + count);
			else strReincarnation = GetStringFromTable(10799);
			if (pFriendExt->last_logintime > 0){
				tm t = GetGame()->GetServerLocalTime(pFriendExt->last_logintime);
				strLastLoginTime.Format(GetStringFromTable(8010),
					t.tm_year+1900,
					t.tm_mon+1,
					t.tm_mday,
					t.tm_hour,
					t.tm_min );
			}else{
				strLastLoginTime = _AL("-");
			}
		}
		pMan->GetFriendRemarks(idFriend, strRemarks);
		strRet.Format(GetStringFromTable(573), strProf, strLevel, strArea, strReincarnation, strLastLoginTime, strRemarks);
	}
	return strRet;
}
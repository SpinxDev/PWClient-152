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

#include "AFI.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A2DSprite.h"
#include "A3DViewport.h"
#include "A3DTerrainWater.h"

#include "gnetdef.h"
#include "AUI\\CSplit.h"
#include "Task\\EC_TaskInterface.h"
#include "EL_BackMusic.h"
#include "EL_Precinct.h"

#include "EC_FullGlowRender.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_ManPlayer.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_HostPlayer.h"
#include "EC_ElsePlayer.h"
#include "EC_Friend.h"
#include "EC_IvtrTypes.h"
#include "EC_Inventory.h"
#include "EC_DealInventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_Resource.h"
#include "EC_Team.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include "EC_Skill.h"

#include "DlgCharacter.h"
#include "DlgInputName.h"
#include "DlgInputNO.h"
#include "DlgShop.h"
#include "DlgMiniMap.h"
#include "DlgWorldMap.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Dialog index
enum
{
	DLG_CONSOLE = 0,		//	Console
	DLG_PLAYERCONTEXT,		//	Player context
	DLG_CHAT,				//	Chat window
	NUM_DIALOG,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

static char* l_aDlgNames[NUM_DIALOG] = 
{
	"Dlg_Console",
	"Win_QuickAction",
	"Win_Chat"
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////
static bool RemoveFilesAndDirectory(char *pszPath);

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECGameUIMan
//	
///////////////////////////////////////////////////////////////////////////

//	On command
bool CECGameUIMan::OnCommand(const char* szCommand, AUIDialog* pDlg)
{
	if (AUILuaManager::OnCommand(szCommand, pDlg))
		return true;

	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if( pHost->IsDead() ) return false;
	
	if( 0 == stricmp(pDlg->GetName(), "Win_Popmsg") )
		return false;
	if( 0 == stricmp(pDlg->GetName(), "Win_BBSDlg") )
		return m_BBSDlg.OnCommand(szCommand, pDlg);

	if( GetDialog("Win_Customize") && GetDialog("Win_Customize")->IsShow() )
		return m_Customize.OnCommand_Customize(szCommand, pDlg);
	
	if( 0 == stricmp(szCommand, "IDCANCEL") )
	{
		pDlg->Show(false);
		return true;
	}

	return false;
}

bool CECGameUIMan::OnMessageBox(int iRetVal, AUIDialog* pDlg)
{
	CECGameSession *pSession = g_pGame->GetGameSession();
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();

	if( 0 == stricmp(pDlg->GetName(), "Game_Quit") && IDYES == iRetVal )
	{
		if (pSession->IsConnected())
		{
			g_pGame->GetGameRun()->SaveConfigsToServer();
			pSession->c2s_CmdLogout(pDlg->GetData());
		}
		else if( pDlg->GetData() == _PLAYER_LOGOUT_HALF )
			g_pGame->GetGameRun()->SetLogoutFlag(2);
		else if( pDlg->GetData() == _PLAYER_LOGOUT_FULL )
			g_pGame->GetGameRun()->PostMessage(MSG_EXITGAME, -1, 0);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_InviteGroup") )
	{
		int idLeader = pDlg->GetData();
		int idTeamSeq = (int)pDlg->GetDataPtr();
		if( IDYES == iRetVal )
			pSession->c2s_CmdTeamAgreeInvite(idLeader, idTeamSeq);
		else
			pSession->c2s_CmdTeamRejectInvite(idLeader);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_InviteTrade") )
	{
		DWORD dwHandle = (DWORD)pDlg->GetDataPtr();
		pSession->trade_StartResponse(dwHandle, IDYES == iRetVal ? true : false);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_Disenchase") && IDOK == iRetVal )
	{
		PAUIDIALOG pMsgBox;
		PAUIDIALOG pDlg = GetDialog("Win_Disenchase");
		PAUIOBJECT pItem = pDlg->GetDlgItem("Item_a");
		CECIvtrItem *pIvtr = (CECIvtrItem *)pItem->GetDataPtr();

		pSession->c2s_CmdNPCSevClearEmbeddedChip(
			(WORD)pItem->GetData(), pIvtr->GetTemplateID());

		pDlg->Show(false);
		pHost->EndNPCService();
		m_pCurNPCEssence = NULL;
		GetDialog("Win_Inventory")->Show(false);
		pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();

		MessageBox("", GetStringFromTable(228), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_TeachSkill") && IDOK == iRetVal )
	{
		CECSkill *pSkill = (CECSkill *)pDlg->GetDataPtr();
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
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_TeamDisband") && IDOK == iRetVal )
	{
		pSession->c2s_CmdTeamLeaveParty();
		GetDialog("Win_ArrangeTeam")->Show(false);
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
	else if( 0 == stricmp(pDlg->GetName(), "Game_DelMark") && IDOK == iRetVal )
	{
		PAUILISTBOX pList = (PAUILISTBOX)GetDialog("Win_Mark")->GetDlgItem("List_Choose");

		m_pDlgMiniMap->m_vecMark.erase(m_pDlgMiniMap->m_vecMark.begin() + pList->GetCurSel());
		pList->DeleteString(pList->GetCurSel());
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_DelAllMarks") && IDOK == iRetVal )
	{
		PAUILISTBOX pList = (PAUILISTBOX)GetDialog("Win_Mark")->GetDlgItem("List_Choose");

		pList->ResetContent();
		m_pDlgMiniMap->m_vecMark.clear();
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_DelWorldMark") && IDOK == iRetVal )
	{
		PAUILISTBOX pList = (PAUILISTBOX)GetDialog("Win_Mark")->GetDlgItem("List_Choose");

		m_pDlgMiniMap->m_vecMark.erase(m_pDlgMiniMap->m_vecMark.begin() + m_pDlgWorldMap->m_nCurMark);
		pList->DeleteString(m_pDlgWorldMap->m_nCurMark);
		CDlgWorldMap::RefreshWorldMapFlags(false);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_DelAllWorldMarks") && IDOK == iRetVal )
	{
		PAUILISTBOX pList = (PAUILISTBOX)GetDialog("Win_Mark")->GetDlgItem("List_Choose");

		m_pDlgMiniMap->m_vecMark.clear();
		pList->ResetContent();
		CDlgWorldMap::RefreshWorldMapFlags(false);
	}
	else if( 0 == stricmp(pDlg->GetName(), "MsgBox_LinkBroken") && IDOK == iRetVal )
	{
		g_pGame->GetGameRun()->SetLogoutFlag(2);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_DelFriend") && IDOK == iRetVal )
	{
		PAUIDIALOG pDlgFriend = GetDialog("Win_FriendList");
		PAUITREEVIEW pTree = (PAUITREEVIEW)pDlgFriend->GetDlgItem("Tv_FriendList");
		P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
		if( !pItem ) return true;

		int idFriend = (int)pTree->GetItemData(pItem);
		pSession->friend_Delete(idFriend);
	}
	else if( 0 == stricmp(pDlg->GetName(), "Game_DelGroup") && IDOK == iRetVal )
	{
		PAUIDIALOG pDlgFriend = GetDialog("Win_FriendList");
		PAUITREEVIEW pTree = (PAUITREEVIEW)pDlgFriend->GetDlgItem("Tv_FriendList");
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
		CECSkill *pSkill = (CECSkill *)pDlg->GetDataPtr();

		pSession->c2s_CmdNPCSevForgetSkill(pSkill->GetSkillID());
		GetDialog("Win_NPC")->Show(false);
		m_pCurNPCEssence = NULL;
		pHost->EndNPCService();
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

	return AUILuaManager::OnMessageBox(iRetVal, pDlg);
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

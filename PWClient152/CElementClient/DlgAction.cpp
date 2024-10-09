// Filename	: DlgAction.cpp
// Creator	: Tom Zhou
// Date		: October 19, 2005

#include "AFI.h"
#include "AUIImagePicture.h"
#include "DlgAction.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_Shortcut.h"
#include "EC_ShortcutSet.h"
#include "TaskInterface.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EC_ManPlayer.h"
#include "EC_ElsePlayer.h"
#include "EC_TaskInterface.h"
#include "EC_Resource.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_FixedMsg.h"
#include "DlgBuddyState.h"
#include "EC_Player.h"

#define new A_DEBUG_NEW

/*
AUI_BEGIN_EVENT_MAP(CDlgAction, CDlgBase)

AUI_ON_EVENT("BscCmd_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("TeamCmd_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("TradeCmd_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("FaceCmd_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)

AUI_END_EVENT_MAP()

CDlgAction::CDlgAction()
{
}

CDlgAction::~CDlgAction()
{
}

void CDlgAction::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECShortcut") ) return;

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();

	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}

void CDlgAction::OnTick()
{
	int i, j, nMax;
	char szName[40];
	AString strFile;
	CECShortcut *pSCThis;
	AUIClockIcon *pClock;
	PAUIIMAGEPICTURE pImage;
	CECGameRun* pGameRun = GetGameRun();
	CECShortcutSet *a_pSC[] =
	{
		pGameRun->GetGenCmdShortcuts(),
		pGameRun->GetTeamCmdShortcuts(),
		pGameRun->GetTradeCmdShortcuts(),
		pGameRun->GetPoseCmdShortcuts()
	};
	char *a_pszPrefix[] = { "BscCmd_", "TeamCmd_","TradeCmd_", "FaceCmd_" };

	for( i = 0; i < sizeof(a_pSC) / sizeof(CECShortcutSet *); i++ )
	{
		for( j = 0; j < a_pSC[i]->GetShortcutNum(); j++ )
		{
			sprintf(szName, "%s%02d", a_pszPrefix[i], j + 1);
			pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
			if( !pImage ) break;

			pSCThis = a_pSC[i]->GetShortcut(j);
			pImage->SetDataPtr(pSCThis,"ptr_CECShortcut");

			af_GetFileTitle(pSCThis->GetIconFile(), strFile);
			strFile.MakeLower();
			pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);

			pClock = pImage->GetClockIcon();
			pClock->SetProgressRange(0, 1);
			pClock->SetProgressPos(1);
			if( pSCThis->GetCoolTime(&nMax) > 0 )
			{
				pClock->SetProgressRange(0, nMax);
				pClock->SetProgressPos(nMax - pSCThis->GetCoolTime());
				pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
			}
		}
	}
}
*/
//////////////////////////////////////////////////////////////////////////


AUI_BEGIN_EVENT_MAP(CDlgTaskAction, CDlgBase)

AUI_ON_EVENT("FaceCmd",		WM_LBUTTONDOWN,		OnEventLButtonDown)

AUI_END_EVENT_MAP()

void CDlgTaskAction::OnShowDialog()
{
	int param = GetData();
	if(param<0) return;

	AString strFile;
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)GetDlgItem("FaceCmd");

	if (param == CECTaskInterface::CMD_EMOTION_BINDBUDDY)
	{
		CECSCCommand* pSC = new CECSCCommand(CECSCCommand::CMD_BINDBUDDY);
		if (pSC)
		{
			af_GetFileTitle(pSC->GetIconFile(), strFile);
			strFile.MakeLower();
			pImage->SetHint(pSC->GetDesc());
			pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);

			delete pSC;
		}
	}
	else if (param == CECTaskInterface::CMD_EMOTION_SITDOWN)
	{
		CECShortcut* pSC = GetGameRun()->GetGenCmdShortcuts()->GetShortcut(CECSCCommand::CMD_SITDOWN);
		if (pSC)
		{
			af_GetFileTitle(pSC->GetIconFile(), strFile);
			strFile.MakeLower();
			pImage->SetHint(pSC->GetDesc());
			pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);
		}
	}	
	else if (param == TaskInterface::CMD_JUMP_TRICKACTION || param == TaskInterface::CMD_RUN_TRICKACTION)
	{
		bool bJump = param == TaskInterface::CMD_JUMP_TRICKACTION;
		af_GetFileTitle(res_IconFile(bJump ? RES_ICON_CMD_JUMPTRICK:RES_ICON_CMD_RUNTRICK), strFile);
		strFile.MakeLower();
		
		CECStringTab* pDescTab = g_pGame->GetItemDesc();
		const wchar_t* szDesc = pDescTab->GetWideString(bJump ? CMDDESC_JUMP_TRICK:CMDDESC_RUN_TRICK);			
		
		pImage->SetHint(szDesc ? szDesc : _AL(""));
		pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);
		
	}
	else if (param == ROLEEXP_TWO_KISS + 1)
	{
		af_GetFileTitle(res_IconFile(RES_ICON_CMD_TWOKISS), strFile);
		strFile.MakeLower();

		CECStringTab* pDescTab = g_pGame->GetItemDesc();
		const wchar_t* szDesc = pDescTab->GetWideString(CMDDESC_TWO_KISS);			
		
		pImage->SetHint(szDesc ? szDesc : _AL(""));
		pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);
			
	}
	else if(param <= NUM_ROLEEXP)
	{
		CECShortcut *pSCThis = GetGameRun()->GetPoseCmdShortcuts()->GetShortcut(param-1); // 序号需减一
		if (pSCThis)
		{
			af_GetFileTitle(pSCThis->GetIconFile(), strFile);
			strFile.MakeLower();
			pImage->SetDataPtr(pSCThis,"ptr_CECShortcut");
			pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);
			pImage->SetHint(pSCThis->GetDesc());
		}
	}
	pImage->SetFlash(true);
}
void CDlgTaskAction::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int param = GetData();
	if(param<0) return;
	
	if (param == CECTaskInterface::CMD_EMOTION_BINDBUDDY) // 
	{
		if(GetHostPlayer()->GetBuddyState()) return;

		int idTarget = GetHostPlayer()->GetSelectedTarget();
		CECElsePlayer *pPlayer = GetWorld()->GetPlayerMan()->GetElsePlayer(idTarget);
		
		int idFRole = GetHostPlayer()->GetFRoleID();
		int idFaction = GetHostPlayer()->GetFactionID();
			
		// 在CDlgQuickAction::OnTick()里检查玩家状态，判断是否关闭相依相偎按钮
		if( pPlayer && 
			pPlayer->GetGender() != GetHostPlayer()->GetGender() && 
			GetHostPlayer()->GetBuddyState() == 0 &&
			!GetHostPlayer()->IsInvisible() &&
			GetHostPlayer()->GetShapeType() != PLAYERMODEL_DUMMYTYPE2 &&
			pPlayer->GetShapeType() != PLAYERMODEL_DUMMYTYPE2 )
		{
			GetHostPlayer()->CmdBindBuddy(idTarget);
		}
		else
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10770),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
		}		
	}
	else if (param == ROLEEXP_TWO_KISS+1) //只有在相依相偎状态下才能亲亲密密
	{
		if(0==GetHostPlayer()->GetBuddyState()) 
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10771),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
			return;
		};
		GetGameUIMan()->m_pDlgBuddyState->OnCommand_Kiss(NULL);
	}
	else if (param == CECTaskInterface::CMD_EMOTION_SITDOWN)
	{
		CECShortcut* pSC = GetGameRun()->GetGenCmdShortcuts()->GetShortcut(CECSCCommand::CMD_SITDOWN);
		if(pSC)
			pSC->Execute();
	}
	else if (param == CECTaskInterface::CMD_JUMP_TRICKACTION)
	{
		if (!GetHostPlayer()->IsJumping())
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10772),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));			
			return;
		}
		GetGameRun()->PostMessage(MSG_HST_PLAYTRICK, MAN_PLAYER, 0);
	}
	else if (param == CECTaskInterface::CMD_RUN_TRICKACTION)
	{
		if (!GetHostPlayer()->IsPlayingAction(CECPlayer::ACT_RUN))
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10773),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));			
			return;
		}
		GetGameRun()->PostMessage(MSG_HST_PLAYTRICK, MAN_PLAYER, 0);
	}
	else if(param <= NUM_ROLEEXP)
	{
		CECShortcut *pSC = (CECShortcut *)pObj->GetDataPtr("ptr_CECShortcut");
		if(pSC)
			pSC->Execute();	
	}
}

void CDlgTaskAction::OnHideDialog()
{
	SetData64(0);
	SetData(0);
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)GetDlgItem("FaceCmd");
	if(pImage)
		pImage->SetDataPtr(NULL,"ptr_CECShortcut");
}
void CDlgTaskAction::OnTick()
{
	unsigned __int64 task_id = GetData64();
	int id = (int)task_id;
	
	if (GetHostPlayer()->GetTaskInterface()->CanFinishTask(id))
	{
		Show(false);		
	}
}

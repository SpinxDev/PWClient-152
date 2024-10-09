// Filename	: DlgSkillSubAction.cpp
// Creator	: zhangyitian
// Date		: 2014/7/7

#include "DlgSkillSubAction.h"
#include "DlgSkillSubList.h"

#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Shortcut.h"
#include "EC_ShortcutSet.h"
#include "EC_GameUIMan.h"

#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIClockIcon.h"
#include "AUISubDialog.h"
#include "AUIDef.h"
#include "AFI.h"

AUI_BEGIN_EVENT_MAP(CDlgSkillSubAction, CDlgBase)

AUI_ON_EVENT("BscCmd_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("TeamCmd_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("TradeCmd_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("FaceCmd_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("*",				WM_MOUSEWHEEL,		OnMouseWheel)	

AUI_END_EVENT_MAP()

CDlgSkillSubAction::CDlgSkillSubAction() {
	m_bSetVScroll = false;
}

void CDlgSkillSubAction::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECShortcut") ) return;
	
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	
	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}

void CDlgSkillSubAction::OnShowDialog() {
	int i, j;
	char szName[40];
	AString strFile;
	CECShortcut *pSCThis;
	PAUIIMAGEPICTURE pImage;
	PAUILABEL pLabel;
	CECGameRun* pGameRun = GetGameRun();
	CECShortcutSet *a_pSC[] =
	{
		pGameRun->GetGenCmdShortcuts(),
			pGameRun->GetTeamCmdShortcuts(),
			pGameRun->GetTradeCmdShortcuts(),
			pGameRun->GetPoseCmdShortcuts()
	};
	char *a_pszPrefix[] = { "BscCmd_", "TeamCmd_","TradeCmd_", "FaceCmd_" };
	char *a_pszTxtPrefix[] = { "Txt_BscCmd", "Txt_TeamCmd", "Txt_TradeCmd", "Txt_FaceCmd" };
	int objCount[] = { 9, 3, 3, 30 };
	
	for( i = 0; i < sizeof(a_pSC) / sizeof(CECShortcutSet *); i++ )
	{
		for( j = 0; j < objCount[i]; j++ )
		{
			sprintf(szName, "%s%02d", a_pszPrefix[i], j + 1);
			pImage = dynamic_cast<AUIImagePicture*>(GetDlgItem(szName));
			
			sprintf(szName, "%s%02d", a_pszTxtPrefix[i], j + 1);
			pLabel = dynamic_cast<AUILabel*>(GetDlgItem(szName));
			
			if( !pImage ) break;
			
			if (j < a_pSC[i]->GetShortcutNum()) {
				pSCThis = a_pSC[i]->GetShortcut(j);
				pImage->SetDataPtr(pSCThis,"ptr_CECShortcut");
				
				af_GetFileTitle(pSCThis->GetIconFile(), strFile);
				strFile.MakeLower();
				pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);
				
				pLabel->SetText(pSCThis->GetDesc());
			} else {
				pImage->Show(false);
				pLabel->Show(false);
			}
		}
	}
	
	// 在切换界面风格的时候，此时可能父对话框指针为空
	if (GetParentDlgControl()) {
		SIZE size;
		size.cx = 326;
		size.cy = 720;
		size.cx = (int)(size.cx * m_pAUIManager->GetWindowScale() + 0.5f);
		size.cy = (int)(size.cy * m_pAUIManager->GetWindowScale() + 0.5f);
		
		if (!m_bSetVScroll) {
			GetParentDlgControl()->SetSubDialogOrgSize(size.cx, size.cy);
			m_bSetVScroll = true;
		}
	}
}


void CDlgSkillSubAction::OnTick() {
	int i, j, nMax;
	char szName[40];
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

void CDlgSkillSubAction::OnChangeLayoutEnd(bool bAllDone) {
	if (IsShow()) {
		SIZE size;
		size.cx = 326;
		size.cy = 720;
		size.cx = (int)(size.cx * m_pAUIManager->GetWindowScale() + 0.5f);
		size.cy = (int)(size.cy * m_pAUIManager->GetWindowScale() + 0.5f);
		
		if (!m_bSetVScroll) {
			GetParentDlgControl()->SetSubDialogOrgSize(size.cx, size.cy);
			m_bSetVScroll = true;
		}
	}
}

void CDlgSkillSubAction::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	GetParentDlgControl()->OnDlgItemMessage(WM_MOUSEWHEEL, wParam, lParam);
}

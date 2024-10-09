// Filename	: DlgAutoMove.cpp
// Creator	: Chen Zhixin
// Date		: 2009/01/05

#include "DlgAutoMove.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Global.h"
#include "EC_MsgDef.h"
#include "EC_Manager.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IntelligentRoute.h"

#include "AUICheckBox.h"

#define AUTOMOVE_MAX_HEIGHT 78
#define INVALID_POS 0xFFFF

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgAutoMove, CDlgBase)

AUI_ON_COMMAND("Confirm",		OnCommand_Confirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_Cancel)
AUI_ON_COMMAND("Stop",			OnCommand_Stop)
AUI_ON_COMMAND("Apply",			OnCommand_Apply)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoMove, CDlgBase)

AUI_ON_EVENT("Check_Min",	WM_LBUTTONDOWN,		OnEventLButtonDownMin)
AUI_ON_EVENT("Chk_Hide",	WM_LBUTTONDOWN,		OnEventLButtonDownMax)

AUI_END_EVENT_MAP()

CDlgAutoMove::CDlgAutoMove()
{
	m_iHigh = 0;
	m_bAutoLand = true;

	Clear2DTargetPos();
}

CDlgAutoMove::~CDlgAutoMove()
{
}

bool CDlgAutoMove::OnInitDialog()
{
	m_pEdt_High = GetDlgItem("Edt_High");
	m_pLab_High = (PAUILABEL)GetDlgItem("Lab_High");

	OnChangeLayoutEnd(true);

	return true;
}
 
void CDlgAutoMove::OnCommand_Confirm(const char * szCommand)
{
	m_bAutoLand = m_pCheck_fly->IsChecked();

	ACString strHigh = m_pEdt_High->GetText();
	m_iHigh = strHigh.ToInt();
	a_Clamp(m_iHigh, 0, AUTOMOVE_MAX_HEIGHT);
	
	// sync the data with UI
	SetHight(m_iHigh);

	g_pGame->GetGameRun()->PostMessage(MSG_HST_AUTOMOVE, MAN_PLAYER, 0, 1, (DWORD)m_iHigh, (m_bAutoLand ? 1:0), 0);
}

void CDlgAutoMove::OnCommand_Cancel(const char * szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgAutoMove::OnCommand_Stop(const char * szCommand)
{
	g_pGame->GetGameRun()->PostMessage(MSG_HST_AUTOMOVE, MAN_PLAYER, 0, 2, 0, 0, 0);
}

void CDlgAutoMove::SetHight(int iHigh)
{
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d"), iHigh);
	m_pLab_High->SetText(szText);
	m_pEdt_High->SetText(szText);
	m_pSlider_high->SetLevel(iHigh);
}

void CDlgAutoMove::OnShowDialog()
{
	if (0 == strcmp(m_szName, "Win_AutoMove"))
	{
		((PAUICHECKBOX)(GetDlgItem("Check_Min")))->Check(false);
	}
	else if (0 == strcmp(m_szName, "Win_HideMove"))
	{
		((PAUICHECKBOX)(GetDlgItem("Chk_Hide")))->Check(true);
	}
}

void CDlgAutoMove::OnTick()
{
	if (0 == strcmp(m_szName, "Win_HideMove"))
	{
		((PAUICHECKBOX)(GetDlgItem("Chk_Hide")))->Check(true);
		return;
	}

	if (m_pSlider_high->IsFocus())
	{
		m_iHigh = m_pSlider_high->GetLevel();
		ACHAR szText[20];
		a_sprintf(szText, _AL("%d"), m_iHigh);
		m_pEdt_High->SetText(szText);
	}

	PAUIOBJECT btnOK = GetDlgItem("Btn_OK");
	if(btnOK)
	{
		if (!CheckFlyCondition())
		{
			btnOK->Enable(false);
			btnOK->SetHint(GetStringFromTable(7191));
		}
		else
		{
			
			btnOK->Enable(true);
			btnOK->SetHint(_AL(""));
		}
	}
}

void CDlgAutoMove::OnEventLButtonDownMin(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	PAUIOBJECT pChkObj = GetDlgItem("Check_Min");
	int x= GetPos().x + pChkObj->GetPos(true).x;
	int y= GetPos().y + pChkObj->GetPos(true).y;
	GetGameUIMan()->m_pDlgHideMove->SetPosEx(x, y);
	GetGameUIMan()->m_pDlgHideMove->Show(true);
	Show(false);
}

void CDlgAutoMove::OnEventLButtonDownMax(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	PAUIOBJECT pChkObj = GetGameUIMan()->m_pDlgAutoMove->GetDlgItem("Check_Min");
	int x= GetPos().x - pChkObj->GetPos(true).x;;
	int y= GetPos().y - pChkObj->GetPos(true).y;;
	GetGameUIMan()->m_pDlgAutoMove->SetPosEx(x, y);
	GetGameUIMan()->m_pDlgAutoMove->Show(true);
	Show(false);	
}

bool CDlgAutoMove::CheckFlyCondition()
{
	CECHostPlayer *pHostPlayer = GetHostPlayer();
	if (!pHostPlayer->CheckCanDoFly())
		return false;
	
	CECIvtrItem* pItem = pHostPlayer->GetEquipment()->GetItem(EQUIPIVTR_FLYSWORD);
	if (!pItem)
		return false;
	
	if (!pHostPlayer->IsFlying())
	{
		//	TODO: Maybe we should let server tell us whether we can fly or not
		bool bCanFly = true;
		
		if (pHostPlayer->GetMoveEnv() == CECHostPlayer::MOVEENV_AIR)
			bCanFly = false;
// 		else if (pHostPlayer->GetMoveEnv() == CECHostPlayer::MOVEENV_WATER && !pHostPlayer->CanTakeOffWater())
// 			bCanFly = false;

		if (!bCanFly)
			return false;
	}
	return true;
}

void CDlgAutoMove::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
	{
		m_pSlider_high = (PAUISLIDER)GetDlgItem("Slider_high");
		if(!m_pSlider_high) m_pSlider_high = (PAUISLIDER)GetDlgItem("Slider_high_Apply");

		m_pCheck_fly = (PAUICHECKBOX)GetDlgItem("check_fly");
		if(!m_pCheck_fly) m_pCheck_fly = (PAUICHECKBOX)GetDlgItem("check_fly_Apply");

		if(m_pSlider_high) m_pSlider_high->SetTotal(AUTOMOVE_MAX_HEIGHT);
	}
}

void CDlgAutoMove::OnCommand_Apply(const char * szCommand)
{
	m_bAutoLand = m_pCheck_fly->IsChecked();
	m_iHigh = m_pSlider_high->GetLevel();
	
	// sync the data with UI
	SetHight(m_iHigh);
	
	g_pGame->GetGameRun()->PostMessage(MSG_HST_AUTOMOVE, MAN_PLAYER, 0, 1, (DWORD)m_iHigh, (DWORD)m_bAutoLand, 0);
}

void CDlgAutoMove::Set2DTargetPos(int x, int y)
{
	m_ptTargetPos.x = x;
	m_ptTargetPos.y = y;
}

void CDlgAutoMove::RefreshHigh()
{
	bool isSimpleMode = (!strcmp(m_pSlider_high->GetName(), "Slider_high_Apply"));

	if(!IsShow())
	{
		// In simple mode, user need not to click the confirm button.
		// Drag the slider will apply the height changing automatically.
		if( isSimpleMode )
		{
			// Use player's current height to init the UI in simple mode
			m_iHigh = ((int)GetHostPlayer()->GetPos().y) / 10;
		}

		SetHight(m_iHigh);
	}

	// HACK: adjust the height when the target pos is too far away.
	// This hack will trigger the fly mode automatically.
	if( isSimpleMode && !GetHostPlayer()->IsFlying()
		&& m_ptTargetPos.x != INVALID_POS && m_ptTargetPos.y != INVALID_POS )
	{
		A3DVECTOR3 hostPos = GetHostPlayer()->GetPos();
		A3DVECTOR3 target((float)m_ptTargetPos.x, hostPos.y, (float)m_ptTargetPos.y);
		float dist = (hostPos - target).Magnitude();
		if(dist > 100.f)
		{
			// adjust by 1 to trigger the fly
			m_iHigh += 1;
			SetHight(m_iHigh);
			OnCommand_Apply(NULL);
		}
	}
}

void CDlgAutoMove::Clear2DTargetPos()
{
	m_ptTargetPos.Set(INVALID_POS, INVALID_POS);
}

A3DPOINT2 CDlgAutoMove::GetTargetPosAs2D() const
{
	if (!CECIntelligentRoute::Instance().IsIdle()){
		const A3DVECTOR3 &dest = CECIntelligentRoute::Instance().GetDest();
		return A3DPOINT2((int)dest.x, (int)dest.z);
	}
	return m_ptTargetPos;
}

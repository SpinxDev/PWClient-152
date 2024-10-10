// Filename	: DlgCamera.cpp
// Creator	: Xiao Zhou
// Date		: 2005/11/18

#include "DlgCamera.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_GameRun.h"
#include "A3DCamera.h"
#include "A3DFuncs.h"
#include "EC_Viewport.h"
#include "EC_Configs.h"
#include <A3DDevice.h>

//#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgCamera, CDlgBase)

AUI_ON_COMMAND("Combo_Scale",	OnCommand_Combo_Scale)
AUI_ON_COMMAND("printscr",				OnCommand_PrintScr)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

CDlgCamera::CDlgCamera()
{
	m_pBtn_CameraMoveLeft = NULL;
	m_pBtn_CameraMoveRight = NULL;
	m_pBtn_CameraMoveUp = NULL;
	m_pBtn_CameraMoveDown = NULL;
	m_pBtn_CameraMoveFront = NULL;
	m_pBtn_CameraMoveBack = NULL;
	m_pBtn_CameraFarFocus = NULL;
	m_pBtn_CameraNearFocus = NULL;
	m_pChk_HideUI = NULL;
	m_pCombo_Scale = NULL;
	m_bPrinting = false;
}

CDlgCamera::~CDlgCamera()
{
}

void CDlgCamera::OnCommand_Combo_Scale(const char *szCommand)
{
	if (m_pCombo_Scale->GetCount() <= 0 || 
		m_pCombo_Scale->GetCurSel() == 0)
	{
		if (!m_pChk_HideUI->IsEnabled())
			m_pChk_HideUI->Enable(true);
		return;
	}

	//	多倍截图时禁止显示UI
	if (!m_pChk_HideUI->IsChecked())
		m_pChk_HideUI->Check(true);

	if (m_pChk_HideUI->IsEnabled())
		m_pChk_HideUI->Enable(false);
}

void CDlgCamera::OnCommand_PrintScr(const char * szCommand)
{
	::PostMessage(GetGame()->GetGameInit().hWnd, WM_KEYUP, VK_SNAPSHOT, 1);
}

void CDlgCamera::OnCommand_CANCEL(const char * szCommand)
{
}

bool CDlgCamera::OnInitDialog()
{
	DDX_Control("Btn_CameraMoveLeft", m_pBtn_CameraMoveLeft);
	DDX_Control("Btn_CameraMoveRight", m_pBtn_CameraMoveRight);
	DDX_Control("Btn_CameraMoveUp", m_pBtn_CameraMoveUp);
	DDX_Control("Btn_CameraMoveDown", m_pBtn_CameraMoveDown);
	DDX_Control("Btn_CameraMoveFront", m_pBtn_CameraMoveFront);
	DDX_Control("Btn_CameraMoveBack", m_pBtn_CameraMoveBack);
	DDX_Control("Btn_CameraFarFocus", m_pBtn_CameraFarFocus);
	DDX_Control("Btn_CameraNearFocus", m_pBtn_CameraNearFocus);
	DDX_Control("Chk_HideUI", m_pChk_HideUI);
	DDX_Control("Combo_Scale", m_pCombo_Scale);
	m_pChk_HideUI->Check(true);
	SetCanOutOfWindow(true);

	return true;
}

void CDlgCamera::OnTick()
{
	if( IsShow() )
	{
		if( !GetGameRun()->GetHostPlayer()->GetCameraCtrl()->SceneryMode_Get() )
		{
			Show(false);
			return;
		}
		bool bState[7];

		bState[0] = m_pBtn_CameraMoveLeft->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK;
		bState[1] = m_pBtn_CameraMoveRight->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK;
		bState[2] = m_pBtn_CameraMoveFront->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK
					|| m_pBtn_CameraNearFocus->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK;
		bState[3] = m_pBtn_CameraMoveBack->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK
					|| m_pBtn_CameraFarFocus->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK;
		bState[4] = m_pBtn_CameraMoveUp->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK;
		bState[5] = m_pBtn_CameraMoveDown->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK;
		bState[6] = m_pBtn_CameraFarFocus->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK
					|| m_pBtn_CameraNearFocus->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK;
		
		GetGameRun()->GetHostPlayer()->GetCameraCtrl()->SetVirtualKeyState(bState);
	}
}

void CDlgCamera::OnShowDialog()
{
	SetPosEx(0 ,0, alignCenter, alignCenter);
	UpdateCaptureScale();
	OnCommand_Combo_Scale(NULL);
}

int CDlgCamera::PreparePrint()
{
	if( m_pChk_HideUI->IsChecked() )
		GetGameUIMan()->m_bShowAllPanels = false;
	m_bPrinting = true;
	m_nPrintTimes = 0;

	int N = 1 + m_pCombo_Scale->GetCurSel();
	if (N <= 0 || N > m_pCombo_Scale->GetCount())
		N = 1;
	return N;
}

void CDlgCamera::UpdateCaptureScale()
{
	//	更新当前设备状态下可供选择的截屏缩放模式

	m_pCombo_Scale->ResetContent();

	int nMin = 1, nMax = 8;

	A3DDEVFMT devFormat = g_pGame->GetA3DDevice()->GetDevFormat();
	if (devFormat.fmtTarget != A3DFMT_A8R8G8B8 &&
		devFormat.fmtTarget != A3DFMT_X8R8G8B8)
		nMax = 1;

	if (GetGame()->GetConfigs()->GetSystemSettings().bWideScreen)
		nMax = 1;

	ACString strFormat = GetStringFromTable(9382);
	ACString strText;
	for (int n = nMin; n <= nMax; ++ n)
	{
		strText.Format(strFormat, n);
		m_pCombo_Scale->AddString(strText);
	}

	if (m_pCombo_Scale->GetCount() > 0)
		m_pCombo_Scale->SetCurSel(0);
}
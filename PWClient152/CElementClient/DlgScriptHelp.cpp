#include <A3DDevice.h>
#include <A3DEngine.h>
#include <A3DViewport.h>
#include "DlgScriptHelp.h"
#include "AUICheckBox.h"
#include "AUITextArea.h"
#include "AUIStillImageButton.h"
#include "ECScriptOption.h"
#include "DlgSettingScriptHelp.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "ECScriptContext.h"
#include "ECScriptController.h"
#include "DlgScriptHelpHistory.h"
#include "ECScriptCheckState.h"

AUI_BEGIN_COMMAND_MAP(CDlgScriptHelp, CDlgBase)

AUI_ON_COMMAND("IDCANCEL", OnCommandIDCANCEL)
AUI_ON_COMMAND("next", OnCommandNext)
AUI_ON_COMMAND("close", OnCommandClose)
AUI_ON_COMMAND("disable_help", OnCommandDisableHelp)
AUI_ON_COMMAND("setting", OnCommandSetting)
AUI_ON_COMMAND("history", OnCommandHistory)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgScriptHelp, CDlgBase)

AUI_ON_EVENT(NULL, WM_MOUSEMOVE, OnMouseMove)
AUI_ON_EVENT("Lbl_Title", WM_MOUSEMOVE, OnMouseMove)
AUI_ON_EVENT("Img_Title", WM_MOUSEMOVE, OnMouseMove)

AUI_END_EVENT_MAP()

CDlgScriptHelp::CDlgScriptHelp()
{
	m_pContext = NULL;
	m_pTAreaTip = NULL;
	m_pBtnNext = NULL;
	m_pChkDisableHelp = NULL;
	m_pBtnClose = NULL;
	m_pEdtImage = NULL;
}

CDlgScriptHelpHistory * CDlgScriptHelp::GetHistoryDlg()
{
	return GetGameUIMan()->m_pDlgScriptHelpHistory;
}

CDlgSettingScriptHelp *	CDlgScriptHelp::GetSettingDlg()
{
	return GetGameUIMan()->m_pDlgSettingScriptHelp;
}

CDlgScriptHelp::~CDlgScriptHelp()
{
}

void CDlgScriptHelp::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("TArea_Tip", m_pTAreaTip);
	DDX_Control("Btn_Next", m_pBtnNext);
	DDX_Control("Btn_Close", m_pBtnClose);
	DDX_Control("Chk_DisableHelp", m_pChkDisableHelp);
	DDX_Control("Img_help", m_pEdtImage);

	m_pEdtImage->GetProperty("Image File", &m_defaultImgProperty);
}

void CDlgScriptHelp::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	AjustPos();
}

void CDlgScriptHelp::Resize(A3DRECT rcOld, A3DRECT rcNew) {
	if (IsShow()) {
		AjustPos();
	}
}

bool CDlgScriptHelp::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	m_pBtnClose->SetHint(GetGameUIMan()->GetStringFromTable(3000));
	return true;
}


void CDlgScriptHelp::SetScriptContext(CECScriptContext * pContext)
{
	m_pContext = pContext;
	GetSettingDlg()->SetScriptContext(m_pContext);
	GetHistoryDlg()->SetScriptContext(m_pContext);
}

void CDlgScriptHelp::OnCommandIDCANCEL(const char * szCommand)
{
}

void CDlgScriptHelp::OnCommandClose(const char * szCommand)
{
	Show(false);
	m_pContext->GetController()->FinishCurrentScript(NULL);
}

void CDlgScriptHelp::OnCommandNext(const char * szCommand)
{
	m_pContext->GetCheckState()->SetDisplayNext(true);
	SetHasNextTip(false);
}

void CDlgScriptHelp::OnCommandDisableHelp(const char * szCommand)
{
	bool bDisable = GetIsDisableHelp();
	m_pContext->GetOption()->SetDisableHelp(bDisable);
	m_pContext->GetOption()->SaveToServer();
	
	if (bDisable)
	{
		CECGameUIMan *pUIMan = GetGameUIMan();
		pUIMan->MessageBox("", pUIMan->GetStringFromTable(3000), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
		m_pContext->GetController()->FinishCurrentScript(NULL);
	}
}

void CDlgScriptHelp::OnCommandSetting(const char * szCommand)
{
	GetSettingDlg()->Show(true);
}

void CDlgScriptHelp::SetTipText(const ACHAR * szText)
{
	m_pTAreaTip->SetText(szText);
}

void CDlgScriptHelp::SetTipImage(const AString& szFile) {
	ScopedDialogSpriteModify _dummy(this);
	if (szFile == "") {
		m_pEdtImage->SetProperty("Frame Image", &m_defaultImgProperty);
	} else {
		AUIOBJECT_SETPROPERTY objProperty;
		memcpy(&objProperty, szFile, szFile.GetLength() + 1);
		m_pEdtImage->SetProperty("Frame Image", &objProperty);
	}
}

void CDlgScriptHelp::SetHasNextTip(bool bNext)
{
	m_pBtnNext->Enable(bNext);
}

bool CDlgScriptHelp::GetIsDisableHelp()
{
	return m_pChkDisableHelp->IsChecked();
}

void CDlgScriptHelp::SetIsDisableHelp(bool bDisable)
{
	m_pChkDisableHelp->Check(bDisable);
}

void CDlgScriptHelp::OnCommandHistory(const char * szCommand)
{
	if (GetHistoryDlg()->IsShow())
	{
		GetHistoryDlg()->Show(false);
	}
	else
	{
		GetHistoryDlg()->Show(true);
		AdjustHelpHistoryWindowPosition();
	}
	
}

void CDlgScriptHelp::OnMouseMove(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (::GetKeyState(VK_LBUTTON) & 0x8000)
	{
		if (GetHistoryDlg()->IsShow())
			AdjustHelpHistoryWindowPosition();	
	}
}

void CDlgScriptHelp::AdjustHelpHistoryWindowPosition()
{
	SIZE sizeHistoryWnd = GetHistoryDlg()->GetSize();
	POINT ptHelpWnd = GetPos();
	int x = ptHelpWnd.x - sizeHistoryWnd.cx;
	if (x < 0)
	{
		// old : SetPos(sizeHistoryWnd.cx, ptHelpWnd.y);
		SetPosEx(sizeHistoryWnd.cx, ptHelpWnd.y);

		x = 0;
	}
	int y = ptHelpWnd.y;
	
	// 界面修改，导致位置计算
	y += GetSize().cy;
	y -= sizeHistoryWnd.cy;

	// old : GetHistoryDlg()->SetPos(x, y);
	GetHistoryDlg()->SetPosEx(x, y);
}

void CDlgScriptHelp::OnChangeLayoutEnd(bool bAllDone)
{
	AjustPos();
}

void CDlgScriptHelp::AjustPos() {
	SIZE sizeWnd = this->GetSize();
	A3DVIEWPORTPARAM* pParam = g_pGame->GetA3DDevice()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int x = 150;
	int y = (pParam->Height - sizeWnd.cy) / 2;
	
	// old : SetPos(x, y);
	SetPosEx(x, y);
	
	SetIsDisableHelp(m_pContext->GetOption()->IsHelpDisabled());
	if (GetHistoryDlg()->IsShow())
		AdjustHelpHistoryWindowPosition();	
}
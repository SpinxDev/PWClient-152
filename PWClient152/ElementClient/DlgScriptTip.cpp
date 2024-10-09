// Filename	: DlgScriptTip.cpp
// Creator	: zhangyitian
// Date		: 2014/07/24

#include "DlgScriptTip.h"

#include "EC_Game.h"
#include "EC_Viewport.h"
#include "ECScriptTipWorkMan.h"
#include "ECScriptTipWork.h"

#include "A3DViewport.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIManager.h"

const int DEFAUT_SHOW_WIDTH = 40;
const int DISTANCE_TO_TOP = 250;

CDlgScriptTip::CDlgScriptTip() {
	m_pLblTitle = NULL;
	m_pLblContent = NULL;
	m_pImgPic = NULL;
	m_fCurShowPercent = 0.0f;
	m_pTipWorkMan = new CECScriptTipWorkMan();
}

CDlgScriptTip::~CDlgScriptTip() {
	delete m_pTipWorkMan;
}

CECScriptTipWorkMan* CDlgScriptTip::GetTipWorkMan() {
	return m_pTipWorkMan;
}

void CDlgScriptTip::DoDataExchange(bool bSave) {
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Txt_Title", m_pLblTitle);
	DDX_Control("Txt_main", m_pLblContent);
	DDX_Control("Img_Gfx", m_pImgPic);
}

void CDlgScriptTip::OnTick() {
	m_pTipWorkMan->Tick();
	if (m_pTipWorkMan->IsFree()) {
		Show(false);
	}
}

void CDlgScriptTip::OnShowDialog() {
	SetShowPercent(m_fCurShowPercent);
}

void CDlgScriptTip::SetTitle(const ACString& strTitle) {
	m_pLblTitle->SetText(strTitle);
}

void CDlgScriptTip::SetContent(const ACString& strContent) {
	m_pLblContent->SetText(strContent);
}

void CDlgScriptTip::SetShowPercent(float fShowPercent) {
	if (fShowPercent < 0.0f) {
		fShowPercent = 0.0f;
	}
	if (fShowPercent > 1.0f) {
		fShowPercent = 1.0f;
	}
	m_fCurShowPercent = fShowPercent;

	int viewportWidth = GetGame()->GetViewport()->GetA3DViewport()->GetParam()->Width;
	int viewportY = GetGame()->GetViewport()->GetA3DViewport()->GetParam()->Y;

	int inViewportWidth = DEFAUT_SHOW_WIDTH + (GetSize().cx - DEFAUT_SHOW_WIDTH) * fShowPercent;
	int x = viewportWidth - (int)(inViewportWidth * m_pAUIManager->GetWindowScale() + 0.5);
	int y = viewportY + DISTANCE_TO_TOP;

	SetPosEx(x, y);
}

void CDlgScriptTip::Resize(A3DRECT rcOld, A3DRECT rcNew) {
	SetShowPercent(m_fCurShowPercent);
}

void CDlgScriptTip::SetTitleAndContent(const ACString& strTitle, const ACString& strContent) {
	CECScriptTipWorkSetTitleAndContent* pWork = new CECScriptTipWorkSetTitleAndContent(this, strTitle, strContent);
	m_pTipWorkMan->AddWork(pWork);
}

void CDlgScriptTip::ShowTip(int iGfxTime, int iInTime, int iStayTime, int iOutTime) {
	Show(true);
	CECScriptTipWorkGfx* pWorkGfx = new CECScriptTipWorkGfx(this, iGfxTime);
	CECScriptTipWorkMove* pWorkMoveIn	= new CECScriptTipWorkMove(this, 0.0f, 1.0f, iInTime);
	CECScriptTipWorkMove* pWorkMoveStay	= new CECScriptTipWorkMove(this, 1.0f, 1.0f, iStayTime);
	CECScriptTipWorkMove* pWorkMoveOut	= new CECScriptTipWorkMove(this, 1.0f, 0.0f, iOutTime, true);
	m_pTipWorkMan->AddWork(pWorkGfx);
	m_pTipWorkMan->AddWork(pWorkMoveIn);
	m_pTipWorkMan->AddWork(pWorkMoveStay);
	m_pTipWorkMan->AddWork(pWorkMoveOut);
}

void CDlgScriptTip::PlayGfx() {
	m_pImgPic->SetGfx("界面\\tips界面01.gfx");
}

void CDlgScriptTip::StopGfx() {
	m_pImgPic->SetGfx(NULL);
}

bool CDlgScriptTip::IsFree() {
	return m_pTipWorkMan->IsFree();
}
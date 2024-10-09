// Filename	: DlgTitleBubble.cpp
// Creator	: Han Guanghui
// Date		: 2013/5/16

#include "DlgTitleBubble.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "AUIImagePicture.h"
#include "elementdataman.h"
#include "EC_TimeSafeChecker.h"

#include "A3DGFXExMan.h"

static const int SHOW_TIME = 2000;
static const int FADE_TIME = 2000;
static const int HIDE_TIME = 1000;
static const char* BUBBLE_GFX = "界面\\称号效果.gfx";

CDlgTitleBubble::CDlgTitleBubble():
m_pLabText(NULL),
m_iState(STATE_CLOSE),
m_pImgGfx(NULL)
{

}

CDlgTitleBubble::~CDlgTitleBubble()
{

}

bool CDlgTitleBubble::OnInitDialog()
{
	DDX_Control("Lbl_TitleName", m_pLabText);
	DDX_Control("Img_GetTitle", m_pImgGfx);
	m_pImgGfx->SetGfx("");
	return true;
}

void CDlgTitleBubble::OnShowDialog()
{
	ChangeState(STATE_SHOW, GetTickCount());
	SetCanEscClose(false);
	SetCanMove(false);
	SetCapture(false);
	SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignMin);
}

void CDlgTitleBubble::AddTitle(unsigned short id, int expiretime)
{
	m_TitlesToBubble.push_back(id);
	CECGameUIMan* pMan = GetGameUIMan();
	const TITLE_CONFIG* pConfig = CECPlayer::GetTitleConfig(id);
	if (pMan && pConfig) {
		ACString strText;
		strText.Format(GetStringFromTable(10609), pConfig->name);
		pMan->AddChatMessage(strText, GP_CHAT_MISC);
	}
	if (!IsShow()) Show(true);
}

void CDlgTitleBubble::OnTick()
{
	DWORD dwNow = GetTickCount();
	DWORD dwDelta = CECTimeSafeChecker::ElapsedTime(dwNow, m_dwCount);
	if (m_iState == STATE_SHOW){
		if (dwDelta > SHOW_TIME) ChangeState(STATE_FADE, dwNow);
			
	} else if (m_iState == STATE_FADE) {
		if (dwDelta < FADE_TIME){
			int alpha = int(255 - 255 * (float)dwDelta / FADE_TIME);
			m_pLabText->SetAlpha(alpha);
		} else if (!m_TitlesToBubble.empty()){
			ChangeState(STATE_HIDE, dwNow);
		} else {
			ChangeState(STATE_CLOSE, 0);
		}
	} else if (m_iState == STATE_HIDE) {
		if (dwDelta > HIDE_TIME) {
			ChangeState(STATE_SHOW, dwNow);
		}
	}
}
void CDlgTitleBubble::Update()
{
	const TITLE_CONFIG* pConfig = CECPlayer::GetTitleConfig(m_BubbleTitleID);
	if (pConfig) m_pLabText->SetText(pConfig->name);
}
void CDlgTitleBubble::ChangeState(int state, DWORD dwCount)
{
	m_iState = state;
	m_dwCount = dwCount;
	if (m_iState == STATE_SHOW) {
		if (!m_TitlesToBubble.empty()) {
			m_BubbleTitleID = m_TitlesToBubble.front();
			m_TitlesToBubble.pop_front();
			m_pLabText->SetAlpha(255);
			m_pImgGfx->SetGfx(BUBBLE_GFX);
			m_pImgGfx->Resize();
			Update();
		}
	} else if (m_iState == STATE_HIDE) {
		
	} else if (m_iState == STATE_CLOSE) {
		Show(false);
	}
}
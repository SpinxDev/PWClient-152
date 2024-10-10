// Filename	: ECScriptTipWork.h
// Creator	: zhangyitian
// Date		: 2014/07/24

#include "ECScriptTipWork.h"
#include "EC_TimeSafeChecker.h"
#include "DlgScriptTip.h"

CECScriptTipWork::CECScriptTipWork(CDlgScriptTip* pDlgScriptTip) {
	m_bFirstTick = true;
	m_bFinished = false;
	m_pDlgScriptTip = pDlgScriptTip;
}

CECScriptTipWork::~CECScriptTipWork() {

}

void CECScriptTipWork::Tick() {
	if (m_bFirstTick) {
		OnFirstTick();
		m_bFirstTick = false;
	} else {
		OnTick();
	}
}

bool CECScriptTipWork::IsFinished() {
	return m_bFinished;
}

void CECScriptTipWork::SetFinished(bool bFinished) {
	m_bFinished = bFinished;
}

/////////////////////////////////////////////////////////////////////////

CECScriptTipWorkSetTitleAndContent::
CECScriptTipWorkSetTitleAndContent(CDlgScriptTip* pDlgScriptTip, const ACString& strTitle, const ACString& strContent)
: CECScriptTipWork(pDlgScriptTip), m_strTitle(strTitle), m_strContent(strContent) {

}

void CECScriptTipWorkSetTitleAndContent::OnFirstTick() {
	m_pDlgScriptTip->SetTitle(m_strTitle);
	m_pDlgScriptTip->SetContent(m_strContent);
	SetFinished(true);
}

void CECScriptTipWorkSetTitleAndContent::OnTick() {

}

///////////////////////////////////////////////////////////////////////////

CECScriptTipWorkMove::CECScriptTipWorkMove(CDlgScriptTip* pDlgScriptTip, float fStart, float fEnd, unsigned int uiTotalTime, bool bStopGfx) 
: CECScriptTipWork(pDlgScriptTip) {
	m_fStart = fStart;
	m_fEnd = fEnd;
	m_uiTotalTime = uiTotalTime;
	m_bStopGfx = bStopGfx;
}

void CECScriptTipWorkMove::OnFirstTick() {
	m_dwStartTime = GetTickCount();
	m_pDlgScriptTip->SetShowPercent(m_fStart);
}

void CECScriptTipWorkMove::OnTick() {
	DWORD dwTimePassed = CECTimeSafeChecker::ElapsedTimeFor(m_dwStartTime);
	if (dwTimePassed >= m_uiTotalTime) {
		dwTimePassed = m_uiTotalTime;
		SetFinished(true);
		if (m_bStopGfx) {
			m_pDlgScriptTip->StopGfx();
		}
	}
	float fPassedPercent = (float)dwTimePassed / (float)m_uiTotalTime;
	m_pDlgScriptTip->SetShowPercent(m_fStart + (m_fEnd - m_fStart) * fPassedPercent);
}

////////////////////////////////////////////////////////////////////////////

CECScriptTipWorkGfx::CECScriptTipWorkGfx(CDlgScriptTip* pDlgScriptTip, unsigned int uiTotalTime)
: CECScriptTipWork(pDlgScriptTip) {
	m_uiTotalTime = uiTotalTime;
}

void CECScriptTipWorkGfx::OnFirstTick() {
	m_dwStartTime = GetTickCount();
	m_pDlgScriptTip->PlayGfx();
}

void CECScriptTipWorkGfx::OnTick() {
	DWORD dwTimePassed = CECTimeSafeChecker::ElapsedTimeFor(m_dwStartTime);
	if (dwTimePassed >= m_uiTotalTime) {
		SetFinished(true);
		//m_pDlgScriptTip->StopGfx();
	}
}
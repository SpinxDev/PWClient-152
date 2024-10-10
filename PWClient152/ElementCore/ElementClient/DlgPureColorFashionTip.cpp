// Filename	: DlgPureColorFashionTip.cpp
// Creator	: zhangyitian
// Date		: 2014/8/26

#include "DlgPureColorFashionTip.h"
#include "EC_TimeSafeChecker.h"

#include <AUILabel.h>
#include <AUIImagePicture.h>
#include <A3DGFXEx.h>

CDlgPureColorFashionTip::CDlgPureColorFashionTip() {
	m_bUsedRandomDye = false;
	m_dwRandomDyeUsedTime = 0;
	m_dwStartTime = 0;

	m_pLblColorName = NULL;
	m_pLblText = NULL;
	m_pImgGfx = NULL;
}

bool CDlgPureColorFashionTip::IsRandomDyeJustUsed() {
	if (!m_bUsedRandomDye) {
		return false;
	}
	const int MAX_TIME_INTERVAL = 100000;
	if (CECTimeSafeChecker::ElapsedTimeFor(m_dwRandomDyeUsedTime) > MAX_TIME_INTERVAL) {
		m_bUsedRandomDye = false;
		return false;
	}
	return true;
}

void CDlgPureColorFashionTip::SetRandomDyeUsed(bool bFlag) {
	m_bUsedRandomDye = bFlag;
	m_dwRandomDyeUsedTime = GetTickCount();
}

void CDlgPureColorFashionTip::StartShowTip(const ACString& strColor) {
	if (!IsRandomDyeJustUsed()) {
		return;
	}
	m_bUsedRandomDye = false;
	m_dwStartTime = GetTickCount();
	Show(true);
	m_pLblColorName->SetAlpha(255);
	m_pLblText->SetAlpha(255);
	m_pLblColorName->SetText(strColor);
	m_pImgGfx->GetGfx()->Stop();
	m_pImgGfx->GetGfx()->Start();
}

bool CDlgPureColorFashionTip::OnInitDialog() {
	DDX_Control("Txt_ColorName", m_pLblColorName);
	DDX_Control("Img_Gfx", m_pImgGfx);
	DDX_Control("Lbl_PureColor", m_pLblText);
	return CDlgBase::OnInitDialog();
}

void CDlgPureColorFashionTip::OnShowDialog() {
	SetPosEx(0, 200, alignCenter);
}

void CDlgPureColorFashionTip::Resize() {
	SetPosEx(0, 200, alignCenter);
}

void CDlgPureColorFashionTip::OnTick() {
	const int TOTAL_SHOW_TIME = 5000;
	const int BEGIN_FADE_OUT_TIME = 3000;
	DWORD dwTimePassed = CECTimeSafeChecker::ElapsedTimeFor(m_dwStartTime); 
	if (dwTimePassed > TOTAL_SHOW_TIME) {
		Show(false);
		return;
	}
	if (dwTimePassed > BEGIN_FADE_OUT_TIME) {
		int alpha = (float)(TOTAL_SHOW_TIME - dwTimePassed) / (TOTAL_SHOW_TIME - BEGIN_FADE_OUT_TIME) * 255.0f + 0.5f;
		m_pLblColorName->SetAlpha(alpha);
		m_pLblText->SetAlpha(alpha);
	}
}
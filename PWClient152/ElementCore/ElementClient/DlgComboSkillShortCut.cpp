// Filename	: DlgComboSkillShortCut.cpp
// Creator	: zhangyitian
// Date		: 2014/8/12

#include "DlgComboSkillShortCut.h"

#include "EC_ComboSkillState.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Skill.h"
#include "EC_UIHelper.h"
#include "EC_Viewport.h"

#include <AUIImagePicture.h>
#include <AUIProgress.h>
#include <AUILabel.h>
#include <A3DGFXEx.h>

AUI_BEGIN_EVENT_MAP(CDlgComboShortCutBar, CDlgBase)

AUI_ON_EVENT("Img_ItemSkill*", WM_LBUTTONDOWN, OnEventLButtonDown)

AUI_END_EVENT_MAP()

CDlgComboShortCutBar::CDlgComboShortCutBar() {
	CECComboSkillState::Instance().RegisterObserver(this);
	m_dwHideTime = 0;
}

CDlgComboShortCutBar::~CDlgComboShortCutBar() {
	CECComboSkillState::Instance().UnregisterObserver(this);
}

void CDlgComboShortCutBar::OnModelChange(const CECComboSkillState *p, const CECObservableChange *q) {
	const CECComboSkillChange *pChange = dynamic_cast<const CECComboSkillChange*>(q);
	if (!pChange) {
		return;
	}

	if (CECComboSkillChange::COMBOSKILL_ACTIVE == pChange->m_changeMask) {
		DWORD dwStartTime = CECComboSkillState::Instance().GetStartTime();
		const std::vector<unsigned int> vecSkills = CECComboSkillState::Instance().GetCurComboSkills();
		const std::vector<DWORD> vecStayTime = CECComboSkillState::Instance().GetSkillStayTime();

		// 给每个技能分配快捷键按钮
		std::vector<int> quickKeyIndexArray;
		if (vecSkills.size() == 1) {
			quickKeyIndexArray.push_back(1);
		} else if (vecSkills.size() == 2) {
			quickKeyIndexArray.push_back(0);
			quickKeyIndexArray.push_back(2);
		} else if (vecSkills.size() == 3) {
			quickKeyIndexArray.push_back(0);
			quickKeyIndexArray.push_back(1);
			quickKeyIndexArray.push_back(2);
		} else {
			ASSERT(vecSkills.size() == 0 && AString().Format("wrong comboskill count: %d", vecSkills.size()));
			return;
		}
		int i;
		for (i = 0; i < COMBO_SKILL_SHORTCUT_COUNT; i++) {
			m_comboShortCut[i].Show(false);
		}
		for (i = 0; i < vecSkills.size(); i++) {
			m_comboShortCut[quickKeyIndexArray[i]].Show(true, dwStartTime, vecStayTime[i], vecSkills[i]);
		}
		if (m_comboShortCut[0].IsShow() || m_comboShortCut[1].IsShow() || m_comboShortCut[2].IsShow()) {
			Show(true);
		}
	} else if (CECComboSkillChange::COMBOSKILL_STOP == pChange->m_changeMask) {
		Show(false);
		for (int i = 0; i < COMBO_SKILL_SHORTCUT_COUNT; i++) {
			m_comboShortCut[i].Show(false);
		}
	}
}

void CDlgComboShortCutBar::OnTick() {
	for (int i = 0; i < COMBO_SKILL_SHORTCUT_COUNT; i++) {
		if (m_comboShortCut[i].IsShow()) {
			m_comboShortCut[i].OnTick();
		}
	}
}

bool CDlgComboShortCutBar::OnInitDialog() {
	if (CDlgBase::OnInitDialog()) {
		for (int i = 0; i < COMBO_SKILL_SHORTCUT_COUNT; i++) {
			m_comboShortCut[i].OnInitDialog(this, i);
		}
		return true;
	}
	return false;
}

void CDlgComboShortCutBar::OnShowDialog() {
	/*
	for (int i = 0; i < COMBO_SKILL_SHORTCUT_COUNT; i++) {
		m_comboShortCut[i].OnShowDialog();
	}
	*/
}

void CDlgComboShortCutBar::CastSkill(int iKey) {
	if (iKey == 'Q') {
		m_comboShortCut[0].CastSkill();
	} else if (iKey == 'E') {
		m_comboShortCut[1].CastSkill();
	} else if (iKey == 'R') {
		m_comboShortCut[2].CastSkill();
	}
}

void CDlgComboShortCutBar::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	int index = atoi(pObj->GetName() + strlen("Img_ItemSkill"));
	m_comboShortCut[index - 1].CastSkill();
}

void CDlgComboShortCutBar::OnHideDialog() {
	m_dwHideTime = GetTickCount();
}

bool CDlgComboShortCutBar::IsUsingQuickKey() {
	if (IsShow()) {
		return true;
	}
	const int USING_QUICK_KEY_DELAY_TIME = 2000;
	if (CECTimeSafeChecker::ElapsedTimeFor(m_dwHideTime) < USING_QUICK_KEY_DELAY_TIME) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////

CECComboSkillShortCut::CECComboSkillShortCut() {
	m_pImgSkill = NULL;
	m_pProgTime = NULL;
	m_pImgCover = NULL;
	m_pLblKey = NULL;
	m_pSkill = NULL;
	m_bTimeOut = false;
}

void CECComboSkillShortCut::OnInitDialog(CDlgComboShortCutBar* pDlg, int index) {
	pDlg->DDX_Control(AString().Format("Img_ItemSkill%d", index + 1), m_pImgSkill);
	pDlg->DDX_Control(AString().Format("Prog_%d", index + 1), m_pProgTime);
	pDlg->DDX_Control(AString().Format("Img_Cover%d", index + 1), m_pImgCover);
	pDlg->DDX_Control(AString().Format("Lbl_%d", index + 1), m_pLblKey);

	AUIOBJECT_SETPROPERTY property;
	m_pImgSkill->GetProperty("Gfx File", &property);
	m_gfxName = property.fn;
}

bool CECComboSkillShortCut::IsShow() {
	return m_pImgSkill->IsShow(); 
}

void CECComboSkillShortCut::Show(bool bShow, DWORD dwStartTime, DWORD dwStayTime, unsigned int skillID) {
	if (bShow) {
		SetStartTime(dwStartTime);
		SetStayTime(dwStayTime);
		SetSkill(skillID);
		if (m_pSkill) {
			m_pImgSkill->Show(true);
			m_pProgTime->Show(true);
			m_pImgCover->Show(true);
			m_pLblKey->Show(true);
			m_bTimeOut = false;
		} else {
			m_pImgSkill->Show(false);
			m_pProgTime->Show(false);
			m_pImgCover->Show(false);
			m_pLblKey->Show(false);
		}
	} else {
		m_pImgSkill->Show(false);
		m_pProgTime->Show(false);
		m_pImgCover->Show(false);
		m_pLblKey->Show(false);
		m_pSkill = NULL;
	}
}

void CECComboSkillShortCut::SetStartTime(DWORD dwStartTime) {
	m_dwStartTime = dwStartTime;
}

void CECComboSkillShortCut::SetStayTime(DWORD dwStayTime) {
	m_dwStayTime = dwStayTime;
}

void CECComboSkillShortCut::SetSkill(unsigned int skillID) {
	m_pSkill = CECUIHelper::GetHostPlayer()->GetPositiveSkillByID(skillID);
	bool bSkillValid = true;
	if (!m_pSkill) {
		m_pSkill = CECComboSkillState::Instance().GetInherentSkillByID(skillID);
	}
	if (!m_pSkill) {
		m_pSkill = new CECSkill(skillID, 1);
		bSkillValid = false;
	}
	CECUIHelper::GetGameUIMan()->SetCover(m_pImgSkill, m_pSkill->GetIconFile(), CECGameUIMan::ICONS_SKILL);

	m_pImgSkill->SetHint(m_pSkill->GetDesc());

	if (!bSkillValid) {
		delete m_pSkill;
		m_pSkill = NULL;
		m_pImgSkill->SetGfx(NULL);
		m_pImgSkill->SetColor(A3DCOLORRGB(64, 64, 64));
	} else {
		m_pImgSkill->SetGfx(m_gfxName);
		m_pImgSkill->SetColor(A3DCOLORRGB(255, 255, 255));
	}
}

void CECComboSkillShortCut::TimeOut() {
	m_bTimeOut = true;
	m_pImgSkill->SetColor(A3DCOLORRGB(64, 64, 64));
	m_pImgSkill->GetGfx()->Stop();
	m_pProgTime->SetProgress(0);
	m_pSkill = NULL;
}

void CECComboSkillShortCut::OnTick() {
	if (m_bTimeOut) {
		return;
	}

	DWORD dwTimePassed = CECTimeSafeChecker::ElapsedTimeFor(m_dwStartTime);
	if (dwTimePassed >= m_dwStayTime) {
		dwTimePassed = m_dwStayTime;
		TimeOut();
		return;
	}

	if (m_pSkill) {
		CECHostPlayer* pHost = CECUIHelper::GetHostPlayer();
		AUIClockIcon* pClock = m_pImgSkill->GetClockIcon();
		
		if( m_pSkill && m_pSkill->ReadyToCast() && pHost->GetPrepSkill() != m_pSkill )
		{
			
			if( !pHost->CheckSkillCastCondition(m_pSkill) )
				m_pImgSkill->SetColor(A3DCOLORRGB(255, 255, 255));
			else
				m_pImgSkill->SetColor(A3DCOLORRGB(128, 128, 128));
			
		}
		else
			pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
		
		
		if( m_pSkill && (m_pSkill->GetCoolingTime() > 0 ||
			pHost->GetPrepSkill() == m_pSkill ))
		{
			pClock->SetProgressRange(0, m_pSkill->GetCoolingTime());
			if( pHost->GetPrepSkill() == m_pSkill )
				pClock->SetProgressPos(0);
			else
				pClock->SetProgressPos(m_pSkill->GetCoolingTime() - m_pSkill->GetCoolingCnt());
		}
	}

	// 设置进度条
	m_pProgTime->SetProgress((m_dwStayTime - dwTimePassed) / (float)m_dwStayTime * 100.0f + 0.5f);
}

void CECComboSkillShortCut::CastSkill() {
	if (m_pSkill) {
		CECUIHelper::GetHostPlayer()->ApplySkillShortcut(m_pSkill->GetSkillID());
	}
}

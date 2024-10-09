// Filename	: DlgSkillAction.cpp
// Creator	: zhangyitian
// Date		: 2014/6/24

#include "DlgSkillAction.h"
#include "DlgSkillSubList.h"
#include "DlgSkillSubTree.h"
#include "DlgSkillSubAction.h"
#include "DlgSkillEdit.h"

#include "EC_HostSkillModel.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_GameRun.h"
#include "ExpTypes.h"
#include "EC_TaoistRank.h"

#include <AUIManager.h>
#include <AUIRadioButton.h>
#include <AUISubDialog.h>
#include <AUILabel.h>
#include <AUICheckBox.h>

extern CECGame*		g_pGame;

AUI_BEGIN_COMMAND_MAP(CDlgSkillAction, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Rdo_Stage1",	OnCommand_Evil)
AUI_ON_COMMAND("Rdo_Stage0",	OnCommand_God)
AUI_ON_COMMAND("Rdo_SA0",		OnCommand_Skill)
AUI_ON_COMMAND("Rdo_SA1",		OnCommand_Action)
AUI_ON_COMMAND("Rdo_SA2",		OnCommand_Pool)
AUI_ON_COMMAND("Chk_CanBeLearned",	OnCommand_ShowSkillCanLearn)
AUI_END_COMMAND_MAP()

CDlgSkillAction::CDlgSkillAction() {
	m_radioBtnSkill			= NULL;
	m_radioBtnAction		= NULL;
	m_radioBtnPool			= NULL;
	m_radioBtnEvil			= NULL;
	m_radioBtnGod			= NULL;
	m_subDlgAction			= NULL;
	m_subDlgSkill			= NULL;
	m_subDlgPool			= NULL;
	m_subDlgTreeEvil		= NULL;
	m_subDlgTreeGod			= NULL;
	m_subDlgTreeBase		= NULL;
	m_subDlgOtherSkill		= NULL;
	m_labelSp				= NULL;
	m_pChkOnlyShowLearn		= NULL;
	m_pLblOnlyShowLearn		= NULL;
	m_bOpenAction			= false;
	m_bFirstShow			= true;
	m_selectedSkill			= 0;
	m_bEvil					= true;
	m_bReceivedNCPGreeting	= false;
}

bool CDlgSkillAction::OnInitDialog() {
	if (CDlgBase::OnInitDialog()) {

		DDX_Control("Rdo_SA0", m_radioBtnSkill);
		m_radioBtnSkill->Check(true);

		DDX_Control("Rdo_SA1", m_radioBtnAction);
		m_radioBtnAction->Check(false);

		DDX_Control("Rdo_SA2", m_radioBtnPool);
		m_radioBtnPool->Check(false);

		DDX_Control("Rdo_Stage1", m_radioBtnEvil);
		m_radioBtnEvil->Check(true);

		DDX_Control("Rdo_Stage0", m_radioBtnGod);
		m_radioBtnGod->Check(false);
		
		DDX_Control("Sub_Skill", m_subDlgSkill);
		m_subDlgSkill->Show(true);

		DDX_Control("Sub_Action", m_subDlgAction);
		m_subDlgAction->Show(false);

		DDX_Control("Sub_Pool", m_subDlgPool);
		m_subDlgPool->Show(false);

		DDX_Control("Sub_Right", m_subDlgOtherSkill);
		m_subDlgOtherSkill->Show(true);

		DDX_Control("Sub_Evil", m_subDlgTreeEvil);
		m_subDlgTreeEvil->Show(false);

		DDX_Control("Sub_God", m_subDlgTreeGod);
		m_subDlgTreeGod->Show(false);

		DDX_Control("Sub_Base", m_subDlgTreeBase);
		m_subDlgTreeBase->Show(false);

		DDX_Control("Txt_RestSP", m_labelSp);

		DDX_Control("Chk_CanBeLearned", m_pChkOnlyShowLearn);
		DDX_Control("Lbl_CanBeLearned", m_pLblOnlyShowLearn);

		return true;
	}
	return false;
}

void CDlgSkillAction::OnShowDialog() {
	// 如果第一次显示的话，需要设置默认的仙魔状态
	if (m_bFirstShow) {
		// 当前的修真等级
		const CECTaoistRank* curTaoistRank = CECTaoistRank::GetTaoistRank(g_pGame->GetGameRun()->GetHostPlayer()->GetBasicProps().iLevel2);
		// 如果当前人物处于仙修真，则修改默认仙魔状态
		if (curTaoistRank->IsGodRank()) {
			m_radioBtnEvil->Check(false);
			m_radioBtnGod->Check(true);
			m_bEvil = false;
			GetGameUIMan()->m_pDlgSkillSubList->ResetGodEvil();
		}
		m_bFirstShow = false;
	}
	CDlgBase::OnShowDialog();
}

void CDlgSkillAction::OnHideDialog() {
	GetGameUIMan()->m_pDlgSkillEdit->Show(false);
}

void CDlgSkillAction::OnCommand_Evil(const char* szCommand) {
	if (m_bEvil) {
		return;
	}
	m_bEvil = true;
	GetGameUIMan()->m_pDlgSkillSubList->ResetGodEvil();
}

void CDlgSkillAction::OnCommand_God(const char* szCommand) {
	if (!m_bEvil) {
		return;
	}
	m_bEvil = false;
	GetGameUIMan()->m_pDlgSkillSubList->ResetGodEvil();
}

void CDlgSkillAction::OnCommand_Skill(const char* szCommand) {
	m_subDlgSkill->Show(true);
	m_subDlgAction->Show(false);
	m_subDlgPool->Show(false);
}

void CDlgSkillAction::OnCommand_Action(const char* szCommand) {
	m_subDlgSkill->Show(false);
	m_subDlgAction->Show(true);
	m_subDlgPool->Show(false);
	HideSkillTree();
}

void CDlgSkillAction::OnCommand_Pool(const char* szCommand) {
	m_subDlgSkill->Show(false);
	m_subDlgAction->Show(false);
	m_subDlgPool->Show(true);
	HideSkillTree();
}

void CDlgSkillAction::OnCommand_ShowSkillCanLearn(const char* szCommand) {
	GetGameUIMan()->m_pDlgSkillSubList->ResetDialog();
}

void CDlgSkillAction::HideSkillTree() {
	m_subDlgOtherSkill->Show(true);
	m_subDlgTreeEvil->Show(false);
	m_subDlgTreeGod->Show(false);
	m_subDlgTreeBase->Show(false);
}

void CDlgSkillAction::ShowSkillTree(int skillID) {

	CECHostSkillModel& model = CECHostSkillModel::Instance();

	// 切换右侧对话框，并在其中显示技能树
	m_subDlgOtherSkill->Show(false);
	m_subDlgTreeEvil->Show(false);
	m_subDlgTreeGod->Show(false);
	m_subDlgTreeBase->Show(false);

	bool isEvil = m_radioBtnEvil->IsChecked();
	int rootID = model.GetRootSkill(skillID, isEvil);

	// 普通技能树
	if (model.GetSkillEvilGod(rootID) == CECHostSkillModel::SKILL_BASE) {
		m_subDlgTreeBase->Show(true);
		GetGameUIMan()->m_pDlgSkillSubTreeBase->ShowSkillTree(rootID);
	}
	// 魔技能树
	else if (model.GetSkillEvilGod(rootID) == CECHostSkillModel::SKILL_EVIL) {
		m_subDlgTreeEvil->Show(true);
		GetGameUIMan()->m_pDlgSkillSubTreeEvil->ShowSkillTree(rootID);
	}
	// 仙技能树
	else {
		m_subDlgTreeGod->Show(true);
		GetGameUIMan()->m_pDlgSkillSubTreeGod->ShowSkillTree(rootID);
	}
}

void CDlgSkillAction::OnCommand_CANCEL(const char * szCommand) {
	Show(false);
}

bool CDlgSkillAction::OnChangeLayoutBegin() {
	GetGameUIMan()->m_pDlgSkillSubList->OnChangeLayoutBegin();
	GetGameUIMan()->m_pDlgSkillSubTreeEvil->OnChangeLayoutBegin();
	GetGameUIMan()->m_pDlgSkillSubTreeGod->OnChangeLayoutBegin();
	GetGameUIMan()->m_pDlgSkillSubTreeBase->OnChangeLayoutBegin();
	return true;
}

void CDlgSkillAction::OnChangeLayoutEnd(bool bAllDone) {
	GetGameUIMan()->m_pDlgSkillSubList->OnChangeLayoutEnd(true);
	GetGameUIMan()->m_pDlgSkillSubAction->OnChangeLayoutEnd(true);
}

void CDlgSkillAction::TryOpenDialog(bool bAction) {
	if (IsShow()) {
		if (bAction && m_radioBtnAction->IsChecked()) {
			Show(false);
			return;
		}
		if (!bAction && m_radioBtnSkill->IsChecked()) {
			Show(false);
			return;
		}
		m_bOpenAction = bAction;
		ForceShowDialog();
		return;
	}
	if (!GetHostPlayer()->IsTalkingWithNPC()) {
		m_bOpenAction = bAction;
		CECHostSkillModel::Instance().SendHelloToSkillLearnNPC();
		SetReceivedNPCGreeting(false);
		ForceShowDialog();
	}
}

void CDlgSkillAction::ForceShowDialog() {
	Show(true);
	if (m_bOpenAction) {
		m_radioBtnAction->Check(true);
		m_radioBtnSkill->Check(false);
		m_radioBtnPool->Check(false);
		OnCommand_Action("");
	} else {
		m_radioBtnAction->Check(false);
		m_radioBtnSkill->Check(true);
		m_radioBtnPool->Check(false);
		OnCommand_Skill("");
	}
}

bool CDlgSkillAction::IsEvil() {
	return m_bEvil;
}

int CDlgSkillAction::GetSelectedSkillID() {
	return m_selectedSkill;
}

void CDlgSkillAction::SetSelectedSkillID(int skillID) {
	m_selectedSkill = skillID;
}

void CDlgSkillAction::OnTick() {
	m_labelSp->SetText(ACString().Format(_AL("%d"), GetHostPlayer()->GetBasicProps().iSP));

	m_subDlgTreeBase->UpdateViewport();
	m_subDlgTreeGod->UpdateViewport();
	m_subDlgTreeEvil->UpdateViewport();

	if (m_subDlgSkill->IsShow()) {
		m_pChkOnlyShowLearn->Show(true);
		m_pLblOnlyShowLearn->Show(true);
	} else {
		m_pChkOnlyShowLearn->Show(false);
		m_pLblOnlyShowLearn->Show(false);
	}
}

bool CDlgSkillAction::IsOnlyShowSkillCanLearn() {
	return m_pChkOnlyShowLearn->IsChecked();
}

bool CDlgSkillAction::IsReceivedNPCGreeting() {
	return m_bReceivedNCPGreeting;
}

void CDlgSkillAction::SetReceivedNPCGreeting(bool bReceived) {
	m_bReceivedNCPGreeting = bReceived;
}
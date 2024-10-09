// Filename	: DlgSkillSubListItem.cpp
// Creator	: zhangyitian
// Date		: 2014/6/24

#include "DlgSkillSubListItem.h"
#include "DlgSkillSubList.h"
#include "DlgSkillAction.h"
#include "DlgSkillSubPool.h"

#include "EC_HostSkillModel.h"

#include "AUIImagePicture.h"
#include "AUITextArea.h"
#include "AUIStillImageButton.h"
#include "AUISubDialog.h"
#include "AUIDef.h"
#include "AFI.h"

#include "EC_GameUIMan.h"
#include "ElementSkill.h"
#include "EC_Skill.h"
#include "EC_HostPlayer.h"

#include <typeinfo>
#include <string>

AUI_BEGIN_COMMAND_MAP(CDlgSkillSubListItem, CDlgBase)

AUI_ON_COMMAND("Btn_SLevelUp", OnCommand_Upgrade)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgSkillSubListItem, CDlgBase)

AUI_ON_EVENT("Img_SIcon",		WM_LBUTTONDOWN, OnSkillPictureDown)
AUI_ON_EVENT("Btn_SLevelUp",	WM_LBUTTONDOWN, DoNoting)
AUI_ON_EVENT("*",				WM_LBUTTONDOWN, OnEventLButtonDown)
AUI_ON_EVENT(NULL,				WM_LBUTTONDOWN, OnEventLButtonDown)
AUI_ON_EVENT("*",				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_ON_EVENT(NULL,				WM_MOUSEWHEEL,	OnMouseWheel)

AUI_END_EVENT_MAP()

const ACString l_colorWhite = _AL("^ffffff");
const ACString l_colorRed = _AL("^ff0000");

CDlgSkillSubListItem::CDlgSkillSubListItem() : m_skillID(0), m_isHighlight(false) {
	
	m_isHighlight		= false;
	m_skillIconImgPic	= NULL;
	m_skillNameLbl		= NULL;
	m_needLbl			= NULL;
	m_upgradeBtn		= NULL;
	m_levelLbl			= NULL;

	m_skillID			= 0;
	m_pCECSkill			= NULL;
	m_bSkillLearned		= false;
	m_bSkillPassive		= false;
	
	for (int i = 0; i < NeedIconImagePictureCount; i++) {
		m_needIconImgPic[i] = NULL;
	}
}

bool CDlgSkillSubListItem::OnInitDialog() {
	if (CDlgBase::OnInitDialog()) {
		DDX_Control("Img_SIcon", m_skillIconImgPic);
		DDX_Control("Txt_SName", m_skillNameLbl);
		DDX_Control("Lbl_SNeed", m_needLbl);
		DDX_Control("Btn_SLevelUp", m_upgradeBtn);
		DDX_Control("Txt_SLevel", m_levelLbl);
		for (int i = 0; i < NeedIconImagePictureCount; i++) {
			AString str;
			str.Format("Img_SNeed0%d", i + 1);
			DDX_Control(str, m_needIconImgPic[i]);
		}
		return true;
	}
	return false;
}

void CDlgSkillSubListItem::UpdateSkill(int skillID) {

	CECHostSkillModel& model = CECHostSkillModel::Instance();
	m_skillID = skillID;
	m_curLevel = model.GetSkillCurrentLevel(m_skillID);
	int requiredItem = model.GetRequiredBook(m_skillID, m_curLevel + 1);

	CECHostSkillModel::enumSkillFitLevelState fitLevel = model.GetSkillFitLevel(m_skillID);
	CECHostSkillModel::enumSkillLearnedState learnedState = model.GetSkillLearnedState(m_skillID);

	SetSkillIsLearnedAndIsPassive(m_skillID, (learnedState != CECHostSkillModel::SKILL_NOT_LEARNED));



	// 设置技能图标
	AString skillIconFile;
	af_GetFileTitle(model.GetSkillIcon(skillID), skillIconFile);
	skillIconFile.MakeLower();
	m_skillIconImgPic->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][skillIconFile]);
	ACString skillDsc;
	int reqLevel;
	int reqRealmLevel;
	if (CECHostSkillModel::SKILL_NOT_LEARNED == learnedState) {
		m_skillIconImgPic->SetColor(A3DCOLORRGB(64, 64, 64));
		skillDsc = model.GetSkillDescription(m_skillID, 1);
		reqLevel = GNET::ElementSkill::GetRequiredLevel(m_skillID, 1);	
		reqRealmLevel = GNET::ElementSkill::GetRequiredRealmLevel(m_skillID, 1);
	} else {
		m_skillIconImgPic->SetColor(A3DCOLORRGB(255, 255, 255));
		skillDsc = model.GetSkillDescription(m_skillID, m_curLevel);
		reqLevel = GNET::ElementSkill::GetRequiredLevel(m_skillID, m_curLevel);
		reqRealmLevel = GNET::ElementSkill::GetRequiredRealmLevel(m_skillID, m_curLevel);
	}
	if (reqLevel == 0) {
		reqLevel = 1;
	}
	skillDsc += ACString().Format(GetStringFromTable(11328), reqLevel);
	if (reqRealmLevel != 0) {
		skillDsc += GetStringFromTable(11401);
		skillDsc += GetGameUIMan()->GetRealmName(reqRealmLevel);
	}
	m_skillIconImgPic->SetHint(skillDsc);

	// 设置技能名称
	ACString skillName = model.GetSkillName(m_skillID);
	if (model.IsPassiveSkill(m_skillID)) {
		skillName += ACString(GetStringFromTable(11322));
	}
	m_skillNameLbl->SetText(skillName);	

	// 设置技能升级按钮
	/*
	int requiredItem = model.GetRequiredBook(m_skillID, m_curLevel + 1);
	if (CECHostSkillModel::SKILL_FULL != learnedState &&
		CECHostSkillModel::SKILL_FIT_LEVEL == fitLevel &&
		(!requiredItem || model.CheckPreItem(requiredItem))) {
		m_upgradeBtn->Show(true);
		ACString str;
		if (model.IsSkillServedByNPC(m_skillID)) {
			int needMoney = model.GetSkillMoney(m_skillID, m_curLevel + 1);
			int needSp = model.GetSkillSp(m_skillID, m_curLevel + 1);
			str.Format(GetStringFromTable(11320), needSp, needMoney);
			m_upgradeBtn->SetHint(str);
			m_upgradeBtn->Enable(true);
		} else {
			str.Format(GetStringFromTable(11321));
			m_upgradeBtn->SetHint(str);
			m_upgradeBtn->Enable(false);
		}
		
	} else {
		m_upgradeBtn->Show(false);
	}
	*/
	UpdateUpgradeBtn();

	// 设置技能级别文字
	if (CECHostSkillModel::SKILL_NOT_LEARNED == learnedState) {
		m_levelLbl->Show(false);
	} else {
		m_levelLbl->Show(true);
		ACString levelStr;
		levelStr.Format(GetStringFromTable(11323), m_curLevel);
		m_levelLbl->SetText(levelStr);
		if (CECHostSkillModel::SKILL_FULL != learnedState) {
			ACString nextDsc;
			nextDsc.Format(GetStringFromTable(11324), model.GetSkillDescription(m_skillID, m_curLevel + 1));
			int reqLevel = GNET::ElementSkill::GetRequiredLevel(m_skillID, m_curLevel + 1);
			int reqRealmLevel = GNET::ElementSkill::GetRequiredRealmLevel(m_skillID, m_curLevel + 1);
			nextDsc += ACString().Format(GetStringFromTable(11328), reqLevel);
			if (reqRealmLevel != 0) {
				nextDsc += GetStringFromTable(11401);
				nextDsc += GetGameUIMan()->GetRealmName(reqRealmLevel);
			}
			m_levelLbl->SetHint(nextDsc);
		} else {
			m_levelLbl->SetHint(GetStringFromTable(11325));
		}
	}

	// 设置前置条件
	m_needLbl->Show(false);
	for (int i = 0; i < NeedIconImagePictureCount; i++) {
		m_needIconImgPic[i]->Show(false);
	}
	// 如果技能不满级，则需要显示前提条件
	if (CECHostSkillModel::SKILL_FULL != learnedState) {
		std::vector<std::pair<unsigned int, int> > requiredSkill = model.GetRequiredSkill(m_skillID, m_curLevel + 1);
		if (requiredItem != 0 || requiredSkill.size() != 0) {
			m_needLbl->Show(true);
			int iconIndex;
			for (iconIndex = 0; iconIndex < requiredSkill.size() && iconIndex < NeedIconImagePictureCount; iconIndex++) {
				SetPreSkill(requiredSkill[iconIndex].first, requiredSkill[iconIndex].second, iconIndex);
			}
			if (requiredItem != 0 && iconIndex < NeedIconImagePictureCount) {
				SetRequiredItem(requiredItem, iconIndex);
			}
		}
	}
} 

void CDlgSkillSubListItem::SetPreSkill(int skillID, int level, int slotIndex) {
	m_needIconImgPic[slotIndex]->Show(true);

	// 技能图标
	AString skillIconFile;
	af_GetFileTitle(CECHostSkillModel::Instance().GetSkillIcon(skillID), skillIconFile);
	skillIconFile.MakeLower();
	m_needIconImgPic[slotIndex]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][skillIconFile]);

	// 文字描述
	ACString skillDsc;
	skillDsc = CECHostSkillModel::Instance().GetSkillDescription(skillID, level);
	m_needIconImgPic[slotIndex]->SetHint(skillDsc);

	if (CECHostSkillModel::Instance().CheckPreSkillLevel(skillID, level)) {
		m_needIconImgPic[slotIndex]->SetColor(A3DCOLORRGB(255, 255, 255));
	} else {
		m_needIconImgPic[slotIndex]->SetColor(A3DCOLORRGB(128, 128, 128));
	}
}

void CDlgSkillSubListItem::SetRequiredItem(int itemId, int slotIndex) {
	m_needIconImgPic[slotIndex]->Show(true);

	GetGameUIMan()->SetCover(m_needIconImgPic[slotIndex], CECHostSkillModel::Instance().GetPreItemIcon(itemId));
	
	// 文字描述
	ACString itemDsc;
	itemDsc = CECHostSkillModel::Instance().GetPreItemDescription(itemId);
	m_needIconImgPic[slotIndex]->SetHint(itemDsc);

	if (CECHostSkillModel::Instance().CheckPreItem(itemId)) {
		m_needIconImgPic[slotIndex]->SetColor(A3DCOLORRGB(255, 255, 255));
	} else {
		m_needIconImgPic[slotIndex]->SetColor(A3DCOLORRGB(128, 128, 128));
	}
}

void CDlgSkillSubListItem::OnCommand_Upgrade(const char* szCommand) {

	CECHostPlayer* player = GetHostPlayer();
	PAUIDIALOG pMsgBox;

	if (player->IsDead()		||
		player->IsSitting()		||
		player->IsChangingFace()	||
		player->IsTrading()		||
		player->GetBoothState() != 0	||
		player->IsRooting()		||
		player->IsHangerOn()	||
		player->GetCurSkill()	||
		player->IsFighting()) {
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(11327),	MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		return;
	}

	int nCondition = CECHostSkillModel::Instance().CheckLearnCondition(m_skillID);
	int nCheckCode = 0;
	

	// 代码抄自DlgTeach.cpp
	if( 1 == nCondition ) {
		nCheckCode = 270;
	} else if( 6 == nCondition ) {
		nCheckCode = 527;
	} else if( 7 == nCondition ) {
		nCheckCode = 541;
	} else if( 8 == nCondition ) {
		nCheckCode = 271;
	} else if( 9 == nCondition ) {
		nCheckCode = 556;
	} else if( 10 == nCondition ) {
		nCheckCode = 557;
	} else if (12 == nCondition) {
		nCheckCode = 11168;
	} else {
		ASSERT(nCheckCode == 0);
	}
	if (nCheckCode == 0) {
		int needMoney = CECHostSkillModel::Instance().GetSkillMoney(m_skillID, m_curLevel + 1);
		int needSp = CECHostSkillModel::Instance().GetSkillSp(m_skillID, m_curLevel + 1);
		ACString str;
		str.Format(GetStringFromTable(11326), needMoney, needSp);
		GetGameUIMan()->MessageBox("Game_LearnSkill", str, //GetGameUIMan()->GetStringFromTable(231),
				MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(m_skillID);

		// 如果打开对话框时向NPC发送的Hello消息没有收到回复，则再次发送
		if (!GetGameUIMan()->m_pDlgSkillAction->IsReceivedNPCGreeting()) {
			CECHostSkillModel::Instance().SendHelloToSkillLearnNPC();
		}
	} else {
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(nCheckCode),	MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
	}
}

void CDlgSkillSubListItem::SetHighlight(bool flag) {
	if (flag == m_isHighlight) {
		return;
	}
	m_isHighlight = flag;
	AUIOBJECT_SETPROPERTY objProperty;
	if (flag) {
		GetGameUIMan()->m_pDlgSkillAction->GetDlgItem("Img_Highlight")->GetProperty("Image File", &objProperty);	
	} else {
		GetGameUIMan()->m_pDlgSkillAction->GetDlgItem("Img_Normal")->GetProperty("Image File", &objProperty);
	}
	ScopedDialogSpriteModify _dummy(this);
	GetDlgItem("Edi_subSkill")->SetProperty("Frame Image", &objProperty);
}

void CDlgSkillSubListItem::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	GetGameUIMan()->m_pDlgSkillSubList->SelectSkill(m_skillID);
}

void CDlgSkillSubListItem::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	GetGameUIMan()->m_pDlgSkillSubList->GetParentDlgControl()->OnDlgItemMessage(WM_MOUSEWHEEL, wParam, lParam);
}

void CDlgSkillSubListItem::OnSkillPictureDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {

	// 如果没有学或者是被动技能，则不可拖动和施放
	if (!m_bSkillLearned || m_bSkillPassive) {
		return;
	}

	pObj->SetDataPtr((void*)m_pCECSkill, "ptr_CECSkill");

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}

void CDlgSkillSubListItem::SetSkillIsLearnedAndIsPassive(int id, bool learned) {
	m_pCECSkill = GetHostPlayer()->GetPositiveSkillByID(id);

	// m_pCECSkill可能为空，所以需要重新创建一个用于查询是否被动
	CECSkill skill(id, 1);
	m_bSkillPassive = (skill.GetType() == CECSkill::TYPE_PASSIVE);
	m_bSkillLearned = learned;
}

void CDlgSkillSubListItem::OnTick() {

	if (!m_bSkillPassive && m_bSkillLearned) {
		CDlgSkillSubPool::TickSkillIcon(m_skillIconImgPic, m_pCECSkill);
	}

	UpdateUpgradeBtn();
}

void CDlgSkillSubListItem::EnableUpgrade(bool bEnable) {
	m_upgradeBtn->Enable(bEnable);
}

void CDlgSkillSubListItem::UpdateUpgradeBtn() {

	CECHostSkillModel& model = CECHostSkillModel::Instance();
	CECHostSkillModel::enumSkillFitLevelState fitLevel = model.GetSkillFitLevel(m_skillID);
	CECHostSkillModel::enumSkillLearnedState learnedState = model.GetSkillLearnedState(m_skillID);

	int requiredItem = model.GetRequiredBook(m_skillID, m_curLevel + 1);
	if (CECHostSkillModel::SKILL_FULL != learnedState &&
		CECHostSkillModel::SKILL_FIT_LEVEL == fitLevel &&
		(!requiredItem || model.CheckPreItem(requiredItem))) {
		m_upgradeBtn->Show(true);
		ACString str;
		if (model.IsSkillServedByNPC(m_skillID)) {
			int needSp = model.GetSkillSp(m_skillID, m_curLevel + 1);
			int needMoney = model.GetSkillMoney(m_skillID, m_curLevel + 1);
			
			int curSp = GetHostPlayer()->GetBasicProps().iSP;
			int curMoney = GetHostPlayer()->GetMoneyAmount();

			
		
			bool spOK = curSp >= needSp;
			bool moneyOK = curMoney >= needMoney;
			bool preSkillOK = true;

			std::vector<std::pair<unsigned int, int> > requiredSkill = model.GetRequiredSkill(m_skillID, m_curLevel + 1);
			for (int i = 0; i < requiredSkill.size(); i++) {
				if (!model.CheckPreSkillLevel(requiredSkill[i].first, requiredSkill[i].second)) {
					preSkillOK = false;
					break;
				}
			}
			

			if (spOK && moneyOK && preSkillOK) {
				m_upgradeBtn->Enable(true);
			} else {
				m_upgradeBtn->Enable(false);
			}

			ACString strSp;
			ACString strMoney;
			ACString strPreSkill;
			strSp.Format(GetStringFromTable(11402), needSp);
			strMoney.Format(GetStringFromTable(11403), needMoney);
			
			if (spOK) {
				strSp = l_colorWhite + strSp;
			} else {
				strSp = l_colorRed + strSp;
			}
			if (moneyOK) {
				strMoney = l_colorWhite + strMoney;
			}  else {
				strMoney = l_colorRed + strMoney;
			}
			if (!preSkillOK) {
				strPreSkill = GetStringFromTable(11404);
			}

			m_upgradeBtn->SetHint(strSp + strMoney + strPreSkill);
		} else {
			str.Format(GetStringFromTable(11321));
			m_upgradeBtn->SetHint(str);
			m_upgradeBtn->Enable(false);
		}
		
	} else {
		m_upgradeBtn->Show(false);
	}
}
// Filename	: DlgSkillSubList.cpp
// Creator	: zhangyitian
// Date		: 2014/6/23

#include "DlgSkillSubList.h"
#include "DlgSkillSubListItem.h"
#include "DlgSkillAction.h"
#include "DlgSkillSubAction.h"
#include "DlgSystem.h"
#include "DlgSystem2.h"

#include "AUIManager.h"
#include "AUILabel.h"
#include "AUISubDialog.h"
#include "AUIDef.h"

#include "EC_HostSkillModel.h"
#include "EC_SkillConvert.h"
#include "ElementSkill.h"
#include "EC_TaoistRank.h"
#include "EC_GameUIMan.h"

CDlgSkillSubList::CDlgSkillSubList() : 
m_skillSubCount(0), m_curBottom(0), m_originBottom(0), m_pSubRank(NULL), m_pSubSkill(NULL), m_bAllocRankDlgs(false) {
	// 注册观察者
	CECHostSkillModel::Instance().RegisterObserver(this);
}

CDlgSkillSubList::~CDlgSkillSubList() {
	CECHostSkillModel::Instance().UnregisterObserver(this);
}

bool CDlgSkillSubList::Release() {

	abase::hash_map<int, AUISubDialog*>::iterator itr;
	for (itr = m_skillSubDialogsMap.begin(); itr != m_skillSubDialogsMap.end(); ++itr) {
		itr->second->SetDialog(NULL);
	}
	
	size_t i;
	for (i = 0; i < m_skillSubDialogs.size(); i++) {
		m_skillSubDialogs[i]->SetDialog(NULL);
	}

	if (!CDlgBase::Release()) {
		return false;
	}

	return true;
}

bool CDlgSkillSubList::OnInitDialog() {
	if (CDlgBase::OnInitDialog()) {
		DDX_Control("Sub_Rank", m_pSubRank);
		DDX_Control("Sub_Skill", m_pSubSkill);

		m_pSubRank->Show(false);
		m_pSubSkill->Show(false);
		
		m_rankObjName = m_pSubRank->GetName();
		m_skillObjName = m_pSubSkill->GetName();

		m_rankDlgFile = m_pSubRank->GetSubDialog()->GetFilename();
		m_skillDlgFile = m_pSubSkill->GetSubDialog()->GetFilename();

		m_rankDlgName = m_pSubRank->GetSubDialog()->GetName();
		m_skillDlgName = m_pSubSkill->GetSubDialog()->GetName();

		AString newRankDlgName;
		newRankDlgName.Format("%s%s", m_rankDlgName, "template");
		m_pSubRank->GetSubDialog()->SetName(newRankDlgName);

		AString newSkillDlgName;
		newSkillDlgName.Format("%s%s", m_skillDlgName, "template");
		m_pSubSkill->GetSubDialog()->SetName(newSkillDlgName);
		
		m_pSubRank->SetDialog(NULL);
		m_pSubSkill->SetDialog(NULL);
	
		m_rankHeight = m_pSubRank->GetSize().cy;
		m_skillHeight = m_pSubSkill->GetSize().cy;

		m_originBottom = m_pSubRank->GetPos().y;
		m_originWidth = GetSize().cx;
		m_originHeight = GetSize().cy;

		
		return true;
	}
	return false;
}

void CDlgSkillSubList::OnShowDialog() {
	InitRankDlgs();
	ResetDialog();

	GetGameUIMan()->m_pDlgSystem->ShowNewImg(false);
	GetGameUIMan()->m_pDlgSystemb->ShowNewImg(false);
	GetGameUIMan()->m_pDlgSystem5->ShowNewImg(false);
	GetGameUIMan()->m_pDlgSystem5b->ShowNewImg(false);
}

void CDlgSkillSubList::InitRankDlgs() {
	if (m_bAllocRankDlgs) {
		return;
	}
	m_bAllocRankDlgs = true;
	AString dlgName;
	const CECTaoistRank* taoistRank;
	for (taoistRank = CECTaoistRank::GetBaseRankBegin(); taoistRank != CECTaoistRank::GetBaseRankEnd();
	taoistRank = taoistRank->GetNext()) {
		CreateOneRankDlg(taoistRank);
	}
	for (taoistRank = CECTaoistRank::GetGodRankBegin(); taoistRank != CECTaoistRank::GetGodRankEnd();
	taoistRank = taoistRank->GetNext()) {
		CreateOneRankDlg(taoistRank);
	}
	for (taoistRank = CECTaoistRank::GetEvilRankBegin(); taoistRank != CECTaoistRank::GetEvilRankEnd();
	taoistRank = taoistRank->GetNext()) {
		CreateOneRankDlg(taoistRank);
	}
		
}

void CDlgSkillSubList::ResetDialog() {
	
	// 清除
	m_skillSubDialogsMap.clear();
	m_skillSubCount = 0;
	m_curBottom = m_originBottom;

	for (abase::hash_map<int, AUISubDialog*>::iterator itr1 = m_rankSubDialogs.begin();
	itr1 != m_rankSubDialogs.end(); ++itr1) {
		itr1->second->Show(false);
	}

	for (abase::vector<AUISubDialog*>::iterator itr2 = m_skillSubDialogs.begin();
	itr2 != m_skillSubDialogs.end(); ++itr2) {
		(*itr2)->Show(false);
	}

	const abase::hash_map<int, abase::vector<int> >& allRankProfSkills
		= CECHostSkillModel::Instance().GetAllRankProfSkills();

	// 遍历所有修真级别
	const CECTaoistRank* taoistRank;
	for (taoistRank = CECTaoistRank::GetBaseRankBegin(); taoistRank != CECTaoistRank::GetBaseRankEnd();
	taoistRank = taoistRank->GetNext()) {
		AddDlgsOfOneRank(taoistRank);
	}

	for (taoistRank = CECTaoistRank::GetGodRankBegin(); taoistRank != CECTaoistRank::GetGodRankEnd();
	taoistRank = taoistRank->GetNext()) {
		AddDlgsOfOneRank(taoistRank);
	}

	for (taoistRank = CECTaoistRank::GetEvilRankBegin(); taoistRank != CECTaoistRank::GetEvilRankEnd();
	taoistRank = taoistRank->GetNext()) {
		AddDlgsOfOneRank(taoistRank);
	}


	// 当界面风格切换时，可能ParentControl还没有被赋值，因此调整父控件的尺寸和调整滚动条位置可能被延迟进行
	if (GetParentDlgControl() != NULL) {
		FitSize();
		ShowLastSelectedSkill();
	}
}

void CDlgSkillSubList::ShowLastSelectedSkill() {
	int originSelected = GetSelectedSkillID();
	SetSelectedSkillID(0);
	SelectSkill(originSelected);
	ScrollToShowSelectedSkill();
}

void CDlgSkillSubList::UpdateOneSubDlg(int skillID) {
	AUISubDialog* pSub = m_skillSubDialogsMap.find(skillID)->second;
	CDlgSkillSubListItem* subListItem = dynamic_cast<CDlgSkillSubListItem*>(pSub->GetSubDialog());
	subListItem->UpdateSkill(skillID);

	if (GetSelectedSkillID() == skillID && !GetGameUIMan()->m_pDlgSkillSubAction->IsShow()) {
		// 更新技能树的显示
		GetGameUIMan()->m_pDlgSkillAction->ShowSkillTree(skillID);
	}
}

void CDlgSkillSubList::AddSkillSubDlg(int skillID) {

	// 如果已建立的技能条对话框数目不够，则新建一个
	if ((size_t)m_skillSubCount >= m_skillSubDialogs.size()) {

		// 复制AUISubDialog
		PAUISUBDIALOG pSubSkill = (PAUISUBDIALOG)CloneControl(m_pSubSkill);
		AString objName;
		objName.Format("%s%d", m_skillObjName, m_skillSubDialogs.size());
		
		// 创建对话框
		PAUIDIALOG pDlg = NULL;
		pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_skillDlgFile));
		
		// 设置名称
		AString dlgName;
		dlgName.Format("%s%d", m_skillDlgName, m_skillSubDialogs.size());
		pDlg->SetName(dlgName);
		
		pSubSkill->SetDialog(pDlg);
		m_skillSubDialogs.push_back(pSubSkill);
	}
		
	PAUISUBDIALOG curSubSkill = m_skillSubDialogs[m_skillSubCount];
	dynamic_cast<CDlgSkillSubListItem*>(curSubSkill->GetSubDialog())->SetHighlight(false);
	curSubSkill->SetPos(0, m_curBottom);
	curSubSkill->Show(true);
	curSubSkill->SetData(m_curBottom);
	m_curBottom += (int)(m_skillHeight * m_pAUIManager->GetWindowScale() + 0.5f);
	m_skillSubDialogsMap[skillID] = curSubSkill;
	m_skillSubCount++;
	UpdateOneSubDlg(skillID);
}

void CDlgSkillSubList::AddRankSubDig(int rankID) {
	
	abase::hash_map<int, AUISubDialog*>::iterator itr = m_rankSubDialogs.find(rankID);
	ASSERT(itr != m_rankSubDialogs.end());

	PAUISUBDIALOG pSub = itr->second;
	pSub->Show(true);
	pSub->SetPos(0, m_curBottom);
	m_curBottom += (int)(m_rankHeight * m_pAUIManager->GetWindowScale() + 0.5f);
	
	AUILabel* label = dynamic_cast<AUILabel*>(pSub->GetSubDialog()->GetDlgItem("Txt_Stage"));
	label->SetText(CECTaoistRank::GetTaoistRank(rankID)->GetName());

}

void CDlgSkillSubList::FitSize() {
	SIZE size;
	size.cx = m_originWidth;
	size.cy = m_curBottom;
	SetSize(m_originWidth, m_curBottom);
	GetParentDlgControl()->SetSubDialogOrgSize(size.cx, size.cy);
}

void CDlgSkillSubList::ResetGodEvil() {
	if (this->IsShow()) {
		ResetDialog();
	}
}

void CDlgSkillSubList::OnModelChange(const CECHostSkillModel *p, const CECObservableChange *q) {
	
	if (!GetGameUIMan()->m_pDlgSkillAction->IsShow()) {
		return;
	}
	const CECSkillPanelChange* modelChange = dynamic_cast<const CECSkillPanelChange*>(q);
	if (!q) {
		ASSERT(false);
	}
	// 有技能被覆盖了，需要重置整个界面
	if (CECSkillPanelChange::CHANGE_SKILL_OVERRIDDEN == modelChange->m_changeMask) {
		this->ResetDialog();
	}
	// 技能升级，需要改变一条技能
	else if (CECSkillPanelChange::CHANGE_SKILL_LEVEL_UP == modelChange->m_changeMask) {
		if (modelChange->m_skillLevel == 1) {
			// 如果新学会了技能，其它技能的需求图标可能需要点亮，故更新全部技能
			abase::hash_map<int, AUISubDialog*>::iterator itr;
			for (itr = m_skillSubDialogsMap.begin(); itr != m_skillSubDialogsMap.end(); ++itr) {
				this->UpdateOneSubDlg(itr->first);
			}
		} else {
			// 只需更新一个技能
			this->UpdateOneSubDlg(modelChange->m_skillID);
		}
	}
	// 场景切换导致NPC变化
	else if (CECSkillPanelChange::CHANGE_SKILL_NPC == modelChange->m_changeMask) {
		GetGameUIMan()->m_pDlgSkillAction->Show(false);
	}
}

void CDlgSkillSubList::SelectSkill(int skillID) {
	
	if (skillID != 0 && m_skillSubDialogsMap.find(skillID) == m_skillSubDialogsMap.end()) {
		int convertSkillID = CECSkillConvert::Instance().GetConvertSkill(skillID);
		if (convertSkillID != 0) {
			skillID = convertSkillID;
		}
	}

	if (skillID != 0 && m_skillSubDialogsMap.find(skillID) == m_skillSubDialogsMap.end()) {
		SetSelectedSkillID(0);
		GetGameUIMan()->m_pDlgSkillAction->HideSkillTree();
		return;
	}

	CDlgSkillSubListItem* sublistItem;

	// 将原先可能选中的技能条设为未选中
	int originSelectedSkillID = GetSelectedSkillID();
	if (originSelectedSkillID != 0) {
		AUISubDialog* sub = m_skillSubDialogsMap.find(originSelectedSkillID)->second;
		sublistItem = dynamic_cast<CDlgSkillSubListItem*>(sub->GetSubDialog());
		sublistItem->SetHighlight(false);
	}

	// 选中新的技能条
	if (skillID != 0) {
		AUISubDialog* sub = m_skillSubDialogsMap.find(skillID)->second;
		sublistItem = dynamic_cast<CDlgSkillSubListItem*>(sub->GetSubDialog());
		sublistItem->SetHighlight(true);

		// 在右侧绘制技能树
		GetGameUIMan()->m_pDlgSkillAction->ShowSkillTree(skillID);
	} else {
		// 没有技能被选中，隐藏技能树
		GetGameUIMan()->m_pDlgSkillAction->HideSkillTree();
	}

	SetSelectedSkillID(skillID);
}

void CDlgSkillSubList::ScrollToShowSelectedSkill() {

	if (GetSelectedSkillID() == 0) {
		return;
	}

	AUISubDialog* sub = m_skillSubDialogsMap.find(GetSelectedSkillID())->second;
	
	int subTop = sub->GetData();
	int subBottom = subTop + sub->GetSize().cy;
	
	float f1 = m_pAUIManager->GetWindowScale();
	float f2 = GetScale();
	
	int fixedHeight = GetParentDlgControl()->GetSize().cy;	// 对话框的固定高度
	int expandedHeight = m_curBottom;						// 对话框展开的高度
	int midTop = fixedHeight / 2;						// 滚动条拖到顶端时对话框中心的高度坐标
	int midBottom = expandedHeight - fixedHeight / 2;	// 滚动条拖到底端时对话框中心的高度坐标

	if (fixedHeight >= expandedHeight) {
		return;
	}
	
	// 需要调整，尽量将技能条放在中间
	int mid = (subTop + subBottom) / 2;
	if (mid < midTop) {
		mid = midTop;
	}
	if (mid > midBottom) {
		mid = midBottom;
	}
	int vBarPos = (float)(mid - midTop) / (float)(midBottom - midTop) * (float)GetParentDlgControl()->GetVBarMax();
			GetParentDlgControl()->SetVBarPos(vBarPos);
}

bool CDlgSkillSubList::OnChangeLayoutBegin() {
	// 清除所有修真级别对话框
	abase::hash_map<int, AUISubDialog*>::iterator itr;
	for (itr = m_rankSubDialogs.begin(); itr != m_rankSubDialogs.end(); ++itr) {
		// 清除Dlg
		itr->second->GetSubDialog()->SetDestroyFlag(true);
		// 清除OBJ
		DeleteControl(itr->second);
	}

	// 清除所有技能条对话框
	size_t i;
	for (i = 0; i < m_skillSubDialogs.size(); i++) {
		// 清除Dlg
		m_skillSubDialogs[i]->GetSubDialog()->SetDestroyFlag(true);
		// 清除OBJ
		DeleteControl(m_skillSubDialogs[i]);
	}

	// 将滚动条位置设为0
	PAUISUBDIALOG pParent = GetParentDlgControl();
	pParent->SetHBarPos(0);
	pParent->SetVBarPos(0);

	// 执行删除对话框操作
	m_pAUIManager->Tick(0);

	return true;
}

void CDlgSkillSubList::OnChangeLayoutEnd(bool bAllDone) {
	if (IsShow()) {
		FitSize();
		ShowLastSelectedSkill();
	}
}

void CDlgSkillSubList::CreateOneRankDlg(const CECTaoistRank* taoistRank) {
	// 复制AUISubDialog
	PAUISUBDIALOG pSubRank = (PAUISUBDIALOG)CloneControl(m_pSubRank);
	AString objName;
	objName.Format("%s%d", m_rankObjName, taoistRank->GetID());
	pSubRank->SetName(objName);
	
	// 创建对话框
	PAUIDIALOG pDlg = NULL;
	pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_rankDlgFile));
	
	// 设置名称
	AString dlgName;
	dlgName.Format("%s%d", m_rankDlgName, taoistRank->GetID());
	pDlg->SetName(dlgName);
	
	pSubRank->SetDialog(pDlg);
	pSubRank->Show(false);
	
	m_rankSubDialogs[taoistRank->GetID()] = pSubRank;
}

void CDlgSkillSubList::AddDlgsOfOneRank(const CECTaoistRank* taoistRank) {
	CECHostSkillModel& model = CECHostSkillModel::Instance();
	const abase::hash_map<int, abase::vector<int> >& allRankProfSkills
		= model.GetAllRankProfSkills();

	int rankID = taoistRank->GetID();
	
	if (IsEvil() && taoistRank->IsGodRank()) {
		return;
	} else if (!IsEvil() && taoistRank->IsEvilRank()) {
		return;
	}
	abase::hash_map<int, abase::vector<int> >::const_iterator rankItr = allRankProfSkills.find(rankID);
	if (rankItr != allRankProfSkills.end()) {
		abase::vector<int> rankSkills;
		for (abase::vector<int>::const_iterator skillItr = rankItr->second.begin();
		skillItr != rankItr->second.end(); ++skillItr) {
			int skillID = *skillItr;
			if (GNET::ElementSkill::IsOverridden(skillID)) {
				continue;
			}
			bool bOnlyShowSkillCanLearn = GetGameUIMan()->m_pDlgSkillAction->IsOnlyShowSkillCanLearn();
			if (bOnlyShowSkillCanLearn) {
				if (model.GetSkillFitLevel(skillID) == CECHostSkillModel::SKILL_NOT_FIT_LEVEL) {
					continue;
				}
				int curLevel = model.GetSkillCurrentLevel(skillID);
				int requiredItem = model.GetRequiredBook(skillID, curLevel + 1);
				if (requiredItem && !model.CheckPreItem(requiredItem)) {
					continue;
				}
			}
			
			rankSkills.push_back(skillID);
		}
		// 如果没有需要显示的技能，则跳过
		if (rankSkills.empty()) {
			return;
		}
		
		// 增加修真级别对话框和技能对话框
		AddRankSubDig(rankID);
		for (abase::vector<int>::iterator it = rankSkills.begin(); it != rankSkills.end(); ++it) {
			AddSkillSubDlg(*it);
		}
	}
}

int CDlgSkillSubList::GetSelectedSkillID() {
	return GetGameUIMan()->m_pDlgSkillAction->GetSelectedSkillID();
}

void CDlgSkillSubList::SetSelectedSkillID(int skillID) {
	GetGameUIMan()->m_pDlgSkillAction->SetSelectedSkillID(skillID);
}

bool CDlgSkillSubList::IsEvil() {
	return GetGameUIMan()->m_pDlgSkillAction->IsEvil();
}

bool CDlgSkillSubList::IsSkillOrConvertSkillExist(int skillID) {
	if (m_skillSubDialogsMap.find(skillID) != m_skillSubDialogsMap.end()) {
		return true;
	}
	int convertSkillID = CECSkillConvert::Instance().GetConvertSkill(skillID);
	if (m_skillSubDialogsMap.find(convertSkillID) != m_skillSubDialogsMap.end()) {
		return true;
	}
	return false;
}

void CDlgSkillSubList::EnableUpgrade(bool bEnable) {
	abase::hash_map<int, AUISubDialog*>::iterator itr = m_skillSubDialogsMap.begin();
	for (; itr != m_skillSubDialogsMap.end(); ++itr) {
		CDlgSkillSubListItem* item = dynamic_cast<CDlgSkillSubListItem*>(itr->second->GetSubDialog());
		item->EnableUpgrade(bEnable);
	}
}
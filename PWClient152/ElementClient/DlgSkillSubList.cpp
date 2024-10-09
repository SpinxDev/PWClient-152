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
	// ע��۲���
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
	
	// ���
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

	// �����������漶��
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


	// ���������л�ʱ������ParentControl��û�б���ֵ����˵������ؼ��ĳߴ�͵���������λ�ÿ��ܱ��ӳٽ���
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
		// ���¼���������ʾ
		GetGameUIMan()->m_pDlgSkillAction->ShowSkillTree(skillID);
	}
}

void CDlgSkillSubList::AddSkillSubDlg(int skillID) {

	// ����ѽ����ļ������Ի�����Ŀ���������½�һ��
	if ((size_t)m_skillSubCount >= m_skillSubDialogs.size()) {

		// ����AUISubDialog
		PAUISUBDIALOG pSubSkill = (PAUISUBDIALOG)CloneControl(m_pSubSkill);
		AString objName;
		objName.Format("%s%d", m_skillObjName, m_skillSubDialogs.size());
		
		// �����Ի���
		PAUIDIALOG pDlg = NULL;
		pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_skillDlgFile));
		
		// ��������
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
	// �м��ܱ������ˣ���Ҫ������������
	if (CECSkillPanelChange::CHANGE_SKILL_OVERRIDDEN == modelChange->m_changeMask) {
		this->ResetDialog();
	}
	// ������������Ҫ�ı�һ������
	else if (CECSkillPanelChange::CHANGE_SKILL_LEVEL_UP == modelChange->m_changeMask) {
		if (modelChange->m_skillLevel == 1) {
			// �����ѧ���˼��ܣ��������ܵ�����ͼ�������Ҫ�������ʸ���ȫ������
			abase::hash_map<int, AUISubDialog*>::iterator itr;
			for (itr = m_skillSubDialogsMap.begin(); itr != m_skillSubDialogsMap.end(); ++itr) {
				this->UpdateOneSubDlg(itr->first);
			}
		} else {
			// ֻ�����һ������
			this->UpdateOneSubDlg(modelChange->m_skillID);
		}
	}
	// �����л�����NPC�仯
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

	// ��ԭ�ȿ���ѡ�еļ�������Ϊδѡ��
	int originSelectedSkillID = GetSelectedSkillID();
	if (originSelectedSkillID != 0) {
		AUISubDialog* sub = m_skillSubDialogsMap.find(originSelectedSkillID)->second;
		sublistItem = dynamic_cast<CDlgSkillSubListItem*>(sub->GetSubDialog());
		sublistItem->SetHighlight(false);
	}

	// ѡ���µļ�����
	if (skillID != 0) {
		AUISubDialog* sub = m_skillSubDialogsMap.find(skillID)->second;
		sublistItem = dynamic_cast<CDlgSkillSubListItem*>(sub->GetSubDialog());
		sublistItem->SetHighlight(true);

		// ���Ҳ���Ƽ�����
		GetGameUIMan()->m_pDlgSkillAction->ShowSkillTree(skillID);
	} else {
		// û�м��ܱ�ѡ�У����ؼ�����
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
	
	int fixedHeight = GetParentDlgControl()->GetSize().cy;	// �Ի���Ĺ̶��߶�
	int expandedHeight = m_curBottom;						// �Ի���չ���ĸ߶�
	int midTop = fixedHeight / 2;						// �������ϵ�����ʱ�Ի������ĵĸ߶�����
	int midBottom = expandedHeight - fixedHeight / 2;	// �������ϵ��׶�ʱ�Ի������ĵĸ߶�����

	if (fixedHeight >= expandedHeight) {
		return;
	}
	
	// ��Ҫ�����������������������м�
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
	// ����������漶��Ի���
	abase::hash_map<int, AUISubDialog*>::iterator itr;
	for (itr = m_rankSubDialogs.begin(); itr != m_rankSubDialogs.end(); ++itr) {
		// ���Dlg
		itr->second->GetSubDialog()->SetDestroyFlag(true);
		// ���OBJ
		DeleteControl(itr->second);
	}

	// ������м������Ի���
	size_t i;
	for (i = 0; i < m_skillSubDialogs.size(); i++) {
		// ���Dlg
		m_skillSubDialogs[i]->GetSubDialog()->SetDestroyFlag(true);
		// ���OBJ
		DeleteControl(m_skillSubDialogs[i]);
	}

	// ��������λ����Ϊ0
	PAUISUBDIALOG pParent = GetParentDlgControl();
	pParent->SetHBarPos(0);
	pParent->SetVBarPos(0);

	// ִ��ɾ���Ի������
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
	// ����AUISubDialog
	PAUISUBDIALOG pSubRank = (PAUISUBDIALOG)CloneControl(m_pSubRank);
	AString objName;
	objName.Format("%s%d", m_rankObjName, taoistRank->GetID());
	pSubRank->SetName(objName);
	
	// �����Ի���
	PAUIDIALOG pDlg = NULL;
	pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_rankDlgFile));
	
	// ��������
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
		// ���û����Ҫ��ʾ�ļ��ܣ�������
		if (rankSkills.empty()) {
			return;
		}
		
		// �������漶��Ի���ͼ��ܶԻ���
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
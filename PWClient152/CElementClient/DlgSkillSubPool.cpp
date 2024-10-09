// Filename	: DlgSkillSubPool.cpp
// Creator	: zhangyitian
// Date		: 2014/08/04

#include "DlgSkillSubPool.h"

#include "EC_GameUIMan.h"
#include "EC_HostSkillModel.h"
#include "EC_HostPlayer.h"
#include "EC_Skill.h"
#include "EC_UIHelper.h"

#include "AUIClockIcon.h"
#include "AUIImagePicture.h"
#include "AFI.h"

AUI_BEGIN_EVENT_MAP(CDlgSkillSubPool, CDlgBase)

AUI_ON_EVENT("Img_*", WM_LBUTTONDOWN, OnEventLButtonDown)

AUI_END_EVENT_MAP()

void CDlgSkillSubPool::OnShowDialog() {
	UpdateView();
}

void CDlgSkillSubPool::OnTick() {
	// 如果已学技能发生改变，则重置整个界面
	if (CheckSkillChanged()) {
		UpdateView();
	}
	
	// 更新技能的CD显示
	int activeImgPicIndex = 1;
	while (true) {
		AUIImagePicture* pImgPic = dynamic_cast<AUIImagePicture*>(GetDlgItem(AString().Format("Img_%02d", activeImgPicIndex++)));
		if (!pImgPic) {
			break;
		}
		CECSkill* pSkill = (CECSkill*)pImgPic->GetDataPtr("ptr_CECSkill");
		if (!pSkill) {
			break;
		}
		TickSkillIcon(pImgPic, pSkill);
	}
}

void CDlgSkillSubPool::UpdateView() {

	m_skills.clear();

	CECHostPlayer* pHost = GetHostPlayer();
	int i = 0;
	int activeImgPicIndex = 1;
	int passiveImgPicIndex = 1;
	int positiveSkillNum = pHost->GetPositiveSkillNum();
	int equipSkillNum = pHost->GetEquipSkillNum();
	int passiveSkillNum = pHost->GetPassiveSkillNum();
	
	// 设置所有主动技能 
	for (i = 0; i < positiveSkillNum; i++) {
		AUIImagePicture* pImgPic = dynamic_cast<AUIImagePicture*>(GetDlgItem(AString().Format("Img_%02d", activeImgPicIndex++)));
		if (!pImgPic) {
			break;
		}
		SetImg(pHost->GetPositiveSkillByIndex(i), pImgPic);
		m_skills.insert(pHost->GetPositiveSkillByIndex(i)->GetSkillID());
	}
	for (i = 0; i < equipSkillNum; i++) {
		AUIImagePicture* pImgPic = dynamic_cast<AUIImagePicture*>(GetDlgItem(AString().Format("Img_%02d", activeImgPicIndex++)));
		if (!pImgPic) {
			break;
		}
		SetImg(pHost->GetEquipSkillByIndex(i), pImgPic);
		m_skills.insert(pHost->GetEquipSkillByIndex(i)->GetSkillID());
	}
	while (true) {
		AUIImagePicture* pImgPic = dynamic_cast<AUIImagePicture*>(GetDlgItem(AString().Format("Img_%02d", activeImgPicIndex++)));
		if (!pImgPic) {
			break;
		}
		SetImg(NULL, pImgPic);
	}

	// 设置所有被动技能
	for (i = 0; i < passiveSkillNum; i++) {
		AUIImagePicture* pImgPic = dynamic_cast<AUIImagePicture*>(GetDlgItem(AString().Format("Passive_%02d", passiveImgPicIndex++)));
		if (!pImgPic) {
			break;
		}
		SetImg(pHost->GetPassiveSkillByIndex(i), pImgPic);
		m_skills.insert(pHost->GetPassiveSkillByIndex(i)->GetSkillID());
	}
	while (true) {
		AUIImagePicture* pImgPic = dynamic_cast<AUIImagePicture*>(GetDlgItem(AString().Format("Passive_%02d", passiveImgPicIndex++)));
		if (!pImgPic) {
			break;
		}
		SetImg(NULL, pImgPic);
	}
}

void CDlgSkillSubPool::SetImg(CECSkill* pSkill, AUIImagePicture* pImgPic) {
	if (pSkill == NULL) {
		pImgPic->SetCover(NULL, 0);
		pImgPic->SetDataPtr(NULL);
		pImgPic->SetHint(_AL(""));
	} else {
		// 图标
		AString skillIconFile(pSkill->GetIconFile());
		skillIconFile.MakeLower();
		GetGameUIMan()->SetCover(pImgPic, skillIconFile, CECGameUIMan::ICONS_SKILL);
		
		// 悬浮文字
		ACString skillDsc(pSkill->GetDesc());
		pImgPic->SetHint(skillDsc);

		// 数据
		pImgPic->SetDataPtr((void*)pSkill, "ptr_CECSkill");
	}
}

void CDlgSkillSubPool::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	if (pObj->GetDataPtr("ptr_CECSkill") == NULL) {
		return;
	}
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}

bool CDlgSkillSubPool::CheckSkillChanged() {
	CECHostPlayer* pHost = GetHostPlayer();
	int positiveSkillNum = pHost->GetPositiveSkillNum();
	int equipSkillNum = pHost->GetEquipSkillNum();
	int passiveSkillNum = pHost->GetPassiveSkillNum();
	if (positiveSkillNum + equipSkillNum + passiveSkillNum != m_skills.size()) {
		return true;
	}
	int i;
	for (i = 0; i < positiveSkillNum; i++) {
		if (m_skills.find(pHost->GetPositiveSkillByIndex(i)->GetSkillID()) == m_skills.end()) {
			return true;
		}
	}
	for (i = 0; i < equipSkillNum; i++) {
		if (m_skills.find(pHost->GetEquipSkillByIndex(i)->GetSkillID()) == m_skills.end()) {
			return true;
		}
	}
	return false;
}

void CDlgSkillSubPool::TickSkillIcon(AUIImagePicture* pImg, CECSkill* pSkill) {
	if (!pImg || !pSkill) {
		return;
	}
	CECHostPlayer* pHost = CECUIHelper::GetHostPlayer();
	AUIClockIcon* pClock = pImg->GetClockIcon();
	if( pSkill && pSkill->ReadyToCast() && pHost->GetPrepSkill() != pSkill ) {
		if( !pHost->CheckSkillCastCondition(pSkill) ) {
			pImg->SetColor(A3DCOLORRGB(255, 255, 255));
		} else {
			pImg->SetColor(A3DCOLORRGB(128, 128, 128));
		}
	} else {
		pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
	}
	
	
	if( pSkill && (pSkill->GetCoolingTime() > 0 || pHost->GetPrepSkill() == pSkill )) {
		pClock->SetProgressRange(0, pSkill->GetCoolingTime());
		if( pHost->GetPrepSkill() == pSkill ) {
			pClock->SetProgressPos(0);
		} else {
			pClock->SetProgressPos(pSkill->GetCoolingTime() - pSkill->GetCoolingCnt());
		}
	}
}
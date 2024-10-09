#include "DlgSkillSubOther.h"
#include "DlgSkillEdit.h"

#include "EC_GameUIMan.h"
#include "EC_Configs.h"
#include "EC_Game.h"
#include "EC_Skill.h"
#include "EC_HostSkillModel.h"
#include "EC_HostPlayer.h"

#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIClockIcon.h"
#include "AFI.h"
#include "ABaseDef.h"

AUI_BEGIN_EVENT_MAP(CDlgSkillSubOther, CDlgBase)

AUI_ON_EVENT("Img_ConSkill*",	WM_LBUTTONDOWN,	OnEventLButtonDownCombo)
AUI_ON_EVENT("Img_InSkill*",	WM_LBUTTONDOWN,	OnEventLButtonDownFixed)
AUI_ON_EVENT("Img_ItemSkill*",	WM_LBUTTONDOWN,	OnEventLButtonDownItem)

AUI_END_EVENT_MAP()

AUI_BEGIN_COMMAND_MAP(CDlgSkillSubOther, CDlgBase)

AUI_ON_COMMAND("Btn_ConEdi",		OnCommandEdit)
AUI_ON_COMMAND("Btn_ConDel",		OnCommandDelete)
AUI_ON_COMMAND("Btn_ConNew",		OnCommandNew)

AUI_END_COMMAND_MAP()

const int ITEM_SKILL_MAX_COUNT = 8;
const int FIXED_SKILL_MAX_COUNT = 4;

CDlgSkillSubOther::CDlgSkillSubOther() {
	m_nComboSelect = 0;

	// 167为回城术
	m_fixedSkills.push_back(167);

	// 分别为：铁匠精通，裁缝精通，巧匠精通，药师精通
	m_produceSkills.push_back(158);
	m_produceSkills.push_back(159);
	m_produceSkills.push_back(160);
	m_produceSkills.push_back(161);
}

void CDlgSkillSubOther::OnCommandEdit(const char * szCommand) {
	GetGameUIMan()->m_pDlgSkillEdit->SetData(m_nComboSelect);
	GetGameUIMan()->m_pDlgSkillEdit->Show(true);
}

void CDlgSkillSubOther::OnCommandNew(const char * szCommand) {
	GetGameUIMan()->m_pDlgSkillEdit->SetData(0);
	GetGameUIMan()->m_pDlgSkillEdit->Show(true);
}

void CDlgSkillSubOther::OnCommandDelete(const char * szCommand) {
	if( m_nComboSelect < 0 || m_nComboSelect > EC_COMBOSKILL_NUM )
		return;
	
	EC_VIDEO_SETTING setting = GetGame()->GetConfigs()->GetVideoSettings();
	setting.comboSkill[m_nComboSelect - 1].nIcon = 0;
	m_nComboSelect = 0;
	GetGame()->GetConfigs()->SetVideoSettings(setting);
	UpdateComboSkill();
}

// 抄自DlgSkill.cpp
void CDlgSkillSubOther::UpdateComboSkill() {
	EC_VIDEO_SETTING setting = GetGame()->GetConfigs()->GetVideoSettings();
	for(int i = 0; i< EC_COMBOSKILL_NUM; i++) 
	{
		AString strName;
		strName.Format("Img_ConSkill%02d", i + 1);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( pImage )
		{
			if( setting.comboSkill[i].nIcon != 0 )
			{
				pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], 
					setting.comboSkill[i].nIcon + 1);
				pImage->SetData(i + 1);
				pImage->SetDataPtr((void*)1);
				ACString strText;
				strText.Format(GetStringFromTable(804), i);
				pImage->SetHint(strText);
			}
			else
			{
				pImage->SetCover(NULL, -1);
				pImage->SetData(0);
				pImage->SetDataPtr(NULL);
				pImage->SetHint(_AL(""));
			}
		}
	}
}

void CDlgSkillSubOther::UpdateFixedSkill() {

	// 先将四个固定技能栏清空
	AString imgName, txtName;
	size_t i;
	for (i = 0; i < FIXED_SKILL_MAX_COUNT; i++) {
		m_fixedImgPics[i]->Show(false);
		m_fixedTxts[i]->Show(false);
	}

	ASSERT(m_fixedSkills.size() <= FIXED_SKILL_MAX_COUNT);

	CECHostSkillModel& model = CECHostSkillModel::Instance();
	for (i = 0; i != m_fixedSkills.size(); i++) {
		m_fixedImgPics[i]->Show(true);
		m_fixedTxts[i]->Show(true);
		CECSkill* pSkill = GetHostPlayer()->GetPositiveSkillByID(m_fixedSkills[i]);
		if (pSkill) {
			SetImage(m_fixedImgPics[i], pSkill);
			m_fixedTxts[i]->SetText(pSkill->GetNameDisplay()); 
		} else {
			m_fixedImgPics[i]->Show(false);
			m_fixedTxts[i]->Show(false);
		}
	}
}

void CDlgSkillSubOther::UpdateItemSkill() {
	int i;
	for (i = 0; i < ITEM_SKILL_MAX_COUNT; i++) {
		AString str;
		str.Format("Img_ItemSkill%02d", i + 1);
		AUIImagePicture* pImgPic = dynamic_cast<AUIImagePicture*>(GetDlgItem(str));
		if (i < GetHostPlayer()->GetEquipSkillNum()) {
			CECSkill *pSkill = GetHostPlayer()->GetEquipSkillByIndex(i);
			SetImage(pImgPic, pSkill);
		} else {
			SetImage(pImgPic, NULL);
		}
	}
}

void CDlgSkillSubOther::UpdateProduceSkill() {
	// 图标、名称、熟练度文字、熟练度数值、级别
	AString imgPicStr, nameLblStr, skilledLblStr, skilledExpLblStr, levelLblStr;
	AUIImagePicture *imgIcon;
	AUILabel *lblName, *lblSkilledTxt, *lblSkilledExp, *lblLevel;
	for (size_t i = 0; i < m_produceSkills.size(); i++) {
		imgPicStr.Format("Img_ProSkill0%d", i + 1);
		nameLblStr.Format("Txt_PSName0%d", i + 1);
		skilledLblStr.Format("Lbl_ProExp0%d", i + 1);
		skilledExpLblStr.Format("Txt_PSExpNum0%d", i + 1);
		levelLblStr.Format("Txt_PSLevel0%d", i + 1);

		imgIcon			= dynamic_cast<AUIImagePicture*>(GetDlgItem(imgPicStr));
		lblName			= dynamic_cast<AUILabel*>(GetDlgItem(nameLblStr));
		lblSkilledTxt	= dynamic_cast<AUILabel*>(GetDlgItem(skilledLblStr));
		lblSkilledExp	= dynamic_cast<AUILabel*>(GetDlgItem(skilledExpLblStr));
		lblLevel		= dynamic_cast<AUILabel*>(GetDlgItem(levelLblStr));

		CECSkill* pSkill = GetHostPlayer()->GetPassiveSkillByID(m_produceSkills[i]);
		imgIcon->Show(true);
		lblName->Show(true);
		if (!pSkill) {
			lblSkilledTxt->Show(false);
			lblSkilledExp->Show(false);
			lblLevel->Show(false);
			imgIcon->SetColor(A3DCOLORRGB(128, 128, 128));
			CECSkill tmpSkill(m_produceSkills[i], 1);
			SetImage(imgIcon, &tmpSkill);
			lblName->SetText(tmpSkill.GetNameDisplay());
		} else {
			lblSkilledTxt->Show(true);
			lblSkilledExp->Show(true);
			lblLevel->Show(true);
			imgIcon->SetColor(A3DCOLORRGB(255, 255, 255));
			SetImage(imgIcon, pSkill);
			lblName->SetText(pSkill->GetNameDisplay());
			
			
			int maxAbility = GNET::ElementSkill::GetMaxAbility(m_produceSkills[i], pSkill->GetSkillLevel());
			int ability = GNET::ElementSkill::GetAbility(m_produceSkills[i]);
			

			lblSkilledExp->SetText(ACString().Format(_AL("%d/%d"), ability, maxAbility));
			lblLevel->SetText(ACString().Format(GetStringFromTable(11323), pSkill->GetSkillLevel()));
		}	
	}
}

void CDlgSkillSubOther::OnShowDialog() {
	CDlgBase::OnShowDialog();
	UpdateComboSkill();
	UpdateFixedSkill();
	UpdateItemSkill();
	UpdateProduceSkill();
}

bool CDlgSkillSubOther::OnInitDialog() {
	if (!CDlgBase::OnInitDialog()) {
		return false;
	}

	// 固有技能
	AString imgName, txtName;
	for (int i = 0; i < 4; i++) {
		imgName.Format("Img_InSkill0%d", i + 1);
		txtName.Format("Txt_InSkill0%d", i + 1);
		m_fixedImgPics.push_back(NULL);
		m_fixedTxts.push_back(NULL);
		DDX_Control(imgName, m_fixedImgPics[i]);
		DDX_Control(txtName, m_fixedTxts[i]);
	}

	return true;
}

// 抄自DlgSkill.cpp
void CDlgSkillSubOther::OnEventLButtonDownCombo(WPARAM wParam, LPARAM lParam, AUIObject * pObj) {
	if( pObj->GetData() == 0 )
		return;
	
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	POINT pt = 
	{
		GET_X_LPARAM(lParam) - p->X,
		GET_Y_LPARAM(lParam) - p->Y,
	};
	
	GetGameUIMan()->m_ptLButtonDown = pt;
	GetGameUIMan()->InvokeDragDrop(this, pObj, pt);
}

void CDlgSkillSubOther::OnEventLButtonDownFixed(WPARAM wParam, LPARAM lParam, AUIObject * pObj) {
	if (pObj->GetDataPtr("ptr_CECSkill") == 0) {
		return;
	}
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}

void CDlgSkillSubOther::OnEventLButtonDownItem(WPARAM wParam, LPARAM lParam, AUIObject * pObj) {
	if (pObj->GetDataPtr("ptr_CECSkill") == 0) {
		return;
	}
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}

// 抄自DlgSkill.cpp
void CDlgSkillSubOther::SelectComboSkill(int n) {
	AString strName;
	if( m_nComboSelect == n )
	{
		strName.Format("Img_ConSkill%02d", n);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( pImage )
			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
		m_nComboSelect = 0;
	}
	else
	{
		if( m_nComboSelect != 0 )
		{
			strName.Format("Img_ConSkill%02d", m_nComboSelect);
			PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
			if( pImage )
				pImage->SetColor(A3DCOLORRGB(255, 255, 255));
		}
		m_nComboSelect = n;
		strName.Format("Img_ConSkill%02d", m_nComboSelect);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( pImage )
			pImage->SetColor(A3DCOLORRGB(160, 160, 160));
	}
}

// 抄自DlgSkill.cpp
void CDlgSkillSubOther::SetImage(AUIImagePicture *pImage, CECSkill *pSkill) {
	ASSERT(pImage);
	
	if (pSkill)
	{
		AString strFile;
		af_GetFileTitle(pSkill->GetIconFile(), strFile);
		strFile.MakeLower();
		pImage->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
		pImage->SetDataPtr(pSkill,"ptr_CECSkill");
		pImage->SetData(0);
		pImage->SetHint(pSkill->GetDesc());
	}
	else
	{
		pImage->SetCover(NULL, -1);
		pImage->SetDataPtr(NULL);
		pImage->SetData(0);
		pImage->SetHint(_AL(""));
	}
}

bool CDlgSkillSubOther::Tick() {
	if (!CDlgBase::Tick()) {
		return false;
	}

	if (!IsShow()) {
		return true;
	}

	// 物品技能和生产技能有可能会改变，更新之
	UpdateItemSkill();
	UpdateProduceSkill();


	size_t i;
	// 固有技能使用后更新CD
	for (i = 0; i < m_fixedSkills.size(); i++) {
		CECSkill* pSkill = (CECSkill*)m_fixedImgPics[i]->GetDataPtr("ptr_CECSkill");
		UpdateImagePictureCD(m_fixedImgPics[i], pSkill);
	}

	// 物品技能使用后更新CD
	for (i = 0; i < GetHostPlayer()->GetEquipSkillNum() && i < ITEM_SKILL_MAX_COUNT; i++) {
		AUIImagePicture* pImgPic = (AUIImagePicture*)GetDlgItem(AString().Format("Img_ItemSkill%02d", i + 1));
		CECSkill* pSkill = (CECSkill*)pImgPic->GetDataPtr("ptr_CECSkill");
		UpdateImagePictureCD(pImgPic, pSkill);
	}
	return true;
}

void CDlgSkillSubOther::UpdateImagePictureCD(AUIImagePicture* pImgPic, CECSkill* pSkill) {
	CECHostPlayer* pHost = GetHostPlayer();
	AUIClockIcon* pClock = pImgPic->GetClockIcon();
	if( pSkill && pSkill->ReadyToCast() && pHost->GetPrepSkill() != pSkill )
	{
		
		if( !pHost->CheckSkillCastCondition(pSkill) )
			pImgPic->SetColor(A3DCOLORRGB(255, 255, 255));
		else
			pImgPic->SetColor(A3DCOLORRGB(128, 128, 128));
		
	}
	else
		pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
	
	
	if( pSkill && (pSkill->GetCoolingTime() > 0 ||
		pHost->GetPrepSkill() == pSkill ))
	{
		pClock->SetProgressRange(0, pSkill->GetCoolingTime());
		if( pHost->GetPrepSkill() == pSkill )
			pClock->SetProgressPos(0);
		else
			pClock->SetProgressPos(pSkill->GetCoolingTime() - pSkill->GetCoolingCnt());
	}
}
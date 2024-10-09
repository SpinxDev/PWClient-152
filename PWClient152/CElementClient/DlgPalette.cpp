// Filename	: DlgPalette.cpp
// Creator	: Xu Wenbin
// Date		: 2010/1/14

#include "AFI.h"
#include "DlgPalette.h"
#include "DlgCustomizePaint.h"
#include "DlgFittingRoom.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrFashion.h"
#include "EC_Viewport.h"
#include "EC_Global.h"

#include "AUIRadioButton.h"

#include <A2DSprite.h>
#include <A3DSurface.h>
#include <A3DSurfaceMan.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPalette, CDlgPaletteFashion)
AUI_ON_COMMAND("restore",	OnCommand_ResetColor)
AUI_END_COMMAND_MAP()

CDlgPalette::CDlgPalette() : CDlgPaletteFashion("surfaces\\图标\\个性化\\调色板.tga") {
	m_pBtnRestore = NULL;
}

bool CDlgPalette::OnInitDialog() {
	if (!CDlgPaletteFashion::OnInitDialog()) {
		return false;
	}
	DDX_Control("Btn_Restore", m_pBtnRestore);
	return true;
}

void CDlgPalette::OnShowDialog() {
	CDlgPaletteFashion::OnShowDialog();

	// 更新按钮的启用状态
	CDlgFittingRoom *pFittingRoom = GetGameUIMan()->m_pDlgFittingRoom;
	for (int i(0); i < CHANGE_ALL; i++) {
		m_pRdos[i]->Enable(GetEquipEnable(static_cast<enumChangeTarget>(i)));
	}
	m_pRdos[CHANGE_ALL]->Enable(GetEnabledTargetCount() > 0);

	// 记录时装原本的颜色
	SaveOriginColor();

	//	计算要显示的目标
	ChangeTarget(CHANGE_NUM);	//	先设置为非法值 CHANGE_NUM，以达到更新界面的目的
	if (m_pRdos[CHANGE_ALL]->IsEnabled()) {
		ChangeTarget(CHANGE_ALL);
		m_pBtnRestore->Enable(true);
	} else {
		m_pBtnRestore->Enable(false);
	}
}

void CDlgPalette::OnTick()
{
	bool cancel(true);
	
	while (true)
	{
		// 检查是否为可调整服装
		CDlgFittingRoom *pFittingRoom = GetGameUIMan()->m_pDlgFittingRoom;
		if (!pFittingRoom->IsShow())
			break;

		// 底部与试衣间对齐
		A3DVIEWPORTPARAM* param = GetGame()->GetViewport()->GetA3DViewport()->GetParam();
		int nAlignX = AUIDIALOG_ALIGN_RIGHT;
		if (pFittingRoom->GetPos().x+pFittingRoom->GetSize().cx+this->GetSize().cx > (int)(param->X+param->Width))
			nAlignX = AUIDIALOG_ALIGN_LEFT;
		this->AlignTo(pFittingRoom, AUIDIALOG_ALIGN_OUTSIDE, nAlignX, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);
		
		cancel = false;
		break;
	}
	
	if (cancel)
		OnCommand_CANCEL("");
}

void CDlgPalette::OnSetColor(const A3DCOLOR& color) {
	CDlgFittingRoom* dlgFittingRoom = GetGameUIMan()->m_pDlgFittingRoom;
	if (m_changeTarget == CHANGE_ALL || m_changeTarget == CHANGE_LOWER) {
		dlgFittingRoom->SetEquipColor(dlgFittingRoom->GetDlgItem("Equip_14"), color);
	}
	if (m_changeTarget == CHANGE_ALL || m_changeTarget == CHANGE_UPPER) {
		dlgFittingRoom->SetEquipColor(dlgFittingRoom->GetDlgItem("Equip_13"), color);
	}
	if (m_changeTarget == CHANGE_ALL || m_changeTarget == CHANGE_WAIST) {
		dlgFittingRoom->SetEquipColor(dlgFittingRoom->GetDlgItem("Equip_16"), color);
	}
	if (m_changeTarget == CHANGE_ALL || m_changeTarget == CHANGE_SHOES) {
		dlgFittingRoom->SetEquipColor(dlgFittingRoom->GetDlgItem("Equip_15"), color);
	}
}

bool CDlgPalette::GetEquipEnable(enumChangeTarget target) {
	AString objName;
	if (target == CHANGE_LOWER) {
		objName = "Equip_14";
	} else if (target == CHANGE_UPPER) {
		objName = "Equip_13";
	} else if (target == CHANGE_WAIST) {
		objName = "Equip_16";
	} else if (target == CHANGE_SHOES) {
		objName = "Equip_15";
	} else {
		ASSERT(false);
		return false;
	}
	A3DCOLOR clr;
	AUIObject* equipObj = GetGameUIMan()->m_pDlgFittingRoom->GetDlgItem(objName);
	return GetGameUIMan()->m_pDlgFittingRoom->GetEquipColor(equipObj, clr);
}

void CDlgPalette::SaveOriginColor() {
	// 记录所有时装原有的颜色
	CDlgFittingRoom* dlgFittingRoom = GetGameUIMan()->m_pDlgFittingRoom;
	A3DCOLOR color;
	
	dlgFittingRoom->GetEquipColor(dlgFittingRoom->GetDlgItem("Equip_13"), color);
	m_originColor[CHANGE_UPPER] = color;
	
	dlgFittingRoom->GetEquipColor(dlgFittingRoom->GetDlgItem("Equip_14"), color);
	m_originColor[CHANGE_LOWER] = color;
	
	dlgFittingRoom->GetEquipColor(dlgFittingRoom->GetDlgItem("Equip_15"), color);
	m_originColor[CHANGE_SHOES] = color;
	
	dlgFittingRoom->GetEquipColor(dlgFittingRoom->GetDlgItem("Equip_16"), color);
	m_originColor[CHANGE_WAIST] = color;
}

void CDlgPalette::OnCommand_ResetColor(const char * szCommand) {
	CDlgFittingRoom* dlgFittingRoom = GetGameUIMan()->m_pDlgFittingRoom;
	if (m_changeTarget == CHANGE_ALL || m_changeTarget == CHANGE_UPPER) {
		dlgFittingRoom->SetEquipColor(dlgFittingRoom->GetDlgItem("Equip_13"), m_originColor[CHANGE_UPPER]);
	}
	if (m_changeTarget == CHANGE_ALL || m_changeTarget == CHANGE_LOWER) {
		dlgFittingRoom->SetEquipColor(dlgFittingRoom->GetDlgItem("Equip_14"), m_originColor[CHANGE_LOWER]);
	}
	if (m_changeTarget == CHANGE_ALL || m_changeTarget == CHANGE_SHOES) {
		dlgFittingRoom->SetEquipColor(dlgFittingRoom->GetDlgItem("Equip_15"), m_originColor[CHANGE_SHOES]);
	}
	if (m_changeTarget == CHANGE_ALL || m_changeTarget == CHANGE_WAIST) {
		dlgFittingRoom->SetEquipColor(dlgFittingRoom->GetDlgItem("Equip_16"), m_originColor[CHANGE_WAIST]);
	}
}
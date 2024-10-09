// Filename	: DlgPalette.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/29

#include "DlgPalette2.h"
#include "DlgCustomizePaint.h"
#include "DlgFashionShop.h"

#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrFashion.h"
#include "EC_Viewport.h"
#include "EC_Global.h"

#include <AUIStillImageButton.h>
#include <AUIRadioButton.h>

#include <AFI.h>
#include <A2DSprite.h>
#include <A3DSurface.h>
#include <A3DSurfaceMan.h>

AUI_BEGIN_COMMAND_MAP(CDlgPalette2, CDlgPaletteFashion)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_BestColor",	OnCommand_BestColor)
AUI_END_COMMAND_MAP()


CDlgPalette2::CDlgPalette2()
: CDlgPaletteFashion("surfaces\\ͼ��\\���Ի�\\��ɫ��.tga"), m_pBtn_BestColor(NULL)
{
	for (int i(0); i < CHANGE_ALL; ++ i){
		m_bestColor[i] = 0;
		m_hasBestColor[i] = false;
	}
}
 
bool CDlgPalette2::OnInitDialog(){
	if (CDlgPaletteFashion::OnInitDialog()){
		DDX_Control("Btn_BestColor",	m_pBtn_BestColor);
		return true;
	}
	return false;
}

void CDlgPalette2::OnShowDialog(){
	CDlgPaletteFashion::OnShowDialog();

	//	���°�ť������״̬
	CDlgFashionShop *pDlgFashionShop = GetGameUIMan()->m_pDlgFashionShop;
	for (int i(0); i < CHANGE_ALL; ++ i){
		int equipSlot = GetEquipSlotForTarget(i);
		m_hasBestColor[i] = pDlgFashionShop->GetFashionBestColor(equipSlot, m_bestColor[i]);
		unsigned short color(0);
		m_pRdos[i]->Enable(pDlgFashionShop->GetFashionColor(equipSlot, color));
	}
	m_pRdos[CHANGE_ALL]->Enable(GetEnabledTargetCount() > 1);

	//	����Ҫ��ʾ��Ŀ��
	ChangeTarget(CHANGE_NUM);	//	������Ϊ�Ƿ�ֵ CHANGE_NUM���Դﵽ���½����Ŀ��
	if (m_pRdos[CHANGE_ALL]->IsEnabled()){
		ChangeTarget(CHANGE_ALL);
	}else{
		for (int j(0); j < CHANGE_ALL; ++ j){
			if (m_pRdos[j]->IsEnabled()){
				ChangeTarget(static_cast<enumChangeTarget>(j));
				break;
			}
		}
	}

	// �����¼��ڲ��ؼ�����
	PAUIOBJECT pObjPalette = pDlgFashionShop->GetDlgItem("Btn_Palette");
	if (pObjPalette){			
		A3DRECT rect = pObjPalette->GetRect();
		SetPosEx(rect.right+10, rect.bottom - GetRect().Height());
	}
}

int CDlgPalette2::GetEquipSlotForTarget(int changeTarget){
	int equipSlots[CHANGE_ALL] = {EQUIPIVTR_FASHION_BODY, EQUIPIVTR_FASHION_LEG, EQUIPIVTR_FASHION_WRIST, EQUIPIVTR_FASHION_FOOT};
	if (changeTarget >= 0 && changeTarget < CHANGE_ALL){
		return equipSlots[changeTarget];
	}
	return -1;
}

void CDlgPalette2::OnTick(){
	bool shouldHide(true);
	while (true){
		// ����Ƿ�Ϊ�ɵ�����װ
		CDlgFashionShop *pDlgFashionShop = GetGameUIMan()->m_pDlgFashionShop;
		if (!pDlgFashionShop->IsShow()){
			break;
		}		
		// �����¼��ڲ��ؼ�����
		PAUIOBJECT pObjPalette = pDlgFashionShop->GetDlgItem("Btn_Palette");//	�ײ��� Btn_Palette ���룬����� Img_Char �Ҳ����
		PAUIOBJECT pObjChar = pDlgFashionShop->GetDlgItem("Img_Char");		//	����� Img_Char �Ҳ����
		if (pObjPalette && pObjChar){			
			SetPosEx(pObjChar->GetRect().right, pObjPalette->GetRect().bottom - GetRect().Height());
		}
		//	��ǰ�Ծ���Ч
		shouldHide = false;
		break;
	}
	if (shouldHide){
		OnCommand("IDCANCEL");
	}
}

void CDlgPalette2::OnCommand_CANCEL(const char * szCommand){
	Show(false);
}

void CDlgPalette2::OnCommand_BestColor(const char * szCommand){
	CDlgFashionShop *pDlgFashionShop = GetGameUIMan()->m_pDlgFashionShop;
	if (m_changeTarget >= 0 && m_changeTarget < CHANGE_ALL){
		pDlgFashionShop->ChangeFashionColor(GetEquipSlotForTarget(m_changeTarget), m_bestColor[m_changeTarget]);
	}else if (m_changeTarget == CHANGE_ALL){
		for (int i(0); i < CHANGE_ALL; ++ i){
			pDlgFashionShop->ChangeFashionColor(GetEquipSlotForTarget(i), m_bestColor[i]);
		}
	}
}

void CDlgPalette2::OnChangeTarget(enumChangeTarget) {
	UpdateBestColorButton();
}

void CDlgPalette2::UpdateBestColorButton(){
	bool hasBestColor(false);
	if (m_changeTarget >= 0 && m_changeTarget < CHANGE_ALL){
		hasBestColor = m_hasBestColor[m_changeTarget];
	}else{
		for (int i(0); i < CHANGE_ALL; ++ i){
			if (m_hasBestColor[i]){
				hasBestColor = true;
				break;
			}
		}
	}
	m_pBtn_BestColor->Enable(hasBestColor);
}

void CDlgPalette2::OnSetColor(const A3DCOLOR& newColor){
	unsigned short color = (unsigned short)FASHION_A3DCOLOR_TO_WORDCOLOR(newColor);
	CDlgFashionShop *pDlgFashionShop = GetGameUIMan()->m_pDlgFashionShop;
	if (m_changeTarget >= 0 && m_changeTarget < CHANGE_ALL){
		pDlgFashionShop->ChangeFashionColor(GetEquipSlotForTarget(m_changeTarget), color);
	}else if (m_changeTarget == CHANGE_ALL){
		for (int i(0); i < CHANGE_ALL; ++ i){
			pDlgFashionShop->ChangeFashionColor(GetEquipSlotForTarget(i), color);
		}
	}
}

int CDlgPalette2::GetEnabledTargetCount()const{
	int result(0);
	for (int i(0); i < CHANGE_ALL; ++ i){
		if (m_pRdos[i]->IsEnabled()){
			++ result;
		}
	}
	return result;
}
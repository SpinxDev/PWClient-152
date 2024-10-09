// Filename	: DlgRedSpark.h
// Creator	: zhangyitian
// Date		: 2014/8/6

#include "DlgRedSpark.h"

#include "EC_Game.h"
#include "EC_Viewport.h"

#include "A3DViewport.h"
#include "AUIImagePicture.h"
#include "A3DGFXEx.h"

const int DlgInitWidth = 1024;
const int DlgInitHeight = 768;

CDlgRedSpark::CDlgRedSpark() {
	m_pImgLeft		= NULL;
	m_pImgRight		= NULL;
	m_pImgTop		= NULL;
	m_pImgBottom	= NULL;
}

bool CDlgRedSpark::OnInitDialog() {
	DDX_Control("Img_Left",		m_pImgLeft);
	DDX_Control("Img_Right",	m_pImgRight);
	DDX_Control("Img_Top",		m_pImgTop);
	DDX_Control("Img_Bottom",	m_pImgBottom);
	SetEnable(false);
	return true;
}

void CDlgRedSpark::OnShowDialog() {
	SetView();
}

void CDlgRedSpark::OnHideDialog() {

}

void CDlgRedSpark::Resize(A3DRECT rcOld, A3DRECT rcNew) {
	SetView();
}

void CDlgRedSpark::OnChangeLayoutEnd(bool bAllDone) {
	SetEnable(false);
	SetView();
}

void CDlgRedSpark::SetView() {
	int viewportWidth = GetGame()->GetViewport()->GetA3DViewport()->GetParam()->Width;
	int viewportHeight = GetGame()->GetViewport()->GetA3DViewport()->GetParam()->Height;
	SetPosEx(0, 0);
	SetSize(viewportWidth, viewportHeight);

	m_pImgLeft->SetPos(0, viewportHeight / 2 - m_pImgLeft->GetSize().cy / 2);
	m_pImgRight->SetPos(viewportWidth - m_pImgRight->GetSize().cx, viewportHeight / 2 - m_pImgLeft->GetSize().cy / 2);
	m_pImgTop->SetPos(viewportWidth / 2 - m_pImgTop->GetSize().cx / 2, 0);
	m_pImgBottom->SetPos(viewportWidth / 2 - m_pImgTop->GetSize().cx / 2, viewportHeight - m_pImgTop->GetSize().cy);

	m_pImgLeft->GetGfx()->SetScale((float)viewportHeight / DlgInitHeight);
	m_pImgRight->GetGfx()->SetScale((float)viewportHeight / DlgInitHeight);
	m_pImgTop->GetGfx()->SetScale((float)viewportWidth / DlgInitWidth);
	m_pImgBottom->GetGfx()->SetScale((float)viewportWidth / DlgInitWidth);
}
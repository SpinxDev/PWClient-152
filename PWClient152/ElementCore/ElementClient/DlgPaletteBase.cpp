// Filename	: DlgPaletteBase.cpp
// Creator	: zhangyitian
// Date		: 2014/7/29

#include "DlgPaletteBase.h"
#include "DlgCustomizePaint.h"

#include "EC_BaseColor.h"
#include "EC_Game.h"
#include "EC_Viewport.h" 
#include "EC_Global.h"
#include "EC_GameUIMan.h"

#include <AFI.h>
#include <A2DSprite.h>
#include <A3DSurface.h>
#include <A3DTypes.h>
#include <A3DViewport.h>
#include <A3DSurfaceMan.h>
#include <AUIImagePicture.h>
#include <AUIRadioButton.h>

AUI_BEGIN_EVENT_MAP(CDlgPaletteBase, CDlgBase)
AUI_ON_EVENT("ImagePicture_ColorPic", WM_LBUTTONDOWN, OnLButtonDownPicPallete)
AUI_ON_EVENT("ImagePicture_ColorPic", WM_MOUSEMOVE,   OnMouseMovePicPallete)
AUI_ON_EVENT("Rdo_*", WM_LBUTTONDOWN, OnLButtonDownBaseColor)
AUI_END_EVENT_MAP()

CDlgPaletteBase::CDlgPaletteBase(const AString& strImgFile, bool bChangeColorOnMove /* = true */) 
: m_strImgFile(strImgFile), m_bChangeColorOnMove(bChangeColorOnMove), m_pPicPane(NULL), m_pPicIndicator(NULL) {

}

bool CDlgPaletteBase::OnInitDialog() {
	if (!CDlgBase::OnInitDialog()) {
		return false;
	}

	DDX_Control("ImagePicture_ColorPic", m_pPicPane);
	DDX_Control("Pic_ColorPos",			 m_pPicIndicator);

	m_pPicPane->SetAcceptMouseMessage(true);

	A3DSurface* pSurface = NULL;
	A2DSprite* pSprite = NULL;

	bool bSuccess(false);
	while (true){
		if (!af_IsFileExist(m_strImgFile)){
			a_LogOutput(1, "CDlgPaletteBase::OnInitDialog, 打开调色板图片文件 %s 失败！需要此图片才能正常调色", m_strImgFile);
			break;
		}			
		// 创建查询颜色的 surface
		A3DSurfaceMan* pA3DSurfaceMan = GetGame()->GetA3DEngine ()->GetA3DSurfaceMan();
		m_strImgFile = (const char*)m_strImgFile + strlen("surfaces\\");  
		if (!pA3DSurfaceMan->LoadCursorSurfaceFromFile(m_strImgFile, 0, &pSurface)){
			break;
		}			
		// 创建显示的图片
		pSprite = new A2DSprite;
		A3DDevice* pDevice = GetGame()->GetA3DDevice();
		if (!pSprite->Init(pDevice, m_strImgFile, 0)){
			A3DRELEASE(pSprite);
			break;
		}
		bSuccess = true;
		break;
	}
	if (bSuccess) {
		m_pPallete = new CPaletteControl(m_pPicPane, m_pPicIndicator, NULL, this);
		m_pPallete->ResetContext(pSurface, pSprite);
		m_pA2DSpriteColorPic = pSprite;
	} else {
		if (pSurface) {
			A3DSurfaceMan* pA3DSurfaceMan = GetGame()->GetA3DEngine()->GetA3DSurfaceMan();
			pA3DSurfaceMan->ReleaseSurface(pSurface);
		}
		A3DRELEASE(pSprite);
	}
	return true;
}

bool CDlgPaletteBase::Release() {
	A3DRELEASE(m_pA2DSpriteColorPic);
	delete m_pPallete;
	m_pPallete = NULL;
	return CDlgBase::Release();
}

void CDlgPaletteBase::OnLButtonDownPicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj) {
	if (!(wParam & MK_LBUTTON) || !m_pPallete){
		return;
	}
	A3DVIEWPORTPARAM* param = GetGame()->GetViewport()->GetA3DViewport()->GetParam();
	POINT pt = {
		GET_X_LPARAM(lParam) - m_pPicPane->GetPos().x - param->X,
		GET_Y_LPARAM(lParam) - m_pPicPane->GetPos().y - param->Y
	};	
	m_pPallete->MoveIndicator(pt);
	
	OnSetColor(m_pPallete->GetColor());
}

void CDlgPaletteBase::OnMouseMovePicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj) {
	if (!m_bChangeColorOnMove) {
		return;
	}
	OnLButtonDownPicPallete(wParam, lParam, pObj);
}

void CDlgPaletteBase::OnLButtonDownBaseColor(WPARAM wParam, LPARAM lParam, AUIObject * pObj) {
	ACString strHint = pObj->GetHint();
	const CECBaseColor::BaseColor* baseColor = CECBaseColor::Instance().GetBaseColorFromName(strHint);
	if (baseColor) {
		m_pPallete->SetColor(baseColor->m_color);
		OnSetColor(m_pPallete->GetColor());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgPaletteFashion, CDlgPaletteBase)
AUI_ON_COMMAND("Rdo_Suite",		OnCommand_Suite)
AUI_ON_COMMAND("Rdo_UpperBody",	OnCommand_UpperBody)
AUI_ON_COMMAND("Rdo_LowerBody",	OnCommand_LowerBody)
AUI_ON_COMMAND("Rdo_Waist",		OnCommand_Waist)
AUI_ON_COMMAND("Rdo_Shoes",		OnCommand_Shoes)
AUI_END_COMMAND_MAP()

CDlgPaletteFashion::CDlgPaletteFashion(const AString& strImgFile, bool bChangeColorOnMove /* = true */)
: CDlgPaletteBase(strImgFile, bChangeColorOnMove) {
	for (int i = 0; i < CHANGE_NUM; i++) {
		m_pRdos[i] = NULL;
	}
}

bool CDlgPaletteFashion::OnInitDialog() {
	if (!CDlgPaletteBase::OnInitDialog()) {
		return false;
	}
	DDX_Control("Rdo_UpperBody",	m_pRdos[CHANGE_UPPER]);
	DDX_Control("Rdo_LowerBody",	m_pRdos[CHANGE_LOWER]);
	DDX_Control("Rdo_Waist",		m_pRdos[CHANGE_WAIST]);
	DDX_Control("Rdo_Shoes",		m_pRdos[CHANGE_SHOES]);
	DDX_Control("Rdo_Suite",		m_pRdos[CHANGE_ALL]);

	return true;
}

void CDlgPaletteFashion::OnCommand_Suite(const char * szCommand) {
	ChangeTarget(CHANGE_ALL);
}

void CDlgPaletteFashion::OnCommand_UpperBody(const char * szCommand) {
	ChangeTarget(CHANGE_UPPER);
}

void CDlgPaletteFashion::OnCommand_LowerBody(const char * szCommand) {
	ChangeTarget(CHANGE_LOWER);
}

void CDlgPaletteFashion::OnCommand_Waist(const char * szCommand) {
	ChangeTarget(CHANGE_WAIST);
}

void CDlgPaletteFashion::OnCommand_Shoes(const char * szCommand) {
	ChangeTarget(CHANGE_SHOES);
}

void CDlgPaletteFashion::ChangeTarget(enumChangeTarget newTarget) {
	if (newTarget == m_changeTarget){
		return;
	}
	m_changeTarget = newTarget;
	for (int i = 0; i < CHANGE_NUM; i++){
		m_pRdos[i]->Check(i == newTarget);
	}
	OnChangeTarget();
}

int CDlgPaletteFashion::GetEnabledTargetCount() {
	int iTargetCount = 0;
	for (int i = 0; i < CHANGE_ALL; i++) {
		if (m_pRdos[i]->IsEnabled()) {
			iTargetCount++;
		}
	}
	return iTargetCount;
}
/*
 * FILE: DlgAutoChooseColor.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoChooseColor.h"
#include "DlgAutoLight.h"
#include "DlgAutoChooseLight.h"
#include "aui/AUIImagePicture.h"

#include <A3DSurface.h>
#include <A3DSurfaceMan.h>
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "A3DMacros.h"
#include "EC_Viewport.h"
#include "A3DViewport.h"

#include "WindowsX.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoChooseColor, CDlgHomeBase)

AUI_ON_COMMAND("OK", OnCommandOK)
AUI_ON_COMMAND("Cancel", OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoChooseColor, CDlgHomeBase)

AUI_ON_EVENT("ImagePic_ColorPic", WM_MOUSEMOVE, OnMouseMoveColorPic)
AUI_ON_EVENT("ImagePic_ColorPic", WM_LBUTTONDOWN, OnLButtonDownColorPic)

AUI_END_EVENT_MAP()


CDlgAutoChooseColor::CDlgAutoChooseColor()
{
	m_pImagePicColorPic = NULL;
	m_pImagePicCurColor = NULL;
	m_pImagePicCurPos = NULL;
	m_ColorType = CT_NULL;

	m_pColorCtrl = NULL;

	m_clrInit = 0xffffffff;
}

CDlgAutoChooseColor::~CDlgAutoChooseColor()
{
	if (m_pColorCtrl)
	{
		delete m_pColorCtrl;
		m_pColorCtrl = NULL;
	}
}

bool CDlgAutoChooseColor::OnInitContext()
{
	return true;
}

void CDlgAutoChooseColor::OnShowDialog()
{
	this->SetCanMove(false);
}

void CDlgAutoChooseColor::DoDataExchange(bool bSave)
{
	DDX_Control("ImagePic_ColorPic", m_pImagePicColorPic);
	DDX_Control("ImagePic_CurColor", m_pImagePicCurColor);
	DDX_Control("ImagePic_CurPos", m_pImagePicCurPos);
}

bool CDlgAutoChooseColor::OnInitDialog()
{
	A3DSurfaceMan* pA3DSurfaceMan = g_pGame->GetA3DEngine ()->GetA3DSurfaceMan ();
				
	if( !pA3DSurfaceMan)
	{
		AutoHomeAssert();
		a_LogOutput(1, "CDlgAutoChooseColor::OnInitDialog(), Failed to Get A3DSurfaceMan!");
		return false;
	}

	char szPicFileName[MAX_PATH];
	sprintf(szPicFileName, "图标\\个性化\\眼睛.tga");
	
	A3DSurface* pSurface = NULL;//在ColorControl中释放

	if( !pA3DSurfaceMan->LoadCursorSurfaceFromFile ( szPicFileName, 0, &pSurface))
	{
		AutoHomeAssert();
		a_LogOutput(1, "CDlgAutoChooseColor::OnInitDialog(), Failed to Load Surface from file!");
		return false;
	}

	m_pColorCtrl = new CColorControl(m_pImagePicColorPic, m_pImagePicCurPos, m_pImagePicCurColor, this, pSurface);
	
	return true;
}

void CDlgAutoChooseColor::InitColorDlg(ColorType_e ct, A3DCOLOR color)
{
	m_ColorType = ct;
	m_pColorCtrl->SetColor(color);
	m_clrInit = color;
}

void CDlgAutoChooseColor::OnCommandOK(const char* szCommand)
{
	A3DCOLOR clr = m_pColorCtrl->GetColor();
	if(m_ColorType == CT_LIGHT)
	{
		GetHomeDlgsMgr()->GetAutoChooseLightDlg()->SetImagePicColor(clr);
	}
	else
	{
		GetHomeDlgsMgr()->GetAutoLightDlg()->SetImagePicColor(m_ColorType, clr );
	}
	this->Show(false);
}

void CDlgAutoChooseColor::OnCommandCancel(const char* szCommand)
{
	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();
	switch( m_ColorType)
	{
	case CT_DAYFOG:
		pConfig->dwFogColorDay = m_clrInit;
		break;
	case CT_NIGHTFOG:
		pConfig->dwFogColorNight = m_clrInit;
		break;
	case CT_WATERFOG:
		pConfig->dwFogColorWater = m_clrInit;
		break;
	default:
		break;
	}

	this->Show(false);
}

void CDlgAutoChooseColor::OnLButtonDownColorPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	OnMouseMoveColorPic(MK_LBUTTON, lParam, pObj);
}

void CDlgAutoChooseColor::OnMouseMoveColorPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (!(wParam & MK_LBUTTON)) return;

	A3DVIEWPORTPARAM* param = g_pGame->GetViewport()->GetA3DViewport()->GetParam();

	POINT pt = 
	{
		GET_X_LPARAM(lParam) - m_pImagePicColorPic->GetPos().x - param->X,
		GET_Y_LPARAM(lParam) - m_pImagePicColorPic->GetPos().y - param->Y
	};

	m_pColorCtrl->MoveCurPos(pt);

	//Update Fog Color
	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();
	A3DCOLOR clrCur = m_pColorCtrl->GetColor();

	switch( m_ColorType)
	{
	case CT_DAYFOG:
		pConfig->dwFogColorDay = clrCur;
		break;
	case CT_NIGHTFOG:
		pConfig->dwFogColorNight = clrCur;
		break;
	case CT_WATERFOG:
		pConfig->dwFogColorWater = clrCur;
		break;
	default:
		break;
	}

	return;

}

//-------------------------------------------------------------------------
#define DEFAULT_COLOR 0xffffffff

CColorControl::CColorControl(AUIImagePicture* pColorPic, AUIImagePicture* pCurPos, AUIImagePicture* pCurColor, AUIDialog * pDlg, A3DSurface* pSurface):
m_pImagePicColorPic(pColorPic),
m_pImagePicCurPos(pCurPos),
m_pImagePicCurColor(pCurColor),
m_pDlg(pDlg),
m_pSurface(pSurface),
m_Color(DEFAULT_COLOR)
{
}

CColorControl::~CColorControl()
{
	A3DRELEASE(m_pSurface);
}

A3DCOLOR CColorControl::GetColor()
{
	return m_Color;
}

void CColorControl::SetColor(A3DCOLOR color)
{
	m_Color = color;
	POINT pt = GetPosFromColorPic();
	SetCurPos(pt);
	UpdateCurColor();
	
}

//ptCenter为图片上位置
void CColorControl::MoveCurPos(POINT ptCenter)
{
	ClampCurPosToColorPic(ptCenter);

	m_Color = GetColorFromPos(ptCenter);
	SetCurPos(ptCenter);
	UpdateCurColor();
}

//获取颜色在图片上的位置
POINT CColorControl::GetPosFromColorPic()
{
	POINT ptFailed = {0, 0};

	if (m_pSurface->GetFormat() != A3DFMT_A8R8G8B8) return ptFailed;
	
	A3DCOLOR *pData = NULL;
	int nPitch = 0;
	if (!m_pSurface->LockRect(NULL, (void **)&pData, &nPitch, 0))
		return ptFailed;
	for (int j = 0; j < m_pSurface->GetHeight(); j++)
	{
		for (int i = 0; i < m_pSurface->GetWidth(); i++)
		{
			if (pData[j * (nPitch / 4) + i] == m_Color)
			{
				m_pSurface->UnlockRect();
				
				POINT pt = {i, j};
				return pt;
			}
		}
	}

	m_pSurface->UnlockRect();
	return ptFailed;
}

void CColorControl::SetCurPos(POINT ptCenter)
{
	A3DRECT rect = m_pImagePicCurPos->GetRect();

	POINT pt = 
	{
		ptCenter.x - rect.Width() / 2, 
		ptCenter.y - rect.Width() / 2  
	};

	POINT ptSrc = MapToScrPos(pt);
	m_pImagePicCurPos->SetPos(ptSrc.x, ptSrc.y);
	m_pDlg->ChangeFocus(m_pImagePicCurPos);
}

POINT CColorControl::MapToScrPos(POINT pt)
{
	POINT ptColorPic = m_pImagePicColorPic->GetPos(true);
	POINT ptRet;

	ptRet.x = ptColorPic.x + pt.x;
	ptRet.y = ptColorPic.y + pt.y;

	return ptRet;
}

void CColorControl::UpdateCurColor()
{
	m_pImagePicCurColor->SetColor(m_Color);
}

POINT CColorControl::ClampCurPosToColorPic(POINT &pt)
{
	A3DRECT rect = m_pImagePicColorPic->GetRect();
	a_Clamp<long>(pt.x, 0, rect.Width() - 1);
	a_Clamp<long>(pt.y, 0, rect.Height() - 1);
	
	return pt;
}

A3DCOLOR CColorControl::GetColorFromPos(POINT pt)
{
	a_Clamp<long>(pt.x, 0, m_pSurface->GetWidth() - 1);
	a_Clamp<long>(pt.y, 0, m_pSurface->GetHeight() - 1);
	RECT rect = {pt.x, pt.y, pt.x + 1, pt.y + 1};

	int nPicPitch = 0;
	A3DCOLOR *pData = NULL;
	if (!m_pSurface->LockRect(&rect, (void**)&pData, &nPicPitch, 0))
		return DEFAULT_COLOR;
	m_pSurface->UnlockRect();
	
	return *pData;
}
*/
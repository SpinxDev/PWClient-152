/*
 * FILE: DlgCustomizeUserPic.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "DlgCustomizeUserPic.h"
#include "EC_CustomizeMgr.h"

#include "EC_Game.h"
#include "EC_Face.h"
#include "AUIImagePicture.h"
#include "AUIListBox.h"
#include "FaceBoneController.h"
#include "EC_Game.h"
#include "EC_UIManager.h"
#include "EC_GameRun.h"
#include "EC_LoginUIMan.h"
#include "elementdataman.h"

#include "DlgCustomizeFace.h"

#include "A3DViewport.h"
#include "WindowsX.h"

AUI_BEGIN_COMMAND_MAP(CDlgCustomizeUserPic, CDlgCustomizeBase)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
AUI_ON_COMMAND("ZoomOutUserPic", OnCommandZoomOutUserPic)
AUI_ON_COMMAND("ZoomInUserPic", OnCommandZoomInUserPic)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgCustomizeUserPic, CDlgCustomizeBase)

AUI_ON_EVENT("Pic_UserPic", WM_MOUSEWHEEL, OnMouseWheelZoom)
AUI_ON_EVENT("Pic_UserPic", WM_LBUTTONDOWN, OnLButtonDownPicUserPic)
AUI_ON_EVENT("Pic_UserPic", WM_LBUTTONUP, OnLButtonUpPicUserPic)
AUI_ON_EVENT("Pic_UserPic", WM_MOUSEMOVE, OnMouseMovePicUserPic)

AUI_ON_EVENT("Btn_UserPicDrag", WM_LBUTTONDOWN, OnLButtonDownBtnDrag)
AUI_ON_EVENT("Btn_UserPicDrag", WM_LBUTTONUP, OnLButtonUpBtnDrag)
AUI_ON_EVENT("Btn_UserPicDrag", WM_MOUSEMOVE, OnMouseMoveBtnDrag)
AUI_END_EVENT_MAP()

//-------------------------------------------------------------------
CDlgCustomizeUserPic::CDlgCustomizeUserPic()
{
	//ddx control
	m_pPicUserPic = NULL;
	m_pBtnUserPicDrag = NULL;
	m_pBtnUserPicZoomIn = NULL;
	m_pBtnUserPicZoomOut = NULL;

	m_pSliderUserPicAlpha = NULL;
	m_pLabelUserPicAlpha = NULL;

	m_pLabelUserPic = NULL;

	//ddx data
	m_nUserPicAlpha = 0;

	//data
	m_pA2DSpriteUserPic = NULL;
	
	m_fA2DSpriteUserPicScale = 1.0f;
	m_fA2DSpriteUserPicOriScale = 1.0f;

	m_nPicUserPicOriWidth = 0;
	m_nPicUserPicOriHeight = 0;

	m_fPicUserPicScale = 1.0f;

	m_nA2DSPriteUserPicItemPosX = 0;
	m_nA2DSPriteUserPicItemPosY =0;
	//鼠标在item上面的位置
	m_nMouseOnItemX = 0;
	m_nMouseOnItemY = 0;

	//鼠标在A2DSprite上面的位置
	m_nMouseOnA2DSpriteX = 0;
	m_nMouseOnA2DSpriteY = 0;

	m_pA2DSpriteUserPic = new A2DSprite;

}

//-------------------------------------------------------------------
CDlgCustomizeUserPic::~CDlgCustomizeUserPic()
{
	if( m_pA2DSpriteUserPic)
	{
		m_pA2DSpriteUserPic->Release();
		delete m_pA2DSpriteUserPic;
	}
	
}

//-------------------------------------------------------------------
bool CDlgCustomizeUserPic::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;

	Default2Data();

	UpdateData(false);

	return true;
}

//-------------------------------------------------------------------
void CDlgCustomizeUserPic::Default2Data()
{
	m_nUserPicAlpha = 255;
}

//-------------------------------------------------------------------
void CDlgCustomizeUserPic::DoDataExchange(bool bSave)
{
	//ddx control
	DDX_Control("Pic_UserPic", m_pPicUserPic);

	DDX_Control("Btn_UserPicDrag", m_pBtnUserPicDrag);
	DDX_Control("Btn_UserPicZoomIn", m_pBtnUserPicZoomIn);
	DDX_Control("Btn_UserPicZoomOut", m_pBtnUserPicZoomOut);

	DDX_Control("Slider_UserPicAlpha", m_pSliderUserPicAlpha);
	DDX_Control("Label_UserPicAlpha", m_pLabelUserPicAlpha);

	DDX_Control("Label_UserPic", m_pLabelUserPic);

	
	
	if (!IsContextReady()) return;
	//ddx data
	DDX_Slider(bSave, "Slider_UserPicAlpha", m_nUserPicAlpha);
}

//-------------------------------------------------------------------
void CDlgCustomizeUserPic::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();

}

float CDlgCustomizeUserPic::GetA2DSpriteUserPicScale()
{
	return m_fA2DSpriteUserPicScale * GetAUIManager()->GetWindowScale();
}

float CDlgCustomizeUserPic::GetPicUserPicScale()
{
	return m_fPicUserPicScale * GetAUIManager()->GetWindowScale();
}

// called after a2dsprite had been updated
void CDlgCustomizeUserPic::ResetLayout()
{
	m_pA2DSpriteUserPic->SetLinearFilter(true);//设置线性过滤
	
	m_fPicUserPicScale = 1.0f;

	m_fA2DSpriteUserPicOriScale = 250.F //控件原始大小 
								  / (float)m_pA2DSpriteUserPic->GetWidth();

	m_fA2DSpriteUserPicScale = m_fA2DSpriteUserPicOriScale;
	
	m_nPicUserPicOriWidth = (int)(m_fA2DSpriteUserPicOriScale * m_pA2DSpriteUserPic->GetWidth()); 
	m_nPicUserPicOriHeight = (int)(m_fA2DSpriteUserPicOriScale * m_pA2DSpriteUserPic->GetHeight()); 

	// reset to (0,0)
	m_nA2DSPriteUserPicItemPosX = 0;
	m_nA2DSPriteUserPicItemPosY =0;

 	SIZE dialogFrameSize = GetFrame()->GetSizeLimit();
	//设置图片窗口大小
	ResetDialogSize((int)(m_nPicUserPicOriWidth * GetAUIManager()->GetWindowScale()) + dialogFrameSize.cx, 
					(int)(m_nPicUserPicOriHeight * GetAUIManager()->GetWindowScale()) + dialogFrameSize.cy + 
					m_pBtnUserPicDrag->GetSize().cy + m_pLabelUserPic->GetSize().cy);
	RefreshUserPic();
}

void CDlgCustomizeUserPic::RefreshUserPic()
{
	m_pA2DSpriteUserPic->SetScaleX(GetA2DSpriteUserPicScale());
	m_pA2DSpriteUserPic->SetScaleY(GetA2DSpriteUserPicScale());
	
	int nPicUserPicWidth = (int)(m_nPicUserPicOriWidth * GetPicUserPicScale());
	int nPicUserPicHeight = (int)(m_nPicUserPicOriHeight * GetPicUserPicScale());
	int nPicUserPicX,nPicUserPicY;
	
	SIZE dialogFrameSize = GetFrame()->GetSizeLimit();
	
	if( nPicUserPicWidth > GetSize().cx - dialogFrameSize.cx)
	{
		nPicUserPicX = dialogFrameSize.cx/2;
		nPicUserPicWidth = GetSize().cx - dialogFrameSize.cx;
	}
	else
	{
		nPicUserPicX = (GetSize().cx - nPicUserPicWidth)/2;	
	}
	
	if( nPicUserPicHeight > GetSize().cy -m_pLabelUserPic->GetSize().cy - dialogFrameSize.cy - m_pBtnUserPicDrag->GetSize().cy)
	{
		nPicUserPicY = m_pLabelUserPic->GetSize().cy + dialogFrameSize.cy/2;
		nPicUserPicHeight = GetSize().cy -m_pLabelUserPic->GetSize().cy - dialogFrameSize.cy - m_pBtnUserPicDrag->GetSize().cy;
	}
	else
	{
		nPicUserPicY = (GetSize().cy - m_pBtnUserPicDrag->GetSize().cy -m_pLabelUserPic->GetSize().cy - dialogFrameSize.cy - nPicUserPicHeight)/2 + m_pLabelUserPic->GetSize().cy + dialogFrameSize.cy/2;
	}
	
	//设置控件尺寸
	m_pPicUserPic->SetSize(nPicUserPicWidth, nPicUserPicHeight);
	
	//设置控件位置
	m_pPicUserPic->SetPos(nPicUserPicX, nPicUserPicY);
	
	//nItemWidth大小
	int nItemWidth = (int)(nPicUserPicWidth * (1.0f/GetA2DSpriteUserPicScale()));
	int nItemHeight = (int) (nPicUserPicHeight * (1.0f/GetA2DSpriteUserPicScale()));
		
	if( m_nA2DSPriteUserPicItemPosX + nItemWidth > m_pA2DSpriteUserPic->GetWidth())
	{
		m_nA2DSPriteUserPicItemPosX = m_pA2DSpriteUserPic->GetWidth() - nItemWidth;
	}
	if( m_nA2DSPriteUserPicItemPosY + nItemHeight > m_pA2DSpriteUserPic->GetHeight())
	{
		m_nA2DSPriteUserPicItemPosY = m_pA2DSpriteUserPic->GetHeight() - nItemHeight;
	}
	
	A3DRECT rectItem(m_nA2DSPriteUserPicItemPosX, m_nA2DSPriteUserPicItemPosY, m_nA2DSPriteUserPicItemPosX + nItemWidth, m_nA2DSPriteUserPicItemPosY + nItemHeight);
	
	m_pA2DSpriteUserPic->ResetItems(1, &rectItem);
	m_pPicUserPic->SetCover( m_pA2DSpriteUserPic, 0);
}

void CDlgCustomizeUserPic::ScaleDialogTo(int nRight, int nButtom)
{
	//获取客户区大小
	HWND hWnd = GetActiveWindow();
	RECT rectClient;
	GetClientRect(hWnd, & rectClient);

	SIZE dialogFrameSize = GetFrame()->GetSizeLimit();
	
	const int minW = (int)(250 * GetAUIManager()->GetWindowScale());
	const int minH = (int)(100 * GetAUIManager()->GetWindowScale());
	
	int nMinW = (GetPos().x + minW + dialogFrameSize.cx/2 - m_pBtnUserPicDrag->GetSize().cx / 2);
	if( nRight < nMinW)
	{
		nRight = nMinW;
	}
	
	//设置图片框在窗口左侧，不能覆盖右侧的个性化对话框
	int nMaxW = (GetCustomizeMan()->m_pDlgCustomizeFace->GetPos().x - dialogFrameSize.cx/2 - m_pBtnUserPicDrag->GetSize().cx / 2);
	if( nRight > nMaxW )
	{
		nRight = nMaxW;
	}
	
	int nMinH = (GetPos().y + minH + m_pLabelUserPic->GetSize().cy + dialogFrameSize.cy/2 + m_pBtnUserPicDrag->GetSize().cy / 2);
	if( nButtom < nMinH)
	{
		nButtom = nMinH;
	}
	
	int nMaxH = (rectClient.bottom - dialogFrameSize.cy/2 - m_pBtnUserPicDrag->GetSize().cy / 2);
	if( nButtom > nMaxH)
	{
		nButtom = nMaxH;
	}

	int nWinWidth = nRight - GetPos().x + m_pBtnUserPicDrag->GetSize().cx/2 + dialogFrameSize.cx/2;
	int nWinHeight = nButtom - GetPos().y + m_pBtnUserPicDrag->GetSize().cy/2 + dialogFrameSize.cy/2;
	
	ResetDialogSize(nWinWidth, nWinHeight);
}

void CDlgCustomizeUserPic::ResetDialogSize(int nWinWidth, int nWinHeight)
{
	//设置窗口尺寸
	SetSize(nWinWidth, nWinHeight);
	
	SIZE dialogFrameSize = GetFrame()->GetSizeLimit();
	
	//设置4个按钮位置
	m_pBtnUserPicDrag->SetPos(nWinWidth - m_pBtnUserPicDrag->GetSize().cx
		-dialogFrameSize.cx/2, 
		nWinHeight - m_pBtnUserPicDrag->GetSize().cy
		-dialogFrameSize.cy/2);
	
	m_pBtnUserPicZoomIn->SetPos(nWinWidth - m_pBtnUserPicDrag->GetSize().cx 
		- m_pBtnUserPicZoomIn->GetSize().cx
		-dialogFrameSize.cx/2,
		nWinHeight- m_pBtnUserPicZoomIn->GetSize().cy
		-dialogFrameSize.cy/2);
	
	m_pBtnUserPicZoomOut->SetPos(nWinWidth - m_pBtnUserPicDrag->GetSize().cx 
		- m_pBtnUserPicZoomIn->GetSize().cx 
		- m_pBtnUserPicZoomOut->GetSize().cx
		-dialogFrameSize.cx/2,
		nWinHeight - m_pBtnUserPicZoomOut->GetSize().cy
		-dialogFrameSize.cy/2);
	
	
	//设置滚动条和标签的位置
	m_pLabelUserPicAlpha->SetPos(dialogFrameSize.cx/2, GetSize().cy - m_pLabelUserPicAlpha->GetSize().cy -dialogFrameSize.cy/2);
	const int minX = (int)(50 * GetAUIManager()->GetWindowScale());
	m_pSliderUserPicAlpha->SetPos(minX, GetSize().cy - m_pSliderUserPicAlpha->GetSize().cy -dialogFrameSize.cy/2);
	m_pLabelUserPic->SetPos((GetSize().cx - m_pLabelUserPic->GetSize().cx)/2, dialogFrameSize.cy/2);
}

void CDlgCustomizeUserPic::MoveUserPicture(int left, int top)
{
	A3DRECT rectItem;
	rectItem.left = left; 
	rectItem.top = top;

	if( rectItem.left < 0)
	{
		rectItem.left = 0;
	}

	if( rectItem.top < 0)
	{
		rectItem.top = 0;
	}
	
	int nItemWidth = (int)(m_pPicUserPic->GetSize().cx * (1.0f/GetA2DSpriteUserPicScale()));
	int nItemHeight = (int)(m_pPicUserPic->GetSize().cy * (1.0f/GetA2DSpriteUserPicScale()));

	if(rectItem.left + nItemWidth > m_pA2DSpriteUserPic->GetWidth())
	{
		rectItem.left = (int)(m_pA2DSpriteUserPic->GetWidth() - nItemWidth);
	}

	if(rectItem.top + nItemHeight > m_pA2DSpriteUserPic->GetHeight())
	{
		rectItem.top = (int)(m_pA2DSpriteUserPic->GetHeight() - nItemHeight);
	}
	
	m_nA2DSPriteUserPicItemPosX = rectItem.left;
	m_nA2DSPriteUserPicItemPosY = rectItem.top;
	
	rectItem.right = rectItem.left + nItemWidth;
	rectItem.bottom = rectItem.top + nItemHeight;
				
	m_pA2DSpriteUserPic->ResetItems(1, &rectItem);
	
	m_pPicUserPic->SetCover( m_pA2DSpriteUserPic, 0);
}

//-------------------------------------------------------------------------------
//放大图片
void CDlgCustomizeUserPic::ZoomOutUserPic()
{
	if( m_fA2DSpriteUserPicScale < 2.0f)
	{
		
		m_fPicUserPicScale += 0.05f;//控件比例
		m_fA2DSpriteUserPicScale += (0.05f * m_fA2DSpriteUserPicOriScale);//图片比例

		RefreshUserPic();
	}
}

//-------------------------------------------------------------------------------
//缩小图片
void CDlgCustomizeUserPic::ZoomInUserPic()
{
	if( m_fA2DSpriteUserPicScale > 0.1f)
	{
		
		m_fPicUserPicScale -= 0.05f;//控件比例
		m_fA2DSpriteUserPicScale -= (0.05f * m_fA2DSpriteUserPicOriScale);//图片比例

		RefreshUserPic();
	}
}

//---------------------------------------------------------------------
//放大图片
void CDlgCustomizeUserPic::OnCommandZoomOutUserPic(const char* szCommand)
{
	ZoomOutUserPic();
}

//---------------------------------------------------------------------
//缩小图片
void CDlgCustomizeUserPic::OnCommandZoomInUserPic(const char* szCommand)
{
	ZoomInUserPic();
}

//---------------------------------------------------------------------
//Move slider
void CDlgCustomizeUserPic::OnCommandMoveSlider(const char* szCommand)
{
	UpdateData(true);
	m_pPicUserPic->SetAlpha(m_nUserPicAlpha);
}

//---------------------------------------------------------------------
void CDlgCustomizeUserPic::OnTick()
{
	//缩小
	if( m_pBtnUserPicZoomIn->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
		ZoomInUserPic();
	}

	//放大
	if( m_pBtnUserPicZoomOut->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
		ZoomOutUserPic();
	}
	
	//禁止参考图片框拖动到窗口右侧的个性化对话框上
	if(GetPos().x + GetSize().cx >GetCustomizeMan()->m_pDlgCustomizeFace->GetPos().x)
	{
		// old : SetPos( GetCustomizeMan()->m_pDlgCustomizeFace->GetPos().x - GetSize().cx, GetPos().y);
		SetPosEx(GetCustomizeMan()->m_pDlgCustomizeFace->GetPos().x - GetSize().cx, GetPos().y);
	}

}

//---------------------------------------------------------------------
//滚动鼠标放大缩小图片
void CDlgCustomizeUserPic::OnMouseWheelZoom(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if((short)HIWORD(wParam) > 0)
	{
		ZoomInUserPic();
	}
	else
	{
		ZoomOutUserPic();
	}
}

//---------------------------------------------------------------------
void CDlgCustomizeUserPic::OnLButtonDownPicUserPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	SetCanMove(false);

	int nMouseX, nMouseY;

	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	nMouseX = GET_X_LPARAM(lParam)  - p->X;
	nMouseY = GET_Y_LPARAM(lParam)  - p->Y;

	//(拖动图片)
	A3DRECT rectItem = m_pA2DSpriteUserPic->GetItem(0)->GetRect();
	//鼠标在图片上的位置
	m_nMouseOnA2DSpriteX = (int)((nMouseX - GetPos().x - m_pPicUserPic->GetPos().x)* (1.0f/GetA2DSpriteUserPicScale())) + rectItem.left;
	m_nMouseOnA2DSpriteY = (int)((nMouseY - GetPos().y - m_pPicUserPic->GetPos().y)* (1.0f/GetA2DSpriteUserPicScale()))+ rectItem.top;

}

//---------------------------------------------------------------------
void CDlgCustomizeUserPic::OnMouseMovePicUserPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//左键按下
	if(wParam & MK_LBUTTON)
	{		
		int nMouseX, nMouseY;

		A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
		nMouseX = GET_X_LPARAM(lParam)  - p->X;
		nMouseY = GET_Y_LPARAM(lParam)  - p->Y;

		//(拖动图片)
		int nMouseOnItemX, nMouseOnItemY;
		nMouseOnItemX = (int)((nMouseX - GetPos().x - m_pPicUserPic->GetPos().x)* (1.0f/GetA2DSpriteUserPicScale()));
		nMouseOnItemY = (int)((nMouseY - GetPos().y - m_pPicUserPic->GetPos().y)* (1.0f/GetA2DSpriteUserPicScale())) ;

		MoveUserPicture(m_nMouseOnA2DSpriteX - nMouseOnItemX, 
						m_nMouseOnA2DSpriteY - nMouseOnItemY);
	}
}

//-------------------------------------------------------------------------
void CDlgCustomizeUserPic::OnLButtonDownBtnDrag(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	SetCaptureObject(pObj);
}

//-------------------------------------------------------------------------
void CDlgCustomizeUserPic::OnLButtonUpBtnDrag(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	SetCaptureObject(NULL);	
}

//-------------------------------------------------------------------------
void CDlgCustomizeUserPic::OnMouseMoveBtnDrag(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//拖动按钮处于下压状态
	//缩放窗口
	if( m_pBtnUserPicDrag->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
		//左键按下
		if(wParam & MK_LBUTTON)
		{		
			int nMouseX, nMouseY;

			A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
			nMouseX = GET_X_LPARAM(lParam)  - p->X;
			nMouseY = GET_Y_LPARAM(lParam)  - p->Y;

			ScaleDialogTo(nMouseX, nMouseY);
			RefreshUserPic();
		}
	}
}

//----------------------------------------------------------------------------------------
void CDlgCustomizeUserPic::OnLButtonUpPicUserPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	SetCanMove(true);
}

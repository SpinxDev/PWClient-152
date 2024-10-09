// CreateState.cpp: implementation of the CCreateState class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStateCreateRect.h"
#include "FWStateSelect.h"
#include "FWGlyph.h"
#include "FWDoc.h"
#include "FWCommandCreate.h"
#include "FWAUIManager.h"
#include "FWCommandList.h"
#include "FWAlgorithm.h"
#include "FWView.h"
#include "FWFlatCollector.h"
#include <WinUser.h>

#define new A_DEBUG_NEW



using namespace FWAlgorithm;



FW_IMPLEMENT_DYNAMIC(FWStateCreateRect, FWState)

FWStateCreateRect::FWStateCreateRect(FWView *pView, FWGlyph* pGlyph):
	FWState(pView),
	m_bMouseDown(false),
	m_bGlyphInUse(false)
{
	m_pGlyph = pGlyph;
}

FWStateCreateRect::~FWStateCreateRect()
{
	GetView()->ReleaseCapture();

	if (!m_bGlyphInUse)
		delete m_pGlyph;
}

void FWStateCreateRect::OnMouseMove(UINT nFlags, APointI point)
{
	if (!m_bMouseDown)
		return;

	ClampPointToRect(point, GetView()->GetClientRectLP());

	ARectI rect(m_upleftPoint.x, m_upleftPoint.y, point.x, point.y);
	
	if (nFlags & MK_SHIFT)
		rect = RegularizeRect(&rect);
	else
		rect.Normalize();

	m_rect = rect;
}

void FWStateCreateRect::OnLButtonUp(UINT nFlags, APointI point)
{
	if (!m_bMouseDown)
		return;

	ClampPointToRect(point, GetView()->GetClientRectLP());

	m_bMouseDown = false;

	if (point.x == m_upleftPoint.x || point.y == m_upleftPoint.y)
	{
		return;
	}

	ARectI rect(m_upleftPoint.x, m_upleftPoint.y, point.x, point.y);
	if (nFlags & MK_SHIFT) 
		rect = RegularizeRect(&rect);
	else
		rect.Normalize();

	m_rect = rect;
	
	m_pGlyph->SetSizeRect(rect);
	m_pGlyph->GenerateMeshParam();

	GetDocument()->GetCommandList()->ExecuteCommand(new FWCommandCreate(GetDocument(), m_pGlyph));
	m_bGlyphInUse = true;
	GetView()->ChangeState(new FWStateSelect(GetView()));
}

void FWStateCreateRect::OnLButtonDown(UINT Flags, APointI point)
{
	if (m_bMouseDown)
		return;

	m_upleftPoint = point;
	m_bMouseDown = true;
	GetView()->SetCapture();
}


void FWStateCreateRect::OnDraw()
{
	FWState::OnDraw();

	ARectI rect = m_rect;
	GetView()->LPtoDP(&rect);
	GetView()->GetFlatCollector()->DrawRectangle2D(
		rect,
		COLOR_DARK_GRAY);
}

void FWStateCreateRect::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE)
	{
		m_bMouseDown = false;
		GetView()->ReleaseCapture();
	}
}

ARectI FWStateCreateRect::RegularizeRect(ARectI *pRect)
{
	ARectI rect;
	int Width;
	int Height;
	Width = abs(pRect->right - pRect->left);
	Height = abs(pRect->top - pRect->bottom);
	int max = Width > Height ? Width : Height;
	if(pRect->left < pRect->right)
	{
		rect.left = pRect->left;
		rect.right = rect.left + max;
	}
	else
	{
		rect.right = pRect->left;
		rect.left = rect.right - max;
	}
	if(pRect->top < pRect->bottom)
	{
		rect.top = pRect->top;
		rect.bottom = rect.top + max;
	}
	else
	{
		rect.bottom = pRect->top;
		rect.top = rect.bottom - max;
	}
	return rect;	
}

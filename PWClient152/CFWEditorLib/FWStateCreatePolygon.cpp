// CreatePolygonState.cpp: implementation of the FWStateCreatePolygon class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStateCreatePolygon.h"
#include "FWAlgorithm.h"
#include "FWCommandCreate.h"
#include "FWStateSelect.h"
#include "FWGlyphPolygon.h"
#include "FWDoc.h"
#include "FWFlatCollector.h"
#include "FWCommandList.h"
#include "FWView.h"

#define new A_DEBUG_NEW


using namespace FWAlgorithm;


FW_IMPLEMENT_DYNAMIC(FWStateCreatePolygon, FWState)

FWStateCreatePolygon::FWStateCreatePolygon(FWView *pView, FWGlyphPolygon* pGlyph):
	FWState(pView),
	m_pGlyph(pGlyph),
	m_bIsGlyphInUse(false),
	m_curentPoint(0, 0)
{
}

FWStateCreatePolygon::~FWStateCreatePolygon()
{
	if (!m_bIsGlyphInUse)
	{
		delete m_pGlyph;
	}
}

void FWStateCreatePolygon::OnMouseMove(UINT nFlags, APointI point)
{
	if(!m_bIsLButtonDown && m_HandleArray.GetSize() > 0)
	{
		m_curentPoint = point;
	}
}

void FWStateCreatePolygon::OnDraw()
{
	FWState::OnDraw();
	m_pGlyph->Draw(GetView());

	FWFlatCollector *pCollector = GetView()->GetFlatCollector();
	int size = m_HandleArray.GetSize();
	if (size > 0)
	{
		APointI p1, p2;
		
		p1 = m_HandleArray[size - 1];
		p2 = m_curentPoint;
		GetView()->LPtoDP(&p1);
		GetView()->LPtoDP(&p2);
		pCollector->DrawLine(p1, p2, COLOR_LIGHT_GRAY);

		p1 = m_curentPoint;
		p2 = m_HandleArray[0];
		GetView()->LPtoDP(&p1);
		GetView()->LPtoDP(&p2);
		pCollector->DrawLine(p1, p2, COLOR_LIGHT_GRAY);

		if (size == 2)
		{
			p1 = m_pGlyph->GetHandle(1);
			p2 = m_pGlyph->GetHandle(2);
			GetView()->LPtoDP(&p1);
			GetView()->LPtoDP(&p2);
			pCollector->DrawLine(p1, p2, COLOR_DARK_GRAY);
		}
	}

}

void FWStateCreatePolygon::OnLClick(UINT nFlags, APointI point)
{
	m_HandleArray.Add(point);
	m_pGlyph->ResetHandles(&m_HandleArray);
	m_pGlyph->GenerateMeshParam();
	
	m_curentPoint = point;
}

void FWStateCreatePolygon::OnRClick(UINT nFlags, APointI point)
{
	if (m_pGlyph->GetHandleCount() < 3)
	{
		GetView()->ChangeState(new FWStateSelect(GetView()));
		return;
	}
	
	GetDocument()->GetCommandList()->ExecuteCommand(new FWCommandCreate(GetDocument(), m_pGlyph));	
	m_bIsGlyphInUse = true;  
	GetView()->ChangeState(new FWStateSelect(GetView()));
}

void FWStateCreatePolygon::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE)
	{
		GetView()->ChangeState(new FWStateSelect(GetView()));
	}
	else if (nChar == VK_RETURN)
	{
		OnRClick(0, APointI());
	}
}

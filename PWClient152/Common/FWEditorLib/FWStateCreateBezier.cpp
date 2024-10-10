// CreateBezierState.cpp: implementation of the FWStateCreateBezier class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStateCreateBezier.h"
#include "FWGlyphBezier.h"
#include "FWDoc.h"
#include "FWStateSelect.h"
#include "FWCommandCreate.h"
#include "FWAlgorithm.h"
#include "FWView.h"
#include "FWCommandList.h"
#include "FWFlatCollector.h"
#include "FWView.h"

using namespace FWAlgorithm;

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNAMIC(FWStateCreateBezier, FWState)



FWStateCreateBezier::FWStateCreateBezier(FWView *pView, FWGlyphBezier *pGlyph) :
	FWState(pView),
	m_pBezier(pGlyph),
	m_bLButtonDowned(false),
	m_bGraphInUse(false),
	m_curentPoint(0, 0)
{
}

FWStateCreateBezier::~FWStateCreateBezier()
{
	if (!m_bGraphInUse)
	{
		delete m_pBezier;
	}
}

void FWStateCreateBezier::OnLButtonUp(UINT nFlags, APointI point)
{
	m_bLButtonDowned = false;
}

void FWStateCreateBezier::OnLButtonDown(UINT nFlags, APointI point)
{
	m_bLButtonDowned = true;
}

void FWStateCreateBezier::OnLClick(UINT nFlags, APointI point)
{
	for (int i = 0; i < 3; i++)
		m_pBezier->m_aryHandle.Add(point);
	m_pBezier->InitModeArray();
	m_pBezier->SetAllMode(FWGlyphBezier::MODE_SMOOTH);
	m_pBezier->ComputeSizeRect();
	m_pBezier->GenerateMeshParam();

	m_curentPoint = point;
}

void FWStateCreateBezier::OnMouseMove(UINT nFlags, APointI point)
{
	if(!m_bLButtonDowned && m_pBezier->GetHandleCount() > 0)
	{
		m_curentPoint = point;
	}
}

void FWStateCreateBezier::OnDraw()
{
	FWState::OnDraw();

	if (m_pBezier)
	{
		m_pBezier->Draw(GetView());
		m_pBezier->DrawHandles(GetView());
	}

	int handleCount = m_pBezier->GetHandleCount();
	if (handleCount > 0)
	{
		FWFlatCollector *pCollector = GetView()->GetFlatCollector();

		APointI p1, p2;
		
		p1 = m_pBezier->GetHandle(handleCount - 1);
		p2 = m_curentPoint;
		GetView()->LPtoDP(&p1); GetView()->LPtoDP(&p2);
		pCollector->DrawLine(p1, p2, COLOR_LIGHT_GRAY);
		
		p1 = m_curentPoint;
		p2 = m_pBezier->GetHandle(2);
		GetView()->LPtoDP(&p1); GetView()->LPtoDP(&p2);
		pCollector->DrawLine(p1, p2, COLOR_LIGHT_GRAY);
		if (handleCount == 6)
		{
			p1 = m_pBezier->GetHandle(2);
			p2 = m_pBezier->GetHandle(5);
			GetView()->LPtoDP(&p1); GetView()->LPtoDP(&p2);
			pCollector->DrawLine(p1, p2, COLOR_DARK_GRAY);
		}
	}
}

void FWStateCreateBezier::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
	{
		OnRClick(0, APointI());
	}
	else if (nChar == VK_ESCAPE)
	{
		GetView()->ChangeState(new FWStateSelect(GetView()));
	}
}


void FWStateCreateBezier::OnRClick(UINT nFlags, APointI point)
{
	if (m_pBezier->GetHandleCount() < 9)
	{
		GetView()->ChangeState(new FWStateSelect(GetView()));
		return;
	}
	
	m_pBezier->SetAngle(0);
	GetDocument()->GetCommandList()->ExecuteCommand(new FWCommandCreate(GetDocument(), m_pBezier));
	m_bGraphInUse = true;
	GetView()->ChangeState(new FWStateSelect(GetView()));
}

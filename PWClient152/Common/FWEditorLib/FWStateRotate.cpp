// RotateState.cpp: implementation of the FWRotateState class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStateRotate.h"
#include "FWGlyph.h"
#include "FWCommandRotate.h"
#include "FWStateSelect.h"
#include <math.h>
#include "FWView.h"
#include "FWDoc.h"
#include "FWCommandList.h"

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNAMIC(FWStateRotate, FWState)



FWStateRotate::FWStateRotate(FWView *pView, FWGlyph *pGlyph) :
	FWState(pView),
	m_pGlyph(pGlyph)
{
	m_bIsLButtonDown = false;
	m_fOldAngle = pGlyph->GetAngle();
	m_fFirstPointAngle = 0;
	m_pGlyph->StartRotateInState();
}

FWStateRotate::~FWStateRotate()
{
	m_pGlyph->EndRotateInState();
}

void FWStateRotate::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE)
	{
		m_pGlyph->SetAngle(m_fOldAngle);
		GetView()->ChangeState(new FWStateSelect(GetView()));
	}
}

void FWStateRotate::OnLButtonUp(UINT nFlags, APointI point)
{
	if (!m_bIsLButtonDown)
		return;

	GetView()->ReleaseCapture();
	m_bIsLButtonDown = false;
	APointI center = m_pGlyph->GetCentralPoint();			
	if (center == point)  
	{
		m_pGlyph->SetAngle(m_fOldAngle);
		return;
	}
	double fSecondPointAngle = atan2(double(center.y - point.y), double(point.x - center.x));	

	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandRotate(m_pGlyph, double(m_fOldAngle + fSecondPointAngle - m_fFirstPointAngle), m_fOldAngle));

	m_fFirstPointAngle = fSecondPointAngle;
	m_fOldAngle = m_pGlyph->GetAngle();
}

void FWStateRotate::OnLButtonDown(UINT nFlags, APointI point)
{
	if (m_bIsLButtonDown)
		return;
	APointI center = m_pGlyph->GetCentralPoint();
	if (center == point)
		return;

	m_bIsLButtonDown = true;	
	GetView()->SetCapture();
	m_fFirstPointAngle = atan2(double(center.y - point.y), double(point.x - center.x));	
}

void FWStateRotate::OnMouseMove(UINT nFlags, APointI point)
{
	if (m_bIsLButtonDown)
	{
		APointI center = m_pGlyph->GetCentralPoint();
		if (point == center)
			return;	

		double fSecondPointAngle = atan2(double(center.y - point.y), double(point.x - center.x));
		m_pGlyph->SetAngle(double(m_fOldAngle + fSecondPointAngle - m_fFirstPointAngle));
	}
}

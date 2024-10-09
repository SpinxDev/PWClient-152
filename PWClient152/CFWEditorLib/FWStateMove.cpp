// MoveState.cpp: implementation of the FWMoveState class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStateMove.h"
#include "FWDoc.h"
#include "FWGlyph.h"
#include "FWCommandMove.h"
#include "FWStateSelect.h"
#include "FWGlyphList.h"
#include "FWCommandList.h"
#include "FWAUIManager.h"
#include "FWAlgorithm.h"
#include "FWView.h"

#define new A_DEBUG_NEW


using namespace FWAlgorithm;

FW_IMPLEMENT_DYNAMIC(FWStateMove, FWState)

FWStateMove::FWStateMove(FWView *pView, APointI mousePoint):
	FWState(pView),
	m_originalPoint(mousePoint)
{
	FWGlyphList* pList = GetDocument()->GetGlyphList();
	FWGlyph* pGlyph;

	ALISTPOSITION pos = pList->GetSelHeadPosition();
	while(pos)
	{
		pGlyph = pList->GetSelNext(pos);
		m_centerList.AddTail(pGlyph->GetCentralPoint());
	}

	m_originalRect = pList->GetSelTrueRect();

	GetView()->SetCapture();
}

FWStateMove::~FWStateMove()
{
	GetView()->ReleaseCapture();
}

void FWStateMove::OnMouseMove(UINT nFlags, APointI point)
{
	FWGlyphList* pList = GetDocument()->GetGlyphList();
	FWGlyph* pGlyph;

	APointI offset = point - m_originalPoint;
	ClampOffsetToView(pList, offset);

	ALISTPOSITION glyphPos = pList->GetSelHeadPosition();
	ALISTPOSITION centerPos = m_centerList.GetHeadPosition();
	while(glyphPos)
	{
		pGlyph = pList->GetSelNext(glyphPos);
		point = m_centerList.GetNext(centerPos);
		pGlyph->SetCentralPoint(point + offset);
	}
}

void FWStateMove::OnLButtonUp(UINT nFlags, APointI point)
{
	APointI offset = point - m_originalPoint;
	FWGlyphList* pList = GetDocument()->GetGlyphList();
	ClampOffsetToView(pList, offset);

	FWCommandMove::AGlyphArray glyphArray;
	FWCommandMove::APointArray oldCenters, newCenters;

	int length = pList->GetSelCount();
	glyphArray.SetSize(length, 0);
	oldCenters.SetSize(length, 0);
	newCenters.SetSize(length, 0);

	ALISTPOSITION glyphPos = pList->GetSelHeadPosition();
	ALISTPOSITION centerPos = m_centerList.GetHeadPosition();
	int i = 0;
	while(glyphPos)
	{
		glyphArray[i] = pList->GetSelNext(glyphPos);
		oldCenters[i] = m_centerList.GetNext(centerPos);
		newCenters[i] = oldCenters[i] + offset;
		i++;
	}

	FWCommandMove* pCommand = new FWCommandMove(
		glyphArray,
		oldCenters,
		newCenters);
	GetDocument()->GetCommandList()->ExecuteCommand(pCommand);


	GetView()->ChangeState(new FWStateSelect(GetView()));
}


void FWStateMove::ClampOffsetToView(const FWGlyphList* pList, APointI& offset)
{
	ARectI rectWnd = GetView()->GetClientRectLP();
	ARectI rectClamped = m_originalRect;

	rectClamped.Offset(offset);
	ClampRectToRect(rectClamped, rectWnd);
	offset.x = rectClamped.left - m_originalRect.left;
	offset.y = rectClamped.top - m_originalRect.top;
}

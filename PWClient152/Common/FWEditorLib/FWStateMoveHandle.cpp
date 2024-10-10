// MoveHandleClass.cpp: implementation of the CMoveHandleClass class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStateMoveHandle.h"
#include "FWGlyph.h"
#include "FWCommandList.h"
#include "FWDoc.h"
#include "FWCommandMoveHandle.h"
#include "FWStateSelect.h"
#include "FWAUIManager.h"
#include "FWAlgorithm.h"
#include "FWView.h"
#include "FWFlatCollector.h"

#define new A_DEBUG_NEW


using namespace FWAlgorithm;

FW_IMPLEMENT_DYNAMIC(FWStateMoveHandle, FWState)


FWStateMoveHandle::FWStateMoveHandle(FWView *pView, FWGlyph* pGlyph, int nHandle):
	FWState(pView),
	m_pGlyph(pGlyph),
	m_nHandle(nHandle)
{
	m_oldPoint = pGlyph->GetHandle(nHandle);
	pGlyph->StartMoveHandleInState(nHandle);
	GetView()->SetCapture();
}

FWStateMoveHandle::~FWStateMoveHandle()
{
	m_pGlyph->EndMoveHandleInState(m_nHandle);
	GetView()->ReleaseCapture();
}

void FWStateMoveHandle::OnMouseMove(UINT nFlags, APointI point)
{
	ClampPointToRect(point, GetView()->GetClientRectLP());

	m_pGlyph->MoveHandleTo(m_nHandle, point);

	SetCursor(m_pGlyph->GetHandleCursor(m_nHandle));
}

void FWStateMoveHandle::OnLButtonUp(UINT nFlags, APointI point)
{
	ClampPointToRect(point, GetView()->GetClientRectLP());

	FWCommandList* pList = GetDocument()->GetCommandList();
	pList->ExecuteCommand(
		new FWCommandMoveHandle(
			m_pGlyph,
			m_nHandle,
			m_oldPoint,
			point));
	GetView()->ReleaseCapture();

	GetView()->ChangeState(new FWStateSelect(GetView()));
}


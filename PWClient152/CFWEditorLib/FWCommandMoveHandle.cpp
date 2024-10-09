// MoveHandleCommand.cpp: implementation of the FWMoveHandleCommand class.
//
//////////////////////////////////////////////////////////////////////


#include "FWCommandMoveHandle.h"
#include "FWGlyph.h"
#include "FWDoc.h"


#define new A_DEBUG_NEW






FWCommandMoveHandle::FWCommandMoveHandle( FWGlyph* pGlyph, 
									   int nHandle, 
									   APointI oldPoint, 
									   APointI newPoint):
	m_pGlyph(pGlyph),
	m_nHandle(nHandle),
	m_oldPoint(oldPoint),
	m_newPoint(newPoint)
{
	
}

FWCommandMoveHandle::~FWCommandMoveHandle()
{

}

void FWCommandMoveHandle::Execute()
{
	m_pGlyph->MoveHandleTo(m_nHandle, m_newPoint);
	m_pGlyph->GenerateMeshParam();
}

void FWCommandMoveHandle::Unexecute()
{
	m_pGlyph->MoveHandleTo(m_nHandle, m_oldPoint);
	m_pGlyph->GenerateMeshParam();
}

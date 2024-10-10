// FWCommandRemoveHandle.cpp: implementation of the FWCommandRemoveHandle class.
//
//////////////////////////////////////////////////////////////////////

#include "FWCommandRemoveHandle.h"
#include "FWGlyph.h"
#include "CodeTemplate.h"



FWCommandRemoveHandle::FWCommandRemoveHandle(FWGlyph *pGlyph, int nHandleToRemove)
{
	m_pGlyph = pGlyph;
	m_nHandleToRemove = nHandleToRemove;
	m_pData = NULL;
}

FWCommandRemoveHandle::~FWCommandRemoveHandle()
{
	SAFE_DELETE_ARRAY(m_pData);
}

void FWCommandRemoveHandle::Execute()
{
	SAFE_DELETE_ARRAY(m_pData);
	m_pGlyph->RemoveHandle(m_nHandleToRemove, &m_aryHandleRemoved, &m_pData);
	m_pGlyph->GenerateMeshParam();
}

void FWCommandRemoveHandle::Unexecute()
{
	m_pGlyph->InsertHandleBefore(m_nHandleToRemove, m_aryHandleRemoved, m_pData);
	m_pGlyph->GenerateMeshParam();
}

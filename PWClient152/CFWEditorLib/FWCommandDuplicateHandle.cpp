// FWCommandDuplicateHandle.cpp: implementation of the FWCommandDuplicateHandle class.
//
//////////////////////////////////////////////////////////////////////

#include "FWCommandDuplicateHandle.h"
#include "FWGlyph.h"



FWCommandDuplicateHandle::FWCommandDuplicateHandle(FWGlyph *pGlyph, int nHandleToDuplicate)
{
	m_pGlyph = pGlyph;
	m_nHandleToDuplicate = nHandleToDuplicate;
}

FWCommandDuplicateHandle::~FWCommandDuplicateHandle()
{

}

void FWCommandDuplicateHandle::Execute()
{
	m_pGlyph->DuplicateHandle(m_nHandleToDuplicate);
	m_pGlyph->GenerateMeshParam();
}

void FWCommandDuplicateHandle::Unexecute()
{
	m_pGlyph->RemoveHandle(m_nHandleToDuplicate);
	m_pGlyph->GenerateMeshParam();
}

// ResizeCommand.cpp: implementation of the FWResizeCommand class.
//
//////////////////////////////////////////////////////////////////////


#include "FWCommandResize.h"
#include "FWGlyph.h"
#include "FWStateSelect.h"

#define new A_DEBUG_NEW





FWCommandResize::FWCommandResize(FWGlyph *pGlyph, ARectI newRect, ARectI oldRect):
	m_newSizeRect(newRect),
	m_oldSizeRect(oldRect),
	m_pGlyph(pGlyph)
{
}

FWCommandResize::~FWCommandResize()
{

}

void FWCommandResize::Execute()
{
	m_pGlyph->SetSizeRect(m_newSizeRect);
	m_pGlyph->GenerateMeshParam();
}

void FWCommandResize::Unexecute()
{
	m_pGlyph->SetSizeRect(m_oldSizeRect);
	m_pGlyph->GenerateMeshParam();
}


// RotateCommand.cpp: implementation of the FWRotateCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "FWCommandRotate.h"
#include "FWGlyph.h"

#define new A_DEBUG_NEW




FWCommandRotate::FWCommandRotate(FWGlyph *pGlyph, double fNewAngle, double fOldAngle):
	m_pGlyph(pGlyph), m_fNewAngle(fNewAngle), m_fOldAngle(fOldAngle)
{

}

FWCommandRotate::~FWCommandRotate()
{

}

void FWCommandRotate::Execute()
{	
	m_pGlyph->SetAngle(m_fNewAngle);
	m_pGlyph->GenerateMeshParam();
}

void FWCommandRotate::Unexecute()
{
	m_pGlyph->SetAngle(m_fOldAngle);
	m_pGlyph->GenerateMeshParam();
}

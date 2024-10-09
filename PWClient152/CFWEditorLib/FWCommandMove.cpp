#include "FWCommandMove.h"
#include "FWGlyph.h"
#include "FWDoc.h"

#define new A_DEBUG_NEW


FWCommandMove::FWCommandMove(
						   const AGlyphArray& glyphArray, 
						   const APointArray& oldCenters, 
						   const APointArray& newCenters)
{
	m_glyphArray.Append(glyphArray);
	m_oldCenters.Append(oldCenters);
	m_newCenters.Append(newCenters);
}

FWCommandMove::~FWCommandMove()
{
}

void FWCommandMove::Execute()
{
	FWGlyph* pGlyph;
	int length = m_glyphArray.GetSize();
	for(int i = 0; i < length; i ++)
	{
		pGlyph = m_glyphArray[i];
		pGlyph->SetCentralPoint(m_newCenters[i]);
		pGlyph->GenerateMeshParam();
	}
}

void FWCommandMove::Unexecute()
{
	FWGlyph* pGlyph;
	int length = m_glyphArray.GetSize();
	for(int i = 0; i < length; i ++)
	{
		pGlyph = m_glyphArray[i];
		pGlyph->SetCentralPoint(m_oldCenters[i]);
		pGlyph->GenerateMeshParam();
	}
}


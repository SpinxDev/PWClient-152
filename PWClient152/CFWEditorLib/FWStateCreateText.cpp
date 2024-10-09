// FWStateCreateText.cpp: implementation of the FWStateCreateText class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStateCreateText.h"
#include "FWGlyphText.h"
#include "FWAlgorithm.h"
#include "FWView.h"
#include "FWDoc.h"
#include "FWCommandList.h"
#include "FWCommandCreate.h"
#include "FWStateSelect.h"


#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNAMIC(FWStateCreateText, FWState)



FWStateCreateText::FWStateCreateText(FWView *pView, FWGlyphText *pGlyph) :
	FWState(pView),
	m_bGlyphInUse(false),
	m_pGlyph(pGlyph)
{

}

FWStateCreateText::~FWStateCreateText()
{
	if (!m_bGlyphInUse)
		SAFE_DELETE(m_pGlyph);
}

void FWStateCreateText::OnLClick(UINT nFlags, APointI point)
{
	m_pGlyph->SetCentralPoint(point);
	m_pGlyph->GenerateMeshParam();
	
	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandCreate(GetDocument(), m_pGlyph));
	m_bGlyphInUse = true;
	
	GetView()->ChangeState(new FWStateSelect(GetView()));
}

#ifndef _FWCOMMANDCOMBINE_H_
#define _FWCOMMANDCOMBINE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FWCommand.h"

class FWGlyphComposite;
class FWGlyph;
class FWDoc;

class FWCommandCombine : public FWCommand  
{
	FWGlyphComposite * m_pComposite;
	bool m_bCompositeInUse;
	FWDoc * m_pDoc;
public:
	FWCommandCombine(FWDoc *pDoc, FWGlyphComposite *pComposite);
	virtual ~FWCommandCombine();

	virtual void Execute();
	virtual void Unexecute();
};

#endif 

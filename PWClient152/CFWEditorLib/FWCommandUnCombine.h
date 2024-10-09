#ifndef _FWCOMMANDUNCOMBINE_H_
#define _FWCOMMANDUNCOMBINE_H_


#include "FWCommand.h"

class FWGlyphComposite;
class FWDoc;

class FWCommandUnCombine : public FWCommand  
{
	FWGlyphComposite * m_pComposite;
	bool m_bCompositeInUse;
	FWDoc * m_pDoc;
public:
	FWCommandUnCombine(FWDoc *pDoc, FWGlyphComposite * pComposite);
	virtual ~FWCommandUnCombine();

	virtual void Execute();
	virtual void Unexecute();
};

#endif 

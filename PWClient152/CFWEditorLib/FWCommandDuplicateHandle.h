#ifndef _FWCOMMANDDUPLICATEHANDLE_H_
#define _FWCOMMANDDUPLICATEHANDLE_H_

#include "FWCommand.h"

class FWGlyph;

class FWCommandDuplicateHandle : public FWCommand  
{
	FWGlyph * m_pGlyph;
	int m_nHandleToDuplicate;
public:
	FWCommandDuplicateHandle(FWGlyph *pGlyph, int nHandleToDuplicate);
	virtual ~FWCommandDuplicateHandle();

	virtual void Execute();
	virtual void Unexecute();

};

#endif 

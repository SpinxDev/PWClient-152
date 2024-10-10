#ifndef _FWCOMMANDREMOVEHANDLE_H_
#define _FWCOMMANDREMOVEHANDLE_H_


#include "FWCommand.h"
#include <AArray.h>
#include <APoint.h>

class FWGlyph;
typedef AArray<APointI, APointI&> APointArray;

class FWCommandRemoveHandle : public FWCommand  
{
	FWGlyph * m_pGlyph;
	int m_nHandleToRemove;
	APointArray m_aryHandleRemoved;
	void *m_pData;
public:
	// note : for some glyph (such as bezier), handles are divided into 
	// groups. handles of a group must be remove/restore at the same time
	FWCommandRemoveHandle(FWGlyph *pGlyph, int nHandleToRemove);
	virtual ~FWCommandRemoveHandle();

	virtual void Execute();
	virtual void Unexecute();

};

#endif 

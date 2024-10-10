#ifndef _MOVEHANDLECOMMAND_H_
#define _MOVEHANDLECOMMAND_H_

#include "FWCommand.h"
#include <APoint.h>

class FWGlyph;

// FWCommandMoveHandle represents the action of 
// moving handles a glyph
class FWCommandMoveHandle : public FWCommand     
{
public:
	FWCommandMoveHandle(FWGlyph* pGlyph, int nHandle, APointI oldPoint, APointI newPoint);
	virtual ~FWCommandMoveHandle();
	
	virtual void Execute();
	virtual void Unexecute();

private:
	FWGlyph* m_pGlyph;
	int m_nHandle;
	APointI m_oldPoint;
	APointI m_newPoint;
};

#endif 

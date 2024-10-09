#ifndef _RESIZECOMMAND_H_
#define _RESIZECOMMAND_H_

#include "FWCommand.h"
#include <ARect.h>

class FWGlyph;

// FWCommandResize represents the action of 
// resizing glyph
class FWCommandResize : public FWCommand     
{
public:
	FWCommandResize(FWGlyph *pGlyph, ARectI newRect, ARectI oldRect);
	virtual ~FWCommandResize();

	virtual void Execute();
	virtual void Unexecute();
private:
	FWGlyph * m_pGlyph;
	ARectI m_oldSizeRect;
	ARectI m_newSizeRect;
};

#endif 

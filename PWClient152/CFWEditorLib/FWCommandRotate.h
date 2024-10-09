#ifndef _ROTATECOMMAND_H_
#define _ROTATECOMMAND_H_

#include "FWCommand.h"

class FWGlyph;

// FWCommandRotate represents the action of 
// rotating glyph
class FWCommandRotate : public FWCommand     
{
public:
	FWCommandRotate(FWGlyph *pGlyph, double fNewAngle, double fOldAngle);
	virtual ~FWCommandRotate();

	virtual void Execute();
	virtual void Unexecute();

private:
	double m_fNewAngle;
	double m_fOldAngle;
	FWGlyph *m_pGlyph;
};

#endif 

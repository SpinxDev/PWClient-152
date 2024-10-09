#ifndef _FWCOMMANDMOVE_H_
#define _FWCOMMANDMOVE_H_

#include "FWCommand.h"
#include <AArray.h>
#include <APoint.h>

class FWDialogBoard;
class FWGlyph;

// FWCommandMove represents the action of 
// moving a glyph
class FWCommandMove : public FWCommand  
{
public:
	typedef AArray<FWGlyph *, FWGlyph *> AGlyphArray;
	typedef AArray<APointI, APointI&> APointArray;

	FWCommandMove(const AGlyphArray& glyphArray, const APointArray& oldCenters, const APointArray& newCenters);
	virtual ~FWCommandMove();

	virtual void Execute();
	virtual void Unexecute();

private:
	AGlyphArray m_glyphArray;
	APointArray m_oldCenters;
	APointArray m_newCenters;
};

#endif 

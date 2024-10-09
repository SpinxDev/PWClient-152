#ifndef _SETBEZIERMODECOMMAND_H_
#define _SETBEZIERMODECOMMAND_H_

#include "FWCommand.h"
#include <AArray.h>

class FWGlyphBezier;
class FWView;

typedef AArray<int, int &> AIntArray;

class FWCommandSetBezierMode : public FWCommand  
{
	int m_nHandle;
	FWGlyphBezier * m_pBezier;
	int m_nNewMode;
	AIntArray m_aryOldMode;
public:
	FWCommandSetBezierMode(FWGlyphBezier * pBezier, int nHandle, int nMode);
	virtual ~FWCommandSetBezierMode();

	virtual void Execute();
	virtual void Unexecute();
};

#endif 

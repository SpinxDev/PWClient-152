#ifndef _FWCOMMANDSETGLYPHPROP_H_
#define _FWCOMMANDSETGLYPHPROP_H_

#include "FWCommand.h"

class AMemFile;
class FWGlyph;
class FWDoc;
typedef unsigned char BYTE;

class FWCommandSetGlyphProp : public FWCommand  
{
	FWGlyph * m_pGlyph;
	BYTE * m_pDataOld;
	BYTE * m_pDataNew;
	int m_nSizeOld;
	int m_nSizeNew;
	FWDoc * m_pDoc;
public:
	FWCommandSetGlyphProp(FWDoc * pDoc, FWGlyph *pGlyph, BYTE *pDataOld, int nSizeOld, BYTE *pDataNew, int nSizeNew);
	virtual ~FWCommandSetGlyphProp();

	// base class interface
	virtual void Execute();
	virtual void Unexecute();
};

#endif // !defined(AFX_FWCOMMANDSETGLYPHPROP_H__48DEDD94_0398_4AEA_91A9_07F08782C21F__INCLUDED_)

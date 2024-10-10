#ifndef _FWGLYPHGRAPHBASE_H_
#define _FWGLYPHGRAPHBASE_H_

#include "FWGlyph.h"

class FWGlyphGraphBase : public FWGlyph  
{
	FW_DECLARE_DYNAMIC(FWGlyphGraphBase)
public:
	FWGlyphGraphBase();
	virtual ~FWGlyphGraphBase();
	// base class interface
	virtual void Serialize(FWArchive &ar);
protected:
	// base class interface
	virtual bool IsCombinable() { return true; }
	virtual bool PrepareFireworkData(float fScale);
	virtual const char * GetDlgToShow();
};

#endif 
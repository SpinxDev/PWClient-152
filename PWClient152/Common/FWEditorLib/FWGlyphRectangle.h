#ifndef _FWGLYPHRECTANGLE_H_
#define _FWGLYPHRECTANGLE_H_

#include "FWGlyphRectBase.h"
#include "FWGlyph.h"

class FWGlyphRectangle : public FWGlyphRectBase  
{
	FW_DECLARE_SERIAL(FWGlyphRectangle);
public:
	// base class interface
	virtual CCharOutline * CreateOutline() const;

	FWGlyphRectangle();
	virtual ~FWGlyphRectangle();
};

#endif 

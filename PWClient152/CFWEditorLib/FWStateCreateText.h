#ifndef _FWSTATECREATETEXT_H_
#define _FWSTATECREATETEXT_H_

#include "FWState.h"

class FWGlyphText;

class FWStateCreateText : public FWState  
{
	FW_DECLARE_DYNAMIC(FWStateCreateText)
private:
	FWGlyphText * m_pGlyph;
	bool m_bGlyphInUse;
public:
	FWStateCreateText(FWView *pView, FWGlyphText *pGlyph);
	virtual ~FWStateCreateText();

	virtual void OnLClick(UINT nFlags, APointI point);
};

#endif 
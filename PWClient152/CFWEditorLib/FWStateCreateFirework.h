#ifndef _FWSTATECREATEFIREWORK_H_
#define _FWSTATECREATEFIREWORK_H_

#include "FWState.h"

class FWGlyphFirework;

class FWStateCreateFirework : public FWState  
{
	FW_DECLARE_DYNAMIC(FWStateCreateFirework)
private:
	FWGlyphFirework * m_pGlyph;
	bool m_bGlyphInUse;
public:
	FWStateCreateFirework(FWView *pView, FWGlyphFirework *pGlyph);
	virtual ~FWStateCreateFirework();
	
	virtual void OnDraw();
	virtual void OnLClick(UINT nFlags, APointI point);
};

#endif 